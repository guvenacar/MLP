/**
 * MODÜL #57: DECORATOR_SYSTEM - Parser Implementation
 * 
 * Parses MLP decorator syntax:
 * - @decorator
 * - @decorator(args)
 * - Multiple stacked decorators
 */

#define _GNU_SOURCE
#include "decorator_system.h"
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
    DecoratorContext* ctx;
} Parser;

// ============================================================================
// PARSER UTILITIES
// ============================================================================

static void parser_init(Parser* p, const char* input, DecoratorContext* ctx) {
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
    while (p->current_char == ' ' || p->current_char == '\t' || p->current_char == '\n' || p->current_char == '\r') {
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

static char* parser_read_string(Parser* p) {
    char buffer[512];
    int i = 0;
    
    // Skip opening quote
    parser_advance(p);
    
    while (p->current_char != '"' && p->current_char != '\0' && i < 511) {
        if (p->current_char == '\\') {
            parser_advance(p);
            // Handle escape sequences
            switch (p->current_char) {
                case 'n': buffer[i++] = '\n'; break;
                case 't': buffer[i++] = '\t'; break;
                case '\\': buffer[i++] = '\\'; break;
                case '"': buffer[i++] = '"'; break;
                default: buffer[i++] = p->current_char; break;
            }
        } else {
            buffer[i++] = p->current_char;
        }
        parser_advance(p);
    }
    buffer[i] = '\0';
    
    // Skip closing quote
    if (p->current_char == '"') {
        parser_advance(p);
    }
    
    return strdup(buffer);
}

static double parser_read_number(Parser* p) {
    char buffer[64];
    int i = 0;
    
    while ((isdigit(p->current_char) || p->current_char == '.' || p->current_char == '-') && i < 63) {
        buffer[i++] = p->current_char;
        parser_advance(p);
    }
    buffer[i] = '\0';
    
    return atof(buffer);
}

// ============================================================================
// DECORATOR ARGUMENT PARSING
// ============================================================================

static DecoratorArg* parser_parse_decorator_arg(Parser* p) {
    parser_skip_whitespace(p);
    
    if (p->current_char == '"') {
        // String argument
        char* str = parser_read_string(p);
        DecoratorArg* arg = decorator_arg_new(ARG_STRING, str);
        free(str);
        return arg;
    } else if (isdigit(p->current_char) || p->current_char == '-') {
        // Number argument
        double num = parser_read_number(p);
        return decorator_arg_new(ARG_NUMBER, &num);
    } else if (parser_match(p, "true")) {
        // Boolean true
        bool val = true;
        return decorator_arg_new(ARG_BOOLEAN, &val);
    } else if (parser_match(p, "false")) {
        // Boolean false
        bool val = false;
        return decorator_arg_new(ARG_BOOLEAN, &val);
    }
    
    return NULL;
}

static void parser_parse_decorator_args(Parser* p, Decorator* dec) {
    if (p->current_char != '(') {
        return;
    }
    
    parser_advance(p); // Skip '('
    parser_skip_whitespace(p);
    
    while (p->current_char != ')' && p->current_char != '\0') {
        DecoratorArg* arg = parser_parse_decorator_arg(p);
        if (arg) {
            // Add to decorator's arg list
            if (!dec->args) {
                dec->args = arg;
            } else {
                DecoratorArg* current = dec->args;
                while (current->next) {
                    current = current->next;
                }
                current->next = arg;
            }
        }
        
        parser_skip_whitespace(p);
        if (p->current_char == ',') {
            parser_advance(p);
            parser_skip_whitespace(p);
        }
    }
    
    if (p->current_char == ')') {
        parser_advance(p);
    }
}

// ============================================================================
// DECORATOR PARSING
// ============================================================================

static Decorator* parser_parse_decorator(Parser* p) {
    if (p->current_char != '@') {
        return NULL;
    }
    
    parser_advance(p); // Skip '@'
    parser_skip_whitespace(p);
    
    // Read decorator name
    char* name = parser_read_identifier(p);
    if (!name) {
        return NULL;
    }
    
    // Create decorator (type and target will be set later)
    Decorator* dec = decorator_new(name, DECORATOR_FUNCTION, "");
    dec->line_number = p->line;
    
    // Parse optional arguments
    parser_skip_whitespace(p);
    if (p->current_char == '(') {
        parser_parse_decorator_args(p, dec);
    }
    
    free(name);
    return dec;
}

// ============================================================================
// FUNCTION/CLASS DETECTION
// ============================================================================

static DecoratorType parser_detect_target_type(Parser* p) {
    parser_skip_whitespace(p);
    
    if (parser_match(p, "function")) {
        return DECORATOR_FUNCTION;
    } else if (parser_match(p, "sub")) {
        return DECORATOR_FUNCTION;
    } else if (parser_match(p, "class")) {
        return DECORATOR_CLASS;
    } else if (parser_match(p, "property")) {
        return DECORATOR_PROPERTY;
    } else if (parser_match(p, "async")) {
        return DECORATOR_ASYNC;
    }
    
    return DECORATOR_FUNCTION; // Default
}

static char* parser_parse_target_name(Parser* p) {
    parser_skip_whitespace(p);
    return parser_read_identifier(p);
}

// ============================================================================
// MAIN PARSING FUNCTIONS
// ============================================================================

bool decorator_parse(DecoratorContext* ctx, const char* input) {
    Parser parser;
    parser_init(&parser, input, ctx);
    
    Decorator* pending_decorators[10];
    int pending_count = 0;
    
    while (parser.current_char != '\0') {
        parser_skip_whitespace(&parser);
        parser_skip_comment(&parser);
        
        if (parser.current_char == '@') {
            // Parse decorator
            Decorator* dec = parser_parse_decorator(&parser);
            if (dec && pending_count < 10) {
                pending_decorators[pending_count++] = dec;
            }
        } else if (pending_count > 0) {
            // Next line should be function/class declaration
            DecoratorType type = parser_detect_target_type(&parser);
            char* target = parser_parse_target_name(&parser);
            
            if (target) {
                // Apply all pending decorators to this target
                for (int i = 0; i < pending_count; i++) {
                    pending_decorators[i]->type = type;
                    free(pending_decorators[i]->target_name);
                    pending_decorators[i]->target_name = strdup(target);
                    decorator_register(ctx, pending_decorators[i]);
                }
                free(target);
            }
            
            pending_count = 0;
        } else {
            // Skip non-decorator lines
            while (parser.current_char != '\n' && parser.current_char != '\0') {
                parser_advance(&parser);
            }
        }
    }
    
    return true;
}

// ============================================================================
// SPECIFIC DECORATOR PARSERS
// ============================================================================

Decorator* decorator_parse_single(const char* line, const char* target_name) {
    DecoratorContext* temp_ctx = decorator_context_new();
    Parser parser;
    parser_init(&parser, line, temp_ctx);
    
    Decorator* dec = parser_parse_decorator(&parser);
    if (dec) {
        free(dec->target_name);
        dec->target_name = strdup(target_name);
    }
    
    decorator_context_free(temp_ctx);
    return dec;
}

bool decorator_parse_file(DecoratorContext* ctx, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        return false;
    }
    
    // Read entire file
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    // Parse
    bool result = decorator_parse(ctx, content);
    free(content);
    
    return result;
}

// ============================================================================
// DECORATOR EXTRACTION
// ============================================================================

int decorator_extract_from_code(DecoratorContext* ctx, const char* code) {
    int count = 0;
    const char* ptr = code;
    
    while (*ptr) {
        if (*ptr == '@') {
            // Found decorator
            const char* start = ptr;
            ptr++; // Skip '@'
            
            // Extract decorator name
            char name[64];
            int i = 0;
            while (isalnum(*ptr) || *ptr == '_') {
                if (i < 63) {
                    name[i++] = *ptr;
                }
                ptr++;
            }
            name[i] = '\0';
            
            if (strlen(name) > 0) {
                count++;
            }
        }
        ptr++;
    }
    
    return count;
}

// ============================================================================
// DECORATOR QUERY
// ============================================================================

Decorator** decorator_find_all_for_target(DecoratorContext* ctx, const char* target_name, int* out_count) {
    // Count matching decorators
    int count = 0;
    Decorator* current = ctx->decorators;
    while (current) {
        if (strcmp(current->target_name, target_name) == 0) {
            count++;
        }
        current = current->next;
    }
    
    if (count == 0) {
        *out_count = 0;
        return NULL;
    }
    
    // Allocate array
    Decorator** result = malloc(sizeof(Decorator*) * count);
    int idx = 0;
    
    current = ctx->decorators;
    while (current) {
        if (strcmp(current->target_name, target_name) == 0) {
            result[idx++] = current;
        }
        current = current->next;
    }
    
    *out_count = count;
    return result;
}
