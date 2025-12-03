#ifndef CONTROL_FLOW_H
#define CONTROL_FLOW_H

typedef enum {
    STMT_IF,
    STMT_WHILE,
    STMT_FOR
} StatementType;

// Forward declarations
typedef struct Expression Expression;
typedef struct Statement Statement;

// If-Then-Else statement
typedef struct {
    Expression* condition;      // Boolean expression
    Statement** then_body;      // Array of statements in 'then' block
    int then_count;             // Number of statements in 'then' block
    Statement** else_body;      // Array of statements in 'else' block (NULL if no else)
    int else_count;             // Number of statements in 'else' block
} IfStatement;

// While loop
typedef struct {
    Expression* condition;
    Statement** body;
    int body_count;
} WhileStatement;

// For loop
typedef struct {
    char* variable;             // Loop variable name
    Expression* start;          // Start value
    Expression* end;            // End value
    Statement** body;
    int body_count;
} ForStatement;

// Generic statement wrapper
struct Statement {
    StatementType type;
    union {
        IfStatement if_stmt;
        WhileStatement while_stmt;
        ForStatement for_stmt;
    } data;
};

#endif
