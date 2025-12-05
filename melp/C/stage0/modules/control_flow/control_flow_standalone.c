#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../../lexer.h"

// Control flow structures
typedef struct Condition {
    char left[64];
    char op[8];      // ==, !=, <, >, <=, >=
    char right[64];
} Condition;

typedef struct IfStatement {
    Condition cond;
    char body[1024];     // Simplified - store as string for now
    char else_body[1024];
    bool has_else;
    struct IfStatement *next;
} IfStatement;

typedef struct Statement {
    enum {
        STMT_ASSIGN,      // x = x + 1
        STMT_PRINT,       // print(x)
        STMT_INCREMENT    // x = x + 1 (special case)
    } type;
    char target[64];      // Variable name
    char expr[256];       // Expression or value
    struct Statement *next;
} Statement;

typedef struct WhileLoop {
    Condition cond;
    Statement *body_stmts;  // Linked list of statements
    struct WhileLoop *next;
} WhileLoop;

typedef struct ForLoop {
    char var[64];       // Loop variable
    char start[64];     // Start value
    char end[64];       // End value
    char step[64];      // Step (optional, default 1)
    char body[1024];
    struct ForLoop *next;
} ForLoop;

static IfStatement *if_list = NULL;
static WhileLoop *while_list = NULL;
static ForLoop *for_list = NULL;
static int if_count = 0;
static int while_count = 0;
static int for_count = 0;
static int label_counter = 0;

// Add if statement to list
static void add_if_statement(Condition cond, const char *body, const char *else_body, bool has_else) {
    IfStatement *new_if = malloc(sizeof(IfStatement));
    new_if->cond = cond;
    strcpy(new_if->body, body);
    if (has_else) {
        strcpy(new_if->else_body, else_body);
    } else {
        new_if->else_body[0] = '\0';
    }
    new_if->has_else = has_else;
    new_if->next = if_list;
    if_list = new_if;
    if_count++;
}

// Add while loop to list
static void add_while_loop(Condition cond, Statement *body_stmts) {
    WhileLoop *new_while = malloc(sizeof(WhileLoop));
    new_while->cond = cond;
    new_while->body_stmts = body_stmts;
    new_while->next = while_list;
    while_list = new_while;
    while_count++;
}

// Add for loop to list
static void add_for_loop(const char *var, const char *start, const char *end, const char *step, const char *body) {
    ForLoop *new_for = malloc(sizeof(ForLoop));
    strcpy(new_for->var, var);
    strcpy(new_for->start, start);
    strcpy(new_for->end, end);
    strcpy(new_for->step, step[0] ? step : "1");
    strcpy(new_for->body, body);
    new_for->next = for_list;
    for_list = new_for;
    for_count++;
}

// Parse statements inside while loop body
static Statement* parse_while_body(Lexer *lexer) {
    Statement *head = NULL;
    Statement *tail = NULL;
    int expr_parts = 0;
    
    Token *token = lexer_next_token(lexer);
    
    while (token->type != TOKEN_END && token->type != TOKEN_EOF) {
        Statement *stmt = NULL;
        bool token_consumed = false;  // Track if we already got next token
        
        // Check for assignment: variable = expression
        if (token->type == TOKEN_IDENTIFIER) {
            char var_name[64];
            strcpy(var_name, token->value);
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            if (token->type == TOKEN_ASSIGN) {
                stmt = malloc(sizeof(Statement));
                stmt->type = STMT_ASSIGN;
                strcpy(stmt->target, var_name);
                stmt->next = NULL;
                
                token_free(token);
                token = lexer_next_token(lexer);
                
                // Parse right side expression (variable + number or variable - number)
                char expr[256] = "";
                expr_parts = 0;
                while (token->type != TOKEN_END && token->type != TOKEN_EOF && 
                       token->type != TOKEN_IDENTIFIER && expr_parts < 3) {
                    if (token->type == TOKEN_NUMBER) {
                        strcat(expr, token->value);
                        strcat(expr, " ");
                        expr_parts++;
                    } else if (token->type == TOKEN_PLUS) {
                        strcat(expr, "+ ");
                        expr_parts++;
                    } else if (token->type == TOKEN_MINUS) {
                        strcat(expr, "- ");
                        expr_parts++;
                    } else if (token->type == TOKEN_MULTIPLY) {
                        strcat(expr, "* ");
                        expr_parts++;
                    } else if (token->type == TOKEN_DIVIDE) {
                        strcat(expr, "/ ");
                        expr_parts++;
                    } else {
                        break;
                    }
                    
                    token_free(token);
                    token = lexer_next_token(lexer);
                }
                
                // If first token was identifier, add it to expr
                if (expr_parts == 0 && token->type == TOKEN_IDENTIFIER) {
                    strcat(expr, token->value);
                    strcat(expr, " ");
                    
                    token_free(token);
                    token = lexer_next_token(lexer);
                    
                    // Continue parsing operator and operand
                    while (token->type != TOKEN_END && token->type != TOKEN_EOF && 
                           token->type != TOKEN_IDENTIFIER) {
                        if (token->type == TOKEN_PLUS) {
                            strcat(expr, "+ ");
                        } else if (token->type == TOKEN_MINUS) {
                            strcat(expr, "- ");
                        } else if (token->type == TOKEN_MULTIPLY) {
                            strcat(expr, "* ");
                        } else if (token->type == TOKEN_DIVIDE) {
                            strcat(expr, "/ ");
                        } else if (token->type == TOKEN_NUMBER) {
                            strcat(expr, token->value);
                            strcat(expr, " ");
                            token_free(token);
                            token = lexer_next_token(lexer);
                            break;
                        } else {
                            break;
                        }
                        
                        token_free(token);
                        token = lexer_next_token(lexer);
                    }
                }
                
                strcpy(stmt->expr, expr);
                
                // Add to list
                if (head == NULL) {
                    head = stmt;
                    tail = stmt;
                } else {
                    tail->next = stmt;
                    tail = stmt;
                }
                
                token_consumed = true;  // We parsed expression and got next token
            }
        }
        
        // Check for print statement
        else if (token->type == TOKEN_PRINT) {
            stmt = malloc(sizeof(Statement));
            stmt->type = STMT_PRINT;
            stmt->next = NULL;
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            // Expect LPAREN
            if (token->type == TOKEN_LPAREN) {
                token_free(token);
                token = lexer_next_token(lexer);
                
                // Get what to print (string or variable)
                if (token->type == TOKEN_STRING || token->type == TOKEN_IDENTIFIER) {
                    strcpy(stmt->expr, token->value);
                }
                
                token_free(token);
                token = lexer_next_token(lexer);
                
                // Expect RPAREN
                if (token->type == TOKEN_RPAREN) {
                    token_free(token);
                    token = lexer_next_token(lexer);
                }
            }
            
            // Add to list
            if (head == NULL) {
                head = stmt;
                tail = stmt;
            } else {
                tail->next = stmt;
                tail = stmt;
            }
            
            token_consumed = true;  // We already got next token after RPAREN
        }
        
        // Advance to next token only if not already consumed
        if (!token_consumed) {
            token_free(token);
            token = lexer_next_token(lexer);
        }
    }
    
    // Token is now TOKEN_END or TOKEN_EOF
    // Free the TOKEN_END token before returning
    if (token) token_free(token);
    
    return head;
}

// Parse if-then-else statements
static void control_flow_parse(Lexer *lexer) {
    Token *tokens[20];
    int token_idx = 0;
    
    Token *token = lexer_next_token(lexer);
    
    while (token->type != TOKEN_EOF) {
        
        // Look for: IF condition THEN ... END IF
        if (token->type == TOKEN_IF) {
            // Parse condition: identifier comparison identifier/number
            token_free(token);
            token = lexer_next_token(lexer);
            
            Condition cond;
            strcpy(cond.left, token->value);
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            // Get comparison operator
            if (token->type == TOKEN_EQUAL) strcpy(cond.op, "==");
            else if (token->type == TOKEN_NOT_EQUAL) strcpy(cond.op, "!=");
            else if (token->type == TOKEN_LESS) strcpy(cond.op, "<");
            else if (token->type == TOKEN_GREATER) strcpy(cond.op, ">");
            else if (token->type == TOKEN_LESS_EQUAL) strcpy(cond.op, "<=");
            else if (token->type == TOKEN_GREATER_EQUAL) strcpy(cond.op, ">=");
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            strcpy(cond.right, token->value);
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            // Expect THEN
            if (token->type == TOKEN_THEN) {
                token_free(token);
                token = lexer_next_token(lexer);
                
                // Collect body until ELSE or END
                char body[1024] = "";
                char else_body[1024] = "";
                bool has_else = false;
                
                while (token->type != TOKEN_ELSE && token->type != TOKEN_END && token->type != TOKEN_EOF) {
                    if (strlen(body) > 0) strcat(body, " ");
                    strcat(body, token->value);
                    token_free(token);
                    token = lexer_next_token(lexer);
                }
                
                // Check for ELSE
                if (token->type == TOKEN_ELSE) {
                    has_else = true;
                    token_free(token);
                    token = lexer_next_token(lexer);
                    
                    // Collect else body
                    while (token->type != TOKEN_END && token->type != TOKEN_EOF) {
                        if (strlen(else_body) > 0) strcat(else_body, " ");
                        strcat(else_body, token->value);
                        token_free(token);
                        token = lexer_next_token(lexer);
                    }
                }
                
                // Expect END IF
                if (token->type == TOKEN_END) {
                    token_free(token);
                    token = lexer_next_token(lexer);
                    // Skip IF token after END
                    if (token->type == TOKEN_IF) {
                        add_if_statement(cond, body, else_body, has_else);
                        printf("  ✓ If statement: %s %s %s\n", cond.left, cond.op, cond.right);
                        if (has_else) {
                            printf("    - with else branch\n");
                        }
                        token_free(token);
                        token = lexer_next_token(lexer);
                        continue;  // Continue to next token
                    }
                }
            }
        }
        
        // Look for: WHILE condition ... END WHILE
        if (token->type == TOKEN_WHILE) {
            // Parse condition
            token_free(token);
            token = lexer_next_token(lexer);
            
            Condition cond;
            strcpy(cond.left, token->value);
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            // Get comparison operator
            if (token->type == TOKEN_EQUAL) strcpy(cond.op, "==");
            else if (token->type == TOKEN_NOT_EQUAL) strcpy(cond.op, "!=");
            else if (token->type == TOKEN_LESS) strcpy(cond.op, "<");
            else if (token->type == TOKEN_GREATER) strcpy(cond.op, ">");
            else if (token->type == TOKEN_LESS_EQUAL) strcpy(cond.op, "<=");
            else if (token->type == TOKEN_GREATER_EQUAL) strcpy(cond.op, ">=");
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            strcpy(cond.right, token->value);
            
            token_free(token);
            
            // Parse body statements
            Statement *body_stmts = parse_while_body(lexer);
            
            // parse_while_body consumed TOKEN_END, so next token should be "while" keyword after "end"
            token = lexer_next_token(lexer);
            
            // Expect WHILE (after END)
            if (token->type == TOKEN_WHILE) {
                add_while_loop(cond, body_stmts);
                printf("  ✓ While loop: %s %s %s\n", cond.left, cond.op, cond.right);
                
                token_free(token);
                token = lexer_next_token(lexer);
                continue;  // Continue to next token
            }
        }
        
        // Look for: FOR var = start TO end [STEP step] ... END FOR
        if (token->type == TOKEN_FOR) {
            token_free(token);
            token = lexer_next_token(lexer);
            
            // Get loop variable
            char var[64];
            strcpy(var, token->value);
            
            token_free(token);
            token = lexer_next_token(lexer);
            
            // Expect ASSIGN
            if (token->type == TOKEN_ASSIGN) {
                token_free(token);
                token = lexer_next_token(lexer);
                
                // Get start value
                char start[64];
                strcpy(start, token->value);
                
                token_free(token);
                token = lexer_next_token(lexer);
                
                // Expect TO
                if (token->type == TOKEN_TO) {
                    token_free(token);
                    token = lexer_next_token(lexer);
                    
                    // Get end value
                    char end[64];
                    strcpy(end, token->value);
                    
                    token_free(token);
                    token = lexer_next_token(lexer);
                    
                    // Check for optional STEP
                    char step[64] = "1";
                    // TODO: Add STEP parsing when TOKEN_STEP is available
                    
                    // Collect body until END
                    char body[1024] = "";
                    while (token->type != TOKEN_END && token->type != TOKEN_EOF) {
                        if (strlen(body) > 0) strcat(body, " ");
                        strcat(body, token->value);
                        token_free(token);
                        token = lexer_next_token(lexer);
                    }
                    
                    // Expect END FOR
                    if (token->type == TOKEN_END) {
                        token_free(token);
                        token = lexer_next_token(lexer);
                        // Skip FOR token after END
                        if (token->type == TOKEN_FOR) {
                            add_for_loop(var, start, end, step, body);
                            printf("  ✓ For loop: %s = %s to %s\n", var, start, end);
                            token_free(token);
                            token = lexer_next_token(lexer);
                            continue;  // Continue to next token
                        }
                    }
                }
            }
        }
        
        if (token) token_free(token);
        token = lexer_next_token(lexer);
    }
    
    if (token) token_free(token);
}

// Generate assembly for if statements
static void control_flow_codegen(FILE *out) {
    if (if_count == 0 && while_count == 0 && for_count == 0) return;
    
    fprintf(out, "; Control flow (if-else) generated by control_flow module\n");
    fprintf(out, "section .text\n");
    
    // Generate if statements
    if (if_count > 0) {
        IfStatement *stmt = if_list;
    while (stmt != NULL) {
        int label_id = label_counter++;
        
        fprintf(out, "; if %s %s %s then\n", stmt->cond.left, stmt->cond.op, stmt->cond.right);
        
        // Load left operand
        if (stmt->cond.left[0] >= '0' && stmt->cond.left[0] <= '9') {
            fprintf(out, "  mov rax, %s\n", stmt->cond.left);
        } else {
            fprintf(out, "  mov rax, [%s]\n", stmt->cond.left);
        }
        
        // Load right operand
        if (stmt->cond.right[0] >= '0' && stmt->cond.right[0] <= '9') {
            fprintf(out, "  mov rbx, %s\n", stmt->cond.right);
        } else {
            fprintf(out, "  mov rbx, [%s]\n", stmt->cond.right);
        }
        
        // Compare
        fprintf(out, "  cmp rax, rbx\n");
        
        // Jump based on condition (inverse logic - jump to else/endif if condition is FALSE)
        if (strcmp(stmt->cond.op, "==") == 0) {
            fprintf(out, "  jne .else_%d\n", label_id);
        } else if (strcmp(stmt->cond.op, "!=") == 0) {
            fprintf(out, "  je .else_%d\n", label_id);
        } else if (strcmp(stmt->cond.op, "<") == 0) {
            fprintf(out, "  jge .else_%d\n", label_id);
        } else if (strcmp(stmt->cond.op, ">") == 0) {
            fprintf(out, "  jle .else_%d\n", label_id);
        } else if (strcmp(stmt->cond.op, "<=") == 0) {
            fprintf(out, "  jg .else_%d\n", label_id);
        } else if (strcmp(stmt->cond.op, ">=") == 0) {
            fprintf(out, "  jl .else_%d\n", label_id);
        }
        
        // Then body (placeholder - actual statements would be generated here)
        fprintf(out, "  ; then: %s\n", stmt->body);
        fprintf(out, "  jmp .endif_%d\n", label_id);
        
        // Else body
        fprintf(out, ".else_%d:\n", label_id);
        if (stmt->has_else) {
            fprintf(out, "  ; else: %s\n", stmt->else_body);
        }
        
        fprintf(out, ".endif_%d:\n", label_id);
        fprintf(out, "\n");
        
        stmt = stmt->next;
    }
    }
    
    // Generate while loops
    if (while_count > 0) {
    WhileLoop *loop = while_list;
    while (loop != NULL) {
        int label_id = label_counter++;
        
        fprintf(out, "; while %s %s %s\n", loop->cond.left, loop->cond.op, loop->cond.right);
        fprintf(out, ".while_start_%d:\n", label_id);
        
        // Load left operand
        if (loop->cond.left[0] >= '0' && loop->cond.left[0] <= '9') {
            fprintf(out, "  mov rax, %s\n", loop->cond.left);
        } else {
            fprintf(out, "  mov rax, [%s]\n", loop->cond.left);
        }
        
        // Load right operand
        if (loop->cond.right[0] >= '0' && loop->cond.right[0] <= '9') {
            fprintf(out, "  mov rbx, %s\n", loop->cond.right);
        } else {
            fprintf(out, "  mov rbx, [%s]\n", loop->cond.right);
        }
        
        // Compare
        fprintf(out, "  cmp rax, rbx\n");
        
        // Jump to end if condition is false (inverse logic)
        if (strcmp(loop->cond.op, "==") == 0) {
            fprintf(out, "  jne .while_end_%d\n", label_id);
        } else if (strcmp(loop->cond.op, "!=") == 0) {
            fprintf(out, "  je .while_end_%d\n", label_id);
        } else if (strcmp(loop->cond.op, "<") == 0) {
            fprintf(out, "  jge .while_end_%d\n", label_id);
        } else if (strcmp(loop->cond.op, ">") == 0) {
            fprintf(out, "  jle .while_end_%d\n", label_id);
        } else if (strcmp(loop->cond.op, "<=") == 0) {
            fprintf(out, "  jg .while_end_%d\n", label_id);
        } else if (strcmp(loop->cond.op, ">=") == 0) {
            fprintf(out, "  jl .while_end_%d\n", label_id);
        }
        
        // Generate body statements
        Statement *stmt = loop->body_stmts;
        while (stmt != NULL) {
            if (stmt->type == STMT_ASSIGN) {
                // Parse expression: var + num or var - num
                char expr_copy[256];
                strcpy(expr_copy, stmt->expr);
                
                char *token = strtok(expr_copy, " ");
                char operand1[64] = "";
                char operator[8] = "";
                char operand2[64] = "";
                
                int part = 0;
                while (token != NULL) {
                    if (part == 0) {
                        strcpy(operand1, token);
                        part = 1;
                    } else if (part == 1) {
                        strcpy(operator, token);
                        part = 2;
                    } else if (part == 2) {
                        strcpy(operand2, token);
                        break;
                    }
                    token = strtok(NULL, " ");
                }
                
                // Generate assembly for: target = operand1 operator operand2
                fprintf(out, "  ; %s = %s\n", stmt->target, stmt->expr);
                
                // Load first operand
                if (operand1[0] >= '0' && operand1[0] <= '9') {
                    fprintf(out, "  mov rax, %s\n", operand1);
                } else {
                    fprintf(out, "  mov rax, [%s]\n", operand1);
                }
                
                // Perform operation with second operand
                if (strlen(operator) > 0 && strlen(operand2) > 0) {
                    if (strcmp(operator, "+") == 0) {
                        if (operand2[0] >= '0' && operand2[0] <= '9') {
                            fprintf(out, "  add rax, %s\n", operand2);
                        } else {
                            fprintf(out, "  mov rbx, [%s]\n", operand2);
                            fprintf(out, "  add rax, rbx\n");
                        }
                    } else if (strcmp(operator, "-") == 0) {
                        if (operand2[0] >= '0' && operand2[0] <= '9') {
                            fprintf(out, "  sub rax, %s\n", operand2);
                        } else {
                            fprintf(out, "  mov rbx, [%s]\n", operand2);
                            fprintf(out, "  sub rax, rbx\n");
                        }
                    } else if (strcmp(operator, "*") == 0) {
                        if (operand2[0] >= '0' && operand2[0] <= '9') {
                            fprintf(out, "  imul rax, %s\n", operand2);
                        } else {
                            fprintf(out, "  mov rbx, [%s]\n", operand2);
                            fprintf(out, "  imul rax, rbx\n");
                        }
                    } else if (strcmp(operator, "/") == 0) {
                        fprintf(out, "  xor rdx, rdx\n");
                        if (operand2[0] >= '0' && operand2[0] <= '9') {
                            fprintf(out, "  mov rbx, %s\n", operand2);
                        } else {
                            fprintf(out, "  mov rbx, [%s]\n", operand2);
                        }
                        fprintf(out, "  idiv rbx\n");
                    }
                }
                
                // Store result
                fprintf(out, "  mov [%s], rax\n", stmt->target);
                
            } else if (stmt->type == STMT_PRINT) {
                fprintf(out, "  ; print(%s)\n", stmt->expr);
                // Print calls would be handled by print module
            }
            
            stmt = stmt->next;
        }
        
        // Jump back to start
        fprintf(out, "  jmp .while_start_%d\n", label_id);
        fprintf(out, ".while_end_%d:\n", label_id);
        fprintf(out, "\n");
        
        loop = loop->next;
    }
    }
    
    // Generate for loops
    if (for_count > 0) {
    ForLoop *floop = for_list;
    while (floop != NULL) {
        int label_id = label_counter++;
        
        fprintf(out, "; for %s = %s to %s step %s\n", floop->var, floop->start, floop->end, floop->step);
        
        // Initialize loop variable
        fprintf(out, "  mov rax, %s\n", floop->start);
        fprintf(out, "  mov [%s], rax\n", floop->var);
        
        fprintf(out, ".for_start_%d:\n", label_id);
        
        // Check condition: var <= end (for positive step)
        fprintf(out, "  mov rax, [%s]\n", floop->var);
        fprintf(out, "  mov rbx, %s\n", floop->end);
        fprintf(out, "  cmp rax, rbx\n");
        fprintf(out, "  jg .for_end_%d\n", label_id);
        
        // Loop body (placeholder)
        fprintf(out, "  ; body: %s\n", floop->body);
        
        // Increment loop variable
        fprintf(out, "  mov rax, [%s]\n", floop->var);
        fprintf(out, "  add rax, %s\n", floop->step);
        fprintf(out, "  mov [%s], rax\n", floop->var);
        
        // Jump back to start
        fprintf(out, "  jmp .for_start_%d\n", label_id);
        fprintf(out, ".for_end_%d:\n", label_id);
        fprintf(out, "\n");
        
        floop = floop->next;
    }
    }
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }

    printf("🔧 Control Flow Module\n");
    printf("======================\n");
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);

    // Read input file
    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "❌ Error: Cannot open input file %s\n", argv[1]);
        return 1;
    }

    fseek(input, 0, SEEK_END);
    long file_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    char *source = malloc(file_size + 1);
    fread(source, 1, file_size, input);
    source[file_size] = '\0';
    fclose(input);

    // Create lexer and parse
    Lexer *lexer = lexer_create(source);
    control_flow_parse(lexer);
    lexer_free(lexer);

    printf("\n  ✓ If statements found: %d\n", if_count);
    printf("  ✓ While loops found: %d\n", while_count);
    printf("  ✓ For loops found: %d\n", for_count);

    // Generate output
    FILE *output = fopen(argv[2], "w");
    if (!output) {
        fprintf(stderr, "❌ Error: Cannot create output file %s\n", argv[2]);
        free(source);
        return 1;
    }

    control_flow_codegen(output);
    fclose(output);
    free(source);

    printf("\n✅ Control flow module complete!\n");
    return 0;
}
