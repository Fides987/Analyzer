#pragma once
#include <string>
#include <ostream>
#include "Token.h"
#include "SymbolTable.h"

class Parser {
public:
    struct Node {
        std::string name;
        Node** children;
        int childCount;
        int childCap;

        Node(const std::string& n);
        ~Node();
        void addChild(Node* ch);
    };

private:
    const Token* tokens;
    int count;
    int pos;
    Token current;

    std::ostream& out;
    bool errorFound;
    SymbolTable symbols;

    Node* root;

    void nextToken();
    void error(const std::string& msg);
    void semanticError(const std::string& msg);

    Node* parseFunction();
    Node* parseBegin();
    Node* parseType();
    Node* parseDescriptions();
    Node* parseDescr();
    Node* parseVarList();
    Node* parseOperators();
    Node* parseOp();
    Node* parseAssign();
    Node* parseWhile();
    Node* parseExpr();
    Node* parseSimpleExpr();
    Node* parseCondition();
    Node* parseRelOp();

    bool isAddOp(const Token& t) const;
    bool isRelOp(const Token& t) const;

public:
    Parser(const Token* toks, int n, std::ostream& treeOut);

    void parse();
    Node* getRoot() const { return root; }
    bool hasErrors() const { return errorFound; }

    void printTree(Node* node, int indent);
};
