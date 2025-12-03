#include "expression_codegen.h"
#include <stdio.h>

void expression_generate_code(FILE* output, Expression* expr) {
    if (!expr) return;
    
    fprintf(output, "    # Expression code generation\n");
    
    switch (expr->type) {
        case EXPR_NUMBER:
            fprintf(output, "    movq $%ld, %%rax\n", expr->data.number_value);
            break;
        case EXPR_STRING:
            fprintf(output, "    # String expression\n");
            break;
        case EXPR_VARIABLE:
            fprintf(output, "    # Variable: %s\n", expr->data.var_name);
            break;
        default:
            fprintf(output, "    # TODO: Other expression types\n");
            break;
    }
}
