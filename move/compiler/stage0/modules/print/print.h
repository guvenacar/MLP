#ifndef PRINT_H
#define PRINT_H

// Print module - Independent print statement handling

typedef struct Expression Expression;

typedef struct {
    Expression* value;  // Expression to print
} PrintStatement;

// Create and free print statements
PrintStatement* print_statement_create(Expression* value);
void print_statement_free(PrintStatement* stmt);

#endif
