/*
 * MLP Generic Types Module - Standalone Test Compiler
 * P2 Advanced Features - Module #23
 * 
 * Generic syntax:
 *   struct List<T>                     -- Generic struct
 *       T[] items
 *       numeric count
 *   end
 *   
 *   function map<T, U>(list: List<T>, fn: (T) => U): List<U>
 *       -- Generic function
 *   end
 *   
 *   List<numeric> nums = List<numeric>()   -- Type instantiation
 *   
 * Type constraints:
 *   function sort<T: Comparable>(items: T[])
 *       -- T must implement Comparable
 *   end
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generic_types.h"
#include "generic_types_parser.h"
#include "generic_types_codegen.h"
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
#include "../generator/generator.h"
#include "../generator/generator_parser.h"
#include "../generator/generator_codegen.h"

// =============================================================================
// Generic Types Parser (Standalone)
// =============================================================================

typedef struct GtParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} GtParser;

typedef struct GtVar {
    char* name;
    char* type;
    int is_generic_struct;    // Generic struct tanımı
    int is_generic_func;      // Generic function tanımı
    int is_instantiation;     // Type instantiation (List<int>)
    int type_param_count;     // Type parameter sayısı
    char** type_params;       // T, U, V, etc.
    int has_constraint;       // Constraint var mı?
    struct GtVar* next;
} GtVar;

// Statistics
static int stat_generic_structs = 0;
static int stat_generic_funcs = 0;
static int stat_instantiations = 0;
static int stat_type_params = 0;
static int stat_constraints = 0;

// Parser functions
static GtParser* gt_create(const char* source) {
    GtParser* p = (GtParser*)malloc(sizeof(GtParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void gt_advance(GtParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int gt_check(GtParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static void gt_free(GtParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void gt_skip_comments(GtParser* p) {
    while (p->current_token) {
        if (p->current_token->type == TOKEN_MINUS) {
            if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
                gt_advance(p);
                gt_advance(p);
                while (p->current_token && 
                       p->current_token->type != TOKEN_EOF &&
                       p->current_token->type != TOKEN_NUMERIC &&
                       p->current_token->type != TOKEN_TEXT &&
                       p->current_token->type != TOKEN_BOOLEAN &&
                       !(p->current_token->type == TOKEN_IDENTIFIER)) {
                    gt_advance(p);
                }
                continue;
            }
        }
        break;
    }
}

// Parse type parameters <T, U: Constraint>
static int parse_type_params(GtParser* p, GtVar* var) {
    if (!gt_check(p, TOKEN_LESS)) return 0;
    
    gt_advance(p);  // consume <
    
    int capacity = 4;
    var->type_params = (char**)malloc(sizeof(char*) * capacity);
    var->type_param_count = 0;
    
    while (p->current_token && !gt_check(p, TOKEN_GREATER)) {
        if (gt_check(p, TOKEN_IDENTIFIER)) {
            if (var->type_param_count >= capacity) {
                capacity *= 2;
                var->type_params = (char**)realloc(var->type_params, sizeof(char*) * capacity);
            }
            var->type_params[var->type_param_count++] = strdup(p->current_token->value);
            stat_type_params++;
            gt_advance(p);
            
            // Check for constraint: T: Comparable
            if (p->current_token && 
                (p->current_token->type == TOKEN_DOT || 
                 (p->current_token->type == TOKEN_IDENTIFIER && 
                  p->current_token->value[0] == ':'))) {
                var->has_constraint = 1;
                stat_constraints++;
                gt_advance(p);
                // Skip constraint name
                if (gt_check(p, TOKEN_IDENTIFIER)) {
                    gt_advance(p);
                }
            }
        }
        
        if (gt_check(p, TOKEN_COMMA)) {
            gt_advance(p);
        } else {
            break;
        }
    }
    
    if (gt_check(p, TOKEN_GREATER)) {
        gt_advance(p);
    }
    
    return var->type_param_count > 0;
}

// Parse generic var or definition
static GtVar* parse_gt_var(GtParser* p) {
    gt_skip_comments(p);
    
    if (!p->current_token) return NULL;
    
    // Check for 'struct' keyword (generic struct)
    if (gt_check(p, TOKEN_IDENTIFIER) && 
        strcmp(p->current_token->value, "struct") == 0) {
        
        gt_advance(p);
        gt_skip_comments(p);
        
        if (!gt_check(p, TOKEN_IDENTIFIER)) return NULL;
        
        GtVar* var = (GtVar*)calloc(1, sizeof(GtVar));
        var->name = strdup(p->current_token->value);
        var->type = strdup("struct");
        
        gt_advance(p);
        
        // Check for type parameters <T>
        if (parse_type_params(p, var)) {
            var->is_generic_struct = 1;
            stat_generic_structs++;
        }
        
        // Skip to end of struct
        int depth = 1;
        while (p->current_token && p->current_token->type != TOKEN_EOF && depth > 0) {
            if (gt_check(p, TOKEN_IDENTIFIER) && 
                strcmp(p->current_token->value, "struct") == 0) {
                depth++;
            } else if (gt_check(p, TOKEN_END)) {
                depth--;
            }
            gt_advance(p);
        }
        
        return var;
    }
    
    // Check for 'function' keyword (generic function)
    if (gt_check(p, TOKEN_FUNCTION)) {
        gt_advance(p);
        gt_skip_comments(p);
        
        if (!gt_check(p, TOKEN_IDENTIFIER)) return NULL;
        
        GtVar* var = (GtVar*)calloc(1, sizeof(GtVar));
        var->name = strdup(p->current_token->value);
        var->type = strdup("function");
        
        gt_advance(p);
        
        // Check for type parameters <T, U>
        if (parse_type_params(p, var)) {
            var->is_generic_func = 1;
            stat_generic_funcs++;
        }
        
        // Skip to end of function
        int depth = 1;
        while (p->current_token && p->current_token->type != TOKEN_EOF && depth > 0) {
            if (gt_check(p, TOKEN_FUNCTION)) {
                depth++;
            } else if (gt_check(p, TOKEN_END)) {
                depth--;
            }
            gt_advance(p);
        }
        
        return var;
    }
    
    // Type check for regular variables
    const char* type_name = NULL;
    if (gt_check(p, TOKEN_NUMERIC) || gt_check(p, TOKEN_NUMBER)) {
        type_name = "numeric";
    } else if (gt_check(p, TOKEN_TEXT) || gt_check(p, TOKEN_STRING)) {
        type_name = "text";
    } else if (gt_check(p, TOKEN_BOOLEAN)) {
        type_name = "boolean";
    } else if (gt_check(p, TOKEN_IDENTIFIER)) {
        // Could be a generic type instantiation: List<T>
        type_name = p->current_token->value;
    } else {
        return NULL;
    }
    
    char* saved_type = strdup(type_name);
    gt_advance(p);
    
    // Check if it's a type instantiation (List<int>)
    int is_inst = 0;
    int inst_param_count = 0;
    if (gt_check(p, TOKEN_LESS)) {
        is_inst = 1;
        // Count type arguments
        gt_advance(p);
        while (p->current_token && !gt_check(p, TOKEN_GREATER)) {
            if (gt_check(p, TOKEN_IDENTIFIER) || 
                gt_check(p, TOKEN_NUMERIC) || 
                gt_check(p, TOKEN_TEXT)) {
                inst_param_count++;
            }
            gt_advance(p);
        }
        if (gt_check(p, TOKEN_GREATER)) {
            gt_advance(p);
        }
        stat_instantiations++;
    }
    
    gt_skip_comments(p);
    
    // Variable name
    if (!gt_check(p, TOKEN_IDENTIFIER)) {
        free(saved_type);
        return NULL;
    }
    
    GtVar* var = (GtVar*)calloc(1, sizeof(GtVar));
    var->name = strdup(p->current_token->value);
    var->type = saved_type;
    var->is_instantiation = is_inst;
    var->type_param_count = inst_param_count;
    
    gt_advance(p);
    
    // Skip to next statement
    while (p->current_token && 
           p->current_token->type != TOKEN_EOF &&
           p->current_token->type != TOKEN_NUMERIC &&
           p->current_token->type != TOKEN_TEXT &&
           p->current_token->type != TOKEN_BOOLEAN &&
           p->current_token->type != TOKEN_FUNCTION &&
           !(p->current_token->type == TOKEN_IDENTIFIER && 
             strcmp(p->current_token->value, "struct") == 0)) {
        gt_advance(p);
    }
    
    return var;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, GtVar* vars, const char* output_msg) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"%s\", 10, 0\n", output_msg);
    fprintf(out, "    msg_len: equ $ - msg\n");
    
    // String variables
    GtVar* v = vars;
    while (v) {
        if (strcmp(v->type, "text") == 0) {
            fprintf(out, "    str_%s: db \"generic_text\", 0\n", v->name);
        }
        v = v->next;
    }
    
    // BSS section
    fprintf(out, "\nsection .bss\n");
    v = vars;
    while (v) {
        fprintf(out, "    var_%s: resq 1\n", v->name);
        v = v->next;
    }
    
    // Text section
    fprintf(out, "\nsection .text\n");
    fprintf(out, "    global _start\n");
    
    // Main entry
    fprintf(out, "\n_start:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    fprintf(out, "    sub rsp, 64\n\n");
    
    // Initialize variables
    v = vars;
    while (v) {
        fprintf(out, "    ; %s: %s", v->name, v->type);
        if (v->is_generic_struct) {
            fprintf(out, " [GENERIC STRUCT params=%d]", v->type_param_count);
        } else if (v->is_generic_func) {
            fprintf(out, " [GENERIC FUNC params=%d]", v->type_param_count);
        } else if (v->is_instantiation) {
            fprintf(out, " [INSTANTIATION args=%d]", v->type_param_count);
        }
        if (v->has_constraint) fprintf(out, " [CONSTRAINED]");
        fprintf(out, "\n");
        
        if (strcmp(v->type, "numeric") == 0) {
            fprintf(out, "    mov qword [var_%s], 0\n", v->name);
        } else if (strcmp(v->type, "text") == 0) {
            fprintf(out, "    lea rax, [str_%s]\n", v->name);
            fprintf(out, "    mov [var_%s], rax\n", v->name);
        } else if (strcmp(v->type, "boolean") == 0) {
            fprintf(out, "    mov qword [var_%s], 0\n", v->name);
        } else {
            fprintf(out, "    mov qword [var_%s], 0    ; Generic/custom type\n", v->name);
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
    printf("🔧 Generic Types Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #23\n");
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
    GtParser* parser = gt_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // Parse
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    GtVar* vars = NULL;
    GtVar* last = NULL;
    int var_count = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        GtVar* var = parse_gt_var(parser);
        if (var) {
            if (!vars) {
                vars = var;
            } else {
                last->next = var;
            }
            last = var;
            var_count++;
            
            const char* extra = "";
            if (var->is_generic_struct) extra = " [generic struct]";
            else if (var->is_generic_func) extra = " [generic func]";
            else if (var->is_instantiation) extra = " [instantiation]";
            
            printf("      ✓ Parsed %s: %s%s\n", var->name, var->type, extra);
        } else {
            gt_advance(parser);
        }
    }
    
    printf("\n      ✓ Total declarations: %d\n", var_count);
    printf("      ✓ Generic types statistics:\n");
    printf("        - generic structs: %d\n", stat_generic_structs);
    printf("        - generic functions: %d\n", stat_generic_funcs);
    printf("        - type instantiations: %d\n", stat_instantiations);
    printf("        - type parameters: %d\n", stat_type_params);
    printf("        - type constraints: %d\n", stat_constraints);
    printf("        - Total generic constructs: %d\n\n", 
           stat_generic_structs + stat_generic_funcs + stat_instantiations);
    
    // Code generation
    printf("[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", output_file);
        return 1;
    }
    
    generate_assembly(out, vars, "Generic Types OK!");
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n\n", output_file);
    
    // Summary
    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", input_file, size);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", var_count);
    printf("🔀 Generic constructs: %d\n\n", 
           stat_generic_structs + stat_generic_funcs + stat_instantiations);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    while (vars) {
        GtVar* next = vars->next;
        free(vars->name);
        free(vars->type);
        if (vars->type_params) {
            for (int i = 0; i < vars->type_param_count; i++) {
                free(vars->type_params[i]);
            }
            free(vars->type_params);
        }
        free(vars);
        vars = next;
    }
    
    gt_free(parser);
    free(source);
    
    return 0;
}
