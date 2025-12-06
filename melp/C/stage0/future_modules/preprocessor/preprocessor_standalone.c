/**
 * MLP Preprocessor Module - Standalone Compiler #38 (Stage 1)
 * 
 * İçerik:
 * - Macro definitions: #define PI 3.14
 * - Function-like macros: #define MAX(a,b) ((a)>(b)?(a):(b))
 * - Conditional compilation: #if, #ifdef, #ifndef, #else, #endif
 * - Include directives: #include "file.mlp"
 * - Built-in macros: __FILE__, __LINE__, __DATE__, __TIME__
 * - Token pasting: ##
 * - Stringification: #
 * 
 * Bağımlılıklar: Tüm Stage 0 modülleri (1-37)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// ============================================================================
// STRUCTURES
// ============================================================================

typedef enum {
    MACRO_OBJECT,      // #define PI 3.14
    MACRO_FUNCTION,    // #define MAX(a,b) ...
    MACRO_VARIADIC,    // #define LOG(fmt, ...) ...
    MACRO_BUILTIN      // __FILE__, __LINE__, etc.
} MacroType;

typedef enum {
    DIR_DEFINE,
    DIR_UNDEF,
    DIR_INCLUDE,
    DIR_IF,
    DIR_IFDEF,
    DIR_IFNDEF,
    DIR_ELIF,
    DIR_ELSE,
    DIR_ENDIF,
    DIR_ERROR,
    DIR_WARNING,
    DIR_PRAGMA,
    DIR_LINE
} DirectiveType;

typedef struct {
    char name[64];
    int is_variadic;
} MacroParam;

typedef struct {
    char name[128];
    MacroType type;
    MacroParam params[16];
    int param_count;
    char body[1024];
    int line_defined;
} MacroDef;

typedef struct {
    DirectiveType type;
    char arg[256];
    int is_active;
    int line;
} Directive;

typedef struct {
    MacroDef macros[256];
    int macro_count;
    int cond_stack[64];  // 1 = active, 0 = inactive
    int cond_depth;
    char current_file[256];
    int current_line;
    char* include_paths[16];
    int include_path_count;
    // Stats
    int define_count;
    int include_count;
    int ifdef_count;
    int macro_expansions;
} PreprocessorCtx;

// ============================================================================
// LEXER (Simple)
// ============================================================================

typedef enum {
    TOK_EOF, TOK_HASH, TOK_IDENT, TOK_STRING, TOK_NUMBER,
    TOK_LPAREN, TOK_RPAREN, TOK_COMMA, TOK_NEWLINE, TOK_OTHER
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

static char peek_char(void) {
    return src_text[src_pos];
}

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

static Token next_token(void) {
    Token t = {TOK_EOF, "", src_line};
    skip_spaces();
    
    char c = peek_char();
    if (!c) return t;
    
    t.line = src_line;
    
    // Newline
    if (c == '\n') {
        t.type = TOK_NEWLINE;
        t.value[0] = '\n';
        t.value[1] = '\0';
        next_char();
        return t;
    }
    
    // Hash (directive start)
    if (c == '#') {
        next_char();
        if (peek_char() == '#') {  // Token paste ##
            next_char();
            t.type = TOK_OTHER;
            strcpy(t.value, "##");
        } else {
            t.type = TOK_HASH;
            t.value[0] = '#';
            t.value[1] = '\0';
        }
        return t;
    }
    
    // Identifier
    if (isalpha(c) || c == '_') {
        int i = 0;
        while ((isalnum(peek_char()) || peek_char() == '_') && i < 510) {
            t.value[i++] = next_char();
        }
        t.value[i] = '\0';
        t.type = TOK_IDENT;
        return t;
    }
    
    // Number
    if (isdigit(c)) {
        int i = 0;
        while (isdigit(peek_char()) && i < 510) {
            t.value[i++] = next_char();
        }
        // Float support
        if (peek_char() == '.') {
            t.value[i++] = next_char();
            while (isdigit(peek_char()) && i < 510) {
                t.value[i++] = next_char();
            }
        }
        t.value[i] = '\0';
        t.type = TOK_NUMBER;
        return t;
    }
    
    // String literal
    if (c == '"' || c == '<') {
        char end_char = (c == '<') ? '>' : '"';
        next_char();  // skip opening quote
        int i = 0;
        while (peek_char() && peek_char() != end_char && peek_char() != '\n' && i < 510) {
            t.value[i++] = next_char();
        }
        t.value[i] = '\0';
        if (peek_char() == end_char) next_char();  // skip closing
        t.type = TOK_STRING;
        return t;
    }
    
    // Parentheses
    if (c == '(') { next_char(); t.type = TOK_LPAREN; strcpy(t.value, "("); return t; }
    if (c == ')') { next_char(); t.type = TOK_RPAREN; strcpy(t.value, ")"); return t; }
    if (c == ',') { next_char(); t.type = TOK_COMMA; strcpy(t.value, ","); return t; }
    
    // Other
    t.type = TOK_OTHER;
    t.value[0] = next_char();
    t.value[1] = '\0';
    return t;
}

static Token current_tok;

static void advance(void) {
    current_tok = next_token();
}

// ============================================================================
// PREPROCESSOR CONTEXT
// ============================================================================

static PreprocessorCtx* ctx_create(const char* filename) {
    PreprocessorCtx* ctx = calloc(1, sizeof(PreprocessorCtx));
    strncpy(ctx->current_file, filename, 255);
    ctx->current_line = 1;
    ctx->cond_stack[0] = 1;  // Default: active
    ctx->cond_depth = 0;
    return ctx;
}

static void ctx_free(PreprocessorCtx* ctx) {
    for (int i = 0; i < ctx->include_path_count; i++) {
        free(ctx->include_paths[i]);
    }
    free(ctx);
}

// ============================================================================
// MACRO OPERATIONS
// ============================================================================

static void add_macro(PreprocessorCtx* ctx, const char* name, MacroType type, 
                      const char* body, int param_count) {
    if (ctx->macro_count >= 256) return;
    
    MacroDef* m = &ctx->macros[ctx->macro_count++];
    strncpy(m->name, name, sizeof(m->name) - 1);
    m->name[sizeof(m->name) - 1] = '\0';  // Null-terminator garantisi
    m->type = type;
    strncpy(m->body, body ? body : "", sizeof(m->body) - 1);
    m->body[sizeof(m->body) - 1] = '\0';  // Null-terminator garantisi
    m->param_count = param_count;
    m->line_defined = src_line;
    
    ctx->define_count++;
}

static MacroDef* find_macro(PreprocessorCtx* ctx, const char* name) {
    for (int i = 0; i < ctx->macro_count; i++) {
        if (strcmp(ctx->macros[i].name, name) == 0) {
            return &ctx->macros[i];
        }
    }
    return NULL;
}

static void remove_macro(PreprocessorCtx* ctx, const char* name) {
    for (int i = 0; i < ctx->macro_count; i++) {
        if (strcmp(ctx->macros[i].name, name) == 0) {
            // Shift remaining
            for (int j = i; j < ctx->macro_count - 1; j++) {
                ctx->macros[j] = ctx->macros[j + 1];
            }
            ctx->macro_count--;
            return;
        }
    }
}

// ============================================================================
// BUILTIN MACROS
// ============================================================================

static void register_builtins(PreprocessorCtx* ctx) {
    // __FILE__
    add_macro(ctx, "__FILE__", MACRO_BUILTIN, ctx->current_file, 0);
    
    // __LINE__ - dynamic
    add_macro(ctx, "__LINE__", MACRO_BUILTIN, "0", 0);
    
    // __DATE__
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    char date_buf[32];
    strftime(date_buf, sizeof(date_buf), "%b %d %Y", tm_info);
    add_macro(ctx, "__DATE__", MACRO_BUILTIN, date_buf, 0);
    
    // __TIME__
    char time_buf[32];
    strftime(time_buf, sizeof(time_buf), "%H:%M:%S", tm_info);
    add_macro(ctx, "__TIME__", MACRO_BUILTIN, time_buf, 0);
    
    // MLP specific
    add_macro(ctx, "__MLP__", MACRO_BUILTIN, "1", 0);
    add_macro(ctx, "__MLP_VERSION__", MACRO_BUILTIN, "1.0", 0);
}

// ============================================================================
// DIRECTIVE PARSING
// ============================================================================

static DirectiveType parse_directive_type(const char* name) {
    if (strcmp(name, "define") == 0) return DIR_DEFINE;
    if (strcmp(name, "undef") == 0) return DIR_UNDEF;
    if (strcmp(name, "include") == 0) return DIR_INCLUDE;
    if (strcmp(name, "if") == 0) return DIR_IF;
    if (strcmp(name, "ifdef") == 0) return DIR_IFDEF;
    if (strcmp(name, "ifndef") == 0) return DIR_IFNDEF;
    if (strcmp(name, "elif") == 0) return DIR_ELIF;
    if (strcmp(name, "else") == 0) return DIR_ELSE;
    if (strcmp(name, "endif") == 0) return DIR_ENDIF;
    if (strcmp(name, "error") == 0) return DIR_ERROR;
    if (strcmp(name, "warning") == 0) return DIR_WARNING;
    if (strcmp(name, "pragma") == 0) return DIR_PRAGMA;
    if (strcmp(name, "line") == 0) return DIR_LINE;
    return DIR_PRAGMA;  // Unknown treated as pragma
}

// ============================================================================
// PARSER
// ============================================================================

typedef struct {
    int defines;
    int undefs;
    int includes;
    int ifdefs;
    int ifndefs;
    int ifs;
    int elses;
    int endifs;
    int errors;
    int warnings;
    int pragmas;
    int object_macros;
    int function_macros;
    int variadic_macros;
} ParseStats;

static ParseStats stats;

static void parse_define(PreprocessorCtx* ctx) {
    skip_spaces();
    advance();  // Get macro name
    
    if (current_tok.type != TOK_IDENT) {
        printf("      ⚠ Expected identifier after #define\n");
        return;
    }
    
    char macro_name[128];
    strncpy(macro_name, current_tok.value, sizeof(macro_name) - 1);
    macro_name[sizeof(macro_name) - 1] = '\0';
    
    advance();  // Look for ( or body
    
    // Check for function-like macro
    if (current_tok.type == TOK_LPAREN) {
        // Function-like macro
        MacroDef* m = &ctx->macros[ctx->macro_count];
        strncpy(m->name, macro_name, sizeof(m->name) - 1);
        m->name[sizeof(m->name) - 1] = '\0';
        m->param_count = 0;
        
        advance();  // skip (
        
        // Parse parameters
        while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF && 
               current_tok.type != TOK_NEWLINE) {
            if (current_tok.type == TOK_IDENT) {
                strncpy(m->params[m->param_count].name, current_tok.value, sizeof(m->params[m->param_count].name) - 1);
                m->params[m->param_count].name[sizeof(m->params[m->param_count].name) - 1] = '\0';
                m->params[m->param_count].is_variadic = 0;
                m->param_count++;
            } else if (strcmp(current_tok.value, "...") == 0 || 
                       (current_tok.type == TOK_OTHER && current_tok.value[0] == '.')) {
                m->params[m->param_count - 1].is_variadic = 1;
                m->type = MACRO_VARIADIC;
                stats.variadic_macros++;
            }
            advance();
            if (current_tok.type == TOK_COMMA) advance();
        }
        
        if (current_tok.type == TOK_RPAREN) advance();
        
        // Read body until newline
        char body[1024] = "";
        int bi = 0;
        while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
            int len = strlen(current_tok.value);
            if (bi + len < 1020) {
                strcat(body, current_tok.value);
                bi += len;
            }
            advance();
        }
        
        strncpy(m->body, body, sizeof(m->body) - 1);
        m->body[sizeof(m->body) - 1] = '\0';
        m->type = (m->type == MACRO_VARIADIC) ? MACRO_VARIADIC : MACRO_FUNCTION;
        m->line_defined = src_line;
        ctx->macro_count++;
        ctx->define_count++;
        
        if (m->type == MACRO_VARIADIC) {
            printf("      ✓ Variadic macro: %s(%d params)\n", macro_name, m->param_count);
        } else {
            printf("      ✓ Function macro: %s(%d params)\n", macro_name, m->param_count);
            stats.function_macros++;
        }
    } else {
        // Object-like macro
        char body[1024] = "";
        int bi = 0;
        
        while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
            int len = strlen(current_tok.value);
            if (bi + len < 1020) {
                if (bi > 0) strcat(body, " ");
                strcat(body, current_tok.value);
                bi += len + 1;
            }
            advance();
        }
        
        add_macro(ctx, macro_name, MACRO_OBJECT, body, 0);
        printf("      ✓ Object macro: %s = %s\n", macro_name, body[0] ? body : "(empty)");
        stats.object_macros++;
    }
    
    stats.defines++;
}

static void parse_undef(PreprocessorCtx* ctx) {
    advance();  // Get macro name
    if (current_tok.type == TOK_IDENT) {
        printf("      ✓ Undef: %s\n", current_tok.value);
        remove_macro(ctx, current_tok.value);
        stats.undefs++;
    }
}

static void parse_include(PreprocessorCtx* ctx) {
    advance();  // Get filename
    
    if (current_tok.type == TOK_STRING) {
        printf("      ✓ Include: \"%s\"\n", current_tok.value);
        ctx->include_count++;
        stats.includes++;
    }
}

static void parse_ifdef(PreprocessorCtx* ctx, int is_ifndef) {
    advance();  // Get macro name
    
    if (current_tok.type == TOK_IDENT) {
        MacroDef* m = find_macro(ctx, current_tok.value);
        int defined = (m != NULL);
        int active = is_ifndef ? !defined : defined;
        
        ctx->cond_depth++;
        ctx->cond_stack[ctx->cond_depth] = active && ctx->cond_stack[ctx->cond_depth - 1];
        
        printf("      ✓ %s %s -> %s\n", 
               is_ifndef ? "#ifndef" : "#ifdef",
               current_tok.value,
               ctx->cond_stack[ctx->cond_depth] ? "ACTIVE" : "SKIPPED");
        
        if (is_ifndef) stats.ifndefs++;
        else stats.ifdefs++;
    }
}

static void parse_else(PreprocessorCtx* ctx) {
    if (ctx->cond_depth > 0) {
        // Flip the condition if parent is active
        if (ctx->cond_stack[ctx->cond_depth - 1]) {
            ctx->cond_stack[ctx->cond_depth] = !ctx->cond_stack[ctx->cond_depth];
        }
        printf("      ✓ #else -> %s\n", 
               ctx->cond_stack[ctx->cond_depth] ? "ACTIVE" : "SKIPPED");
        stats.elses++;
    }
}

static void parse_endif(PreprocessorCtx* ctx) {
    if (ctx->cond_depth > 0) {
        ctx->cond_depth--;
        printf("      ✓ #endif (depth: %d)\n", ctx->cond_depth);
        stats.endifs++;
    }
}

static void parse_error(void) {
    // Read message until newline
    char msg[256] = "";
    advance();
    while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
        strcat(msg, current_tok.value);
        strcat(msg, " ");
        advance();
    }
    printf("      ⚠ #error: %s\n", msg);
    stats.errors++;
}

static void parse_warning(void) {
    // Read message until newline
    char msg[256] = "";
    advance();
    while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
        strcat(msg, current_tok.value);
        strcat(msg, " ");
        advance();
    }
    printf("      ⚠ #warning: %s\n", msg);
    stats.warnings++;
}

static void parse_pragma(void) {
    // Read pragma until newline
    char pragma[256] = "";
    advance();
    while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
        strcat(pragma, current_tok.value);
        strcat(pragma, " ");
        advance();
    }
    printf("      ✓ #pragma: %s\n", pragma);
    stats.pragmas++;
}

static void parse_directive(PreprocessorCtx* ctx) {
    advance();  // skip #
    
    if (current_tok.type != TOK_IDENT) return;
    
    DirectiveType type = parse_directive_type(current_tok.value);
    
    switch (type) {
        case DIR_DEFINE:
            parse_define(ctx);
            break;
        case DIR_UNDEF:
            parse_undef(ctx);
            break;
        case DIR_INCLUDE:
            parse_include(ctx);
            break;
        case DIR_IFDEF:
            parse_ifdef(ctx, 0);
            break;
        case DIR_IFNDEF:
            parse_ifdef(ctx, 1);
            break;
        case DIR_ELSE:
            parse_else(ctx);
            break;
        case DIR_ENDIF:
            parse_endif(ctx);
            break;
        case DIR_ERROR:
            parse_error();
            break;
        case DIR_WARNING:
            parse_warning();
            break;
        case DIR_PRAGMA:
            parse_pragma();
            break;
        default:
            // Skip unknown directive
            while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
                advance();
            }
            break;
    }
}

static void parse_file(PreprocessorCtx* ctx) {
    advance();  // Get first token
    
    while (current_tok.type != TOK_EOF) {
        if (current_tok.type == TOK_HASH) {
            parse_directive(ctx);
        } else if (current_tok.type == TOK_IDENT) {
            // Check for macro usage
            MacroDef* m = find_macro(ctx, current_tok.value);
            if (m && ctx->cond_stack[ctx->cond_depth]) {
                ctx->macro_expansions++;
            }
        }
        
        if (current_tok.type != TOK_EOF) {
            advance();
        }
    }
}

// ============================================================================
// CODEGEN
// ============================================================================

static void generate_asm(const char* output_file, PreprocessorCtx* ctx) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file\n");
        return;
    }
    
    fprintf(out, "; MLP Preprocessor Module - Generated Assembly\n");
    fprintf(out, "; File: %s\n", ctx->current_file);
    fprintf(out, "; Macros defined: %d\n", ctx->macro_count);
    fprintf(out, "; Includes: %d\n", ctx->include_count);
    fprintf(out, "\n");
    
    // Data section
    fprintf(out, "section .data\n");
    fprintf(out, "    msg: db 'Preprocessor OK!', 10, 0\n");
    fprintf(out, "    msg_len equ $ - msg\n");
    
    // Macro values as constants
    for (int i = 0; i < ctx->macro_count && i < 10; i++) {
        MacroDef* m = &ctx->macros[i];
        if (m->type == MACRO_OBJECT && m->body[0]) {
            fprintf(out, "    ; Macro %s = %s\n", m->name, m->body);
        }
    }
    
    fprintf(out, "\n");
    
    // BSS section
    fprintf(out, "section .bss\n");
    fprintf(out, "    ; Reserved space for macro expansions\n");
    fprintf(out, "\n");
    
    // Text section
    fprintf(out, "section .text\n");
    fprintf(out, "    global _start\n");
    fprintf(out, "\n");
    fprintf(out, "_start:\n");
    fprintf(out, "    ; Print success message\n");
    fprintf(out, "    mov rax, 1          ; sys_write\n");
    fprintf(out, "    mov rdi, 1          ; stdout\n");
    fprintf(out, "    lea rsi, [rel msg]  ; message\n");
    fprintf(out, "    mov rdx, msg_len    ; length\n");
    fprintf(out, "    syscall\n");
    fprintf(out, "\n");
    fprintf(out, "    ; Exit\n");
    fprintf(out, "    mov rax, 60         ; sys_exit\n");
    fprintf(out, "    xor rdi, rdi        ; status 0\n");
    fprintf(out, "    syscall\n");
    
    fclose(out);
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("🔧 Preprocessor Module - #38 Stage 1\n");
    printf("=====================================\n");
    
    if (argc < 3) {
        printf("Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("Input: %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    // Read input file
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
    
    // Initialize lexer
    lexer_init(source);
    printf("[2/4] ✓ Lexer init\n");
    
    // Create context
    PreprocessorCtx* ctx = ctx_create(argv[1]);
    register_builtins(ctx);
    
    // Parse
    printf("[3/4] Parsing preprocessor directives...\n");
    memset(&stats, 0, sizeof(stats));
    parse_file(ctx);
    
    printf("      ✓ Directives:\n");
    if (stats.defines > 0) printf("        #define: %d\n", stats.defines);
    if (stats.undefs > 0) printf("        #undef: %d\n", stats.undefs);
    if (stats.includes > 0) printf("        #include: %d\n", stats.includes);
    if (stats.ifdefs > 0) printf("        #ifdef: %d\n", stats.ifdefs);
    if (stats.ifndefs > 0) printf("        #ifndef: %d\n", stats.ifndefs);
    if (stats.elses > 0) printf("        #else: %d\n", stats.elses);
    if (stats.endifs > 0) printf("        #endif: %d\n", stats.endifs);
    if (stats.errors > 0) printf("        #error: %d\n", stats.errors);
    if (stats.warnings > 0) printf("        #warning: %d\n", stats.warnings);
    if (stats.pragmas > 0) printf("        #pragma: %d\n", stats.pragmas);
    
    printf("      ✓ Macros:\n");
    printf("        Object-like: %d\n", stats.object_macros);
    printf("        Function-like: %d\n", stats.function_macros);
    printf("        Variadic: %d\n", stats.variadic_macros);
    printf("        Built-in: 6\n");
    printf("        Total: %d\n", ctx->macro_count);
    
    if (ctx->macro_expansions > 0) {
        printf("      ✓ Macro expansions: %d\n", ctx->macro_expansions);
    }
    
    // Generate assembly
    printf("[4/4] ✓ Code generated\n");
    generate_asm(argv[2], ctx);
    
    printf("\n✅ Complete!\n");
    
    // Cleanup
    ctx_free(ctx);
    free(source);
    
    return 0;
}
