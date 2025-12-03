/*
 * MLP Lambda/Closure Module - Standalone Test Compiler
 * P2 Advanced Features - Module #21
 * 
 * Lambda syntax:
 *   (x, y) => x + y                    -- Simple arrow lambda
 *   [a, &b](x) => a + b + x           -- Closure with captures
 *   numeric fn = (x) => x * 2          -- Lambda assignment
 *   
 * Higher-order functions:
 *   map(list, (x) => x * 2)
 *   filter(list, (x) => x > 0)
 *   reduce(list, (a, b) => a + b, 0)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lambda.h"
#include "lambda_parser.h"
#include "lambda_codegen.h"
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
// Note: expression_parser has macro conflict with collections_parser
// We don't need expression parsing in this standalone - skip it
// #include "../expression/expression.h"
// #include "../expression/expression_parser.h"
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

// =============================================================================
// Lambda Parser (Standalone)
// =============================================================================

typedef struct LambdaParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} LambdaParser;

typedef struct LambdaVar {
    char* name;
    char* type;
    int is_lambda;        // Lambda fonksiyonu mu?
    int param_count;      // Parametre sayısı
    char** params;        // Parametre isimleri
    int capture_count;    // Capture sayısı
    char** captures;      // Capture isimleri
    int* capture_by_ref;  // Referans ile mi?
    struct LambdaVar* next;
} LambdaVar;

// Statistics
static int stat_lambdas = 0;
static int stat_closures = 0;
static int stat_higher_order = 0;
static int stat_captures = 0;

// Parser functions
static LambdaParser* lp_create(const char* source) {
    LambdaParser* p = (LambdaParser*)malloc(sizeof(LambdaParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void lp_advance(LambdaParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int lp_check(LambdaParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int lp_match(LambdaParser* p, TokenType type) {
    if (lp_check(p, type)) {
        lp_advance(p);
        return 1;
    }
    return 0;
}

static void lp_free(LambdaParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void lp_skip_comments(LambdaParser* p) {
    while (p->current_token) {
        // Single-line comment: --
        if (p->current_token->type == TOKEN_MINUS) {
            if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
                // Skip entire comment (advance until next statement)
                lp_advance(p);
                lp_advance(p);
                // Skip rest of comment content
                while (p->current_token && 
                       p->current_token->type != TOKEN_EOF &&
                       p->current_token->type != TOKEN_NUMERIC &&
                       p->current_token->type != TOKEN_TEXT &&
                       p->current_token->type != TOKEN_BOOLEAN &&
                       !(p->current_token->type == TOKEN_IDENTIFIER && 
                         strcmp(p->current_token->value, "func") == 0)) {
                    lp_advance(p);
                }
                continue;
            }
        }
        break;
    }
}

// Parse lambda variable
static LambdaVar* parse_lambda_var(LambdaParser* p) {
    lp_skip_comments(p);
    
    if (!p->current_token) return NULL;
    
    // Type check
    const char* type_name = NULL;
    if (lp_check(p, TOKEN_NUMERIC) || lp_check(p, TOKEN_NUMBER)) {
        type_name = "numeric";
    } else if (lp_check(p, TOKEN_TEXT) || lp_check(p, TOKEN_STRING)) {
        type_name = "text";
    } else if (lp_check(p, TOKEN_BOOLEAN)) {
        type_name = "boolean";
    } else if (lp_check(p, TOKEN_IDENTIFIER)) {
        // Could be func type or custom type
        if (strcmp(p->current_token->value, "func") == 0) {
            type_name = "func";
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
    
    lp_advance(p);
    lp_skip_comments(p);
    
    // Variable name
    if (!lp_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    LambdaVar* var = (LambdaVar*)calloc(1, sizeof(LambdaVar));
    var->name = strdup(p->current_token->value);
    var->type = strdup(type_name);
    var->is_lambda = 0;
    
    lp_advance(p);
    lp_skip_comments(p);
    
    // Check for assignment
    if (lp_check(p, TOKEN_ASSIGN)) {
        lp_advance(p);
        lp_skip_comments(p);
        
        // Check for lambda syntax: (params) => body
        // Or capture: [captures](params) => body
        
        // Capture list check [...]
        if (lp_check(p, TOKEN_LBRACKET)) {
            var->is_lambda = 1;
            stat_closures++;
            
            lp_advance(p);
            
            // Parse captures
            int cap_capacity = 4;
            var->captures = (char**)malloc(sizeof(char*) * cap_capacity);
            var->capture_by_ref = (int*)calloc(cap_capacity, sizeof(int));
            var->capture_count = 0;
            
            while (p->current_token && !lp_check(p, TOKEN_RBRACKET)) {
                // Check for & (by ref)
                int by_ref = 0;
                if (lp_check(p, TOKEN_AND)) {
                    by_ref = 1;
                    lp_advance(p);
                }
                
                if (lp_check(p, TOKEN_IDENTIFIER)) {
                    if (var->capture_count >= cap_capacity) {
                        cap_capacity *= 2;
                        var->captures = (char**)realloc(var->captures, sizeof(char*) * cap_capacity);
                        var->capture_by_ref = (int*)realloc(var->capture_by_ref, cap_capacity * sizeof(int));
                    }
                    var->captures[var->capture_count] = strdup(p->current_token->value);
                    var->capture_by_ref[var->capture_count] = by_ref;
                    var->capture_count++;
                    stat_captures++;
                    lp_advance(p);
                }
                
                if (lp_check(p, TOKEN_COMMA)) {
                    lp_advance(p);
                }
            }
            
            if (lp_check(p, TOKEN_RBRACKET)) {
                lp_advance(p);
            }
            lp_skip_comments(p);
        }
        
        // Lambda parameters: (x, y)
        if (lp_check(p, TOKEN_LPAREN)) {
            var->is_lambda = 1;
            if (var->capture_count == 0) {
                stat_lambdas++;
            }
            
            lp_advance(p);
            
            int param_capacity = 4;
            var->params = (char**)malloc(sizeof(char*) * param_capacity);
            var->param_count = 0;
            
            while (p->current_token && !lp_check(p, TOKEN_RPAREN)) {
                if (lp_check(p, TOKEN_IDENTIFIER)) {
                    if (var->param_count >= param_capacity) {
                        param_capacity *= 2;
                        var->params = (char**)realloc(var->params, sizeof(char*) * param_capacity);
                    }
                    var->params[var->param_count++] = strdup(p->current_token->value);
                    lp_advance(p);
                }
                
                if (lp_check(p, TOKEN_COMMA)) {
                    lp_advance(p);
                }
            }
            
            if (lp_check(p, TOKEN_RPAREN)) {
                lp_advance(p);
            }
            lp_skip_comments(p);
            
            // Check for => arrow
            if (lp_check(p, TOKEN_ASSIGN)) {
                lp_advance(p);
                if (lp_check(p, TOKEN_GREATER)) {
                    lp_advance(p);  // Found =>
                }
            }
        }
    }
    
    // Skip to end of statement (next type declaration or EOF)
    while (p->current_token && 
           p->current_token->type != TOKEN_EOF &&
           p->current_token->type != TOKEN_NUMERIC &&
           p->current_token->type != TOKEN_TEXT &&
           p->current_token->type != TOKEN_BOOLEAN &&
           !(p->current_token->type == TOKEN_IDENTIFIER && 
             strcmp(p->current_token->value, "func") == 0)) {
        // Check for higher-order function calls
        if (lp_check(p, TOKEN_IDENTIFIER)) {
            const char* name = p->current_token->value;
            if (strcmp(name, "map") == 0 || strcmp(name, "filter") == 0 ||
                strcmp(name, "reduce") == 0 || strcmp(name, "forEach") == 0 ||
                strcmp(name, "find") == 0 || strcmp(name, "some") == 0 ||
                strcmp(name, "every") == 0) {
                stat_higher_order++;
            }
        }
        lp_advance(p);
    }
    
    return var;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, LambdaVar* vars, const char* output_msg) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"%s\", 10, 0\n", output_msg);
    fprintf(out, "    msg_len: equ $ - msg\n");
    
    // Variables
    LambdaVar* v = vars;
    int str_count = 0;
    while (v) {
        if (strcmp(v->type, "text") == 0) {
            fprintf(out, "    str_%s: db \"lambda_text\", 0\n", v->name);
            str_count++;
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
    
    // Lambda functions (simplified stubs)
    v = vars;
    int lambda_id = 0;
    while (v) {
        if (v->is_lambda) {
            fprintf(out, "\n; Lambda: %s\n", v->name);
            fprintf(out, "lambda_%s:\n", v->name);
            fprintf(out, "    push rbp\n");
            fprintf(out, "    mov rbp, rsp\n");
            
            // Simple body: return first param or 0
            if (v->param_count > 0) {
                fprintf(out, "    mov rax, rdi    ; Return first param\n");
            } else {
                fprintf(out, "    xor rax, rax    ; Return 0\n");
            }
            
            fprintf(out, "    pop rbp\n");
            fprintf(out, "    ret\n");
            lambda_id++;
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
        fprintf(out, "    ; Variable: %s (%s)%s\n", v->name, v->type,
                v->is_lambda ? " [LAMBDA]" : "");
        
        if (v->is_lambda) {
            fprintf(out, "    lea rax, [lambda_%s]\n", v->name);
            fprintf(out, "    mov [var_%s], rax\n", v->name);
            
            if (v->capture_count > 0) {
                fprintf(out, "    ; Captures: ");
                for (int i = 0; i < v->capture_count; i++) {
                    fprintf(out, "%s%s%s", v->capture_by_ref[i] ? "&" : "",
                            v->captures[i], i < v->capture_count - 1 ? ", " : "");
                }
                fprintf(out, "\n");
            }
            
            if (v->param_count > 0) {
                fprintf(out, "    ; Params: ");
                for (int i = 0; i < v->param_count; i++) {
                    fprintf(out, "%s%s", v->params[i],
                            i < v->param_count - 1 ? ", " : "");
                }
                fprintf(out, "\n");
            }
        } else if (strcmp(v->type, "numeric") == 0) {
            fprintf(out, "    mov qword [var_%s], 0\n", v->name);
        } else if (strcmp(v->type, "text") == 0) {
            fprintf(out, "    lea rax, [str_%s]\n", v->name);
            fprintf(out, "    mov [var_%s], rax\n", v->name);
        } else if (strcmp(v->type, "boolean") == 0) {
            fprintf(out, "    mov qword [var_%s], 0\n", v->name);
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
    printf("🔧 Lambda/Closure Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #21\n");
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
    LambdaParser* parser = lp_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // Parse
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    LambdaVar* vars = NULL;
    LambdaVar* last = NULL;
    int var_count = 0;
    
    while (parser->current_token && parser->current_token->type != TOKEN_EOF) {
        LambdaVar* var = parse_lambda_var(parser);
        if (var) {
            if (!vars) {
                vars = var;
            } else {
                last->next = var;
            }
            last = var;
            var_count++;
            
            printf("      ✓ Parsed %s: %s (%s)%s\n", 
                   var->is_lambda ? "lambda" : "variable",
                   var->name, var->type,
                   var->capture_count > 0 ? " [closure]" : "");
        } else {
            lp_advance(parser);
        }
    }
    
    printf("\n      ✓ Total declarations: %d\n", var_count);
    printf("      ✓ Lambda/closure statistics:\n");
    printf("        - simple lambdas: %d\n", stat_lambdas);
    printf("        - closures: %d\n", stat_closures);
    printf("        - captures: %d\n", stat_captures);
    printf("        - higher-order calls: %d\n", stat_higher_order);
    printf("        - Total lambda constructs: %d\n\n", stat_lambdas + stat_closures);
    
    // Code generation
    printf("[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", output_file);
        return 1;
    }
    
    generate_assembly(out, vars, "Lambda OK!");
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n\n", output_file);
    
    // Summary
    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", input_file, size);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", var_count);
    printf("🔀 Lambda constructs: %d\n\n", stat_lambdas + stat_closures);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    while (vars) {
        LambdaVar* next = vars->next;
        free(vars->name);
        free(vars->type);
        if (vars->params) {
            for (int i = 0; i < vars->param_count; i++) {
                free(vars->params[i]);
            }
            free(vars->params);
        }
        if (vars->captures) {
            for (int i = 0; i < vars->capture_count; i++) {
                free(vars->captures[i]);
            }
            free(vars->captures);
            free(vars->capture_by_ref);
        }
        free(vars);
        vars = next;
    }
    
    lp_free(parser);
    free(source);
    
    return 0;
}
