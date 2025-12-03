#include <stdio.h>
#include "string_operations_codegen.h"
#include "string_operations.h"

// Generate code for string concatenation
void string_concat_generate(FILE* output, StringOperation* op) {
    fprintf(output, "    # String concatenation\n");
    
    // Evaluate first string (result in %rdi)
    fprintf(output, "    # First string operand\n");
    fprintf(output, "    leaq string1(%%rip), %%rdi\n");
    
    // Evaluate second string (result in %rsi)
    fprintf(output, "    # Second string operand\n");
    fprintf(output, "    leaq string2(%%rip), %%rsi\n");
    
    // Call string concatenation function
    fprintf(output, "    call string_concat\n");
    fprintf(output, "    # Result in %%rax\n");
}

// Generate code for string length
void string_length_generate(FILE* output, StringOperation* op) {
    fprintf(output, "    # String length\n");
    
    // Load string address
    fprintf(output, "    leaq string_var(%%rip), %%rdi\n");
    
    // Call strlen
    fprintf(output, "    call strlen\n");
    fprintf(output, "    # Length in %%rax\n");
}

// Generate code for substring
void string_substring_generate(FILE* output, StringOperation* op) {
    fprintf(output, "    # String substring\n");
    
    // Load string address
    fprintf(output, "    leaq string_var(%%rip), %%rdi\n");
    
    // Load start index
    fprintf(output, "    movq $0, %%rsi  # start index\n");
    
    // Load end index
    fprintf(output, "    movq $10, %%rdx  # end index\n");
    
    // Call substring function
    fprintf(output, "    call string_substring\n");
    fprintf(output, "    # Result in %%rax\n");
}

// Generate code for string comparison
void string_compare_generate(FILE* output, StringOperation* op) {
    fprintf(output, "    # String comparison\n");
    
    // Load first string
    fprintf(output, "    leaq string1(%%rip), %%rdi\n");
    
    // Load second string
    fprintf(output, "    leaq string2(%%rip), %%rsi\n");
    
    // Call strcmp
    fprintf(output, "    call strcmp\n");
    
    // Convert result to boolean (0 = equal, non-zero = not equal)
    fprintf(output, "    testq %%rax, %%rax\n");
    fprintf(output, "    setz %%al\n");
    fprintf(output, "    movzbq %%al, %%rax\n");
}

// Main code generation dispatcher
void string_op_generate_code(FILE* output, StringOperation* op) {
    if (!op) return;
    
    switch (op->op_type) {
        case STRING_OP_CONCAT:
            string_concat_generate(output, op);
            break;
        case STRING_OP_LENGTH:
            string_length_generate(output, op);
            break;
        case STRING_OP_SUBSTRING:
            string_substring_generate(output, op);
            break;
        case STRING_OP_COMPARE:
            string_compare_generate(output, op);
            break;
        case STRING_OP_INDEXOF:
            fprintf(output, "    # String indexOf - TODO\n");
            break;
        case STRING_OP_CHARAT:
            fprintf(output, "    # String charAt - TODO\n");
            break;
        case STRING_OP_UPPER:
            fprintf(output, "    # String toUpper - TODO\n");
            break;
        case STRING_OP_LOWER:
            fprintf(output, "    # String toLower - TODO\n");
            break;
    }
}
