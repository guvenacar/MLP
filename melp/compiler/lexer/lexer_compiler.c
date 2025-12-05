#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Enhanced Lexer Compiler for MELP Stage 3 Phase 2
// Real tokenization implementation (not placeholder)
// Compiles lexer.mlp by performing actual lexical analysis

// ============================================
// TOKEN TYPES (matching lexer.mlp)
// ============================================

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_ERROR = 1,
    
    // Literals
    TOKEN_IDENTIFIER = 10,
    TOKEN_NUMBER = 11,
    TOKEN_STRING = 12,
    
    // Keywords
    TOKEN_FN = 20,
    TOKEN_LET = 21,
    TOKEN_CONST = 22,
    TOKEN_IF = 23,
    TOKEN_ELSE = 24,
    TOKEN_WHILE = 25,
    TOKEN_FOR = 26,
    TOKEN_RETURN = 27,
    TOKEN_BREAK = 28,
    TOKEN_CONTINUE = 29,
    TOKEN_STRUCT = 30,
    TOKEN_ENUM = 31,
    TOKEN_IMPORT = 32,
    TOKEN_EXPORT = 33,
    TOKEN_DEFER = 34,
    TOKEN_MATCH = 35,
    TOKEN_TRUE = 36,
    TOKEN_FALSE = 37,
    TOKEN_NULL = 38,
    
    // Operators
    TOKEN_PLUS = 50,        // +
    TOKEN_MINUS = 51,       // -
    TOKEN_STAR = 52,        // *
    TOKEN_SLASH = 53,       // /
    TOKEN_PERCENT = 54,     // %
    TOKEN_EQUAL = 55,       // =
    TOKEN_EQ_EQ = 56,       // ==
    TOKEN_NOT_EQ = 57,      // !=
    TOKEN_LT = 58,          // <
    TOKEN_GT = 59,          // >
    TOKEN_LT_EQ = 60,       // <=
    TOKEN_GT_EQ = 61,       // >=
    TOKEN_AND_AND = 62,     // &&
    TOKEN_OR_OR = 63,       // ||
    TOKEN_NOT = 64,         // !
    TOKEN_AMP = 65,         // &
    TOKEN_PIPE = 66,        // |
    
    // Delimiters
    TOKEN_LPAREN = 80,      // (
    TOKEN_RPAREN = 81,      // )
    TOKEN_LBRACE = 82,      // {
    TOKEN_RBRACE = 83,      // }
    TOKEN_LBRACKET = 84,    // [
    TOKEN_RBRACKET = 85,    // ]
    TOKEN_COMMA = 86,       // ,
    TOKEN_SEMICOLON = 87,   // ;
    TOKEN_COLON = 88,       // :
    TOKEN_AT = 89,          // @
    TOKEN_DOT = 72          // .
} TokenType;

// ============================================
// TOKEN STRUCTURE
// ============================================

typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int column;
    long long value;  // For numbers
} Token;

// ============================================
// LEXER STATE
// ============================================

typedef struct {
    const char* source;
    size_t length;
    size_t position;
    int line;
    int column;
    char current_char;
} Lexer;

// ============================================
// KEYWORD MAP (19 keywords)
// ============================================

TokenType is_keyword(const char* lexeme) {
    if (strcmp(lexeme, "fn") == 0) return TOKEN_FN;
    if (strcmp(lexeme, "let") == 0) return TOKEN_LET;
    if (strcmp(lexeme, "const") == 0) return TOKEN_CONST;
    if (strcmp(lexeme, "if") == 0) return TOKEN_IF;
    if (strcmp(lexeme, "else") == 0) return TOKEN_ELSE;
    if (strcmp(lexeme, "while") == 0) return TOKEN_WHILE;
    if (strcmp(lexeme, "for") == 0) return TOKEN_FOR;
    if (strcmp(lexeme, "return") == 0) return TOKEN_RETURN;
    if (strcmp(lexeme, "break") == 0) return TOKEN_BREAK;
    if (strcmp(lexeme, "continue") == 0) return TOKEN_CONTINUE;
    if (strcmp(lexeme, "struct") == 0) return TOKEN_STRUCT;
    if (strcmp(lexeme, "enum") == 0) return TOKEN_ENUM;
    if (strcmp(lexeme, "import") == 0) return TOKEN_IMPORT;
    if (strcmp(lexeme, "export") == 0) return TOKEN_EXPORT;
    if (strcmp(lexeme, "defer") == 0) return TOKEN_DEFER;
    if (strcmp(lexeme, "match") == 0) return TOKEN_MATCH;
    if (strcmp(lexeme, "true") == 0) return TOKEN_TRUE;
    if (strcmp(lexeme, "false") == 0) return TOKEN_FALSE;
    if (strcmp(lexeme, "null") == 0) return TOKEN_NULL;
    return TOKEN_IDENTIFIER;
}

// ============================================
// CHARACTER CHECKS
// ============================================

bool is_alpha_char(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_digit_char(char c) {
    return c >= '0' && c <= '9';
}

bool is_whitespace_char(char c) {
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

// ============================================
// LEXER CORE FUNCTIONS
// ============================================

void lexer_init(Lexer* lexer, const char* source) {
    lexer->source = source;
    lexer->length = strlen(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = (lexer->length > 0) ? source[0] : '\0';
}

void lexer_advance(Lexer* lexer) {
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    
    lexer->position++;
    if (lexer->position < lexer->length) {
        lexer->current_char = lexer->source[lexer->position];
    } else {
        lexer->current_char = '\0';
    }
}

char lexer_peek(Lexer* lexer, int offset) {
    size_t pos = lexer->position + offset;
    if (pos < lexer->length) {
        return lexer->source[pos];
    }
    return '\0';
}

void lexer_skip_whitespace(Lexer* lexer) {
    while (is_whitespace_char(lexer->current_char)) {
        lexer_advance(lexer);
    }
}

void lexer_skip_comment(Lexer* lexer) {
    // Single-line comment //
    if (lexer->current_char == '/' && lexer_peek(lexer, 1) == '/') {
        while (lexer->current_char != '\n' && lexer->current_char != '\0') {
            lexer_advance(lexer);
        }
        return;
    }
    
    // Multi-line comment /* */
    if (lexer->current_char == '/' && lexer_peek(lexer, 1) == '*') {
        lexer_advance(lexer);  // skip /
        lexer_advance(lexer);  // skip *
        while (true) {
            if (lexer->current_char == '\0') {
                break;
            }
            if (lexer->current_char == '*' && lexer_peek(lexer, 1) == '/') {
                lexer_advance(lexer);  // skip *
                lexer_advance(lexer);  // skip /
                break;
            }
            lexer_advance(lexer);
        }
    }
}

Token lexer_read_identifier(Lexer* lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    
    size_t start_pos = lexer->position;
    while (is_alpha_char(lexer->current_char) || is_digit_char(lexer->current_char)) {
        lexer_advance(lexer);
    }
    
    size_t length = lexer->position - start_pos;
    token.lexeme = (char*)malloc(length + 1);
    strncpy(token.lexeme, lexer->source + start_pos, length);
    token.lexeme[length] = '\0';
    
    token.type = is_keyword(token.lexeme);
    token.value = 0;
    return token;
}

Token lexer_read_number(Lexer* lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    
    size_t start_pos = lexer->position;
    
    // Read digits
    while (is_digit_char(lexer->current_char)) {
        lexer_advance(lexer);
    }
    
    // Check for decimal point
    if (lexer->current_char == '.' && is_digit_char(lexer_peek(lexer, 1))) {
        lexer_advance(lexer);  // skip .
        while (is_digit_char(lexer->current_char)) {
            lexer_advance(lexer);
        }
    }
    
    size_t length = lexer->position - start_pos;
    token.lexeme = (char*)malloc(length + 1);
    strncpy(token.lexeme, lexer->source + start_pos, length);
    token.lexeme[length] = '\0';
    
    token.type = TOKEN_NUMBER;
    token.value = atoll(token.lexeme);
    return token;
}

Token lexer_read_string(Lexer* lexer) {
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    
    lexer_advance(lexer);  // skip opening "
    size_t start_pos = lexer->position;
    
    while (lexer->current_char != '"' && lexer->current_char != '\0') {
        if (lexer->current_char == '\\') {
            lexer_advance(lexer);  // skip escape char
        }
        lexer_advance(lexer);
    }
    
    size_t length = lexer->position - start_pos;
    token.lexeme = (char*)malloc(length + 1);
    strncpy(token.lexeme, lexer->source + start_pos, length);
    token.lexeme[length] = '\0';
    
    if (lexer->current_char == '"') {
        lexer_advance(lexer);  // skip closing "
    }
    
    token.type = TOKEN_STRING;
    token.value = 0;
    return token;
}

Token lexer_next_token(Lexer* lexer) {
    // Skip whitespace and comments
    while (true) {
        lexer_skip_whitespace(lexer);
        if (lexer->current_char == '/' && 
            (lexer_peek(lexer, 1) == '/' || lexer_peek(lexer, 1) == '*')) {
            lexer_skip_comment(lexer);
        } else {
            break;
        }
    }
    
    Token token;
    token.line = lexer->line;
    token.column = lexer->column;
    token.value = 0;
    token.lexeme = NULL;
    
    // EOF
    if (lexer->current_char == '\0') {
        token.type = TOKEN_EOF;
        token.lexeme = strdup("EOF");
        return token;
    }
    
    // Identifier or keyword
    if (is_alpha_char(lexer->current_char)) {
        return lexer_read_identifier(lexer);
    }
    
    // Number
    if (is_digit_char(lexer->current_char)) {
        return lexer_read_number(lexer);
    }
    
    // String
    if (lexer->current_char == '"') {
        return lexer_read_string(lexer);
    }
    
    // Character literal (e.g., 'a', '\n')
    if (lexer->current_char == '\'') {
        Token token;
        token.line = lexer->line;
        token.column = lexer->column;
        
        lexer_advance(lexer);  // skip opening '
        
        if (lexer->current_char == '\\') {
            lexer_advance(lexer);  // skip backslash
            // Escape sequences
            char escape_char = lexer->current_char;
            lexer_advance(lexer);
            switch (escape_char) {
                case 'n': token.value = '\n'; break;
                case 't': token.value = '\t'; break;
                case 'r': token.value = '\r'; break;
                case '0': token.value = '\0'; break;
                case '\\': token.value = '\\'; break;
                case '\'': token.value = '\''; break;
                default: token.value = escape_char; break;
            }
        } else {
            token.value = lexer->current_char;
            lexer_advance(lexer);
        }
        
        if (lexer->current_char == '\'') {
            lexer_advance(lexer);  // skip closing '
        }
        
        token.type = TOKEN_NUMBER;  // Character literals are numbers
        char lexeme_buf[16];
        snprintf(lexeme_buf, sizeof(lexeme_buf), "'%c'", (char)token.value);
        token.lexeme = strdup(lexeme_buf);
        return token;
    }
    
    // Single character operators and delimiters
    char c = lexer->current_char;
    lexer_advance(lexer);
    
    // Check for two-character operators
    if (c == '=' && lexer->current_char == '=') {
        lexer_advance(lexer);
        token.type = TOKEN_EQ_EQ;
        token.lexeme = strdup("==");
        return token;
    }
    if (c == '!' && lexer->current_char == '=') {
        lexer_advance(lexer);
        token.type = TOKEN_NOT_EQ;
        token.lexeme = strdup("!=");
        return token;
    }
    if (c == '<' && lexer->current_char == '=') {
        lexer_advance(lexer);
        token.type = TOKEN_LT_EQ;
        token.lexeme = strdup("<=");
        return token;
    }
    if (c == '>' && lexer->current_char == '=') {
        lexer_advance(lexer);
        token.type = TOKEN_GT_EQ;
        token.lexeme = strdup(">=");
        return token;
    }
    if (c == '&' && lexer->current_char == '&') {
        lexer_advance(lexer);
        token.type = TOKEN_AND_AND;
        token.lexeme = strdup("&&");
        return token;
    }
    if (c == '|' && lexer->current_char == '|') {
        lexer_advance(lexer);
        token.type = TOKEN_OR_OR;
        token.lexeme = strdup("||");
        return token;
    }
    
    // Single character tokens
    char lexeme_buf[2] = {c, '\0'};
    token.lexeme = strdup(lexeme_buf);
    
    switch (c) {
        case '+': token.type = TOKEN_PLUS; break;
        case '-': token.type = TOKEN_MINUS; break;
        case '*': token.type = TOKEN_STAR; break;
        case '/': token.type = TOKEN_SLASH; break;
        case '%': token.type = TOKEN_PERCENT; break;
        case '=': token.type = TOKEN_EQUAL; break;
        case '<': token.type = TOKEN_LT; break;
        case '>': token.type = TOKEN_GT; break;
        case '!': token.type = TOKEN_NOT; break;
        case '&': token.type = TOKEN_AMP; break;
        case '|': token.type = TOKEN_PIPE; break;
        case '(': token.type = TOKEN_LPAREN; break;
        case ')': token.type = TOKEN_RPAREN; break;
        case '{': token.type = TOKEN_LBRACE; break;
        case '}': token.type = TOKEN_RBRACE; break;
        case '[': token.type = TOKEN_LBRACKET; break;
        case ']': token.type = TOKEN_RBRACKET; break;
        case ',': token.type = TOKEN_COMMA; break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        case ':': token.type = TOKEN_COLON; break;
        case '@': token.type = TOKEN_AT; break;
        case '.': token.type = TOKEN_DOT; break;
        default:
            token.type = TOKEN_ERROR;
            free(token.lexeme);
            token.lexeme = strdup("Invalid character");
            break;
    }
    
    return token;
}

void token_free(Token* token) {
    if (token->lexeme) {
        free(token->lexeme);
        token->lexeme = NULL;
    }
}

// ============================================
// MAIN - Real Tokenization
// ============================================

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.mlp> [output_tokens.txt]\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = (argc >= 3) ? argv[2] : "lexer_tokens.txt";

    // Read input file
    FILE* in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        return 1;
    }

    fseek(in, 0, SEEK_END);
    long file_size = ftell(in);
    fseek(in, 0, SEEK_SET);

    char* source = (char*)malloc(file_size + 1);
    fread(source, 1, file_size, in);
    source[file_size] = '\0';
    fclose(in);

    // Initialize lexer
    Lexer lexer;
    lexer_init(&lexer, source);

    // Open output file
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        free(source);
        return 1;
    }

    // Tokenize
    fprintf(out, "=== MELP Lexer Token Output ===\n\n");
    int token_count = 0;
    
    while (true) {
        Token token = lexer_next_token(&lexer);
        
        if (token.type == TOKEN_EOF) {
            fprintf(out, "[%d] EOF at line %d, col %d\n", token_count, token.line, token.column);
            token_free(&token);
            break;
        }
        
        fprintf(out, "[%d] Type=%d, Lexeme=\"%s\", Line=%d, Col=%d", 
                token_count, token.type, token.lexeme, token.line, token.column);
        
        if (token.type == TOKEN_NUMBER) {
            fprintf(out, ", Value=%lld", token.value);
        }
        
        fprintf(out, "\n");
        
        token_count++;
        token_free(&token);
        
        if (token.type == TOKEN_ERROR) {
            break;
        }
    }

    fprintf(out, "\n=== Total Tokens: %d ===\n", token_count);

    fclose(out);
    free(source);

    printf("✓ Enhanced Lexer: %s -> %s (%d tokens)\n", input_file, output_file, token_count);
    return 0;
}
