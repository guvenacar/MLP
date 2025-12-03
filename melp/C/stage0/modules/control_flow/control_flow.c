#include "control_flow.h"
#include <stdlib.h>

void if_statement_free(IfStatement* stmt) {
    if (!stmt) return;
    free(stmt);
}

void while_statement_free(WhileStatement* stmt) {
    if (!stmt) return;
    free(stmt);
}

void for_statement_free(ForStatement* stmt) {
    if (!stmt) return;
    if (stmt->iterator) free(stmt->iterator);
    free(stmt);
}
