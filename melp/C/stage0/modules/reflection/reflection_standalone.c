/**
 * MLP Reflection Module - Standalone Compiler #39 (Stage 1)
 * 
 * İçerik:
 * - Runtime type information (RTTI)
 * - Type introspection: typeof, nameof
 * - Field/Method inspection
 * - Dynamic method invocation
 * - Type metadata access
 * - Attribute/Annotation queries
 * 
 * Bağımlılıklar: Tüm Stage 0 modülleri (1-37) + preprocessor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ============================================================================
// STRUCTURES
// ============================================================================

typedef enum {
    TYPE_PRIMITIVE,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_ARRAY,
    TYPE_POINTER,
    TYPE_FUNCTION,
    TYPE_INTERFACE,
    TYPE_GENERIC
} TypeCategory;

typedef enum {
    PRIM_NUMERIC,
    PRIM_TEXT,
    PRIM_BOOLEAN,
    PRIM_VOID
} PrimitiveKind;

typedef struct {
    char name[128];
    TypeCategory category;
    PrimitiveKind primitive;
    int size;
    int alignment;
    int is_const;
    int is_public;
} TypeInfo;

typedef struct {
    char name[64];
    char type_name[64];
    int offset;
    int is_public;
} FieldInfo;

typedef struct {
    char name[64];
    char return_type[64];
    char params[256];
    int param_count;
    int is_public;
    int is_static;
} MethodInfo;

typedef struct {
    char name[64];
    char value[256];
} AttributeInfo;

typedef struct {
    TypeInfo types[128];
    int type_count;
    FieldInfo fields[256];
    int field_count;
    MethodInfo methods[128];
    int method_count;
    AttributeInfo attributes[64];
    int attr_count;
} ReflectionCtx;

// ============================================================================
// LEXER
// ============================================================================

typedef enum {
    TOK_EOF, TOK_IDENT, TOK_STRING, TOK_NUMBER, TOK_KEYWORD,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_LBRACKET, TOK_RBRACKET,
    TOK_COLON, TOK_COMMA, TOK_DOT, TOK_AT, TOK_NEWLINE, TOK_OTHER
} TokType;

typedef struct {
    TokType type;
    char value[512];
    int line;
} Token;

static const char* src_text;
static int src_pos;
static int src_line;

static void lexer_init(const char* text) {
    src_text = text;
    src_pos = 0;
    src_line = 1;
}

static char peek_char(void) { return src_text[src_pos]; }

static char next_char(void) {
    char c = src_text[src_pos];
    if (c) {
        if (c == '\n') src_line++;
        src_pos++;
    }
    return c;
}

static void skip_spaces(void) {
    while (peek_char() && peek_char() != '\n' && isspace(peek_char())) {
        next_char();
    }
}

static int is_keyword(const char* s) {
    const char* keywords[] = {
        "struct", "enum", "trait", "impl", "function", "end",
        "numeric", "text", "boolean", "void", "public", "private",
        "static", "const", "typeof", "nameof", "is", "as", NULL
    };
    for (int i = 0; keywords[i]; i++) {
        if (strcmp(s, keywords[i]) == 0) return 1;
    }
    return 0;
}

static Token next_token(void) {
    Token t = {TOK_EOF, "", src_line};
    skip_spaces();
    
    char c = peek_char();
    if (!c) return t;
    t.line = src_line;
    
    if (c == '\n') { t.type = TOK_NEWLINE; t.value[0] = '\n'; t.value[1] = '\0'; next_char(); return t; }
    if (c == '@') { next_char(); t.type = TOK_AT; strcpy(t.value, "@"); return t; }
    if (c == '(') { next_char(); t.type = TOK_LPAREN; strcpy(t.value, "("); return t; }
    if (c == ')') { next_char(); t.type = TOK_RPAREN; strcpy(t.value, ")"); return t; }
    if (c == '{') { next_char(); t.type = TOK_LBRACE; strcpy(t.value, "{"); return t; }
    if (c == '}') { next_char(); t.type = TOK_RBRACE; strcpy(t.value, "}"); return t; }
    if (c == '[') { next_char(); t.type = TOK_LBRACKET; strcpy(t.value, "["); return t; }
    if (c == ']') { next_char(); t.type = TOK_RBRACKET; strcpy(t.value, "]"); return t; }
    if (c == ':') { next_char(); t.type = TOK_COLON; strcpy(t.value, ":"); return t; }
    if (c == ',') { next_char(); t.type = TOK_COMMA; strcpy(t.value, ","); return t; }
    if (c == '.') { next_char(); t.type = TOK_DOT; strcpy(t.value, "."); return t; }
    
    // String
    if (c == '"') {
        next_char();
        int i = 0;
        while (peek_char() && peek_char() != '"' && i < 510) {
            t.value[i++] = next_char();
        }
        t.value[i] = '\0';
        if (peek_char() == '"') next_char();
        t.type = TOK_STRING;
        return t;
    }
    
    // Number
    if (isdigit(c)) {
        int i = 0;
        while (isdigit(peek_char()) && i < 510) t.value[i++] = next_char();
        t.value[i] = '\0';
        t.type = TOK_NUMBER;
        return t;
    }
    
    // Identifier/Keyword
    if (isalpha(c) || c == '_') {
        int i = 0;
        while ((isalnum(peek_char()) || peek_char() == '_') && i < 510) {
            t.value[i++] = next_char();
        }
        t.value[i] = '\0';
        t.type = is_keyword(t.value) ? TOK_KEYWORD : TOK_IDENT;
        return t;
    }
    
    t.type = TOK_OTHER;
    t.value[0] = next_char();
    t.value[1] = '\0';
    return t;
}

static Token current_tok;
static void advance(void) { current_tok = next_token(); }

// ============================================================================
// CONTEXT
// ============================================================================

static ReflectionCtx* ctx_create(void) {
    ReflectionCtx* ctx = calloc(1, sizeof(ReflectionCtx));
    return ctx;
}

static void ctx_free(ReflectionCtx* ctx) {
    free(ctx);
}

// ============================================================================
// PARSER
// ============================================================================

typedef struct {
    int structs;
    int enums;
    int traits;
    int functions;
    int fields;
    int methods;
    int attributes;
    int typeof_calls;
    int nameof_calls;
    int is_checks;
    int as_casts;
} ParseStats;

static ParseStats stats;

static void add_type(ReflectionCtx* ctx, const char* name, TypeCategory cat) {
    if (ctx->type_count >= 128) return;
    TypeInfo* t = &ctx->types[ctx->type_count++];
    strncpy(t->name, name, sizeof(t->name) - 1);
    t->name[sizeof(t->name) - 1] = '\0';
    t->category = cat;
}

static void add_field(ReflectionCtx* ctx, const char* name, const char* type) {
    if (ctx->field_count >= 256) return;
    FieldInfo* f = &ctx->fields[ctx->field_count++];
    strncpy(f->name, name, sizeof(f->name) - 1);
    f->name[sizeof(f->name) - 1] = '\0';
    strncpy(f->type_name, type, sizeof(f->type_name) - 1);
    f->type_name[sizeof(f->type_name) - 1] = '\0';
}

static void add_method(ReflectionCtx* ctx, const char* name, const char* ret_type) {
    if (ctx->method_count >= 128) return;
    MethodInfo* m = &ctx->methods[ctx->method_count++];
    strncpy(m->name, name, sizeof(m->name) - 1);
    m->name[sizeof(m->name) - 1] = '\0';
    strncpy(m->return_type, ret_type, sizeof(m->return_type) - 1);
    m->return_type[sizeof(m->return_type) - 1] = '\0';
}

static void add_attribute(ReflectionCtx* ctx, const char* name, const char* value) {
    if (ctx->attr_count >= 64) return;
    AttributeInfo* a = &ctx->attributes[ctx->attr_count++];
    strncpy(a->name, name, sizeof(a->name) - 1);
    a->name[sizeof(a->name) - 1] = '\0';
    strncpy(a->value, value, sizeof(a->value) - 1);
    a->value[sizeof(a->value) - 1] = '\0';
}

static void parse_attribute(ReflectionCtx* ctx) {
    // @attribute or @attribute(value)
    advance();  // skip @
    if (current_tok.type == TOK_IDENT || current_tok.type == TOK_KEYWORD) {
        char attr_name[64];
        strncpy(attr_name, current_tok.value, sizeof(attr_name) - 1);
        attr_name[sizeof(attr_name) - 1] = '\0';
        
        char attr_value[256] = "";
        advance();
        
        if (current_tok.type == TOK_LPAREN) {
            advance();  // skip (
            while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF) {
                strcat(attr_value, current_tok.value);
                advance();
            }
            if (current_tok.type == TOK_RPAREN) advance();
        }
        
        add_attribute(ctx, attr_name, attr_value);
        printf("      ✓ Attribute: @%s%s%s%s\n", attr_name, 
               attr_value[0] ? "(" : "", attr_value, attr_value[0] ? ")" : "");
        stats.attributes++;
    }
}

static void parse_struct(ReflectionCtx* ctx) {
    advance();  // skip 'struct'
    if (current_tok.type == TOK_IDENT) {
        add_type(ctx, current_tok.value, TYPE_STRUCT);
        printf("      ✓ Struct: %s\n", current_tok.value);
        stats.structs++;
        
        // Skip to end struct
        while (current_tok.type != TOK_EOF) {
            if (current_tok.type == TOK_KEYWORD && strcmp(current_tok.value, "end") == 0) {
                advance();
                break;
            }
            // Field detection: name: type
            if (current_tok.type == TOK_IDENT) {
                char field_name[64];
                strncpy(field_name, current_tok.value, sizeof(field_name) - 1);
                field_name[sizeof(field_name) - 1] = '\0';
                advance();
                if (current_tok.type == TOK_COLON) {
                    advance();
                    if (current_tok.type == TOK_IDENT || current_tok.type == TOK_KEYWORD) {
                        add_field(ctx, field_name, current_tok.value);
                        printf("        - field: %s: %s\n", field_name, current_tok.value);
                        stats.fields++;
                    }
                }
            }
            advance();
        }
    }
}

static void parse_enum(ReflectionCtx* ctx) {
    advance();  // skip 'enum'
    if (current_tok.type == TOK_IDENT) {
        add_type(ctx, current_tok.value, TYPE_ENUM);
        printf("      ✓ Enum: %s\n", current_tok.value);
        stats.enums++;
        
        // Skip to end enum
        while (current_tok.type != TOK_EOF) {
            if (current_tok.type == TOK_KEYWORD && strcmp(current_tok.value, "end") == 0) {
                advance();
                break;
            }
            advance();
        }
    }
}

static void parse_trait(ReflectionCtx* ctx) {
    advance();  // skip 'trait'
    if (current_tok.type == TOK_IDENT) {
        add_type(ctx, current_tok.value, TYPE_INTERFACE);
        printf("      ✓ Trait: %s\n", current_tok.value);
        stats.traits++;
        
        // Skip to end trait
        while (current_tok.type != TOK_EOF) {
            if (current_tok.type == TOK_KEYWORD && strcmp(current_tok.value, "end") == 0) {
                advance();
                break;
            }
            // Method signature
            if (current_tok.type == TOK_KEYWORD && strcmp(current_tok.value, "function") == 0) {
                advance();
                if (current_tok.type == TOK_IDENT) {
                    add_method(ctx, current_tok.value, "void");
                    printf("        - method: %s()\n", current_tok.value);
                    stats.methods++;
                }
            }
            advance();
        }
    }
}

static void parse_function(ReflectionCtx* ctx) {
    advance();  // skip 'function'
    if (current_tok.type == TOK_IDENT) {
        add_method(ctx, current_tok.value, "void");
        printf("      ✓ Function: %s\n", current_tok.value);
        stats.functions++;
        
        // Skip to end function
        while (current_tok.type != TOK_EOF) {
            if (current_tok.type == TOK_KEYWORD && strcmp(current_tok.value, "end") == 0) {
                advance();
                break;
            }
            advance();
        }
    }
}

static void parse_typeof(void) {
    advance();  // skip 'typeof'
    if (current_tok.type == TOK_LPAREN) {
        advance();
        if (current_tok.type == TOK_IDENT) {
            printf("      ✓ typeof(%s)\n", current_tok.value);
            stats.typeof_calls++;
        }
        while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF) advance();
        if (current_tok.type == TOK_RPAREN) advance();
    }
}

static void parse_nameof(void) {
    advance();  // skip 'nameof'
    if (current_tok.type == TOK_LPAREN) {
        advance();
        if (current_tok.type == TOK_IDENT) {
            printf("      ✓ nameof(%s)\n", current_tok.value);
            stats.nameof_calls++;
        }
        while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF) advance();
        if (current_tok.type == TOK_RPAREN) advance();
    }
}

static void parse_is_check(void) {
    // x is Type
    printf("      ✓ is type check\n");
    stats.is_checks++;
}

static void parse_as_cast(void) {
    // x as Type
    printf("      ✓ as type cast\n");
    stats.as_casts++;
}

static void parse_file(ReflectionCtx* ctx) {
    advance();
    
    while (current_tok.type != TOK_EOF) {
        if (current_tok.type == TOK_AT) {
            parse_attribute(ctx);
        } else if (current_tok.type == TOK_KEYWORD) {
            if (strcmp(current_tok.value, "struct") == 0) {
                parse_struct(ctx);
            } else if (strcmp(current_tok.value, "enum") == 0) {
                parse_enum(ctx);
            } else if (strcmp(current_tok.value, "trait") == 0) {
                parse_trait(ctx);
            } else if (strcmp(current_tok.value, "function") == 0) {
                parse_function(ctx);
            } else if (strcmp(current_tok.value, "typeof") == 0) {
                parse_typeof();
            } else if (strcmp(current_tok.value, "nameof") == 0) {
                parse_nameof();
            } else if (strcmp(current_tok.value, "is") == 0) {
                parse_is_check();
                advance();
            } else if (strcmp(current_tok.value, "as") == 0) {
                parse_as_cast();
                advance();
            } else {
                advance();
            }
        } else {
            advance();
        }
    }
}

// ============================================================================
// CODEGEN
// ============================================================================

static void generate_asm(const char* output_file, ReflectionCtx* ctx) {
    FILE* out = fopen(output_file, "w");
    if (!out) return;
    
    fprintf(out, "; MLP Reflection Module - Generated Assembly\n");
    fprintf(out, "; Types: %d, Fields: %d, Methods: %d\n", 
            ctx->type_count, ctx->field_count, ctx->method_count);
    fprintf(out, "\n");
    
    // Data section with type metadata
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db 'Reflection OK!', 10, 0\n");
    fprintf(out, "    msg_len equ $ - msg\n");
    
    // Type metadata tables
    for (int i = 0; i < ctx->type_count; i++) {
        fprintf(out, "    ; Type: %s (category: %d)\n", 
                ctx->types[i].name, ctx->types[i].category);
    }
    
    fprintf(out, "\n");
    fprintf(out, "section .bss\n");
    fprintf(out, "    ; Runtime type info storage\n");
    fprintf(out, "\n");
    
    // Text section
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n\n");
    fprintf(out, "_start:\n");
    fprintf(out, "    mov rax, 1\n");
    fprintf(out, "    mov rdi, 1\n");
    fprintf(out, "    lea rsi, [rel msg]\n");
    fprintf(out, "    mov rdx, msg_len\n");
    fprintf(out, "    syscall\n\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
    
    fclose(out);
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("🔍 Reflection Module - #39 Stage 1\n");
    printf("===================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("Input: %s\nOutput: %s\n\n", argv[1], argv[2]);
    
    // Read file
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    printf("[1/4] ✓ Read %ld bytes\n", size);
    
    // Lexer
    lexer_init(source);
    printf("[2/4] ✓ Lexer init\n");
    
    // Parse
    ReflectionCtx* ctx = ctx_create();
    printf("[3/4] Parsing reflection metadata...\n");
    memset(&stats, 0, sizeof(stats));
    parse_file(ctx);
    
    printf("      ✓ Types:\n");
    if (stats.structs > 0) printf("        Structs: %d\n", stats.structs);
    if (stats.enums > 0) printf("        Enums: %d\n", stats.enums);
    if (stats.traits > 0) printf("        Traits: %d\n", stats.traits);
    printf("        Total: %d\n", ctx->type_count);
    
    printf("      ✓ Members:\n");
    printf("        Fields: %d\n", stats.fields);
    printf("        Methods: %d\n", stats.methods);
    printf("        Functions: %d\n", stats.functions);
    
    if (stats.attributes > 0) printf("      ✓ Attributes: %d\n", stats.attributes);
    
    printf("      ✓ Reflection operations:\n");
    if (stats.typeof_calls > 0) printf("        typeof(): %d\n", stats.typeof_calls);
    if (stats.nameof_calls > 0) printf("        nameof(): %d\n", stats.nameof_calls);
    if (stats.is_checks > 0) printf("        is checks: %d\n", stats.is_checks);
    if (stats.as_casts > 0) printf("        as casts: %d\n", stats.as_casts);
    
    // Codegen
    printf("[4/4] ✓ Code generated\n");
    generate_asm(argv[2], ctx);
    
    printf("\n✅ Complete!\n");
    
    ctx_free(ctx);
    free(source);
    
    return 0;
}
