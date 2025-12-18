#include "HashTable.h"

HashTable::HashTable(int size) : capacity(size) {
    table = new Entry * [capacity];
    for (int i = 0; i < capacity; ++i)
        table[i] = nullptr;
}

HashTable::~HashTable() {
    for (int i = 0; i < capacity; ++i) {
        Entry* cur = table[i];
        while (cur) {
            Entry* tmp = cur;
            cur = cur->next;
            delete tmp;
        }
    }
    delete[] table;
}

unsigned long HashTable::hash(const std::string& s) const {
    unsigned long h = 5381;
    for (size_t i = 0; i < s.size(); ++i)
        h = ((h << 5) + h) + (unsigned char)s[i];
    return h % capacity;
}

int HashTable::insert(const Token& token) {
    unsigned long h = hash(token.lexeme);
    Entry* cur = table[h];
    while (cur) {
        if (cur->token.lexeme == token.lexeme && cur->token.type == token.type)
            return (int)h;
        cur = cur->next;
    }
    Entry* e = new Entry(token);
    e->next = table[h];
    table[h] = e;
    return (int)h;
}

void HashTable::print(std::ostream& os) {
    for (int i = 0; i < capacity; ++i) {
        Entry* cur = table[i];
        while (cur) {
            os << tokenTypeToString(cur->token.type)
                << " | " << cur->token.lexeme
                << " | " << i << "\n";
            cur = cur->next;
        }
    }
}

std::string HashTable::tokenTypeToString(TokenType t) {
    switch (t) {
    case T_ID:        return "id";
    case T_KEYWORD:   return "keyword";
    case T_OPERATOR:  return "operator";
    case T_DELIMITER: return "delimiter";
    case T_CONST_INT: return "int_const";
    case T_UNKNOWN:   return "unknown";
    case T_EOF:       return "eof";
    default:          return "unknown";
    }
}
