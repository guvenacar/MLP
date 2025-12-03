#include "parser.h"
#include "statement_parser.h"
#include "modules/control_flow/control_flow.h"
#include "modules/arithmetic/arithmetic_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void parser_free(Parser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    free(parser);
}

static void advance(Parser* parser) {
    if (parser->current_token) token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

static Declaration* parse_declaration(Parser* parser) {
    Declaration* decl = malloc(sizeof(Declaration));
    decl->value = NULL;
    decl->init_expr = NULL;
    
    // Type
    if (parser->current_token->type == TOKEN_NUMERIC) {
        decl->type = VAR_NUMERIC;
    } else if (parser->current_token->type == TOKEN_TEXT) {
        decl->type = VAR_STRING;
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        decl->type = VAR_BOOLEAN;
    } else {
        fprintf(stderr, "Error: Expected type keyword\n");
        free(decl);
        return NULL;
    }
    advance(parser);
    
    // Name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected identifier\n");
        free(decl);
        return NULL;
    }
    decl->name = strdup(parser->current_token->value);
    advance(parser);
    
    // =
    if (parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Error: Expected '='\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    advance(parser);
    
    // Value - check for simple literal first
    if (parser->current_token->type == TOKEN_NUMBER) {
        decl->value = strdup(parser->current_token->value);
        advance(parser);
    } else if (parser->current_token->type == TOKEN_STRING) {
        decl->value = strdup(parser->current_token->value);
        advance(parser);
    } else if (parser->current_token->type == TOKEN_TRUE) {
        decl->value = strdup("true");
        advance(parser);
    } else if (parser->current_token->type == TOKEN_FALSE) {
        decl->value = strdup("false");
        advance(parser);
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        // Could be variable, function call, or expression
        // Use expression_parse for complex cases
        Parser temp_parser;
        temp_parser.lexer = parser->lexer;
        temp_parser.current_token = parser->current_token;
        
        Expression* expr = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        decl->init_expr = expr;
    } else {
        fprintf(stderr, "Error: Expected value or expression\n");
        free(decl->name);
        free(decl);
        return NULL;
    }
    
    return decl;
}

// Helper: Add statement to program
static void add_statement(Program* program, Statement* stmt) {
    if (program->stmt_count >= program->stmt_capacity) {
        program->stmt_capacity *= 2;
        program->statements = realloc(program->statements, 
                                     sizeof(Statement*) * program->stmt_capacity);
    }
    program->statements[program->stmt_count++] = stmt;
}

// Helper: Add declaration to program
static void add_declaration(Program* program, Declaration* decl) {
    if (program->decl_count >= program->decl_capacity) {
        program->decl_capacity *= 2;
        program->declarations = realloc(program->declarations, 
                                       sizeof(Declaration*) * program->decl_capacity);
    }
    program->declarations[program->decl_count++] = decl;
}

// Helper: Add function to program
static void add_function(Program* program, Statement* func) {
    if (program->func_count >= program->func_capacity) {
        program->func_capacity *= 2;
        program->functions = realloc(program->functions, 
                                    sizeof(Statement*) * program->func_capacity);
    }
    program->functions[program->func_count++] = func;
}

// Forward declaration for recursive parsing
static Statement* parse_body_statement(Parser* parser);

// Parse function body until "end function"
static Statement** parse_function_body(Parser* parser, int* count) {
    int capacity = 8;
    Statement** body = malloc(sizeof(Statement*) * capacity);
    *count = 0;
    
    while (parser->current_token && 
           parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* stmt = parse_body_statement(parser);
        if (stmt) {
            if (*count >= capacity) {
                capacity *= 2;
                body = realloc(body, sizeof(Statement*) * capacity);
            }
            body[*count] = stmt;
            (*count)++;
        }
    }
    
    return body;
}

// Parse a statement inside function body
static Statement* parse_body_statement(Parser* parser) {
    // Return statement
    if (parser->current_token->type == TOKEN_RETURN) {
        advance(parser);  // consume 'return'
        
        Statement* stmt = malloc(sizeof(Statement));
        stmt->type = STMT_RETURN;
        
        // Parse return expression (if any)
        if (parser->current_token->type != TOKEN_END &&
            parser->current_token->type != TOKEN_EOF) {
            Parser temp_parser;
            temp_parser.lexer = parser->lexer;
            temp_parser.current_token = parser->current_token;
            
            Expression* expr = expression_parse(&temp_parser);
            parser->current_token = temp_parser.current_token;
            stmt->data.return_stmt.value = expr;
        } else {
            stmt->data.return_stmt.value = NULL;
        }
        
        return stmt;
    }
    
    // Print statement
    if (parser->current_token->type == TOKEN_PRINT) {
        advance(parser);  // consume 'print'
        
        Statement* stmt = malloc(sizeof(Statement));
        stmt->type = STMT_PRINT;
        
        if (parser->current_token->type == TOKEN_STRING) {
            Expression* expr = malloc(sizeof(Expression));
            expr->type = EXPR_STRING;
            expr->data.string_value = strdup(parser->current_token->value);
            stmt->data.print_stmt.value = expr;
            advance(parser);
        } else {
            Parser temp_parser;
            temp_parser.lexer = parser->lexer;
            temp_parser.current_token = parser->current_token;
            
            Expression* expr = expression_parse(&temp_parser);
            parser->current_token = temp_parser.current_token;
            stmt->data.print_stmt.value = expr;
        }
        
        return stmt;
    }
    
    // Control flow statements
    if (parser->current_token->type == TOKEN_IF ||
        parser->current_token->type == TOKEN_WHILE ||
        parser->current_token->type == TOKEN_FOR) {
        return parse_statement(parser);
    }
    
    // Declaration: numeric x = 5
    if (parser->current_token->type == TOKEN_NUMERIC ||
        parser->current_token->type == TOKEN_BOOLEAN ||
        parser->current_token->type == TOKEN_TEXT) {
        
        Statement* stmt = malloc(sizeof(Statement));
        stmt->type = STMT_DECLARATION;
        
        int var_type = (parser->current_token->type == TOKEN_NUMERIC) ? 0 :
                       (parser->current_token->type == TOKEN_BOOLEAN) ? 1 : 2;
        advance(parser);
        
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            free(stmt);
            return NULL;
        }
        stmt->data.decl_stmt.name = strdup(parser->current_token->value);
        stmt->data.decl_stmt.var_type = var_type;
        advance(parser);
        
        if (parser->current_token->type != TOKEN_ASSIGN) {
            free(stmt->data.decl_stmt.name);
            free(stmt);
            return NULL;
        }
        advance(parser);
        
        Parser temp_parser;
        temp_parser.lexer = parser->lexer;
        temp_parser.current_token = parser->current_token;
        
        Expression* value = expression_parse(&temp_parser);
        parser->current_token = temp_parser.current_token;
        stmt->data.decl_stmt.value = value;
        
        return stmt;
    }
    
    // Assignment or function call: identifier ...
    if (parser->current_token->type == TOKEN_IDENTIFIER) {
        char* name = strdup(parser->current_token->value);
        advance(parser);
        
        // Function call: name(args)
        if (parser->current_token->type == TOKEN_LPAREN) {
            advance(parser);  // consume '('
            
            Statement* stmt = malloc(sizeof(Statement));
            stmt->type = STMT_CALL;
            stmt->data.call_stmt.name = name;
            stmt->data.call_stmt.args = NULL;
            stmt->data.call_stmt.arg_count = 0;
            
            // Parse arguments
            if (parser->current_token->type != TOKEN_RPAREN) {
                int arg_capacity = 4;
                stmt->data.call_stmt.args = malloc(sizeof(Expression*) * arg_capacity);
                
                // Parse first argument
                Parser temp_parser;
                temp_parser.lexer = parser->lexer;
                temp_parser.current_token = parser->current_token;
                Expression* arg = expression_parse(&temp_parser);
                parser->current_token = temp_parser.current_token;
                stmt->data.call_stmt.args[stmt->data.call_stmt.arg_count++] = arg;
                
                // Parse remaining arguments
                while (parser->current_token->type == TOKEN_COMMA) {
                    advance(parser);  // consume ','
                    
                    temp_parser.lexer = parser->lexer;
                    temp_parser.current_token = parser->current_token;
                    arg = expression_parse(&temp_parser);
                    parser->current_token = temp_parser.current_token;
                    
                    if (stmt->data.call_stmt.arg_count >= arg_capacity) {
                        arg_capacity *= 2;
                        stmt->data.call_stmt.args = realloc(stmt->data.call_stmt.args,
                                                           sizeof(Expression*) * arg_capacity);
                    }
                    stmt->data.call_stmt.args[stmt->data.call_stmt.arg_count++] = arg;
                }
            }
            
            if (parser->current_token->type == TOKEN_RPAREN) {
                advance(parser);  // consume ')'
            }
            
            return stmt;
        }
        
        // Assignment: name = expr
        if (parser->current_token->type == TOKEN_ASSIGN) {
            advance(parser);  // consume '='
            
            Statement* stmt = malloc(sizeof(Statement));
            stmt->type = STMT_ASSIGNMENT;
            stmt->data.assign_stmt.name = name;
            
            Parser temp_parser;
            temp_parser.lexer = parser->lexer;
            temp_parser.current_token = parser->current_token;
            
            Expression* value = expression_parse(&temp_parser);
            parser->current_token = temp_parser.current_token;
            stmt->data.assign_stmt.value = value;
            
            return stmt;
        }
        
        free(name);
    }
    
    // Unknown - skip
    advance(parser);
    return NULL;
}

// Parse function definition
// Syntax: function name(type1 param1, type2 param2) ... end function
static Statement* parse_function_def(Parser* parser) {
    advance(parser);  // consume 'function'
    
    // Function name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected function name\n");
        return NULL;
    }
    char* func_name = strdup(parser->current_token->value);
    advance(parser);
    
    // Parameters: (type1 param1, type2 param2)
    if (parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        free(func_name);
        return NULL;
    }
    advance(parser);  // consume '('
    
    int param_capacity = 4;
    int param_count = 0;
    FunctionParam** params = malloc(sizeof(FunctionParam*) * param_capacity);
    
    // Parse parameters
    while (parser->current_token->type != TOKEN_RPAREN &&
           parser->current_token->type != TOKEN_EOF) {
        
        // Parameter type
        int var_type = 0;  // VAR_NUMERIC by default
        if (parser->current_token->type == TOKEN_NUMERIC) {
            var_type = 0;  // VAR_NUMERIC
        } else if (parser->current_token->type == TOKEN_BOOLEAN) {
            var_type = 1;  // VAR_BOOLEAN
        } else if (parser->current_token->type == TOKEN_TEXT) {
            var_type = 2;  // VAR_STRING
        } else {
            fprintf(stderr, "Error: Expected parameter type\n");
            break;
        }
        advance(parser);
        
        // Parameter name
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected parameter name\n");
            break;
        }
        
        FunctionParam* param = malloc(sizeof(FunctionParam));
        param->name = strdup(parser->current_token->value);
        param->var_type = var_type;
        advance(parser);
        
        if (param_count >= param_capacity) {
            param_capacity *= 2;
            params = realloc(params, sizeof(FunctionParam*) * param_capacity);
        }
        params[param_count++] = param;
        
        // Check for comma
        if (parser->current_token->type == TOKEN_COMMA) {
            advance(parser);
        }
    }
    
    if (parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Error: Expected ')' after parameters\n");
        free(func_name);
        for (int i = 0; i < param_count; i++) {
            free(params[i]->name);
            free(params[i]);
        }
        free(params);
        return NULL;
    }
    advance(parser);  // consume ')'
    
    // Parse function body
    int body_count = 0;
    Statement** body = parse_function_body(parser, &body_count);
    
    // Expect 'end'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Error: Expected 'end' to close function\n");
    } else {
        advance(parser);  // consume 'end'
        
        // Expect 'function'
        if (parser->current_token->type == TOKEN_FUNCTION) {
            advance(parser);  // consume 'function'
        }
    }
    
    // Create function statement
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_FUNCTION;
    stmt->data.func_stmt.name = func_name;
    stmt->data.func_stmt.params = params;
    stmt->data.func_stmt.param_count = param_count;
    stmt->data.func_stmt.body = body;
    stmt->data.func_stmt.body_count = body_count;
    
    return stmt;
}

Program* parser_parse(Parser* parser) {
    Program* program = malloc(sizeof(Program));
    program->decl_capacity = 10;
    program->decl_count = 0;
    program->declarations = malloc(sizeof(Declaration*) * program->decl_capacity);
    program->stmt_capacity = 10;
    program->stmt_count = 0;
    program->statements = malloc(sizeof(Statement*) * program->stmt_capacity);
    program->func_capacity = 10;
    program->func_count = 0;
    program->functions = malloc(sizeof(Statement*) * program->func_capacity);
    
    while (parser->current_token->type != TOKEN_EOF) {
        // Parse function definition
        if (parser->current_token->type == TOKEN_FUNCTION) {
            Statement* func = parse_function_def(parser);
            if (func) {
                add_function(program, func);
                continue;
            }
        }
        
        // Try to parse control flow statement first (if, while, for)
        if (parser->current_token->type == TOKEN_IF ||
            parser->current_token->type == TOKEN_WHILE ||
            parser->current_token->type == TOKEN_FOR) {
            Statement* stmt = parse_statement(parser);
            if (stmt) {
                add_statement(program, stmt);
                continue;
            }
        }
        
        // Parse print statement: print expr or print "string"
        if (parser->current_token->type == TOKEN_PRINT) {
            advance(parser);  // consume 'print'
            
            Statement* stmt = malloc(sizeof(Statement));
            stmt->type = STMT_PRINT;
            
            // Check if it's a string literal or expression
            if (parser->current_token->type == TOKEN_STRING) {
                // Create a string expression
                Expression* expr = malloc(sizeof(Expression));
                expr->type = EXPR_STRING;
                expr->data.string_value = strdup(parser->current_token->value);
                stmt->data.print_stmt.value = expr;
                advance(parser);
            } else {
                // Parse numeric expression
                Parser temp_parser;
                temp_parser.lexer = parser->lexer;
                temp_parser.current_token = parser->current_token;
                
                Expression* expr = expression_parse(&temp_parser);
                parser->current_token = temp_parser.current_token;
                stmt->data.print_stmt.value = expr;
            }
            
            add_statement(program, stmt);
            continue;
        }
        
        // Parse function call at top level: name(args)
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Look ahead for '('
            char* name = strdup(parser->current_token->value);
            advance(parser);
            
            if (parser->current_token->type == TOKEN_LPAREN) {
                // It's a function call
                advance(parser);  // consume '('
                
                Statement* stmt = malloc(sizeof(Statement));
                stmt->type = STMT_CALL;
                stmt->data.call_stmt.name = name;
                stmt->data.call_stmt.args = NULL;
                stmt->data.call_stmt.arg_count = 0;
                
                // Parse arguments
                if (parser->current_token->type != TOKEN_RPAREN) {
                    int arg_capacity = 4;
                    stmt->data.call_stmt.args = malloc(sizeof(Expression*) * arg_capacity);
                    
                    Parser temp_parser;
                    temp_parser.lexer = parser->lexer;
                    temp_parser.current_token = parser->current_token;
                    Expression* arg = expression_parse(&temp_parser);
                    parser->current_token = temp_parser.current_token;
                    stmt->data.call_stmt.args[stmt->data.call_stmt.arg_count++] = arg;
                    
                    while (parser->current_token->type == TOKEN_COMMA) {
                        advance(parser);
                        
                        temp_parser.lexer = parser->lexer;
                        temp_parser.current_token = parser->current_token;
                        arg = expression_parse(&temp_parser);
                        parser->current_token = temp_parser.current_token;
                        
                        if (stmt->data.call_stmt.arg_count >= arg_capacity) {
                            arg_capacity *= 2;
                            stmt->data.call_stmt.args = realloc(stmt->data.call_stmt.args,
                                                               sizeof(Expression*) * arg_capacity);
                        }
                        stmt->data.call_stmt.args[stmt->data.call_stmt.arg_count++] = arg;
                    }
                }
                
                if (parser->current_token->type == TOKEN_RPAREN) {
                    advance(parser);  // consume ')'
                }
                
                add_statement(program, stmt);
                continue;
            } else {
                // Not a function call, might be assignment - rewind not possible
                // Create a simple token to push back
                // For now, handle as declaration if we see =
                if (parser->current_token->type == TOKEN_ASSIGN) {
                    // This is an assignment at top level - treat as error for now
                    fprintf(stderr, "Error: Assignment at top level requires declaration\n");
                }
                free(name);
                // Skip to next statement
                continue;
            }
        }
        
        // Parse declaration
        Declaration* decl = parse_declaration(parser);
        if (decl == NULL) {
            program_free(program);
            return NULL;
        }
        
        add_declaration(program, decl);
    }
    
    return program;
}

void program_free(Program* program) {
    // Free declarations
    for (int i = 0; i < program->decl_count; i++) {
        free(program->declarations[i]->name);
        free(program->declarations[i]->value);
        if (program->declarations[i]->init_expr) {
            expression_free(program->declarations[i]->init_expr);
        }
        free(program->declarations[i]);
    }
    free(program->declarations);
    
    // Free statements
    for (int i = 0; i < program->stmt_count; i++) {
        statement_free(program->statements[i]);
    }
    free(program->statements);
    
    // Free functions
    for (int i = 0; i < program->func_count; i++) {
        statement_free(program->functions[i]);
    }
    free(program->functions);
    
    free(program);
}
