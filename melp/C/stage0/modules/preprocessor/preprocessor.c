#include "preprocessor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Macro param işlemleri

MacroParam* macro_param_create(char* name, bool is_variadic) {
    MacroParam* param = malloc(sizeof(MacroParam));
    if (!param) return NULL;
    
    param->name = name ? strdup(name) : NULL;
    param->is_variadic = is_variadic;
    param->default_value = NULL;
    
    return param;
}

void macro_param_free(MacroParam* param) {
    if (!param) return;
    
    if (param->name) free(param->name);
    if (param->default_value) free(param->default_value);
    free(param);
}

// Macro definition işlemleri

MacroDefinition* macro_definition_create(char* name, MacroType type) {
    MacroDefinition* def = malloc(sizeof(MacroDefinition));
    if (!def) return NULL;
    
    def->name = name ? strdup(name) : NULL;
    def->type = type;
    def->params = NULL;
    def->param_count = 0;
    def->body = NULL;
    def->expansion_count = 0;
    def->is_defined = true;
    def->line_number = 0;
    def->file_name = NULL;
    
    return def;
}

void macro_definition_free(MacroDefinition* def) {
    if (!def) return;
    
    if (def->name) free(def->name);
    if (def->body) free(def->body);
    if (def->file_name) free(def->file_name);
    
    if (def->params) {
        for (size_t i = 0; i < def->param_count; i++) {
            macro_param_free(def->params[i]);
        }
        free(def->params);
    }
    
    free(def);
}

void macro_definition_add_param(MacroDefinition* def, MacroParam* param) {
    if (!def || !param) return;
    
    size_t new_count = def->param_count + 1;
    MacroParam** new_params = realloc(def->params, sizeof(MacroParam*) * new_count);
    if (!new_params) return;
    
    new_params[def->param_count] = param;
    def->params = new_params;
    def->param_count = new_count;
}

void macro_definition_set_body(MacroDefinition* def, char* body) {
    if (!def) return;
    
    if (def->body) free(def->body);
    def->body = body ? strdup(body) : NULL;
}

// Macro invocation işlemleri

MacroInvocation* macro_invocation_create(MacroDefinition* macro) {
    MacroInvocation* inv = malloc(sizeof(MacroInvocation));
    if (!inv) return NULL;
    
    inv->macro = macro;
    inv->arguments = NULL;
    inv->arg_count = 0;
    inv->expanded_text = NULL;
    inv->line_number = 0;
    
    return inv;
}

void macro_invocation_free(MacroInvocation* inv) {
    if (!inv) return;
    
    if (inv->arguments) {
        for (size_t i = 0; i < inv->arg_count; i++) {
            if (inv->arguments[i]) free(inv->arguments[i]);
        }
        free(inv->arguments);
    }
    
    if (inv->expanded_text) free(inv->expanded_text);
    free(inv);
}

void macro_invocation_add_arg(MacroInvocation* inv, char* arg) {
    if (!inv || !arg) return;
    
    size_t new_count = inv->arg_count + 1;
    char** new_args = realloc(inv->arguments, sizeof(char*) * new_count);
    if (!new_args) return;
    
    new_args[inv->arg_count] = strdup(arg);
    inv->arguments = new_args;
    inv->arg_count = new_count;
}

ExpansionResult* macro_invocation_expand(MacroInvocation* inv) {
    ExpansionResult* result = expansion_result_create();
    if (!result || !inv || !inv->macro) return result;
    
    MacroDefinition* macro = inv->macro;
    
    // Parametre sayısı kontrolü
    if (macro->type == MACRO_FUNCTION_LIKE && inv->arg_count != macro->param_count) {
        result->has_error = true;
        result->error_message = strdup("Argument count mismatch");
        return result;
    }
    
    // Macro body'yi kopyala
    if (!macro->body) {
        result->expanded_text = strdup("");
        return result;
    }
    
    char* expanded = strdup(macro->body);
    
    // Parametreleri değiştir
    for (size_t i = 0; i < macro->param_count; i++) {
        MacroParam* param = macro->params[i];
        char* arg = inv->arguments[i];
        
        // TODO: Stage 1'de gerçek token replacement yapılacak
        // Şimdilik basit string replacement
    }
    
    result->expanded_text = expanded;
    result->length = strlen(expanded);
    macro->expansion_count++;
    
    return result;
}

// Directive işlemleri

Directive* directive_create(DirectiveType type) {
    Directive* dir = malloc(sizeof(Directive));
    if (!dir) return NULL;
    
    dir->type = type;
    dir->argument = NULL;
    dir->condition = NULL;
    dir->is_active = true;
    dir->line_number = 0;
    
    return dir;
}

void directive_free(Directive* dir) {
    if (!dir) return;
    
    if (dir->argument) free(dir->argument);
    free(dir);
}

void directive_set_argument(Directive* dir, char* arg) {
    if (!dir) return;
    
    if (dir->argument) free(dir->argument);
    dir->argument = arg ? strdup(arg) : NULL;
}

bool directive_evaluate_condition(Directive* dir, PreprocessorContext* ctx) {
    if (!dir || !ctx) return false;
    
    switch (dir->type) {
        case DIRECTIVE_IFDEF: {
            // Macro tanımlı mı kontrol et
            MacroDefinition* macro = preprocessor_context_find_macro(ctx, dir->argument);
            return macro != NULL && macro->is_defined;
        }
        
        case DIRECTIVE_IFNDEF: {
            // Macro tanımlı değil mi kontrol et
            MacroDefinition* macro = preprocessor_context_find_macro(ctx, dir->argument);
            return macro == NULL || !macro->is_defined;
        }
        
        case DIRECTIVE_IF: {
            // TODO: Stage 1'de expression evaluation yapılacak
            return true;
        }
        
        default:
            return false;
    }
}

// Condition stack işlemleri

ConditionStack* condition_stack_create(void) {
    ConditionStack* stack = malloc(sizeof(ConditionStack));
    if (!stack) return NULL;
    
    stack->conditions = NULL;
    stack->depth = 0;
    stack->capacity = 0;
    stack->is_active = NULL;
    
    return stack;
}

void condition_stack_free(ConditionStack* stack) {
    if (!stack) return;
    
    if (stack->conditions) free(stack->conditions);
    if (stack->is_active) free(stack->is_active);
    free(stack);
}

void condition_stack_push(ConditionStack* stack, Directive* dir) {
    if (!stack || !dir) return;
    
    if (stack->depth >= stack->capacity) {
        size_t new_capacity = stack->capacity == 0 ? 8 : stack->capacity * 2;
        Directive** new_conditions = realloc(stack->conditions, sizeof(Directive*) * new_capacity);
        bool* new_active = realloc(stack->is_active, sizeof(bool) * new_capacity);
        
        if (!new_conditions || !new_active) return;
        
        stack->conditions = new_conditions;
        stack->is_active = new_active;
        stack->capacity = new_capacity;
    }
    
    stack->conditions[stack->depth] = dir;
    stack->is_active[stack->depth] = dir->is_active;
    stack->depth++;
}

Directive* condition_stack_pop(ConditionStack* stack) {
    if (!stack || stack->depth == 0) return NULL;
    
    stack->depth--;
    return stack->conditions[stack->depth];
}

bool condition_stack_is_active(ConditionStack* stack) {
    if (!stack || stack->depth == 0) return true;
    
    // Tüm seviyelerin aktif olması gerekir
    for (size_t i = 0; i < stack->depth; i++) {
        if (!stack->is_active[i]) return false;
    }
    
    return true;
}

// Include guard işlemleri

IncludeGuard* include_guard_create(char* macro_name, char* file_path) {
    IncludeGuard* guard = malloc(sizeof(IncludeGuard));
    if (!guard) return NULL;
    
    guard->macro_name = macro_name ? strdup(macro_name) : NULL;
    guard->file_path = file_path ? strdup(file_path) : NULL;
    guard->is_included = false;
    
    return guard;
}

void include_guard_free(IncludeGuard* guard) {
    if (!guard) return;
    
    if (guard->macro_name) free(guard->macro_name);
    if (guard->file_path) free(guard->file_path);
    free(guard);
}

bool include_guard_check(IncludeGuard* guard) {
    if (!guard) return false;
    
    if (guard->is_included) return false;
    
    guard->is_included = true;
    return true;
}

// Preprocessor context işlemleri

PreprocessorContext* preprocessor_context_create(void) {
    PreprocessorContext* ctx = malloc(sizeof(PreprocessorContext));
    if (!ctx) return NULL;
    
    ctx->macros = NULL;
    ctx->macro_count = 0;
    ctx->macro_capacity = 0;
    ctx->cond_stack = condition_stack_create();
    ctx->guards = NULL;
    ctx->guard_count = 0;
    ctx->include_paths = NULL;
    ctx->include_path_count = 0;
    ctx->current_line = 1;
    ctx->current_file = NULL;
    
    return ctx;
}

void preprocessor_context_free(PreprocessorContext* ctx) {
    if (!ctx) return;
    
    if (ctx->macros) {
        for (size_t i = 0; i < ctx->macro_count; i++) {
            macro_definition_free(ctx->macros[i]);
        }
        free(ctx->macros);
    }
    
    if (ctx->cond_stack) {
        condition_stack_free(ctx->cond_stack);
    }
    
    if (ctx->guards) {
        for (size_t i = 0; i < ctx->guard_count; i++) {
            include_guard_free(ctx->guards[i]);
        }
        free(ctx->guards);
    }
    
    if (ctx->include_paths) {
        for (size_t i = 0; i < ctx->include_path_count; i++) {
            if (ctx->include_paths[i]) free(ctx->include_paths[i]);
        }
        free(ctx->include_paths);
    }
    
    if (ctx->current_file) free(ctx->current_file);
    free(ctx);
}

void preprocessor_context_add_macro(PreprocessorContext* ctx, MacroDefinition* macro) {
    if (!ctx || !macro) return;
    
    if (ctx->macro_count >= ctx->macro_capacity) {
        size_t new_capacity = ctx->macro_capacity == 0 ? 16 : ctx->macro_capacity * 2;
        MacroDefinition** new_macros = realloc(ctx->macros, sizeof(MacroDefinition*) * new_capacity);
        if (!new_macros) return;
        
        ctx->macros = new_macros;
        ctx->macro_capacity = new_capacity;
    }
    
    ctx->macros[ctx->macro_count++] = macro;
}

MacroDefinition* preprocessor_context_find_macro(PreprocessorContext* ctx, char* name) {
    if (!ctx || !name) return NULL;
    
    for (size_t i = 0; i < ctx->macro_count; i++) {
        if (ctx->macros[i] && ctx->macros[i]->name && 
            strcmp(ctx->macros[i]->name, name) == 0) {
            return ctx->macros[i];
        }
    }
    
    return NULL;
}

void preprocessor_context_remove_macro(PreprocessorContext* ctx, char* name) {
    if (!ctx || !name) return;
    
    for (size_t i = 0; i < ctx->macro_count; i++) {
        if (ctx->macros[i] && ctx->macros[i]->name && 
            strcmp(ctx->macros[i]->name, name) == 0) {
            macro_definition_free(ctx->macros[i]);
            
            // Shift remaining macros
            for (size_t j = i; j < ctx->macro_count - 1; j++) {
                ctx->macros[j] = ctx->macros[j + 1];
            }
            ctx->macro_count--;
            return;
        }
    }
}

void preprocessor_context_add_include_path(PreprocessorContext* ctx, char* path) {
    if (!ctx || !path) return;
    
    size_t new_count = ctx->include_path_count + 1;
    char** new_paths = realloc(ctx->include_paths, sizeof(char*) * new_count);
    if (!new_paths) return;
    
    new_paths[ctx->include_path_count] = strdup(path);
    ctx->include_paths = new_paths;
    ctx->include_path_count = new_count;
}

// Token paste işlemleri

TokenPaste* token_paste_create(char* left, char* right) {
    TokenPaste* paste = malloc(sizeof(TokenPaste));
    if (!paste) return NULL;
    
    paste->left_token = left ? strdup(left) : NULL;
    paste->right_token = right ? strdup(right) : NULL;
    paste->result = NULL;
    
    return paste;
}

void token_paste_free(TokenPaste* paste) {
    if (!paste) return;
    
    if (paste->left_token) free(paste->left_token);
    if (paste->right_token) free(paste->right_token);
    if (paste->result) free(paste->result);
    free(paste);
}

char* token_paste_execute(TokenPaste* paste) {
    if (!paste || !paste->left_token || !paste->right_token) return NULL;
    
    // Token paste: a ## b -> ab
    size_t left_len = strlen(paste->left_token);
    size_t right_len = strlen(paste->right_token);
    
    char* result = malloc(left_len + right_len + 1);
    if (!result) return NULL;
    
    strcpy(result, paste->left_token);
    strcat(result, paste->right_token);
    
    paste->result = result;
    return result;
}

// Stringification işlemleri

Stringification* stringification_create(char* token) {
    Stringification* str = malloc(sizeof(Stringification));
    if (!str) return NULL;
    
    str->token = token ? strdup(token) : NULL;
    str->stringified = NULL;
    
    return str;
}

void stringification_free(Stringification* str) {
    if (!str) return;
    
    if (str->token) free(str->token);
    if (str->stringified) free(str->stringified);
    free(str);
}

char* stringification_execute(Stringification* str) {
    if (!str || !str->token) return NULL;
    
    // Stringification: #x -> "x"
    size_t len = strlen(str->token);
    char* result = malloc(len + 3); // +2 for quotes, +1 for null
    if (!result) return NULL;
    
    result[0] = '"';
    strcpy(result + 1, str->token);
    result[len + 1] = '"';
    result[len + 2] = '\0';
    
    str->stringified = result;
    return result;
}

// Expansion işlemleri

ExpansionResult* expansion_result_create(void) {
    ExpansionResult* result = malloc(sizeof(ExpansionResult));
    if (!result) return NULL;
    
    result->expanded_text = NULL;
    result->length = 0;
    result->has_error = false;
    result->error_message = NULL;
    
    return result;
}

void expansion_result_free(ExpansionResult* result) {
    if (!result) return;
    
    if (result->expanded_text) free(result->expanded_text);
    if (result->error_message) free(result->error_message);
    free(result);
}

// Preprocessor ana işlemler

char* preprocess_file(char* file_path, PreprocessorContext* ctx) {
    if (!file_path || !ctx) return NULL;
    
    // TODO: Stage 1'de dosya okuma ve preprocessing yapılacak
    return strdup("");
}

char* preprocess_text(char* text, PreprocessorContext* ctx) {
    if (!text || !ctx) return NULL;
    
    // TODO: Stage 1'de text preprocessing yapılacak
    return strdup(text);
}

bool process_directive(char* line, PreprocessorContext* ctx) {
    if (!line || !ctx) return false;
    
    // TODO: Stage 1'de directive processing yapılacak
    return true;
}

char* expand_macros(char* text, PreprocessorContext* ctx) {
    if (!text || !ctx) return NULL;
    
    // TODO: Stage 1'de macro expansion yapılacak
    return strdup(text);
}

// Built-in macro'lar

void register_builtin_macros(PreprocessorContext* ctx) {
    if (!ctx) return;
    
    // __FILE__
    MacroDefinition* file_macro = macro_definition_create("__FILE__", MACRO_BUILTIN);
    preprocessor_context_add_macro(ctx, file_macro);
    
    // __LINE__
    MacroDefinition* line_macro = macro_definition_create("__LINE__", MACRO_BUILTIN);
    preprocessor_context_add_macro(ctx, line_macro);
    
    // __DATE__
    MacroDefinition* date_macro = macro_definition_create("__DATE__", MACRO_BUILTIN);
    preprocessor_context_add_macro(ctx, date_macro);
    
    // __TIME__
    MacroDefinition* time_macro = macro_definition_create("__TIME__", MACRO_BUILTIN);
    preprocessor_context_add_macro(ctx, time_macro);
}

char* get_builtin_macro_value(char* name, PreprocessorContext* ctx) {
    if (!name || !ctx) return NULL;
    
    if (strcmp(name, "__FILE__") == 0) {
        return ctx->current_file ? strdup(ctx->current_file) : strdup("unknown");
    }
    
    if (strcmp(name, "__LINE__") == 0) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d", ctx->current_line);
        return strdup(buffer);
    }
    
    if (strcmp(name, "__DATE__") == 0) {
        return strdup(__DATE__);
    }
    
    if (strcmp(name, "__TIME__") == 0) {
        return strdup(__TIME__);
    }
    
    return NULL;
}
