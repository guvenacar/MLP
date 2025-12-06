// Print Module - Data Structures
#include "print.h"
#include <stdlib.h>

PrintStatement* print_statement_create(Expression* value) {
    PrintStatement* stmt = malloc(sizeof(PrintStatement));
    stmt->value = value;
    return stmt;
}

void print_statement_free(PrintStatement* stmt) {
    if (!stmt) return;
    // Note: value freed elsewhere in AST cleanup
    free(stmt);
}
