/**
 * MODÜL #58: CONST_GENERICS - Parser Implementation
 * 
 * Parses MLP const generic syntax:
 * - dim numbers as i32 = 10
 * - type Array<T, N>
 * - Array<i32, 100>
 */

#define _GNU_SOURCE
#include "const_generics.h"
#include <ctype.h>

// ============================================================================
// PARSER STATE
// ============================================================================

typedef struct {
    const char* input;
    int pos;
    int line;
    int col;
    char current_char;
    ConstGenericContext* ctx;
} Parser;

// ============================================================================
// PARSER UTILITIES
// ============================================================================

static void parser_init(Parser* p, const char* input, ConstGenericContext* ctx) {
    p->input = input;
    p->pos = 0;
    p->line = 1;
    p->col = 1;
    p->current_char = input[0];
    p->ctx = ctx;
}

static void parser_advance(Parser* p) {
    if (p->current_char == '\n') {
        p->line++;
        p->col = 1;
    } else {
        p->col++;
    }
    
    p->pos++;
    if (p->input[p->pos] != '\0') {
        p->current_char = p->input[p->pos];
    } else {
        p->current_char = '\0';
    }
}

static void parser_skip_whitespace(Parser* p) {
    while (p->current_char == ' ' || p->current_char == '\t' || 
           p->current_char == '\n' || p->current_char == '\r') {
        parser_advance(p);
    }
}

static void parser_skip_comment(Parser* p) {
    if (p->current_char == '-' && p->input[p->pos + 1] == '-') {
        // MLP-style comment: -- comment
        while (p->current_char != '\n' && p->current_char != '\0') {
            parser_advance(p);
        }
    }
}

static bool parser_match(Parser* p, const char* str) {
    int len = strlen(str);
    if (strncmp(&p->input[p->pos], str, len) == 0) {
        for (int i = 0; i < len; i++) {
            parser_advance(p);
        }
        return true;
    }
    return false;
}

static char* parser_read_identifier(Parser* p) {
    char buffer[256];
    int i = 0;
    
    while ((isalnum(p->current_char) || p->current_char == '_') && i < 255) {
        buffer[i++] = p->current_char;
        parser_advance(p);
    }
    buffer[i] = '\0';
    
    return strdup(buffer);
}

static int64_t parser_read_number(Parser* p) {
    char buffer[64];
    int i = 0;
    
    bool is_negative = false;
    if (p->current_char == '-') {
        is_negative = true;
        parser_advance(p);
    }
    
    while (isdigit(p->current_char) && i < 63) {
        buffer[i++] = p->current_char;
        parser_advance(p);
    }
    buffer[i] = '\0';
    
    int64_t val = atoll(buffer);
    return is_negative ? -val : val;
}

// ============================================================================
// CONST VALUE PARSING
// ============================================================================

static ConstValue* parser_parse_const_value(Parser* p) {
    parser_skip_whitespace(p);
    
    if (isdigit(p->current_char) || p->current_char == '-') {
        // Number
        int64_t val = parser_read_number(p);
        return const_value_new_usize((uint64_t)val);
    } else if (parser_match(p, "true")) {
        return const_value_new_bool(true);
    } else if (parser_match(p, "false")) {
        return const_value_new_bool(false);
    } else if (p->current_char == '\'') {
        // Char literal
        parser_advance(p); // Skip opening '
        char ch = p->current_char;
        parser_advance(p);
        if (p->current_char == '\'') {
            parser_advance(p); // Skip closing '
        }
        return const_value_new_char(ch);
    } else if (isalpha(p->current_char)) {
        // Const expression reference
        char* expr = parser_read_identifier(p);
        ConstValue* val = const_value_new_expr(expr);
        free(expr);
        return val;
    }
    
    return NULL;
}

// ============================================================================
// CONST DECLARATION PARSING (MLP-style)
// ============================================================================

static bool parser_parse_const_declaration(Parser* p) {
    // MLP syntax: dim numbers as i32 = 10
    // or: const N as usize = 100
    
    parser_skip_whitespace(p);
    parser_skip_comment(p);
    
    bool is_dim = false;
    if (parser_match(p, "dim ")) {
        is_dim = true;
    } else if (parser_match(p, "const ")) {
        is_dim = false;
    } else {
        return false;
    }
    
    parser_skip_whitespace(p);
    
    // Parse const name
    char* name = parser_read_identifier(p);
    if (!name) {
        return false;
    }
    
    parser_skip_whitespace(p);
    
    // Parse "as type"
    ConstKind kind = CONST_INT;
    if (parser_match(p, "as ")) {
        parser_skip_whitespace(p);
        char* type_name = parser_read_identifier(p);
        
        if (strcmp(type_name, "i32") == 0) {
            kind = CONST_INT;
        } else if (strcmp(type_name, "usize") == 0) {
            kind = CONST_USIZE;
        } else if (strcmp(type_name, "bool") == 0) {
            kind = CONST_BOOL;
        } else if (strcmp(type_name, "char") == 0) {
            kind = CONST_CHAR;
        }
        
        free(type_name);
    }
    
    parser_skip_whitespace(p);
    
    // Parse "= value"
    ConstValue* value = NULL;
    if (p->current_char == '=') {
        parser_advance(p);
        parser_skip_whitespace(p);
        value = parser_parse_const_value(p);
    }
    
    // For now, just track that we saw it
    (void)is_dim;
    (void)value;
    
    free(name);
    return true;
}

// ============================================================================
// TYPE DECLARATION PARSING (MLP-style)
// ============================================================================

static ConstGenericType* parser_parse_type_declaration(Parser* p) {
    // MLP syntax: type Array<T, N>
    
    parser_skip_whitespace(p);
    
    if (!parser_match(p, "type ")) {
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Parse type name
    char* base_type = parser_read_identifier(p);
    if (!base_type) {
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Parse generic parameters
    if (p->current_char != '<') {
        free(base_type);
        return NULL;
    }
    parser_advance(p); // Skip '<'
    parser_skip_whitespace(p);
    
    // First parameter is type parameter
    char* type_param = parser_read_identifier(p);
    ConstGenericType* gen_type = const_generic_type_new(base_type, type_param);
    
    free(base_type);
    free(type_param);
    
    parser_skip_whitespace(p);
    
    // Parse const parameters
    while (p->current_char == ',') {
        parser_advance(p); // Skip ','
        parser_skip_whitespace(p);
        
        // Optional "const" keyword
        if (parser_match(p, "const ")) {
            parser_skip_whitespace(p);
        }
        
        char* param_name = parser_read_identifier(p);
        if (!param_name) break;
        
        ConstGenericParam* param = const_generic_param_new(param_name, CONST_USIZE);
        const_generic_type_add_param(gen_type, param);
        
        free(param_name);
        parser_skip_whitespace(p);
    }
    
    // Skip '>'
    if (p->current_char == '>') {
        parser_advance(p);
    }
    
    return gen_type;
}

// ============================================================================
// TYPE INSTANTIATION PARSING
// ============================================================================

static ConstGenericInstance* parser_parse_instantiation(Parser* p) {
    // MLP syntax: dim arr as Array<i32, 10>
    
    parser_skip_whitespace(p);
    
    if (!parser_match(p, "dim ")) {
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Parse variable name
    char* var_name = parser_read_identifier(p);
    if (!var_name) {
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Parse "as"
    if (!parser_match(p, "as ")) {
        free(var_name);
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Parse type name
    char* type_name = parser_read_identifier(p);
    if (!type_name) {
        free(var_name);
        return NULL;
    }
    
    // Find generic type
    ConstGenericType* gen_type = const_generic_type_find(p->ctx, type_name);
    if (!gen_type) {
        free(var_name);
        free(type_name);
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Parse '<'
    if (p->current_char != '<') {
        free(var_name);
        free(type_name);
        return NULL;
    }
    parser_advance(p);
    parser_skip_whitespace(p);
    
    // Parse concrete type
    char* concrete_type = parser_read_identifier(p);
    parser_skip_whitespace(p);
    
    // Parse const arguments
    ConstValue* const_args[10];
    int const_arg_count = 0;
    
    while (p->current_char == ',' && const_arg_count < 10) {
        parser_advance(p); // Skip ','
        parser_skip_whitespace(p);
        
        ConstValue* val = parser_parse_const_value(p);
        if (val) {
            const_args[const_arg_count++] = val;
        }
        
        parser_skip_whitespace(p);
    }
    
    // Skip '>'
    if (p->current_char == '>') {
        parser_advance(p);
    }
    
    // Create instance
    ConstGenericInstance* inst = const_generic_instantiate(
        p->ctx,
        gen_type,
        concrete_type,
        const_args,
        const_arg_count
    );
    
    free(var_name);
    free(type_name);
    free(concrete_type);
    
    return inst;
}

// ============================================================================
// MAIN PARSING FUNCTIONS
// ============================================================================

bool const_generic_parse(ConstGenericContext* ctx, const char* input) {
    Parser parser;
    parser_init(&parser, input, ctx);
    
    while (parser.current_char != '\0') {
        parser_skip_whitespace(&parser);
        parser_skip_comment(&parser);
        
        if (parser.current_char == '\0') break;
        
        // Try parsing type declaration first
        ConstGenericType* type = parser_parse_type_declaration(&parser);
        if (type) {
            const_generic_type_register(ctx, type);
            continue;
        }
        
        // Check if it's a const declaration (has '=' sign)
        // Look ahead to distinguish between "dim X as i32 = 10" and "dim arr as Array<...>"
        int saved_pos = parser.pos;
        bool has_equals = false;
        while (parser.current_char != '\n' && parser.current_char != '\0') {
            if (parser.current_char == '=') {
                has_equals = true;
                break;
            }
            if (parser.current_char == '<') {
                // This is a generic instantiation
                break;
            }
            parser_advance(&parser);
        }
        
        // Restore position
        parser.pos = saved_pos;
        parser.current_char = parser.input[parser.pos];
        
        if (has_equals) {
            // Try parsing const declaration
            if (parser_parse_const_declaration(&parser)) {
                continue;
            }
        } else {
            // Try parsing instantiation
            ConstGenericInstance* inst = parser_parse_instantiation(&parser);
            if (inst) {
                // Instance already added to context by const_generic_instantiate
                continue;
            }
        }
        
        // Skip unknown line
        while (parser.current_char != '\n' && parser.current_char != '\0') {
            parser_advance(&parser);
        }
    }
    
    return true;
}

bool const_generic_parse_file(ConstGenericContext* ctx, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        return false;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    bool result = const_generic_parse(ctx, content);
    free(content);
    
    return result;
}

// ============================================================================
// QUERY FUNCTIONS
// ============================================================================

int const_generic_count_instances_of_type(ConstGenericContext* ctx, const char* type_name) {
    int count = 0;
    ConstGenericInstance* current = ctx->instances;
    while (current) {
        if (strcmp(current->generic_type->base_type, type_name) == 0) {
            count++;
        }
        current = current->next;
    }
    return count;
}
