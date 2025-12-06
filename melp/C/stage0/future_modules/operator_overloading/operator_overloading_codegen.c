#include "operator_overloading_codegen.h"
#include <string.h>

// Operator overload için kod üret
void codegen_operator_overload(FILE* out, OperatorOverload* overload) {
    fprintf(out, "    ; Operator overload: %s for %s\n",
            operator_type_to_string(overload->op),
            overload->type_name);
    
    // Function signature
    fprintf(out, "\n.op_%s_%s:\n", 
            overload->type_name,
            operator_type_to_string(overload->op));
    
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    if (overload->is_binary) {
        codegen_binary_operator(out, overload);
    } else {
        codegen_unary_operator(out, overload);
    }
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Operator dispatch için kod üret
void codegen_operator_dispatch(FILE* out, OperatorDispatch* dispatch) {
    fprintf(out, "    ; Operator dispatch: %s\n",
            operator_type_to_string(dispatch->op));
    
    // Left operand type check
    fprintf(out, "    mov rax, [rbp - 8]    ; Left operand\n");
    codegen_operator_type_check(out, dispatch->left_type);
    
    if (dispatch->right_operand) {
        // Right operand type check
        fprintf(out, "    mov rbx, [rbp - 16]   ; Right operand\n");
        codegen_operator_type_check(out, dispatch->right_type);
    }
    
    // Dispatch lookup
    codegen_dispatch_lookup(out, dispatch->op);
    
    // Call implementation
    fprintf(out, "    call rax    ; Call overloaded operator\n");
}

// Custom operator için kod üret
void codegen_custom_operator(FILE* out, CustomOperator* custom) {
    fprintf(out, "    ; Custom operator: %s\n", custom->symbol);
    fprintf(out, "    ; Precedence: %d\n", custom->precedence);
    
    // Implementation
    fprintf(out, "\n.custom_op_%s:\n", custom->symbol);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    // Custom operator body
    fprintf(out, "    ; Custom operator implementation\n");
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Operator registry için kod üret
void codegen_operator_registry(FILE* out, OperatorRegistry* registry) {
    fprintf(out, "\n; Operator Registry\n");
    
    // Dispatch table
    codegen_dispatch_table(out, registry);
    
    // Registry functions
    fprintf(out, "\n.registry_find_overload:\n");
    fprintf(out, "    ; rdi = operator type\n");
    fprintf(out, "    ; rsi = left type\n");
    fprintf(out, "    ; rdx = right type\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    fprintf(out, "    ; Search dispatch table\n");
    fprintf(out, "    xor rcx, rcx    ; Index\n");
    fprintf(out, ".search_loop:\n");
    fprintf(out, "    cmp rcx, [rel .registry_count]\n");
    fprintf(out, "    jge .search_not_found\n");
    
    fprintf(out, "    ; Compare operator type\n");
    fprintf(out, "    mov rax, [rel .dispatch_table + rcx*8]\n");
    fprintf(out, "    cmp [rax], rdi\n");
    fprintf(out, "    jne .search_next\n");
    
    fprintf(out, "    ; Type matching\n");
    fprintf(out, "    ; TODO: Type comparison\n");
    
    fprintf(out, "    jmp .search_found\n");
    
    fprintf(out, ".search_next:\n");
    fprintf(out, "    inc rcx\n");
    fprintf(out, "    jmp .search_loop\n");
    
    fprintf(out, ".search_found:\n");
    fprintf(out, "    mov rax, [rel .dispatch_table + rcx*8]\n");
    fprintf(out, "    jmp .search_done\n");
    
    fprintf(out, ".search_not_found:\n");
    fprintf(out, "    xor rax, rax\n");
    
    fprintf(out, ".search_done:\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Dispatch table
void codegen_dispatch_table(FILE* out, OperatorRegistry* registry) {
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".dispatch_table:\n");
    
    for (int i = 0; i < registry->count; i++) {
        OperatorOverload* overload = registry->overloads[i];
        fprintf(out, "    dq .op_%s_%s\n",
                overload->type_name,
                operator_type_to_string(overload->op));
    }
    
    fprintf(out, ".registry_count: dq %d\n", registry->count);
    fprintf(out, "section .text\n");
}

// Dispatch lookup
void codegen_dispatch_lookup(FILE* out, OperatorType op) {
    fprintf(out, "    ; Dispatch lookup for operator: %s\n",
            operator_type_to_string(op));
    
    fprintf(out, "    mov rdi, %d    ; Operator type\n", op);
    fprintf(out, "    mov rsi, [rbp - 24]   ; Left type\n");
    fprintf(out, "    mov rdx, [rbp - 32]   ; Right type\n");
    fprintf(out, "    call .registry_find_overload\n");
    
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .dispatch_not_found\n");
}

// Dispatch call
void codegen_dispatch_call(FILE* out, OperatorOverload* overload) {
    fprintf(out, "    ; Call overloaded operator\n");
    
    // Load parameters
    fprintf(out, "    mov rdi, [rbp - 8]    ; Left operand\n");
    
    if (overload->is_binary) {
        fprintf(out, "    mov rsi, [rbp - 16]   ; Right operand\n");
    }
    
    // Call
    fprintf(out, "    call .op_%s_%s\n",
            overload->type_name,
            operator_type_to_string(overload->op));
}

// Type check
void codegen_operator_type_check(FILE* out, const char* type) {
    if (!type) return;
    
    fprintf(out, "    ; Type check: %s\n", type);
    fprintf(out, "    mov rbx, [rax]    ; Get type info\n");
    fprintf(out, "    lea rcx, [rel .type_%s]\n", type);
    fprintf(out, "    cmp rbx, rcx\n");
    fprintf(out, "    jne .type_error\n");
}

// Type match
void codegen_type_match(FILE* out, const char* expected, const char* actual) {
    fprintf(out, "    ; Type match: %s == %s\n", expected, actual);
    fprintf(out, "    lea rax, [rel .type_%s]\n", expected);
    fprintf(out, "    lea rbx, [rel .type_%s]\n", actual);
    fprintf(out, "    cmp rax, rbx\n");
    fprintf(out, "    sete al\n");
    fprintf(out, "    movzx rax, al\n");
}

// Binary operator
void codegen_binary_operator(FILE* out, OperatorOverload* overload) {
    fprintf(out, "    ; Binary operator: %s\n",
            operator_type_to_string(overload->op));
    
    fprintf(out, "    mov rax, [rbp + 16]   ; Left (param 1)\n");
    fprintf(out, "    mov rbx, [rbp + 24]   ; Right (param 2)\n");
    
    // Implementation body burada
    fprintf(out, "    ; Operator implementation\n");
    
    fprintf(out, "    ; Result in rax\n");
}

// Unary operator
void codegen_unary_operator(FILE* out, OperatorOverload* overload) {
    fprintf(out, "    ; Unary operator: %s\n",
            operator_type_to_string(overload->op));
    
    fprintf(out, "    mov rax, [rbp + 16]   ; Operand\n");
    
    // Implementation body burada
    fprintf(out, "    ; Operator implementation\n");
    
    fprintf(out, "    ; Result in rax\n");
}

// Custom operator precedence
void codegen_custom_precedence(FILE* out, CustomOperator* custom) {
    fprintf(out, "section .data\n");
    fprintf(out, ".custom_op_%s_prec: dq %d\n", 
            custom->symbol, custom->precedence);
    fprintf(out, ".custom_op_%s_assoc: db %d\n",
            custom->symbol, custom->is_left_assoc);
    fprintf(out, "section .text\n");
}
