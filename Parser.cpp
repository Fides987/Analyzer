#include "Parser.h"

Parser::Node::Node(const std::string& n)
    : name(n), children(nullptr), childCount(0), childCap(0) {
}

Parser::Node::~Node() {
    for (int i = 0; i < childCount; ++i)
        delete children[i];
    delete[] children;
}

void Parser::Node::addChild(Node* ch) {
    if (!ch) return;
    if (childCount == childCap) {
        int newCap = (childCap == 0 ? 4 : childCap * 2);
        Node** arr = new Node * [newCap];
        for (int i = 0; i < childCount; ++i) arr[i] = children[i];
        delete[] children;
        children = arr;
        childCap = newCap;
    }
    children[childCount++] = ch;
}


Parser::Parser(const Token* toks, int n, std::ostream& treeOut)
    : tokens(toks),
    count(n),
    pos(0),
    out(treeOut),
    errorFound(false),
    symbols(211),
    root(nullptr)
{
    if (count > 0) current = tokens[0];
    else current = Token(T_EOF, "", 0);
}

void Parser::nextToken() {
    if (pos < count - 1) {
        ++pos;
        current = tokens[pos];
    }
    else {
        current = Token(T_EOF, "", current.line);
    }
}

void Parser::error(const std::string& msg) {
    out << "Syntax error (line " << current.line << "): " << msg << ", got '" << current.lexeme << "'\n";
    errorFound = true;
    nextToken();
}

void Parser::semanticError(const std::string& msg) {
    out << "Semantic error (line " << current.line << "): " << msg << "\n";
    errorFound = true;
}

bool Parser::isAddOp(const Token& t) const {
    return (t.type == T_OPERATOR && (t.lexeme == "+" || t.lexeme == "-"));
}

bool Parser::isRelOp(const Token& t) const {
    if (t.type != T_OPERATOR) return false;
    return (t.lexeme == "==" || t.lexeme == "!=" ||
        t.lexeme == "<" || t.lexeme == ">" ||
        t.lexeme == "<=" || t.lexeme == ">=");
}

void Parser::printTree(Node* node, int indent) {
    if (!node) return;
    for (int i = 0; i < indent; ++i) out << "  ";
    out << node->name << "\n";
    for (int i = 0; i < node->childCount; ++i)
        printTree(node->children[i], indent + 1);
}

void Parser::parse() {
    root = parseFunction();

    if (current.type != T_EOF)
        error("extra tokens after end of function");

    out << "Parse tree:\n";
    printTree(root, 0);
}

Parser::Node* Parser::parseFunction() {
    Node* node = new Node("Function");

    node->addChild(parseBegin());
    node->addChild(parseDescriptions());
    node->addChild(parseOperators());

    Node* endNode = new Node("End");

    if (current.type == T_KEYWORD && current.lexeme == "return") {
        endNode->addChild(new Node("return"));
        nextToken();
    }
    else {
        error("expected 'return'");
    }

    if (current.type == T_ID) {
        std::string var = current.lexeme;
        if (!symbols.exists(var))
            semanticError("returning undeclared variable '" + var + "'");
        else if (!symbols.isInitialized(var))
            semanticError("returning uninitialized variable '" + var + "'");
        endNode->addChild(new Node("Id(" + var + ")"));
        nextToken();
    }
    else {
        error("expected identifier after 'return'");
    }

    if (current.type == T_DELIMITER && current.lexeme == ";") {
        endNode->addChild(new Node(";"));
        nextToken();
    }
    else {
        error("expected ';' after return Id");
    }

    if (current.type == T_DELIMITER && current.lexeme == "}") {
        endNode->addChild(new Node("}"));
        nextToken();
    }
    else {
        error("expected '}' at end of function");
    }

    node->addChild(endNode);
    return node;
}

Parser::Node* Parser::parseBegin() {
    Node* node = new Node("Begin");

    node->addChild(parseType());

    if (current.type == T_ID) {
        node->addChild(new Node("FunctionName(" + current.lexeme + ")"));
        nextToken();
    }
    else {
        error("expected function name (identifier)");
    }

    if (current.type == T_DELIMITER && current.lexeme == "(") {
        node->addChild(new Node("("));
        nextToken();
    }
    else {
        error("expected '(' after function name");
    }

    if (current.type == T_DELIMITER && current.lexeme == ")") {
        node->addChild(new Node(")"));
        nextToken();
    }
    else {
        error("expected ')' after '('");
    }

    if (current.type == T_DELIMITER && current.lexeme == "{") {
        node->addChild(new Node("{"));
        nextToken();
    }
    else {
        error("expected '{' after function header");
    }

    return node;
}

Parser::Node* Parser::parseType() {
    Node* node = new Node("Type");
    if (current.type == T_KEYWORD && current.lexeme == "int") {
        node->addChild(new Node("int"));
        nextToken();
    }
    else {
        error("expected type 'int'");
    }
    return node;
}

Parser::Node* Parser::parseDescriptions() {
    Node* node = new Node("Descriptions");

    while (current.type == T_KEYWORD && current.lexeme == "int") {
        node->addChild(parseDescr());
    }

    return node;
}

Parser::Node* Parser::parseDescr() {
    Node* node = new Node("Descr");

    Node* typeNode = parseType();
    Node* varList = parseVarList();

    node->addChild(typeNode);
    node->addChild(varList);

    for (int i = 0; i < varList->childCount; ++i) {
        Node* v = varList->children[i];
        if (v->name.rfind("Id(", 0) == 0) {
            std::string var = v->name.substr(3, v->name.size() - 4);
            if (!symbols.declare(var, "int"))
                semanticError("variable '" + var + "' redeclared");
        }
    }

    if (current.type == T_DELIMITER && current.lexeme == ";") {
        nextToken();
    }
    else {
        error("expected ';' after variable description");
    }

    return node;
}

Parser::Node* Parser::parseVarList() {
    Node* node = new Node("VarList");

    if (current.type == T_ID) {
        node->addChild(new Node("Id(" + current.lexeme + ")"));
        nextToken();
    }
    else {
        error("expected identifier in VarList");
        return node;
    }

    while (current.type == T_DELIMITER && current.lexeme == ",") {
        node->addChild(new Node(","));
        nextToken();
        if (current.type == T_ID) {
            node->addChild(new Node("Id(" + current.lexeme + ")"));
            nextToken();
        }
        else {
            error("expected identifier after ',' in VarList");
            break;
        }
    }

    return node;
}

Parser::Node* Parser::parseOperators() {
    Node* node = new Node("Operators");

    while (current.type == T_ID ||
        (current.type == T_KEYWORD && current.lexeme == "while")) {
        node->addChild(parseOp());
    }
    return node;
}

Parser::Node* Parser::parseOp() {
    Node* node = new Node("Op");

    if (current.type == T_ID) {
        node->addChild(parseAssign());
    }
    else if (current.type == T_KEYWORD && current.lexeme == "while") {
        node->addChild(parseWhile());
    }
    else {
        error("expected assignment or while-operator");
    }

    return node;
}

Parser::Node* Parser::parseAssign() {
    Node* node = new Node("Assign");

    if (current.type != T_ID) {
        error("expected identifier at left side of assignment");
        return node;
    }

    std::string left = current.lexeme;
    if (!symbols.exists(left))
        semanticError("assignment to undeclared variable '" + left + "'");

    node->addChild(new Node("Id(" + left + ")"));
    nextToken();

    if (current.type == T_OPERATOR && current.lexeme == "=") {
        node->addChild(new Node("="));
        nextToken();
    }
    else {
        error("expected '=' in assignment");
    }

    Node* exprNode = parseExpr();
    node->addChild(exprNode);
    symbols.setInitialized(left);

    if (current.type == T_DELIMITER && current.lexeme == ";") {
        node->addChild(new Node(";"));
        nextToken();
    }
    else {
        error("expected ';' after assignment");
    }

    return node;
}

Parser::Node* Parser::parseWhile() {
    Node* node = new Node("While");
    node->addChild(new Node("while"));
    nextToken();

    if (current.type == T_DELIMITER && current.lexeme == "(") {
        node->addChild(new Node("("));
        nextToken();
    }
    else {
        error("expected '(' after 'while'");
    }

    node->addChild(parseCondition());

    if (current.type == T_DELIMITER && current.lexeme == ")") {
        node->addChild(new Node(")"));
        nextToken();
    }
    else {
        error("expected ')' after condition");
    }

    if (current.type == T_DELIMITER && current.lexeme == "{") {
        node->addChild(new Node("{"));
        nextToken();
    }
    else {
        error("expected '{' after while(...)");
    }

    node->addChild(parseOperators());

    if (current.type == T_DELIMITER && current.lexeme == "}") {
        node->addChild(new Node("}"));
        nextToken();
    }
    else {
        error("expected '}' after while-body");
    }

    return node;
}

Parser::Node* Parser::parseExpr() {
    Node* left = parseSimpleExpr();

    while (isAddOp(current)) {
        std::string op = current.lexeme;
        nextToken();
        Node* opNode = new Node("ExprOp(" + op + ")");
        opNode->addChild(left);
        opNode->addChild(parseSimpleExpr());
        left = opNode;
    }

    return left;
}

Parser::Node* Parser::parseSimpleExpr() {
    Node* node = new Node("SimpleExpr");

    if (current.type == T_ID) {
        std::string var = current.lexeme;
        if (!symbols.exists(var))
            semanticError("use of undeclared variable '" + var + "'");
        else if (!symbols.isInitialized(var))
            semanticError("variable '" + var + "' used before initialization");

        node->addChild(new Node("Id(" + var + ")"));
        nextToken();

    }
    else if (current.type == T_CONST_INT) {
        node->addChild(new Node("Const(" + current.lexeme + ")"));
        nextToken();
    }
    else if (current.type == T_DELIMITER && current.lexeme == "(") {
        node->addChild(new Node("("));
        nextToken();
        Node* exprInside = parseExpr();
        node->addChild(exprInside);
        if (current.type == T_DELIMITER && current.lexeme == ")") {
            node->addChild(new Node(")"));
            nextToken();
        }
        else {
            error("expected ')' after '(' Expr");
        }
    }
    else {
        error("expected Id, Const or '(' Expr ')'");
    }

    return node;
}

Parser::Node* Parser::parseCondition() {
    Node* node = new Node("Condition");

    node->addChild(parseExpr());

    Node* rel = parseRelOp();
    node->addChild(rel);

    node->addChild(parseExpr());

    return node;
}

Parser::Node* Parser::parseRelOp() {
    Node* node = new Node("RelOp");
    if (isRelOp(current)) {
        node->addChild(new Node(current.lexeme));
        nextToken();
    }
    else {
        error("expected relation operator (== != < > <= >=)");
    }
    return node;
}
