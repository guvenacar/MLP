// ============================================================================
// MELP Modular Codegen - codegen_compare.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_generate_comparison
// Karşılaştırma ifadelerini işler (==, !=, <, >, <=, >=, &&, ||)

void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label) {
    char buffer[256];
    
    // Check if condition is a comparison expression
    if (condition->type != EXPR_COMPARISON) {
        // Not a comparison - evaluate as boolean (0 = false, non-zero = true)
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Evaluate boolean condition");
        codegen_generate_expression_value(gen, condition);
        codegen_emit(gen, "    cmp rax, 0");
        snprintf(buffer, sizeof(buffer), "    je .L%d          ; Jump if false", false_label);
        codegen_emit(gen, buffer);
        return;
    }
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Evaluate comparison");
    
    // Check if this is a string comparison
    int is_string_comp = is_string_expression(gen, condition->comparison.left) || 
                         is_string_expression(gen, condition->comparison.right);
    
    // TTO: Check if this is a double comparison
    int is_double_comp = is_double_expression(gen, condition->comparison.left) || 
                         is_double_expression(gen, condition->comparison.right);
    
    if (is_string_comp) {
        // String comparison - use runtime functions
        
        // Load left operand (string pointer) into rdi
        codegen_generate_expression_value(gen, condition->comparison.left);
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    push rdi");
        
        // Load right operand (string pointer) into rsi
        codegen_generate_expression_value(gen, condition->comparison.right);
        codegen_emit(gen, "    mov rsi, rax");
        codegen_emit(gen, "    pop rdi");
        
        // Call appropriate string comparison function
        if (condition->comparison.op == CMP_EQUAL) {
            codegen_emit(gen, "    call string_equal");
        } else if (condition->comparison.op == CMP_NOT_EQUAL) {
            codegen_emit(gen, "    call string_not_equal");
        } else {
            fprintf(stderr, "Error: Unsupported string comparison operator\n");
            exit(1);
        }
        
        // Result is in rax (1 = true, 0 = false)
        // Jump to false_label if result is 0
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d", false_label);
        codegen_emit(gen, buffer);
    } else if (is_double_comp) {
        // TTO: Double comparison - use SSE comisd instruction
        codegen_emit(gen, "    ; TTO: Double comparison using comisd");
        
        // Load left side - may need to convert int to double
        int left_is_double = is_double_expression(gen, condition->comparison.left);
        codegen_generate_expression_value(gen, condition->comparison.left);
        
        if (left_is_double) {
            // Value is already in xmm0 (via movq from rax)
            codegen_emit(gen, "    movq xmm0, rax");
        } else {
            // Integer - convert to double
            codegen_emit(gen, "    cvtsi2sd xmm0, rax");
        }
        
        // Save xmm0 on stack
        codegen_emit(gen, "    sub rsp, 8");
        codegen_emit(gen, "    movsd [rsp], xmm0");
        
        // Load right side
        int right_is_double = is_double_expression(gen, condition->comparison.right);
        codegen_generate_expression_value(gen, condition->comparison.right);
        
        if (right_is_double) {
            codegen_emit(gen, "    movq xmm1, rax");
        } else {
            codegen_emit(gen, "    cvtsi2sd xmm1, rax");
        }
        
        // Restore left operand to xmm0
        codegen_emit(gen, "    movsd xmm0, [rsp]");
        codegen_emit(gen, "    add rsp, 8");
        
        // Compare: comisd sets flags based on xmm0 vs xmm1
        // CF=1 if xmm0 < xmm1 or unordered
        // ZF=1 if xmm0 == xmm1
        // CF=0 and ZF=0 if xmm0 > xmm1
        codegen_emit(gen, "    comisd xmm0, xmm1");
        
        // Jump to false label based on comparison
        // Note: For floating point, we use unsigned jumps (ja, jb, jae, jbe)
        const char* jump_instr;
        switch (condition->comparison.op) {
            case CMP_EQUAL:         jump_instr = "jne"; break;  // Also handles unordered (NaN)
            case CMP_NOT_EQUAL:     jump_instr = "je"; break;
            case CMP_LESS:          jump_instr = "jae"; break;  // jump if not below (CF=0)
            case CMP_LESS_EQUAL:    jump_instr = "ja"; break;   // jump if above (CF=0 and ZF=0)
            case CMP_GREATER:       jump_instr = "jbe"; break;  // jump if below or equal
            case CMP_GREATER_EQUAL: jump_instr = "jb"; break;   // jump if below (CF=1)
            default: jump_instr = "jmp"; break;
        }
        
        snprintf(buffer, sizeof(buffer), "    %s .L%d", jump_instr, false_label);
        codegen_emit(gen, buffer);
    } else {
        // Numeric comparison - use cmp instruction
        
        // Load left side into rax
        codegen_generate_expression_value(gen, condition->comparison.left);
        codegen_emit(gen, "    push rax");
        
        // Load right side into rbx
        codegen_generate_expression_value(gen, condition->comparison.right);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        
        // Compare
        codegen_emit(gen, "    cmp rax, rbx");
        
        // Jump to false label based on comparison
        const char* jump_instr;
        switch (condition->comparison.op) {
            case CMP_EQUAL:         jump_instr = "jne"; break;
            case CMP_NOT_EQUAL:     jump_instr = "je"; break;
            case CMP_LESS:          jump_instr = "jge"; break;
            case CMP_LESS_EQUAL:    jump_instr = "jg"; break;
            case CMP_GREATER:       jump_instr = "jle"; break;
            case CMP_GREATER_EQUAL: jump_instr = "jl"; break;
            default: jump_instr = "jmp"; break;
        }
        
        snprintf(buffer, sizeof(buffer), "    %s .L%d", jump_instr, false_label);
        codegen_emit(gen, buffer);
    }
}

