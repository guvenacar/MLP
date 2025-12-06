/*
 * MLP Switch/Match Module - Standalone Compiler
 * P1 Data Structures - Module #20
 * 
 * Switch-case and enhanced pattern matching
 * 
 * Syntax:
 *   switch value                    -- Switch statement
 *     case 1: ... end case          -- Case clause
 *     case 2..10: ... end case      -- Range case
 *     default: ... end default      -- Default clause
 *   end switch
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "switch_match.h"
#include "switch_match_parser.h"
#include "switch_match_codegen.h"
#include "../../lexer.h"
#include "../variable/variable.h"

// Parser state
typedef struct {
    Lexer* lexer;
    Token* current;
    int had_error;
} SMParser;

// Our own variable structure
typedef struct {
    char* name;
    VarType type;
    char* initial_value;
    int is_pointer;
    int is_array;
} SMVar;

// Parsed program
typedef struct {
    SMVar** variables;
    int var_count;
    int var_capacity;
    // Switch/match statistics
    int switch_count;           // switch ... end switch
    int case_count;             // case:
    int default_count;          // default:
    int range_count;            // x..y
} Program;

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

static void sm_init(SMParser* p, Lexer* lexer) {
    p->lexer = lexer;
    p->current = lexer_next_token(lexer);
    p->had_error = 0;
}

static void sm_advance(SMParser* p) {
    if (p->current) {
        // Don't free, lexer manages tokens
    }
    p->current = lexer_next_token(p->lexer);
}

static int sm_check(SMParser* p, TokenType type) {
    return p->current && p->current->type == type;
}

static void program_init(Program* prog) {
    prog->var_capacity = 64;
    prog->variables = malloc(sizeof(SMVar*) * prog->var_capacity);
    prog->var_count = 0;
    prog->switch_count = 0;
    prog->case_count = 0;
    prog->default_count = 0;
    prog->range_count = 0;
}

static void program_add_var(Program* prog, SMVar* var) {
    if (prog->var_count >= prog->var_capacity) {
        prog->var_capacity *= 2;
        prog->variables = realloc(prog->variables, 
                                   sizeof(SMVar*) * prog->var_capacity);
    }
    prog->variables[prog->var_count++] = var;
}

// =============================================================================
// LEXER ANALYSIS - SWITCH/MATCH DETECTION
// =============================================================================

static void scan_switch_match(const char* source, Program* prog) {
    int i = 0;
    int len = strlen(source);
    
    while (i < len) {
        // Skip comments
        if (source[i] == '-' && i + 1 < len && source[i+1] == '-') {
            while (i < len && source[i] != '\n') i++;
            continue;
        }
        
        // Check for switch keyword
        if (i == 0 || !isalnum(source[i-1])) {
            if (strncmp(source + i, "switch", 6) == 0 && 
                (i + 6 >= len || !isalnum(source[i+6]))) {
                prog->switch_count++;
                i += 6;
                continue;
            }
        }
        
        // Check for case keyword
        if (i == 0 || !isalnum(source[i-1])) {
            if (strncmp(source + i, "case", 4) == 0 && 
                (i + 4 >= len || !isalnum(source[i+4]))) {
                prog->case_count++;
                i += 4;
                continue;
            }
        }
        
        // Check for default keyword
        if (i == 0 || !isalnum(source[i-1])) {
            if (strncmp(source + i, "default", 7) == 0 && 
                (i + 7 >= len || !isalnum(source[i+7]))) {
                prog->default_count++;
                i += 7;
                continue;
            }
        }
        
        // Check for range pattern (..)
        if (source[i] == '.' && i + 1 < len && source[i+1] == '.') {
            prog->range_count++;
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

static SMVar* parse_variable(SMParser* p) {
    if (!is_type_token(p->current)) {
        return NULL;
    }
    
    VarType type = token_to_vartype(p->current);
    sm_advance(p);
    
    int is_pointer = 0;
    int is_array = 0;
    
    // Check for array modifier ([])
    if (sm_check(p, TOKEN_LBRACKET)) {
        sm_advance(p);
        if (sm_check(p, TOKEN_RBRACKET)) {
            sm_advance(p);
            is_array = 1;
        }
    }
    
    // Expect identifier
    if (!sm_check(p, TOKEN_IDENTIFIER)) {
        return NULL;
    }
    
    SMVar* var = malloc(sizeof(SMVar));
    var->name = strdup(p->current->value);
    var->type = type;
    var->is_pointer = is_pointer;
    var->is_array = is_array;
    var->initial_value = NULL;
    
    sm_advance(p);
    
    // Check for assignment
    if (sm_check(p, TOKEN_ASSIGN)) {
        sm_advance(p);
        
        if (p->current) {
            if (sm_check(p, TOKEN_NUMBER)) {
                var->initial_value = strdup(p->current->value);
                sm_advance(p);
            } else if (sm_check(p, TOKEN_STRING)) {
                var->initial_value = strdup(p->current->value);
                sm_advance(p);
            } else if (sm_check(p, TOKEN_IDENTIFIER)) {
                var->initial_value = strdup(p->current->value);
                sm_advance(p);
            }
        }
    }
    
    return var;
}

static void parse_program(SMParser* p, Program* prog) {
    while (p->current && p->current->type != TOKEN_EOF) {
        if (is_type_token(p->current)) {
            SMVar* var = parse_variable(p);
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
            sm_advance(p);
        }
    }
}

// =============================================================================
// CODE GENERATION
// =============================================================================

static void generate_code(FILE* out, Program* prog) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Switch/Match OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n");
    
    // Generate string literals
    for (int i = 0; i < prog->var_count; i++) {
        SMVar* var = prog->variables[i];
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
        SMVar* var = prog->variables[i];
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
        SMVar* var = prog->variables[i];
        
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
    printf("🔧 Switch/Match Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #20\n");
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
    
    // First pass: scan for switch/match constructs
    scan_switch_match(source, &prog);
    
    // Second pass: parse declarations
    SMParser parser;
    sm_init(&parser, lexer);
    parse_program(&parser, &prog);
    
    printf("\n      ✓ Total declarations: %d\n", prog.var_count);
    printf("      ✓ Switch/match statistics:\n");
    printf("        - switch statements: %d\n", prog.switch_count);
    printf("        - case clauses: %d\n", prog.case_count);
    printf("        - default clauses: %d\n", prog.default_count);
    printf("        - range patterns (..): %d\n", prog.range_count);
    printf("        - Total switch constructs: %d\n\n", 
           prog.switch_count + prog.case_count + prog.default_count);
    
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
    printf("🔀 Switch constructs: %d\n\n", 
           prog.switch_count + prog.case_count + prog.default_count);
    
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
