/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CSVParser.h
 * Author: ibrahim
 *
 * Created on August 2, 2017, 12:57 PM
 */

#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <map>
#include <vector>
#include <stdexcept>
#include "Lexer.h"
#include "error.h"

namespace csv {

  class CSVParser {
  public:
    CSVParser() = delete;
    CSVParser(const CSVParser& orig) = delete;

    virtual ~CSVParser() {
      delete _lexer;
      delete _format;
    }

    CSVParser(const string& fileName, csv::CSVFormat* format)
    : CSVParser(fileName, format, 0, 1) {
    }

    CSVParser(const string& fileName, csv::CSVFormat* format,
            long characterOffset, long recordNumber) {
      _format = format;
      _fileName = fileName;
      _characterOffset = characterOffset;
      _recordNumber = recordNumber - 1;

      _lexer = new Lexer(format, fileName);
      _headerMap = initializeHeader();

    }

    vector<CSVRecord> getRecords() {
      vector<CSVRecord> records;
      CSVRecord rec;
      while (nextRecord(&rec)) {
        records.push_back(rec);
      }
      return records;
    }

    bool nextRecord(CSVRecord * const result) {
      bool retValue = false;
      _record.clear();
      string sb;
      long startCharPosition = _lexer->getCharacterPosition() + _characterOffset;
      do {
        _reusableToken.reset();
        _lexer->nextToken(_reusableToken);
        switch (_reusableToken.type()) {
          case Token::TokenType::TOKEN:
            addRecordValue(false);
            break;
          case Token::TokenType::EORECORD:
            addRecordValue(true);
            break;
          case Token::TokenType::END_OF_FILE:
            if (_reusableToken.isReady()) {
              addRecordValue(true);
            }
            break;
          case Token::TokenType::INVALID:
          {
            error::io_error_with_file_line err;
            err.set_file_name(_fileName.c_str());
            err.set_file_line(getCurrentLineNumber());
            err.set_message("file: %s, (line %d) invalid parse sequence");
            throw err;
          }
            break;
          case Token::TokenType::COMMENT: // Ignored currently
            if (!sb.empty()) { // first comment for this record
              sb.push_back(csv::constants::LF);
            }
            sb.append(_reusableToken.content());
            _reusableToken.type(Token::TokenType::TOKEN); // Read another token
            break;
          default:
          {
            //throw new IllegalStateException("Unexpected Token type: " + _reusableToken.type());
            error::io_error_with_file_line err;
            err.set_file_name(_fileName.c_str());
            err.set_file_line(_lexer->getCurrentLineNumber());
            err.set_message("file: %s, (line %d) Unexpected Token type");
            throw err;
          }
        }
      } while (_reusableToken.type() == Token::TokenType::TOKEN);

      if (!_record.empty()) {
        _recordNumber++;
        string comment = sb;
        if (result != nullptr) {
          result->set(_record, _headerMap, comment,
                  _recordNumber, startCharPosition);
        }
        retValue = true;
      }
      return retValue;
    }

    long getCurrentLineNumber() {
      return _lexer->getCurrentLineNumber();
    }

  private:
    // TBD... need to refactor this for clarity

    void addRecordValue(bool lastRecord) {
      string input = _reusableToken.content();
      string inputClean = _format->getTrim() ? csv::trimTrailingSpaces(input) : input;
      if (lastRecord && inputClean.empty() && _format->getTrailingDelimiter()) {
        return;
      }
      _record.push_back(inputClean);
    }

    /**
     * Initializes the name to index mapping if the format defines a header.
     *
     * @return null if the format has no header.
     */
    map<string, int> initializeHeader() {
      map<string, int> hdrMap;
      vector<string> formatHeader = _format->getHeader();
      if (formatHeader.empty()) {
        // TBD... ignore... header case is not supported.
        //            hdrMap = _format.getIgnoreHeaderCase() ?
        //                    new TreeMap<String, Integer>(String.CASE_INSENSITIVE_ORDER) :
        //                    new LinkedHashMap<String, Integer>();

        vector<string> headerRecord;
        if (_format->getFirstRecordAsHeader()) {
          // read the header from the first line of the file
          CSVRecord record;
          nextRecord(&record);
          headerRecord = record.values();
        } else {
          if (_format->getSkipHeaderRecord()) {
            nextRecord(nullptr);
          }
          headerRecord = formatHeader;
        }

        // build the name to index mappings
        if (!headerRecord.empty()) {
          for (unsigned i = 0; i < headerRecord.size(); i++) {
            string header = headerRecord[i];
            bool containsHeader = (hdrMap.find(header) != hdrMap.end());
            bool emptyHeader = csv::trimTrailingSpaces(header).empty();
            if (containsHeader && (!emptyHeader || !_format->getAllowMissingColumnNames())) {
              error::duplicate_name_in_header err;
              err.set_file_name(_fileName.c_str());
              err.set_column_name(header.c_str());
              throw err;
            }
            hdrMap[header] = i;
          }
        }
      }
      return hdrMap;
    }

  private:
    csv::CSVFormat* _format;
    Lexer* _lexer;
    long _characterOffset;
    int _recordNumber;
    string _fileName;
    vector<string> _record;
    Token _reusableToken;
    map<string, int> _headerMap;
  };

}

#endif /* CSVPARSER_H */

