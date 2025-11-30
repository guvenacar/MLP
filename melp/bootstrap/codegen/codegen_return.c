/**
 * MLP Codegen - Return Statement Code Generation
 * 
 * Handles return statements including:
 * - Single value return
 * - Multiple value return (tuples)
 * - Struct return
 * - Empty return
 */

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
void codegen_generate_expression_value(Codegen* gen, Expression* expr);

void codegen_generate_return(Codegen* gen, Statement* stmt) {
    char buffer[256];
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Return statement");
    
    if (stmt->return_stmt.value_count == 0 || stmt->return_stmt.values[0] == NULL) {
        // Return void / no value
        codegen_emit(gen, "    xor rax, rax        ; Return 0 (void)");
    } else if (stmt->return_stmt.value_count == 1) {
        // Single return value
        codegen_generate_expression_value(gen, stmt->return_stmt.values[0]);
        // Result already in rax
    } else if (stmt->return_stmt.value_count == 2) {
        // Two return values (rax, rdx)
        codegen_generate_expression_value(gen, stmt->return_stmt.values[0]);
        codegen_emit(gen, "    push rax            ; Save first value");
        
        codegen_generate_expression_value(gen, stmt->return_stmt.values[1]);
        codegen_emit(gen, "    mov rdx, rax        ; Second value in rdx");
        codegen_emit(gen, "    pop rax             ; First value in rax");
    } else {
        // More than 2 return values - would need struct or array
        // For now, just return first two
        fprintf(stderr, "Codegen warning: More than 2 return values, using only first 2\n");
        
        codegen_generate_expression_value(gen, stmt->return_stmt.values[0]);
        codegen_emit(gen, "    push rax");
        
        codegen_generate_expression_value(gen, stmt->return_stmt.values[1]);
        codegen_emit(gen, "    mov rdx, rax");
        codegen_emit(gen, "    pop rax");
    }
    
    // Function epilogue
    codegen_emit(gen, "    mov rsp, rbp");
    codegen_emit(gen, "    pop rbp");
    codegen_emit(gen, "    ret");
}
