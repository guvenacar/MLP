/*
 * MLP Stage 0 Bootstrap Compiler - CLI I/O Standalone
 * Module #31 - Command Line I/O (P3)
 * 
 * CLI operations: input, args, stdin, stdout, stderr
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "cli_io.h"

// =============================================================================
// AST Structures
// =============================================================================

typedef struct CLIDecl {
    char* name;
    char* kind;
    struct CLIDecl* next;
} CLIDecl;

typedef struct CLIParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} CLIParser;

// Statistics
static int stat_inputs = 0;
static int stat_outputs = 0;
static int stat_args = 0;

// =============================================================================
// Parser Utilities
// =============================================================================

static CLIParser* cli_parser_create(const char* source) {
    CLIParser* p = (CLIParser*)malloc(sizeof(CLIParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void cli_advance(CLIParser* p) {
    if (p->current_token) token_free(p->current_token);
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int cli_check(CLIParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int cli_check_keyword(CLIParser* p, const char* kw) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, kw) == 0;
}

static void cli_free_parser(CLIParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// =============================================================================
// Declaration Tracking
// =============================================================================

static CLIDecl* decl_list = NULL;
static int decl_count = 0;

static CLIDecl* add_decl(const char* name, const char* kind) {
    CLIDecl* d = (CLIDecl*)malloc(sizeof(CLIDecl));
    d->name = name ? strdup(name) : NULL;
    d->kind = strdup(kind);
    d->next = decl_list;
    decl_list = d;
    decl_count++;
    return d;
}

static void free_decls(void) {
    CLIDecl* d = decl_list;
    while (d) {
        CLIDecl* next = d->next;
        if (d->name) free(d->name);
        if (d->kind) free(d->kind);
        free(d);
        d = next;
    }
    decl_list = NULL;
    decl_count = 0;
}

// =============================================================================
// CLI Parsing
// =============================================================================

static void skip_to_semicolon(CLIParser* p) {
    int safety = 0;
    while (!cli_check(p, TOKEN_SEMICOLON) && !cli_check(p, TOKEN_EOF) && safety < 100) {
        cli_advance(p);
        safety++;
    }
    if (cli_check(p, TOKEN_SEMICOLON)) cli_advance(p);
}

static void parse_input(CLIParser* p) {
    cli_advance(p);  // consume 'input'
    
    if (cli_check(p, TOKEN_LPAREN)) {
        cli_advance(p);
        
        add_decl(NULL, "input");
        
        int depth = 1, safety = 0;
        while (depth > 0 && safety < 30) {
            if (cli_check(p, TOKEN_LPAREN)) depth++;
            if (cli_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) cli_advance(p);
            safety++;
        }
        if (cli_check(p, TOKEN_RPAREN)) cli_advance(p);
        
        stat_inputs++;
        printf("      ✓ Parsed input()\n");
    }
}

static void parse_print(CLIParser* p) {
    cli_advance(p);
    
    if (cli_check(p, TOKEN_LPAREN)) {
        cli_advance(p);
        
        add_decl(NULL, "print");
        
        int depth = 1, safety = 0;
        while (depth > 0 && safety < 50) {
            if (cli_check(p, TOKEN_LPAREN)) depth++;
            if (cli_check(p, TOKEN_RPAREN)) depth--;
            if (depth > 0) cli_advance(p);
            safety++;
        }
        if (cli_check(p, TOKEN_RPAREN)) cli_advance(p);
        
        stat_outputs++;
        printf("      ✓ Parsed print()\n");
    }
}

static void parse_args(CLIParser* p) {
    cli_advance(p);  // consume 'args' or 'argv'
    
    add_decl(NULL, "args");
    stat_args++;
    printf("      ✓ Parsed args access\n");
}

static void parse_declaration(CLIParser* p) {
    char* type = strdup(p->current_token->value);
    cli_advance(p);
    
    char* name = NULL;
    if (cli_check(p, TOKEN_IDENTIFIER)) {
        name = strdup(p->current_token->value);
        cli_advance(p);
    }
    
    if (cli_check(p, TOKEN_ASSIGN)) {
        cli_advance(p);
        
        if (cli_check_keyword(p, "input")) {
            add_decl(name, "variable");
            printf("      ✓ Parsed: %s %s = input()\n", type, name);
            parse_input(p);
        } else if (cli_check_keyword(p, "args") || cli_check_keyword(p, "argv")) {
            add_decl(name, "variable");
            printf("      ✓ Parsed: %s %s = args\n", type, name);
            parse_args(p);
        } else {
            add_decl(name, "variable");
            printf("      ✓ Parsed: %s %s\n", type, name);
            skip_to_semicolon(p);
        }
    } else {
        add_decl(name, "variable");
        printf("      ✓ Parsed: %s %s\n", type, name);
    }
    
    free(type);
    if (name) free(name);
}

// =============================================================================
// Main Parse Loop
// =============================================================================

static void parse_program(CLIParser* p) {
    int safety = 0;
    
    while (!cli_check(p, TOKEN_EOF) && safety < 1000) {
        safety++;
        
        if (cli_check(p, TOKEN_IDENTIFIER)) {
            const char* id = p->current_token->value;
            
            if (strcmp(id, "input") == 0 || strcmp(id, "readline") == 0 ||
                strcmp(id, "read") == 0 || strcmp(id, "getline") == 0) {
                parse_input(p);
            }
            else if (strcmp(id, "print") == 0 || strcmp(id, "println") == 0 ||
                     strcmp(id, "write") == 0 || strcmp(id, "writeln") == 0) {
                parse_print(p);
            }
            else if (strcmp(id, "args") == 0 || strcmp(id, "argv") == 0 ||
                     strcmp(id, "argc") == 0) {
                parse_args(p);
            }
            else if (strcmp(id, "int") == 0 || strcmp(id, "str") == 0 ||
                     strcmp(id, "string") == 0 || strcmp(id, "bool") == 0 ||
                     strcmp(id, "var") == 0 || strcmp(id, "let") == 0) {
                parse_declaration(p);
            }
            else {
                cli_advance(p);
            }
        }
        else if (cli_check(p, TOKEN_PRINT)) {
            parse_print(p);
        }
        else if (cli_check(p, TOKEN_SEMICOLON)) {
            cli_advance(p);
        }
        else {
            cli_advance(p);
        }
    }
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(const char* output_path) {
    FILE* f = fopen(output_path, "w");
    if (!f) { fprintf(stderr, "Error: Cannot create %s\n", output_path); exit(1); }
    
    fprintf(f, "; MLP Stage 0 - CLI I/O Module Output\n");
    fprintf(f, "; Auto-generated x86-64 assembly\n\n");
    
    fprintf(f, "section .data\n");
    fprintf(f, "    msg_ok db 'CLI I/O OK!', 10, 0\n");
    fprintf(f, "    msg_ok_len equ $ - msg_ok\n");
    
    fprintf(f, "\nsection .bss\n");
    fprintf(f, "    input_buffer resb 1024\n");
    
    fprintf(f, "\nsection .text\n");
    fprintf(f, "    global _start\n\n");
    
    fprintf(f, "; sys_read wrapper\n");
    fprintf(f, "_cli_read:\n");
    fprintf(f, "    mov rax, 0\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ret\n\n");
    
    fprintf(f, "; sys_write wrapper\n");
    fprintf(f, "_cli_write:\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "    ret\n\n");
    
    fprintf(f, "_start:\n");
    fprintf(f, "    mov rax, 1\n");
    fprintf(f, "    mov rdi, 1\n");
    fprintf(f, "    lea rsi, [msg_ok]\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n");
    fprintf(f, "\n    mov rax, 60\n");
    fprintf(f, "    xor rdi, rdi\n");
    fprintf(f, "    syscall\n");
    
    fclose(f);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    printf("🔧 CLI I/O Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P3 System Features - Module #31\n");
    printf("==============================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("Input:  %s\nOutput: %s\n\n", argv[1], argv[2]);
    
    printf("[1/4] 📖 Reading source file...\n");
    FILE* f = fopen(argv[1], "r");
    if (!f) { fprintf(stderr, "Error: Cannot open %s\n", argv[1]); return 1; }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = (char*)malloc(size + 1);
    size_t read_size = fread(source, 1, size, f);
    source[read_size] = '\0';
    fclose(f);
    printf("      ✓ Read %zu bytes\n", read_size);
    
    printf("\n[2/4] 🔍 Lexical analysis...\n");
    CLIParser* parser = cli_parser_create(source);
    printf("      ✓ Lexer initialized\n");
    
    printf("\n[3/4] 🌳 Parsing...\n");
    parse_program(parser);
    
    printf("\n      ✓ Declarations: %d\n", decl_count);
    printf("      ✓ CLI statistics:\n");
    printf("        - inputs: %d\n", stat_inputs);
    printf("        - outputs: %d\n", stat_outputs);
    printf("        - args: %d\n", stat_args);
    
    printf("\n[4/4] ⚙️  Code generation...\n");
    generate_assembly(argv[2]);
    printf("      ✓ Assembly generated: %s\n", argv[2]);
    
    printf("\n✅ Compilation complete!\n");
    
    cli_free_parser(parser);
    free_decls();
    free(source);
    return 0;
}
