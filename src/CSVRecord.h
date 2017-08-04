/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   CSVRecord.h
 * Author: ibrahim
 *
 * Created on August 2, 2017, 12:57 PM
 */

#ifndef CSVRECORD_H
#define CSVRECORD_H
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <iostream>

using namespace std;

class CSVRecord {
public:

  CSVRecord() {
    _mapping = nullptr;
  }

  CSVRecord(const CSVRecord& orig) {
    this->_characterPosition = orig._characterPosition;
    this->_comment = orig._comment;
    this->_mapping = orig._mapping;
    this->_recordNumber = orig._recordNumber;
    this->_values = orig._values;
  }

  virtual ~CSVRecord() {
  }

  string& get(int index) {
    try {
      return _values.at(index);
    } catch (std::out_of_range& ex) {
      cerr << "Index passed is " << index << " but CSVRecord only has " <<
              _values.size() << " values!" << endl;
      throw ex;
    }
  }

  string& get(string& name) {
    if (_mapping == nullptr) {
      //cerr << "No header mapping was specified, the record values can't be accessed by name" << endl;
      throw std::invalid_argument("No header mapping was specified, the record values can't be accessed by name");
    }
    int index = -1;
    try {
      index = _mapping->at(name);
    } catch (std::out_of_range& ex) {
      cerr << "Mapping for '" << name << "' not found, expected one of...  (TBD: display possible keys)" << endl;
      throw ex;
    }
    return get(index);
  }

  void set(vector<string> values, map<string, int>& mapping,
          string& comment, long recordNumber, long characterPosition) {
    _recordNumber = recordNumber;
    _values = values;
    _mapping = &mapping;
    _comment = comment;
    _characterPosition = characterPosition;
  }

  vector<string> values() {
    return _values;
  }

private:

  vector<string>    _values;
  map<string, int>* _mapping;
  long              _characterPosition;
  long              _recordNumber;
  /** The accumulated comments (if any) */
  string            _comment;

};

#endif /* CSVRECORD_H */

