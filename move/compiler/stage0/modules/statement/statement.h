#ifndef STATEMENT_H
#define STATEMENT_H

#include "../expression/expression.h"

// Statement types
typedef enum {
    STMT_EXPRESSION,
    STMT_VARIABLE_DECL,
    STMT_ASSIGNMENT,
    STMT_IF,
    STMT_WHILE,
    STMT_FOR,
    STMT_RETURN,
    STMT_PRINT,
    STMT_FUNCTION,
    STMT_BLOCK
} StatementType;

// Statement structure (simplified)
typedef struct Statement {
    StatementType type;
    void* data;  // Points to specific statement data (IfStatement, WhileStatement, etc.)
} Statement;

// Statement management
Statement* statement_create(StatementType type);
void statement_free(Statement* stmt);

#endif
