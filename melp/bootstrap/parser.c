// MELP Bootstrap Compiler - Parser
// Simple recursive descent parser for MELP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "lexer.h"

static Token current_token;
static ASTNode *ast_root = NULL;

static void advance() {
    current_token = next_token();
}

static int match(TokenType type, const char *value) {
    if (current_token.type != type) return 0;
    if (value && strcmp(current_token.value, value) != 0) return 0;
    return 1;
}

static int expect(TokenType type, const char *value) {
    if (!match(type, value)) {
        fprintf(stderr, "Parse error at line %d:%d: expected %s '%s', got %s '%s'\n",
                current_token.line, current_token.col,
                token_type_name(type), value ? value : "any",
                token_type_name(current_token.type), current_token.value);
        return 0;
    }
    advance();
    return 1;
}

ASTNode* create_node(NodeType type) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->value[0] = '\0';
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;
    return node;
}

// Forward declarations
static ASTNode* parse_var_decl();
static ASTNode* parse_if();
static ASTNode* parse_while();
static ASTNode* parse_for();

// Parse variable declaration: numeric x = 42
static ASTNode* parse_var_decl() {
    if (!match(TK_KEYWORD, NULL)) return NULL;
    
    // Check if it's a type keyword
    if (strcmp(current_token.value, "numeric") != 0 &&
        strcmp(current_token.value, "string") != 0 &&
        strcmp(current_token.value, "boolean") != 0) {
        return NULL;
    }
    
    ASTNode *node = create_node(NODE_VAR_DECL);
    strcpy(node->value, current_token.value); // type
    advance();
    
    // Save identifier name BEFORE advancing
    if (!match(TK_IDENTIFIER, NULL)) return NULL;
    node->left = create_node(NODE_IDENTIFIER);
    strcpy(node->left->value, current_token.value);
    advance();
    
    if (match(TK_OPERATOR, "=")) {
        advance();
        // Parse expression (simple: just number or string for now)
        if (current_token.type == TK_NUMBER || current_token.type == TK_STRING) {
            node->right = create_node(NODE_LITERAL);
            strcpy(node->right->value, current_token.value);
            advance();
        }
    }
    
    return node;
}

// Parse print statement: print "message" or print variable
static ASTNode* parse_print() {
    if (!match(TK_KEYWORD, "print")) return NULL;
    
    ASTNode *node = create_node(NODE_PRINT);
    advance();
    
    // Parse expression to print (simple: string literal, number, or identifier)
    if (current_token.type == TK_STRING || 
        current_token.type == TK_NUMBER || 
        current_token.type == TK_IDENTIFIER) {
        node->left = create_node(NODE_LITERAL);
        strcpy(node->left->value, current_token.value);
        node->left->type = (current_token.type == TK_STRING) ? NODE_LITERAL : 
                          (current_token.type == TK_NUMBER) ? NODE_LITERAL : NODE_IDENTIFIER;
        advance();
    }
    
    return node;
}

// Parse function declaration
static ASTNode* parse_function() {
    if (!match(TK_KEYWORD, "function")) return NULL;
    
    ASTNode *node = create_node(NODE_FUNCTION);
    advance();
    
    if (!expect(TK_IDENTIFIER, NULL)) return NULL;
    strcpy(node->value, current_token.value);
    
    // Skip parameters for now
    if (match(TK_SYMBOL, "(")) {
        advance();
        while (!match(TK_SYMBOL, ")") && current_token.type != TK_EOF) {
            advance();
        }
        expect(TK_SYMBOL, ")");
    }
    
    // Parse return type (optional)
    if (current_token.type == TK_KEYWORD) {
        if (strcmp(current_token.value, "numeric") == 0 ||
            strcmp(current_token.value, "string") == 0 ||
            strcmp(current_token.value, "boolean") == 0) {
            // Store return type
            node->right = create_node(NODE_TYPE);
            strcpy(node->right->value, current_token.value);
            advance();
        }
    }
    
    // Parse function body until "end function"
    ASTNode *body = NULL;
    ASTNode *last = NULL;
    
    while (!match(TK_KEYWORD, "end") && current_token.type != TK_EOF) {
        ASTNode *stmt = NULL;
        
        // Try if statement
        if (match(TK_KEYWORD, "if")) {
            stmt = parse_if();
        }
        // Try while statement
        else if (match(TK_KEYWORD, "while")) {
            stmt = parse_while();
        }
        // Try for statement
        else if (match(TK_KEYWORD, "for")) {
            stmt = parse_for();
        }
        // Try variable declaration
        else {
            stmt = parse_var_decl();
        }
        
        // Try return statement
        if (!stmt && match(TK_KEYWORD, "return")) {
            stmt = create_node(NODE_RETURN);
            advance();
            
            // Parse return value (simple expression for now)
            if (current_token.type == TK_NUMBER) {
                stmt->left = create_node(NODE_LITERAL);
                strcpy(stmt->left->value, current_token.value);
                advance();
            }
        }
        
        if (stmt) {
            if (!body) {
                body = stmt;
                last = stmt;
            } else {
                last->next = stmt;
                last = stmt;
            }
        } else {
            advance(); // Skip unknown statements
        }
    }
    
    node->left = body;
    
    if (match(TK_KEYWORD, "end")) {
        advance();
        expect(TK_KEYWORD, "function");
    }
    
    return node;
}

// Parse struct declaration
static ASTNode* parse_struct() {
    if (!match(TK_KEYWORD, "struct")) return NULL;
    
    ASTNode *node = create_node(NODE_STRUCT);
    advance();
    
    // Struct name
    if (!expect(TK_IDENTIFIER, NULL)) return NULL;
    strcpy(node->value, current_token.value);
    
    // Parse fields
    ASTNode *fields = NULL;
    ASTNode *last_field = NULL;
    
    while (!match(TK_KEYWORD, "end") && current_token.type != TK_EOF) {
        ASTNode *field = parse_var_decl();
        if (field) {
            if (!fields) {
                fields = field;
                last_field = field;
            } else {
                last_field->next = field;
                last_field = field;
            }
        } else {
            advance();
        }
    }
    
    node->left = fields;
    
    // Expect "end struct"
    if (match(TK_KEYWORD, "end")) {
        advance();
        expect(TK_KEYWORD, "struct");
    }
    
    return node;
}

// Parse if statement
static ASTNode* parse_if() {
    if (!match(TK_KEYWORD, "if")) return NULL;
    
    ASTNode *node = create_node(NODE_IF);
    advance();
    
    // Parse condition (simple: just identifier or number)
    if (current_token.type == TK_IDENTIFIER || current_token.type == TK_NUMBER) {
        node->left = create_node(NODE_LITERAL);
        strcpy(node->left->value, current_token.value);
        advance();
    }
    
    // Expect "then"
    if (match(TK_KEYWORD, "then")) {
        advance();
    }
    
    // Parse body statements until "end if"
    ASTNode *body = NULL;
    ASTNode *last = NULL;
    
    while (!match(TK_KEYWORD, "end") && current_token.type != TK_EOF) {
        ASTNode *stmt = parse_var_decl();
        
        if (stmt) {
            if (!body) {
                body = stmt;
                last = stmt;
            } else {
                last->next = stmt;
                last = stmt;
            }
        } else {
            advance();
        }
    }
    
    node->right = body;
    
    // Expect "end if"
    if (match(TK_KEYWORD, "end")) {
        advance();
        if (match(TK_KEYWORD, "if")) {
            advance();
        }
    }
    
    return node;
}

// Parse while statement
static ASTNode* parse_while() {
    if (!match(TK_KEYWORD, "while")) return NULL;
    
    ASTNode *node = create_node(NODE_WHILE);
    advance();
    
    // Parse condition
    if (current_token.type == TK_IDENTIFIER || current_token.type == TK_NUMBER) {
        node->left = create_node(NODE_LITERAL);
        strcpy(node->left->value, current_token.value);
        advance();
    }
    
    // Parse body until "end while"
    ASTNode *body = NULL;
    ASTNode *last = NULL;
    
    while (!match(TK_KEYWORD, "end") && current_token.type != TK_EOF) {
        ASTNode *stmt = parse_var_decl();
        
        if (stmt) {
            if (!body) {
                body = stmt;
                last = stmt;
            } else {
                last->next = stmt;
                last = stmt;
            }
        } else {
            advance();
        }
    }
    
    node->right = body;
    
    // Expect "end while"
    if (match(TK_KEYWORD, "end")) {
        advance();
        if (match(TK_KEYWORD, "while")) {
            advance();
        }
    }
    
    return node;
}

// Parse for statement
static ASTNode* parse_for() {
    if (!match(TK_KEYWORD, "for")) return NULL;
    
    ASTNode *node = create_node(NODE_FOR);
    advance();
    
    // Parse loop variable (simple: just identifier)
    if (current_token.type == TK_IDENTIFIER) {
        node->left = create_node(NODE_IDENTIFIER);
        strcpy(node->left->value, current_token.value);
        advance();
    }
    
    // Skip rest of for header (to/in/etc)
    while (!match(TK_KEYWORD, "end") && current_token.type != TK_EOF) {
        ASTNode *stmt = parse_var_decl();
        if (!stmt) {
            advance();
        }
    }
    
    // Expect "end for"
    if (match(TK_KEYWORD, "end")) {
        advance();
        if (match(TK_KEYWORD, "for")) {
            advance();
        }
    }
    
    return node;
}

ASTNode* parse(const char *source) {
    lexer_init(source);
    advance();
    
    ASTNode *root = NULL;
    ASTNode *last = NULL;
    
    while (current_token.type != TK_EOF) {
        ASTNode *node = NULL;
        
        if (match(TK_KEYWORD, "struct")) {
            node = parse_struct();
        } else if (match(TK_KEYWORD, "function")) {
            node = parse_function();
        } else if (match(TK_KEYWORD, "if")) {
            node = parse_if();
        } else if (match(TK_KEYWORD, "while")) {
            node = parse_while();
        } else if (match(TK_KEYWORD, "for")) {
            node = parse_for();
        } else if (match(TK_KEYWORD, "print")) {
            node = parse_print();
        } else if (match(TK_KEYWORD, NULL)) {
            node = parse_var_decl();
        } else {
            advance(); // Skip unknown
        }
        
        if (node) {
            if (!root) {
                root = node;
                last = node;
            } else {
                last->next = node;
                last = node;
            }
        }
    }
    
    ast_root = root;
    return root;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);
    free(node);
}
