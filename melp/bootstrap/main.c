// MELP Phase 0 - Bootstrap Compiler
// Main: Tüm pipeline'ı çalıştırır (Multi-file compilation support)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.c"

// Track imported modules to avoid duplicates and detect cycles
typedef struct ImportedModule {
    char* module_name;
    char* file_path;
    struct ImportedModule* next;
} ImportedModule;

typedef struct ImportStack {
    char* module_name;
    struct ImportStack* next;
} ImportStack;

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        exit(1);
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* content = malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';
    
    fclose(file);
    return content;
}

// Check if module already imported
int is_already_imported(ImportedModule* list, const char* module_name) {
    while (list) {
        if (strcmp(list->module_name, module_name) == 0) {
            return 1;
        }
        list = list->next;
    }
    return 0;
}

// Check if module is in current import stack (circular dependency)
int is_in_import_stack(ImportStack* stack, const char* module_name) {
    while (stack) {
        if (strcmp(stack->module_name, module_name) == 0) {
            return 1;
        }
        stack = stack->next;
    }
    return 0;
}

// Convert module name to file path (e.g., "math_module" -> "math_module.mlp")
char* module_name_to_path(const char* module_name) {
    // Check if already has .mlp extension
    if (strstr(module_name, ".mlp") != NULL) {
        char* path = malloc(strlen(module_name) + 1);
        strcpy(path, module_name);
        return path;
    }
    
    char* path = malloc(strlen(module_name) + 5); // +5 for ".mlp\0"
    sprintf(path, "%s.mlp", module_name);
    return path;
}

// Forward declaration
AST* parse_file_with_imports(const char* filename, ImportedModule** imported_list, 
                             ImportStack* import_stack, int* total_imported);

// Merge two ASTs
AST* merge_ast(AST* main_ast, AST* import_ast) {
    if (!import_ast || import_ast->count == 0) {
        return main_ast;
    }
    
    int new_capacity = main_ast->count + import_ast->count;
    main_ast->statements = realloc(main_ast->statements, sizeof(Statement*) * new_capacity);
    
    // Add imported statements to main AST (at the beginning for proper ordering)
    // Move existing statements forward
    for (int i = main_ast->count - 1; i >= 0; i--) {
        main_ast->statements[i + import_ast->count] = main_ast->statements[i];
    }
    
    // Copy imported statements to beginning
    for (int i = 0; i < import_ast->count; i++) {
        main_ast->statements[i] = import_ast->statements[i];
    }
    
    main_ast->count += import_ast->count;
    
    return main_ast;
}

// Parse a file and recursively parse its imports
AST* parse_file_with_imports(const char* filename, ImportedModule** imported_list,
                             ImportStack* import_stack, int* total_imported) {
    // Read and parse the file
    char* source = read_file(filename);
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    AST* ast = parser_parse(parser);
    
    // Look for import statements and recursively process them
    for (int i = 0; i < ast->count; i++) {
        Statement* stmt = ast->statements[i];
        
        if (stmt->type == STMT_IMPORT) {
            const char* module_name = stmt->import_stmt.module_name;
            
            // Check for circular dependency
            if (is_in_import_stack(import_stack, module_name)) {
                fprintf(stderr, "Error: Circular dependency detected!\n");
                fprintf(stderr, "  Import chain: ");
                ImportStack* s = import_stack;
                while (s) {
                    fprintf(stderr, "%s -> ", s->module_name);
                    s = s->next;
                }
                fprintf(stderr, "%s\n", module_name);
                exit(1);
            }
            
            // Skip if already imported
            if (is_already_imported(*imported_list, module_name)) {
                continue;
            }
            
            // Convert module name to file path
            char* import_path = module_name_to_path(module_name);
            
            // Add to imported list
            ImportedModule* new_import = malloc(sizeof(ImportedModule));
            new_import->module_name = malloc(strlen(module_name) + 1);
            strcpy(new_import->module_name, module_name);
            new_import->file_path = import_path;
            new_import->next = *imported_list;
            *imported_list = new_import;
            
            // Add to import stack for cycle detection
            ImportStack* stack_entry = malloc(sizeof(ImportStack));
            stack_entry->module_name = malloc(strlen(module_name) + 1);
            strcpy(stack_entry->module_name, module_name);
            stack_entry->next = import_stack;
            
            // Recursively parse imported file
            (*total_imported)++;
            printf("      Importing: %s\n", import_path);
            AST* import_ast = parse_file_with_imports(import_path, imported_list, 
                                                     stack_entry, total_imported);
            
            // Merge imported AST with current AST
            ast = merge_ast(ast, import_ast);
            
            // Clean up stack entry
            free(stack_entry->module_name);
            free(stack_entry);
            
            // Note: Don't free import_ast completely as its statements are now in main ast
            free(import_ast->statements);
            free(import_ast);
        }
    }
    
    // Cleanup
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    
    return ast;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    const char* input_file = argv[1];
    const char* output_file = argv[2];
    
    printf("MELP Phase 0 Bootstrap Compiler (Multi-file)\n");
    printf("=====================================\n");
    printf("Input:  %s\n", input_file);
    printf("Output: %s\n\n", output_file);
    
    // Parse main file with imports
    printf("[1/3] Lexing & Parsing (with imports)...\n");
    ImportedModule* imported_list = NULL;
    int total_imported = 0;
    AST* ast = parse_file_with_imports(input_file, &imported_list, NULL, &total_imported);
    
    printf("      Found %d declarations", ast->count);
    if (total_imported > 0) {
        printf(" (%d modules imported)", total_imported);
    }
    printf("\n");
    
    // Codegen
    printf("[2/3] Generating assembly...\n");
    Codegen* codegen = codegen_create(output_file);
    codegen_generate(codegen, ast);
    
    printf("\n✓ Compilation successful!\n");
    printf("  Assembly written to: %s\n", output_file);
    if (total_imported > 0) {
        printf("  Imported modules:\n");
        ImportedModule* im = imported_list;
        while (im) {
            printf("    - %s (from %s)\n", im->module_name, im->file_path);
            im = im->next;
        }
    }
    printf("\nTo assemble and link:\n");
    printf("  nasm -f elf64 %s -o output.o\n", output_file);
    printf("  gcc output.o runtime/runtime.o -o program -lc -no-pie\n");
    
    // Cleanup
    codegen_free(codegen);
    ast_free(ast);
    
    // Free imported list
    ImportedModule* im = imported_list;
    while (im) {
        ImportedModule* next = im->next;
        free(im->module_name);
        free(im->file_path);
        free(im);
        im = next;
    }
    
    return 0;
}
