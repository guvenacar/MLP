// MELP Bootstrap Compiler - Code Generator
// Generates x86-64 assembly that uses runtime.c functions

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "parser.h"

static FILE *output;
static int var_count = 0;
static int label_counter = 0;

void codegen_init(const char *filename) {
    output = fopen(filename, "w");
    if (!output) {
        fprintf(stderr, "Error: cannot open output file %s\n", filename);
        exit(1);
    }
}

void codegen_close() {
    if (output) {
        fclose(output);
        output = NULL;
    }
}

static void emit(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(output, fmt, args);
    va_end(args);
}

static void gen_header() {
    emit("; MELP Bootstrap Compiler Output\n");
    emit("; Generated assembly using runtime.c functions\n\n");
    emit("section .data\n");
    emit("    startup_msg: db 'MELP Module Running...', 10, 0\n\n");
}

static void gen_bss() {
    emit("\nsection .bss\n");
}

static void gen_var_decl(ASTNode *node) {
    if (!node->left) return;
    
    const char *var_name = node->left->value;
    const char *type = node->value;
    
    // Debug: check if variable name is empty
    if (!var_name || var_name[0] == '\0') {
        fprintf(stderr, "[Warning] Empty variable name, using placeholder\n");
        var_name = "var_unknown";
    }
    
    // Allocate space in .bss section
    if (strcmp(type, "numeric") == 0) {
        emit("    %s: resq 1    ; numeric variable\n", var_name);
    } else if (strcmp(type, "string") == 0) {
        emit("    %s: resq 1    ; string pointer\n", var_name);
    } else if (strcmp(type, "boolean") == 0) {
        emit("    %s: resb 1    ; boolean variable\n", var_name);
    }
    
    var_count++;
}

static void gen_text_section() {
    emit("\nsection .text\n");
    emit("    global _start\n");
    emit("    extern mlp_print_string\n");
    emit("    extern mlp_exit\n\n");
}

static void gen_main() {
    emit("_start:\n");
    emit("    ; Print startup message\n");
    emit("    lea rdi, [startup_msg]\n");
    emit("    call mlp_print_string\n\n");
    
    emit("    ; Exit program\n");
    emit("    xor rdi, rdi\n");
    emit("    call mlp_exit\n");
}

static void gen_if(ASTNode *node) {
    int label_id = label_counter++;
    
    emit("    ; if statement\n");
    
    // Condition evaluation (simple: compare with 0)
    if (node->left) {
        emit("    cmp qword [%s], 0\n", node->left->value);
    }
    
    // Jump if false
    emit("    je .else_%d\n", label_id);
    
    // Then body
    emit("    ; then body\n");
    ASTNode *body = node->right;
    while (body) {
        if (body->type == NODE_VAR_DECL) {
            gen_var_decl(body);
        }
        body = body->next;
    }
    
    emit("    jmp .end_if_%d\n", label_id);
    emit(".else_%d:\n", label_id);
    
    // Else body (empty for now)
    emit("    ; else body\n");
    
    emit(".end_if_%d:\n", label_id);
}

static void gen_while(ASTNode *node) {
    int label_id = label_counter++;
    
    emit(".while_start_%d:\n", label_id);
    emit("    ; while condition\n");
    
    if (node->left) {
        emit("    cmp qword [%s], 0\n", node->left->value);
    }
    
    emit("    je .while_end_%d\n", label_id);
    
    // While body
    emit("    ; while body\n");
    ASTNode *body = node->right;
    while (body) {
        if (body->type == NODE_VAR_DECL) {
            gen_var_decl(body);
        }
        body = body->next;
    }
    
    emit("    jmp .while_start_%d\n", label_id);
    emit(".while_end_%d:\n", label_id);
}

void codegen(ASTNode *ast) {
    if (!output) return;
    
    gen_header();
    gen_bss();
    
    // Generate variable declarations
    ASTNode *node = ast;
    while (node) {
        if (node->type == NODE_VAR_DECL) {
            gen_var_decl(node);
        }
        node = node->next;
    }
    
    gen_text_section();
    gen_main();
    
    // Generate if/while statements
    node = ast;
    while (node) {
        if (node->type == NODE_IF) {
            gen_if(node);
        } else if (node->type == NODE_WHILE) {
            gen_while(node);
        }
        node = node->next;
    }
    
    fprintf(stderr, "[Codegen] Generated assembly with %d variables\n", var_count);
}
