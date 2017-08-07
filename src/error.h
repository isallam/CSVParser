/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   error.h
 * Author: ibrahim
 * 
 * This error mechanism wwas borrowed from csv.h (fast-cpp-csv-parser)
 *
 * Created on August 3, 2017, 1:14 PM
 */

#ifndef ERROR_H
#define ERROR_H

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <utility>
#include <cstdio>
#include <exception>
#include <memory>
#include <cassert>
#include <cerrno>
#include <istream>

namespace csv {
  namespace error {

    struct base : std::exception {
      virtual void format_error_message()const = 0;

      const char*what()const throw () {
        format_error_message();
        return error_message_buffer;
      }

      mutable char error_message_buffer[256];
    };

    const int max_file_name_length = 255;

    struct with_file_name {

      with_file_name() {
        std::memset(file_name, 0, max_file_name_length + 1);
      }

      void set_file_name(const char*file_name) {
        std::strncpy(this->file_name, file_name, max_file_name_length);
        this->file_name[max_file_name_length] = '\0';
      }

      char file_name[max_file_name_length + 1];
    };

    struct with_file_line {

      with_file_line() {
        file_line = -1;
      }

      void set_file_line(int file_line) {
        this->file_line = file_line;
      }

      int file_line;
    };

    struct with_message {

      with_message() {
        message = " error in file %s";
      }

      void set_message(string message) {
        this->message = message;
      }
      string message;
    };

    struct with_errno {

      with_errno() {
        errno_value = 0;
      }

      void set_errno(int errno_value) {
        this->errno_value = errno_value;
      }

      int errno_value;
    };

    struct io_error_with_filename :
    base,
    with_file_name,
    with_message {

      void format_error_message()const {
        std::snprintf(error_message_buffer, sizeof (error_message_buffer),
                message.c_str(), file_name);
      }
    };

    struct io_error_with_file_line :
    base,
    with_file_name,
    with_file_line,
    with_message {

      void format_error_message()const {
        std::snprintf(error_message_buffer, sizeof (error_message_buffer),
                message.c_str(), file_name, file_line);
      }
    };

    struct can_not_open_file :
    base,
    with_file_name,
    with_errno {

      void format_error_message()const {
        if (errno_value != 0)
          std::snprintf(error_message_buffer, sizeof (error_message_buffer),
                "Can not open file \"%s\" because \"%s\"."
                , file_name, std::strerror(errno_value));
        else
          std::snprintf(error_message_buffer, sizeof (error_message_buffer),
                "Can not open file \"%s\"."
                , file_name);
      }
    };

    struct line_length_limit_exceeded :
    base,
    with_file_name,
    with_file_line {

      void format_error_message()const {
        std::snprintf(error_message_buffer, sizeof (error_message_buffer),
                "Line number %d in file \"%s\" exceeds the maximum length of 2^24-1."
                , file_line, file_name);
      }
    };

    const int max_column_name_length = 63;

    struct with_column_name {

      with_column_name() {
        std::memset(column_name, 0, max_column_name_length + 1);
      }

      void set_column_name(const char*column_name) {
        std::strncpy(this->column_name, column_name, max_column_name_length);
        this->column_name[max_column_name_length] = '\0';
      }

      char column_name[max_column_name_length + 1];
    };

    struct duplicate_name_in_header :
    base,
    with_file_name,
    with_column_name {

      void format_error_message()const {
        std::snprintf(error_message_buffer, sizeof (error_message_buffer),
                "Duplicate column \"%s\" in header of file \"%s\"."
                , column_name, file_name);
      }
    };

  }
}

#endif /* ERROR_H */

