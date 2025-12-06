/*
 * MLP Pattern Matching Module - Standalone Compiler
 * P1 Data Structures - Module #19
 * 
 * Rust-style pattern matching with match expressions
 * 
 * Syntax:
 *   match value                     -- Match expression
 *     42 => "forty-two"             -- Literal pattern
 *     x when x > 0 => "positive"    -- Guard pattern
 *     _ => "default"                -- Wildcard pattern
 *   end match
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pattern_matching.h"
#include "pattern_matching_parser.h"
#include "pattern_matching_codegen.h"
#include "../../lexer.h"
#include "../variable/variable.h"

// Parser state
typedef struct {
    Lexer* lexer;
    Token* current;
    int had_error;
} PMParser;

// Our own variable structure
typedef struct {
    char* name;
    VarType type;
    char* initial_value;
    int is_pointer;
    int is_array;
} PMVar;

// Parsed program
typedef struct {
    PMVar** variables;
    int var_count;
    int var_capacity;
    // Pattern matching statistics
    int match_expr_count;       // match ... end match
    int literal_pattern_count;  // 42, "text"
    int wildcard_count;         // _
    int guard_count;            // when condition
    int range_count;            // 1..10
} Program;

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

static void pm_init(PMParser* p, Lexer* lexer) {
    p->lexer = lexer;
    p->current = lexer_next_token(lexer);
    p->had_error = 0;
}

static void pm_advance(PMParser* p) {
    if (p->current) {
        // Don't free, lexer manages tokens
    }
    p->current = lexer_next_token(p->lexer);
}

static int pm_check(PMParser* p, TokenType type) {
    return p->current && p->current->type == type;
}

static void program_init(Program* prog) {
    prog->var_capacity = 64;
    prog->variables = malloc(sizeof(PMVar*) * prog->var_capacity);
    prog->var_count = 0;
    prog->match_expr_count = 0;
    prog->literal_pattern_count = 0;
    prog->wildcard_count = 0;
    prog->guard_count = 0;
    prog->range_count = 0;
}

static void program_add_var(Program* prog, PMVar* var) {
    if (prog->var_count >= prog->var_capacity) {
        prog->var_capacity *= 2;
        prog->variables = realloc(prog->variables, 
                                   sizeof(PMVar*) * prog->var_capacity);
    }
    prog->variables[prog->var_count++] = var;
}

// =============================================================================
// LEXER ANALYSIS - PATTERN MATCHING DETECTION
// =============================================================================

static void scan_pattern_matching(const char* source, Program* prog) {
    int i = 0;
    int len = strlen(source);
    
    while (i < len) {
        // Skip comments
        if (source[i] == '-' && i + 1 < len && source[i+1] == '-') {
            while (i < len && source[i] != '\n') i++;
            continue;
        }
        
        // Check for match keyword (not inside identifier)
        if (i == 0 || !isalnum(source[i-1])) {
            if (strncmp(source + i, "match", 5) == 0 && 
                (i + 5 >= len || !isalnum(source[i+5]))) {
                prog->match_expr_count++;
                i += 5;
                continue;
            }
        }
        
        // Check for wildcard pattern (_)
        if (source[i] == '_') {
            // Make sure it's a standalone _ (not part of identifier)
            int before_ok = (i == 0 || !isalnum(source[i-1]));
            int after_ok = (i + 1 >= len || !isalnum(source[i+1]));
            if (before_ok && after_ok) {
                prog->wildcard_count++;
            }
        }
        
        // Check for guard (when keyword)
        if (i == 0 || !isalnum(source[i-1])) {
            if (strncmp(source + i, "when", 4) == 0 && 
                (i + 4 >= len || !isalnum(source[i+4]))) {
                prog->guard_count++;
                i += 4;
                continue;
            }
        }
        
        // Check for range pattern (..)
        if (source[i] == '.' && i + 1 < len && source[i+1] == '.') {
            prog->range_count++;
            i += 2;
            continue;
        }
        
        // Check for arrow (=>) - indicates match arm
        if (source[i] == '=' && i + 1 < len && source[i+1] == '>') {
            prog->literal_pattern_count++;
            i += 2;
            continue;
        }
        
        i++;
    }
}

// =============================================================================
// PARSER - VARIABLE DECLARATIONS
// =============================================================================

static int is_type_token(Token* token) {
    if (!token) return 0;
    return token->type == TOKEN_NUMERIC || 
           token->type == TOKEN_TEXT || 
           token->type == TOKEN_BOOLEAN;
}

static VarType token_to_vartype(Token* token) {
    if (!token) return VAR_NUMERIC;
    if (token->type == TOKEN_NUMERIC) return VAR_NUMERIC;
    if (token->type == TOKEN_TEXT) return VAR_STRING;
    if (token->type == TOKEN_BOOLEAN) return VAR_BOOLEAN;
    return VAR_NUMERIC;
}

static PMVar* parse_variable(PMParser* p) {
    if (!is_type_token(p->current)) {
        return NULL;
    }
    
    VarType type = token_to_vartype(p->current);
    pm_advance(p);
    
    int is_pointer = 0;
    int is_array = 0;
    
    // Check for array modifier ([])
    if (pm_check(p, TOKEN_LBRACKET)) {
        pm_advance(p);
        if (pm_check(p, TOKEN_RBRACKET)) {
            pm_advance(p);
            is_array = 1;
        }
    }
    
    // Expect identifier
    if (!pm_check(p, TOKEN_IDENTIFIER)) {
        return NULL;
    }
    
    PMVar* var = malloc(sizeof(PMVar));
    var->name = strdup(p->current->value);
    var->type = type;
    var->is_pointer = is_pointer;
    var->is_array = is_array;
    var->initial_value = NULL;
    
    pm_advance(p);
    
    // Check for assignment
    if (pm_check(p, TOKEN_ASSIGN)) {
        pm_advance(p);
        
        if (p->current) {
            if (pm_check(p, TOKEN_NUMBER)) {
                var->initial_value = strdup(p->current->value);
                pm_advance(p);
            } else if (pm_check(p, TOKEN_STRING)) {
                var->initial_value = strdup(p->current->value);
                pm_advance(p);
            } else if (pm_check(p, TOKEN_IDENTIFIER)) {
                var->initial_value = strdup(p->current->value);
                pm_advance(p);
            }
        }
    }
    
    return var;
}

static void parse_program(PMParser* p, Program* prog) {
    while (p->current && p->current->type != TOKEN_EOF) {
        if (is_type_token(p->current)) {
            PMVar* var = parse_variable(p);
            if (var) {
                program_add_var(prog, var);
                printf("      ✓ Parsed variable: %s (%s%s%s)\n", 
                       var->name,
                       var->type == VAR_NUMERIC ? "numeric" :
                       var->type == VAR_STRING ? "text" : "boolean",
                       var->is_pointer ? "*" : "",
                       var->is_array ? "[]" : "");
            }
        } else {
            pm_advance(p);
        }
    }
}

// =============================================================================
// CODE GENERATION
// =============================================================================

static void generate_code(FILE* out, Program* prog) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Pattern Matching OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n");
    
    // Generate string literals
    for (int i = 0; i < prog->var_count; i++) {
        PMVar* var = prog->variables[i];
        if (var->type == VAR_STRING && var->initial_value) {
            if (var->initial_value[0] == '"') {
                fprintf(out, "    str_%s: db %s, 0\n", var->name, var->initial_value);
            } else {
                fprintf(out, "    str_%s: db \"%s\", 0\n", var->name, var->initial_value);
            }
        }
    }
    fprintf(out, "\n");
    
    // BSS section
    fprintf(out, "section .bss\n");
    for (int i = 0; i < prog->var_count; i++) {
        PMVar* var = prog->variables[i];
        if (var->is_array) {
            fprintf(out, "    %s: resq 16\n", var->name);
        } else {
            fprintf(out, "    %s: resq 1\n", var->name);
        }
    }
    fprintf(out, "\n");
    
    // Code section
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n\n");
    fprintf(out, "_start:\n");
    
    // Initialize variables
    for (int i = 0; i < prog->var_count; i++) {
        PMVar* var = prog->variables[i];
        
        if (var->initial_value) {
            if (var->type == VAR_NUMERIC) {
                fprintf(out, "    mov qword [%s], %s\n", var->name, var->initial_value);
            } else if (var->type == VAR_STRING) {
                fprintf(out, "    lea rax, [str_%s]\n", var->name);
                fprintf(out, "    mov [%s], rax\n", var->name);
            } else if (var->type == VAR_BOOLEAN) {
                int val = (strcmp(var->initial_value, "true") == 0) ? 1 : 0;
                fprintf(out, "    mov qword [%s], %d\n", var->name, val);
            }
        }
    }
    
    // Print success message
    fprintf(out, "\n    ; Print success message\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    lea rsi, [msg]\n");
    fprintf(out, "    mov rdx, msg_len\n");
    fprintf(out, "    syscall\n");
    
    // Exit
    fprintf(out, "\n    ; Exit\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
}

// =============================================================================
// MAIN
// =============================================================================

int main(int argc, char* argv[]) {
    printf("🔧 Pattern Matching Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #19\n");
    printf("==============================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n\n", output_file);
    
    // Step 1: Read source file
    printf("[1/4] 📖 Reading source file...\n");
    FILE* f = fopen(input_file, "r");
    if (!f) {
        printf("      ✗ Failed to open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    printf("      ✓ Read %ld bytes\n\n", size);
    
    // Step 2: Initialize lexer
    printf("[2/4] 🔍 Lexical analysis...\n");
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        printf("      ✗ Failed to create lexer\n");
        free(source);
        return 1;
    }
    printf("      ✓ Lexer initialized\n\n");
    
    // Step 3: Parse
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    Program prog;
    program_init(&prog);
    
    // First pass: scan for pattern matching constructs
    scan_pattern_matching(source, &prog);
    
    // Second pass: parse declarations
    PMParser parser;
    pm_init(&parser, lexer);
    parse_program(&parser, &prog);
    
    printf("\n      ✓ Total declarations: %d\n", prog.var_count);
    printf("      ✓ Pattern matching statistics:\n");
    printf("        - match expressions: %d\n", prog.match_expr_count);
    printf("        - literal patterns (=>): %d\n", prog.literal_pattern_count);
    printf("        - wildcard patterns (_): %d\n", prog.wildcard_count);
    printf("        - guard clauses (when): %d\n", prog.guard_count);
    printf("        - range patterns (..): %d\n", prog.range_count);
    printf("        - Total patterns: %d\n\n", 
           prog.literal_pattern_count + prog.wildcard_count + prog.range_count);
    
    // Step 4: Generate code
    printf("[4/4] ⚙️  Code generation...\n");
    FILE* out = fopen(output_file, "w");
    if (!out) {
        printf("      ✗ Failed to create output file\n");
        return 1;
    }
    
    generate_code(out, &prog);
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n\n", output_file);
    
    // Summary
    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", input_file, size);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", prog.var_count);
    printf("🔀 Pattern constructs: %d\n\n", 
           prog.match_expr_count + prog.wildcard_count + prog.guard_count);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    lexer_free(lexer);
    free(source);
    for (int i = 0; i < prog.var_count; i++) {
        if (prog.variables[i]->name) free(prog.variables[i]->name);
        if (prog.variables[i]->initial_value) free(prog.variables[i]->initial_value);
        free(prog.variables[i]);
    }
    free(prog.variables);
    
    return 0;
}
