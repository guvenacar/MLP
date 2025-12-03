// MELP Bootstrap Compiler - Parser Header

#ifndef PARSER_H
#define PARSER_H

typedef enum {
    NODE_VAR_DECL,
    NODE_FUNCTION,
    NODE_PRINT,
    NODE_IDENTIFIER,
    NODE_LITERAL,
    NODE_EXPRESSION
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char value[256];
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *next;
} ASTNode;

ASTNode* parse(const char *source);
ASTNode* create_node(NodeType type);
void free_ast(ASTNode *node);

#endif
