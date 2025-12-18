#pragma once
#include <fstream>
#include <string>
#include "Token.h"

class Lexer {
    std::ifstream fin;
    int curChar;
    int line;

public:
    Lexer(const std::string& filename);
    ~Lexer();

    Token getNextToken();
    int getLine() const { return line; }

private:
    void nextChar();
    bool isKeyword(const std::string& s);
};
