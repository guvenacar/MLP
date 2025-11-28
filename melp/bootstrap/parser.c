// MELP Phase 1 - Bootstrap Compiler
// Parser: Deklarasyonlar + Assignment + Print

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.c"

typedef enum {
    TYPE_NUMERIC,
    TYPE_DECIMAL,
    TYPE_BOOLEAN
} VarType;

typedef enum {
    STMT_DECLARATION,
    STMT_ASSIGNMENT,
    STMT_PRINT,
    STMT_IF,
    STMT_FOR,
    STMT_WHILE,
    STMT_EXIT,
    STMT_CONTINUE
} StmtType;

typedef enum {
    EXPR_NUMBER,
    EXPR_VARIABLE,
    EXPR_COMPARISON,
    EXPR_BINARY_OP
} ExprType;

typedef enum {
    BIN_OP_ADD,
    BIN_OP_SUB,
    BIN_OP_MUL,
    BIN_OP_DIV
} BinaryOp;

typedef enum {
    CMP_EQUAL,
    CMP_NOT_EQUAL,
    CMP_LESS,
    CMP_LESS_EQUAL,
    CMP_GREATER,
    CMP_GREATER_EQUAL
} ComparisonOp;

typedef struct Expression {
    ExprType type;
    union {
        long number_value;
        char* var_name;
        struct {
            struct Expression* left;
            struct Expression* right;
            ComparisonOp op;
        } comparison;
        struct {
            struct Expression* left;
            struct Expression* right;
            BinaryOp op;
        } binary_op;
    };
} Expression;

typedef struct {
    VarType type;
    char* name;
    Expression* init_value;  // NULL if no initialization
} Declaration;

typedef struct Statement {
    StmtType type;
    union {
        Declaration* declaration;
        struct {
            char* var_name;
            Expression* value;
        } assignment;
        struct {
            Expression* expr;
        } print_stmt;
        struct {
            Expression* condition;
            struct Statement** then_body;
            int then_count;
            struct Statement** else_body;
            int else_count;
        } if_stmt;
        struct {
            char* var_name;       // Loop variable (e.g., "i")
            Expression* start;     // Start value
            Expression* end;       // End value
            Expression* step;      // Step value (NULL = default 1)
            struct Statement** body;
            int body_count;
        } for_stmt;
        struct {
            Expression* condition;
            struct Statement** body;
            int body_count;
        } while_stmt;
    };
} Statement;

typedef struct {
    Statement** statements;
    int count;
    int capacity;
} AST;

typedef struct {
    Lexer* lexer;
    Token* current_token;
} Parser;

Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void parser_advance(Parser* parser) {
    token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

AST* ast_create() {
    AST* ast = malloc(sizeof(AST));
    ast->statements = malloc(sizeof(Statement*) * 10);
    ast->count = 0;
    ast->capacity = 10;
    return ast;
}

void ast_add_statement(AST* ast, Statement* stmt) {
    if (ast->count >= ast->capacity) {
        ast->capacity *= 2;
        ast->statements = realloc(ast->statements, 
                                   sizeof(Statement*) * ast->capacity);
    }
    ast->statements[ast->count++] = stmt;
}

Expression* expression_create_number(long value) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_NUMBER;
    expr->number_value = value;
    return expr;
}

Expression* expression_create_variable(const char* name) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_VARIABLE;
    expr->var_name = malloc(strlen(name) + 1);
    strcpy(expr->var_name, name);
    return expr;
}

Expression* expression_create_comparison(Expression* left, Expression* right, ComparisonOp op) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_COMPARISON;
    expr->comparison.left = left;
    expr->comparison.right = right;
    expr->comparison.op = op;
    return expr;
}

Expression* expression_create_binary_op(Expression* left, Expression* right, BinaryOp op) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_BINARY_OP;
    expr->binary_op.left = left;
    expr->binary_op.right = right;
    expr->binary_op.op = op;
    return expr;
}

Expression* parser_parse_primary_expression(Parser* parser) {
    if (parser->current_token->type == TOKEN_NUMBER) {
        long value = atol(parser->current_token->value);
        parser_advance(parser);
        return expression_create_number(value);
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        char* name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(name, parser->current_token->value);
        parser_advance(parser);
        Expression* expr = expression_create_variable(name);
        free(name);
        return expr;
    } else {
        fprintf(stderr, "Parser error: Expected expression at line %d\n",
                parser->current_token->line);
        exit(1);
    }
}

Expression* parser_parse_expression(Parser* parser) {
    Expression* left = parser_parse_primary_expression(parser);
    
    // Check for arithmetic operators (+, -, *, /)
    BinaryOp bin_op;
    int has_bin_op = 0;
    
    if (parser->current_token->type == TOKEN_PLUS) {
        bin_op = BIN_OP_ADD;
        has_bin_op = 1;
    } else if (parser->current_token->type == TOKEN_MINUS) {
        bin_op = BIN_OP_SUB;
        has_bin_op = 1;
    } else if (parser->current_token->type == TOKEN_MULTIPLY) {
        bin_op = BIN_OP_MUL;
        has_bin_op = 1;
    } else if (parser->current_token->type == TOKEN_DIVIDE) {
        bin_op = BIN_OP_DIV;
        has_bin_op = 1;
    }
    
    if (has_bin_op) {
        parser_advance(parser);
        Expression* right = parser_parse_primary_expression(parser);
        left = expression_create_binary_op(left, right, bin_op);
    }
    
    // Check for comparison operators
    ComparisonOp op;
    int has_comparison = 0;
    
    if (parser->current_token->type == TOKEN_EQUAL) {
        op = CMP_EQUAL;
        has_comparison = 1;
    } else if (parser->current_token->type == TOKEN_NOT_EQUAL) {
        op = CMP_NOT_EQUAL;
        has_comparison = 1;
    } else if (parser->current_token->type == TOKEN_LESS) {
        op = CMP_LESS;
        has_comparison = 1;
    } else if (parser->current_token->type == TOKEN_LESS_EQUAL) {
        op = CMP_LESS_EQUAL;
        has_comparison = 1;
    } else if (parser->current_token->type == TOKEN_GREATER) {
        op = CMP_GREATER;
        has_comparison = 1;
    } else if (parser->current_token->type == TOKEN_GREATER_EQUAL) {
        op = CMP_GREATER_EQUAL;
        has_comparison = 1;
    }
    
    if (has_comparison) {
        parser_advance(parser);
        Expression* right = parser_parse_primary_expression(parser);
        return expression_create_comparison(left, right, op);
    }
    
    return left;
}

Declaration* parser_parse_declaration(Parser* parser) {
    Declaration* decl = malloc(sizeof(Declaration));
    
    // Type keyword
    if (parser->current_token->type == TOKEN_NUMERIC) {
        decl->type = TYPE_NUMERIC;
    } else if (parser->current_token->type == TOKEN_DECIMAL) {
        decl->type = TYPE_DECIMAL;
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        decl->type = TYPE_BOOLEAN;
    } else {
        fprintf(stderr, "Parser error: Expected type keyword at line %d\n", 
                parser->current_token->line);
        exit(1);
    }
    
    parser_advance(parser);
    
    // Variable name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected identifier at line %d\n", 
                parser->current_token->line);
        exit(1);
    }
    
    decl->name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(decl->name, parser->current_token->value);
    parser_advance(parser);
    
    // Check for initialization (= value)
    decl->init_value = NULL;
    if (parser->current_token->type == TOKEN_ASSIGN) {
        parser_advance(parser);
        decl->init_value = parser_parse_expression(parser);
    }
    
    return decl;
}

Statement* parser_parse_statement(Parser* parser);  // Forward declaration

Statement* parser_parse_if_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_IF;
    
    // Skip 'if'
    parser_advance(parser);
    
    // Parse condition
    stmt->if_stmt.condition = parser_parse_expression(parser);
    
    // Expect 'then'
    if (parser->current_token->type != TOKEN_THEN) {
        fprintf(stderr, "Parser error: Expected 'then' after if condition at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse then body
    stmt->if_stmt.then_body = malloc(sizeof(Statement*) * 10);
    stmt->if_stmt.then_count = 0;
    int then_capacity = 10;
    
    while (parser->current_token->type != TOKEN_ELSE && 
           parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* body_stmt = parser_parse_statement(parser);
        
        if (stmt->if_stmt.then_count >= then_capacity) {
            then_capacity *= 2;
            stmt->if_stmt.then_body = realloc(stmt->if_stmt.then_body,
                                              sizeof(Statement*) * then_capacity);
        }
        stmt->if_stmt.then_body[stmt->if_stmt.then_count++] = body_stmt;
    }
    
    // Check for else
    stmt->if_stmt.else_body = NULL;
    stmt->if_stmt.else_count = 0;
    
    if (parser->current_token->type == TOKEN_ELSE) {
        parser_advance(parser);
        
        stmt->if_stmt.else_body = malloc(sizeof(Statement*) * 10);
        int else_capacity = 10;
        
        while (parser->current_token->type != TOKEN_END &&
               parser->current_token->type != TOKEN_EOF) {
            
            Statement* body_stmt = parser_parse_statement(parser);
            
            if (stmt->if_stmt.else_count >= else_capacity) {
                else_capacity *= 2;
                stmt->if_stmt.else_body = realloc(stmt->if_stmt.else_body,
                                                  sizeof(Statement*) * else_capacity);
            }
            stmt->if_stmt.else_body[stmt->if_stmt.else_count++] = body_stmt;
        }
    }
    
    // Expect 'end' 'if'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after if block at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_IF) {
        fprintf(stderr, "Parser error: Expected 'if' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

Statement* parser_parse_for_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_FOR;
    
    // Skip 'for'
    parser_advance(parser);
    
    // Parse loop variable
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected identifier after 'for' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    stmt->for_stmt.var_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->for_stmt.var_name, parser->current_token->value);
    parser_advance(parser);
    
    // Expect '='
    if (parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Parser error: Expected '=' after loop variable at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse start value
    stmt->for_stmt.start = parser_parse_expression(parser);
    
    // Expect 'to'
    if (parser->current_token->type != TOKEN_TO) {
        fprintf(stderr, "Parser error: Expected 'to' in for loop at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse end value
    stmt->for_stmt.end = parser_parse_expression(parser);
    
    // Check for optional 'step'
    stmt->for_stmt.step = NULL;
    if (parser->current_token->type == TOKEN_STEP) {
        parser_advance(parser);
        stmt->for_stmt.step = parser_parse_expression(parser);
    }
    
    // Parse body
    stmt->for_stmt.body = malloc(sizeof(Statement*) * 10);
    stmt->for_stmt.body_count = 0;
    int body_capacity = 10;
    
    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* body_stmt = parser_parse_statement(parser);
        
        if (stmt->for_stmt.body_count >= body_capacity) {
            body_capacity *= 2;
            stmt->for_stmt.body = realloc(stmt->for_stmt.body,
                                          sizeof(Statement*) * body_capacity);
        }
        stmt->for_stmt.body[stmt->for_stmt.body_count++] = body_stmt;
    }
    
    // Expect 'end' 'for'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after for loop body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_FOR) {
        fprintf(stderr, "Parser error: Expected 'for' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

Statement* parser_parse_while_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_WHILE;
    
    // Skip 'while'
    parser_advance(parser);
    
    // Parse condition
    stmt->while_stmt.condition = parser_parse_expression(parser);
    
    // Parse body
    stmt->while_stmt.body = malloc(sizeof(Statement*) * 10);
    stmt->while_stmt.body_count = 0;
    int body_capacity = 10;
    
    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* body_stmt = parser_parse_statement(parser);
        
        if (stmt->while_stmt.body_count >= body_capacity) {
            body_capacity *= 2;
            stmt->while_stmt.body = realloc(stmt->while_stmt.body,
                                           sizeof(Statement*) * body_capacity);
        }
        stmt->while_stmt.body[stmt->while_stmt.body_count++] = body_stmt;
    }
    
    // Expect 'end' 'while'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after while loop body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_WHILE) {
        fprintf(stderr, "Parser error: Expected 'while' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

Statement* parser_parse_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    
    // Check for if
    if (parser->current_token->type == TOKEN_IF) {
        free(stmt);
        return parser_parse_if_statement(parser);
    }
    // Check for for loop
    else if (parser->current_token->type == TOKEN_FOR) {
        free(stmt);
        return parser_parse_for_statement(parser);
    }
    // Check for while loop
    else if (parser->current_token->type == TOKEN_WHILE) {
        free(stmt);
        return parser_parse_while_statement(parser);
    }
    // Check for exit
    else if (parser->current_token->type == TOKEN_EXIT) {
        stmt->type = STMT_EXIT;
        parser_advance(parser);
        return stmt;
    }
    // Check for continue
    else if (parser->current_token->type == TOKEN_CONTINUE) {
        stmt->type = STMT_CONTINUE;
        parser_advance(parser);
        return stmt;
    }
    // Check for type keywords (declaration)
    else if (parser->current_token->type == TOKEN_NUMERIC ||
        parser->current_token->type == TOKEN_DECIMAL ||
        parser->current_token->type == TOKEN_BOOLEAN) {
        stmt->type = STMT_DECLARATION;
        stmt->declaration = parser_parse_declaration(parser);
    }
    // Check for print
    else if (parser->current_token->type == TOKEN_PRINT) {
        parser_advance(parser);
        if (parser->current_token->type != TOKEN_LPAREN) {
            fprintf(stderr, "Parser error: Expected '(' after print at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
        
        stmt->type = STMT_PRINT;
        stmt->print_stmt.expr = parser_parse_expression(parser);
        
        if (parser->current_token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Parser error: Expected ')' after print expression at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
    }
    // Check for assignment (identifier = value)
    else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        stmt->type = STMT_ASSIGNMENT;
        stmt->assignment.var_name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->assignment.var_name, parser->current_token->value);
        parser_advance(parser);
        
        if (parser->current_token->type != TOKEN_ASSIGN) {
            fprintf(stderr, "Parser error: Expected '=' after identifier at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
        
        stmt->assignment.value = parser_parse_expression(parser);
    }
    else {
        fprintf(stderr, "Parser error: Unexpected token at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    return stmt;
}

AST* parser_parse(Parser* parser) {
    AST* ast = ast_create();
    
    while (parser->current_token->type != TOKEN_EOF) {
        Statement* stmt = parser_parse_statement(parser);
        ast_add_statement(ast, stmt);
    }
    
    return ast;
}

void parser_free(Parser* parser) {
    token_free(parser->current_token);
    free(parser);
}

void ast_free(AST* ast) {
    for (int i = 0; i < ast->count; i++) {
        Statement* stmt = ast->statements[i];
        if (stmt->type == STMT_DECLARATION) {
            free(stmt->declaration->name);
            if (stmt->declaration->init_value) {
                if (stmt->declaration->init_value->type == EXPR_VARIABLE) {
                    free(stmt->declaration->init_value->var_name);
                }
                free(stmt->declaration->init_value);
            }
            free(stmt->declaration);
        } else if (stmt->type == STMT_ASSIGNMENT) {
            free(stmt->assignment.var_name);
            if (stmt->assignment.value->type == EXPR_VARIABLE) {
                free(stmt->assignment.value->var_name);
            }
            free(stmt->assignment.value);
        } else if (stmt->type == STMT_PRINT) {
            if (stmt->print_stmt.expr->type == EXPR_VARIABLE) {
                free(stmt->print_stmt.expr->var_name);
            }
            free(stmt->print_stmt.expr);
        }
        free(stmt);
    }
    free(ast->statements);
    free(ast);
}
