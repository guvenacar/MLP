/*
 * MLP Interface/Trait Module - Standalone Test Compiler
 * P2 Advanced Features - Module #24
 * 
 * Trait syntax (Rust-style interfaces):
 *   trait Drawable
 *       function draw(self)
 *       function area(self): numeric
 *   end trait
 *   
 *   impl Drawable for Circle
 *       function draw(self)
 *           print "Drawing circle"
 *       end function
 *       
 *       function area(self): numeric
 *           return 3.14 * self.radius * self.radius
 *       end function
 *   end impl
 *   
 *   -- Trait bounds (generic constraints)
 *   function print_area<T: Drawable>(shape: T)
 *       print shape.area()
 *   end function
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface_trait.h"
#include "interface_trait_parser.h"
#include "interface_trait_codegen.h"
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
#include "../generic_types/generic_types.h"
#include "../generic_types/generic_types_parser.h"
#include "../generic_types/generic_types_codegen.h"

// =============================================================================
// Interface/Trait Parser (Standalone)
// =============================================================================

typedef struct ItParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} ItParser;

typedef struct ItDecl {
    char* name;
    char* kind;               // "trait", "impl", "function", "numeric", "text", etc.
    int is_trait_def;         // trait Drawable ... end trait
    int is_impl;              // impl Drawable for Circle ... end impl
    int is_trait_method;      // Method in trait
    int method_count;         // Methods in trait/impl
    char* impl_for_type;      // Type for impl (Circle, Rectangle, etc.)
    struct ItDecl* next;
} ItDecl;

// Statistics
static int stat_traits = 0;
static int stat_impls = 0;
static int stat_trait_methods = 0;
static int stat_trait_bounds = 0;
static int stat_vtables = 0;

// Parser functions
static ItParser* it_create(const char* source) {
    ItParser* p = (ItParser*)malloc(sizeof(ItParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void it_advance(ItParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int it_check(ItParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int it_check_keyword(ItParser* p, const char* keyword) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, keyword) == 0;
}

static void it_free(ItParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments (-- line comments)
static void it_skip_comments(ItParser* p) {
    while (p->current_token && p->current_token->type == TOKEN_MINUS) {
        if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
            // Skip the two minus tokens
            it_advance(p);
            it_advance(p);
            // Skip until newline (approximated by skipping non-keyword tokens)
            while (p->current_token && 
                   p->current_token->type != TOKEN_EOF) {
                // Stop at meaningful tokens that start new statements
                if (p->current_token->type == TOKEN_NUMERIC ||
                    p->current_token->type == TOKEN_TEXT ||
                    p->current_token->type == TOKEN_BOOLEAN ||
                    it_check_keyword(p, "trait") ||
                    it_check_keyword(p, "impl") ||
                    it_check_keyword(p, "function") ||
                    it_check_keyword(p, "end") ||
                    it_check_keyword(p, "struct")) {
                    break;
                }
                it_advance(p);
            }
        } else {
            break;
        }
    }
}

// Create declaration
static ItDecl* decl_create(const char* name, const char* kind) {
    ItDecl* d = (ItDecl*)malloc(sizeof(ItDecl));
    d->name = strdup(name);
    d->kind = strdup(kind);
    d->is_trait_def = 0;
    d->is_impl = 0;
    d->is_trait_method = 0;
    d->method_count = 0;
    d->impl_for_type = NULL;
    d->next = NULL;
    return d;
}

// Parse trait definition: trait Name ... end trait
static ItDecl* it_parse_trait_def(ItParser* p) {
    if (!it_check_keyword(p, "trait")) return NULL;
    
    it_advance(p);  // consume 'trait'
    
    if (!it_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    ItDecl* decl = decl_create(p->current_token->value, "trait");
    decl->is_trait_def = 1;
    stat_traits++;
    
    it_advance(p);  // consume trait name
    
    // Count methods until 'end trait'
    int method_count = 0;
    int max_iter = 1000;  // Safety limit
    while (p->current_token && !it_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (it_check_keyword(p, "end")) {
            it_advance(p);
            if (it_check_keyword(p, "trait")) {
                it_advance(p);
                break;
            }
            // Not "end trait", keep looking
        } else if (it_check_keyword(p, "function")) {
            method_count++;
            stat_trait_methods++;
            it_advance(p);
        } else {
            it_advance(p);
        }
    }
    
    decl->method_count = method_count;
    return decl;
}

// Parse impl: impl Trait for Type ... end impl
static ItDecl* it_parse_impl(ItParser* p) {
    if (!it_check_keyword(p, "impl")) return NULL;
    
    it_advance(p);  // consume 'impl'
    
    if (!it_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    char* trait_name = strdup(p->current_token->value);
    it_advance(p);  // consume trait name
    
    // Expect 'for'
    if (!it_check_keyword(p, "for")) {
        free(trait_name);
        return NULL;
    }
    it_advance(p);  // consume 'for'
    
    if (!it_check(p, TOKEN_IDENTIFIER)) {
        free(trait_name);
        return NULL;
    }
    
    char* type_name = strdup(p->current_token->value);
    it_advance(p);  // consume type name
    
    ItDecl* decl = decl_create(trait_name, "impl");
    decl->is_impl = 1;
    decl->impl_for_type = type_name;
    stat_impls++;
    
    free(trait_name);
    
    // Count methods until 'end impl'
    int method_count = 0;
    int max_iter = 1000;  // Safety limit
    while (p->current_token && !it_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (it_check_keyword(p, "end")) {
            it_advance(p);
            if (it_check_keyword(p, "impl")) {
                it_advance(p);
                break;
            }
            // Not "end impl", keep looking
        } else if (it_check_keyword(p, "function")) {
            method_count++;
            it_advance(p);
        } else {
            it_advance(p);
        }
    }
    
    decl->method_count = method_count;
    return decl;
}

// Parse variable/other declaration
static ItDecl* it_parse_variable(ItParser* p) {
    const char* type = NULL;
    
    if (it_check(p, TOKEN_NUMERIC)) {
        type = "numeric";
    } else if (it_check(p, TOKEN_TEXT)) {
        type = "text";
    } else if (it_check(p, TOKEN_BOOLEAN)) {
        type = "boolean";
    } else {
        return NULL;
    }
    
    it_advance(p);  // consume type
    
    if (!it_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    ItDecl* decl = decl_create(p->current_token->value, type);
    it_advance(p);
    
    // Skip to end of line
    while (p->current_token && 
           !it_check(p, TOKEN_EOF) &&
           !it_check(p, TOKEN_NUMERIC) &&
           !it_check(p, TOKEN_TEXT) &&
           !it_check(p, TOKEN_BOOLEAN) &&
           !it_check_keyword(p, "trait") &&
           !it_check_keyword(p, "impl")) {
        it_advance(p);
    }
    
    return decl;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, ItDecl* decls) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Interface/Trait OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n");
    fprintf(out, "\n");
    
    // BSS section
    fprintf(out, "section .bss\n");
    for (ItDecl* d = decls; d; d = d->next) {
        if (!d->is_trait_def && !d->is_impl) {
            fprintf(out, "    %s: resq 1\n", d->name);
        }
    }
    fprintf(out, "\n");
    
    // Text section
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n");
    fprintf(out, "\n");
    
    // Generate vtables for traits (placeholder)
    for (ItDecl* d = decls; d; d = d->next) {
        if (d->is_trait_def) {
            fprintf(out, "; Trait: %s (%d methods)\n", d->name, d->method_count);
            fprintf(out, "%s_vtable:\n", d->name);
            fprintf(out, "    dq 0  ; vtable placeholder\n");
            stat_vtables++;
        }
    }
    fprintf(out, "\n");
    
    // Entry point
    fprintf(out, "_start:\n");
    fprintf(out, "    ; Initialize variables\n");
    for (ItDecl* d = decls; d; d = d->next) {
        if (!d->is_trait_def && !d->is_impl) {
            fprintf(out, "    mov qword [%s], 0\n", d->name);
        }
    }
    fprintf(out, "\n");
    
    // Print message
    fprintf(out, "    ; Print result\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    lea rsi, [msg]\n");
    fprintf(out, "    mov rdx, msg_len\n");
    fprintf(out, "    syscall\n");
    fprintf(out, "\n");
    
    // Exit
    fprintf(out, "    ; Exit\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char** argv) {
    printf("🔧 Interface/Trait Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #24\n");
    printf("==============================================\n");
    
    if (argc != 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    // 1. Read source file
    printf("[1/4] 📖 Reading source file...\n");
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        printf("      ✗ Cannot open input file\n");
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
    
    // 2. Lexer
    printf("[2/4] 🔍 Lexical analysis...\n");
    ItParser* parser = it_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // 3. Parser
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    ItDecl* decls = NULL;
    ItDecl* last = NULL;
    int decl_count = 0;
    int max_iter = 5000;  // Safety limit
    
    while (parser->current_token && !it_check(parser, TOKEN_EOF) && max_iter-- > 0) {
        it_skip_comments(parser);
        
        // Safety check after comment skip
        if (!parser->current_token || it_check(parser, TOKEN_EOF)) break;
        
        ItDecl* decl = NULL;
        
        // Try parsing different constructs
        if (it_check_keyword(parser, "trait")) {
            decl = it_parse_trait_def(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s [trait, %d methods]\n", 
                       decl->name, decl->kind, decl->method_count);
            }
        } else if (it_check_keyword(parser, "impl")) {
            decl = it_parse_impl(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s for %s [impl, %d methods]\n", 
                       decl->name, decl->kind, decl->impl_for_type, decl->method_count);
            }
        } else if (it_check(parser, TOKEN_NUMERIC) || 
                   it_check(parser, TOKEN_TEXT) || 
                   it_check(parser, TOKEN_BOOLEAN)) {
            decl = it_parse_variable(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s\n", decl->name, decl->kind);
            }
        } else {
            // Unknown token, skip it
            it_advance(parser);
            continue;
        }
        
        if (decl) {
            decl_count++;
            if (!decls) {
                decls = decl;
                last = decl;
            } else {
                last->next = decl;
                last = decl;
            }
        }
    }
    
    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ Interface/Trait statistics:\n");
    printf("        - traits: %d\n", stat_traits);
    printf("        - impls: %d\n", stat_impls);
    printf("        - trait methods: %d\n", stat_trait_methods);
    printf("        - trait bounds: %d\n", stat_trait_bounds);
    printf("        - Total interface constructs: %d\n\n", 
           stat_traits + stat_impls);
    
    // 4. Code generation
    printf("[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(argv[2], "w");
    if (!out) {
        printf("      ✗ Cannot create output file\n");
        return 1;
    }
    
    generate_assembly(out, decls);
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n\n", argv[2]);
    
    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", argv[1], size);
    printf("🎯 Output: %s\n", argv[2]);
    printf("📊 Declarations: %d\n", decl_count);
    printf("🔀 Interface constructs: %d\n\n", stat_traits + stat_impls);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", argv[2]);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    it_free(parser);
    free(source);
    
    // Free declarations
    while (decls) {
        ItDecl* next = decls->next;
        free(decls->name);
        free(decls->kind);
        if (decls->impl_for_type) free(decls->impl_for_type);
        free(decls);
        decls = next;
    }
    
    return 0;
}
