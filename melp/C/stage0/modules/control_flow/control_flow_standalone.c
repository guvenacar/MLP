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

typedef struct WhileLoop {
    Condition cond;
    char body[1024];
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
static void add_while_loop(Condition cond, const char *body) {
    WhileLoop *new_while = malloc(sizeof(WhileLoop));
    new_while->cond = cond;
    strcpy(new_while->body, body);
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
            token = lexer_next_token(lexer);
            
            // Collect body until END
            char body[1024] = "";
            
            while (token->type != TOKEN_END && token->type != TOKEN_EOF) {
                if (strlen(body) > 0) strcat(body, " ");
                strcat(body, token->value);
                token_free(token);
                token = lexer_next_token(lexer);
            }
            
            // Expect END WHILE
            if (token->type == TOKEN_END) {
                token_free(token);
                token = lexer_next_token(lexer);
                // Skip WHILE token after END
                if (token->type == TOKEN_WHILE) {
                    add_while_loop(cond, body);
                    printf("  ✓ While loop: %s %s %s\n", cond.left, cond.op, cond.right);
                }
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
        
        // Loop body (placeholder)
        fprintf(out, "  ; body: %s\n", loop->body);
        
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
