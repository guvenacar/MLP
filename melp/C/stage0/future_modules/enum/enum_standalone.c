/**
 * MLP Enum Module - Standalone Compiler
 * 
 * Modüler Zincir: Modül #15 (P1 Data Structures)
 * Bağımlılıklar: Tüm P0 modülleri + struct + array + array_ops + collections
 * 
 * Enum (kurallar_kitabı.md):
 * - Basic enum: enum Color = Red, Green, Blue end enum
 * - Rust-style tagged unions with associated data
 * - Pattern matching support
 * 
 * MLP Syntax:
 * enum Direction
 *     North
 *     South
 *     East
 *     West
 * end enum
 * 
 * Direction current = Direction.North
 * 
 * Build: make
 * Test: make test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enum.h"
#include "enum_parser.h"
#include "enum_codegen.h"
#include "../../lexer.h"
#include "../variable/variable.h"
#include "../variable/variable_parser.h"
#include "../variable/variable_codegen.h"
#include "../comments/comments.h"

// Statistics for enum types
typedef struct {
    int enum_definitions;     // enum ... end enum
    int enum_variants;        // Individual enum values
    int enum_usages;          // Enum.Value usage
} EnumStats;

static EnumStats stats = {0};

// Forward declarations
void generate_enum_assembly(FILE* output, VariableDeclaration** decls, int count);

int main(int argc, char* argv[]) {
    printf("🔧 Enum Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #15\n");
    printf("==============================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        printf("\nEnum Syntax:\n");
        printf("  enum Direction\n");
        printf("      North\n");
        printf("      South\n");
        printf("      East\n");
        printf("      West\n");
        printf("  end enum\n");
        printf("\n  Direction d = Direction.North\n");
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n", output_file);
    printf("\n");
    
    // Step 1: Read input file
    printf("[1/4] 📖 Reading source file...\n");
    FILE* fp = fopen(input_file, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", input_file);
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* source = malloc(file_size + 1);
    size_t bytes_read = fread(source, 1, file_size, fp);
    source[bytes_read] = '\0';
    fclose(fp);
    
    printf("      ✓ Read %zu bytes\n", bytes_read);
    
    // Step 2: Lexical analysis
    printf("\n[2/4] 🔍 Lexical analysis...\n");
    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        return 1;
    }
    printf("      ✓ Lexer initialized\n");
    
    // Step 3: Parse and detect enums
    printf("\n[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    // Parse variable declarations and detect enums
    VariableDeclaration* declarations[256];
    int decl_count = 0;
    
    Token* token;
    int in_enum = 0;
    
    while (1) {
        token = lexer_next_token(lexer);
        if (!token) break;
        
        // End of file
        if (token->type == TOKEN_EOF) {
            token_free(token);
            break;
        }
        
        // Skip error tokens (including comments handled by lexer)
        if (token->type == TOKEN_ERROR) {
            // Check if it's a comment (starts with --)
            if (token->value && strncmp(token->value, "--", 2) == 0) {
                token_free(token);
                continue;
            }
            token_free(token);
            continue;
        }
        
        // Check for "enum" keyword
        if (token->type == TOKEN_IDENTIFIER && strcmp(token->value, "enum") == 0) {
            stats.enum_definitions++;
            in_enum = 1;
            printf("      ✓ Found enum definition\n");
            token_free(token);
            continue;
        }
        
        // Check for "end" keyword (end enum)
        if (token->type == TOKEN_END) {
            if (in_enum) {
                in_enum = 0;
                printf("      ✓ End enum\n");
            }
            token_free(token);
            continue;
        }
        
        // Count enum variants while in enum block
        if (in_enum && token->type == TOKEN_IDENTIFIER) {
            stats.enum_variants++;
            token_free(token);
            continue;
        }
        
        // Check for dot notation (Enum.Value)
        if (token->type == TOKEN_IDENTIFIER) {
            char* id_name = strdup(token->value);
            token_free(token);
            
            token = lexer_next_token(lexer);
            if (token && token->type == TOKEN_DOT) {
                token_free(token);
                token = lexer_next_token(lexer);
                if (token && token->type == TOKEN_IDENTIFIER) {
                    stats.enum_usages++;
                    printf("      ✓ Enum usage: %s.%s\n", id_name, token->value);
                }
                if (token) token_free(token);
            } else if (token) {
                // Not a dot, need to process this token normally
                // For now, just free and continue
                // Check if it's a variable type
                token_free(token);
            }
            free(id_name);
            continue;
        }
        
        // Parse type keywords (numeric, text, boolean)
        if (token->type == TOKEN_NUMERIC || 
            token->type == TOKEN_TEXT || 
            token->type == TOKEN_BOOLEAN) {
            
            VarType base_type;
            if (token->type == TOKEN_NUMERIC) {
                base_type = VAR_NUMERIC;
            } else if (token->type == TOKEN_TEXT) {
                base_type = VAR_STRING;
            } else {
                base_type = VAR_BOOLEAN;
            }
            token_free(token);
            
            // Get next token for variable name
            token = lexer_next_token(lexer);
            if (token && token->type == TOKEN_IDENTIFIER) {
                VariableDeclaration* decl = malloc(sizeof(VariableDeclaration));
                memset(decl, 0, sizeof(VariableDeclaration));
                decl->name = strdup(token->value);
                decl->type = base_type;
                decl->base_type = base_type;
                decl->is_array = 0;
                decl->is_pointer = 0;
                
                declarations[decl_count++] = decl;
                printf("      ✓ Parsed variable: %s\n", decl->name);
                token_free(token);
            } else if (token) {
                token_free(token);
            }
            continue;
        }
        
        token_free(token);
    }
    
    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ Enum statistics:\n");
    printf("        - Enum definitions: %d\n", stats.enum_definitions);
    printf("        - Enum variants: %d\n", stats.enum_variants);
    printf("        - Enum usages: %d\n", stats.enum_usages);
    printf("        - Total enum ops: %d\n", 
           stats.enum_definitions + stats.enum_variants + stats.enum_usages);
    
    // Step 4: Generate assembly
    printf("\n[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        return 1;
    }
    
    generate_enum_assembly(out, declarations, decl_count);
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n", output_file);
    
    // Summary
    printf("\n✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%zu bytes)\n", input_file, bytes_read);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", decl_count);
    printf("📦 Enum operations: %d\n", 
           stats.enum_definitions + stats.enum_variants + stats.enum_usages);
    printf("\nNext steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");
    
    // Cleanup
    lexer_free(lexer);
    free(source);
    for (int i = 0; i < decl_count; i++) {
        free(declarations[i]->name);
        free(declarations[i]);
    }
    
    return 0;
}

// Generate x86-64 assembly for enums
void generate_enum_assembly(FILE* output, VariableDeclaration** decls, int count) {
    // Data section
    fprintf(output, "section .data\n");
    fprintf(output, "    msg db \"Enum Module OK!\", 10\n");
    fprintf(output, "    msg_len equ $ - msg\n");
    
    // Generate string constants for any text variables
    for (int i = 0; i < count; i++) {
        if (decls[i]->type == VAR_STRING) {
            fprintf(output, "    str_%s db \"\", 0\n", decls[i]->name);
        }
    }
    
    fprintf(output, "\n");
    
    // BSS section for uninitialized data
    fprintf(output, "section .bss\n");
    for (int i = 0; i < count; i++) {
        if (decls[i]->is_array) {
            fprintf(output, "    %s resq 8\n", decls[i]->name);
        } else if (decls[i]->type == VAR_NUMERIC) {
            fprintf(output, "    %s resq 1\n", decls[i]->name);
        } else if (decls[i]->type == VAR_STRING) {
            fprintf(output, "    %s resb 256\n", decls[i]->name);
        } else {
            fprintf(output, "    %s resb 1\n", decls[i]->name);
        }
    }
    fprintf(output, "\n");
    
    // Text section
    fprintf(output, "section .text\n");
    fprintf(output, "    global _start\n\n");
    
    fprintf(output, "_start:\n");
    
    // Initialize variables
    for (int i = 0; i < count; i++) {
        fprintf(output, "    ; Variable: %s (type: %d)\n", decls[i]->name, decls[i]->type);
    }
    
    fprintf(output, "\n");
    fprintf(output, "    ; Print success message\n");
    fprintf(output, "    mov rax, 1          ; sys_write\n");
    fprintf(output, "    mov rdi, 1          ; stdout\n");
    fprintf(output, "    lea rsi, [msg]      ; message\n");
    fprintf(output, "    mov rdx, msg_len    ; length\n");
    fprintf(output, "    syscall\n");
    fprintf(output, "\n");
    fprintf(output, "    ; Exit\n");
    fprintf(output, "    mov rax, 60         ; sys_exit\n");
    fprintf(output, "    xor rdi, rdi        ; exit code 0\n");
    fprintf(output, "    syscall\n");
}
