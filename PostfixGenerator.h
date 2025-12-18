#pragma once
#include <ostream>
#include "Parser.h"

class PostfixGenerator {
    std::ostream& out;

    typedef Parser::Node Node;

public:
    PostfixGenerator(std::ostream& o) : out(o) {}

    void generate(Node* root);

private:
    void genFunction(Node* node);
    void genDescriptions(Node* node);
    void genOperators(Node* node);
    void genOp(Node* node);
    void genAssign(Node* node);
    void genWhile(Node* node);
    void genExpr(Node* node);
    void genSimpleExpr(Node* node);
    void genCondition(Node* node);
};
