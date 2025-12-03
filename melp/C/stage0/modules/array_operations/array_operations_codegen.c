#include <stdio.h>
#include "array_operations_codegen.h"
#include "array_operations.h"

// Generate code for array push
void array_push_generate(FILE* output, ArrayOperation* op) {
    fprintf(output, "    # Array push\n");
    
    // Load array address
    fprintf(output, "    leaq array_var(%%rip), %%rdi\n");
    
    // Load value to push
    fprintf(output, "    movq $0, %%rsi  # value\n");
    
    // Call array_push function
    fprintf(output, "    call array_push\n");
}

// Generate code for array pop
void array_pop_generate(FILE* output, ArrayOperation* op) {
    fprintf(output, "    # Array pop\n");
    
    // Load array address
    fprintf(output, "    leaq array_var(%%rip), %%rdi\n");
    
    // Call array_pop function
    fprintf(output, "    call array_pop\n");
    fprintf(output, "    # Popped value in %%rax\n");
}

// Generate code for array slice
void array_slice_generate(FILE* output, ArrayOperation* op) {
    fprintf(output, "    # Array slice\n");
    
    // Load array address
    fprintf(output, "    leaq array_var(%%rip), %%rdi\n");
    
    // Load start index
    fprintf(output, "    movq $0, %%rsi  # start\n");
    
    // Load end index
    fprintf(output, "    movq $10, %%rdx  # end\n");
    
    // Call array_slice function
    fprintf(output, "    call array_slice\n");
    fprintf(output, "    # New array in %%rax\n");
}

// Generate code for array length
void array_length_generate(FILE* output, ArrayOperation* op) {
    fprintf(output, "    # Array length\n");
    
    // Load array address
    fprintf(output, "    leaq array_var(%%rip), %%rdi\n");
    
    // Array length is stored at offset 0 (first field)
    fprintf(output, "    movq (%%rdi), %%rax\n");
    fprintf(output, "    # Length in %%rax\n");
}

// Main code generation dispatcher
void array_op_generate_code(FILE* output, ArrayOperation* op) {
    if (!op) return;
    
    switch (op->op_type) {
        case ARRAY_OP_PUSH:
            array_push_generate(output, op);
            break;
        case ARRAY_OP_POP:
            array_pop_generate(output, op);
            break;
        case ARRAY_OP_SLICE:
            array_slice_generate(output, op);
            break;
        case ARRAY_OP_LENGTH:
            array_length_generate(output, op);
            break;
        case ARRAY_OP_INDEXOF:
            fprintf(output, "    # Array indexOf - TODO\n");
            break;
        case ARRAY_OP_CONCAT:
            fprintf(output, "    # Array concat - TODO\n");
            break;
        case ARRAY_OP_REVERSE:
            fprintf(output, "    # Array reverse - TODO\n");
            break;
        case ARRAY_OP_SORT:
            fprintf(output, "    # Array sort - TODO\n");
            break;
    }
}
