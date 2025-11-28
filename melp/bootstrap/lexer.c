// MELP Phase 1 - Bootstrap Compiler
// Lexer: Deklarasyonlar + Assignment + Print + Comments

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    TOKEN_NUMERIC,
    TOKEN_DECIMAL,
    TOKEN_BOOLEAN,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_ASSIGN,        // =
    TOKEN_LPAREN,        // (
    TOKEN_RPAREN,        // )
    TOKEN_PLUS,          // +
    TOKEN_MINUS,         // -
    TOKEN_MULTIPLY,      // *
    TOKEN_DIVIDE,        // /
    TOKEN_PRINT,         // print
    // Phase 2: Conditionals
    TOKEN_IF,            // if
    TOKEN_THEN,          // then
    TOKEN_ELSE,          // else
    TOKEN_END_IF,        // end if
    TOKEN_END,           // end
    // Comparison operators
    TOKEN_EQUAL,         // ==
    TOKEN_NOT_EQUAL,     // !=
    TOKEN_LESS,          // <
    TOKEN_LESS_EQUAL,    // <=
    TOKEN_GREATER,       // >
    TOKEN_GREATER_EQUAL, // >=
    // Phase 3: Loops
    TOKEN_FOR,           // for
    TOKEN_TO,            // to
    TOKEN_STEP,          // step
    TOKEN_WHILE,         // while
    TOKEN_EXIT,          // exit
    TOKEN_CONTINUE,      // continue
    // Debug keywords
    TOKEN_DEBUG,         // debug
    TOKEN_GOTO,          // goto (only with debug)
    TOKEN_PAUSE,         // pause (only with debug)
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
} Token;

typedef struct {
    const char* source;
    int pos;
    int line;
} Lexer;

Lexer* lexer_create(const char* source) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->source = source;
    lexer->pos = 0;
    lexer->line = 1;
    return lexer;
}

void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->source[lexer->pos] != '\0') {
        char c = lexer->source[lexer->pos];
        if (c == ' ' || c == '\t' || c == '\r') {
            lexer->pos++;
        } else if (c == '\n') {
            lexer->pos++;
            lexer->line++;
        } else {
            break;
        }
    }
}

void lexer_skip_single_line_comment(Lexer* lexer) {
    // Skip --
    lexer->pos += 2;
    while (lexer->source[lexer->pos] != '\0' && lexer->source[lexer->pos] != '\n') {
        lexer->pos++;
    }
}

void lexer_skip_multi_line_comment(Lexer* lexer) {
    // Skip ---
    lexer->pos += 3;
    while (lexer->source[lexer->pos] != '\0') {
        if (lexer->source[lexer->pos] == '-' && 
            lexer->source[lexer->pos + 1] == '-' &&
            lexer->source[lexer->pos + 2] == '-') {
            lexer->pos += 3;
            return;
        }
        if (lexer->source[lexer->pos] == '\n') {
            lexer->line++;
        }
        lexer->pos++;
    }
}

void lexer_skip_whitespace_and_comments(Lexer* lexer) {
    while (lexer->source[lexer->pos] != '\0') {
        // Whitespace
        if (lexer->source[lexer->pos] == ' ' || 
            lexer->source[lexer->pos] == '\t' || 
            lexer->source[lexer->pos] == '\r') {
            lexer->pos++;
        } else if (lexer->source[lexer->pos] == '\n') {
            lexer->pos++;
            lexer->line++;
        }
        // Multi-line comment: ---
        else if (lexer->source[lexer->pos] == '-' && 
                 lexer->source[lexer->pos + 1] == '-' &&
                 lexer->source[lexer->pos + 2] == '-') {
            lexer_skip_multi_line_comment(lexer);
        }
        // Single-line comment: --
        else if (lexer->source[lexer->pos] == '-' && 
                 lexer->source[lexer->pos + 1] == '-') {
            lexer_skip_single_line_comment(lexer);
        } else {
            break;
        }
    }
}

char* lexer_read_word(Lexer* lexer) {
    int start = lexer->pos;
    while (isalnum(lexer->source[lexer->pos]) || lexer->source[lexer->pos] == '_') {
        lexer->pos++;
    }
    int len = lexer->pos - start;
    char* word = malloc(len + 1);
    strncpy(word, &lexer->source[start], len);
    word[len] = '\0';
    return word;
}

char* lexer_read_number(Lexer* lexer) {
    int start = lexer->pos;
    while (isdigit(lexer->source[lexer->pos]) || lexer->source[lexer->pos] == '.') {
        lexer->pos++;
    }
    int len = lexer->pos - start;
    char* num = malloc(len + 1);
    strncpy(num, &lexer->source[start], len);
    num[len] = '\0';
    return num;
}

char* lexer_read_string(Lexer* lexer) {
    // Skip opening "
    lexer->pos++;
    int start = lexer->pos;
    int capacity = 64;
    int len = 0;
    char* str = malloc(capacity);
    
    while (lexer->source[lexer->pos] != '\0') {
        // Closing "
        if (lexer->source[lexer->pos] == '"') {
            lexer->pos++;
            break;
        }
        
        // Line continuation: \ at end of line
        if (lexer->source[lexer->pos] == '\\' && lexer->source[lexer->pos + 1] == '\n') {
            lexer->pos += 2;
            lexer->line++;
            continue;
        }
        
        // Escape sequences
        if (lexer->source[lexer->pos] == '\\') {
            lexer->pos++;
            char escaped;
            switch (lexer->source[lexer->pos]) {
                case 'n': escaped = '\n'; break;
                case 't': escaped = '\t'; break;
                case '\\': escaped = '\\'; break;
                case '"': escaped = '"'; break;
                default: escaped = lexer->source[lexer->pos];
            }
            str[len++] = escaped;
            lexer->pos++;
        } else {
            if (lexer->source[lexer->pos] == '\n') {
                lexer->line++;
            }
            str[len++] = lexer->source[lexer->pos++];
        }
        
        if (len >= capacity - 1) {
            capacity *= 2;
            str = realloc(str, capacity);
        }
    }
    
    str[len] = '\0';
    return str;
}

Token* lexer_next_token(Lexer* lexer) {
    lexer_skip_whitespace_and_comments(lexer);
    
    if (lexer->source[lexer->pos] == '\0') {
        Token* token = malloc(sizeof(Token));
        token->type = TOKEN_EOF;
        token->value = NULL;
        token->line = lexer->line;
        return token;
    }
    
    Token* token = malloc(sizeof(Token));
    token->line = lexer->line;
    
    // Numbers
    if (isdigit(lexer->source[lexer->pos])) {
        token->type = TOKEN_NUMBER;
        token->value = lexer_read_number(lexer);
        return token;
    }
    
    // Strings
    if (lexer->source[lexer->pos] == '"') {
        token->type = TOKEN_STRING;
        token->value = lexer_read_string(lexer);
        return token;
    }
    
    // Keywords and identifiers
    if (isalpha(lexer->source[lexer->pos])) {
        char* word = lexer_read_word(lexer);
        
        if (strcmp(word, "numeric") == 0) {
            token->type = TOKEN_NUMERIC;
        } else if (strcmp(word, "decimal") == 0) {
            token->type = TOKEN_DECIMAL;
        } else if (strcmp(word, "boolean") == 0) {
            token->type = TOKEN_BOOLEAN;
        } else if (strcmp(word, "print") == 0) {
            token->type = TOKEN_PRINT;
        } else if (strcmp(word, "if") == 0) {
            token->type = TOKEN_IF;
        } else if (strcmp(word, "then") == 0) {
            token->type = TOKEN_THEN;
        } else if (strcmp(word, "else") == 0) {
            token->type = TOKEN_ELSE;
        } else if (strcmp(word, "end") == 0) {
            token->type = TOKEN_END;
        } else if (strcmp(word, "for") == 0) {
            token->type = TOKEN_FOR;
        } else if (strcmp(word, "to") == 0) {
            token->type = TOKEN_TO;
        } else if (strcmp(word, "step") == 0) {
            token->type = TOKEN_STEP;
        } else if (strcmp(word, "while") == 0) {
            token->type = TOKEN_WHILE;
        } else if (strcmp(word, "exit") == 0) {
            token->type = TOKEN_EXIT;
        } else if (strcmp(word, "continue") == 0) {
            token->type = TOKEN_CONTINUE;
        } else if (strcmp(word, "debug") == 0) {
            token->type = TOKEN_DEBUG;
        } else if (strcmp(word, "goto") == 0) {
            token->type = TOKEN_GOTO;
        } else if (strcmp(word, "pause") == 0) {
            token->type = TOKEN_PAUSE;
        } else {
            token->type = TOKEN_IDENTIFIER;
        }
        token->value = word;
        return token;
    }
    
    // Operators and punctuation
    char c = lexer->source[lexer->pos++];
    token->value = NULL;
    
    switch (c) {
        case '=':
            if (lexer->source[lexer->pos] == '=') {
                lexer->pos++;
                token->type = TOKEN_EQUAL;
            } else {
                token->type = TOKEN_ASSIGN;
            }
            break;
        case '!':
            if (lexer->source[lexer->pos] == '=') {
                lexer->pos++;
                token->type = TOKEN_NOT_EQUAL;
            } else {
                fprintf(stderr, "Lexer error: Expected '=' after '!' at line %d\n", lexer->line);
                exit(1);
            }
            break;
        case '<':
            if (lexer->source[lexer->pos] == '=') {
                lexer->pos++;
                token->type = TOKEN_LESS_EQUAL;
            } else {
                token->type = TOKEN_LESS;
            }
            break;
        case '>':
            if (lexer->source[lexer->pos] == '=') {
                lexer->pos++;
                token->type = TOKEN_GREATER_EQUAL;
            } else {
                token->type = TOKEN_GREATER;
            }
            break;
        case '(': token->type = TOKEN_LPAREN; break;
        case ')': token->type = TOKEN_RPAREN; break;
        case '+': token->type = TOKEN_PLUS; break;
        case '-': token->type = TOKEN_MINUS; break;
        case '*': token->type = TOKEN_MULTIPLY; break;
        case '/': token->type = TOKEN_DIVIDE; break;
        default:
            fprintf(stderr, "Lexer error: Unknown character '%c' at line %d\n", c, lexer->line);
            exit(1);
    }
    
    return token;
}

void lexer_free(Lexer* lexer) {
    free(lexer);
}

void token_free(Token* token) {
    if (token->value) {
        free(token->value);
    }
    free(token);
}
