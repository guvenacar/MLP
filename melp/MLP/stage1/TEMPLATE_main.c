/* 
 * MLP Compiler v2 - Built from Stage 0 Modules
 * Simple bootstrap compiler to enable MLP self-hosting
 * 
 * Build:
 *   gcc -o melpc2 main.c lexer.c -I../C/stage0/modules -L../C/stage0/modules -lm
 *   
 * Usage:
 *   ./melpc2 input.mlp output.s
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === MINIMAL LEXER (Replace with proper lexer.c) ===

typedef enum {
    TOK_NUMERIC = 0,
    TOK_TEXT = 1,
    TOK_FUNCTION = 31,
    TOK_RETURN = 32,
    TOK_IDENTIFIER = 3,
    TOK_NUMBER = 5,
    TOK_ASSIGN = 4,
    TOK_LPAREN = 34,
    TOK_RPAREN = 35,
    TOK_EOF = 41,
    TOK_ERROR = 42
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
    int line;
} Token;

// Global state (simple)
char* g_source = NULL;
int g_pos = 0;
int g_line = 1;

void lexer_init(const char* source) {
    g_source = strdup(source);
    g_pos = 0;
    g_line = 1;
}

Token lexer_next_token() {
    Token tok = {TOK_ERROR, "", g_line};
    
    // Skip whitespace
    while (g_source[g_pos] == ' ' || g_source[g_pos] == '\n' || g_source[g_pos] == '\t') {
        if (g_source[g_pos] == '\n') g_line++;
        g_pos++;
    }
    
    // EOF
    if (g_source[g_pos] == '\0') {
        tok.type = TOK_EOF;
        return tok;
    }
    
    // Identifiers and keywords
    if (isalpha(g_source[g_pos]) || g_source[g_pos] == '_') {
        int start = g_pos;
        while (isalnum(g_source[g_pos]) || g_source[g_pos] == '_') g_pos++;
        int len = g_pos - start;
        strncpy(tok.value, g_source + start, len);
        tok.value[len] = '\0';
        
        if (strcmp(tok.value, "numeric") == 0) tok.type = TOK_NUMERIC;
        else if (strcmp(tok.value, "text") == 0) tok.type = TOK_TEXT;
        else if (strcmp(tok.value, "function") == 0) tok.type = TOK_FUNCTION;
        else if (strcmp(tok.value, "return") == 0) tok.type = TOK_RETURN;
        else tok.type = TOK_IDENTIFIER;
        return tok;
    }
    
    // Numbers
    if (isdigit(g_source[g_pos])) {
        int start = g_pos;
        while (isdigit(g_source[g_pos]) || g_source[g_pos] == '.') g_pos++;
        int len = g_pos - start;
        strncpy(tok.value, g_source + start, len);
        tok.value[len] = '\0';
        tok.type = TOK_NUMBER;
        return tok;
    }
    
    // Single char tokens
    if (g_source[g_pos] == '=') {
        tok.type = TOK_ASSIGN;
        tok.value[0] = '=';
        tok.value[1] = '\0';
        g_pos++;
        return tok;
    }
    
    if (g_source[g_pos] == '(') {
        tok.type = TOK_LPAREN;
        tok.value[0] = '(';
        tok.value[1] = '\0';
        g_pos++;
        return tok;
    }
    
    if (g_source[g_pos] == ')') {
        tok.type = TOK_RPAREN;
        tok.value[0] = ')';
        tok.value[1] = '\0';
        g_pos++;
        return tok;
    }
    
    // Unknown
    tok.value[0] = g_source[g_pos];
    tok.value[1] = '\0';
    g_pos++;
    return tok;
}

// === MINIMAL PARSER & CODEGEN ===

void parse_and_generate(FILE* out) {
    fprintf(out, "section .data\n");
    fprintf(out, "section .text\n");
    fprintf(out, "global _start\n");
    fprintf(out, "_start:\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
    
    // TODO: Implement proper parser
    // Use Stage 0 modules: parser_core, expression, statement, etc.
}

// === MAIN ===

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.mlp output.s\n", argv[0]);
        return 1;
    }
    
    // Read input
    FILE* in = fopen(argv[1], "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, in);
    source[size] = '\0';
    fclose(in);
    
    // Lex and parse
    lexer_init(source);
    
    // Generate assembly
    FILE* out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(source);
        return 1;
    }
    
    parse_and_generate(out);
    fclose(out);
    
    printf("✅ Compiled: %s -> %s\n", argv[1], argv[2]);
    
    free(source);
    return 0;
}
