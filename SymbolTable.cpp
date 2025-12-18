#include "SymbolTable.h"

SymbolTable::SymbolTable(int size) : capacity(size) {
    table = new Entry * [capacity];
    for (int i = 0; i < capacity; ++i)
        table[i] = nullptr;
}

SymbolTable::~SymbolTable() {
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

unsigned long SymbolTable::hash(const std::string& s) const {
    unsigned long h = 5381;
    for (size_t i = 0; i < s.size(); ++i)
        h = ((h << 5) + h) + (unsigned char)s[i];
    return h % capacity;
}

bool SymbolTable::declare(const std::string& name, const std::string& type) {
    unsigned long h = hash(name);
    Entry* cur = table[h];
    while (cur) {
        if (cur->name == name)   // переменная уже объявлена
            return false;
        cur = cur->next;
    }
    Entry* e = new Entry(name, type);
    e->next = table[h];
    table[h] = e;
    return true;
}

bool SymbolTable::exists(const std::string& name) const {
    unsigned long h = hash(name);
    Entry* cur = table[h];
    while (cur) {
        if (cur->name == name) // Проверка на существование, все также как и сверху
            return true;
        cur = cur->next;
    }
    return false;
}

std::string SymbolTable::getType(const std::string& name) const {
    unsigned long h = hash(name);
    Entry* cur = table[h];
    while (cur) {
        if (cur->name == name)
            return cur->type;
        cur = cur->next;
    }
    return "";
}

bool SymbolTable::isInitialized(const std::string& name) const {
    unsigned long h = hash(name);
    Entry* curr = table[h];
    while (curr) {
        if (curr->name == name)
            return curr->initialized;
        curr = curr->next;
    }
    return true;
}

void SymbolTable::setInitialized(const std::string& name) {
    unsigned long h = hash(name);
    Entry* curr = table[h];
    while (curr) {
        if (curr->name == name) {
            curr->initialized = true;
            return;
        }
        curr = curr->next;
    }
}

