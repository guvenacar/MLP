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
    TOKEN_TEXT,          // text (string type keyword)
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,        // String literal value
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
    TOKEN_ELSE_IF,       // else if
    TOKEN_END_IF,        // end if
    TOKEN_END_ENUM,      // end enum
    TOKEN_END,           // end
    // Logical operators
    TOKEN_AND,           // and
    TOKEN_OR,            // or
    TOKEN_NOT,           // not
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
    TOKEN_DO,            // do
    TOKEN_EXIT,          // exit
    TOKEN_CONTINUE,      // continue
    // Switch/Case
    TOKEN_SWITCH,        // switch
    TOKEN_CASE,          // case
    TOKEN_DEFAULT,       // default
    // Debug keywords
    TOKEN_DEBUG,         // debug
    TOKEN_GOTO,          // goto (only with debug)
    TOKEN_PAUSE,         // pause (only with debug)
    // Phase 9: Modules & Imports
    TOKEN_IMPORT,        // import
    TOKEN_MODULE,        // module
    TOKEN_AS,            // as (for import aliasing)
    TOKEN_EXPORT,        // export (public visibility)
    TOKEN_PRIVATE,       // private (internal visibility)
    // Phase 12: Exception handling
    TOKEN_TRY,           // try
    TOKEN_CATCH,         // catch
    TOKEN_FINALLY,       // finally
    TOKEN_THROW,         // throw
    TOKEN_END_TRY,       // end try
    // Phase 12: Async/Await
    TOKEN_ASYNC,         // async
    TOKEN_AWAIT,         // await
    // Phase 14: Iterator/Generator
    TOKEN_YIELD,         // yield
    TOKEN_YIELDS,        // yields (for generator return type)
    TOKEN_IN,            // in (for for-in loops)
    // Phase 4: Functions
    TOKEN_FUNC,          // func
    TOKEN_RETURN,        // return
    TOKEN_RETURNS,       // returns (for return type declaration)
    TOKEN_COMMA,         // ,
    // Phase 6: Structs
    TOKEN_STRUCT,        // struct
    TOKEN_TYPE,          // type (for type aliases)
    TOKEN_DOT,           // . (field access)
    // Interfaces
    TOKEN_INTERFACE,     // interface
    TOKEN_IMPLEMENTS,    // implements
    // Enums
    TOKEN_ENUM,          // enum
    // Ternary operator
    TOKEN_QUESTION,      // ?
    TOKEN_COLON,         // :
    // Nullable types
    TOKEN_NULL,          // null
    // Phase 7: String interpolation
    TOKEN_INTERPOLATED_STRING, // $"text {expr} text"
    // Phase 8: Arrays
    TOKEN_LBRACKET,      // [
    TOKEN_RBRACKET,      // ]
    // Phase 10: Pointers
    TOKEN_AMPERSAND,     // & (address-of)
    // Phase 11: Union types
    TOKEN_PIPE,          // | (union type separator)
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

// Read interpolated string: $"text {expr} text"
// Returns string with special markers: TEXT\1EXPR\2TEXT
// \1 = start of expression, \2 = end of expression
char* lexer_read_interpolated_string(Lexer* lexer) {
    // Skip $ and opening "
    lexer->pos += 2;
    
    int capacity = 128;
    int len = 0;
    char* str = malloc(capacity);
    
    while (lexer->source[lexer->pos] != '\0') {
        // Closing "
        if (lexer->source[lexer->pos] == '"') {
            lexer->pos++;
            break;
        }
        
        // Start of expression: {
        if (lexer->source[lexer->pos] == '{') {
            // Add marker for expression start
            str[len++] = '\1';  // SOH (Start of Heading) - ASCII 1
            lexer->pos++;
            
            // Read expression until }
            int brace_depth = 1;
            while (lexer->source[lexer->pos] != '\0' && brace_depth > 0) {
                if (lexer->source[lexer->pos] == '{') {
                    brace_depth++;
                } else if (lexer->source[lexer->pos] == '}') {
                    brace_depth--;
                    if (brace_depth == 0) {
                        lexer->pos++;  // Skip closing }
                        break;
                    }
                }
                
                if (len >= capacity - 2) {
                    capacity *= 2;
                    str = realloc(str, capacity);
                }
                
                str[len++] = lexer->source[lexer->pos++];
            }
            
            // Add marker for expression end
            str[len++] = '\2';  // STX (Start of Text) - ASCII 2
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
                case '{': escaped = '{'; break;  // Escaped brace
                default: escaped = lexer->source[lexer->pos];
            }
            str[len++] = escaped;
            lexer->pos++;
        } else {
            if (lexer->source[lexer->pos] == '\n') {
                lexer->line++;
            }
            
            if (len >= capacity - 1) {
                capacity *= 2;
                str = realloc(str, capacity);
            }
            
            str[len++] = lexer->source[lexer->pos++];
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
    
    // Interpolated strings: $"..."
    if (lexer->source[lexer->pos] == '$' && lexer->source[lexer->pos + 1] == '"') {
        token->type = TOKEN_INTERPOLATED_STRING;
        token->value = lexer_read_interpolated_string(lexer);
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
        } else if (strcmp(word, "text") == 0) {
            token->type = TOKEN_TEXT;
        } else if (strcmp(word, "print") == 0) {
            token->type = TOKEN_PRINT;
        } else if (strcmp(word, "yazdir") == 0) {  // Turkish: print
            token->type = TOKEN_PRINT;
        } else if (strcmp(word, "if") == 0) {
            token->type = TOKEN_IF;
        } else if (strcmp(word, "then") == 0) {
            token->type = TOKEN_THEN;
        } else if (strcmp(word, "else") == 0) {
            // Peek ahead for "if" to make "else if"
            lexer_skip_whitespace(lexer);
            int len = strlen(lexer->source);
            if (lexer->pos + 1 < len && 
                lexer->source[lexer->pos] == 'i' && 
                lexer->source[lexer->pos + 1] == 'f' &&
                (lexer->pos + 2 >= len || !isalnum(lexer->source[lexer->pos + 2]))) {
                lexer->pos += 2; // Skip "if"
                token->type = TOKEN_ELSE_IF;
                free(token->value);
                token->value = malloc(8);
                strcpy(token->value, "else if");
            } else {
                token->type = TOKEN_ELSE;
            }
        } else if (strcmp(word, "and") == 0) {
            token->type = TOKEN_AND;
        } else if (strcmp(word, "or") == 0) {
            token->type = TOKEN_OR;
        } else if (strcmp(word, "not") == 0) {
            token->type = TOKEN_NOT;
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
        } else if (strcmp(word, "do") == 0) {
            token->type = TOKEN_DO;
        } else if (strcmp(word, "exit") == 0) {
            token->type = TOKEN_EXIT;
        } else if (strcmp(word, "continue") == 0) {
            token->type = TOKEN_CONTINUE;
        } else if (strcmp(word, "switch") == 0) {
            token->type = TOKEN_SWITCH;
        } else if (strcmp(word, "case") == 0) {
            token->type = TOKEN_CASE;
        } else if (strcmp(word, "default") == 0) {
            token->type = TOKEN_DEFAULT;
        } else if (strcmp(word, "null") == 0) {
            token->type = TOKEN_NULL;
        } else if (strcmp(word, "debug") == 0) {
            token->type = TOKEN_DEBUG;
        } else if (strcmp(word, "goto") == 0) {
            token->type = TOKEN_GOTO;
        } else if (strcmp(word, "pause") == 0) {
            token->type = TOKEN_PAUSE;
        } else if (strcmp(word, "try") == 0) {
            token->type = TOKEN_TRY;
        } else if (strcmp(word, "catch") == 0) {
            token->type = TOKEN_CATCH;
        } else if (strcmp(word, "finally") == 0) {
            token->type = TOKEN_FINALLY;
        } else if (strcmp(word, "throw") == 0) {
            token->type = TOKEN_THROW;
        } else if (strcmp(word, "func") == 0) {
            token->type = TOKEN_FUNC;
        } else if (strcmp(word, "yaz") == 0) {  // Turkish: write/func
            token->type = TOKEN_FUNC;
        } else if (strcmp(word, "return") == 0) {
            token->type = TOKEN_RETURN;
        } else if (strcmp(word, "dondur") == 0) {  // Turkish: return
            token->type = TOKEN_RETURN;
        } else if (strcmp(word, "returns") == 0) {
            token->type = TOKEN_RETURNS;
        } else if (strcmp(word, "struct") == 0) {
            token->type = TOKEN_STRUCT;
        } else if (strcmp(word, "type") == 0) {
            token->type = TOKEN_TYPE;
        } else if (strcmp(word, "enum") == 0) {
            token->type = TOKEN_ENUM;
        } else if (strcmp(word, "interface") == 0) {
            token->type = TOKEN_INTERFACE;
        } else if (strcmp(word, "implements") == 0) {
            token->type = TOKEN_IMPLEMENTS;
        } else if (strcmp(word, "import") == 0) {
            token->type = TOKEN_IMPORT;
        } else if (strcmp(word, "module") == 0) {
            token->type = TOKEN_MODULE;
        } else if (strcmp(word, "as") == 0) {
            token->type = TOKEN_AS;
        } else if (strcmp(word, "export") == 0) {
            token->type = TOKEN_EXPORT;
        } else if (strcmp(word, "private") == 0) {
            token->type = TOKEN_PRIVATE;
        } else if (strcmp(word, "async") == 0) {
            token->type = TOKEN_ASYNC;
        } else if (strcmp(word, "await") == 0) {
            token->type = TOKEN_AWAIT;
        } else if (strcmp(word, "yield") == 0) {
            token->type = TOKEN_YIELD;
        } else if (strcmp(word, "yields") == 0) {
            token->type = TOKEN_YIELDS;
        } else if (strcmp(word, "in") == 0) {
            token->type = TOKEN_IN;
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
        case '[': token->type = TOKEN_LBRACKET; break;
        case ']': token->type = TOKEN_RBRACKET; break;
        case ',': token->type = TOKEN_COMMA; break;
        case '.': token->type = TOKEN_DOT; break;
        case '?': token->type = TOKEN_QUESTION; break;
        case ':': token->type = TOKEN_COLON; break;
        case '+': token->type = TOKEN_PLUS; break;
        case '-': token->type = TOKEN_MINUS; break;
        case '*': token->type = TOKEN_MULTIPLY; break;
        case '/': token->type = TOKEN_DIVIDE; break;
        case '&': token->type = TOKEN_AMPERSAND; break; // Phase 10: Address-of
        case '|': token->type = TOKEN_PIPE; break; // Phase 11: Union types
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
