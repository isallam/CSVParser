/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CSVFormat.h
 * Author: ibrahim
 *
 * Created on August 2, 2017, 12:58 PM
 */

#ifndef CSVFORMAT_H
#define CSVFORMAT_H

#include <string>
#include <stdexcept>
#include <vector>
#include <set>
#include <memory>

using namespace std;

namespace csv {

  /**
   * Defines quote behavior when printing.
   *
   */
  enum class QuoteMode {
    /**
     * Quotes all fields.
     */
    ALL,

    /**
     * Quotes all non-null fields.
     */
    ALL_NON_NULL,

    /**
     * Quotes fields which contain special characters such as a delimiter, quotes character or any of the characters in
     * line separator.
     */
    MINIMAL,

    /**
     * Quotes all non-numeric fields.
     */
    NON_NUMERIC,

    /**
     * Never quotes fields. When the delimiter occurs in data, the printer prefixes it with the current escape
     * character. If the escape character is not set, format validation throws an exception.
     */
    NONE
  };

  namespace constants {
    static constexpr char BACKSLASH = '\\';

    static constexpr char BACKSPACE = '\b';

    static constexpr char COMMA = ',';

    /**
     * Starts a comment, the remainder of the line is the comment.
     */
    static constexpr char COMMENT = '#';

    static constexpr char CR = '\r';

    /** RFC 4180 defines line breaks as CRLF */
    static constexpr const char* CRLF = "\r\n";

    static constexpr char DOUBLE_QUOTE_CHAR = '"';

    static constexpr const char* EMPTY = "";

    /** The end of stream symbol */
    static constexpr int END_OF_STREAM = -1;

    static constexpr char FF = '\f';

    static constexpr char LF = '\n';

    /**
     * Unicode line separator.
     */
    static constexpr const char* LINE_SEPARATOR = "\u2028";

    /**
     * Unicode next line.
     */
    static constexpr const char* NEXT_LINE = "\u0085";

    /**
     * Unicode paragraph separator.
     */
    static constexpr const char* PARAGRAPH_SEPARATOR = "\u2029";

    static constexpr char PIPE = '|';

    /** ASCII record separator */
    static constexpr char RS = 30;

    static constexpr char SP = ' ';

    static constexpr char TAB = '\t';

    /** Undefined state for the lookahead char */
    static constexpr int UNDEFINED = -2;

    /** ASCII unit separator */
    static constexpr char US = 31;
  }

  /**
   * For now we'll only support RFC4180.
   * In the future we'll add more format that commons-csv is supporting
   */
  enum class FormatType {
    RFC4180,
    NONE
  };

  class CSVFormat {
  public:

    static csv::CSVFormat* create(csv::FormatType parserType) {
      CSVFormat* format = nullptr;

      switch (parserType) {
        case csv::FormatType::RFC4180:
          format = new CSVFormat();
          break;
        default:
          break;
      }

      return format;
    }

    virtual ~CSVFormat() {
    };

  public:

    CSVFormat() : CSVFormat(
    csv::constants::COMMA, /* delimiter */
    csv::constants::DOUBLE_QUOTE_CHAR, /* quoteChar */
    csv::QuoteMode::NONE, /* quoteMode */
    false, /* ignoreSurroundingSpaces */
    false, /* ignoreEmptyLines */
    false, /* firstRecordAsHeader */
    false, /* skipHeaderRecord */
    false, /* allowMissingColumnNames */
    false, /* ignoreHeaderCase */
    false, /* trim */
    false /* trailingDelimiter */
    ) {
      _recordSeparator = const_cast<char*> (csv::constants::CRLF);
    }

    CSVFormat(const CSVFormat& orig) = delete;

    void withFirstRecordAsHeader() {
      _firstRecordAsHeader = true;
      this->withSkipHeaderRecord();
    }

    void withSkipHeaderRecord() {
      _skipHeaderRecord = true;
    }

    /**
     * Specifies whether missing column names are allowed when parsing the header line.
     *
     * @return {@code true} if missing column names are allowed when parsing the header line, {@code false} to throw an
     *         {@link IllegalArgumentException}.
     */
    bool getAllowMissingColumnNames() const {
      return _allowMissingColumnNames;
    }

    /**
     * Returns the character marking the start of a line comment.
     *
     * @return the comment start marker, may be {@code null}
     */
    char getCommentMarker() const {
      return _commentMarker;
    }

    /**
     * Returns the character delimiting the values (typically ';', ',' or '\t').
     *
     * @return the delimiter character
     */
    char getDelimiter() const {
      return _delimiter;
    }

    /**
     * Returns the escape character.
     *
     * @return the escape character, may be {@code null}
     */
    char getEscapeCharacter() const {
      return _escapeCharacter;
    }

    /**
     * Returns a copy of the header array.
     *
     * @return a copy of the header array; {@code null} if disabled, the empty array if to be read from the file
     */
    vector<string> getHeader() const {
      return _header;
    }

    /**
     * Returns a copy of the header comment array.
     *
     * @return a copy of the header comment array; {@code null} if disabled.
     */
    vector<string> getHeaderComments() const {
      return _headerComments;
    }

    /**
     * Specifies whether empty lines between records are ignored when parsing input.
     *
     * @return {@code true} if empty lines between records are ignored, {@code false} if they are turned into empty
     *         records.
     */
    bool getIgnoreEmptyLines() const {
      return _ignoreEmptyLines;
    }

    /**
     * Specifies whether header names will be accessed ignoring case.
     *
     * @return {@code true} if header names cases are ignored, {@code false} if they are case sensitive.
     * @since 1.3
     */
    bool getIgnoreHeaderCase() const {
      return _ignoreHeaderCase;
    }

    /**
     * Specifies whether spaces around values are ignored when parsing input.
     *
     * @return {@code true} if spaces around values are ignored, {@code false} if they are treated as part of the value.
     */
    bool getIgnoreSurroundingSpaces() const {
      return _ignoreSurroundingSpaces;
    }

    /**
     * Gets the String to convert to and from {@code null}.
     * <ul>
     * <li><strong>Reading:</strong> Converts strings equal to the given {@code nullString} to {@code null} when reading
     * records.</li>
     * <li><strong>Writing:</strong> Writes {@code null} as the given {@code nullString} when writing records.</li>
     * </ul>
     *
     * @return the String to convert to and from {@code null}. No substitution occurs if {@code null}
     */
    string getNullString() const {
      return _nullString;
    }

    /**
     * Returns the character used to encapsulate values containing special characters.
     *
     * @return the quoteChar character, may be {@code null}
     */
    char getQuoteCharacter() const {
      return _quoteCharacter;
    }

    /**
     * Returns the quote policy output fields.
     *
     * @return the quote policy
     */
    csv::QuoteMode getQuoteMode() const {
      return _quoteMode;
    }

    /**
     * Returns the record separator delimiting output records.
     *
     * @return the record separator
     */
    char* getRecordSeparator() const {
      return _recordSeparator;
    }

    /**
     * Returns whether we use the first record as header
     *
     * @return whether to read first record as header
     */
    bool getFirstRecordAsHeader() const {
      return _firstRecordAsHeader;
    }

    /**
     * Returns whether to skip the header record.
     *
     * @return whether to skip the header record.
     */
    bool getSkipHeaderRecord() const {
      return _skipHeaderRecord;
    }

    /**
     * Returns whether to add a trailing delimiter.
     *
     * @return whether to add a trailing delimiter.
     * @since 1.3
     */
    bool getTrailingDelimiter() const {
      return _trailingDelimiter;
    }

    /**
     * Returns whether to trim leading and trailing blanks.
     *
     * @return whether to trim leading and trailing blanks.
     */
    bool getTrim() const {
      return _trim;
    }

    CSVFormat& withIgnoreEmptyLines(bool ignoreEmptyLines) {
      _ignoreEmptyLines = ignoreEmptyLines;
      return *this;
    }

  private:

    CSVFormat(char delimiter, char quoteChar,
            csv::QuoteMode quoteMode, bool ignoreSurroundingSpaces,
            bool ignoreEmptyLines, bool firstRecordAsHeader,
            bool skipHeaderRecord,
            bool allowMissingColumnNames, bool ignoreHeaderCase,
            bool trim, bool trailingDelimiter) {
      _delimiter = delimiter;
      _quoteCharacter = quoteChar;
      _quoteMode = quoteMode;
      _ignoreSurroundingSpaces = ignoreSurroundingSpaces;
      _allowMissingColumnNames = allowMissingColumnNames;
      _ignoreEmptyLines = ignoreEmptyLines;
      _firstRecordAsHeader = firstRecordAsHeader;
      _skipHeaderRecord = skipHeaderRecord;
      _ignoreHeaderCase = ignoreHeaderCase;
      _trailingDelimiter = trailingDelimiter;
      _trim = trim;
      validate();
    }

    CSVFormat(char delimiter, char quoteChar, csv::QuoteMode quoteMode,
            char commentStart, char escape, bool ignoreSurroundingSpaces,
            bool ignoreEmptyLines, char* recordSeparator, string& nullString,
            vector<string>& headerComments, vector<string>& header,
            bool firstRecordAsHeader, bool skipHeaderRecord,
            bool allowMissingColumnNames, bool ignoreHeaderCase, bool trim,
            bool trailingDelimiter)
    : CSVFormat(delimiter, quoteChar, quoteMode,
    ignoreSurroundingSpaces,
    ignoreEmptyLines,
    firstRecordAsHeader, skipHeaderRecord,
    allowMissingColumnNames, ignoreHeaderCase,
    trim, trailingDelimiter) {
      _commentMarker = commentStart;
      _escapeCharacter = escape;
      _recordSeparator = recordSeparator;
      _nullString = nullString;
      _headerComments = headerComments;
      _header = header;
      validate();
    }

    /**
     * Verifies the consistency of the parameters and throws an IllegalArgumentException if necessary.
     *
     * TBD...
     * 
     * @throws IllegalArgumentException
     */
    void validate() {
      /* 
        if (isLineBreak(_delimiter)) {
            throw std::invalid_argument("The delimiter cannot be a line break");
        }

        if (_delimiter == _quoteCharacter) {
            throw std::invalid_argument(
                    "The quoteChar character and the delimiter cannot be the same ('" + _quoteCharacter + "')");
        }

        if (_delimiter == _escapeCharacter) {
            throw std::invalid_argument(
                    "The escape character and the delimiter cannot be the same ('" + _escapeCharacter + "')");
        }

        if (_delimiter == _commentMarker) {
            throw std::invalid_argument(
                    "The comment start character and the delimiter cannot be the same ('" + _commentMarker + "')");
        }

        if (_quoteCharacter == _commentMarker) {
            throw std::invalid_argument(
                    "The comment start character and the quoteChar cannot be the same ('" + _commentMarker + "')");
        }

        if (_escapeCharacter == _commentMarker) {
            throw std::invalid_argument(
                    "The comment start and the escape character cannot be the same ('" + _commentMarker + "')");
        }

        if (_escapeCharacter == '\0' && _quoteMode == QuoteMode.NONE) {
            throw std::invalid_argument("No quotes mode set but no escape character is set");
        }

        // validate header
        if (!_header.empty()) {
            std::set<string> dupCheck;
            for (unsigned i = 0; i < _header.size(); i++) {
              string hdr = _header[i];
                if (dupCheck.find(hdr) != dupCheck.end()) {
                    throw std::invalid_argument(
                            "The header contains a duplicate entry: '" + hdr + " in provided header keys";
                }
                else {
                  dupCheck.insert(hdr);
                }
            }
        }
       */
    }

    bool _allowMissingColumnNames;
    char _commentMarker; // empty if commenting is disabled
    char _delimiter;
    char _escapeCharacter; // empty if escaping is disabled
    vector<string> _header; // array of header column names
    vector<string> _headerComments; // array of header comment lines
    bool _ignoreEmptyLines;
    bool _ignoreHeaderCase; // should ignore header names case
    bool _ignoreSurroundingSpaces; // Should leading/trailing spaces be ignored around values?
    string _nullString; // the string to be used for null values
    char _quoteCharacter; // empty if quoting is disabled
    csv::QuoteMode _quoteMode;
    char* _recordSeparator; // for outputs
    bool _firstRecordAsHeader;
    bool _skipHeaderRecord;
    bool _trailingDelimiter;
    bool _trim;

  };

} // namespace csv

#endif /* CSVFORMAT_H */

