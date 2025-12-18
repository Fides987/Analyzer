#pragma once
#include <string>

class SymbolTable {
    struct Entry {
        std::string name;
        std::string type;
        bool initialized;
        Entry* next;

        Entry(const std::string& n, const std::string& t)
            : name(n), type(t), initialized(false), next(nullptr) {
        }
    };

    Entry** table;
    int capacity;

    unsigned long hash(const std::string& s) const;

public:
    SymbolTable(int size = 211);
    ~SymbolTable();

    bool declare(const std::string& name, const std::string& type);

    bool exists(const std::string& name) const;

    std::string getType(const std::string& name) const;

    bool isInitialized(const std::string& name) const;

    void setInitialized(const std::string& name);
};
