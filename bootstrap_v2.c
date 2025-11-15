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
    TOK_FONKSIYON, TOK_VAR, TOK_SAYISAL, TOK_METIN, TOK_MANTIKSAL,
    TOK_DIZI, TOK_DONUSTUR, TOK_EGER, TOK_DEGILSE, TOK_ISE,
    TOK_HER, TOK_ICINDE, TOK_YAZDIR, TOK_DOGRU, TOK_YANLIS,
    TOK_YAPI, TOK_YENI, TOK_KULLAN, TOK_PAKET, TOK_SINIF,
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
    int indent_level;
    bool in_main;
    bool in_class;
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
        {"var", TOK_VAR}, {"VAR", TOK_VAR},
        {"sayısal", TOK_SAYISAL}, {"SAYISAL", TOK_SAYISAL},
        {"metin", TOK_METIN}, {"METİN", TOK_METIN},
        {"mantıksal", TOK_MANTIKSAL}, {"MANTIKSAL", TOK_MANTIKSAL},
        {"dizi", TOK_DIZI}, {"DİZİ", TOK_DIZI},
        {"döndür", TOK_DONUSTUR}, {"DÖNDÜR", TOK_DONUSTUR},
        {"eğer", TOK_EGER}, {"EĞER", TOK_EGER},
        {"değilse", TOK_DEGILSE}, {"DEĞİLSE", TOK_DEGILSE},
        {"ise", TOK_ISE}, {"İSE", TOK_ISE},
        {"her", TOK_HER}, {"HER", TOK_HER},
        {"içinde", TOK_ICINDE}, {"İÇİNDE", TOK_ICINDE},
        {"yazdır", TOK_YAZDIR}, {"YAZDIR", TOK_YAZDIR},
        {"doğru", TOK_DOGRU}, {"DOĞRU", TOK_DOGRU},
        {"yanlış", TOK_YANLIS}, {"YANLIŞ", TOK_YANLIS},
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
        case TOK_DYNAMIC: return "void*";
        case TOK_ARRAY_TYPE: return "void**";
        case TOK_DICT: return "void*";
        default: return "void*";
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
        // new ClassName(args) -> malloc + constructor
        fprintf(p->output, "malloc(sizeof(struct TODO))");
    } else if (parser_check(p, TOK_IDENT)) {
        fprintf(p->output, "%s", p->lexer->current.value);
        parser_advance(p);

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
        // Dict literal {key: value, ...}
        fprintf(p->output, "dict_new("); // TODO: proper dict implementation
        int count = 0;
        if (!parser_check(p, TOK_RBRACE)) {
            parser_parse_expression(p);
            count++;
            if (parser_match(p, TOK_COLON)) {
                fprintf(p->output, ", ");
                parser_parse_expression(p);
            }
            while (parser_match(p, TOK_COMMA)) {
                fprintf(p->output, ", ");
                parser_parse_expression(p);
                count++;
                if (parser_match(p, TOK_COLON)) {
                    fprintf(p->output, ", ");
                    parser_parse_expression(p);
                }
            }
        }
        parser_expect(p, TOK_RBRACE, "} bekleniyor");
        fprintf(p->output, ")");
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
    parser_parse_multiplicative(p);
    while (parser_check(p, TOK_PLUS) || parser_check(p, TOK_MINUS)) {
        char* op_str = str_dup(p->lexer->current.value);
        parser_advance(p);
        fprintf(p->output, " %s ", op_str);
        free(op_str);
        parser_parse_multiplicative(p);
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

    if (!parser_check(p, TOK_IDENT)) {
        fprintf(stderr, "Hata: Değişken adı bekleniyor\n");
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
    parser_match(p, TOK_SEMICOLON);
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

    char* var_name = str_dup(p->lexer->current.value);
    parser_advance(p);

    parser_expect(p, TOK_ICINDE, "İÇİNDE bekleniyor");

    // HER item İÇİNDE list -> for (int i = 0; i < list_len; i++)
    fprintf(p->output, "for (int %s_i = 0; %s_i < ", var_name, var_name);
    parser_parse_expression(p);
    fprintf(p->output, "_len; %s_i++) ", var_name);

    parser_parse_block(p);
    free(var_name);
}

void parser_parse_return_statement(Parser* p) {
    parser_advance(p); // Skip DONUSTUR/RETURN

    indent(p);
    fprintf(p->output, "return");

    if (!parser_check(p, TOK_SEMICOLON) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
        fprintf(p->output, " ");
        parser_parse_expression(p);
    }

    fprintf(p->output, ";\n");
    parser_match(p, TOK_SEMICOLON);
}

void parser_parse_statement(Parser* p) {
    // Variable declaration
    if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
        parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_VAR) ||
        parser_check(p, TOK_DYNAMIC) || parser_check(p, TOK_STRING_TYPE) ||
        parser_check(p, TOK_NUMBER_TYPE) || parser_check(p, TOK_ARRAY_TYPE)) {
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
        parser_match(p, TOK_SEMICOLON);
    }
    // Expression statement or assignment
    else if (parser_check(p, TOK_IDENT) || parser_check(p, TOK_THIS)) {
        indent(p);
        parser_parse_expression(p);

        if (parser_match(p, TOK_ASSIGN)) {
            fprintf(p->output, " = ");
            parser_parse_expression(p);
        }

        fprintf(p->output, ";\n");
        parser_match(p, TOK_SEMICOLON);
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

    // Parameters
    if (!parser_check(p, TOK_RPAREN)) {
        do {
            if (parser_check(p, TOK_COMMA)) parser_advance(p);

            TokenType param_type = p->lexer->current.type;
            if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
                parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
                parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE)) {
                parser_advance(p);
            }

            if (parser_check(p, TOK_IDENT)) {
                fprintf(p->output, ", %s %s", mlp_type_to_c(param_type), p->lexer->current.value);
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

    // Parameters
    bool first = true;
    if (!parser_check(p, TOK_RPAREN)) {
        do {
            if (!first) fprintf(p->output, ", ");
            first = false;

            if (parser_check(p, TOK_COMMA)) parser_advance(p);

            TokenType param_type = p->lexer->current.type;
            if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
                parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
                parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE)) {
                parser_advance(p);
            }

            if (parser_check(p, TOK_IDENT)) {
                fprintf(p->output, "%s %s", mlp_type_to_c(param_type), p->lexer->current.value);
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

    p->in_class = true;
    p->current_class_name = class_name;

    fprintf(p->output, "\n// Class: %s\n", class_name);
    fprintf(p->output, "typedef struct %s {\n", class_name);

    // Parse class body
    bool struct_closed = false;
    while (!parser_check(p, TOK_EOF) && !parser_check(p, TOK_END) && !parser_check(p, TOK_SON)) {
        // Field declaration
        if (parser_check(p, TOK_SAYISAL) || parser_check(p, TOK_METIN) ||
            parser_check(p, TOK_MANTIKSAL) || parser_check(p, TOK_DYNAMIC) ||
            parser_check(p, TOK_STRING_TYPE) || parser_check(p, TOK_NUMBER_TYPE) ||
            parser_check(p, TOK_ARRAY_TYPE) || parser_check(p, TOK_DICT)) {

            TokenType field_type = p->lexer->current.type;
            parser_advance(p);

            if (parser_check(p, TOK_IDENT)) {
                fprintf(p->output, "    %s %s;\n", mlp_type_to_c(field_type), p->lexer->current.value);
                parser_advance(p);
                parser_match(p, TOK_SEMICOLON);
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

    parser_match(p, TOK_SEMICOLON);
}

void parser_parse(Parser* p) {
    fprintf(p->output, "#include <stdio.h>\n");
    fprintf(p->output, "#include <stdlib.h>\n");
    fprintf(p->output, "#include <string.h>\n");
    fprintf(p->output, "#include <stdbool.h>\n");
    fprintf(p->output, "#include <stdint.h>\n");
    fprintf(p->output, "#include <ctype.h>\n\n");

    // Helper functions for MLP operations
    fprintf(p->output, "// MLP Runtime Helper Functions\n");
    fprintf(p->output, "typedef struct { void** items; int len; int cap; } Array;\n\n");
    fprintf(p->output, "Array* array_new() { Array* a = malloc(sizeof(Array)); a->items = malloc(16*sizeof(void*)); a->len = 0; a->cap = 16; return a; }\n");
    fprintf(p->output, "void array_push(Array* a, void* item) { if(a->len >= a->cap) { a->cap *= 2; a->items = realloc(a->items, a->cap*sizeof(void*)); } a->items[a->len++] = item; }\n");
    fprintf(p->output, "void* array_pop(Array* a) { return a->len > 0 ? a->items[--a->len] : NULL; }\n");
    fprintf(p->output, "void array_insert(Array* a, int idx, void* item) { if(a->len >= a->cap) { a->cap *= 2; a->items = realloc(a->items, a->cap*sizeof(void*)); } memmove(&a->items[idx+1], &a->items[idx], (a->len-idx)*sizeof(void*)); a->items[idx] = item; a->len++; }\n\n");
    fprintf(p->output, "char* str_lower(char* s) { char* r = strdup(s); for(int i=0; r[i]; i++) r[i] = tolower(r[i]); return r; }\n");
    fprintf(p->output, "char* str_substr(char* s, int start, int len) { char* r = malloc(len+1); strncpy(r, s+start, len); r[len] = 0; return r; }\n");
    fprintf(p->output, "bool str_startswith(char* s, char* prefix) { return strncmp(s, prefix, strlen(prefix)) == 0; }\n");
    fprintf(p->output, "bool str_contains(char* s, char* sub) { return strstr(s, sub) != NULL; }\n\n");
    fprintf(p->output, "// Exception handling stubs\n");
    fprintf(p->output, "void mlp_throw(char* msg) { fprintf(stderr, \"Exception: %%s\\n\", msg); exit(1); }\n\n");

    while (!parser_check(p, TOK_EOF)) {
        if (parser_check(p, TOK_CLASS)) {
            parser_parse_class(p);
        }
        else if (parser_check(p, TOK_KULLAN)) {
            parser_parse_kullan(p);
        }
        else if (parser_check(p, TOK_FONKSIYON)) {
            // Top-level function
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
            // TODO: Parse parameters properly
            while (!parser_check(p, TOK_RPAREN) && !parser_check(p, TOK_EOF)) {
                parser_advance(p);
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
            parser_advance(p);
        }
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
