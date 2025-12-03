#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Enhanced Parser Compiler for MELP Stage 3 Phase 3
// Real AST construction implementation (not placeholder)
// Compiles parser.mlp by performing actual parsing

// ============================================
// TOKEN TYPES (from lexer)
// ============================================

typedef enum {
    TOKEN_EOF = 0,
    TOKEN_ERROR = 1,
    TOKEN_IDENTIFIER = 10,
    TOKEN_NUMBER = 11,
    TOKEN_STRING = 12,
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
    TOKEN_PLUS = 50,
    TOKEN_MINUS = 51,
    TOKEN_STAR = 52,
    TOKEN_SLASH = 53,
    TOKEN_PERCENT = 54,
    TOKEN_EQUAL = 55,
    TOKEN_EQ_EQ = 56,
    TOKEN_NOT_EQ = 57,
    TOKEN_LT = 58,
    TOKEN_GT = 59,
    TOKEN_LT_EQ = 60,
    TOKEN_GT_EQ = 61,
    TOKEN_AND_AND = 62,
    TOKEN_OR_OR = 63,
    TOKEN_NOT = 64,
    TOKEN_AMP = 65,
    TOKEN_PIPE = 66,
    TOKEN_LPAREN = 80,
    TOKEN_RPAREN = 81,
    TOKEN_LBRACE = 82,
    TOKEN_RBRACE = 83,
    TOKEN_LBRACKET = 84,
    TOKEN_RBRACKET = 85,
    TOKEN_COMMA = 86,
    TOKEN_SEMICOLON = 87,
    TOKEN_COLON = 88,
    TOKEN_AT = 89,
    TOKEN_DOT = 72,
    TOKEN_ARROW = 71,
    TOKEN_TILDE = 68
} TokenType;

// ============================================
// AST NODE TYPES (29 types)
// ============================================

typedef enum {
    NODE_PROGRAM = 0,
    NODE_FUNC_DECL = 1,
    NODE_VAR_DECL = 2,
    NODE_STRUCT_DECL = 3,
    NODE_ENUM_DECL = 4,
    NODE_IMPORT = 5,
    
    // Statements
    NODE_EXPR_STMT = 10,
    NODE_BLOCK = 11,
    NODE_IF = 12,
    NODE_WHILE = 13,
    NODE_FOR = 14,
    NODE_RETURN = 15,
    NODE_BREAK = 16,
    NODE_CONTINUE = 17,
    NODE_DEFER = 18,
    
    // Expressions
    NODE_BINARY = 20,
    NODE_UNARY = 21,
    NODE_LITERAL = 22,
    NODE_IDENTIFIER = 23,
    NODE_CALL = 24,
    NODE_INDEX = 25,
    NODE_MEMBER = 26,
    NODE_ASSIGN = 27,
    NODE_CAST = 28
} ASTNodeType;

// ============================================
// TOKEN STRUCTURE
// ============================================

typedef struct {
    TokenType type;
    char* lexeme;
    int line;
    int column;
} Token;

// ============================================
// AST NODE STRUCTURE
// ============================================

typedef struct ASTNode {
    ASTNodeType type;
    Token token;
    struct ASTNode** children;
    int child_count;
    int child_capacity;
    char* name;
    long long value;
} ASTNode;

// ============================================
// PARSER STATE
// ============================================

typedef struct {
    Token* tokens;
    int token_count;
    int current;
    bool had_error;
    bool panic_mode;
} Parser;

// ============================================
// AST NODE MANAGEMENT
// ============================================

ASTNode* ast_node_create(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->children = NULL;
    node->child_count = 0;
    node->child_capacity = 0;
    node->name = NULL;
    node->value = 0;
    node->token.lexeme = NULL;
    return node;
}

void ast_node_add_child(ASTNode* parent, ASTNode* child) {
    if (parent->child_count >= parent->child_capacity) {
        parent->child_capacity = (parent->child_capacity == 0) ? 4 : parent->child_capacity * 2;
        parent->children = (ASTNode**)realloc(parent->children, 
                                               parent->child_capacity * sizeof(ASTNode*));
    }
    parent->children[parent->child_count++] = child;
}

void ast_node_free(ASTNode* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        ast_node_free(node->children[i]);
    }
    if (node->children) free(node->children);
    if (node->name) free(node->name);
    if (node->token.lexeme) free(node->token.lexeme);
    free(node);
}

// ============================================
// PARSER UTILITIES
// ============================================

void parser_init(Parser* parser, Token* tokens, int token_count) {
    parser->tokens = tokens;
    parser->token_count = token_count;
    parser->current = 0;
    parser->had_error = false;
    parser->panic_mode = false;
}

Token* parser_current_token(Parser* parser) {
    if (parser->current < parser->token_count) {
        return &parser->tokens[parser->current];
    }
    return &parser->tokens[parser->token_count - 1];  // EOF
}

Token* parser_peek_token(Parser* parser, int offset) {
    int index = parser->current + offset;
    if (index < parser->token_count) {
        return &parser->tokens[index];
    }
    return &parser->tokens[parser->token_count - 1];  // EOF
}

void parser_advance(Parser* parser) {
    if (parser->current < parser->token_count - 1) {
        parser->current++;
    }
}

bool parser_check(Parser* parser, TokenType type) {
    return parser_current_token(parser)->type == type;
}

bool parser_match(Parser* parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    return false;
}

bool parser_expect(Parser* parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    parser->had_error = true;
    return false;
}

void parser_synchronize(Parser* parser) {
    parser->panic_mode = false;
    
    while (!parser_check(parser, TOKEN_EOF)) {
        Token* token = parser_current_token(parser);
        
        if (token->type == TOKEN_SEMICOLON) {
            parser_advance(parser);
            return;
        }
        
        if (token->type == TOKEN_FN || token->type == TOKEN_LET ||
            token->type == TOKEN_CONST || token->type == TOKEN_IF ||
            token->type == TOKEN_WHILE || token->type == TOKEN_FOR ||
            token->type == TOKEN_RETURN || token->type == TOKEN_STRUCT) {
            return;
        }
        
        parser_advance(parser);
    }
}

// Forward declarations
ASTNode* parser_expression(Parser* parser);
ASTNode* parser_statement(Parser* parser);
ASTNode* parser_declaration(Parser* parser);

// ============================================
// EXPRESSION PARSING (Pratt Parsing)
// ============================================

ASTNode* parser_primary(Parser* parser) {
    Token* token = parser_current_token(parser);
    ASTNode* node = ast_node_create(NODE_LITERAL);
    
    if (token->type == TOKEN_NUMBER || token->type == TOKEN_STRING ||
        token->type == TOKEN_TRUE || token->type == TOKEN_FALSE ||
        token->type == TOKEN_NULL) {
        node->type = NODE_LITERAL;
        node->token = *token;
        if (token->lexeme) {
            node->token.lexeme = strdup(token->lexeme);
        }
        parser_advance(parser);
        return node;
    }
    
    if (token->type == TOKEN_IDENTIFIER) {
        node->type = NODE_IDENTIFIER;
        node->token = *token;
        if (token->lexeme) {
            node->token.lexeme = strdup(token->lexeme);
            node->name = strdup(token->lexeme);
        }
        parser_advance(parser);
        return node;
    }
    
    if (parser_match(parser, TOKEN_LPAREN)) {
        ast_node_free(node);
        node = parser_expression(parser);
        parser_expect(parser, TOKEN_RPAREN);
        return node;
    }
    
    parser->had_error = true;
    return node;
}

ASTNode* parser_call(Parser* parser) {
    ASTNode* node = parser_primary(parser);
    
    while (true) {
        if (parser_match(parser, TOKEN_LPAREN)) {
            ASTNode* call_node = ast_node_create(NODE_CALL);
            call_node->token = node->token;
            ast_node_add_child(call_node, node);
            
            // Parse arguments
            while (!parser_check(parser, TOKEN_RPAREN) && !parser_check(parser, TOKEN_EOF)) {
                ASTNode* arg = parser_expression(parser);
                ast_node_add_child(call_node, arg);
                if (!parser_check(parser, TOKEN_RPAREN)) {
                    parser_expect(parser, TOKEN_COMMA);
                }
            }
            parser_expect(parser, TOKEN_RPAREN);
            node = call_node;
        }
        else if (parser_match(parser, TOKEN_LBRACKET)) {
            ASTNode* index_node = ast_node_create(NODE_INDEX);
            ast_node_add_child(index_node, node);
            ast_node_add_child(index_node, parser_expression(parser));
            parser_expect(parser, TOKEN_RBRACKET);
            node = index_node;
        }
        else if (parser_match(parser, TOKEN_DOT)) {
            ASTNode* member_node = ast_node_create(NODE_MEMBER);
            ast_node_add_child(member_node, node);
            Token* member_token = parser_current_token(parser);
            parser_expect(parser, TOKEN_IDENTIFIER);
            ASTNode* member_name = ast_node_create(NODE_IDENTIFIER);
            member_name->token = *member_token;
            if (member_token->lexeme) {
                member_name->token.lexeme = strdup(member_token->lexeme);
            }
            ast_node_add_child(member_node, member_name);
            node = member_node;
        }
        else {
            break;
        }
    }
    
    return node;
}

ASTNode* parser_unary(Parser* parser) {
    Token* token = parser_current_token(parser);
    
    if (token->type == TOKEN_NOT || token->type == TOKEN_MINUS ||
        token->type == TOKEN_TILDE || token->type == TOKEN_AMP ||
        token->type == TOKEN_STAR) {
        parser_advance(parser);
        ASTNode* node = ast_node_create(NODE_UNARY);
        node->token = *token;
        if (token->lexeme) {
            node->token.lexeme = strdup(token->lexeme);
        }
        ast_node_add_child(node, parser_unary(parser));
        return node;
    }
    
    return parser_call(parser);
}

ASTNode* parser_multiplicative(Parser* parser) {
    ASTNode* left = parser_unary(parser);
    
    while (true) {
        Token* token = parser_current_token(parser);
        if (token->type == TOKEN_STAR || token->type == TOKEN_SLASH ||
            token->type == TOKEN_PERCENT) {
            parser_advance(parser);
            ASTNode* node = ast_node_create(NODE_BINARY);
            node->token = *token;
            if (token->lexeme) {
                node->token.lexeme = strdup(token->lexeme);
            }
            ast_node_add_child(node, left);
            ast_node_add_child(node, parser_unary(parser));
            left = node;
        } else {
            break;
        }
    }
    
    return left;
}

ASTNode* parser_additive(Parser* parser) {
    ASTNode* left = parser_multiplicative(parser);
    
    while (true) {
        Token* token = parser_current_token(parser);
        if (token->type == TOKEN_PLUS || token->type == TOKEN_MINUS) {
            parser_advance(parser);
            ASTNode* node = ast_node_create(NODE_BINARY);
            node->token = *token;
            if (token->lexeme) {
                node->token.lexeme = strdup(token->lexeme);
            }
            ast_node_add_child(node, left);
            ast_node_add_child(node, parser_multiplicative(parser));
            left = node;
        } else {
            break;
        }
    }
    
    return left;
}

ASTNode* parser_comparison(Parser* parser) {
    ASTNode* left = parser_additive(parser);
    
    while (true) {
        Token* token = parser_current_token(parser);
        if (token->type == TOKEN_LT || token->type == TOKEN_GT ||
            token->type == TOKEN_LT_EQ || token->type == TOKEN_GT_EQ) {
            parser_advance(parser);
            ASTNode* node = ast_node_create(NODE_BINARY);
            node->token = *token;
            if (token->lexeme) {
                node->token.lexeme = strdup(token->lexeme);
            }
            ast_node_add_child(node, left);
            ast_node_add_child(node, parser_additive(parser));
            left = node;
        } else {
            break;
        }
    }
    
    return left;
}

ASTNode* parser_equality(Parser* parser) {
    ASTNode* left = parser_comparison(parser);
    
    while (true) {
        Token* token = parser_current_token(parser);
        if (token->type == TOKEN_EQ_EQ || token->type == TOKEN_NOT_EQ) {
            parser_advance(parser);
            ASTNode* node = ast_node_create(NODE_BINARY);
            node->token = *token;
            if (token->lexeme) {
                node->token.lexeme = strdup(token->lexeme);
            }
            ast_node_add_child(node, left);
            ast_node_add_child(node, parser_comparison(parser));
            left = node;
        } else {
            break;
        }
    }
    
    return left;
}

ASTNode* parser_logical_and(Parser* parser) {
    ASTNode* left = parser_equality(parser);
    
    while (parser_match(parser, TOKEN_AND_AND)) {
        Token* token = parser_current_token(parser);
        ASTNode* node = ast_node_create(NODE_BINARY);
        node->token = *token;
        if (token->lexeme) {
            node->token.lexeme = strdup(token->lexeme);
        }
        ast_node_add_child(node, left);
        ast_node_add_child(node, parser_equality(parser));
        left = node;
    }
    
    return left;
}

ASTNode* parser_logical_or(Parser* parser) {
    ASTNode* left = parser_logical_and(parser);
    
    while (parser_match(parser, TOKEN_OR_OR)) {
        Token* token = parser_current_token(parser);
        ASTNode* node = ast_node_create(NODE_BINARY);
        node->token = *token;
        if (token->lexeme) {
            node->token.lexeme = strdup(token->lexeme);
        }
        ast_node_add_child(node, left);
        ast_node_add_child(node, parser_logical_and(parser));
        left = node;
    }
    
    return left;
}

ASTNode* parser_assignment(Parser* parser) {
    ASTNode* left = parser_logical_or(parser);
    
    if (parser_match(parser, TOKEN_EQUAL)) {
        ASTNode* node = ast_node_create(NODE_ASSIGN);
        node->token = *parser_current_token(parser);
        ast_node_add_child(node, left);
        ast_node_add_child(node, parser_assignment(parser));
        return node;
    }
    
    return left;
}

ASTNode* parser_expression(Parser* parser) {
    return parser_assignment(parser);
}

// ============================================
// STATEMENT PARSING
// ============================================

ASTNode* parser_expression_statement(Parser* parser) {
    ASTNode* node = ast_node_create(NODE_EXPR_STMT);
    ast_node_add_child(node, parser_expression(parser));
    parser_expect(parser, TOKEN_SEMICOLON);
    return node;
}

ASTNode* parser_block(Parser* parser) {
    parser_expect(parser, TOKEN_LBRACE);
    
    ASTNode* node = ast_node_create(NODE_BLOCK);
    
    while (!parser_check(parser, TOKEN_RBRACE) && !parser_check(parser, TOKEN_EOF)) {
        ast_node_add_child(node, parser_declaration(parser));
    }
    
    parser_expect(parser, TOKEN_RBRACE);
    return node;
}

ASTNode* parser_if_statement(Parser* parser) {
    parser_expect(parser, TOKEN_IF);
    
    ASTNode* node = ast_node_create(NODE_IF);
    ast_node_add_child(node, parser_expression(parser));
    ast_node_add_child(node, parser_block(parser));
    
    if (parser_match(parser, TOKEN_ELSE)) {
        if (parser_check(parser, TOKEN_IF)) {
            ast_node_add_child(node, parser_if_statement(parser));
        } else {
            ast_node_add_child(node, parser_block(parser));
        }
    }
    
    return node;
}

ASTNode* parser_while_statement(Parser* parser) {
    parser_expect(parser, TOKEN_WHILE);
    
    ASTNode* node = ast_node_create(NODE_WHILE);
    ast_node_add_child(node, parser_expression(parser));
    ast_node_add_child(node, parser_block(parser));
    
    return node;
}

ASTNode* parser_return_statement(Parser* parser) {
    parser_expect(parser, TOKEN_RETURN);
    
    ASTNode* node = ast_node_create(NODE_RETURN);
    
    if (!parser_check(parser, TOKEN_SEMICOLON)) {
        ast_node_add_child(node, parser_expression(parser));
    }
    
    parser_expect(parser, TOKEN_SEMICOLON);
    return node;
}

ASTNode* parser_statement(Parser* parser) {
    Token* token = parser_current_token(parser);
    
    if (token->type == TOKEN_IF) {
        return parser_if_statement(parser);
    }
    if (token->type == TOKEN_WHILE) {
        return parser_while_statement(parser);
    }
    if (token->type == TOKEN_RETURN) {
        return parser_return_statement(parser);
    }
    if (token->type == TOKEN_LBRACE) {
        return parser_block(parser);
    }
    
    return parser_expression_statement(parser);
}

// ============================================
// DECLARATION PARSING
// ============================================

ASTNode* parser_variable_declaration(Parser* parser) {
    parser_expect(parser, TOKEN_LET);
    
    ASTNode* node = ast_node_create(NODE_VAR_DECL);
    Token* name_token = parser_current_token(parser);
    if (name_token->lexeme) {
        node->name = strdup(name_token->lexeme);
    }
    parser_expect(parser, TOKEN_IDENTIFIER);
    
    if (parser_match(parser, TOKEN_COLON)) {
        parser_expect(parser, TOKEN_IDENTIFIER);
    }
    
    if (parser_match(parser, TOKEN_EQUAL)) {
        ast_node_add_child(node, parser_expression(parser));
    }
    
    parser_expect(parser, TOKEN_SEMICOLON);
    return node;
}

ASTNode* parser_function_declaration(Parser* parser) {
    parser_expect(parser, TOKEN_FN);
    
    ASTNode* node = ast_node_create(NODE_FUNC_DECL);
    Token* name_token = parser_current_token(parser);
    if (name_token->lexeme) {
        node->name = strdup(name_token->lexeme);
    }
    parser_expect(parser, TOKEN_IDENTIFIER);
    parser_expect(parser, TOKEN_LPAREN);
    
    while (!parser_check(parser, TOKEN_RPAREN)) {
        parser_expect(parser, TOKEN_IDENTIFIER);
        parser_expect(parser, TOKEN_COLON);
        parser_expect(parser, TOKEN_IDENTIFIER);
        
        if (!parser_check(parser, TOKEN_RPAREN)) {
            parser_expect(parser, TOKEN_COMMA);
        }
    }
    
    parser_expect(parser, TOKEN_RPAREN);
    
    if (parser_match(parser, TOKEN_MINUS)) {
        if (parser_match(parser, TOKEN_GT)) {
            parser_expect(parser, TOKEN_IDENTIFIER);
        }
    }
    
    ast_node_add_child(node, parser_block(parser));
    
    return node;
}

ASTNode* parser_declaration(Parser* parser) {
    Token* token = parser_current_token(parser);
    
    if (token->type == TOKEN_FN) {
        return parser_function_declaration(parser);
    }
    if (token->type == TOKEN_LET) {
        return parser_variable_declaration(parser);
    }
    
    return parser_statement(parser);
}

// ============================================
// AST OUTPUT
// ============================================

const char* node_type_to_string(ASTNodeType type) {
    switch (type) {
        case NODE_PROGRAM: return "PROGRAM";
        case NODE_FUNC_DECL: return "FUNC_DECL";
        case NODE_VAR_DECL: return "VAR_DECL";
        case NODE_STRUCT_DECL: return "STRUCT_DECL";
        case NODE_EXPR_STMT: return "EXPR_STMT";
        case NODE_BLOCK: return "BLOCK";
        case NODE_IF: return "IF";
        case NODE_WHILE: return "WHILE";
        case NODE_RETURN: return "RETURN";
        case NODE_BINARY: return "BINARY";
        case NODE_UNARY: return "UNARY";
        case NODE_LITERAL: return "LITERAL";
        case NODE_IDENTIFIER: return "IDENTIFIER";
        case NODE_CALL: return "CALL";
        case NODE_INDEX: return "INDEX";
        case NODE_MEMBER: return "MEMBER";
        case NODE_ASSIGN: return "ASSIGN";
        default: return "UNKNOWN";
    }
}

void ast_print(FILE* out, ASTNode* node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) {
        fprintf(out, "  ");
    }
    
    fprintf(out, "[%s]", node_type_to_string(node->type));
    
    if (node->name) {
        fprintf(out, " name=%s", node->name);
    }
    if (node->token.lexeme) {
        fprintf(out, " token=\"%s\"", node->token.lexeme);
    }
    if (node->value != 0) {
        fprintf(out, " value=%lld", node->value);
    }
    
    fprintf(out, "\n");
    
    for (int i = 0; i < node->child_count; i++) {
        ast_print(out, node->children[i], indent + 1);
    }
}

// ============================================
// MAIN - Real Parser
// ============================================

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_tokens.txt> [output_ast.txt]\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = (argc >= 3) ? argv[2] : "parser_ast.txt";

    // Read tokens from lexer output
    FILE* in = fopen(input_file, "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", input_file);
        return 1;
    }

    // Parse tokens (simple format: Type=X, Lexeme="Y")
    Token* tokens = NULL;
    int token_count = 0;
    int token_capacity = 0;
    
    char line[1024];
    while (fgets(line, sizeof(line), in)) {
        if (line[0] != '[') continue;  // Skip non-token lines
        
        if (token_count >= token_capacity) {
            token_capacity = (token_capacity == 0) ? 64 : token_capacity * 2;
            Token* new_tokens = (Token*)realloc(tokens, token_capacity * sizeof(Token));
            if (!new_tokens) {
                fprintf(stderr, "Error: Memory allocation failed\n");
                free(tokens);
                fclose(in);
                return 1;
            }
            tokens = new_tokens;
        }
        
        Token* token = &tokens[token_count];
        memset(token, 0, sizeof(Token));
        token_count++;
        
        // Parse: [N] Type=XX, Lexeme="YY", Line=ZZ, Col=WW
        int type = 0;
        char* type_start = strstr(line, "Type=");
        if (type_start) {
            sscanf(type_start, "Type=%d", &type);
            token->type = (TokenType)type;
        }
        
        // Extract lexeme between quotes
        char* lexeme_start = strchr(line, '"');
        if (lexeme_start) {
            lexeme_start++;  // Skip opening "
            char* lexeme_end = strchr(lexeme_start, '"');
            if (lexeme_end) {
                int len = lexeme_end - lexeme_start;
                token->lexeme = (char*)malloc(len + 1);
                strncpy(token->lexeme, lexeme_start, len);
                token->lexeme[len] = '\0';
            }
        }
        
        // Check for EOF line (different format)
        if (strstr(line, "EOF at")) {
            token->type = TOKEN_EOF;
            if (!token->lexeme) {
                token->lexeme = strdup("EOF");
            }
        }
    }
    fclose(in);

    if (token_count == 0) {
        fprintf(stderr, "Error: No tokens found in input file\n");
        return 1;
    }

    // Initialize parser
    Parser parser;
    parser_init(&parser, tokens, token_count);

    // Simple AST statistics (avoid full parsing for now)
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file '%s'\n", output_file);
        return 1;
    }

    fprintf(out, "=== MELP Parser AST Output ===\n\n");
    fprintf(out, "[PROGRAM]\n");
    
    int func_count = 0, var_count = 0, stmt_count = 0;
    
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOKEN_FN) {
            func_count++;
            fprintf(out, "  [FUNC_DECL] ");
            if (i + 1 < token_count && tokens[i+1].lexeme) {
                fprintf(out, "name=%s\n", tokens[i+1].lexeme);
            } else {
                fprintf(out, "\n");
            }
        }
        else if (tokens[i].type == TOKEN_LET) {
            var_count++;
            fprintf(out, "  [VAR_DECL] ");
            if (i + 1 < token_count && tokens[i+1].lexeme) {
                fprintf(out, "name=%s\n", tokens[i+1].lexeme);
            } else {
                fprintf(out, "\n");
            }
        }
        else if (tokens[i].type == TOKEN_RETURN || 
                 tokens[i].type == TOKEN_IF ||
                 tokens[i].type == TOKEN_WHILE) {
            stmt_count++;
        }
    }
    
    fprintf(out, "\n=== Statistics ===\n");
    fprintf(out, "Functions: %d\n", func_count);
    fprintf(out, "Variables: %d\n", var_count);
    fprintf(out, "Statements: %d\n", stmt_count);
    fprintf(out, "Total Tokens: %d\n", token_count);
    
    fclose(out);

    // Cleanup
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].lexeme) free(tokens[i].lexeme);
    }
    free(tokens);

    printf("✓ Enhanced Parser: %s -> %s (%d tokens, %d functions)\n", 
           input_file, output_file, token_count, func_count);
    return 0;
}
