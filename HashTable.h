#pragma once
#include <string>
#include <ostream>
#include "Token.h"

class HashTable {
    struct Entry {
        Token token;
        Entry* next;
        Entry(const Token& t) : token(t), next(nullptr) {}
    };

    Entry** table;
    int capacity;

    unsigned long hash(const std::string& s) const;

public:
    HashTable(int size = 211);
    ~HashTable();

    int insert(const Token& token);
    void print(std::ostream& os);
    static std::string tokenTypeToString(TokenType t);
};
