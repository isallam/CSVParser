/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Utils.h
 * Author: ibrahim
 *
 * Created on August 3, 2017, 5:21 PM
 */

#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <string>

using namespace std;

namespace utils {

  static string trimTrailingSpaces(string str) {
    //        int length = buffer.length();
    //        while (length > 0 && Character.isWhitespace(buffer.charAt(length - 1))) {
    //            length = length - 1;
    //        }
    //        if (length != buffer.length()) {
    //            buffer.setLength(length);
    //        }
    str.erase(std::remove_if(str.begin(),
            str.end(),
            [](char x) {
              return std::isspace(x);
            }),
    str.end());
    return str;
  }

}


#endif /* UTILS_H */

