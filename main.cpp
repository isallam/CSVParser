/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ibrahim
 *
 * Simple test for CSVParser
 * 
 * Created on August 2, 2017
 */

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include "CSVFormat.h"
#include "CSVRecord.h"
#include "CSVParser.h"

using namespace std;

void test_csv_without_header(string& csvFile);
void test_csv_with_header(string& csvFile);

/*
 * 
 */
int main(int argc, char** argv) {

  string csvFileWithoutHeader = "./csvfile-no-header.csv";
  string csvFileWithHeader = "./csvfile-with-header.csv";
  
  test_csv_without_header(csvFileWithoutHeader);
  test_csv_with_header(csvFileWithHeader);
  
  return 0;
}

/**
 */
void test_csv_without_header(string& csvFile)
{
  try {
    cout << "processing file: '" << csvFile << "'" << endl;
    csv::CSVFormat* csvFormat = csv::CSVFormat::create(csv::FormatType::RFC4180);
    csv::CSVParser csvParser(csvFile, csvFormat);
    cout << "Getting records from file..." << endl;
    vector<CSVRecord> records = csvParser.getRecords();
    cout << "iterating over records..." << endl;
    for (CSVRecord record : records) {
        string& column1 = record.get(0);
        string& column2 = record.get(1);
        string& column3 = record.get(2);
        cout << "[1]: " << column1 
                << ", [2]: " << column2 
                << ", [3]: " << column3 
                << endl;
    }
  } catch (...) {
    cout << "processing file failed." << endl;
  }
}

/**
 */
void test_csv_with_header(string& csvFile)
{
  std::cout << "processing file: '" << csvFile << "'" << std::endl;
  string index("index");
  string first_name("first_name");
  string last_name("last_name");
  
  csv::CSVFormat* csvFormat = csv::CSVFormat::create(csv::FormatType::RFC4180);
  csvFormat->withFirstRecordAsHeader();
  
  csv::CSVParser csvParser(csvFile, csvFormat);
  
  vector<CSVRecord> records = csvParser.getRecords();
  for (CSVRecord& record : records) {
      string& id = record.get(index);
      string& firstName = record.get(first_name);
      string& lastName = record.get(last_name);
      cout << index << ": " << id << ", "
              << first_name << ": " << firstName << ", "
              << last_name << ": " << lastName << ", "
              << endl;
  }
}
