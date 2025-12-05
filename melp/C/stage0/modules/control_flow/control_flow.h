#ifndef CONTROL_FLOW_H
#define CONTROL_FLOW_H

// Control flow statement types
typedef enum {
    CTRL_IF,
    CTRL_IF_ELSE,
    CTRL_WHILE,
    CTRL_FOR
} ControlFlowType;

// If statement structure
typedef struct IfStatement {
    void* condition;           // Comparison or logical expression
    int has_else;
} IfStatement;

// While statement structure
typedef struct WhileStatement {
    void* condition;
} WhileStatement;

// For statement structure
typedef struct ForStatement {
    char* iterator;
    void* start;
    void* end;
} ForStatement;

// Free functions
void if_statement_free(IfStatement* stmt);
void while_statement_free(WhileStatement* stmt);
void for_statement_free(ForStatement* stmt);

#endif // CONTROL_FLOW_H
