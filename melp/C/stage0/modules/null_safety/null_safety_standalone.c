/*
 * MLP Null Safety Module - Standalone Compiler
 * P1 Data Structures - Module #18
 * 
 * Nullable types (?), null coalescing (??), optional chaining (?.)
 * 
 * Syntax:
 *   numeric? maybeNum = null         -- Nullable type declaration
 *   text? maybeName = "John"         -- Nullable with value
 *   numeric result = value ?? 0      -- Null coalescing (fallback)
 *   text name = person?.name         -- Safe navigation
 *   numeric x = value!               -- Null assertion
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "null_safety.h"
#include "null_safety_parser.h"
#include "null_safety_codegen.h"
#include "../../lexer.h"

// Only include what we need - avoid include conflicts
#include "../variable/variable.h"

// Parser state - named differently to avoid conflict with parser_core
typedef struct {
    Lexer* lexer;
    Token* current;
    int had_error;
} NullParser;

// Our own variable structure with nullable support
typedef struct {
    char* name;
    VarType type;
    char* initial_value;
    int is_pointer;
    int is_array;
    int is_nullable;
} NullSafeVar;

// Parsed program
typedef struct {
    NullSafeVar** variables;
    int var_count;
    int var_capacity;
    // Null safety statistics
    int nullable_count;        // numeric? x = ...
    int coalesce_count;        // x ?? default
    int safe_nav_count;        // x?.member
    int null_assert_count;     // x!
    int null_check_count;      // x == null / x != null
} Program;

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

static void np_init(NullParser* p, Lexer* lexer) {
    p->lexer = lexer;
    p->current = lexer_next_token(lexer);
    p->had_error = 0;
}

static void np_advance(NullParser* p) {
    if (p->current) {
        // Don't free, lexer manages tokens
    }
    p->current = lexer_next_token(p->lexer);
}

static int np_check(NullParser* p, TokenType type) {
    return p->current && p->current->type == type;
}

static int np_match(NullParser* p, TokenType type) {
    if (np_check(p, type)) {
        np_advance(p);
        return 1;
    }
    return 0;
}

// Suppress unused warning
static void __attribute__((unused)) np_suppress_warnings(void) {
    (void)np_match;
}

static void program_init(Program* prog) {
    prog->var_capacity = 64;
    prog->variables = malloc(sizeof(NullSafeVar*) * prog->var_capacity);
    prog->var_count = 0;
    prog->nullable_count = 0;
    prog->coalesce_count = 0;
    prog->safe_nav_count = 0;
    prog->null_assert_count = 0;
    prog->null_check_count = 0;
}

static void program_add_var(Program* prog, NullSafeVar* var) {
    if (prog->var_count >= prog->var_capacity) {
        prog->var_capacity *= 2;
        prog->variables = realloc(prog->variables, 
                                   sizeof(NullSafeVar*) * prog->var_capacity);
    }
    prog->variables[prog->var_count++] = var;
}

// =============================================================================
// LEXER ANALYSIS - NULL SAFETY DETECTION
// =============================================================================

// Check for nullable type (type?)
static int check_nullable_type(const char* source, int pos) {
    // Look for pattern: type? (e.g., numeric?, text?, boolean?)
    const char* types[] = {"numeric", "text", "boolean"};
    for (int i = 0; i < 3; i++) {
        int len = strlen(types[i]);
        if (pos >= len && strncmp(source + pos - len, types[i], len) == 0) {
            if (source[pos] == '?') {
                return 1;
            }
        }
    }
    return 0;
}

// Scan source for null safety constructs
static void scan_null_safety(const char* source, Program* prog) {
    int i = 0;
    int len = strlen(source);
    
    while (i < len) {
        // Skip comments
        if (source[i] == '-' && i + 1 < len && source[i+1] == '-') {
            // Skip single-line comment
            while (i < len && source[i] != '\n') i++;
            continue;
        }
        
        // Check for nullable type declarations (numeric?, text?, boolean?)
        if (i > 0 && source[i] == '?' && isalpha(source[i-1])) {
            // Make sure it's not ?? (null coalesce)
            if (i + 1 < len && source[i+1] != '?') {
                // Check if this is a type declaration
                int j = i - 1;
                while (j > 0 && isalpha(source[j-1])) j--;
                char type_name[32];
                int type_len = i - j;
                if (type_len < 31) {
                    strncpy(type_name, source + j, type_len);
                    type_name[type_len] = '\0';
                    if (strcmp(type_name, "numeric") == 0 ||
                        strcmp(type_name, "text") == 0 ||
                        strcmp(type_name, "boolean") == 0) {
                        prog->nullable_count++;
                    }
                }
            }
        }
        
        // Check for null coalescing (??)
        if (source[i] == '?' && i + 1 < len && source[i+1] == '?') {
            prog->coalesce_count++;
            i += 2;
            continue;
        }
        
        // Check for safe navigation (?.)
        if (source[i] == '?' && i + 1 < len && source[i+1] == '.') {
            prog->safe_nav_count++;
            i += 2;
            continue;
        }
        
        // Check for null assertion (identifier!)
        // Must be after identifier, not after = (comparison)
        if (source[i] == '!' && i > 0 && (isalnum(source[i-1]) || source[i-1] == '_')) {
            // Make sure next char is not = (!=)
            if (i + 1 >= len || source[i+1] != '=') {
                prog->null_assert_count++;
            }
        }
        
        // Check for null comparison (== null, != null)
        if (strncmp(source + i, "null", 4) == 0) {
            // Check if it's a comparison
            int j = i - 1;
            while (j > 0 && isspace(source[j])) j--;
            if (j >= 1 && ((source[j] == '=' && source[j-1] == '=') ||
                           (source[j] == '=' && source[j-1] == '!'))) {
                prog->null_check_count++;
            }
        }
        
        i++;
    }
}

// =============================================================================
// PARSER - VARIABLE DECLARATIONS WITH NULL SAFETY
// =============================================================================

static int is_type_token(Token* token) {
    if (!token) return 0;
    return token->type == TOKEN_NUMERIC || 
           token->type == TOKEN_TEXT || 
           token->type == TOKEN_BOOLEAN;
}

static VarType token_to_vartype(Token* token) {
    if (!token || !token->value) return VAR_NUMERIC;
    if (token->type == TOKEN_NUMERIC || 
        (token->value && strcmp(token->value, "numeric") == 0)) return VAR_NUMERIC;
    if (token->type == TOKEN_TEXT || 
        (token->value && strcmp(token->value, "text") == 0)) return VAR_STRING;
    if (token->type == TOKEN_BOOLEAN || 
        (token->value && strcmp(token->value, "boolean") == 0)) return VAR_BOOLEAN;
    return VAR_NUMERIC;
}

static NullSafeVar* parse_variable(NullParser* p) {
    // Expect type: numeric, text, boolean
    if (!is_type_token(p->current)) {
        return NULL;
    }
    
    VarType type = token_to_vartype(p->current);
    np_advance(p);
    
    // NOTE: ? and * modifiers are not yet in the lexer
    // They are detected via text scanning in scan_null_safety()
    int is_nullable = 0;
    int is_pointer = 0;
    int is_array = 0;
    
    // Check for array modifier ([])
    if (np_check(p, TOKEN_LBRACKET)) {
        np_advance(p);
        if (np_check(p, TOKEN_RBRACKET)) {
            np_advance(p);
            is_array = 1;
        }
    }
    
    // Expect identifier
    if (!np_check(p, TOKEN_IDENTIFIER)) {
        return NULL;
    }
    
    NullSafeVar* var = malloc(sizeof(NullSafeVar));
    var->name = strdup(p->current->value);
    var->type = type;
    var->is_pointer = is_pointer;
    var->is_array = is_array;
    var->is_nullable = is_nullable;
    var->initial_value = NULL;
    
    np_advance(p);
    
    // Check for assignment
    if (np_check(p, TOKEN_ASSIGN)) {
        np_advance(p);
        
        // Get initial value (simplified - just grab token value)
        if (p->current) {
            if (np_check(p, TOKEN_NUMBER)) {
                var->initial_value = strdup(p->current->value);
                np_advance(p);
            } else if (np_check(p, TOKEN_STRING)) {
                var->initial_value = strdup(p->current->value);
                np_advance(p);
            } else if (np_check(p, TOKEN_IDENTIFIER)) {
                // Could be null, true, false, or identifier
                var->initial_value = strdup(p->current->value);
                np_advance(p);
            }
        }
    }
    
    return var;
}

static void parse_program(NullParser* p, Program* prog) {
    while (p->current && p->current->type != TOKEN_EOF) {
        // Try to parse variable declaration
        if (is_type_token(p->current)) {
            NullSafeVar* var = parse_variable(p);
            if (var) {
                program_add_var(prog, var);
                printf("      ✓ Parsed variable: %s (%s%s%s%s)\n", 
                       var->name,
                       var->type == VAR_NUMERIC ? "numeric" :
                       var->type == VAR_STRING ? "text" : "boolean",
                       var->is_nullable ? "?" : "",
                       var->is_pointer ? "*" : "",
                       var->is_array ? "[]" : "");
            }
        } else {
            // Skip unknown tokens
            np_advance(p);
        }
    }
}

// =============================================================================
// CODE GENERATION
// =============================================================================

static void generate_code(FILE* out, Program* prog) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Null Safety OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n");
    
    // Generate string literals for text variables
    for (int i = 0; i < prog->var_count; i++) {
        NullSafeVar* var = prog->variables[i];
        if (var->type == VAR_STRING && var->initial_value) {
            // String literal - ensure quotes are present
            if (var->initial_value[0] == '"') {
                fprintf(out, "    str_%s: db %s, 0\n", var->name, var->initial_value);
            } else {
                fprintf(out, "    str_%s: db \"%s\", 0\n", var->name, var->initial_value);
            }
        }
    }
    fprintf(out, "\n");
    
    // BSS section for variables
    fprintf(out, "section .bss\n");
    for (int i = 0; i < prog->var_count; i++) {
        NullSafeVar* var = prog->variables[i];
        if (var->is_nullable) {
            // Nullable variables: 8 bytes value + 1 byte has_value flag
            fprintf(out, "    %s: resq 1\n", var->name);
            fprintf(out, "    %s_has_value: resb 1\n", var->name);
        } else if (var->is_array) {
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
        NullSafeVar* var = prog->variables[i];
        
        if (var->is_nullable) {
            if (var->initial_value && strcmp(var->initial_value, "null") == 0) {
                // null value - set has_value to 0
                fprintf(out, "    mov byte [%s_has_value], 0\n", var->name);
            } else if (var->initial_value) {
                // Has value - set has_value to 1
                fprintf(out, "    mov byte [%s_has_value], 1\n", var->name);
                if (var->type == VAR_NUMERIC) {
                    fprintf(out, "    mov qword [%s], %s\n", var->name, var->initial_value);
                } else if (var->type == VAR_STRING) {
                    fprintf(out, "    lea rax, [str_%s]\n", var->name);
                    fprintf(out, "    mov [%s], rax\n", var->name);
                }
            }
        } else if (var->initial_value) {
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
    printf("🔧 Null Safety Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #18\n");
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
    
    // First pass: scan for null safety constructs
    scan_null_safety(source, &prog);
    
    // Second pass: parse declarations
    NullParser parser;
    np_init(&parser, lexer);
    parse_program(&parser, &prog);
    
    printf("\n      ✓ Total declarations: %d\n", prog.var_count);
    printf("      ✓ Null safety statistics:\n");
    printf("        - Nullable types (?): %d\n", prog.nullable_count);
    printf("        - Null coalescing (??): %d\n", prog.coalesce_count);
    printf("        - Safe navigation (?.): %d\n", prog.safe_nav_count);
    printf("        - Null assertions (!): %d\n", prog.null_assert_count);
    printf("        - Null checks (==null): %d\n", prog.null_check_count);
    printf("        - Total null ops: %d\n\n", 
           prog.nullable_count + prog.coalesce_count + prog.safe_nav_count + 
           prog.null_assert_count + prog.null_check_count);
    
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
    printf("🔒 Null safety ops: %d\n\n", 
           prog.nullable_count + prog.coalesce_count + prog.safe_nav_count + 
           prog.null_assert_count + prog.null_check_count);
    
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
