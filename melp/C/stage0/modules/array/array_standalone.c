/*
 * Array Module - Standalone Test Compiler
 * 
 * Modüler Zincir: Modül #12 (P1 Veri Yapıları)
 * Bağımlılıklar: Tüm P0 modülleri + struct
 * 
 * MLP Koleksiyon Tipleri:
 * - Array []: Homojen, mutable, stack/heap
 * - List  (): Heterojen, mutable, heap
 * - Tuple <>: Heterojen, immutable, stack
 * 
 * Syntax:
 * - numeric[] arr = [1, 2, 3]    -- Array
 * - list() = (10; 20; 30;)       -- List (semicolon separator)
 * - tuple<> = <10, "ali", true>  -- Tuple (immutable)
 * 
 * Kurallar Kitabı: /user/kurallar_kitabı.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "array.h"
#include "array_parser.h"
#include "array_codegen.h"
#include "../variable/variable.h"
#include "../variable/variable_parser.h"
#include "../variable/variable_codegen.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <source_file.mlp> [output_file.s]\n", program_name);
    printf("\n");
    printf("Array Standalone Test Compiler - Module #12 (P1)\n");
    printf("  Dependencies: ALL P0 modules + struct\n");
    printf("  Features:\n");
    printf("    - Array literals: [1, 2, 3]\n");
    printf("    - Array declarations: numeric[] arr\n");
    printf("    - Index access: arr[0]\n");
    printf("    - Dynamic resize\n");
    printf("    - List/Tuple detection\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s test_array.mlp test_array.s\n", program_name);
}

// Array detection statistics
typedef struct {
    int array_decls;
    int array_literals;
    int list_literals;
    int tuple_literals;
    int index_accesses;
} ArrayStats;

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argc > 2 ? argv[2] : "output.s";

    printf("📦 Array Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #12\n");
    printf("==============================================\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n\n", output_file);

    // ═══════════════════════════════════════════════════════════
    // STAGE 1: FILE READING
    // ═══════════════════════════════════════════════════════════
    printf("[1/4] 📖 Reading source file...\n");

    FILE* file = fopen(input_file, "r");
    if (!file) {
        fprintf(stderr, "❌ Error: Cannot open input file '%s'\n", input_file);
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* source = (char*)malloc(file_size + 1);
    fread(source, 1, file_size, file);
    source[file_size] = '\0';
    fclose(file);

    printf("      ✓ Read %ld bytes\n\n", file_size);

    // ═══════════════════════════════════════════════════════════
    // STAGE 2: LEXER
    // ═══════════════════════════════════════════════════════════
    printf("[2/4] 🔍 Lexical analysis...\n");

    Lexer* lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "❌ Error: Failed to create lexer\n");
        free(source);
        return 1;
    }

    printf("      ✓ Lexer initialized\n\n");

    // ═══════════════════════════════════════════════════════════
    // STAGE 3: PARSER (Variable + Array detection)
    // ═══════════════════════════════════════════════════════════
    printf("[3/4] 🌳 Parsing (syntax analysis)...\n");

    VariableParser* var_parser = variable_parser_create(lexer);
    if (!var_parser) {
        fprintf(stderr, "❌ Error: Failed to create variable parser\n");
        lexer_free(lexer);
        free(source);
        return 1;
    }

    // Variables array
    VariableDeclaration** declarations = NULL;
    int decl_count = 0;
    int decl_capacity = 10;
    declarations = malloc(decl_capacity * sizeof(VariableDeclaration*));

    // Array statistics
    ArrayStats stats = {0};

    // Parse and detect array-related constructs
    int iteration = 0;
    const int MAX_ITERATIONS = 1000;

    while (var_parser->current_token && 
           var_parser->current_token->type != TOKEN_EOF &&
           iteration++ < MAX_ITERATIONS) {
        
        Token* tok = var_parser->current_token;

        // Array literal detection: [
        if (tok->type == TOKEN_LBRACKET) {
            stats.array_literals++;
            printf("      ✓ Found: array literal [...]\n");
            token_free(var_parser->current_token);
            var_parser->current_token = lexer_next_token(lexer);
            continue;
        }

        // List literal detection: ( with ; separator
        if (tok->type == TOKEN_LPAREN) {
            stats.list_literals++;
            printf("      ✓ Found: possible list literal (...)\n");
            token_free(var_parser->current_token);
            var_parser->current_token = lexer_next_token(lexer);
            continue;
        }

        // Tuple literal detection: <
        if (tok->type == TOKEN_LESS) {
            // Could be tuple or comparison
            stats.tuple_literals++;
            printf("      ✓ Found: possible tuple literal <...>\n");
            token_free(var_parser->current_token);
            var_parser->current_token = lexer_next_token(lexer);
            continue;
        }

        // Try variable declaration only if we see a type keyword
        if (tok->type == TOKEN_NUMERIC || tok->type == TOKEN_TEXT || tok->type == TOKEN_BOOLEAN) {
            VariableDeclaration* decl = variable_parse_declaration(var_parser);

            if (decl) {
                // Check if it's an array declaration
                if (decl->is_array) {
                    stats.array_decls++;
                    printf("      ✓ Parsed array: %s[%d]\n", decl->name, decl->array_size);
                } else {
                    printf("      ✓ Parsed variable: %s\n", decl->name);
                }

                // Add to list
                if (decl_count >= decl_capacity) {
                    decl_capacity *= 2;
                    declarations = realloc(declarations, decl_capacity * sizeof(VariableDeclaration*));
                }
                declarations[decl_count++] = decl;
                continue;
            }
        }

        // Skip any other token
        if (var_parser->current_token) {
            token_free(var_parser->current_token);
            var_parser->current_token = lexer_next_token(lexer);
        }
    }

    if (iteration >= MAX_ITERATIONS) {
        printf("      ⚠️  Warning: Reached iteration limit\n");
    }

    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ Array statistics:\n");
    printf("        - Array declarations: %d\n", stats.array_decls);
    printf("        - Array literals: %d\n", stats.array_literals);
    printf("        - List literals: %d\n", stats.list_literals);
    printf("        - Tuple literals: %d\n", stats.tuple_literals);
    printf("        - Index accesses: %d\n\n", stats.index_accesses);

    // ═══════════════════════════════════════════════════════════
    // STAGE 4: CODE GENERATION
    // ═══════════════════════════════════════════════════════════
    printf("[4/4] ⚙️  Code generation...\n");

    FILE* output = fopen(output_file, "w");
    if (!output) {
        fprintf(stderr, "❌ Error: Cannot create output file '%s'\n", output_file);
        variable_parser_free(var_parser);
        lexer_free(lexer);
        free(source);
        free(declarations);
        return 1;
    }

    // Assembly header
    fprintf(output, "; Array Module Test - Generated Assembly\n");
    fprintf(output, "; Source: %s\n", input_file);
    fprintf(output, "; P1 Data Structures - Module #12\n");
    fprintf(output, "; Declarations: %d\n", decl_count);
    fprintf(output, "; Array declarations: %d\n", stats.array_decls);
    fprintf(output, "; Array literals: %d\n\n", stats.array_literals);

    fprintf(output, "section .data\n");
    fprintf(output, "    ; Array module data\n");
    fprintf(output, "    msg_array: db \"Array Module OK!\", 10, 0\n");
    fprintf(output, "    msg_len: equ $ - msg_array\n");
    
    // Generate string constants for array string elements
    int str_idx = 0;
    for (int i = 0; i < decl_count; i++) {
        if (declarations[i]->is_array && declarations[i]->value && declarations[i]->value[0] == '[') {
            // Check if this array contains strings
            char* ptr = declarations[i]->value + 1;
            while (*ptr && *ptr != ']') {
                while (*ptr == ' ' || *ptr == ',') ptr++;
                if (*ptr == '"') {
                    // Found string element
                    char str_value[256];
                    int str_len = 0;
                    ptr++;  // Skip opening "
                    while (*ptr && *ptr != '"' && str_len < 255) {
                        str_value[str_len++] = *ptr++;
                    }
                    str_value[str_len] = '\0';
                    if (*ptr == '"') ptr++;
                    
                    fprintf(output, "    arr_str_%d: db \"%s\", 0\n", str_idx, str_value);
                    str_idx++;
                } else {
                    // Skip non-string elements
                    while (*ptr && *ptr != ',' && *ptr != ']') ptr++;
                }
            }
        }
    }
    fprintf(output, "\n");

    fprintf(output, "section .text\n");
    fprintf(output, "global _start\n\n");

    // Array memory layout comment (from kurallar_kitabı.md)
    fprintf(output, "; MLP Collection Types:\n");
    fprintf(output, "; - Array []: Homogeneous, mutable, stack/heap\n");
    fprintf(output, "; - List  (): Heterogeneous, mutable, heap\n");
    fprintf(output, "; - Tuple <>: Heterogeneous, immutable, stack\n\n");

    // Main entry point
    fprintf(output, "_start:\n");
    fprintf(output, "    ; Program start - P1 Array Module\n");
    fprintf(output, "    push rbp\n");
    fprintf(output, "    mov rbp, rsp\n");
    fprintf(output, "    sub rsp, %d  ; Stack for variables + arrays\n\n", 
            (decl_count + 8) * 8);

    // Print array message
    fprintf(output, "    ; Print array module message\n");
    fprintf(output, "    mov rax, 1\n");
    fprintf(output, "    mov rdi, 1\n");
    fprintf(output, "    lea rsi, [msg_array]\n");
    fprintf(output, "    mov rdx, msg_len\n");
    fprintf(output, "    syscall\n\n");

    // Array initialization code
    fprintf(output, "    ; Array initialization\n");
    int str_count = 0;
    for (int i = 0; i < decl_count; i++) {
        if (declarations[i]->is_array && declarations[i]->value && declarations[i]->value[0] == '[') {
            fprintf(output, "    ; Initialize array: %s = %s\n", 
                    declarations[i]->name, declarations[i]->value);
            fprintf(output, "    lea rbx, [%s]  ; Array base address\n", declarations[i]->name);
            
            // Parse array literal: [10, 20, 30] or ["Ali", "Veli"]
            char* value_copy = strdup(declarations[i]->value);
            char* ptr = value_copy + 1;  // Skip '['
            int index = 0;
            
            while (*ptr && *ptr != ']') {
                // Skip whitespace and commas
                while (*ptr == ' ' || *ptr == ',') ptr++;
                if (*ptr == ']') break;
                
                if (*ptr == '"') {
                    // String element
                    char str_value[256];
                    int str_len = 0;
                    ptr++;  // Skip opening "
                    while (*ptr && *ptr != '"' && str_len < 255) {
                        str_value[str_len++] = *ptr++;
                    }
                    str_value[str_len] = '\0';
                    if (*ptr == '"') ptr++;
                    
                    // Add string to .data section (will be added after main code)
                    fprintf(output, "    ; Array[%d] = \"%s\" (stored at arr_str_%d)\n", 
                            index, str_value, str_count);
                    fprintf(output, "    lea rax, [arr_str_%d]\n", str_count);
                    fprintf(output, "    mov [rbx + %d], rax\n", index * 8);
                    str_count++;
                } else {
                    // Numeric element
                    char num_str[64];
                    int num_len = 0;
                    while (*ptr && *ptr != ',' && *ptr != ']' && *ptr != ' ' && num_len < 63) {
                        num_str[num_len++] = *ptr++;
                    }
                    num_str[num_len] = '\0';
                    
                    if (num_len > 0) {
                        fprintf(output, "    mov qword [rbx + %d], %s  ; Array[%d] = %s\n", 
                                index * 8, num_str, index, num_str);
                    }
                }
                
                index++;
            }
            
            free(value_copy);
            fprintf(output, "    ; Array %s: %d elements initialized\n\n", 
                    declarations[i]->name, index);
        }
    }
    
    fprintf(output, "    ; Array operations:\n");
    fprintf(output, "    ; - Literal: [1, 2, 3] -> allocate, fill\n");
    fprintf(output, "    ; - Index: arr[i] -> base + i * element_size\n");
    fprintf(output, "    ; - Length: arr.length -> stored in header\n");
    fprintf(output, "    ; - Resize: realloc if needed\n\n");

    // Exit
    fprintf(output, "    ; Program exit\n");
    fprintf(output, "    mov rsp, rbp\n");
    fprintf(output, "    pop rbp\n");
    fprintf(output, "    mov rax, 60\n");
    fprintf(output, "    xor rdi, rdi\n");
    fprintf(output, "    syscall\n");

    fprintf(output, "\nsection .bss\n");
    for (int i = 0; i < decl_count; i++) {
        if (declarations[i]->is_array) {
            // Array: reserve space for header + elements
            int size = declarations[i]->array_size > 0 ? declarations[i]->array_size : 10;
            fprintf(output, "    %s: resq %d  ; array[%d]\n", 
                    declarations[i]->name, size + 2, size);
        } else {
            fprintf(output, "    %s: resq 1\n", declarations[i]->name);
        }
    }

    fclose(output);

    printf("      ✓ Assembly code generated: %s\n\n", output_file);

    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", input_file, file_size);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", decl_count);
    printf("📦 Array declarations: %d\n", stats.array_decls);
    printf("📋 Array literals: %d\n", stats.array_literals);
    printf("\n");
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  ld output.o -o program\n");
    printf("  ./program\n");

    // Cleanup
    variable_parser_free(var_parser);
    lexer_free(lexer);
    free(source);
    
    for (int i = 0; i < decl_count; i++) {
        variable_declaration_free(declarations[i]);
    }
    free(declarations);

    return 0;
}
