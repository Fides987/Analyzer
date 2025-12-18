#include "Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& filename) : fin(filename), line(1) {
    curChar = fin.get();
}

Lexer::~Lexer() {
    if (fin.is_open())
        fin.close();
}

void Lexer::nextChar() {
    curChar = fin.get();
    if (curChar == '\n')
        line++;
}

bool Lexer::isKeyword(const std::string& s) {
    static const char* keywords[] = { "int", "while", "return" };
    for (int i = 0; i < 3; ++i)
        if (s == keywords[i])
            return true;
    return false;
}

Token Lexer::getNextToken() {
    enum State {
        START,
        ID,
        INT_CONST
    } state = START;

    std::string lexeme;
    bool leadingZero = false;

    while (true) {
        switch (state) {
        case START:
            if (!fin.good() || curChar == EOF)
                return Token(T_EOF, "", line);

            if (std::isspace(curChar)) {
                nextChar();
                break;
            }

            if (std::isalpha(curChar)) {
                lexeme += (char)curChar;
                state = ID;
                nextChar();
                break;
            }

            if (std::isdigit(curChar)) {
                lexeme += (char)curChar;
                leadingZero = (lexeme == "0");
                state = INT_CONST;
                nextChar();
                break;
            }

            if (curChar == '+' || curChar == '-' ||
                curChar == '=' || curChar == '<' ||
                curChar == '>' || curChar == '!') {

                lexeme += (char)curChar;

                if ((curChar == '=' || curChar == '<' ||
                    curChar == '>' || curChar == '!') &&
                    fin.peek() == '=') {
                    nextChar();
                    lexeme += (char)curChar;
                }

                nextChar();
                return Token(T_OPERATOR, lexeme, line);
            }

            if (curChar == ';' || curChar == ',' ||
                curChar == '(' || curChar == ')' ||
                curChar == '{' || curChar == '}') {

                lexeme += (char)curChar;
                nextChar();
                return Token(T_DELIMITER, lexeme, line);
            }

            while (!std::isspace(curChar) &&
                curChar != ';' && curChar != ',' &&
                curChar != '(' && curChar != ')' &&
                curChar != '{' && curChar != '}' &&
                curChar != '+' && curChar != '-' &&
                curChar != '=' && curChar != '<' &&
                curChar != '>' && curChar != '!' &&
                curChar != EOF) {
                lexeme += (char)curChar;
                nextChar();
            }
            return Token(T_UNKNOWN, lexeme, line);

        case ID:
            if (std::isalpha(curChar)) {
                lexeme += (char)curChar;
                nextChar();
            }
            else if (std::isdigit(curChar)) {
                while (!std::isspace(curChar) &&
                    curChar != ';' && curChar != ',' &&
                    curChar != '(' && curChar != ')' &&
                    curChar != '{' && curChar != '}' &&
                    curChar != '+' && curChar != '-' &&
                    curChar != '=' && curChar != '<' &&
                    curChar != '>' && curChar != '!' &&
                    curChar != EOF) {
                    lexeme += (char)curChar;
                    nextChar();
                }
                return Token(T_UNKNOWN, lexeme, line);
            }
            else {
                if (isKeyword(lexeme))
                    return Token(T_KEYWORD, lexeme, line);
                return Token(T_ID, lexeme, line);
            }
            break;

        case INT_CONST:
            if (std::isdigit(curChar)) {
                if (leadingZero) {
                    while (std::isdigit(curChar)) {
                        lexeme += (char)curChar;
                        nextChar();
                    }
                    while (!std::isspace(curChar) &&
                        curChar != ';' && curChar != ',' &&
                        curChar != '(' && curChar != ')' &&
                        curChar != '{' && curChar != '}' &&
                        curChar != '+' && curChar != '-' &&
                        curChar != '=' && curChar != '<' &&
                        curChar != '>' && curChar != '!' &&
                        curChar != EOF) {
                        lexeme += (char)curChar;
                        nextChar();
                    }
                    return Token(T_UNKNOWN, lexeme, line);
                }
                lexeme += (char)curChar;
                nextChar();
            }
            else if (std::isalpha(curChar) || curChar == '.') {
                while (!std::isspace(curChar) &&
                    curChar != ';' && curChar != ',' &&
                    curChar != '(' && curChar != ')' &&
                    curChar != '{' && curChar != '}' &&
                    curChar != '+' && curChar != '-' &&
                    curChar != '=' && curChar != '<' &&
                    curChar != '>' && curChar != '!' &&
                    curChar != EOF) {
                    lexeme += (char)curChar;
                    nextChar();
                }
                return Token(T_UNKNOWN, lexeme, line);
            }
            else {
                return Token(T_CONST_INT, lexeme, line);
            }
            break;
        }
    }
}
