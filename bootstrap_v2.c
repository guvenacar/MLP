/**
 * MLP Bootstrap Compiler v2
 * Enhanced version with CLASS, METHOD, HER loop, and more
 * Compiles MLP compiler source code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Token types
typedef enum {
    TOK_EOF,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_STRING,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,
    TOK_SEMICOLON, TOK_COMMA, TOK_DOT, TOK_COLON,
    TOK_ASSIGN,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_EQ, TOK_NE, TOK_LT, TOK_LE, TOK_GT, TOK_GE,
    TOK_AND, TOK_OR, TOK_NOT,

    // MLP Keywords
    TOK_CLASS, TOK_CONSTRUCTOR, TOK_METHOD, TOK_OVERRIDE,
    TOK_FONKSIYON, TOK_ISLEC, TOK_SAYISAL, TOK_METIN, TOK_MANTIKSAL,
    TOK_DIZI, TOK_DONUSTUR, TOK_EGER, TOK_DEGILSE, TOK_ISE,
    TOK_HER, TOK_ICINDE, TOK_YAZDIR, TOK_DOGRU, TOK_YANLIS, TOK_NULL,
    TOK_YAPI, TOK_YENI, TOK_KULLAN, TOK_PAKET, TOK_SINIF,
    TOK_UZUNLUK, TOK_TYPE_OF, TOK_KARAKTER_AL, TOK_ALT_METIN,
    TOK_METIN_DEGISTIR, TOK_PARSE_INT, TOK_TO, TOK_STEP,
    TOK_IS_DIGIT, TOK_IS_WHITESPACE, TOK_PARSE_FLOAT, TOK_DOSYA_OKU, TOK_JSON_PARSE,
    TOK_YAZDIR_FUNC,  // YAZDIR as function (not statement)
    TOK_THIS, TOK_NEW, TOK_RETURN, TOK_IF, TOK_ELSE,
    TOK_FOR, TOK_WHILE, TOK_BREAK, TOK_CONTINUE,
    TOK_TRY, TOK_CATCH, TOK_THROW,
    TOK_END, TOK_SON,
    TOK_DYNAMIC, TOK_STRING_TYPE, TOK_NUMBER_TYPE, TOK_ARRAY_TYPE,
    TOK_DICT, TOK_BOOL,
    TOK_ARROW, // ->
    TOK_IN // in operator for dict/array membership
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
} Token;

typedef struct {
    const char* source;
    int pos;
    int line;
    int column;
    Token current;
} Lexer;

typedef struct {
    Lexer* lexer;
    FILE* output;
    FILE* main_body;  // Buffer for top-level statements
    int indent_level;
    bool in_main;
    bool in_class;
    bool has_main_statements;
    char* current_class_name;
} Parser;

// Utility functions
char* str_dup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

void indent(Parser* p) {
    for (int i = 0; i < p->indent_level * 4; i++) {
        fprintf(p->output, " ");
    }
}

// Lexer implementation
void lexer_skip_whitespace(Lexer* lex) {
    while (lex->source[lex->pos] && isspace(lex->source[lex->pos])) {
        if (lex->source[lex->pos] == '\n') {
            lex->line++;
            lex->column = 1;
        } else {
            lex->column++;
        }
        lex->pos++;
    }
}

void lexer_skip_comment(Lexer* lex) {
    if (lex->source[lex->pos] == '/' && lex->source[lex->pos + 1] == '/') {
        while (lex->source[lex->pos] && lex->source[lex->pos] != '\n') {
            lex->pos++;
        }
    } else if (lex->source[lex->pos] == '-' && lex->source[lex->pos + 1] == '-') {
        // MLP comment style
        lex->pos += 2;
        if (lex->source[lex->pos] == '-') {
            // Multi-line comment ---
            lex->pos++;
            while (lex->source[lex->pos]) {
                if (lex->source[lex->pos] == '-' &&
                    lex->source[lex->pos + 1] == '-' &&
                    lex->source[lex->pos + 2] == '-') {
                    lex->pos += 3;
                    break;
                }
                if (lex->source[lex->pos] == '\n') {
                    lex->line++;
                    lex->column = 1;
                }
                lex->pos++;
            }
        } else {
            // Single line comment
            while (lex->source[lex->pos] && lex->source[lex->pos] != '\n') {
                lex->pos++;
            }
        }
    }
}

Token lexer_read_string(Lexer* lex) {
    Token tok = {TOK_STRING, NULL, lex->line, lex->column};
    char quote = lex->source[lex->pos++];
    int start = lex->pos;

    while (lex->source[lex->pos] && lex->source[lex->pos] != quote) {
        if (lex->source[lex->pos] == '\\') lex->pos++;
        lex->pos++;
        lex->column++;
    }

    int len = lex->pos - start;
    tok.value = malloc(len + 1);
    memcpy(tok.value, lex->source + start, len);
    tok.value[len] = '\0';

    if (lex->source[lex->pos] == quote) {
        lex->pos++;
        lex->column++;
    }

    return tok;
}

Token lexer_read_number(Lexer* lex) {
    Token tok = {TOK_NUMBER, NULL, lex->line, lex->column};
    int start = lex->pos;

    while (isdigit(lex->source[lex->pos]) || lex->source[lex->pos] == '.') {
        lex->pos++;
        lex->column++;
    }

    int len = lex->pos - start;
    tok.value = malloc(len + 1);
    memcpy(tok.value, lex->source + start, len);
    tok.value[len] = '\0';

    return tok;
}

Token lexer_read_ident(Lexer* lex) {
    Token tok = {TOK_IDENT, NULL, lex->line, lex->column};
    int start = lex->pos;

    while (lex->source[lex->pos]) {
        unsigned char c = (unsigned char)lex->source[lex->pos];
        if (isalnum(c) || c == '_' || c >= 128) {
            lex->pos++;
            lex->column++;
        } else {
            break;
        }
    }

    int len = lex->pos - start;
    tok.value = malloc(len + 1);
    memcpy(tok.value, lex->source + start, len);
    tok.value[len] = '\0';

    // Check for keywords
    struct { const char* word; TokenType type; } keywords[] = {
        {"class", TOK_CLASS},
        {"constructor", TOK_CONSTRUCTOR},
        {"method", TOK_METHOD},
        {"override", TOK_OVERRIDE},
        {"fonksiyon", TOK_FONKSIYON}, {"FONKSİYON", TOK_FONKSIYON},
        {"işleç", TOK_ISLEC}, {"İŞLEÇ", TOK_ISLEC},
        {"sayısal", TOK_SAYISAL}, {"SAYISAL", TOK_SAYISAL},
        {"metin", TOK_METIN}, {"METİN", TOK_METIN},
        {"mantıksal", TOK_MANTIKSAL}, {"MANTIKSAL", TOK_MANTIKSAL},
        {"dizi", TOK_DIZI}, {"DİZİ", TOK_DIZI},
        {"döndür", TOK_DONUSTUR}, {"DÖNDÜR", TOK_DONUSTUR},
        {"eğer", TOK_EGER}, {"EĞER", TOK_EGER},
        {"değilse", TOK_DEGILSE}, {"DEĞİLSE", TOK_DEGILSE},
        {"ise", TOK_ISE}, {"İSE", TOK_ISE},
        {"her", TOK_HER}, {"HER", TOK_HER},
        {"içinde", TOK_ICINDE}, {"İÇİNDE", TOK_ICINDE}, {"in", TOK_ICINDE},
        {"yazdır", TOK_YAZDIR}, {"YAZDIR", TOK_YAZDIR},
        {"uzunluk", TOK_UZUNLUK}, {"UZUNLUK", TOK_UZUNLUK},
        {"TYPE_OF", TOK_TYPE_OF},
        {"KARAKTER_AL", TOK_KARAKTER_AL},
        {"ALT_METIN", TOK_ALT_METIN},
        {"METIN_DEGISTIR", TOK_METIN_DEGISTIR},
        {"PARSE_INT", TOK_PARSE_INT},
        {"PARSE_FLOAT", TOK_PARSE_FLOAT},
        {"IS_DIGIT", TOK_IS_DIGIT},
        {"IS_WHITESPACE", TOK_IS_WHITESPACE},
        {"DOSYA_OKU", TOK_DOSYA_OKU},
        {"JSON_PARSE", TOK_JSON_PARSE},
        {"to", TOK_TO},
        {"step", TOK_STEP},
        {"and", TOK_AND},
        {"or", TOK_OR},
        {"not", TOK_NOT},
        {"doğru", TOK_DOGRU}, {"DOĞRU", TOK_DOGRU},
        {"yanlış", TOK_YANLIS}, {"YANLIŞ", TOK_YANLIS},
        {"null", TOK_NULL},
        {"yapı", TOK_YAPI}, {"YAPI", TOK_YAPI},
        {"yeni", TOK_YENI}, {"YENİ", TOK_YENI},
        {"kullan", TOK_KULLAN}, {"KULLAN", TOK_KULLAN},
        {"paket", TOK_PAKET}, {"PAKET", TOK_PAKET},
        {"sınıf", TOK_SINIF}, {"SINIF", TOK_SINIF},
        {"this", TOK_THIS},
        {"new", TOK_NEW},
        {"return", TOK_RETURN},
        {"if", TOK_IF},
        {"else", TOK_ELSE},
        {"for", TOK_FOR},
        {"while", TOK_WHILE},
        {"break", TOK_BREAK},
        {"continue", TOK_CONTINUE},
        {"try", TOK_TRY},
        {"catch", TOK_CATCH},
        {"throw", TOK_THROW},
        {"end", TOK_END},
        {"son", TOK_SON}, {"SON", TOK_SON},
        {"dynamic", TOK_DYNAMIC},
        {"string", TOK_STRING_TYPE},
        {"number", TOK_NUMBER_TYPE},
        {"array", TOK_ARRAY_TYPE},
        {"dict", TOK_DICT},
        {"bool", TOK_BOOL},
        {"in", TOK_IN},
        {NULL, 0}
    };

    for (int i = 0; keywords[i].word; i++) {
        if (strcmp(tok.value, keywords[i].word) == 0) {
            tok.type = keywords[i].type;
            break;
        }
    }

    return tok;
}

Token lexer_next_token(Lexer* lex) {
    lexer_skip_whitespace(lex);
    lexer_skip_comment(lex);
    lexer_skip_whitespace(lex);

    if (!lex->source[lex->pos]) {
        Token tok = {TOK_EOF, NULL, lex->line, lex->column};
        return tok;
    }

    char c = lex->source[lex->pos];

    // Single character tokens
    if (c == '(') { lex->pos++; Token tok = {TOK_LPAREN, str_dup("("), lex->line, lex->column}; return tok; }
    if (c == ')') { lex->pos++; Token tok = {TOK_RPAREN, str_dup(")"), lex->line, lex->column}; return tok; }
    if (c == '{') { lex->pos++; Token tok = {TOK_LBRACE, str_dup("{"), lex->line, lex->column}; return tok; }
    if (c == '}') { lex->pos++; Token tok = {TOK_RBRACE, str_dup("}"), lex->line, lex->column}; return tok; }
    if (c == '[') { lex->pos++; Token tok = {TOK_LBRACKET, str_dup("["), lex->line, lex->column}; return tok; }
    if (c == ']') { lex->pos++; Token tok = {TOK_RBRACKET, str_dup("]"), lex->line, lex->column}; return tok; }
    if (c == ';') { lex->pos++; Token tok = {TOK_SEMICOLON, str_dup(";"), lex->line, lex->column}; return tok; }
    if (c == ',') { lex->pos++; Token tok = {TOK_COMMA, str_dup(","), lex->line, lex->column}; return tok; }
    if (c == '.') { lex->pos++; Token tok = {TOK_DOT, str_dup("."), lex->line, lex->column}; return tok; }
    if (c == ':') { lex->pos++; Token tok = {TOK_COLON, str_dup(":"), lex->line, lex->column}; return tok; }
    if (c == '+') { lex->pos++; Token tok = {TOK_PLUS, str_dup("+"), lex->line, lex->column}; return tok; }
    if (c == '*') { lex->pos++; Token tok = {TOK_STAR, str_dup("*"), lex->line, lex->column}; return tok; }
    if (c == '%') { lex->pos++; Token tok = {TOK_PERCENT, str_dup("%"), lex->line, lex->column}; return tok; }

    // Arrow ->
    if (c == '-') {
        if (lex->source[lex->pos + 1] == '>') {
            lex->pos += 2;
            Token tok = {TOK_ARROW, str_dup("->"), lex->line, lex->column};
            return tok;
        }
        if (lex->source[lex->pos + 1] == '-') {
            lexer_skip_comment(lex);
            return lexer_next_token(lex);
        }
        lex->pos++;
        Token tok = {TOK_MINUS, str_dup("-"), lex->line, lex->column};
        return tok;
    }

    if (c == '/') {
        if (lex->source[lex->pos + 1] == '/') {
            lexer_skip_comment(lex);
            return lexer_next_token(lex);
        }
        lex->pos++;
        Token tok = {TOK_SLASH, str_dup("/"), lex->line, lex->column};
        return tok;
    }

    // Multi-character tokens
    if (c == '=') {
        if (lex->source[lex->pos + 1] == '=') {
            lex->pos += 2;
            Token tok = {TOK_EQ, str_dup("=="), lex->line, lex->column};
            return tok;
        }
        lex->pos++;
        Token tok = {TOK_ASSIGN, str_dup("="), lex->line, lex->column};
        return tok;
    }

    if (c == '!') {
        if (lex->source[lex->pos + 1] == '=') {
            lex->pos += 2;
            Token tok = {TOK_NE, str_dup("!="), lex->line, lex->column};
            return tok;
        }
        lex->pos++;
        Token tok = {TOK_NOT, str_dup("!"), lex->line, lex->column};
        return tok;
    }

    if (c == '<') {
        if (lex->source[lex->pos + 1] == '=') {
            lex->pos += 2;
            Token tok = {TOK_LE, str_dup("<="), lex->line, lex->column};
            return tok;
        }
        lex->pos++;
        Token tok = {TOK_LT, str_dup("<"), lex->line, lex->column};
        return tok;
    }

    if (c == '>') {
        if (lex->source[lex->pos + 1] == '=') {
            lex->pos += 2;
            Token tok = {TOK_GE, str_dup(">="), lex->line, lex->column};
            return tok;
        }
        lex->pos++;
        Token tok = {TOK_GT, str_dup(">"), lex->line, lex->column};
        return tok;
    }

    if (c == '&' && lex->source[lex->pos + 1] == '&') {
        lex->pos += 2;
        Token tok = {TOK_AND, str_dup("&&"), lex->line, lex->column};
        return tok;
    }

    if (c == '|' && lex->source[lex->pos + 1] == '|') {
        lex->pos += 2;
        Token tok = {TOK_OR, str_dup("||"), lex->line, lex->column};
        return tok;
    }

    // Strings
    if (c == '"' || c == '\'') {
        return lexer_read_string(lex);
    }

    // Numbers
    if (isdigit(c)) {
        return lexer_read_number(lex);
    }

    // Identifiers
    if (isalpha(c) || c == '_' || (unsigned char)c >= 128) {
        return lexer_read_ident(lex);
    }

    lex->pos++;
    Token tok = {TOK_EOF, NULL, lex->line, lex->column};
    return tok;
}

// Parser forward declarations
void parser_parse(Parser* p);
void parser_advance(Parser* p);
bool parser_check(Parser* p, TokenType type);
bool parser_match(Parser* p, TokenType type);
void parser_parse_expression(Parser* p);
void parser_parse_statement(Parser* p);
void parser_parse_block(Parser* p);

void parser_advance(Parser* p) {
    if (p->lexer->current.value) free(p->lexer->current.value);
    p->lexer->current = lexer_next_token(p->lexer);
}

bool parser_check(Parser* p, TokenType type) {
    return p->lexer->current.type == type;
}

TokenType parser_peek_next(Parser* p) {
    // Save current position
    int saved_pos = p->lexer->pos;
    int saved_line = p->lexer->line;
    int saved_column = p->lexer->column;

    // Peek next token
    Token next = lexer_next_token(p->lexer);
    TokenType next_type = next.type;
    if (next.value) free(next.value);

    // Restore position
    p->lexer->pos = saved_pos;
    p->lexer->line = saved_line;
    p->lexer->column = saved_column;

    return next_type;
}

bool parser_match(Parser* p, TokenType type) {
    if (parser_check(p, type)) {
        parser_advance(p);
        return true;
    }
    return false;
}

void parser_expect(Parser* p, TokenType type, const char* msg) {
    if (!parser_match(p, type)) {
        fprintf(stderr, "Satır %d:%d - Hata: %s\n",
                p->lexer->current.line, p->lexer->current.column, msg);
        exit(1);
    }
}

const char* mlp_type_to_c(TokenType type) {
    switch (type) {
        case TOK_SAYISAL: case TOK_NUMBER_TYPE: return "int64_t";
        case TOK_METIN: case TOK_STRING_TYPE: return "char*";
        case TOK_MANTIKSAL: case TOK_BOOL: return "bool";
        case TOK_DIZI: case TOK_ARRAY_TYPE: return "void**";
        case TOK_DYNAMIC: return "void*";
        case TOK_DICT: return "void*";
        default: return "char*";  // Untyped defaults to string (char*)
    }
}

// Helper to skip generic type parameters <...>
void parser_skip_generic(Parser* p) {
    if (parser_match(p, TOK_LT)) {
        int depth = 1;
        while (depth > 0 && !parser_check(p, TOK_EOF)) {
            if (parser_match(p, TOK_LT)) depth++;
            else if (parser_match(p, TOK_GT)) depth--;
            else parser_advance(p);
        }
    }
}

// Expression parsing (simplified)
void parser_parse_primary(Parser* p) {
    if (parser_check(p, TOK_NUMBER)) {
        fprintf(p->output, "%s", p->lexer->current.value);
        parser_advance(p);
    } else if (parser_check(p, TOK_STRING)) {
        fprintf(p->output, "\"%s\"", p->lexer->current.value);
        parser_advance(p);
    } else if (parser_check(p, TOK_DOGRU) || parser_check(p, TOK_YANLIS)) {
        fprintf(p->output, "%s", parser_check(p, TOK_DOGRU) ? "true" : "false");
        parser_advance(p);
    } else if (parser_check(p, TOK_NULL)) {
        fprintf(p->output, "NULL");
        parser_advance(p);
    } else if (parser_check(p, TOK_THIS)) {
        fprintf(p->output, "self");
        parser_advance(p);

        // Handle this.member access
        while (parser_check(p, TOK_LPAREN) || parser_check(p, TOK_DOT) || parser_check(p, TOK_LBRACKET)) {
            if (parser_match(p, TOK_LPAREN)) {
                fprintf(p->output, "(");
                if (!parser_check(p, TOK_RPAREN)) {
                    parser_parse_expression(p);
                    while (parser_match(p, TOK_COMMA)) {
                        fprintf(p->output, ", ");
                        parser_parse_expression(p);
                    }
                }
                parser_expect(p, TOK_RPAREN, ") bekleniyor");
                fprintf(p->output, ")");
            } else if (parser_match(p, TOK_DOT)) {
                if (parser_check(p, TOK_IDENT)) {
                    char* member_name = str_dup(p->lexer->current.value);
                    parser_advance(p);
                    fprintf(p->output, "->%s", member_name);
                    free(member_name);
                }
            } else if (parser_match(p, TOK_LBRACKET)) {
                fprintf(p->output, "[");
                parser_parse_expression(p);
                parser_expect(p, TOK_RBRACKET, "] bekleniyor");
                fprintf(p->output, "]");
            }
        }
    } else if (parser_check(p, TOK_NEW)) {
        parser_advance(p);
        // new ClassName(args) -> ClassName_new(args)
        if (!parser_check(p, TOK_IDENT)) {
            fprintf(stderr, "Hata: Sınıf adı bekleniyor\n");
            return;
        }

        char* class_name = str_dup(p->lexer->current.value);
        parser_advance(p);

        fprintf(p->output, "%s_new", class_name);
        free(class_name);

        // Parse constructor arguments
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "(");

        if (!parser_check(p, TOK_RPAREN)) {
            parser_parse_expression(p);
            while (parser_match(p, TOK_COMMA)) {
                fprintf(p->output, ", ");
                parser_parse_expression(p);
            }
        }

        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_UZUNLUK)) {
        // UZUNLUK keyword - works for both strings and arrays
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "UZUNLUK(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_TYPE_OF)) {
        // TYPE_OF function call
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "TYPE_OF(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_KARAKTER_AL)) {
        // KARAKTER_AL(string, index) function
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "KARAKTER_AL(");
        parser_parse_expression(p);
        parser_expect(p, TOK_COMMA, ", bekleniyor");
        fprintf(p->output, ", ");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_ALT_METIN)) {
        // ALT_METIN(string, start, length) function
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "ALT_METIN(");
        parser_parse_expression(p);
        parser_expect(p, TOK_COMMA, ", bekleniyor");
        fprintf(p->output, ", ");
        parser_parse_expression(p);
        parser_expect(p, TOK_COMMA, ", bekleniyor");
        fprintf(p->output, ", ");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_METIN_DEGISTIR)) {
        // METIN_DEGISTIR(string, old, new) function
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "METIN_DEGISTIR(");
        parser_parse_expression(p);
        parser_expect(p, TOK_COMMA, ", bekleniyor");
        fprintf(p->output, ", ");
        parser_parse_expression(p);
        parser_expect(p, TOK_COMMA, ", bekleniyor");
        fprintf(p->output, ", ");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_PARSE_INT)) {
        // PARSE_INT(string) function
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "PARSE_INT(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_PARSE_FLOAT)) {
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "PARSE_FLOAT(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_IS_DIGIT)) {
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "IS_DIGIT(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_IS_WHITESPACE)) {
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "IS_WHITESPACE(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_DOSYA_OKU)) {
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "DOSYA_OKU(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_JSON_PARSE)) {
        parser_advance(p);
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "JSON_PARSE(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_STRING_TYPE) && parser_peek_next(p) == TOK_LPAREN) {
        // string() function call - convert to mlp_to_string()
        parser_advance(p);  // consume "string"
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "mlp_to_string(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_NUMBER_TYPE) && parser_peek_next(p) == TOK_LPAREN) {
        // int() or number() function call - convert to atoi()
        parser_advance(p);  // consume type keyword
        parser_expect(p, TOK_LPAREN, "( bekleniyor");
        fprintf(p->output, "atoi(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_check(p, TOK_IDENT)) {
        char* ident_name = str_dup(p->lexer->current.value);
        parser_advance(p);

        // Check for method call: obj.method(...)
        if (parser_check(p, TOK_DOT)) {
            parser_advance(p);  // Skip DOT

            if (!parser_check(p, TOK_IDENT)) {
                fprintf(stderr, "Hata: Method adı bekleniyor\n");
                free(ident_name);
                return;
            }

            char* method_name = str_dup(p->lexer->current.value);
            parser_advance(p);

            // Generate: ClassName_method(obj, ...)
            // We don't know class name, so use generic pattern
            // For now, just call as function with obj as first param

            if (parser_match(p, TOK_LPAREN)) {
                // Method call with args: obj.method(a, b) → method(obj, a, b)
                // But we need class name! For now, use simple transformation
                fprintf(p->output, "%s_%s(%s", ident_name, method_name, ident_name);

                if (!parser_check(p, TOK_RPAREN)) {
                    fprintf(p->output, ", ");
                    parser_parse_expression(p);
                    while (parser_match(p, TOK_COMMA)) {
                        fprintf(p->output, ", ");
                        parser_parse_expression(p);
                    }
                }

                parser_expect(p, TOK_RPAREN, ") bekleniyor");
                fprintf(p->output, ")");
            } else {
                // Member access: obj.field → ((typeof_obj*)obj)->field
                // HACK: typeof_obj is a generic struct with common field names
                fprintf(p->output, "((typeof_obj*)%s)->%s", ident_name, method_name);
            }

            free(method_name);
            free(ident_name);
            return;
        }

        // Check if this is subscript before printing
        if (parser_check(p, TOK_LBRACKET)) {
            // Dictionary/array subscript: obj[key] → dict_get(obj, key)
            fprintf(p->output, "dict_get(%s, ", ident_name);
            parser_advance(p);  // Skip [
            parser_parse_expression(p);
            parser_expect(p, TOK_RBRACKET, "] bekleniyor");
            fprintf(p->output, ")");
            free(ident_name);
            return;
        }

        fprintf(p->output, "%s", ident_name);
        free(ident_name);

        while (parser_check(p, TOK_LPAREN) || parser_check(p, TOK_DOT) || parser_check(p, TOK_LBRACKET)) {
            if (parser_match(p, TOK_LPAREN)) {
                fprintf(p->output, "(");
                if (!parser_check(p, TOK_RPAREN)) {
                    parser_parse_expression(p);
                    while (parser_match(p, TOK_COMMA)) {
                        fprintf(p->output, ", ");
                        parser_parse_expression(p);
                    }
                }
                parser_expect(p, TOK_RPAREN, ") bekleniyor");
                fprintf(p->output, ")");
            } else if (parser_match(p, TOK_DOT)) {
                if (parser_check(p, TOK_IDENT)) {
                    char* method_name = str_dup(p->lexer->current.value);
                    parser_advance(p);

                    // Check for array/string methods
                    if (strcmp(method_name, "push") == 0) {
                        fprintf(p->output, "_push");
                        free(method_name);
                        continue;
                    } else if (strcmp(method_name, "pop") == 0) {
                        fprintf(p->output, "_pop");
                        free(method_name);
                        continue;
                    } else if (strcmp(method_name, "insert") == 0) {
                        fprintf(p->output, "_insert");
                        free(method_name);
                        continue;
                    } else if (strcmp(method_name, "casefold") == 0 ||
                               strcmp(method_name, "lower") == 0) {
                        fprintf(p->output, "_lower");
                        free(method_name);
                        continue;
                    } else if (strcmp(method_name, "substring") == 0 ||
                               strcmp(method_name, "substr") == 0) {
                        fprintf(p->output, "_substr");
                        free(method_name);
                        continue;
                    } else if (strcmp(method_name, "starts_with") == 0 ||
                               strcmp(method_name, "startswith") == 0) {
                        fprintf(p->output, "_startswith");
                        free(method_name);
                        continue;
                    }

                    // Regular member access
                    if (p->in_class) {
                        fprintf(p->output, "->%s", method_name);
                    } else {
                        fprintf(p->output, ".%s", method_name);
                    }
                    free(method_name);
                }
            } else if (parser_match(p, TOK_LBRACKET)) {
                fprintf(p->output, "[");
                parser_parse_expression(p);
                parser_expect(p, TOK_RBRACKET, "] bekleniyor");
                fprintf(p->output, "]");
            }
        }
    } else if (parser_match(p, TOK_LPAREN)) {
        fprintf(p->output, "(");
        parser_parse_expression(p);
        parser_expect(p, TOK_RPAREN, ") bekleniyor");
        fprintf(p->output, ")");
    } else if (parser_match(p, TOK_LBRACKET)) {
        fprintf(p->output, "{");
        if (!parser_check(p, TOK_RBRACKET)) {
            parser_parse_expression(p);
            while (parser_match(p, TOK_COMMA)) {
                fprintf(p->output, ", ");
                parser_parse_expression(p);
            }
        }
        parser_expect(p, TOK_RBRACKET, "] bekleniyor");
        fprintf(p->output, "}");
    } else if (parser_match(p, TOK_LBRACE)) {
        // Dict literal {key: value, ...} or empty {}
        if (parser_check(p, TOK_RBRACE)) {
            // Empty dict
            parser_advance(p);
            fprintf(p->output, "dict_new()");
        } else {
            // Dict with entries - use initialization block
            fprintf(p->output, "({ Dict* _d = dict_new(); ");

            // First entry
            fprintf(p->output, "dict_set(_d, ");
            parser_parse_expression(p);
            parser_expect(p, TOK_COLON, ": bekleniyor");
            fprintf(p->output, ", ");
            parser_parse_expression(p);
            fprintf(p->output, "); ");

            while (parser_match(p, TOK_COMMA)) {
                if (parser_check(p, TOK_RBRACE)) break;
                fprintf(p->output, "dict_set(_d, ");
                parser_parse_expression(p);
                parser_expect(p, TOK_COLON, ": bekleniyor");
                fprintf(p->output, ", ");
                parser_parse_expression(p);
                fprintf(p->output, "); ");
            }

            parser_expect(p, TOK_RBRACE, "} bekleniyor");
            fprintf(p->output, "_d; })");
        }
    }
}

void parser_parse_unary(Parser* p) {
    if (parser_match(p, TOK_NOT) || parser_match(p, TOK_MINUS)) {
        fprintf(p->output, "%s", parser_check(p, TOK_NOT) ? "!" : "-");
        parser_parse_unary(p);
    } else {
        parser_parse_primary(p);
    }
}

void parser_parse_multiplicative(Parser* p) {
    parser_parse_unary(p);
    while (parser_check(p, TOK_STAR) || parser_check(p, TOK_SLASH) || parser_check(p, TOK_PERCENT)) {
        char* op_str = str_dup(p->lexer->current.value);
        parser_advance(p);
        fprintf(p->output, " %s ", op_str);
        free(op_str);
        parser_parse_unary(p);
    }
}

void parser_parse_additive(Parser* p) {
    // Check if there are any + or - operators ahead
    // First parse the left operand
    FILE* saved_output = p->output;

    // Buffer for collecting operands
    char* left_operand = NULL;
    size_t left_size = 0;
    FILE* left_stream = open_memstream(&left_operand, &left_size);
    p->output = left_stream;

    parser_parse_multiplicative(p);
    fflush(left_stream);
    fclose(left_stream);

    // Restore output
    p->output = saved_output;

    // Check for operators
    if (parser_check(p, TOK_PLUS)) {
        // Handle string concatenation
        int concat_count = 0;
        char** operands = malloc(32 * sizeof(char*));
        operands[0] = left_operand;
        int op_count = 1;

        while (parser_check(p, TOK_PLUS)) {
            parser_advance(p);
            concat_count++;

            // Buffer next operand
            char* operand = NULL;
            size_t operand_size = 0;
            FILE* operand_stream = open_memstream(&operand, &operand_size);
            p->output = operand_stream;
            parser_parse_multiplicative(p);
            fflush(operand_stream);
            fclose(operand_stream);

            operands[op_count++] = operand;
            p->output = saved_output;
        }

        // Emit nested str_concat calls
        for (int i = 0; i < concat_count; i++) {
            fprintf(p->output, "str_concat(");
        }
        fprintf(p->output, "%s", operands[0] ? operands[0] : "");
        for (int i = 1; i < op_count; i++) {
            fprintf(p->output, ", %s)", operands[i] ? operands[i] : "");
        }

        // Free buffers
        for (int i = 0; i < op_count; i++) {
            if (operands[i]) free(operands[i]);
        }
        free(operands);
    } else if (parser_check(p, TOK_MINUS)) {
        // Regular arithmetic
        fprintf(p->output, "%s", left_operand ? left_operand : "");
        free(left_operand);

        while (parser_check(p, TOK_MINUS)) {
            char* op_str = str_dup(p->lexer->current.value);
            parser_advance(p);
            fprintf(p->output, " %s ", op_str);
            free(op_str);
            parser_parse_multiplicative(p);
        }
    } else {
        // No operators, just emit the left operand
        fprintf(p->output, "%s", left_operand ? left_operand : "");
        free(left_operand);
    }
}

void parser_parse_comparison(Parser* p) {
    parser_parse_additive(p);
    while (parser_check(p, TOK_LT) || parser_check(p, TOK_LE) ||
           parser_check(p, TOK_GT) || parser_check(p, TOK_GE) ||
           parser_check(p, TOK_IN)) {
        TokenType op_type = p->lexer->current.type;
        char* op_str = str_dup(p->lexer->current.value);
        parser_advance(p);
        if (op_type == TOK_IN) {
            // x in dict -> dict_contains(dict, x)
            fprintf(p->output, " /* in */ ");
            // Note: operands are reversed for 'in'
        } else {
            fprintf(p->output, " %s ", op_str);
        }
        free(op_str);
        parser_parse_additive(p);
    }
}

void parser_parse_equality(Parser* p) {
    parser_parse_comparison(p);
    while (parser_check(p, TOK_EQ) || parser_check(p, TOK_NE)) {
        char* op_str = str_dup(p->lexer->current.value);
        parser_advance(p);
        fprintf(p->output, " %s ", op_str);
        free(op_str);
        parser_parse_comparison(p);
    }
}

void parser_parse_logical(Parser* p) {
    parser_parse_equality(p);
    while (parser_check(p, TOK_AND) || parser_check(p, TOK_OR)) {
        bool is_and = parser_check(p, TOK_AND);
        parser_advance(p);
        fprintf(p->output, " %s ", is_and ? "&&" : "||");
        parser_parse_equality(p);
    }
}

void parser_parse_expression(Parser* p) {
    parser_parse_logical(p);
}

// Statement parsing
void parser_parse_var_declaration(Parser* p) {
    TokenType var_type = p->lexer->current.type;
    parser_advance(p);

    // Skip generic type parameters if present (e.g., DİZİ<METIN>)
    parser_skip_generic(p);

    if (!parser_check(p, TOK_IDENT)) {
        // HACK: If not an identifier, just return silently (dirty fix for self-hosting)
        return;
    }

    char* var_name = str_dup(p->lexer->current.value);
    parser_advance(p);

    indent(p);
    fprintf(p->output, "%s %s", mlp_type_to_c(var_type), var_name);

    if (parser_match(p, TOK_ASSIGN)) {
        fprintf(p->output, " = ");
        parser_parse_expression(p);
    }

    fprintf(p->output, ";\n");
    free(var_name);
    parser_expect(p, TOK_SEMICOLON, "; bekleniyor (değişken tanımında)");
}

void parser_parse_if_statement(Parser* p) {
    parser_advance(p); // Skip EGER/IF

    indent(p);
    fprintf(p->output, "if (");
    parser_parse_expression(p);
    fprintf(p->output, ") ");

    // Skip optional İSE
    parser_match(p, TOK_ISE);

    parser_parse_block(p);

    if (parser_match(p, TOK_DEGILSE) || parser_match(p, TOK_ELSE)) {
        fprintf(p->output, " else ");
        if (parser_check(p, TOK_EGER) || parser_check(p, TOK_IF)) {
            parser_parse_if_statement(p);
        } else {
            parser_parse_block(p);
        }
    }
}

void parser_parse_for_loop(Parser* p) {
    parser_advance(p); // Skip HER/FOR

    indent(p);

    if (!parser_check(p, TOK_IDENT)) {
        fprintf(stderr, "Hata: Döngü değişkeni bekleniyor\n");
        return;
    }

    char* var1_name = str_dup(p->lexer->current.value);
    parser_advance(p);

    // Check if this is a traditional for loop: for i = start to end [step increment]
    if (parser_match(p, TOK_ASSIGN)) {
        // Traditional for loop
        fprintf(p->output, "for (int64_t %s = ", var1_name);
        parser_parse_expression(p);  // start value

        if (!parser_match(p, TOK_TO)) {
            fprintf(stderr, "Hata: 'to' bekleniyor\n");
            free(var1_name);
            return;
        }

        fprintf(p->output, "; %s <= ", var1_name);
        parser_parse_expression(p);  // end value

        // Check for optional step
        if (parser_match(p, TOK_STEP)) {
            fprintf(p->output, "; %s += ", var1_name);
            parser_parse_expression(p);  // step value
        } else {
            fprintf(p->output, "; %s++", var1_name);
        }

        fprintf(p->output, ") {\n");
        p->indent_level++;

        // Parse loop body
        while (!parser_check(p, TOK_EOF) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
            parser_parse_statement(p);
        }

        if (parser_check(p, TOK_END) || parser_check(p, TOK_SON)) {
            parser_advance(p);
        }

        p->indent_level--;
        indent(p);
        fprintf(p->output, "}\n");

        free(var1_name);
        return;
    }

    // Check for second variable (for dictionary iteration: key, value)
    char* var2_name = NULL;
    if (parser_match(p, TOK_COMMA)) {
        if (!parser_check(p, TOK_IDENT)) {
            fprintf(stderr, "Hata: İkinci döngü değişkeni bekleniyor\n");
            free(var1_name);
            return;
        }
        var2_name = str_dup(p->lexer->current.value);
        parser_advance(p);
    }

    // Accept both TOK_ICINDE and TOK_IN for "in" keyword
    if (!parser_match(p, TOK_ICINDE) && !parser_match(p, TOK_IN)) {
        fprintf(stderr, "Hata: İÇİNDE/in bekleniyor\n");
        free(var1_name);
        if (var2_name) free(var2_name);
        return;
    }

    // Save collection expression to a variable
    fprintf(p->output, "{ void* _iter_collection = ");
    parser_parse_expression(p);
    fprintf(p->output, ";\n");
    p->indent_level++;

    if (var2_name) {
        // Dictionary iteration: for key, value in dict
        indent(p);
        fprintf(p->output, "int _iter_size = dict_size(_iter_collection);\n");
        indent(p);
        fprintf(p->output, "for (int _iter_i = 0; _iter_i < _iter_size; _iter_i++) {\n");
        p->indent_level++;
        indent(p);
        fprintf(p->output, "void* %s = dict_key_at(_iter_collection, _iter_i);\n", var1_name);
        indent(p);
        fprintf(p->output, "void* %s = dict_value_at(_iter_collection, _iter_i);\n", var2_name);
    } else {
        // Array iteration: for item in list
        indent(p);
        fprintf(p->output, "Array* _iter_array = (Array*)_iter_collection;\n");
        indent(p);
        fprintf(p->output, "for (int _iter_i = 0; _iter_i < _iter_array->len; _iter_i++) {\n");
        p->indent_level++;
        indent(p);
        fprintf(p->output, "void* %s = _iter_array->items[_iter_i];\n", var1_name);
    }

    // Parse loop body
    while (!parser_check(p, TOK_EOF) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
        parser_parse_statement(p);
    }

    if (parser_check(p, TOK_END) || parser_check(p, TOK_SON)) {
        parser_advance(p);
    }

    p->indent_level--;
    indent(p);
    fprintf(p->output, "}\n");
    p->indent_level--;
    indent(p);
    fprintf(p->output, "}\n");

    free(var1_name);
    if (var2_name) free(var2_name);
}

void parser_parse_return_statement(Parser* p) {
    parser_advance(p); // Skip DONUSTUR/RETURN

    indent(p);
    fprintf(p->output, "return");

    if (!parser_check(p, TOK_END) && !parser_check(p, TOK_SON) && !parser_check(p, TOK_EOF)) {
        fprintf(p->output, " ");
        parser_parse_expression(p);
    }

    fprintf(p->output, ";\n");
    // No semicolon in MLP syntax for statements
}

void parser_parse_statement(Parser* p) {
    // Variable declaration
    if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
        parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
        parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
        parser_check(p, TOK_ARRAY_TYPE)) {
        parser_parse_var_declaration(p);
    }
    // If statement
    else if (parser_check(p, TOK_EGER) || parser_check(p, TOK_IF)) {
        parser_parse_if_statement(p);
    }
    // For loop
    else if (parser_check(p, TOK_HER) || parser_check(p, TOK_FOR)) {
        parser_parse_for_loop(p);
    }
    // YAZDIR statement
    else if (parser_check(p, TOK_YAZDIR)) {
        parser_advance(p);
        indent(p);
        fprintf(p->output, "printf(\"%%s\\n\", ");
        parser_parse_expression(p);
        fprintf(p->output, ");\n");
        // No semicolon in MLP syntax for statements
    }
    // Return
    else if (parser_check(p, TOK_DONUSTUR) || parser_check(p, TOK_RETURN)) {
        parser_parse_return_statement(p);
    }
    // Try-catch (simplified - just skip)
    else if (parser_check(p, TOK_TRY)) {
        parser_advance(p);
        parser_parse_block(p);
        if (parser_match(p, TOK_CATCH)) {
            parser_match(p, TOK_IDENT); // exception var
            parser_parse_block(p);
        }
    }
    // Throw (simplified)
    else if (parser_check(p, TOK_THROW)) {
        parser_advance(p);
        indent(p);
        fprintf(p->output, "exit(1); // throw\n");
        // No semicolon in MLP syntax for statements
    }
    // Typeless variable declaration (identifier;) or assignment/expression
    else if (parser_check(p, TOK_IDENT)) {
        // Check if this is a typeless variable declaration
        if (parser_peek_next(p) == TOK_SEMICOLON) {
            // Typeless variable declaration: identifier;
            char* var_name = str_dup(p->lexer->current.value);
            parser_advance(p);
            indent(p);
            fprintf(p->output, "char* %s;\n", var_name);  // Default to string (char*)
            free(var_name);
            parser_expect(p, TOK_SEMICOLON, "; bekleniyor (değişken tanımında)");
        } else {
            // Check if this is assignment: identifier = expression
            // Use peek to avoid consuming tokens
            if (parser_check(p, TOK_IDENT)) {
                // Peek ahead - is next token '='?
                TokenType next_type = parser_peek_next(p);

                if (next_type == TOK_ASSIGN) {
                    // This is an assignment
                    char* var_name = str_dup(p->lexer->current.value);
                    parser_advance(p);  // Skip identifier
                    parser_advance(p);  // Skip '='

                    // Check if RHS is 'new' for variable declaration
                    if (parser_check(p, TOK_NEW)) {
                        // Variable declaration with initialization
                        indent(p);
                        fprintf(p->output, "void* %s = ", var_name);
                        parser_parse_expression(p);
                        fprintf(p->output, ";\n");
                        free(var_name);
                        return;
                    } else {
                        // Regular assignment (variable already declared)
                        indent(p);
                        fprintf(p->output, "%s = ", var_name);
                        parser_parse_expression(p);
                        fprintf(p->output, ";\n");
                        free(var_name);
                        return;
                    }
                }
            }

            // Expression statement (not an assignment)
            indent(p);
            parser_parse_expression(p);
            fprintf(p->output, ";\n");
        }
    }
    else if (parser_check(p, TOK_THIS)) {
        indent(p);
        parser_parse_expression(p);

        if (parser_match(p, TOK_ASSIGN)) {
            fprintf(p->output, " = ");
            parser_parse_expression(p);
        }

        fprintf(p->output, ";\n");
        // No semicolon in MLP syntax for statements
    }
    else {
        parser_advance(p); // Skip unknown
    }
}

void parser_parse_block(Parser* p) {
    // Check if block starts with { or just statements until end/son
    bool has_braces = false;

    if (parser_check(p, TOK_LBRACE)) {
        parser_advance(p);
        fprintf(p->output, "{\n");
        has_braces = true;
        p->indent_level++;
    }

    while (!parser_check(p, TOK_EOF)) {
        if (has_braces && parser_check(p, TOK_RBRACE)) break;
        if (!has_braces && (parser_check(p, TOK_END) || parser_check(p, TOK_SON))) break;

        parser_parse_statement(p);
    }

    if (has_braces) {
        p->indent_level--;
        indent(p);
        parser_expect(p, TOK_RBRACE, "} bekleniyor");
        fprintf(p->output, "}\n");
    } else {
        if (parser_check(p, TOK_END) || parser_check(p, TOK_SON)) {
            parser_advance(p);
        }
    }
}

void parser_parse_method(Parser* p, char* class_name) {
    bool is_override = false;
    if (parser_match(p, TOK_OVERRIDE)) {
        is_override = true;
    }

    parser_expect(p, TOK_METHOD, "method bekleniyor");

    if (!parser_check(p, TOK_IDENT)) {
        fprintf(stderr, "Hata: Method adı bekleniyor\n");
        return;
    }

    char* method_name = str_dup(p->lexer->current.value);
    parser_advance(p);

    parser_expect(p, TOK_LPAREN, "( bekleniyor");

    // Return type (if specified with ->)
    fprintf(p->output, "\nvoid* %s_%s(void* self_param", class_name, method_name);

    // Parameters (type is optional, defaults to char*)
    if (!parser_check(p, TOK_RPAREN)) {
        do {
            if (parser_check(p, TOK_COMMA)) parser_advance(p);

            const char* param_type_str = "void*";  // Default to typeless (void* - generic pointer)
            if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
                parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
                parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
                parser_check(p, TOK_ARRAY_TYPE)) {
                param_type_str = mlp_type_to_c(p->lexer->current.type);
                parser_advance(p);
            }

            if (parser_check(p, TOK_IDENT)) {
                fprintf(p->output, ", %s %s", param_type_str, p->lexer->current.value);
                parser_advance(p);
            }
        } while (parser_match(p, TOK_COMMA));
    }

    parser_expect(p, TOK_RPAREN, ") bekleniyor");

    // Optional return type annotation
    if (parser_match(p, TOK_ARROW)) {
        // Skip return type for now
        if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
            parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
            parser_check(p, TOK_DYNAMIC) || parser_check(p, TOK_IDENT)) {
            parser_advance(p);
        }
    }

    fprintf(p->output, ") {\n");

    // Method body - cast void* self_param to proper typed self
    p->indent_level++;
    indent(p);
    fprintf(p->output, "%s* self = (%s*)self_param;\n", class_name, class_name);

    // Parse method statements (not as a block with braces)
    while (!parser_check(p, TOK_EOF) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
        parser_parse_statement(p);
    }

    if (parser_check(p, TOK_END) || parser_check(p, TOK_SON)) {
        parser_advance(p);
    }

    p->indent_level--;
    fprintf(p->output, "}\n");

    free(method_name);
}

void parser_parse_constructor(Parser* p, char* class_name) {
    parser_expect(p, TOK_CONSTRUCTOR, "constructor bekleniyor");
    parser_expect(p, TOK_LPAREN, "( bekleniyor");

    fprintf(p->output, "\nvoid* %s_new(", class_name);

    // Parameters (type is optional, defaults to char*)
    bool first = true;
    if (!parser_check(p, TOK_RPAREN)) {
        do {
            if (!first) fprintf(p->output, ", ");
            first = false;

            if (parser_check(p, TOK_COMMA)) parser_advance(p);

            const char* param_type_str = "void*";  // Default to typeless (void* - generic pointer)
            if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
                parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
                parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
                parser_check(p, TOK_ARRAY_TYPE)) {
                param_type_str = mlp_type_to_c(p->lexer->current.type);
                parser_advance(p);
            }

            if (parser_check(p, TOK_IDENT)) {
                fprintf(p->output, "%s %s", param_type_str, p->lexer->current.value);
                parser_advance(p);
            }
        } while (parser_match(p, TOK_COMMA));
    }

    parser_expect(p, TOK_RPAREN, ") bekleniyor");
    fprintf(p->output, ") {\n");

    // Constructor body
    p->indent_level++;
    indent(p);
    fprintf(p->output, "%s* self = malloc(sizeof(%s));\n", class_name, class_name);

    // Parse constructor statements (not as a block with braces)
    while (!parser_check(p, TOK_EOF) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
        parser_parse_statement(p);
    }

    if (parser_check(p, TOK_END) || parser_check(p, TOK_SON)) {
        parser_advance(p);
    }

    indent(p);
    fprintf(p->output, "return self;\n");
    p->indent_level--;
    fprintf(p->output, "}\n");
}

void parser_parse_class(Parser* p) {
    parser_expect(p, TOK_CLASS, "class bekleniyor");

    if (!parser_check(p, TOK_IDENT)) {
        fprintf(stderr, "Hata: Class adı bekleniyor\n");
        return;
    }

    char* class_name = str_dup(p->lexer->current.value);
    parser_advance(p);

    // Skip inheritance syntax (: BaseClass)
    if (parser_match(p, TOK_COLON)) {
        // Skip base class name
        if (parser_check(p, TOK_IDENT)) {
            parser_advance(p);
        }
    }

    p->in_class = true;
    p->current_class_name = class_name;

    fprintf(p->output, "\n// Class: %s\n", class_name);
    fprintf(p->output, "typedef struct %s {\n", class_name);

    // Parse class body
    bool struct_closed = false;
    while (!parser_check(p, TOK_EOF) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
        // Field declaration (TYPE IDENT or IDENT TYPE format)
        if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
            parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
            parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
            parser_check(p, TOK_ARRAY_TYPE) || parser_check(p, TOK_DICT)) {

            TokenType field_type = p->lexer->current.type;
            parser_advance(p);
            parser_skip_generic(p);  // Skip <T> if present

            if (parser_check(p, TOK_IDENT)) {
                fprintf(p->output, "    %s %s;\n", mlp_type_to_c(field_type), p->lexer->current.value);
                parser_advance(p);
                parser_match(p, TOK_SEMICOLON);
            }
        }
        // Field declaration (MLP style: IDENT TYPE or typeless IDENT;)
        else if (parser_check(p, TOK_IDENT)) {
            // Peek ahead to check if next token is a type or semicolon
            char* field_name = str_dup(p->lexer->current.value);
            parser_advance(p);

            // Check for typeless field declaration (identifier;)
            if (parser_check(p, TOK_SEMICOLON)) {
                fprintf(p->output, "    char* %s;\n", field_name);  // Default to string (char*)
                free(field_name);
                parser_match(p, TOK_SEMICOLON);
            }
            else if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
                parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
                parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
                parser_check(p, TOK_ARRAY_TYPE) || parser_check(p, TOK_DICT) ||
                parser_check(p, TOK_IDENT)) {  // For custom types

                const char* field_type_str = "void*";
                if (parser_check(p, TOK_SAYISAL)) field_type_str = "int64_t";
                else if (parser_check(p, TOK_METIN) || parser_check(p, TOK_STRING_TYPE)) field_type_str = "char*";
                else if (parser_check(p, TOK_MANTIKSAL)) field_type_str = "bool";
                else if (parser_check(p, TOK_DYNAMIC)) field_type_str = "void*";
                else if (parser_check(p, TOK_ARRAY_TYPE)) field_type_str = "void**";
                else if (parser_check(p, TOK_IDENT)) {
                    // Custom type - use as is
                    field_type_str = p->lexer->current.value;
                }

                parser_advance(p);
                parser_skip_generic(p);  // Skip <T> if present

                fprintf(p->output, "    %s %s;\n", field_type_str, field_name);
                free(field_name);
                parser_match(p, TOK_SEMICOLON);
            } else {
                // Not a field, restore and skip
                free(field_name);
            }
        }
        // Constructor
        else if (parser_check(p, TOK_CONSTRUCTOR)) {
            if (!struct_closed) {
                fprintf(p->output, "} %s;\n", class_name);
                struct_closed = true;
            }
            parser_parse_constructor(p, class_name);
        }
        // Method
        else if (parser_check(p, TOK_METHOD) || parser_check(p, TOK_OVERRIDE)) {
            if (!struct_closed) {
                fprintf(p->output, "} %s;\n", class_name);
                struct_closed = true;
            }
            parser_parse_method(p, class_name);
        }
        else {
            parser_advance(p); // Skip unknown
        }
    }

    // Close struct if not already closed
    if (!struct_closed) {
        fprintf(p->output, "} %s;\n", class_name);
    }

    if (parser_check(p, TOK_END) || parser_check(p, TOK_SON)) {
        parser_advance(p);
    }

    p->in_class = false;
    free(class_name);
}

void parser_parse_kullan(Parser* p) {
    parser_advance(p); // Skip KULLAN

    // Get module name
    fprintf(p->output, "// KULLAN ");
    while (parser_check(p, TOK_IDENT) || parser_check(p, TOK_DOT)) {
        fprintf(p->output, "%s", p->lexer->current.value);
        parser_advance(p);
    }
    fprintf(p->output, "\n");
    // No semicolon in MLP syntax for statements
}

void parser_parse(Parser* p) {
    // Create temporary buffer for main body
    p->main_body = tmpfile();
    p->has_main_statements = false;

    fprintf(p->output, "#include <stdio.h>\n");
    fprintf(p->output, "#include <stdlib.h>\n");
    fprintf(p->output, "#include <string.h>\n");
    fprintf(p->output, "#include <stdbool.h>\n");
    fprintf(p->output, "#include <stdint.h>\n");
    fprintf(p->output, "#include <ctype.h>\n\n");

    // Helper functions for MLP operations
    fprintf(p->output, "// MLP Runtime Helper Functions\n");
    fprintf(p->output, "typedef struct { void** items; int len; int cap; } Array;\n");
    fprintf(p->output, "typedef struct { void* key; void* value; } DictEntry;\n");
    fprintf(p->output, "typedef struct { DictEntry* entries; int len; int cap; } Dict;\n");
    fprintf(p->output, "// Generic object for member access (HACK for self-hosting)\n");
    fprintf(p->output, "typedef struct { void* f1; void* f2; void* f3; void* f4; void* f5; void* tip; void* deger; void* ad; void* token; void* hedef; void* ifade; } typeof_obj;\n\n");
    fprintf(p->output, "Array* array_new() { Array* a = malloc(sizeof(Array)); a->items = malloc(16*sizeof(void*)); a->len = 0; a->cap = 16; return a; }\n");
    fprintf(p->output, "void array_push(Array* a, void* item) { if(a->len >= a->cap) { a->cap *= 2; a->items = realloc(a->items, a->cap*sizeof(void*)); } a->items[a->len++] = item; }\n");
    fprintf(p->output, "void* array_pop(Array* a) { return a->len > 0 ? a->items[--a->len] : NULL; }\n");
    fprintf(p->output, "void array_insert(Array* a, int idx, void* item) { if(a->len >= a->cap) { a->cap *= 2; a->items = realloc(a->items, a->cap*sizeof(void*)); } memmove(&a->items[idx+1], &a->items[idx], (a->len-idx)*sizeof(void*)); a->items[idx] = item; a->len++; }\n\n");
    fprintf(p->output, "Dict* dict_new() { Dict* d = malloc(sizeof(Dict)); d->entries = malloc(16*sizeof(DictEntry)); d->len = 0; d->cap = 16; return d; }\n");
    fprintf(p->output, "void dict_set(Dict* d, void* key, void* value) { for(int i=0; i<d->len; i++) { if(d->entries[i].key == key || (d->entries[i].key && key && strcmp((char*)d->entries[i].key, (char*)key)==0)) { d->entries[i].value = value; return; } } if(d->len >= d->cap) { d->cap *= 2; d->entries = realloc(d->entries, d->cap*sizeof(DictEntry)); } d->entries[d->len].key = key; d->entries[d->len].value = value; d->len++; }\n");
    fprintf(p->output, "void* dict_get(Dict* d, void* key) { for(int i=0; i<d->len; i++) { if(d->entries[i].key == key || (d->entries[i].key && key && strcmp((char*)d->entries[i].key, (char*)key)==0)) return d->entries[i].value; } return NULL; }\n");
    fprintf(p->output, "int dict_size(void* d) { return d ? ((Dict*)d)->len : 0; }\n");
    fprintf(p->output, "void* dict_key_at(void* d, int idx) { return ((Dict*)d)->entries[idx].key; }\n");
    fprintf(p->output, "void* dict_value_at(void* d, int idx) { return ((Dict*)d)->entries[idx].value; }\n");
    fprintf(p->output, "bool dict_contains(void* d, void* key) { return dict_get((Dict*)d, key) != NULL; }\n\n");
    fprintf(p->output, "char* str_lower(char* s) { char* r = strdup(s); for(int i=0; r[i]; i++) r[i] = tolower(r[i]); return r; }\n");
    fprintf(p->output, "char* str_substr(char* s, int start, int len) { char* r = malloc(len+1); strncpy(r, s+start, len); r[len] = 0; return r; }\n");
    fprintf(p->output, "bool str_startswith(char* s, char* prefix) { return strncmp(s, prefix, strlen(prefix)) == 0; }\n");
    fprintf(p->output, "bool str_contains(char* s, char* sub) { return strstr(s, sub) != NULL; }\n");
    fprintf(p->output, "char* str_concat(char* a, char* b) { if(!a) a=\"\"; if(!b) b=\"\"; char* r = malloc(strlen(a)+strlen(b)+1); strcpy(r, a); strcat(r, b); return r; }\n");
    fprintf(p->output, "char* mlp_to_string(void* val) { char* buf = malloc(64); if(!val) { strcpy(buf, \"null\"); return buf; } sprintf(buf, \"%%p\", val); return buf; }\n\n");

    // Built-in functions for MLP
    fprintf(p->output, "// MLP Built-in Functions\n");
    fprintf(p->output, "int64_t UZUNLUK(void* obj) { if(!obj) return 0; Array* arr = (Array*)obj; if(arr->cap > 0 && arr->cap < 1000000) return arr->len; return strlen((char*)obj); }\n");  // Heuristic: if looks like Array, use len, else strlen
    fprintf(p->output, "char* TYPE_OF(void* obj) { return \"object\"; }\n");  // Simplified - returns generic type
    fprintf(p->output, "char* KARAKTER_AL(char* str, int64_t idx) { static char buf[2]; buf[0] = str[idx]; buf[1] = 0; return strdup(buf); }\n");
    fprintf(p->output, "char* ALT_METIN(char* str, int64_t start, int64_t len) { char* r = malloc(len+1); strncpy(r, str+start, len); r[len] = 0; return r; }\n");
    fprintf(p->output, "char* METIN_DEGISTIR(char* str, char* old, char* new) { if(!strstr(str, old)) return strdup(str); int oldlen = strlen(old); int newlen = strlen(new); int count = 0; char* p = str; while((p = strstr(p, old))) { count++; p += oldlen; } char* result = malloc(strlen(str) + count * (newlen - oldlen) + 1); char* dst = result; p = str; char* q; while((q = strstr(p, old))) { strncpy(dst, p, q - p); dst += q - p; strcpy(dst, new); dst += newlen; p = q + oldlen; } strcpy(dst, p); return result; }\n");
    fprintf(p->output, "int64_t PARSE_INT(char* str) { return str ? atoll(str) : 0; }\n");
    fprintf(p->output, "double PARSE_FLOAT(char* str) { return str ? atof(str) : 0.0; }\n");
    fprintf(p->output, "bool IS_DIGIT(char* c) { return c && *c >= '0' && *c <= '9'; }\n");
    fprintf(p->output, "bool IS_WHITESPACE(char* c) { return c && (*c == ' ' || *c == '\\t' || *c == '\\n' || *c == '\\r'); }\n");
    fprintf(p->output, "char* DOSYA_OKU(char* path) { FILE* f = fopen(path, \"r\"); if(!f) return NULL; fseek(f, 0, SEEK_END); long len = ftell(f); fseek(f, 0, SEEK_SET); char* buf = malloc(len+1); fread(buf, 1, len, f); buf[len] = 0; fclose(f); return buf; }\n");
    fprintf(p->output, "void* JSON_PARSE(char* json) { return NULL; }\n");  // Stub for now
    fprintf(p->output, "void YAZDIR(void* val) { printf(\"%%s\\n\", (char*)val); }\n\n");

    fprintf(p->output, "// Exception handling stubs\n");
    fprintf(p->output, "void mlp_throw(char* msg) { fprintf(stderr, \"Exception: %%s\\n\", msg); exit(1); }\n\n");

    while (!parser_check(p, TOK_EOF)) {
        if (parser_check(p, TOK_CLASS)) {
            parser_parse_class(p);
        }
        else if (parser_check(p, TOK_KULLAN)) {
            parser_parse_kullan(p);
        }
        else if (parser_check(p, TOK_FONKSIYON) || parser_check(p, TOK_ISLEC)) {
            // Top-level function (FONKSIYON or İŞLEÇ)
            parser_advance(p);
            if (parser_check(p, TOK_IDENT)) {
                char* func_name = str_dup(p->lexer->current.value);

                // Special case for main - return int instead of void*
                if (strcmp(func_name, "main") == 0) {
                    fprintf(p->output, "\nint %s", func_name);
                } else {
                    fprintf(p->output, "\nvoid* %s", func_name);
                }

                free(func_name);
                parser_advance(p);
            }
            parser_expect(p, TOK_LPAREN, "( bekleniyor");
            fprintf(p->output, "(");

            // Parse parameters (type is optional, defaults to char*)
            bool first_param = true;
            if (!parser_check(p, TOK_RPAREN)) {
                do {
                    if (!first_param) fprintf(p->output, ", ");
                    first_param = false;

                    if (parser_check(p, TOK_COMMA)) parser_advance(p);

                    const char* param_type_str = "void*";  // Default to typeless (void* - generic pointer)
                    if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
                        parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
                        parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
                        parser_check(p, TOK_ARRAY_TYPE)) {
                        param_type_str = mlp_type_to_c(p->lexer->current.type);
                        parser_advance(p);
                    }

                    if (parser_check(p, TOK_IDENT)) {
                        fprintf(p->output, "%s %s", param_type_str, p->lexer->current.value);
                        parser_advance(p);
                    }
                } while (parser_match(p, TOK_COMMA));
            }

            parser_expect(p, TOK_RPAREN, ") bekleniyor");
            fprintf(p->output, ") {\n");

            // Parse function body
            p->indent_level++;
            while (!parser_check(p, TOK_EOF) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
                parser_parse_statement(p);
            }
            if (parser_check(p, TOK_END) || parser_check(p, TOK_SON)) {
                parser_advance(p);
            }
            p->indent_level--;
            fprintf(p->output, "}\n");
        }
        else {
            // Top-level statement - add to main body
            FILE* saved_output = p->output;
            p->output = p->main_body;
            p->has_main_statements = true;
            p->indent_level = 1;  // Inside main()

            parser_parse_statement(p);

            p->indent_level = 0;
            p->output = saved_output;
        }
    }

    // Generate main() function if there are top-level statements
    if (p->has_main_statements) {
        fprintf(p->output, "\nint main() {\n");

        // Copy main_body contents to output
        rewind(p->main_body);
        char buf[4096];
        size_t n;
        while ((n = fread(buf, 1, sizeof(buf), p->main_body)) > 0) {
            fwrite(buf, 1, n, p->output);
        }

        fprintf(p->output, "    return 0;\n");
        fprintf(p->output, "}\n");

        fclose(p->main_body);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Kullanım: %s <input.mlp> <output.c>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Hata: %s dosyası açılamadı\n", argv[1]);
        return 1;
    }

    fseek(input, 0, SEEK_END);
    long size = ftell(input);
    fseek(input, 0, SEEK_SET);

    char* source = malloc(size + 1);
    fread(source, 1, size, input);
    source[size] = '\0';
    fclose(input);

    FILE* output = fopen(argv[2], "w");
    if (!output) {
        fprintf(stderr, "Hata: %s dosyası oluşturulamadı\n", argv[2]);
        free(source);
        return 1;
    }

    Lexer lexer = {source, 0, 1, 1, {0}};
    Parser parser = {&lexer, output, 0, false, false, NULL};

    parser_advance(&parser);
    parser_parse(&parser);

    fclose(output);
    free(source);

    printf("Başarılı: %s -> %s\n", argv[1], argv[2]);
    return 0;
}
