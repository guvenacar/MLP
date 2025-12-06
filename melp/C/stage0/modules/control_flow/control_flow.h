#ifndef CONTROL_FLOW_H
#define CONTROL_FLOW_H

// Control flow statement types
typedef enum {
    CTRL_IF,
    CTRL_IF_ELSE,
    CTRL_WHILE,
    CTRL_FOR,
    CTRL_IF_EXPR,      // TIER 1: let x = if cond then a else b
    CTRL_LOOP_LABELED  // TIER 1: 'outer: for ... break 'outer
} ControlFlowType;

// If statement structure
typedef struct IfStatement {
    ControlFlowType type;      // TIER 1: Type of if statement (CTRL_IF, CTRL_IF_ELSE, CTRL_IF_EXPR)
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
    char* label;  // TIER 1: 'outer, 'inner for labeled loops
} ForStatement;

// Free functions
void if_statement_free(IfStatement* stmt);
void while_statement_free(WhileStatement* stmt);
void for_statement_free(ForStatement* stmt);

#endif // CONTROL_FLOW_H
