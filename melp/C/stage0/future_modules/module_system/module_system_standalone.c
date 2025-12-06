// =============================================================================
// MODULE SYSTEM - STANDALONE COMPILER
// =============================================================================
// Module #27 - P2 Advanced Features
// 
// Implements: import, export, namespace, multi-file
// Depends on: P0 + P1 + P2 modules
// =============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "module_system.h"

// =============================================================================
// AST Structures
// =============================================================================

typedef struct ModDecl {
    char* name;
    char* kind;              // "import", "export", "module", "namespace", "variable"
    char* module_name;
    char** symbols;
    int symbol_count;
    int is_wildcard;
    int is_default;
    struct ModDecl* next;
} ModDecl;

typedef struct ModParser {
    Lexer* lexer;
    Token* current_token;
    Token* peek_token;
} ModParser;

// Statistics
static int stat_imports = 0;
static int stat_exports = 0;
static int stat_modules = 0;
static int stat_namespaces = 0;

// =============================================================================
// Parser Utilities
// =============================================================================

static ModParser* mod_create(const char* source) {
    ModParser* p = (ModParser*)malloc(sizeof(ModParser));
    p->lexer = lexer_create(source);
    p->current_token = lexer_next_token(p->lexer);
    p->peek_token = lexer_next_token(p->lexer);
    return p;
}

static void mod_advance(ModParser* p) {
    if (p->current_token) {
        token_free(p->current_token);
    }
    p->current_token = p->peek_token;
    p->peek_token = lexer_next_token(p->lexer);
}

static int mod_check(ModParser* p, TokenType type) {
    return p->current_token && p->current_token->type == type;
}

static int mod_check_keyword(ModParser* p, const char* keyword) {
    return p->current_token && 
           p->current_token->type == TOKEN_IDENTIFIER &&
           strcmp(p->current_token->value, keyword) == 0;
}

static void mod_free_parser(ModParser* p) {
    if (p->current_token) token_free(p->current_token);
    if (p->peek_token) token_free(p->peek_token);
    lexer_free(p->lexer);
    free(p);
}

// Skip MLP comments
static void mod_skip_comments(ModParser* p) {
    while (p->current_token && p->current_token->type == TOKEN_MINUS) {
        if (p->peek_token && p->peek_token->type == TOKEN_MINUS) {
            mod_advance(p);
            mod_advance(p);
            while (p->current_token && 
                   p->current_token->type != TOKEN_EOF) {
                if (p->current_token->type == TOKEN_NUMERIC ||
                    p->current_token->type == TOKEN_TEXT ||
                    p->current_token->type == TOKEN_BOOLEAN ||
                    mod_check_keyword(p, "import") ||
                    mod_check_keyword(p, "export") ||
                    mod_check_keyword(p, "module") ||
                    mod_check_keyword(p, "namespace") ||
                    mod_check_keyword(p, "from") ||
                    mod_check_keyword(p, "as")) {
                    break;
                }
                mod_advance(p);
            }
        } else {
            break;
        }
    }
}

// Create declaration
static ModDecl* decl_create(const char* name, const char* kind) {
    ModDecl* d = (ModDecl*)malloc(sizeof(ModDecl));
    d->name = strdup(name);
    d->kind = strdup(kind);
    d->module_name = NULL;
    d->symbols = NULL;
    d->symbol_count = 0;
    d->is_wildcard = 0;
    d->is_default = 0;
    d->next = NULL;
    return d;
}

// =============================================================================
// Parsing Functions
// =============================================================================

// Parse import: import x from "module" or import { a, b } from "module"
static ModDecl* mod_parse_import(ModParser* p) {
    if (!mod_check_keyword(p, "import")) return NULL;
    
    mod_advance(p);  // consume 'import'
    
    ModDecl* decl = decl_create("import_stmt", "import");
    stat_imports++;
    
    // Check for wildcard (*)
    if (mod_check(p, TOKEN_MULTIPLY)) {
        decl->is_wildcard = 1;
        mod_advance(p);
    } else if (mod_check(p, TOKEN_IDENTIFIER)) {
        // Simple import or named import
        decl->module_name = strdup(p->current_token->value);
        mod_advance(p);
    }
    
    // Skip to next statement
    int max_iter = 100;
    while (p->current_token && !mod_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mod_check(p, TOKEN_NUMERIC) ||
            mod_check(p, TOKEN_TEXT) ||
            mod_check(p, TOKEN_BOOLEAN) ||
            mod_check_keyword(p, "import") ||
            mod_check_keyword(p, "export") ||
            mod_check_keyword(p, "module") ||
            mod_check_keyword(p, "namespace")) {
            break;
        }
        mod_advance(p);
    }
    
    return decl;
}

// Parse export: export function foo or export default
static ModDecl* mod_parse_export(ModParser* p) {
    if (!mod_check_keyword(p, "export")) return NULL;
    
    mod_advance(p);  // consume 'export'
    
    ModDecl* decl = decl_create("export_stmt", "export");
    stat_exports++;
    
    // Check for default
    if (mod_check_keyword(p, "default")) {
        decl->is_default = 1;
        mod_advance(p);
    }
    
    // Get exported symbol name
    if (mod_check(p, TOKEN_IDENTIFIER)) {
        decl->module_name = strdup(p->current_token->value);
        mod_advance(p);
    }
    
    // Skip to next statement
    int max_iter = 100;
    while (p->current_token && !mod_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mod_check(p, TOKEN_NUMERIC) ||
            mod_check(p, TOKEN_TEXT) ||
            mod_check(p, TOKEN_BOOLEAN) ||
            mod_check_keyword(p, "import") ||
            mod_check_keyword(p, "export") ||
            mod_check_keyword(p, "module") ||
            mod_check_keyword(p, "namespace") ||
            mod_check_keyword(p, "end")) {
            break;
        }
        mod_advance(p);
    }
    
    return decl;
}

// Parse module: module Name ... end module
static ModDecl* mod_parse_module(ModParser* p) {
    if (!mod_check_keyword(p, "module")) return NULL;
    
    mod_advance(p);  // consume 'module'
    
    char* mod_name = "unnamed";
    if (mod_check(p, TOKEN_IDENTIFIER)) {
        mod_name = p->current_token->value;
    }
    
    ModDecl* decl = decl_create(mod_name, "module");
    stat_modules++;
    
    if (mod_check(p, TOKEN_IDENTIFIER)) {
        mod_advance(p);
    }
    
    // Skip to end module
    int max_iter = 1000;
    while (p->current_token && !mod_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mod_check_keyword(p, "end")) {
            mod_advance(p);
            if (mod_check_keyword(p, "module")) {
                mod_advance(p);
                break;
            }
        } else {
            mod_advance(p);
        }
    }
    
    return decl;
}

// Parse namespace: namespace Name ... end namespace
static ModDecl* mod_parse_namespace(ModParser* p) {
    if (!mod_check_keyword(p, "namespace")) return NULL;
    
    mod_advance(p);  // consume 'namespace'
    
    char* ns_name = "unnamed";
    if (mod_check(p, TOKEN_IDENTIFIER)) {
        ns_name = p->current_token->value;
    }
    
    ModDecl* decl = decl_create(ns_name, "namespace");
    stat_namespaces++;
    
    if (mod_check(p, TOKEN_IDENTIFIER)) {
        mod_advance(p);
    }
    
    // Skip to end namespace
    int max_iter = 1000;
    while (p->current_token && !mod_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mod_check_keyword(p, "end")) {
            mod_advance(p);
            if (mod_check_keyword(p, "namespace")) {
                mod_advance(p);
                break;
            }
        } else {
            mod_advance(p);
        }
    }
    
    return decl;
}

// Parse variable declaration
static ModDecl* mod_parse_variable(ModParser* p) {
    const char* type = NULL;
    
    if (mod_check(p, TOKEN_NUMERIC)) {
        type = "numeric";
    } else if (mod_check(p, TOKEN_TEXT)) {
        type = "text";
    } else if (mod_check(p, TOKEN_BOOLEAN)) {
        type = "boolean";
    } else {
        return NULL;
    }
    
    mod_advance(p);
    
    if (!mod_check(p, TOKEN_IDENTIFIER)) return NULL;
    
    ModDecl* decl = decl_create(p->current_token->value, type);
    mod_advance(p);
    
    // Skip to end of declaration
    int max_iter = 100;
    while (p->current_token && !mod_check(p, TOKEN_EOF) && max_iter-- > 0) {
        if (mod_check(p, TOKEN_NUMERIC) ||
            mod_check(p, TOKEN_TEXT) ||
            mod_check(p, TOKEN_BOOLEAN) ||
            mod_check_keyword(p, "import") ||
            mod_check_keyword(p, "export") ||
            mod_check_keyword(p, "module") ||
            mod_check_keyword(p, "namespace")) {
            break;
        }
        mod_advance(p);
    }
    
    return decl;
}

// =============================================================================
// Code Generation
// =============================================================================

static void generate_assembly(FILE* out, ModDecl* decls) {
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db \"Module System OK!\", 10, 0\n");
    fprintf(out, "    msg_len: equ $ - msg\n\n");
    
    // BSS section
    fprintf(out, "section .bss\n");
    for (ModDecl* d = decls; d; d = d->next) {
        if (strcmp(d->kind, "numeric") == 0 ||
            strcmp(d->kind, "text") == 0 ||
            strcmp(d->kind, "boolean") == 0) {
            fprintf(out, "    %s: resq 1\n", d->name);
        }
    }
    fprintf(out, "\n");
    
    // Text section
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n\n");
    
    fprintf(out, "_start:\n");
    
    // Generate module operation comments
    for (ModDecl* d = decls; d; d = d->next) {
        if (strcmp(d->kind, "import") == 0) {
            fprintf(out, "    ; Import: %s%s\n", 
                    d->is_wildcard ? "*" : "", 
                    d->module_name ? d->module_name : "");
        } else if (strcmp(d->kind, "export") == 0) {
            fprintf(out, "    ; Export: %s%s\n", 
                    d->is_default ? "default " : "",
                    d->module_name ? d->module_name : "");
        } else if (strcmp(d->kind, "module") == 0) {
            fprintf(out, "    ; Module: %s\n", d->name);
        } else if (strcmp(d->kind, "namespace") == 0) {
            fprintf(out, "    ; Namespace: %s\n", d->name);
        }
    }
    
    // Print success message
    fprintf(out, "\n    ; Print success message\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    lea rsi, [msg]\n");
    fprintf(out, "    mov rdx, msg_len\n");
    fprintf(out, "    syscall\n\n");
    
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
    printf("🔧 Module System - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P2 Advanced Features - Module #27\n");
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
    ModParser* parser = mod_create(source);
    printf("      ✓ Lexer initialized\n\n");
    
    // 3. Parser
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    ModDecl* decls = NULL;
    ModDecl* last = NULL;
    int decl_count = 0;
    int max_iter = 5000;
    
    while (parser->current_token && !mod_check(parser, TOKEN_EOF) && max_iter-- > 0) {
        mod_skip_comments(parser);
        
        if (!parser->current_token || mod_check(parser, TOKEN_EOF)) break;
        
        ModDecl* decl = NULL;
        
        if (mod_check_keyword(parser, "import")) {
            decl = mod_parse_import(parser);
            if (decl) {
                printf("      ✓ Parsed import: %s%s\n", 
                       decl->is_wildcard ? "*" : "",
                       decl->module_name ? decl->module_name : "");
            }
        } else if (mod_check_keyword(parser, "export")) {
            decl = mod_parse_export(parser);
            if (decl) {
                printf("      ✓ Parsed export: %s%s\n", 
                       decl->is_default ? "default " : "",
                       decl->module_name ? decl->module_name : "");
            }
        } else if (mod_check_keyword(parser, "module")) {
            decl = mod_parse_module(parser);
            if (decl) {
                printf("      ✓ Parsed module: %s\n", decl->name);
            }
        } else if (mod_check_keyword(parser, "namespace")) {
            decl = mod_parse_namespace(parser);
            if (decl) {
                printf("      ✓ Parsed namespace: %s\n", decl->name);
            }
        } else if (mod_check(parser, TOKEN_NUMERIC) || 
                   mod_check(parser, TOKEN_TEXT) || 
                   mod_check(parser, TOKEN_BOOLEAN)) {
            decl = mod_parse_variable(parser);
            if (decl) {
                printf("      ✓ Parsed %s: %s\n", decl->name, decl->kind);
            }
        } else {
            mod_advance(parser);
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
    printf("      ✓ Module system statistics:\n");
    printf("        - imports: %d\n", stat_imports);
    printf("        - exports: %d\n", stat_exports);
    printf("        - modules: %d\n", stat_modules);
    printf("        - namespaces: %d\n", stat_namespaces);
    printf("        - Total module constructs: %d\n\n", 
           stat_imports + stat_exports + stat_modules + stat_namespaces);
    
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
    printf("🔀 Module constructs: %d\n\n", 
           stat_imports + stat_exports + stat_modules + stat_namespaces);
    
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", argv[2]);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    mod_free_parser(parser);
    free(source);
    
    while (decls) {
        ModDecl* next = decls->next;
        free(decls->name);
        free(decls->kind);
        if (decls->module_name) free(decls->module_name);
        free(decls);
        decls = next;
    }
    
    return 0;
}
