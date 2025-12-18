#include "PostfixGenerator.h"

typedef Parser::Node Node;

static bool startsWith(const std::string& s, const char* pref) {
    int i = 0;
    while (pref[i] && i < (int)s.size()) {
        if (s[i] != pref[i]) return false;
        ++i;
    }
    return pref[i] == '\0';
}

void PostfixGenerator::generate(Node* root) {
    if (!root) return;
    if (root->name != "Function") return;
    genFunction(root);
}

void PostfixGenerator::genFunction(Node* node) {
    if (node->childCount < 4) return;

    Node* begin = node->children[0];
    Node* descr = node->children[1];
    Node* ops = node->children[2];
    Node* end = node->children[3];

    if (!begin || begin->childCount < 2) return;

    Node* typeNode = begin->children[0];
    Node* fnameNode = begin->children[1];

    std::string type = "int";
    if (typeNode && typeNode->childCount > 0)
        type = typeNode->children[0]->name;

    std::string fname = fnameNode->name;
    if (startsWith(fname, "FunctionName(") && fname.size() > 14)
        fname = fname.substr(13, fname.size() - 14);

    out << type << " " << fname << " FUNC_BEGIN\n";

    genDescriptions(descr);
    genOperators(ops);

    if (end && end->childCount >= 2) {
        Node* retId = end->children[1];
        std::string name = retId->name;
        if (startsWith(name, "Id(") && name.size() > 4)
            name = name.substr(3, name.size() - 4);
        out << name << " RETURN\n";
    }
}

void PostfixGenerator::genDescriptions(Node* node) {
    if (!node) return;
    for (int i = 0; i < node->childCount; ++i) {
        Node* descr = node->children[i];
        if (!descr || descr->name != "Descr" || descr->childCount < 2) continue;

        Node* typeNode = descr->children[0];
        Node* varList = descr->children[1];

        std::string type = "int";
        if (typeNode && typeNode->childCount > 0)
            type = typeNode->children[0]->name;

        if (!varList) continue;
        for (int j = 0; j < varList->childCount; ++j) {
            Node* v = varList->children[j];
            if (!v) continue;
            if (startsWith(v->name, "Id(") && v->name.size() > 4) {
                std::string var = v->name.substr(3, v->name.size() - 4);
                out << type << " " << var << " DECL\n";
            }
        }
    }
}

void PostfixGenerator::genOperators(Node* node) {
    if (!node) return;
    for (int i = 0; i < node->childCount; ++i)
        genOp(node->children[i]);
}

void PostfixGenerator::genOp(Node* node) {
    if (!node || node->childCount == 0) return;
    Node* sub = node->children[0];
    if (!sub) return;

    if (sub->name == "Assign")
        genAssign(sub);
    else if (sub->name == "While")
        genWhile(sub);
}

void PostfixGenerator::genAssign(Node* node) {
    if (!node || node->childCount < 3) return;

    Node* idNode = node->children[0];
    Node* exprNode = node->children[2];

    std::string var = idNode->name;
    if (startsWith(var, "Id(") && var.size() > 4)
        var = var.substr(3, var.size() - 4);

    genExpr(exprNode);
    out << var << " =\n";
}

void PostfixGenerator::genWhile(Node* node) {
    if (!node || node->childCount < 6) return;

    Node* condNode = node->children[2];
    Node* opsNode = node->children[5];

    genCondition(condNode);
    out << "WHILE_BEGIN\n";
    genOperators(opsNode);
    out << "WHILE_END\n";
}

void PostfixGenerator::genExpr(Node* node) {
    if (!node) return;

    if (node->name == "SimpleExpr") {
        genSimpleExpr(node);
        return;
    }

    if (startsWith(node->name, "ExprOp(") && node->childCount >= 2) {
        Node* left = node->children[0];
        Node* right = node->children[1];

        genExpr(left);
        genExpr(right);

        std::string op = node->name.substr(7, node->name.size() - 8);
        out << op << " ";

        return;
    }

    for (int i = 0; i < node->childCount; ++i)
        genExpr(node->children[i]);
}


void PostfixGenerator::genSimpleExpr(Node* node) {
    if (!node || node->childCount == 0) return;

    Node* c = node->children[0];
    if (!c) return;

    if (startsWith(c->name, "Id(") && c->name.size() > 4) {
        std::string v = c->name.substr(3, c->name.size() - 4);
        out << v << " ";
    }
    else if (startsWith(c->name, "Const(") && c->name.size() > 7) {
        std::string v = c->name.substr(6, c->name.size() - 7);
        out << v << " ";
    }
    else if (c->name == "(" && node->childCount >= 2) {
        genExpr(node->children[1]);
    }
}


void PostfixGenerator::genCondition(Node* node) {
    if (!node || node->childCount < 3) return;

    Node* left = node->children[0];
    Node* rel = node->children[1];
    Node* right = node->children[2];

    genExpr(left);
    genExpr(right);

    if (rel && rel->childCount > 0) {
        std::string op = rel->children[0]->name;
        out << op << " ";
    }
}
