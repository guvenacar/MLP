/**
 * MLP Codegen - Comparison Code Generation
 * 
 * Handles comparison expressions for conditional jumps including:
 * - Integer comparisons
 * - Double/float comparisons (TTO)
 * - String comparisons
 * - Logical operators (&&, ||, !)
 */

#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
void codegen_generate_expression_value(Codegen* gen, Expression* expr);
int is_string_expression(Codegen* gen, Expression* expr);
int is_double_expression(Codegen* gen, Expression* expr);

void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label) {
    char buffer[256];
    
    // Check if condition is a comparison expression
    if (condition->type != EXPR_COMPARISON) {
        // Not a comparison - evaluate as boolean (0 = false, non-zero = true)
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Evaluate boolean condition");
        codegen_generate_expression_value(gen, condition);
        codegen_emit(gen, "    test rax, rax       ; Check if zero");
        snprintf(buffer, sizeof(buffer), "    jz .L%d             ; Jump if false (zero)", false_label);
        codegen_emit(gen, buffer);
        return;
    }
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Comparison");
    
    // Check if this is a string comparison
    int left_is_string = is_string_expression(gen, condition->comparison.left);
    int right_is_string = is_string_expression(gen, condition->comparison.right);
    
    if (left_is_string || right_is_string) {
        // String comparison using strcmp
        codegen_emit(gen, "    ; String comparison");
        
        codegen_generate_expression_value(gen, condition->comparison.left);
        codegen_emit(gen, "    push rax            ; Save left string");
        
        codegen_generate_expression_value(gen, condition->comparison.right);
        codegen_emit(gen, "    mov rsi, rax        ; Right string in rsi");
        codegen_emit(gen, "    pop rdi             ; Left string in rdi");
        
        codegen_emit(gen, "    call strcmp");
        // strcmp returns: <0 if s1<s2, 0 if equal, >0 if s1>s2
        
        codegen_emit(gen, "    cmp rax, 0");
        
        switch (condition->comparison.op) {
            case CMP_EQUAL:
                snprintf(buffer, sizeof(buffer), "    jne .L%d", false_label);
                break;
            case CMP_NOT_EQUAL:
                snprintf(buffer, sizeof(buffer), "    je .L%d", false_label);
                break;
            case CMP_LESS:
                snprintf(buffer, sizeof(buffer), "    jge .L%d", false_label);
                break;
            case CMP_LESS_EQUAL:
                snprintf(buffer, sizeof(buffer), "    jg .L%d", false_label);
                break;
            case CMP_GREATER:
                snprintf(buffer, sizeof(buffer), "    jle .L%d", false_label);
                break;
            case CMP_GREATER_EQUAL:
                snprintf(buffer, sizeof(buffer), "    jl .L%d", false_label);
                break;
        }
        codegen_emit(gen, buffer);
        return;
    }
    
    // Check for TTO double comparison
    int left_is_double = is_double_expression(gen, condition->comparison.left);
    int right_is_double = is_double_expression(gen, condition->comparison.right);
    int use_double = left_is_double || right_is_double;
    
    if (use_double) {
        // TTO: Double comparison using SSE
        codegen_emit(gen, "    ; TTO: Double comparison");
        
        // Evaluate left operand
        codegen_generate_expression_value(gen, condition->comparison.left);
        if (!left_is_double) {
            codegen_emit(gen, "    cvtsi2sd xmm0, rax   ; Convert int to double");
        } else {
            codegen_emit(gen, "    movq xmm0, rax        ; Load double bit pattern");
        }
        codegen_emit(gen, "    sub rsp, 8");
        codegen_emit(gen, "    movsd [rsp], xmm0      ; Save left operand");
        
        // Evaluate right operand
        codegen_generate_expression_value(gen, condition->comparison.right);
        if (!right_is_double) {
            codegen_emit(gen, "    cvtsi2sd xmm1, rax   ; Convert int to double");
        } else {
            codegen_emit(gen, "    movq xmm1, rax        ; Load double bit pattern");
        }
        codegen_emit(gen, "    movsd xmm0, [rsp]      ; Restore left operand");
        codegen_emit(gen, "    add rsp, 8");
        
        // Compare doubles
        codegen_emit(gen, "    ucomisd xmm0, xmm1     ; Compare doubles");
        
        // Generate conditional jump based on operator
        // Note: ucomisd sets flags: ZF=1 if equal, CF=1 if less than
        switch (condition->comparison.op) {
            case CMP_EQUAL:
                snprintf(buffer, sizeof(buffer), "    jne .L%d", false_label);
                codegen_emit(gen, buffer);
                snprintf(buffer, sizeof(buffer), "    jp .L%d             ; Also check for NaN", false_label);
                break;
            case CMP_NOT_EQUAL:
                codegen_emit(gen, "    jp .L_true_ne        ; NaN is not equal");
                snprintf(buffer, sizeof(buffer), "    je .L%d", false_label);
                codegen_emit(gen, buffer);
                codegen_emit(gen, ".L_true_ne:");
                return;
            case CMP_LESS:
                snprintf(buffer, sizeof(buffer), "    jae .L%d             ; Jump if not below", false_label);
                break;
            case CMP_LESS_EQUAL:
                snprintf(buffer, sizeof(buffer), "    ja .L%d              ; Jump if above", false_label);
                break;
            case CMP_GREATER:
                snprintf(buffer, sizeof(buffer), "    jbe .L%d             ; Jump if not above", false_label);
                break;
            case CMP_GREATER_EQUAL:
                snprintf(buffer, sizeof(buffer), "    jb .L%d              ; Jump if below", false_label);
                break;
        }
        codegen_emit(gen, buffer);
        return;
    }
    
    // Integer comparison (default)
    codegen_generate_expression_value(gen, condition->comparison.left);
    codegen_emit(gen, "    push rax");
    
    codegen_generate_expression_value(gen, condition->comparison.right);
    codegen_emit(gen, "    mov rbx, rax         ; Right value in rbx");
    codegen_emit(gen, "    pop rax              ; Left value in rax");
    codegen_emit(gen, "    cmp rax, rbx");
    
    // Generate conditional jump based on operator
    switch (condition->comparison.op) {
        case CMP_EQUAL:
            snprintf(buffer, sizeof(buffer), "    jne .L%d", false_label);
            break;
        case CMP_NOT_EQUAL:
            snprintf(buffer, sizeof(buffer), "    je .L%d", false_label);
            break;
        case CMP_LESS:
            snprintf(buffer, sizeof(buffer), "    jge .L%d", false_label);
            break;
        case CMP_LESS_EQUAL:
            snprintf(buffer, sizeof(buffer), "    jg .L%d", false_label);
            break;
        case CMP_GREATER:
            snprintf(buffer, sizeof(buffer), "    jle .L%d", false_label);
            break;
        case CMP_GREATER_EQUAL:
            snprintf(buffer, sizeof(buffer), "    jl .L%d", false_label);
            break;
    }
    codegen_emit(gen, buffer);
}
