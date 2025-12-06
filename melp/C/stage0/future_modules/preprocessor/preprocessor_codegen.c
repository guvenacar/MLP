#include "preprocessor_codegen.h"
#include <stdio.h>
#include <string.h>

// Macro expansion codegen
void codegen_macro_expansion(MacroDefinition* macro, MacroInvocation* inv) {
    if (!macro || !inv) return;
    
    printf("; Macro expansion: %s\n", macro->name ? macro->name : "unknown");
    
    switch (macro->type) {
        case MACRO_OBJECT_LIKE:
            // Object-like macro: #define PI 3.14
            printf("    ; Object-like macro\n");
            if (macro->body) {
                printf("    ; Expanded to: %s\n", macro->body);
            }
            break;
            
        case MACRO_FUNCTION_LIKE:
            // Function-like macro: #define MAX(a,b) ((a)>(b)?(a):(b))
            printf("    ; Function-like macro\n");
            printf("    ; Parameters: %zu\n", macro->param_count);
            
            // Parametreleri genişlet
            for (size_t i = 0; i < macro->param_count; i++) {
                if (i < inv->arg_count) {
                    printf("    ; %s -> %s\n", 
                           macro->params[i]->name, 
                           inv->arguments[i]);
                }
            }
            
            if (macro->body) {
                printf("    ; Body: %s\n", macro->body);
            }
            break;
            
        case MACRO_VARIADIC:
            // Variadic macro: #define LOG(fmt, ...) printf(fmt, __VA_ARGS__)
            printf("    ; Variadic macro\n");
            printf("    ; Fixed params: %zu\n", macro->param_count - 1);
            printf("    ; Variadic args: %zu\n", 
                   inv->arg_count > macro->param_count ? 
                   inv->arg_count - macro->param_count : 0);
            break;
            
        case MACRO_BUILTIN:
            // Built-in macro: __FILE__, __LINE__
            printf("    ; Built-in macro\n");
            codegen_builtin_macro(macro->name, NULL);
            break;
    }
    
    printf("\n");
}

// Conditional section codegen
void codegen_conditional_section(Directive* dir, char* code_block) {
    if (!dir) return;
    
    printf("; Conditional compilation\n");
    
    switch (dir->type) {
        case DIRECTIVE_IF:
            printf("    ; #if directive\n");
            if (dir->argument) {
                printf("    ; Condition: %s\n", dir->argument);
            }
            if (dir->is_active) {
                printf("    ; Section is active\n");
                if (code_block) {
                    printf("%s\n", code_block);
                }
            } else {
                printf("    ; Section is inactive (skipped)\n");
            }
            break;
            
        case DIRECTIVE_IFDEF:
            printf("    ; #ifdef directive\n");
            if (dir->argument) {
                printf("    ; Macro: %s\n", dir->argument);
            }
            if (dir->is_active) {
                printf("    ; Macro is defined\n");
                if (code_block) {
                    printf("%s\n", code_block);
                }
            } else {
                printf("    ; Macro not defined (skipped)\n");
            }
            break;
            
        case DIRECTIVE_IFNDEF:
            printf("    ; #ifndef directive\n");
            if (dir->argument) {
                printf("    ; Macro: %s\n", dir->argument);
            }
            if (dir->is_active) {
                printf("    ; Macro not defined\n");
                if (code_block) {
                    printf("%s\n", code_block);
                }
            } else {
                printf("    ; Macro is defined (skipped)\n");
            }
            break;
            
        case DIRECTIVE_ELSE:
            printf("    ; #else directive\n");
            if (dir->is_active) {
                if (code_block) {
                    printf("%s\n", code_block);
                }
            }
            break;
            
        case DIRECTIVE_ENDIF:
            printf("    ; #endif directive\n");
            break;
            
        default:
            break;
    }
    
    printf("\n");
}

// Include codegen
void codegen_include(Directive* dir, PreprocessorContext* ctx) {
    if (!dir || !ctx) return;
    
    printf("; Include directive\n");
    
    if (dir->argument) {
        printf("    ; File: %s\n", dir->argument);
        
        // Include guard kontrolü
        bool is_system_header = dir->argument[0] == '<';
        
        if (is_system_header) {
            printf("    ; System header (angle brackets)\n");
        } else {
            printf("    ; Local header (quotes)\n");
        }
        
        // Include path'leri göster
        printf("    ; Include paths: %zu\n", ctx->include_path_count);
        for (size_t i = 0; i < ctx->include_path_count; i++) {
            printf("    ;   %s\n", ctx->include_paths[i]);
        }
    }
    
    printf("\n");
}

// Token paste codegen
void codegen_token_paste(TokenPaste* paste) {
    if (!paste) return;
    
    printf("; Token paste (##)\n");
    
    if (paste->left_token && paste->right_token) {
        printf("    ; Left: %s\n", paste->left_token);
        printf("    ; Right: %s\n", paste->right_token);
        
        char* result = token_paste_execute(paste);
        if (result) {
            printf("    ; Result: %s\n", result);
        }
    }
    
    printf("\n");
}

// Stringification codegen
void codegen_stringification(Stringification* str) {
    if (!str) return;
    
    printf("; Stringification (#)\n");
    
    if (str->token) {
        printf("    ; Token: %s\n", str->token);
        
        char* result = stringification_execute(str);
        if (result) {
            printf("    ; Stringified: %s\n", result);
        }
    }
    
    printf("\n");
}

// Built-in macro codegen
void codegen_builtin_macro(char* macro_name, PreprocessorContext* ctx) {
    if (!macro_name) return;
    
    printf("; Built-in macro: %s\n", macro_name);
    
    if (strcmp(macro_name, "__FILE__") == 0) {
        printf("section .data\n");
        if (ctx && ctx->current_file) {
            printf("    __file_name: db \"%s\", 0\n", ctx->current_file);
        } else {
            printf("    __file_name: db \"unknown\", 0\n");
        }
        printf("section .text\n");
        printf("    lea rax, [__file_name]\n");
    }
    else if (strcmp(macro_name, "__LINE__") == 0) {
        if (ctx) {
            printf("    mov rax, %d  ; Current line\n", ctx->current_line);
        } else {
            printf("    mov rax, 0   ; Unknown line\n");
        }
    }
    else if (strcmp(macro_name, "__DATE__") == 0) {
        printf("section .data\n");
        printf("    __date_str: db \"%s\", 0\n", __DATE__);
        printf("section .text\n");
        printf("    lea rax, [__date_str]\n");
    }
    else if (strcmp(macro_name, "__TIME__") == 0) {
        printf("section .data\n");
        printf("    __time_str: db \"%s\", 0\n", __TIME__);
        printf("section .text\n");
        printf("    lea rax, [__time_str]\n");
    }
    else {
        printf("    ; Unknown built-in macro\n");
    }
    
    printf("\n");
}
