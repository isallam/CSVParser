/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Token.h
 * Author: ibrahim
 *
 * Created on August 3, 2017, 10:24 AM
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <vector>
using namespace std;

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
    Token& append(char ch)
    {
      _content.push_back(ch);
      return *this;
    }
    Token& append(string& str)
    {
      for(char ch : str) {
        _content.push_back(ch);
      }
      return *this;
    }
    
    string content() 
    {
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



#endif /* TOKEN_H */

