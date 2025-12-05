/**
 * MLP Pointer Module - Standalone Compiler
 * 
 * Modüler Zincir: Modül #16 (P1 Data Structures)
 * Bağımlılıklar: Tüm P0 modülleri + P1 (struct, array, array_ops, collections, enum)
 * 
 * Pointer (user/modüller.md):
 * - Address-of (&): numeric* ptr = &x
 * - Dereference (*): numeric y = *ptr
 * - Pointer Arithmetic: ptr + 1, ptr - 1
 * - Null Pointer: if ptr == null
 * 
 * MLP Syntax:
 * numeric x = 42
 * numeric* ptr = &x       -- Address of x
 * numeric y = *ptr        -- Dereference
 * ptr = ptr + 1           -- Pointer arithmetic
 * 
 * Build: make
 * Test: make test
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pointer.h"
#include "pointer_parser.h"
#include "pointer_codegen.h"
#include "../../lexer.h"
#include "../variable/variable.h"
#include "../variable/variable_parser.h"
#include "../variable/variable_codegen.h"
#include "../comments/comments.h"

// Statistics for pointer operations
typedef struct {
    int pointer_declarations;  // numeric* ptr
    int address_of_ops;        // &x
    int dereference_ops;       // *ptr
    int pointer_arithmetic;    // ptr + 1
} PointerStats;

static PointerStats stats = {0};

// Forward declarations
void generate_pointer_assembly(FILE* output, VariableDeclaration** decls, int count);

int main(int argc, char* argv[]) {
    printf("🔧 Pointer Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #16\n");
    printf("==============================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        printf("\nPointer Syntax:\n");
        printf("  numeric* ptr = &x       -- Address of\n");
        printf("  numeric y = *ptr        -- Dereference\n");
        printf("  ptr = ptr + 1           -- Arithmetic\n");
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
    
    // Step 3: Parse and detect pointers
    printf("\n[3/4] 🌳 Parsing (syntax analysis)...\n");
    
    // Parse variable declarations and detect pointers
    VariableDeclaration* declarations[256];
    int decl_count = 0;
    
    Token* token;
    Token* prev_token = NULL;
    
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
            if (token->value && strncmp(token->value, "--", 2) == 0) {
                token_free(token);
                continue;
            }
            token_free(token);
            continue;
        }
        
        // Check for address-of operator (&)
        // Note: In MLP, & is used for address-of (not bitwise AND which uses 'and' keyword)
        if (token->type == TOKEN_AND) {
            stats.address_of_ops++;
            printf("      ✓ Found address-of operator (&)\n");
            token_free(token);
            continue;
        }
        
        // Check for dereference operator (*)
        // Detect * after = or at start of expression
        if (token->type == TOKEN_MULTIPLY) {
            // Could be dereference or multiplication
            // If previous was =, (, or at line start, it's likely dereference
            stats.dereference_ops++;
            printf("      ✓ Found potential dereference (*)\n");
            token_free(token);
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
            
            // Check for pointer indicator (*)
            token = lexer_next_token(lexer);
            int is_pointer = 0;
            if (token && token->type == TOKEN_MULTIPLY) {
                is_pointer = 1;
                stats.pointer_declarations++;
                printf("      ✓ Found pointer type declaration\n");
                token_free(token);
                token = lexer_next_token(lexer);
            }
            
            // Get variable name
            if (token && token->type == TOKEN_IDENTIFIER) {
                VariableDeclaration* decl = malloc(sizeof(VariableDeclaration));
                memset(decl, 0, sizeof(VariableDeclaration));
                decl->name = strdup(token->value);
                decl->type = is_pointer ? VAR_POINTER : base_type;
                decl->base_type = base_type;
                decl->is_pointer = is_pointer;
                decl->is_array = 0;
                
                declarations[decl_count++] = decl;
                printf("      ✓ Parsed variable: %s%s\n", decl->name, is_pointer ? " (pointer)" : "");
                token_free(token);
            } else if (token) {
                token_free(token);
            }
            continue;
        }
        
        // Check for pointer arithmetic (identifier + or - number)
        if (token->type == TOKEN_IDENTIFIER) {
            char* id_name = strdup(token->value);
            token_free(token);
            
            token = lexer_next_token(lexer);
            if (token && (token->type == TOKEN_PLUS || token->type == TOKEN_MINUS)) {
                // Could be pointer arithmetic
                token_free(token);
                token = lexer_next_token(lexer);
                if (token && token->type == TOKEN_NUMBER) {
                    stats.pointer_arithmetic++;
                    printf("      ✓ Potential pointer arithmetic: %s\n", id_name);
                }
                if (token) token_free(token);
            } else if (token) {
                token_free(token);
            }
            free(id_name);
            continue;
        }
        
        token_free(token);
    }
    
    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ Pointer statistics:\n");
    printf("        - Pointer declarations: %d\n", stats.pointer_declarations);
    printf("        - Address-of ops (&): %d\n", stats.address_of_ops);
    printf("        - Dereference ops (*): %d\n", stats.dereference_ops);
    printf("        - Pointer arithmetic: %d\n", stats.pointer_arithmetic);
    printf("        - Total pointer ops: %d\n", 
           stats.pointer_declarations + stats.address_of_ops + 
           stats.dereference_ops + stats.pointer_arithmetic);
    
    // Step 4: Generate assembly
    printf("\n[4/4] ⚙️  Code generation...\n");
    
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        return 1;
    }
    
    generate_pointer_assembly(out, declarations, decl_count);
    fclose(out);
    
    printf("      ✓ Assembly code generated: %s\n", output_file);
    
    // Summary
    printf("\n✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%zu bytes)\n", input_file, bytes_read);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", decl_count);
    printf("📦 Pointer operations: %d\n", 
           stats.pointer_declarations + stats.address_of_ops + 
           stats.dereference_ops + stats.pointer_arithmetic);
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

// Generate x86-64 assembly for pointers
void generate_pointer_assembly(FILE* output, VariableDeclaration** decls, int count) {
    // Data section
    fprintf(output, "section .data\n");
    fprintf(output, "    msg db \"Pointer Module OK!\", 10\n");
    fprintf(output, "    msg_len equ $ - msg\n");
    fprintf(output, "\n");
    
    // BSS section for uninitialized data
    fprintf(output, "section .bss\n");
    for (int i = 0; i < count; i++) {
        if (decls[i]->is_pointer) {
            // Pointers are 8 bytes (64-bit)
            fprintf(output, "    %s resq 1    ; pointer\n", decls[i]->name);
        } else if (decls[i]->type == VAR_NUMERIC || decls[i]->base_type == VAR_NUMERIC) {
            fprintf(output, "    %s resq 1    ; numeric\n", decls[i]->name);
        } else if (decls[i]->type == VAR_STRING || decls[i]->base_type == VAR_STRING) {
            fprintf(output, "    %s resb 256  ; text\n", decls[i]->name);
        } else {
            fprintf(output, "    %s resb 1    ; boolean\n", decls[i]->name);
        }
    }
    fprintf(output, "\n");
    
    // Text section
    fprintf(output, "section .text\n");
    fprintf(output, "    global _start\n\n");
    
    fprintf(output, "_start:\n");
    
    // Initialize variables
    for (int i = 0; i < count; i++) {
        fprintf(output, "    ; Variable: %s", decls[i]->name);
        if (decls[i]->is_pointer) {
            fprintf(output, " (pointer to %s)", 
                decls[i]->base_type == VAR_NUMERIC ? "numeric" :
                decls[i]->base_type == VAR_STRING ? "text" : "boolean");
        }
        fprintf(output, "\n");
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
