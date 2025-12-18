#include <iostream>
#include <fstream>
#include "Lexer.h"
#include "HashTable.h"
#include "Parser.h"
#include "PostfixGenerator.h"

int main() {
    Lexer lexer("input.txt");
    HashTable hashTable(211);

    Token* tokens = nullptr;
    int capacity = 0;
    int count = 0;

    std::ofstream lexOut("lexical.txt");

    while (true) {
        Token t = lexer.getNextToken();
        if (t.type == T_EOF)
            break;

        hashTable.insert(t);

        if (count == capacity) {
            int newCap = (capacity == 0 ? 128 : capacity * 2);
            Token* arr = new Token[newCap];
            for (int i = 0; i < count; ++i)
                arr[i] = tokens[i];
            delete[] tokens;
            tokens = arr;
            capacity = newCap;
        }
        tokens[count++] = t;
    }

    hashTable.print(lexOut);
    lexOut.close();

    if (count == 0) {
        std::ofstream parseOut("parseTree.txt");
        parseOut << "Empty input.\n";
        parseOut.close();
        std::ofstream postOut("postfix.txt");
        postOut << "No postfix: empty input.\n";
        postOut.close();
        delete[] tokens;
        return 0;
    }

    std::ofstream parseOut("parseTree.txt");
    Parser parser(tokens, count, parseOut);
    parser.parse();
    Parser::Node* root = parser.getRoot();
    parseOut.close();

    std::ofstream postOut("postfix.txt");
    PostfixGenerator gen(postOut);
    gen.generate(root);
    postOut.close();

    delete[] tokens;
    return 0;
}
