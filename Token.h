#pragma once
#include <string>

enum TokenType {
    T_ID,
    T_KEYWORD,
    T_OPERATOR,
    T_DELIMITER,
    T_CONST_INT,
    T_UNKNOWN,
    T_EOF
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;

    Token() : type(T_UNKNOWN), lexeme(""), line(0) {}
    Token(TokenType t, const std::string& l, int ln)
        : type(t), lexeme(l), line(ln) {
    }
};
