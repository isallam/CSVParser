/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Lexer.h
 * Author: ibrahim
 *
 * Created on August 3, 2017, 11:18 AM
 */

#ifndef LEXER_H
#define LEXER_H

#include <algorithm>
#include <string>
#include <fstream>
#include "error.h"

namespace csv {

  /**
   * Helper function 
   */
  static string trimTrailingSpaces(string str) {
      str.erase(std::remove_if(str.begin(),
            str.end(),
            [](char x) {
              return std::isspace(x);
            }),
    str.end());
    return str;
  }

  /**
   * Internal token representation.
   * <p/>
   * It is used as contract between the lexer and the parser.
   *
   */
  class Token {
  public:
    /** length of the initial token (content-)buffer */
    //private static final int INITIAL_TOKEN_LENGTH = 50;

    enum class TokenType {
      /** Token has no valid content, i.e. is in its initialized state. */
      INVALID,

      /** Token with content, at beginning or in the middle of a line. */
      TOKEN,

      /** Token (which can have content) when the end of file is reached. */
      END_OF_FILE,

      /** Token with content when the end of a line is reached. */
      EORECORD,

      /** Token is a comment line. */
      COMMENT
    };

    Token() {
      _type = Token::TokenType::INVALID;
    }

    void reset() {
      _content.clear();
      _type = Token::TokenType::INVALID;
      _isReady = false;
    }

    TokenType type() {
      return _type;
    }

    void type(TokenType type) {
      _type = type;
    }

    bool isReady() {
      return _isReady;
    }

    void isReady(bool ready) {
      _isReady = ready;
    }

    Token& append(char ch) {
      _content.push_back(ch);
      return *this;
    }

    Token& append(string& str) {
      for (char ch : str) {
        _content.push_back(ch);
      }
      return *this;
    }

    string content() {
      return _content;
    }

  private:
    /** Token type */
    TokenType _type = TokenType::INVALID;

    /** The content buffer. */
    // TBD... review for performance impact.
    string _content;

    /** Token ready flag: indicates a valid token with content (ready for the parser). */
    bool _isReady;


    /**
     * Eases IDE debugging.
     *
     * @return a string helpful for debugging.
     */
    // TBD...
    //    String toString() {
    //        return _type.name() + " [" + content.toString() + "]";
    //    }
  };

  /**
   * Lexical analyzer.
   * based on apache.commons.csv project
   * 
   */


  class Lexer {

    struct ExtendedBufferedReader {
      /** The last char returned */
      int _lastChar = csv::constants::UNDEFINED;

      /** The count of EOLs (CR/LF/CRLF) seen so far */
      long _eolCounter;

      /** The position, which is number of characters read so far */
      long _position;

      bool _closed;

      ifstream _io;

      /**
       * Created extended buffered reader using default buffer-size
       */
      ExtendedBufferedReader() {
      }

      void setFile(const string& fileName) {
        try {
          _io.open(fileName, std::ifstream::in);
          if (!_io.good()) {
            error::can_not_open_file err;
            err.set_file_name(fileName.c_str());
            err.set_errno(-1);
            throw err;
          }
        } catch (std::ifstream::failure e) {
          std::cerr << "Exception opening file: " << fileName << " -error: " << e.what();
        }
      }

      int read() /*throws IOException*/ {
        int current = _io.get();
        if ((current == csv::constants::CR || current == csv::constants::LF)
                && _lastChar != csv::constants::CR) {
          _eolCounter++;
        }
        _lastChar = current;
        _position++;
        return _lastChar;
      }

      /**
       * Returns the last character that was read as an integer (0 to 65535). This will be the last character returned by
       * any of the read methods. This will not include a character read using the {@link #lookAhead()} method. If no
       * character has been read then this will return {@link Constants#UNDEFINED}. If the end of the stream was reached
       * on the last read then this will return {@link Constants#END_OF_STREAM}.
       *
       * @return the last character that was read
       */
      int getLastChar() {
        return _lastChar;
      }

      int read(char* buf, int offset, int length) /*throws IOException*/ {
        if (length == 0) {
          return 0;
        }

        _io.seekg(offset);
        _io.read(buf, length);
        int len = _io.gcount();

        if (!_io.eof()) {

          for (int i = offset; i < offset + len; i++) {
            char ch = buf[i];
            if (ch == csv::constants::LF) {
              if (csv::constants::CR != (i > 0 ? buf[i - 1] : _lastChar)) {
                _eolCounter++;
              }
            } else if (ch == csv::constants::CR) {
              _eolCounter++;
            }
          }

          _lastChar = buf[offset + len - 1];

        } else {
          _lastChar = csv::constants::END_OF_STREAM;
        }

        _position += len;
        return len;
      }

      /**
       * Calls {@link BufferedReader#readLine()} which drops the line terminator(s). This method should only be called
       * when processing a comment, otherwise information can be lost.
       * <p>
       * Increments {@link #eolCounter}
       * <p>
       * Sets {@link #lastChar} to {@link Constants#END_OF_STREAM} at EOF, otherwise to LF
       *
       * @return the line that was read, or null if reached EOF.
       */

      string readLine() /*throws IOException*/ {
        string line;

        std::getline(_io, line);

        if (!line.empty()) {
          _lastChar = csv::constants::LF; // needed for detecting start of line
          _eolCounter++;
        } else {
          _lastChar = csv::constants::END_OF_STREAM;
        }

        return line;
      }

      /**
       * Returns the next character in the current reader without consuming it. So the next call to {@link #read()} will
       * still return this value. Does not affect line number or last character.
       *
       * @return the next character or eof if no more chars available.
       *
       */
      int lookAhead() {
        return _io.peek();
      }

      /**
       * Returns the current line number
       *
       * @return the current line number
       */
      long getCurrentLineNumber() {
        // Check if we are at EOL or EOF or just starting
        if (_lastChar == csv::constants::CR ||
                _lastChar == csv::constants::LF ||
                _lastChar == csv::constants::UNDEFINED ||
                _lastChar == csv::constants::END_OF_STREAM) {
          return _eolCounter; // counter is accurate
        }
        return _eolCounter + 1; // Allow for counter being incremented only at EOL
      }

      /**
       * Gets the character position in the reader.
       *
       * @return the current position in the reader (counting characters, not bytes since this is a Reader)
       */
      long getPosition() {
        return _position;
      }

      bool isClosed() {
        return _closed;
      }

      /**
       * Closes the stream.
       *
       */

      void close() {
        // Set ivars before calling super close() in case close() throws an IOException.
        _closed = true;
        _lastChar = csv::constants::END_OF_STREAM;
        _io.close();
      }

    };


  public:

    Lexer(csv::CSVFormat* format, const string& fileName) {
      _delimiter = format->getDelimiter();
      _escape = mapNullToDisabled(format->getEscapeCharacter());
      _quoteChar = mapNullToDisabled(format->getQuoteCharacter());
      _commentStart = mapNullToDisabled(format->getCommentMarker());
      _ignoreSurroundingSpaces = format->getIgnoreSurroundingSpaces();
      _ignoreEmptyLines = format->getIgnoreEmptyLines();
      _fileName = const_cast<string&> (fileName);
      _reader.setFile(fileName);
    }

    /**
     * Returns the next token.
     * <p>
     * A token corresponds to a term, a record change or an end-of-file indicator.
     * </p>
     *
     * @param token
     *            an existing Token object to reuse. The caller is responsible to initialize the Token.
     * @return the next token found
     * @throws java.io.IOException
     *             on stream access error
     */
    Token nextToken(Token& token) /*throws IOException*/ {

      // get the last read char (required for empty line detection)
      int lastChar = _reader.getLastChar();

      // read the next char and set eol
      int c = _reader.read();
      /*
       * Note: The following call will swallow LF if c == CR. But we don't need to know if the last char was CR or LF
       * - they are equivalent here.
       */
      bool eol = readEndOfLine(c);

      // empty line detection: eol AND (last char was EOL or beginning)
      if (_ignoreEmptyLines) {
        while (eol && isStartOfLine(lastChar)) {
          // go on char ahead ...
          lastChar = c;
          c = _reader.read();
          eol = readEndOfLine(c);
          // reached end of file without any content (empty line at the end)
          if (isEndOfFile(c)) {
            token.type(Token::TokenType::END_OF_FILE);
            // don't set token.isReady here because no content
            return token;
          }
        }
      }

      // did we reach eof during the last iteration already ? EOF
      if ((isEndOfFile(lastChar) || !isDelimiter(lastChar)) && isEndOfFile(c)) {
        token.type(Token::TokenType::END_OF_FILE);
        // don't set token.isReady here because no content
        return token;
      }

      if (isStartOfLine(lastChar) && isCommentStart(c)) {
        string line = _reader.readLine();
        if (line.empty()) {
          token.type(Token::TokenType::END_OF_FILE);
          // don't set token.isReady here because no content
          return token;
        }
        string comment = line; //line.trim();
        token.append(comment);
        token.type(Token::TokenType::COMMENT);
        return token;
      }

      // important: make sure a new char gets consumed in each iteration
      while (token.type() == Token::TokenType::INVALID) {
        // ignore whitespaces at beginning of a token
        if (_ignoreSurroundingSpaces) {
          while (isWhitespace(c) && !eol) {
            c = _reader.read();
            eol = readEndOfLine(c);
          }
        }

        // ok, start of token reached: encapsulated, or token
        if (isDelimiter(c)) {
          // empty token return TOKEN("")
          token.type(Token::TokenType::TOKEN);
        } else if (eol) {
          // empty token return EORECORD("")
          // noop: token.content.append("");
          token.type(Token::TokenType::EORECORD);
        } else if (isQuoteChar(c)) {
          // consume encapsulated token
          parseEncapsulatedToken(token);
        } else if (isEndOfFile(c)) {
          // end of file return EOF()
          // noop: token.content.append("");
          token.type(Token::TokenType::END_OF_FILE);
          token.isReady(true); // there is data at EOF
        } else {
          // next token must be a simple token
          // add removed blanks when not ignoring whitespace chars...
          parseSimpleToken(token, c);
        }
      }
      return token;
    }

    /**
     * Closes resources.
     *
     * @throws IOException
     *             If an I/O error occurs
     */
    void close() /* throws IOException*/ {
      _reader.close();
    }

    /**
     * Returns the current line number
     *
     * @return the current line number
     */
    long getCurrentLineNumber() {
      return _reader.getCurrentLineNumber();
    }

    /**
     * Returns the current character position
     *
     * @return the current character position
     */
    long getCharacterPosition() {
      return _reader.getPosition();
    }

    // TODO escape handling needs more work

    /**
     * Handle an escape sequence.
     * The current character must be the escape character.
     * On return, the next character is available by calling {@link ExtendedBufferedReader#getLastChar()}
     * on the input stream.
     *
     * @return the unescaped character (as an int) or {@link Constants#END_OF_STREAM} if char following the escape is
     *      invalid.
     * @throws IOException if there is a problem reading the stream or the end of stream is detected:
     *      the escape character is not allowed at end of strem
     */
    int readEscape()/* throws IOException */ {
      // the escape char has just been read (normally a backslash)
      int ch = _reader.read();
      switch (ch) {
        case 'r':
          return csv::constants::CR;
        case 'n':
          return csv::constants::LF;
        case 't':
          return csv::constants::TAB;
        case 'b':
          return csv::constants::BACKSPACE;
        case 'f':
          return csv::constants::FF;
        case csv::constants::CR:
        case csv::constants::LF:
        case csv::constants::FF: // TODO is this correct?
        case csv::constants::TAB: // TODO is this correct? Do tabs need to be escaped?
        case csv::constants::BACKSPACE: // TODO is this correct?
          return ch;
        case csv::constants::END_OF_STREAM:
        {
          error::io_error_with_filename err;
          err.set_file_name(_fileName.c_str());
          err.set_message("EOF whilst processing escape sequence of %s");
          throw err;
        }
          break;
        default:
          // Now check for meta-characters
          if (isMetaChar(ch)) {
            return ch;
          }
          // indicate unexpected char - available from in.getLastChar()
          return csv::constants::END_OF_STREAM;
      }
    }

    /**
     * Greedily accepts \n, \r and \r\n This checker consumes silently the second control-character...
     *
     * @return true if the given or next character is a line-terminator
     */
    bool readEndOfLine(int ch) /*throws IOException */ {
      // check if we have \r\n...
      if (ch == csv::constants::CR && _reader.lookAhead() == csv::constants::LF) {
        // note: does not change ch outside of this method!
        ch = _reader.read();
      }
      return ch == csv::constants::LF || ch == csv::constants::CR;
    }

    bool isClosed() {
      return _reader.isClosed();
    }

    /**
     * @return true if the given char is a whitespace character
     */
    bool isWhitespace(int ch) {
      return !isDelimiter(ch) && std::isspace((char) ch);
    }

    /**
     * Checks if the current character represents the start of a line: a CR, LF or is at the start of the file.
     *
     * @param ch the character to check
     * @return true if the character is at the start of a line.
     */
    bool isStartOfLine(int ch) {
      return (ch == csv::constants::LF || ch == csv::constants::CR || ch == csv::constants::UNDEFINED);
    }

    /**
     * @return true if the given character indicates end of file
     */
    bool isEndOfFile(int ch) {
      return ch == csv::constants::END_OF_STREAM;
    }

    bool isDelimiter(int ch) {
      return ch == _delimiter;
    }

    bool isEscape(int ch) {
      return ch == _escape;
    }

    bool isQuoteChar(int ch) {
      return ch == _quoteChar;
    }

    bool isCommentStart(int ch) {
      return ch == _commentStart;
    }

  private:

    /**
     * Parses a simple token.
     * <p/>
     * Simple token are tokens which are not surrounded by encapsulators. A simple token might contain escaped
     * delimiters (as \, or \;). The token is finished when one of the following conditions become true:
     * <ul>
     * <li>end of line has been reached (EORECORD)</li>
     * <li>end of stream has been reached (EOF)</li>
     * <li>an unescaped delimiter has been reached (TOKEN)</li>
     * </ul>
     *
     * @param token
     *            the current token
     * @param ch
     *            the current character
     * @return the filled token
     * @throws IOException
     *             on stream access error
     */
    Token parseSimpleToken(Token& token, int ch) /*throws IOException */ {
      // Faster to use while(true)+break than while(token.type == INVALID)
      while (true) {
        if (readEndOfLine(ch)) {
          token.type(Token::TokenType::EORECORD);
          break;
        } else if (isEndOfFile(ch)) {
          token.type(Token::TokenType::END_OF_FILE);
          token.isReady(true); // There is data at EOF
          break;
        } else if (isDelimiter(ch)) {
          token.type(Token::TokenType::TOKEN);
          break;
        } else if (isEscape(ch)) {
          int unescaped = readEscape();
          if (unescaped == csv::constants::END_OF_STREAM) { // unexpected char after escape
            token.append((char) ch).append((char) _reader.getLastChar());
          } else {
            token.append((char) unescaped);
          }
          ch = _reader.read(); // continue
        } else {
          token.append((char) ch);
          ch = _reader.read(); // continue
        }
      }

      if (_ignoreSurroundingSpaces) {
        csv::trimTrailingSpaces(token.content());
      }

      return token;
    }

    /**
     * Parses an encapsulated token.
     * <p/>
     * Encapsulated tokens are surrounded by the given encapsulating-string. The encapsulator itself might be included
     * in the token using a doubling syntax (as "", '') or using escaping (as in \", \'). Whitespaces before and after
     * an encapsulated token are ignored. The token is finished when one of the following conditions become true:
     * <ul>
     * <li>an unescaped encapsulator has been reached, and is followed by optional whitespace then:</li>
     * <ul>
     * <li>delimiter (TOKEN)</li>
     * <li>end of line (EORECORD)</li>
     * </ul>
     * <li>end of stream has been reached (EOF)</li> </ul>
     *
     * @param token
     *            the current token
     * @return a valid token object
     * @throws IOException
     *             on invalid state: EOF before closing encapsulator or invalid character before delimiter or EOL
     */
    Token parseEncapsulatedToken(Token token) /*throws IOException*/ {
      // save current line number in case needed for IOE
      long startLineNumber = getCurrentLineNumber();
      int c;
      while (true) {
        c = _reader.read();

        if (isEscape(c)) {
          int unescaped = readEscape();
          if (unescaped == csv::constants::END_OF_STREAM) { // unexpected char after escape
            token.append((char) c).append((char) _reader.getLastChar());
          } else {
            token.append((char) unescaped);
          }
        } else if (isQuoteChar(c)) {
          if (isQuoteChar(_reader.lookAhead())) {
            // double or escaped encapsulator -> add single encapsulator to token
            c = _reader.read();
            token.append((char) c);
          } else {
            // token finish mark (encapsulator) reached: ignore whitespace till delimiter
            while (true) {
              c = _reader.read();
              if (isDelimiter(c)) {
                token.type(Token::TokenType::TOKEN);
                return token;
              } else if (isEndOfFile(c)) {
                token.type(Token::TokenType::END_OF_FILE);
                token.isReady(true); // There is data at EOF
                return token;
              } else if (readEndOfLine(c)) {
                token.type(Token::TokenType::EORECORD);
                return token;
              } else if (!isWhitespace(c)) {
                error::io_error_with_file_line err;
                err.set_file_name(_fileName.c_str());
                err.set_file_line(getCurrentLineNumber());
                err.set_message("file: %s, (line %d) invalid char between encapsulated token and delimiter");
                throw err;
              }
            }
          }
        } else if (isEndOfFile(c)) {
          // error condition (end of file before end of token)
          error::io_error_with_file_line err;
          err.set_file_name(_fileName.c_str());
          err.set_file_line(startLineNumber);
          err.set_message("file: %s, (startline %d) EOF reached before encapsulated token finished");
          throw err;
        } else {
          // consume character
          token.append(c);
        }
      }
    }

    char mapNullToDisabled(char c) {
      return c == '\0' ? _DISABLED : c;
    }

    bool isMetaChar(int ch) {
      return ch == _delimiter ||
              ch == _escape ||
              ch == _quoteChar ||
              ch == _commentStart;
    }


    /**
     * Constant char to use for disabling comments, escapes and encapsulation. The value -2 is used because it
     * won't be confused with an EOF signal (-1), and because the Unicode value {@code FFFE} would be encoded as two
     * chars (using surrogates) and thus there should never be a collision with a real text char.
     */
    char _DISABLED = -2;

    char _delimiter;
    char _escape;
    char _quoteChar;
    char _commentStart;

    bool _ignoreSurroundingSpaces;
    bool _ignoreEmptyLines;

    string _fileName;

    /** The input handler */
  private:
    ExtendedBufferedReader _reader;


  };


}
#endif /* LEXER_H */

