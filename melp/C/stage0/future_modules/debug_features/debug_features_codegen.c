#include "debug_features_codegen.h"
#include <string.h>

// Debug label için kod üret
void codegen_debug_label(FILE* out, DebugLabel* label, DebugContext* ctx) {
    if (!ctx || !is_debug_mode(ctx)) {
        // Production mode: Label'ı skip et
        return;
    }
    
    fprintf(out, "    ; Debug label: %s (line %d)\n", 
            label->name, label->line_number);
    
    codegen_label_definition(out, label);
}

// Debug goto için kod üret
void codegen_debug_goto(FILE* out, DebugGoto* dgoto, DebugContext* ctx) {
    if (!ctx || !is_debug_mode(ctx)) {
        // Production mode: goto'yu skip et
        return;
    }
    
    fprintf(out, "    ; Debug goto: %s\n", dgoto->target_label);
    
    // Mode check
    codegen_debug_mode_check(out, ctx);
    
    // Conditional goto
    if (dgoto->is_conditional) {
        fprintf(out, "    ; Conditional goto\n");
        fprintf(out, "    cmp rax, 0\n");
        fprintf(out, "    je .skip_goto_%s\n", dgoto->target_label);
    }
    
    // Jump
    codegen_goto_jump(out, dgoto);
    
    if (dgoto->is_conditional) {
        fprintf(out, ".skip_goto_%s:\n", dgoto->target_label);
    }
}

// Debug pause için kod üret
void codegen_debug_pause(FILE* out, DebugPause* pause, DebugContext* ctx) {
    if (!ctx || !is_debug_mode(ctx)) {
        // Production mode: pause'u skip et
        return;
    }
    
    fprintf(out, "    ; Debug pause (breakpoint) at line %d\n", 
            pause->line_number);
    
    // Mode check
    codegen_debug_mode_check(out, ctx);
    
    // Conditional pause
    if (pause->is_conditional) {
        fprintf(out, "    ; Conditional pause\n");
        fprintf(out, "    cmp rax, 0\n");
        fprintf(out, "    je .skip_pause_%d\n", pause->line_number);
    }
    
    // Breakpoint
    codegen_breakpoint(out, pause);
    
    if (pause->is_conditional) {
        fprintf(out, ".skip_pause_%d:\n", pause->line_number);
    }
}

// Debug print için kod üret
void codegen_debug_print(FILE* out, DebugPrint* print, DebugContext* ctx) {
    if (!ctx || !is_debug_mode(ctx)) {
        // Production mode: print'i skip et
        return;
    }
    
    fprintf(out, "    ; Debug print: %s\n", print->var_name);
    
    // Mode check
    codegen_debug_mode_check(out, ctx);
    
    codegen_debug_var_print(out, print);
}

// Debug if için kod üret
void codegen_debug_if(FILE* out, DebugIf* dif, DebugContext* ctx) {
    if (!ctx || !is_debug_mode(ctx)) {
        // Production mode: debug if'i skip et
        return;
    }
    
    fprintf(out, "    ; Debug if block\n");
    
    // Mode check
    codegen_debug_mode_check(out, ctx);
    
    // Condition
    codegen_debug_if_condition(out, dif);
    
    fprintf(out, "    cmp rax, 0\n");
    fprintf(out, "    je .debug_if_else\n");
    
    // Then block
    fprintf(out, "    ; Debug if then\n");
    fprintf(out, "    ; Then block içinde goto ve pause debug keyword'süz\n");
    
    fprintf(out, "    jmp .debug_if_end\n");
    
    // Else block
    fprintf(out, ".debug_if_else:\n");
    if (dif->else_block) {
        fprintf(out, "    ; Debug if else\n");
    }
    
    fprintf(out, ".debug_if_end:\n");
}

// Debug mode check
void codegen_debug_mode_check(FILE* out, DebugContext* ctx) {
    fprintf(out, "    ; Check debug mode\n");
    fprintf(out, "    cmp byte [rel .debug_mode], 0\n");
    fprintf(out, "    je .debug_skip\n");
}

// Strip debug code (production)
void codegen_strip_debug(FILE* out) {
    fprintf(out, "    ; Debug code stripped (production mode)\n");
}

// Label definition
void codegen_label_definition(FILE* out, DebugLabel* label) {
    fprintf(out, "\n.debug_label_%s:\n", label->name);
    fprintf(out, "    ; Line %d\n", label->line_number);
}

// Goto jump
void codegen_goto_jump(FILE* out, DebugGoto* dgoto) {
    fprintf(out, "    jmp .debug_label_%s\n", dgoto->target_label);
}

// Breakpoint
void codegen_breakpoint(FILE* out, DebugPause* pause) {
    fprintf(out, "    ; BREAKPOINT\n");
    
    // Print message
    fprintf(out, "    lea rdi, [rel .debug_pause_msg_%d]\n", 
            pause->line_number);
    fprintf(out, "    call puts\n");
    
    // Sonsuz döngü önlemi: Production'da bu kod yok
    fprintf(out, "    ; Wait for debugger (nop)\n");
    fprintf(out, "    nop\n");
    
    // Message data
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".debug_pause_msg_%d: db \"[DEBUG] Breakpoint at line %d\"",
            pause->line_number, pause->line_number);
    
    if (pause->message) {
        fprintf(out, ", \": %s\"", pause->message);
    }
    
    fprintf(out, ", 0\n");
    fprintf(out, "section .text\n");
}

// Debug var print
void codegen_debug_var_print(FILE* out, DebugPrint* print) {
    fprintf(out, "    ; Print variable: %s\n", print->var_name);
    
    // Load variable value
    fprintf(out, "    mov rax, [rbp - 8]    ; Variable value\n");
    
    // Print format
    fprintf(out, "    lea rdi, [rel .debug_print_fmt_%s]\n", print->var_name);
    fprintf(out, "    mov rsi, rax\n");
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, "    call printf\n");
    
    // Format string
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".debug_print_fmt_%s: db \"[DEBUG] %s = ", 
            print->var_name, print->var_name);
    
    if (print->format) {
        fprintf(out, "%s", print->format);
    } else {
        fprintf(out, "%%ld");
    }
    
    fprintf(out, "\", 10, 0\n");
    fprintf(out, "section .text\n");
}

// Debug if condition
void codegen_debug_if_condition(FILE* out, DebugIf* dif) {
    fprintf(out, "    ; Debug if condition\n");
    fprintf(out, "    ; Evaluate condition -> rax\n");
    
    // Condition evaluation (Stage 0: Placeholder)
    fprintf(out, "    mov rax, 1    ; Placeholder condition\n");
}
