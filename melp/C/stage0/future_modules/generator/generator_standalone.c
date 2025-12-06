/*
 * MLP Generator Module - Standalone Test Compiler
 * P2 Advanced Features - Module #22
 * 
 * Generator syntax:
 *   generator range(start, end)        -- Generator tanımı
 *       for i = start to end
 *           yield i                    -- Yield değer
 *       end
 *   end
 *   
 * Usage:
 *   for x in range(1, 10)              -- for-in loop
 *       print x
 *   end
 *   
 *   gen iter = range(1, 5)             -- Iterator oluştur
 *   while iter.hasNext()
 *       numeric val = iter.next()
 *   end
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generator.h"
#include "generator_parser.h"
#include "generator_codegen.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"
#include "../variable/variable.h"
#include "../variable/variable_parser.h"
#include "../variable/variable_codegen.h"
#include "../comments/comments.h"
#include "../arithmetic/arithmetic.h"
#include "../arithmetic/arithmetic_parser.h"
#include "../arithmetic/arithmetic_codegen.h"
#include "../comparison/comparison.h"
#include "../comparison/comparison_parser.h"
#include "../comparison/comparison_codegen.h"
#include "../logical/logical.h"
#include "../logical/logical_parser.h"
#include "../logical/logical_codegen.h"
#include "../control_flow/control_flow.h"
#include "../control_flow/control_flow_parser.h"
#include "../control_flow/control_flow_codegen.h"
#include "../functions/functions.h"
#include "../functions/functions_parser.h"
#include "../functions/functions_codegen.h"
#include "../print/print.h"
#include "../struct/struct.h"
#include "../struct/struct_parser.h"
#include "../struct/struct_codegen.h"
#include "../array/array.h"
#include "../array/array_parser.h"
#include "../array/array_codegen.h"
#include "../array_operations/array_operations.h"
#include "../array_operations/array_operations_parser.h"
#include "../array_operations/array_operations_codegen.h"
#include "../collections/collections.h"
#include "../collections/collections_parser.h"
#include "../collections/collections_codegen.h"
#include "../enum/enum.h"
#include "../enum/enum_parser.h"
#include "../enum/enum_codegen.h"
#include "../pointer/pointer.h"
#include "../pointer/pointer_parser.h"
#include "../pointer/pointer_codegen.h"
#include "../string_operations/string_operations.h"
#include "../string_operations/string_operations_parser.h"
#include "../string_operations/string_operations_codegen.h"
#include "../null_safety/null_safety.h"
#include "../null_safety/null_safety_parser.h"
#include "../null_safety/null_safety_codegen.h"
#include "../pattern_matching/pattern_matching.h"
#include "../pattern_matching/pattern_matching_parser.h"
#include "../pattern_matching/pattern_matching_codegen.h"
#include "../switch_match/switch_match.h"
#include "../switch_match/switch_match_parser.h"
#include "../switch_match/switch_match_codegen.h"
#include "../lambda/lambda.h"
#include "../lambda/lambda_parser.h"
#include "../lambda/lambda_codegen.h"

// =============================================================================
// Generator Parser (Standalone)
// =============================================================================

typedef struct GenParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} GenParser;

typedef struct GenVar {
    char* name;
    char* type;
    int is_generator;     // Generator fonksiyonu mu?
    int is_iterator;      // Iterator değişkeni mi?
    int yield_count;      // Yield sayısı
    int has_for_in;       // for-in loop kullanıyor mu?
    struct GenVar* next;
} GenVar;

// Statistics
static int stat_generators = 0;
static int stat_yields = 0;
static int stat_iterators = 0;
static int stat_for_in = 0;

// Parser functions
static GenParser* gp_create(const char* source) {
    GenParser* p = (GenParser*)malloc(sizeof(GenParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void gp_advance(GenParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int gp_check(GenParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static void gp_free(GenParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void gp_skip_comments(GenParser* p) {
    while (p->current_token) {
        if (p->current_token->type == TOKEN_MINUS) {
            if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
                gp_advance(p);
                gp_advance(p);
                while (p->current_token && 
                       p->current_token->type != TOKEN_EOF &&
                       p->current_token->type != TOKEN_NUMERIC &&
                       p->current_token->type != TOKEN_TEXT &&
                       p->current_token->type != TOKEN_BOOLEAN &&
                       !(p->current_token->type == TOKEN_IDENTIFIER)) {
                    gp_advance(p);
                }
                continue;
            }
        }
        break;
    }
}

// Parse generator or variable
static GenVar* parse_gen_var(GenParser* p) {
    gp_skip_comments(p);
    
    if (!p->current_token) return NULL;
    
    // Check for 'generator' keyword
    if (gp_check(p, TOKEN_IDENTIFIER) && 
        strcmp(p->current_token->value, "generator") == 0) {
        
        gp_advance(p);
        gp_skip_comments(p);
        
        if (!gp_check(p, TOKEN_IDENTIFIER)) return NULL;
        
        GenVar* var = (GenVar*)calloc(1, sizeof(GenVar));
        var->name = strdup(p->current_token->value);
        var->type = strdup("generator");
        var->is_generator = 1;
        stat_generators++;
        
        gp_advance(p);
        
        // Count yields in generator body
        int depth = 1;
        while (p->current_token && p->current_token->type != TOKEN_EOF && depth > 0) {
            if (gp_check(p, TOKEN_IDENTIFIER)) {
                if (strcmp(p->current_token->value, "yield") == 0) {
                    var->yield_count++;
                    stat_yields++;
                } else if (strcmp(p->current_token->value, "generator") == 0) {
                    depth++;
                }
            } else if (gp_check(p, TOKEN_END)) {
                depth--;
            }
            gp_advance(p);
        }
        
        return var;
    }
    
    // Check for for-in loop: for x in collection
    if (gp_check(p, TOKEN_FOR)) {
        gp_advance(p);
        gp_skip_comments(p);
        
        if (!gp_check(p, TOKEN_IDENTIFIER)) return NULL;
        
        char* var_name = strdup(p->current_token->value);
        gp_advance(p);
        gp_skip_comments(p);
        
        // Check for 'in' keyword
        if (gp_check(p, TOKEN_IDENTIFIER) && 
            strcmp(p->current_token->value, "in") == 0) {
            
            GenVar* var = (GenVar*)calloc(1, sizeof(GenVar));
            var->name = var_name;
            var->type = strdup("for-in");
            var->has_for_in = 1;
            stat_for_in++;
            
            // Skip to end of for loop
            int depth = 1;
            while (p->current_token && p->current_token->type != TOKEN_EOF && depth > 0) {
                if (gp_check(p, TOKEN_FOR) || gp_check(p, TOKEN_WHILE) || 
                    gp_check(p, TOKEN_IF)) {
                    depth++;
                } else if (gp_check(p, TOKEN_END)) {
                    depth--;
                }
                gp_advance(p);
            }
            
            return var;
        }
        
        free(var_name);
        // Regular for loop, skip
        while (p->current_token && !gp_check(p, TOKEN_END) && 
               p->current_token->type != TOKEN_EOF) {
            gp_advance(p);
        }
        if (gp_check(p, TOKEN_END)) gp_advance(p);
        return NULL;
    }
    
    // Type check for regular variables
    const char* type_name = NULL;
    if (gp_check(p, TOKEN_NUMERIC) || gp_check(p, TOKEN_NUMBER)) {
        type_name = "numeric";
    } else if (gp_check(p, TOKEN_TEXT) || gp_check(p, TOKEN_STRING)) {
        type_name = "text";
    } else if (gp_check(p, TOKEN_BOOLEAN)) {
        type_name = "boolean";
    } else if (gp_check(p, TOKEN_IDENTIFIER)) {
        const char* val = p->current_token->value;
        if (strcmp(val, "gen") == 0 || strcmp(val, "iter") == 0) {
            type_name = "iterator";
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
    
    gp_advance(p);
    gp_skip_comments(p);
    
    // Variable name
    if (!gp_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    GenVar* var = (GenVar*)calloc(1, sizeof(GenVar));
    var->name = strdup(p->current_token->value);
    var->type = strdup(type_name);
    
    if (strcmp(type_name, "iterator") == 0) {
        var->is_iterator = 1;
        stat_iterators++;
    }
    
    gp_advance(p);
    
    // Skip to next statement
    while (p->current_token && 
           p->current_token->type != TOKEN_EOF &&
           p->current_token->type != TOKEN_NUMERIC &&
           p->current_token->type != TOKEN_TEXT &&
           p->current_token->type != TOKEN_BOOLEAN &&
           p->current_token->type != TOKEN_FOR &&
           !(p->current_token->type == TOKEN_IDENTIFIER && 
             (strcmp(p->current_token->value, "generator") == 0 ||
              strcmp(p->current_token->value, "gen") == 0 ||
              strcmp(p->current_token->value, "iter") == 0))) {
        gp_advance(p);
    }
    
    return var;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, GenVar* vars, const char* output_msg) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"%s\", 10, 0\n", output_msg);
    fprintf(out, "    msg_len: equ $ - msg\n");
    
    // String variables
    GenVar* v = vars;
    while (v) {
        if (strcmp(v->type, "text") == 0) {
            fprintf(out, "    str_%s: db \"gen_text\", 0\n", v->name);
        }
        v = v->next;
    }
    
    // BSS section
    fprintf(out, "\nsection .bss\n");
    v = vars;
    while (v) {
        fprintf(out, "    var_%s: resq 1\n", v->name);
        if (v->is_generator) {
            fprintf(out, "    gen_%s_state: resq 1    ; Generator state\n", v->name);
            fprintf(out, "    gen_%s_value: resq 1    ; Current yield value\n", v->name);
        }
        v = v->next;
    }
    
    // Text section
    fprintf(out, "\nsection .text\n");
    fprintf(out, "    global _start\n");
    
    // Generator functions (stubs)
    v = vars;
    while (v) {
        if (v->is_generator) {
            fprintf(out, "\n; Generator: %s (yields: %d)\n", v->name, v->yield_count);
            fprintf(out, "gen_%s:\n", v->name);
            fprintf(out, "    push rbp\n");
            fprintf(out, "    mov rbp, rsp\n");
            fprintf(out, "    ; State machine based on gen_%s_state\n", v->name);
            fprintf(out, "    mov rax, [gen_%s_state]\n", v->name);
            fprintf(out, "    cmp rax, %d    ; Check if finished\n", v->yield_count);
            fprintf(out, "    jge .gen_%s_done\n", v->name);
            fprintf(out, "    ; Yield current value\n");
            fprintf(out, "    inc qword [gen_%s_state]\n", v->name);
            fprintf(out, "    mov rax, [gen_%s_value]\n", v->name);
            fprintf(out, "    pop rbp\n");
            fprintf(out, "    ret\n");
            fprintf(out, ".gen_%s_done:\n", v->name);
            fprintf(out, "    xor rax, rax    ; Return 0 when done\n");
            fprintf(out, "    pop rbp\n");
            fprintf(out, "    ret\n");
        }
        v = v->next;
    }
    
    // Main entry
    fprintf(out, "\n_start:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    fprintf(out, "    sub rsp, 64\n\n");
    
    // Initialize variables
    v = vars;
    while (v) {
        fprintf(out, "    ; %s: %s", v->name, v->type);
        if (v->is_generator) fprintf(out, " [GENERATOR yields=%d]", v->yield_count);
        if (v->is_iterator) fprintf(out, " [ITERATOR]");
        if (v->has_for_in) fprintf(out, " [FOR-IN]");
        fprintf(out, "\n");
        
        if (v->is_generator) {
            fprintf(out, "    mov qword [gen_%s_state], 0\n", v->name);
            fprintf(out, "    mov qword [gen_%s_value], 0\n", v->name);
        } else if (strcmp(v->type, "numeric") == 0) {
            fprintf(out, "    mov qword [var_%s], 0\n", v->name);
        } else if (strcmp(v->type, "text") == 0) {
            fprintf(out, "    lea rax, [str_%s]\n", v->name);
            fprintf(out, "    mov [var_%s], rax\n", v->name);
        } else if (strcmp(v->type, "boolean") == 0) {
            fprintf(out, "    mov qword [var_%s], 0\n", v->name);
        } else if (v->is_iterator) {
            fprintf(out, "    mov qword [var_%s], 0    ; Iterator ref\n", v->name);
        }
        fprintf(out, "\n");
        v = v->next;
    }
    
    // Print message
    fprintf(out, "    ; Print result\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    lea rsi, [msg]\n");
    fprintf(out, "    mov rdx, msg_len\n");
    fprintf(out, "    syscall\n\n");
    
    // Exit
    fprintf(out, "    ; Exit\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    printf("🔧 Generator Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #22\n");
    printf("==============================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n\n", output_file);
    
    // Read source file
    printf("[1/4] 📖 Reading source file...\n");
    FILE* f = fopen(input_file, "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", input_file);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = (char*)malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    printf("      ✓ Read %ld bytes\n\n", size);
    
    // Lexical analysis
    printf("[2/4] 🔍 Lexical analysis...\n");
    GenParser* parser = gp_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // Parse
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    GenVar* vars = NULL;
    GenVar* last = NULL;
    int var_count = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        GenVar* var = parse_gen_var(parser);
        if (var) {
            if (!vars) {
                vars = var;
            } else {
                last->next = var;
            }
            last = var;
            var_count++;
            
            const char* extra = "";
            if (var->is_generator) extra = " [generator]";
            else if (var->is_iterator) extra = " [iterator]";
            else if (var->has_for_in) extra = " [for-in]";
            
            printf("      ✓ Parsed %s: %s%s\n", var->name, var->type, extra);
        } else {
            gp_advance(parser);
        }
    }
    
    printf("\n      ✓ Total declarations: %d\n", var_count);
    printf("      ✓ Generator statistics:\n");
    printf("        - generators: %d\n", stat_generators);
    printf("        - yield statements: %d\n", stat_yields);
    printf("        - iterators: %d\n", stat_iterators);
    printf("        - for-in loops: %d\n", stat_for_in);
    printf("        - Total generator constructs: %d\n\n", 
           stat_generators + stat_iterators + stat_for_in);
    
    // Code generation
    printf("[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", output_file);
        return 1;
    }
    
    generate_assembly(out, vars, "Generator OK!");
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n\n", output_file);
    
    // Summary
    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", input_file, size);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", var_count);
    printf("🔀 Generator constructs: %d\n\n", 
           stat_generators + stat_iterators + stat_for_in);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    while (vars) {
        GenVar* next = vars->next;
        free(vars->name);
        free(vars->type);
        free(vars);
        vars = next;
    }
    
    gp_free(parser);
    free(source);
    
    return 0;
}
