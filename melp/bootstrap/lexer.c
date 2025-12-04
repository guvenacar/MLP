// MELP Bootstrap Compiler - Lexer
// Minimal lexer for Stage 1 MELP modules
// Will be replaced by MELP version in Stage 2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static const char *source;
static int pos = 0;
static int line = 1;
static int col = 1;

void lexer_init(const char *src) {
    source = src;
    pos = 0;
    line = 1;
    col = 1;
}

static void skip_whitespace() {
    while (source[pos]) {
        if (source[pos] == ' ' || source[pos] == '\t' || source[pos] == '\r') {
            pos++;
            col++;
        } else if (source[pos] == '\n') {
            pos++;
            line++;
            col = 1;
        } else {
            break;
        }
    }
}

static void skip_comment() {
    if (source[pos] == '-' && source[pos+1] == '-') {
        // Single line comment or multi-line
        if (source[pos+2] == '-') {
            // Multi-line comment: --- ... ---
            pos += 3;
            while (source[pos]) {
                if (source[pos] == '-' && source[pos+1] == '-' && source[pos+2] == '-') {
                    pos += 3;
                    break;
                }
                if (source[pos] == '\n') {
                    line++;
                    col = 1;
                } else {
                    col++;
                }
                pos++;
            }
        } else {
            // Single line comment: -- ...
            pos += 2;
            while (source[pos] && source[pos] != '\n') {
                pos++;
                col++;
            }
        }
    }
}

Token next_token() {
    Token tok = {TK_EOF, "", line, col};
    
    skip_whitespace();
    skip_comment();
    skip_whitespace();
    
    if (!source[pos]) {
        return tok;
    }
    
    tok.line = line;
    tok.col = col;
    
    // Keywords and identifiers
    if (isalpha(source[pos]) || source[pos] == '_') {
        int i = 0;
        while ((isalnum(source[pos]) || source[pos] == '_') && i < 255) {
            tok.value[i++] = source[pos++];
            col++;
        }
        tok.value[i] = '\0';
        
        // Check for keywords
        if (strcmp(tok.value, "numeric") == 0 ||
            strcmp(tok.value, "string") == 0 ||
            strcmp(tok.value, "boolean") == 0 ||
            strcmp(tok.value, "function") == 0 ||
            strcmp(tok.value, "struct") == 0 ||
            strcmp(tok.value, "return") == 0 ||
            strcmp(tok.value, "if") == 0 ||
            strcmp(tok.value, "then") == 0 ||
            strcmp(tok.value, "else") == 0 ||
            strcmp(tok.value, "end") == 0 ||
            strcmp(tok.value, "while") == 0 ||
            strcmp(tok.value, "for") == 0 ||
            strcmp(tok.value, "to") == 0 ||
            strcmp(tok.value, "print") == 0 ||
            strcmp(tok.value, "true") == 0 ||
            strcmp(tok.value, "false") == 0) {
            tok.type = TK_KEYWORD;
        } else {
            tok.type = TK_IDENTIFIER;
        }
        return tok;
    }
    
    // Numbers
    if (isdigit(source[pos])) {
        int i = 0;
        while (isdigit(source[pos]) && i < 255) {
            tok.value[i++] = source[pos++];
            col++;
        }
        tok.value[i] = '\0';
        tok.type = TK_NUMBER;
        return tok;
    }
    
    // String literals
    if (source[pos] == '"') {
        pos++; col++;
        int i = 0;
        while (source[pos] && source[pos] != '"' && i < 255) {
            if (source[pos] == '\\' && source[pos+1]) {
                pos++; col++;
                switch (source[pos]) {
                    case 'n': tok.value[i++] = '\n'; break;
                    case 't': tok.value[i++] = '\t'; break;
                    case '\\': tok.value[i++] = '\\'; break;
                    case '"': tok.value[i++] = '"'; break;
                    default: tok.value[i++] = source[pos]; break;
                }
                pos++; col++;
            } else {
                tok.value[i++] = source[pos++];
                col++;
            }
        }
        if (source[pos] == '"') {
            pos++; col++;
        }
        tok.value[i] = '\0';
        tok.type = TK_STRING;
        return tok;
    }
    
    // Operators
    if (strchr("+-*/%=<>!&|", source[pos])) {
        tok.value[0] = source[pos++];
        col++;
        // Check for two-character operators
        if ((tok.value[0] == '=' && source[pos] == '=') ||
            (tok.value[0] == '!' && source[pos] == '=') ||
            (tok.value[0] == '<' && source[pos] == '=') ||
            (tok.value[0] == '>' && source[pos] == '=') ||
            (tok.value[0] == '&' && source[pos] == '&') ||
            (tok.value[0] == '|' && source[pos] == '|')) {
            tok.value[1] = source[pos++];
            tok.value[2] = '\0';
            col++;
        } else {
            tok.value[1] = '\0';
        }
        tok.type = TK_OPERATOR;
        return tok;
    }
    
    // Symbols
    if (strchr("()[]{},.:", source[pos])) {
        tok.value[0] = source[pos++];
        tok.value[1] = '\0';
        col++;
        tok.type = TK_SYMBOL;
        return tok;
    }
    
    // Unknown character
    tok.value[0] = source[pos++];
    tok.value[1] = '\0';
    col++;
    tok.type = TK_UNKNOWN;
    return tok;
}

const char* token_type_name(TokenType type) {
    switch (type) {
        case TK_EOF: return "EOF";
        case TK_KEYWORD: return "KEYWORD";
        case TK_IDENTIFIER: return "IDENTIFIER";
        case TK_NUMBER: return "NUMBER";
        case TK_STRING: return "STRING";
        case TK_OPERATOR: return "OPERATOR";
        case TK_SYMBOL: return "SYMBOL";
        case TK_UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}
