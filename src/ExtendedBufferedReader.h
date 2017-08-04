/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ExtendedBufferedReader.h
 * Author: ibrahim
 *
 * Created on August 3, 2017, 11:26 AM
 */

#ifndef EXTENDEDBUFFEREDREADER_H
#define EXTENDEDBUFFEREDREADER_H

#include <string>
#include <fstream> 
#include "CSVFormat.h"
#include "error.h"

using namespace std;

/**
 * A special buffered reader which supports sophisticated read access.
 * <p>
 * In particular the reader supports a look-ahead option, which allows you to see the next char returned by
 * {@link #read()}. This reader also tracks how many characters have been read with {@link #getPosition()}.
 * </p>
 *
 * @version $Id$
 */
class ExtendedBufferedReader {

private:
    /** The last char returned */
    int _lastChar = csv::constants::UNDEFINED;

    /** The count of EOLs (CR/LF/CRLF) seen so far */
    long _eolCounter;

    /** The position, which is number of characters read so far */
    long _position;

    bool _closed;
    
    ifstream _io;

public:
    /**
     * Created extended buffered reader using default buffer-size
     */
    ExtendedBufferedReader() {
    }
    
    void setFile(const string& fileName) {
      try {
        _io.open(fileName, std::ifstream::in );
        if( !_io.good())
        {
          error::can_not_open_file err;
          err.set_file_name(fileName.c_str());
          err.set_errno(-1);
          throw err;
        }
      }
      catch (std::ifstream::failure e) {
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

     int read( char* buf,  int offset,  int length) /*throws IOException*/ {
        if (length == 0) {
            return 0;
        }

        _io.seekg(offset);
        _io.read(buf, length);
        int len = _io.gcount();
        
        if (!_io.eof()) 
        { 

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
     * @return the next character
     *
     * @throws IOException
     *             if there is an error in reading
     */
    int lookAhead() /*throws IOException*/ {
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
     * @throws IOException
     *             If an I/O error occurs
     */
    
    void close() /*throws IOException*/ {
        // Set ivars before calling super close() in case close() throws an IOException.
        _closed = true;
        _lastChar = csv::constants::END_OF_STREAM;
        _io.close();
    }

};


#endif /* EXTENDEDBUFFEREDREADER_H */

