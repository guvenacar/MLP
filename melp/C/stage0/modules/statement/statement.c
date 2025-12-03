#include "statement.h"
#include <stdlib.h>

Statement* statement_create(StatementType type) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = type;
    stmt->data = NULL;
    return stmt;
}

void statement_free(Statement* stmt) {
    if (!stmt) return;
    
    // Data freed by specific statement modules
    free(stmt);
}
