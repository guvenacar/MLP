#include "string_ops_codegen.h"
#include <stdio.h>

// Generate string concatenation
// Calls C runtime: char* mlp_string_concat(const char* a, const char* b)
void codegen_string_concat(FILE* output, StringOpExpr* expr) {
    fprintf(output, "    ; String concatenation: a + b\n");
    
    // TODO: Evaluate left operand
    fprintf(output, "    ; TODO: Evaluate left string operand\n");
    fprintf(output, "    push rax  ; Save left string pointer\n");
    
    // TODO: Evaluate right operand
    fprintf(output, "    ; TODO: Evaluate right string operand\n");
    fprintf(output, "    mov rsi, rax  ; Right string in rsi\n");
    fprintf(output, "    pop rdi       ; Left string in rdi\n");
    
    // Call runtime function
    fprintf(output, "    extern mlp_string_concat\n");
    fprintf(output, "    call mlp_string_concat\n");
    fprintf(output, "    ; Result pointer in rax\n");
}

// Generate string length
// Calls C runtime: int mlp_string_length(const char* str)
void codegen_string_length(FILE* output, StringOpExpr* expr) {
    fprintf(output, "    ; String length: length(s)\n");
    
    // TODO: Evaluate string operand
    fprintf(output, "    ; TODO: Evaluate string operand\n");
    fprintf(output, "    mov rdi, rax  ; String pointer in rdi\n");
    
    // Call runtime function
    fprintf(output, "    extern mlp_string_length\n");
    fprintf(output, "    call mlp_string_length\n");
    fprintf(output, "    ; Result (int) in rax\n");
}

// Generate substring
// Calls C runtime: char* mlp_string_substr(const char* str, int start, int len)
void codegen_string_substr(FILE* output, StringOpExpr* expr) {
    fprintf(output, "    ; Substring: substr(s, start, len)\n");
    
    // TODO: Evaluate all three operands
    fprintf(output, "    ; TODO: Evaluate string operand (rdi)\n");
    fprintf(output, "    ; TODO: Evaluate start operand (rsi)\n");
    fprintf(output, "    ; TODO: Evaluate length operand (rdx)\n");
    
    // Call runtime function
    fprintf(output, "    extern mlp_string_substr\n");
    fprintf(output, "    call mlp_string_substr\n");
    fprintf(output, "    ; Result pointer in rax\n");
}

// Generate string split
// Calls C runtime: void* mlp_string_split(const char* str, const char* delim)
// Returns array/list of strings
void codegen_string_split(FILE* output, StringOpExpr* expr) {
    fprintf(output, "    ; String split: split(s, delim)\n");
    
    // TODO: Evaluate operands
    fprintf(output, "    ; TODO: Evaluate string operand (rdi)\n");
    fprintf(output, "    ; TODO: Evaluate delimiter operand (rsi)\n");
    
    // Call runtime function
    fprintf(output, "    extern mlp_string_split\n");
    fprintf(output, "    call mlp_string_split\n");
    fprintf(output, "    ; Result (array pointer) in rax\n");
}

// Generic dispatcher for all string operations
void codegen_string_operation(FILE* output, StringOpExpr* expr) {
    switch (expr->op) {
        case STR_OP_CONCAT:
            codegen_string_concat(output, expr);
            break;
        case STR_OP_LENGTH:
            codegen_string_length(output, expr);
            break;
        case STR_OP_SUBSTR:
            codegen_string_substr(output, expr);
            break;
        case STR_OP_SPLIT:
            codegen_string_split(output, expr);
            break;
        case STR_OP_REPLACE:
        case STR_OP_TRIM:
        case STR_OP_UPPER:
        case STR_OP_LOWER:
        case STR_OP_CONTAINS:
        case STR_OP_INDEXOF:
            fprintf(output, "    ; TODO: String operation not yet implemented\n");
            break;
    }
}
