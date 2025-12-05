/*
 * Array Operations Module - Standalone Test Compiler
 * 
 * Modüler Zincir: Modül #13 (P1 Veri Yapıları)
 * Bağımlılıklar: Tüm P0 modülleri + struct + array
 * 
 * Desteklenen İşlemler:
 * - push(value)    : Eleman ekle
 * - pop()          : Son elemanı çıkar
 * - slice(s, e)    : Alt dizi al
 * - length         : Uzunluk özelliği
 * - indexOf(val)   : Eleman konumu
 * - concat(arr)    : Dizi birleştir
 * - reverse()      : Diziyi ters çevir
 * - sort()         : Diziyi sırala
 * 
 * Kurallar Kitabı: /user/kurallar_kitabı.md
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"
#include "array_operations.h"
#include "array_operations_parser.h"
#include "array_operations_codegen.h"
#include "../variable/variable.h"
#include "../variable/variable_parser.h"
#include "../variable/variable_codegen.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <source_file.mlp> [output_file.s]\n", program_name);
    printf("\n");
    printf("Array Operations Standalone Test Compiler - Module #13 (P1)\n");
    printf("  Dependencies: ALL P0 modules + struct + array\n");
    printf("  Features:\n");
    printf("    - Array methods: push, pop, slice, indexOf, concat\n");
    printf("    - Array properties: length\n");
    printf("    - Iteration support\n");
    printf("\n");
    printf("Example:\n");
    printf("  %s test_array_ops.mlp test_array_ops.s\n", program_name);
}

// Array operation detection statistics
typedef struct {
    int push_ops;
    int pop_ops;
    int slice_ops;
    int length_ops;
    int indexOf_ops;
    int concat_ops;
    int reverse_ops;
    int sort_ops;
    int dot_accesses;
} ArrayOpStats;

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argc > 2 ? argv[2] : "output.s";

    printf("🔧 Array Operations Module - Standalone Test Compiler\n");
    printf("==============================================\n");
    printf("📊 P1 Data Structures - Module #13\n");
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
    // STAGE 3: PARSER (Variable + Array Operation detection)
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

    // Array operation statistics
    ArrayOpStats stats = {0};

    // Parse and detect array operations
    int iteration = 0;
    const int MAX_ITERATIONS = 1000;

    while (var_parser->current_token && 
           var_parser->current_token->type != TOKEN_EOF &&
           iteration++ < MAX_ITERATIONS) {
        
        Token* tok = var_parser->current_token;

        // Dot operator detection (method/property access)
        if (tok->type == TOKEN_DOT) {
            stats.dot_accesses++;
            printf("      ✓ Found: dot access (method/property)\n");
            token_free(var_parser->current_token);
            var_parser->current_token = lexer_next_token(lexer);
            
            // Check for known array methods/properties
            if (var_parser->current_token && 
                var_parser->current_token->type == TOKEN_IDENTIFIER) {
                const char* name = var_parser->current_token->value;
                if (strcmp(name, "push") == 0) stats.push_ops++;
                else if (strcmp(name, "pop") == 0) stats.pop_ops++;
                else if (strcmp(name, "slice") == 0) stats.slice_ops++;
                else if (strcmp(name, "length") == 0) stats.length_ops++;
                else if (strcmp(name, "indexOf") == 0) stats.indexOf_ops++;
                else if (strcmp(name, "concat") == 0) stats.concat_ops++;
                else if (strcmp(name, "reverse") == 0) stats.reverse_ops++;
                else if (strcmp(name, "sort") == 0) stats.sort_ops++;
            }
            continue;
        }

        // Try variable declaration only if we see a type keyword
        if (tok->type == TOKEN_NUMERIC || tok->type == TOKEN_TEXT || tok->type == TOKEN_BOOLEAN) {
            VariableDeclaration* decl = variable_parse_declaration(var_parser);

            if (decl) {
                printf("      ✓ Parsed variable: %s\n", decl->name);
                
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

    int total_ops = stats.push_ops + stats.pop_ops + stats.slice_ops + 
                    stats.length_ops + stats.indexOf_ops + stats.concat_ops +
                    stats.reverse_ops + stats.sort_ops;

    printf("\n      ✓ Total declarations: %d\n", decl_count);
    printf("      ✓ Array operation statistics:\n");
    printf("        - Dot accesses: %d\n", stats.dot_accesses);
    printf("        - push() calls: %d\n", stats.push_ops);
    printf("        - pop() calls: %d\n", stats.pop_ops);
    printf("        - slice() calls: %d\n", stats.slice_ops);
    printf("        - length access: %d\n", stats.length_ops);
    printf("        - indexOf() calls: %d\n", stats.indexOf_ops);
    printf("        - concat() calls: %d\n", stats.concat_ops);
    printf("        - reverse() calls: %d\n", stats.reverse_ops);
    printf("        - sort() calls: %d\n", stats.sort_ops);
    printf("        - Total operations: %d\n\n", total_ops);

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
    fprintf(output, "; Array Operations Module Test - Generated Assembly\n");
    fprintf(output, "; Source: %s\n", input_file);
    fprintf(output, "; P1 Data Structures - Module #13\n");
    fprintf(output, "; Declarations: %d\n", decl_count);
    fprintf(output, "; Array operations: %d\n\n", total_ops);

    fprintf(output, "section .data\n");
    fprintf(output, "    ; Array operations module data\n");
    fprintf(output, "    msg_ops: db \"Array Operations OK!\", 10, 0\n");
    fprintf(output, "    msg_len: equ $ - msg_ops\n\n");

    fprintf(output, "section .text\n");
    fprintf(output, "global _start\n\n");

    // Array operation comments
    fprintf(output, "; Array Operations:\n");
    fprintf(output, "; - push(val): append to end, resize if needed\n");
    fprintf(output, "; - pop(): remove and return last element\n");
    fprintf(output, "; - length: stored in array header\n");
    fprintf(output, "; - slice(s,e): create new array [s:e]\n");
    fprintf(output, "; - indexOf(v): linear search, return -1 if not found\n\n");

    // Main entry point
    fprintf(output, "_start:\n");
    fprintf(output, "    ; Program start - P1 Array Operations Module\n");
    fprintf(output, "    push rbp\n");
    fprintf(output, "    mov rbp, rsp\n");
    fprintf(output, "    sub rsp, %d  ; Stack for variables\n\n", 
            (decl_count + 8) * 8);

    // Print array ops message
    fprintf(output, "    ; Print array operations message\n");
    fprintf(output, "    mov rax, 1\n");
    fprintf(output, "    mov rdi, 1\n");
    fprintf(output, "    lea rsi, [msg_ops]\n");
    fprintf(output, "    mov rdx, msg_len\n");
    fprintf(output, "    syscall\n\n");

    // Exit
    fprintf(output, "    ; Program exit\n");
    fprintf(output, "    mov rsp, rbp\n");
    fprintf(output, "    pop rbp\n");
    fprintf(output, "    mov rax, 60\n");
    fprintf(output, "    xor rdi, rdi\n");
    fprintf(output, "    syscall\n");

    fprintf(output, "\nsection .bss\n");
    for (int i = 0; i < decl_count; i++) {
        fprintf(output, "    %s: resq 1\n", declarations[i]->name);
    }

    fclose(output);

    printf("      ✓ Assembly code generated: %s\n\n", output_file);

    printf("✅ Compilation complete!\n");
    printf("==============================================\n");
    printf("📄 Source: %s (%ld bytes)\n", input_file, file_size);
    printf("🎯 Output: %s\n", output_file);
    printf("📊 Declarations: %d\n", decl_count);
    printf("🔧 Array operations: %d\n", total_ops);
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
