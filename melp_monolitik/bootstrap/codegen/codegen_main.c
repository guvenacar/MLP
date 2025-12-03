// ============================================================================
// MELP Modular Codegen - codegen_main.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_generate, codegen_free
// Ana kod üretim fonksiyonu ve temizleme

void codegen_generate(Codegen* gen, AST* ast) {
    codegen_emit_prologue(gen);
    
    // Pre-pass: Collect all string literals (traverse AST)
    // For simplicity, we'll collect them during code generation
    // and emit data section at the end before rewriting
    // Actually, let's use a simpler approach: generate code first,
    // then emit string data at the top
    
    // We'll need to reorganize output. For now, let's use memory buffer approach
    // Store current file position, generate code, then insert strings at top
    
    // Simple workaround: Close file, generate to temp, rewrite with strings
    // Even simpler: Just emit strings before code section
    
    codegen_emit(gen, "");
    
    // First pass: Register struct types, state variables, generate function definitions, operator definitions, and module definitions
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type == STMT_STRUCT_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_STATE_DECL) {
            // Phase 18: Register state variables before functions so they can be used
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_FUNC_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_OPERATOR_DEF) {
            // Phase 16: Generate operator overload functions before _start
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_MODULE_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        }
    }
    
    // Generate main code
    codegen_emit(gen, "");
    codegen_emit(gen, "_start:");
    
    // Get argc and argv from stack BEFORE modifying stack
    // At program start: [rsp] = argc, [rsp+8] = argv[0], [rsp+16] = argv[1], ...
    codegen_emit(gen, "    ; Get argc and argv from stack");
    codegen_emit(gen, "    pop rdi               ; argc (first item on stack)");
    codegen_emit(gen, "    mov rsi, rsp          ; argv (pointer to argv[0])");
    codegen_emit(gen, "    call mlp_get_argv     ; Convert to MLP string array");
    codegen_emit(gen, "");
    
    // Phase 19: Initialize GC
    codegen_emit(gen, "    ; Initialize GC");
    codegen_emit(gen, "    call gc_init");
    codegen_emit(gen, "");
    
    // Now setup stack frame
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    codegen_emit(gen, "");
    
    // Phase 18: Initialize state variables (AFTER stack frame setup)
    if (gen->global_var_count > 0) {
        codegen_emit(gen, "    ; Initialize state variables");
        char buffer[256];
        for (int i = 0; i < gen->global_var_count; i++) {
            if (gen->global_vars[i].init_expr) {
                codegen_generate_expression_value(gen, gen->global_vars[i].init_expr);
                snprintf(buffer, sizeof(buffer), "    mov [state_%s], rax", gen->global_vars[i].name);
                codegen_emit(gen, buffer);
            }
        }
        codegen_emit(gen, "");
    }
    
    // Check if main function exists and call it
    int has_main = 0;
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type == STMT_FUNC_DEF &&
            strcmp(ast->statements[i]->func_def.func_name, "main") == 0) {
            has_main = 1;
            break;
        }
    }
    
    if (has_main) {
        codegen_emit(gen, "    ; Call main function");
        codegen_emit(gen, "    call func_main");
    }
    
    // Second pass: Generate non-function, non-struct, non-module, non-operator, non-state statements
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type != STMT_FUNC_DEF && 
            ast->statements[i]->type != STMT_STRUCT_DEF &&
            ast->statements[i]->type != STMT_MODULE_DEF &&
            ast->statements[i]->type != STMT_OPERATOR_DEF &&
            ast->statements[i]->type != STMT_STATE_DECL) {
            codegen_generate_statement(gen, ast->statements[i]);
        }
    }
    
    codegen_emit_epilogue(gen);
    
    // Phase 13: Generate pending generic function instances
    // These are collected during code generation when generic calls are encountered
    GenericInstance* gi = gen->generic_instances;
    while (gi) {
        if (!gi->generated) {
            gi->generated = 1;
            
            // Find the generic function definition
            GenericFunc* gf = codegen_find_generic_func(gen, gi->original_name);
            if (gf && gf->func_stmt) {
                Statement* func_stmt = gf->func_stmt;
                
                // Create modified param_types based on type arguments
                VarType* original_types = func_stmt->func_def.param_types;
                VarType* instantiated_types = malloc(sizeof(VarType) * func_stmt->func_def.param_count);
                
                for (int i = 0; i < func_stmt->func_def.param_count; i++) {
                    if (func_stmt->func_def.param_is_generic && func_stmt->func_def.param_is_generic[i]) {
                        int idx = func_stmt->func_def.param_generic_index[i];
                        instantiated_types[i] = gi->type_args[idx];
                    } else {
                        instantiated_types[i] = original_types[i];
                    }
                }
                
                // Temporarily replace types for code generation
                func_stmt->func_def.param_types = instantiated_types;
                
                // Save original name and replace with mangled name
                char* original_name = func_stmt->func_def.func_name;
                func_stmt->func_def.func_name = gi->mangled_name;
                
                // Temporarily disable generic flag to actually generate code
                int saved_type_param_count = func_stmt->func_def.type_param_count;
                func_stmt->func_def.type_param_count = 0;
                
                // Generate the function (now in .text section at end)
                codegen_emit(gen, "");
                codegen_emit(gen, "section .text");
                codegen_generate_func_def(gen, func_stmt);
                
                // Restore original values
                func_stmt->func_def.type_param_count = saved_type_param_count;
                func_stmt->func_def.func_name = original_name;
                func_stmt->func_def.param_types = original_types;
                free(instantiated_types);
            }
        }
        gi = gi->next;
    }
    
    // Now emit string data section at the end
    // We need to reorganize - strings should be at top after section .data
    // For now, let's just add them at the end as data section
    if (gen->strings) {
        codegen_emit(gen, "");
        codegen_emit(gen, "section .data");
        StringLiteral* str = gen->strings;
        while (str) {
            char buffer[2048];
            char escaped[1024];
            int j = 0;
            // Escape special characters for NASM
            for (int i = 0; str->value[i] && j < 1020; i++) {
                char c = str->value[i];
                if (c == '\n') {
                    escaped[j++] = '"';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '1';
                    escaped[j++] = '0';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '"';
                } else if (c == '\t') {
                    escaped[j++] = '"';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '9';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '"';
                } else if (c == '\r') {
                    escaped[j++] = '"';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '1';
                    escaped[j++] = '3';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '"';
                } else if (c == '"') {
                    escaped[j++] = '"';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '3';
                    escaped[j++] = '4';
                    escaped[j++] = ',';
                    escaped[j++] = ' ';
                    escaped[j++] = '"';
                } else if (c == '\\') {
                    escaped[j++] = '\\';
                    escaped[j++] = '\\';
                } else {
                    escaped[j++] = c;
                }
            }
            escaped[j] = '\0';
            snprintf(buffer, sizeof(buffer), "str_%d: db \"%s\", 0", str->id, escaped);
            codegen_emit(gen, buffer);
            str = str->next;
        }
    }
    
    // Phase 18: Emit state variables in .bss section
    if (gen->global_var_count > 0) {
        codegen_emit(gen, "");
        codegen_emit(gen, "section .bss");
        codegen_emit(gen, "    ; Global state variables");
        char buffer[256];
        for (int i = 0; i < gen->global_var_count; i++) {
            snprintf(buffer, sizeof(buffer), "state_%s: resq 1", gen->global_vars[i].name);
            codegen_emit(gen, buffer);
        }
    }
}

void codegen_free(Codegen* gen) {
    fclose(gen->out);
    free(gen);
}

