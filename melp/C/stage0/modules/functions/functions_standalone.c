#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lexer.h"

// Function Parameter Structure
typedef struct Parameter {
    char type[64];         // numeric, text, boolean
    char name[256];        // parameter name
    struct Parameter *next;
} Parameter;

// Local Variable Structure
typedef struct LocalVar {
    char type[64];         // numeric, text, boolean
    char name[256];        // variable name
    int stack_offset;      // offset from rbp
    struct LocalVar *next;
} LocalVar;

// Expression Structure (for arithmetic, function calls, etc.)
typedef struct Expression {
    char content[512];     // expression as string
} Expression;

// Statement Types
typedef enum {
    STMT_VARIABLE_DECL,
    STMT_ASSIGNMENT,
    STMT_PRINT,
    STMT_RETURN,
    STMT_FUNCTION_CALL,
    STMT_WHILE
} StmtType;

// Statement Structure
typedef struct Statement {
    StmtType type;
    char content[512];     // statement content
    struct Statement *next;
} Statement;

// Function Definition Structure
typedef struct FunctionDef {
    char name[256];        // function name
    char return_type[64];  // return type (or "void")
    Parameter *params;     // linked list of parameters
    int param_count;
    LocalVar *locals;      // local variables
    int local_count;
    Statement *body;       // function body statements
    struct FunctionDef *next;
} FunctionDef;

static FunctionDef *func_head = NULL;
static FunctionDef *func_tail = NULL;

// Helper: Add statement to function body
void add_statement(FunctionDef *func, StmtType type, const char *content) {
    Statement *stmt = malloc(sizeof(Statement));
    stmt->type = type;
    strncpy(stmt->content, content, 511);
    stmt->content[511] = '\0';
    stmt->next = NULL;
    
    // printf("    [DEBUG] Adding statement: type=%d content='%s'\n", type, content);
    
    if (func->body == NULL) {
        func->body = stmt;
    } else {
        Statement *last = func->body;
        while (last->next) last = last->next;
        last->next = stmt;
    }
}

// Helper: Add local variable
void add_local_var(FunctionDef *func, const char *type, const char *name) {
    LocalVar *var = malloc(sizeof(LocalVar));
    strcpy(var->type, type);
    strcpy(var->name, name);
    var->stack_offset = -8 * (func->local_count + 1);
    var->next = NULL;
    
    if (func->locals == NULL) {
        func->locals = var;
    } else {
        LocalVar *last = func->locals;
        while (last->next) last = last->next;
        last->next = var;
    }
    func->local_count++;
}

// Helper: Find local variable offset
int find_local_offset(FunctionDef *func, const char *name) {
    LocalVar *var = func->locals;
    while (var) {
        if (strcmp(var->name, name) == 0) {
            return var->stack_offset;
        }
        var = var->next;
    }
    return 0; // not found
}

// Helper: Find parameter index (for register mapping)
int find_param_index(FunctionDef *func, const char *name) {
    int idx = 0;
    Parameter *p = func->params;
    while (p) {
        if (strcmp(p->name, name) == 0) return idx;
        p = p->next;
        idx++;
    }
    return -1;
}

// Parse function body statements
void parse_function_body(Lexer *lexer, FunctionDef *func, Token *first_tok) {
    Token *tok = first_tok ? first_tok : lexer_next_token(lexer);
    
    while (tok->type != TOKEN_EOF) {
        // Check for "end function" - MUST come first!
        if (tok->type == TOKEN_END) {
            Token *next = lexer_next_token(lexer);
            if (next->type == TOKEN_FUNCTION) {
                token_free(tok);
                token_free(next);
                return;  // Exit function body parsing
            }
            // Not "end function", continue with next token
            token_free(tok);
            tok = next;
            continue;
        }
        
        // If we see another "function" keyword, something is wrong - exit
        if (tok->type == TOKEN_FUNCTION) {
            // This shouldn't happen in well-formed code
            // but let's handle it gracefully
            token_free(tok);
            return;
        }
        
        // Variable declaration: numeric x = 10
        if (tok->type == TOKEN_NUMERIC || tok->type == TOKEN_TEXT || tok->type == TOKEN_BOOLEAN) {
            Token *name_tok = lexer_next_token(lexer);
            if (name_tok->type == TOKEN_IDENTIFIER) {
                Token *assign_tok = lexer_next_token(lexer);
                if (assign_tok->type == TOKEN_ASSIGN) {
                    // Collect value expression until newline (or closing paren for function calls)
                    char expr[512] = "";
                    int paren_depth = 0;
                    Token *val_tok = lexer_next_token(lexer);
                    
                    // Read until we find a statement boundary
                    while (val_tok->type != TOKEN_EOF) {
                        // Track parentheses
                        if (val_tok->type == TOKEN_LPAREN) paren_depth++;
                        if (val_tok->type == TOKEN_RPAREN) paren_depth--;
                        
                        // Add token to expression
                        if (strlen(expr) > 0) strcat(expr, " ");
                        strcat(expr, val_tok->value);
                        
                        token_free(val_tok);
                        val_tok = lexer_next_token(lexer);
                        
                        // Stop if we hit a statement keyword and parens are balanced
                        if (paren_depth == 0) {
                            if (val_tok->type == TOKEN_PRINT || val_tok->type == TOKEN_RETURN ||
                                val_tok->type == TOKEN_END || val_tok->type == TOKEN_IF ||
                                val_tok->type == TOKEN_WHILE || val_tok->type == TOKEN_FOR ||
                                val_tok->type == TOKEN_NUMERIC || val_tok->type == TOKEN_TEXT ||
                                val_tok->type == TOKEN_BOOLEAN) {
                                // Put this token back by not consuming it
                                break;
                            }
                        }
                    }
                    
                    // Don't free val_tok - it will be processed in next iteration
                    
                    // Add local variable
                    add_local_var(func, tok->value, name_tok->value);
                    
                    // Add variable declaration statement
                    char stmt[512];
                    snprintf(stmt, 512, "%s %s = %s", tok->value, name_tok->value, expr);
                    add_statement(func, STMT_VARIABLE_DECL, stmt);
                    
                    token_free(assign_tok);
                    token_free(name_tok);
                    token_free(tok);
                    tok = val_tok; // Continue with the token we didn't consume
                    continue;
                }
                token_free(assign_tok);
            }
            token_free(name_tok);
            token_free(tok);
            tok = lexer_next_token(lexer);
            continue;
        }
        
        // Variable assignment: x = expression
        if (tok->type == TOKEN_IDENTIFIER) {
            Token *peek = lexer_next_token(lexer);
            
            // Assignment: x = ...
            if (peek->type == TOKEN_ASSIGN) {
                char assign_expr[512];
                snprintf(assign_expr, 512, "%s = ", tok->value);
                
                // Check if RHS is a function call
                Token *rhs_tok = lexer_next_token(lexer);
                if (rhs_tok->type == TOKEN_IDENTIFIER) {
                    Token *rhs_peek = lexer_next_token(lexer);
                    if (rhs_peek->type == TOKEN_LPAREN) {
                        // Function call: result = func(args)
                        char call_expr[512];
                        snprintf(call_expr, 512, "%s(", rhs_tok->value);
                        
                        Token *arg_tok = lexer_next_token(lexer);
                        while (arg_tok->type != TOKEN_RPAREN && arg_tok->type != TOKEN_EOF) {
                            strcat(call_expr, arg_tok->value);
                            Token *next = lexer_next_token(lexer);
                            token_free(arg_tok);
                            arg_tok = next;
                        }
                        strcat(call_expr, ")");
                        
                        // Create full assignment with call
                        char full_stmt[512];
                        snprintf(full_stmt, 512, "%s = %s", tok->value, call_expr);
                        add_statement(func, STMT_ASSIGNMENT, full_stmt);
                        
                        token_free(arg_tok);
                        token_free(rhs_peek);
                        token_free(rhs_tok);
                        token_free(peek);
                        token_free(tok);
                        tok = lexer_next_token(lexer);
                        continue;
                    } else {
                        // Regular expression
                        strcat(assign_expr, rhs_tok->value);
                        strcat(assign_expr, " ");
                        strcat(assign_expr, rhs_peek->value);
                        strcat(assign_expr, " ");
                        
                        Token *val_tok = lexer_next_token(lexer);
                        while (val_tok->type != TOKEN_EOF && val_tok->type != TOKEN_END) {
                            strcat(assign_expr, val_tok->value);
                            strcat(assign_expr, " ");
                            Token *next = lexer_next_token(lexer);
                            token_free(val_tok);
                            val_tok = next;
                            if (val_tok->type == TOKEN_END) {
                                token_free(val_tok);
                                break;
                            }
                        }
                        
                        token_free(rhs_peek);
                        token_free(rhs_tok);
                    }
                } else {
                    // Simple value or expression
                    strcat(assign_expr, rhs_tok->value);
                    strcat(assign_expr, " ");
                    
                    Token *val_tok = lexer_next_token(lexer);
                    while (val_tok->type != TOKEN_EOF && val_tok->type != TOKEN_END) {
                        strcat(assign_expr, val_tok->value);
                        strcat(assign_expr, " ");
                        Token *next = lexer_next_token(lexer);
                        token_free(val_tok);
                        val_tok = next;
                        if (val_tok->type == TOKEN_END) {
                            token_free(val_tok);
                            break;
                        }
                    }
                    
                    token_free(rhs_tok);
                }
                
                add_statement(func, STMT_ASSIGNMENT, assign_expr);
                token_free(peek);
                token_free(tok);
                tok = lexer_next_token(lexer);
                continue;
            }
            
            // Function call: result = func(args) or just func(args)
            if (peek->type == TOKEN_LPAREN) {
                char call_expr[512];
                snprintf(call_expr, 512, "%s(", tok->value);
                
                Token *arg_tok = lexer_next_token(lexer);
                while (arg_tok->type != TOKEN_RPAREN && arg_tok->type != TOKEN_EOF) {
                    strcat(call_expr, arg_tok->value);
                    Token *next = lexer_next_token(lexer);
                    token_free(arg_tok);
                    arg_tok = next;
                }
                strcat(call_expr, ")");
                
                add_statement(func, STMT_FUNCTION_CALL, call_expr);
                token_free(arg_tok);
                token_free(peek);
                token_free(tok);
                tok = lexer_next_token(lexer);
                continue;
            }
            
            token_free(peek);
        }
        
        // Print statement: print("text") or print(variable)
        if (tok->type == TOKEN_PRINT) {
            Token *lparen = lexer_next_token(lexer);
            if (lparen->type == TOKEN_LPAREN) {
                char print_expr[512] = "print(";
                Token *arg_tok = lexer_next_token(lexer);
                while (arg_tok->type != TOKEN_RPAREN && arg_tok->type != TOKEN_EOF) {
                    strcat(print_expr, arg_tok->value);
                    Token *next = lexer_next_token(lexer);
                    token_free(arg_tok);
                    arg_tok = next;
                }
                strcat(print_expr, ")");
                
                add_statement(func, STMT_PRINT, print_expr);
                token_free(arg_tok);
            }
            token_free(lparen);
            token_free(tok);
            tok = lexer_next_token(lexer);
            continue;
        }
        
        // Return statement: return expression
        if (tok->type == TOKEN_RETURN) {
            char ret_expr[512] = "return ";
            Token *expr_tok = lexer_next_token(lexer);
            while (expr_tok->type != TOKEN_EOF && expr_tok->type != TOKEN_END) {
                strcat(ret_expr, expr_tok->value);
                strcat(ret_expr, " ");
                Token *next = lexer_next_token(lexer);
                token_free(expr_tok);
                expr_tok = next;
                if (expr_tok->type == TOKEN_END) {
                    token_free(expr_tok);
                    break;
                }
            }
            
            add_statement(func, STMT_RETURN, ret_expr);
            token_free(tok);
            tok = lexer_next_token(lexer);
            continue;
        }
        
        // While loop: while condition ... end while
        if (tok->type == TOKEN_WHILE) {
            char while_stmt[512] = "while ";
            
            // Parse condition and body
            Token *cond_tok = lexer_next_token(lexer);
            int paren_depth = 0;
            
            while (cond_tok->type != TOKEN_EOF && cond_tok->type != TOKEN_END) {
                if (cond_tok->type == TOKEN_LPAREN) paren_depth++;
                if (cond_tok->type == TOKEN_RPAREN) paren_depth--;
                
                strcat(while_stmt, cond_tok->value);
                strcat(while_stmt, " ");
                
                Token *next = lexer_next_token(lexer);
                token_free(cond_tok);
                cond_tok = next;
                
                // Check if we hit "end while"
                if (cond_tok->type == TOKEN_END) {
                    Token *check_while = lexer_next_token(lexer);
                    if (check_while->type == TOKEN_WHILE) {
                        token_free(check_while);
                        token_free(cond_tok);
                        break;
                    }
                    // Not "end while", put back and continue
                    strcat(while_stmt, "end ");
                    token_free(check_while);
                    cond_tok = lexer_next_token(lexer);
                }
            }
            
            add_statement(func, STMT_WHILE, while_stmt);
            token_free(tok);
            tok = lexer_next_token(lexer);
            continue;
        }
        
        Token *next = lexer_next_token(lexer);
        token_free(tok);
        tok = next;
    }
    
    token_free(tok);
}

// Parse function definitions: function name(type param, ...) ... end function
void functions_parse(Lexer *lexer) {
    Token *tok = lexer_next_token(lexer);
    
    while (tok->type != TOKEN_EOF) {
        // Debug: print token
        // printf("DEBUG: Token type=%d value='%s'\n", tok->type, tok->value);
        
        // Look for: function identifier ( params ) ... end function
        if (tok->type == TOKEN_FUNCTION) {
            Token *name_tok = lexer_next_token(lexer);
            if (name_tok->type != TOKEN_IDENTIFIER) {
                token_free(tok);
                tok = name_tok;
                continue;
            }
            
            FunctionDef *func = malloc(sizeof(FunctionDef));
            strcpy(func->name, name_tok->value);
            func->params = NULL;
            func->param_count = 0;
            func->locals = NULL;
            func->local_count = 0;
            func->body = NULL;
            strcpy(func->return_type, "void");
            func->next = NULL;
            
            Token *lparen_tok = lexer_next_token(lexer);
            if (lparen_tok->type != TOKEN_LPAREN) {
                free(func);
                token_free(tok);
                token_free(name_tok);
                tok = lparen_tok;
                continue;
            }
            
            // Parse parameters: type name, type name, ...
            Parameter *last_param = NULL;
            Token *param_tok = lexer_next_token(lexer);
            
            while (param_tok->type != TOKEN_RPAREN && param_tok->type != TOKEN_EOF) {
                // Expect: type name
                if (param_tok->type == TOKEN_NUMERIC || param_tok->type == TOKEN_TEXT || 
                    param_tok->type == TOKEN_BOOLEAN) {
                    
                    Token *param_name_tok = lexer_next_token(lexer);
                    if (param_name_tok->type == TOKEN_IDENTIFIER) {
                        Parameter *param = malloc(sizeof(Parameter));
                        strcpy(param->type, param_tok->value);
                        strcpy(param->name, param_name_tok->value);
                        param->next = NULL;
                        
                        if (func->params == NULL) {
                            func->params = param;
                        } else {
                            last_param->next = param;
                        }
                        last_param = param;
                        func->param_count++;
                        
                        token_free(param_tok);
                        token_free(param_name_tok);
                        
                        // Check for comma or rparen
                        param_tok = lexer_next_token(lexer);
                        if (param_tok->type == TOKEN_COMMA) {
                            token_free(param_tok);
                            param_tok = lexer_next_token(lexer);
                        }
                    } else {
                        token_free(param_tok);
                        token_free(param_name_tok);
                        break;
                    }
                } else {
                    token_free(param_tok);
                    break;
                }
            }
            
            if (param_tok->type == TOKEN_RPAREN) {
                token_free(param_tok);
            }
            
            // Check for return type after parameters: function name() type
            Token *return_type_tok = lexer_next_token(lexer);
            if (return_type_tok->type == TOKEN_NUMERIC || 
                return_type_tok->type == TOKEN_TEXT || 
                return_type_tok->type == TOKEN_BOOLEAN) {
                strcpy(func->return_type, return_type_tok->value);
                // Parse function body - pass NULL as first token (will read fresh)
                token_free(return_type_tok);
                parse_function_body(lexer, func, NULL);
            } else {
                // Not a return type token - pass it as first token to function body
                parse_function_body(lexer, func, return_type_tok);
            }
            
            // Add to function list
            if (func_head == NULL) {
                func_head = func;
                func_tail = func;
            } else {
                func_tail->next = func;
                func_tail = func;
            }
            
            // Count statements
            int stmt_count = 0;
            Statement *s = func->body;
            while (s) {
                stmt_count++;
                s = s->next;
            }
            
            printf("  ✓ Function: %s(", func->name);
            Parameter *p = func->params;
            while (p) {
                printf("%s %s", p->type, p->name);
                if (p->next) printf(", ");
                p = p->next;
            }
            printf(") - %d statements, %d locals\n", stmt_count, func->local_count);
            
            token_free(tok);
            token_free(name_tok);
            token_free(lparen_tok);
            // DON'T read next token here - parse_function_body already consumed "end function"
            // Just continue the loop which will read the next token
            tok = lexer_next_token(lexer);
            continue;
        }
        
        Token *next_tok = lexer_next_token(lexer);
        token_free(tok);
        tok = next_tok;
    }
    
    token_free(tok);
}

// Generate assembly for function definitions
void functions_codegen(FILE *out) {
    int func_count = 0;
    FunctionDef *func = func_head;
    while (func != NULL) {
        func_count++;
        func = func->next;
    }
    
    if (func_count == 0) {
        return;
    }
    
    fprintf(out, "\n; Function definitions (%d functions)\n", func_count);
    
    func = func_head;
    int string_counter = 0;
    
    // First, generate all string constants in data section
    fprintf(out, "section .data\n");
    func = func_head;
    while (func != NULL) {
        Statement *stmt = func->body;
        while (stmt) {
            if (stmt->type == STMT_PRINT) {
                char arg[256];
                if (sscanf(stmt->content, "print(\"%[^\"]\")", arg) == 1) {
                    fprintf(out, "  str_%d: db \"%s\", 10, 0\n", string_counter, arg);
                    fprintf(out, "  str_%d_len: equ $-str_%d-2\n", string_counter, string_counter);
                    string_counter++;
                }
            }
            stmt = stmt->next;
        }
        func = func->next;
    }
    
    fprintf(out, "\nsection .text\n");
    fprintf(out, "  global _start\n\n");
    
    func = func_head;
    string_counter = 0;
    
    while (func != NULL) {
        fprintf(out, "; function %s(", func->name);
        Parameter *p = func->params;
        while (p) {
            fprintf(out, "%s %s", p->type, p->name);
            if (p->next) fprintf(out, ", ");
            p = p->next;
        }
        fprintf(out, ")\n");
        
        // Generate function label and prologue
        fprintf(out, "%s:\n", func->name);
        fprintf(out, "  push rbp\n");
        fprintf(out, "  mov rbp, rsp\n");
        
        // Reserve space for local variables
        if (func->local_count > 0) {
            fprintf(out, "  sub rsp, %d     ; allocate %d locals\n", 
                    func->local_count * 8, func->local_count);
        }
        
        // Generate code for each statement in function body
        Statement *stmt = func->body;
        while (stmt) {
            switch (stmt->type) {
                case STMT_VARIABLE_DECL: {
                    // Parse: numeric x = 10 or numeric x = a + b or numeric x = func(args)
                    char type[64], name[256], value[512];
                    if (sscanf(stmt->content, "%s %s = %[^\n]", type, name, value) == 3) {
                        int offset = find_local_offset(func, name);
                        fprintf(out, "  ; %s\n", stmt->content);
                        
                        // Check if value is a function call
                        char func_name[256], args_str[512];
                        if (sscanf(value, "%[^(](%[^)])", func_name, args_str) == 2) {
                            // Function call: result = func(args)
                            
                            // Parse and load arguments
                            char *args[10];
                            int arg_count = 0;
                            char args_copy[512];
                            strcpy(args_copy, args_str);
                            char *token = strtok(args_copy, ",");
                            while (token && arg_count < 10) {
                                while (*token == ' ') token++;
                                args[arg_count++] = token;
                                token = strtok(NULL, ",");
                            }
                            
                            // Load arguments into registers
                            const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                            for (int i = 0; i < arg_count && i < 6; i++) {
                                int num_value;
                                if (sscanf(args[i], "%d", &num_value) == 1) {
                                    fprintf(out, "  mov %s, %d       ; arg%d\n", regs[i], num_value, i+1);
                                } else {
                                    int arg_offset = find_local_offset(func, args[i]);
                                    int param_idx = find_param_index(func, args[i]);
                                    if (arg_offset != 0) {
                                        fprintf(out, "  mov %s, [rbp%d]  ; arg%d = %s\n", 
                                                regs[i], arg_offset, i+1, args[i]);
                                    } else if (param_idx >= 0) {
                                        fprintf(out, "  mov %s, %s       ; arg%d = %s\n",
                                                regs[i], regs[param_idx], i+1, args[i]);
                                    }
                                }
                            }
                            
                            fprintf(out, "  call %s\n", func_name);
                            fprintf(out, "  mov [rbp%d], rax     ; store result in %s\n", offset, name);
                        }
                        // Check if value is a number
                        else {
                            int num_value;
                            if (sscanf(value, "%d", &num_value) == 1) {
                                // Simple number
                                fprintf(out, "  mov qword [rbp%d], %d\n", offset, num_value);
                            } else {
                                // Expression - parse simple binary operations
                                char left[256], op[8], right[256];
                                if (sscanf(value, "%s %s %s", left, op, right) == 3) {
                                // Load left operand
                                int left_num, right_num;
                                if (sscanf(left, "%d", &left_num) == 1) {
                                    fprintf(out, "  mov rax, %d\n", left_num);
                                } else {
                                    int left_param = find_param_index(func, left);
                                    int left_offset = find_local_offset(func, left);
                                    if (left_param >= 0) {
                                        const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                                        fprintf(out, "  mov rax, %s      ; %s\n", regs[left_param], left);
                                    } else if (left_offset != 0) {
                                        fprintf(out, "  mov rax, [rbp%d]  ; %s\n", left_offset, left);
                                    }
                                }
                                
                                // Apply operation with right operand
                                if (sscanf(right, "%d", &right_num) == 1) {
                                    if (strcmp(op, "+") == 0) {
                                        fprintf(out, "  add rax, %d\n", right_num);
                                    } else if (strcmp(op, "-") == 0) {
                                        fprintf(out, "  sub rax, %d\n", right_num);
                                    } else if (strcmp(op, "*") == 0) {
                                        fprintf(out, "  imul rax, %d\n", right_num);
                                    }
                                } else {
                                    int right_param = find_param_index(func, right);
                                    int right_offset = find_local_offset(func, right);
                                    if (right_param >= 0) {
                                        const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                                        if (strcmp(op, "+") == 0) {
                                            fprintf(out, "  add rax, %s      ; + %s\n", regs[right_param], right);
                                        } else if (strcmp(op, "-") == 0) {
                                            fprintf(out, "  sub rax, %s      ; - %s\n", regs[right_param], right);
                                        } else if (strcmp(op, "*") == 0) {
                                            fprintf(out, "  imul rax, %s     ; * %s\n", regs[right_param], right);
                                        }
                                    } else if (right_offset != 0) {
                                        if (strcmp(op, "+") == 0) {
                                            fprintf(out, "  add rax, [rbp%d]  ; + %s\n", right_offset, right);
                                        } else if (strcmp(op, "-") == 0) {
                                            fprintf(out, "  sub rax, [rbp%d]  ; - %s\n", right_offset, right);
                                        } else if (strcmp(op, "*") == 0) {
                                            fprintf(out, "  imul rax, [rbp%d] ; * %s\n", right_offset, right);
                                        }
                                    }
                                }
                                
                                fprintf(out, "  mov [rbp%d], rax     ; store %s\n", offset, name);
                            }
                        }
                        }
                    }
                    break;
                }
                
                case STMT_ASSIGNMENT: {
                    // Parse: x = expression OR x = func(args)
                    char name[256], value[512];
                    if (sscanf(stmt->content, "%s = %[^\n]", name, value) == 2) {
                        int offset = find_local_offset(func, name);
                        fprintf(out, "  ; %s\n", stmt->content);
                        
                        // Check if it's a function call
                        char func_name[256], args_str[512];
                        if (sscanf(value, "%[^(](%[^)])", func_name, args_str) == 2) {
                            // Function call assignment: x = func(args)
                            
                            // Parse and load arguments
                            char *args[10];
                            int arg_count = 0;
                            char args_copy[512];
                            strcpy(args_copy, args_str);
                            char *token = strtok(args_copy, ",");
                            while (token && arg_count < 10) {
                                while (*token == ' ') token++;
                                args[arg_count++] = token;
                                token = strtok(NULL, ",");
                            }
                            
                            // Load arguments into registers
                            const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                            for (int i = 0; i < arg_count && i < 6; i++) {
                                int num_value;
                                if (sscanf(args[i], "%d", &num_value) == 1) {
                                    fprintf(out, "  mov %s, %d       ; arg%d\n", regs[i], num_value, i+1);
                                } else {
                                    int arg_offset = find_local_offset(func, args[i]);
                                    int param_idx = find_param_index(func, args[i]);
                                    if (arg_offset != 0) {
                                        fprintf(out, "  mov %s, [rbp%d]  ; arg%d = %s\n", 
                                                regs[i], arg_offset, i+1, args[i]);
                                    } else if (param_idx >= 0) {
                                        fprintf(out, "  mov %s, %s       ; arg%d = %s\n",
                                                regs[i], regs[param_idx], i+1, args[i]);
                                    }
                                }
                            }
                            
                            fprintf(out, "  call %s\n", func_name);
                            fprintf(out, "  mov [rbp%d], rax     ; store result in %s\n", offset, name);
                        } else {
                            // Simple assignment or expression
                            int num_value;
                            if (sscanf(value, "%d", &num_value) == 1) {
                                fprintf(out, "  mov qword [rbp%d], %d\n", offset, num_value);
                            } else {
                                // Expression (similar to variable decl)
                                char left[256], op[8], right[256];
                                if (sscanf(value, "%s %s %s", left, op, right) == 3) {
                                    // Load, compute, store
                                    int left_offset = find_local_offset(func, left);
                                    fprintf(out, "  mov rax, [rbp%d]     ; load %s\n", left_offset, left);
                                    
                                    int right_num;
                                    if (sscanf(right, "%d", &right_num) == 1) {
                                        if (strcmp(op, "+") == 0) {
                                            fprintf(out, "  add rax, %d\n", right_num);
                                        } else if (strcmp(op, "-") == 0) {
                                            fprintf(out, "  sub rax, %d\n", right_num);
                                        }
                                    }
                                    
                                    fprintf(out, "  mov [rbp%d], rax     ; store %s\n", offset, name);
                                }
                            }
                        }
                    }
                    break;
                }
                
                case STMT_PRINT: {
                    // Parse: print("text") or print(variable)
                    fprintf(out, "  ; %s\n", stmt->content);
                    char arg[256];
                    if (sscanf(stmt->content, "print(\"%[^\"]\")", arg) == 1) {
                        // String literal
                        fprintf(out, "  mov rax, 1           ; sys_write\n");
                        fprintf(out, "  mov rdi, 1           ; stdout\n");
                        fprintf(out, "  lea rsi, [rel str_%d]\n", string_counter);
                        fprintf(out, "  mov rdx, str_%d_len\n", string_counter);
                        fprintf(out, "  add rdx, 1\n");
                        fprintf(out, "  syscall\n");
                        string_counter++;
                    } else if (sscanf(stmt->content, "print(%[^)])", arg) == 1) {
                        // Variable
                        int offset = find_local_offset(func, arg);
                        fprintf(out, "  mov rdi, [rbp%d]     ; %s\n", offset, arg);
                        fprintf(out, "  call print_number\n");
                    }
                    break;
                }
                
                case STMT_RETURN: {
                    // Parse: return expression
                    char expr[512];
                    if (sscanf(stmt->content, "return %[^\n]", expr) == 1) {
                        fprintf(out, "  ; %s\n", stmt->content);
                        
                        int num_value;
                        if (sscanf(expr, "%d", &num_value) == 1) {
                            // Simple number
                            fprintf(out, "  mov rax, %d\n", num_value);
                        } else {
                            // Expression: a + b, a - b, etc.
                            char left[256], op[8], right[256];
                            if (sscanf(expr, "%s %s %s", left, op, right) == 3) {
                                // Load left operand
                                int left_param = find_param_index(func, left);
                                int left_offset = find_local_offset(func, left);
                                if (left_param >= 0) {
                                    const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                                    fprintf(out, "  mov rax, %s      ; %s\n", regs[left_param], left);
                                } else if (left_offset != 0) {
                                    fprintf(out, "  mov rax, [rbp%d]  ; %s\n", left_offset, left);
                                }
                                
                                // Apply operation
                                int right_param = find_param_index(func, right);
                                int right_offset = find_local_offset(func, right);
                                if (right_param >= 0) {
                                    const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                                    if (strcmp(op, "+") == 0) {
                                        fprintf(out, "  add rax, %s      ; + %s\n", regs[right_param], right);
                                    } else if (strcmp(op, "-") == 0) {
                                        fprintf(out, "  sub rax, %s      ; - %s\n", regs[right_param], right);
                                    } else if (strcmp(op, "*") == 0) {
                                        fprintf(out, "  imul rax, %s     ; * %s\n", regs[right_param], right);
                                    }
                                } else if (right_offset != 0) {
                                    if (strcmp(op, "+") == 0) {
                                        fprintf(out, "  add rax, [rbp%d]  ; + %s\n", right_offset, right);
                                    } else if (strcmp(op, "-") == 0) {
                                        fprintf(out, "  sub rax, [rbp%d]  ; - %s\n", right_offset, right);
                                    } else if (strcmp(op, "*") == 0) {
                                        fprintf(out, "  imul rax, [rbp%d] ; * %s\n", right_offset, right);
                                    }
                                }
                            } else {
                                // Single variable or parameter
                                int param_idx = find_param_index(func, expr);
                                int local_offset = find_local_offset(func, expr);
                                if (param_idx >= 0) {
                                    const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                                    fprintf(out, "  mov rax, %s      ; %s\n", regs[param_idx], expr);
                                } else if (local_offset != 0) {
                                    fprintf(out, "  mov rax, [rbp%d]  ; %s\n", local_offset, expr);
                                }
                            }
                        }
                    }
                    break;
                }
                
                case STMT_FUNCTION_CALL: {
                    // Parse: func(arg1, arg2, ...)
                    char func_name[256], args_str[512];
                    if (sscanf(stmt->content, "%[^(](%[^)])", func_name, args_str) == 2) {
                        fprintf(out, "  ; call %s\n", func_name);
                        
                        // Parse arguments
                        char *args[10];
                        int arg_count = 0;
                        char args_copy[512];
                        strcpy(args_copy, args_str);
                        char *token = strtok(args_copy, ",");
                        while (token && arg_count < 10) {
                            // Trim spaces
                            while (*token == ' ') token++;
                            args[arg_count++] = token;
                            token = strtok(NULL, ",");
                        }
                        
                        // Load arguments into registers (x86-64 calling convention)
                        const char *regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                        for (int i = 0; i < arg_count && i < 6; i++) {
                            int num_value;
                            if (sscanf(args[i], "%d", &num_value) == 1) {
                                fprintf(out, "  mov %s, %d       ; arg%d\n", regs[i], num_value, i+1);
                            } else {
                                // Variable
                                int offset = find_local_offset(func, args[i]);
                                int param_idx = find_param_index(func, args[i]);
                                if (offset != 0) {
                                    fprintf(out, "  mov %s, [rbp%d]  ; arg%d = %s\n", 
                                            regs[i], offset, i+1, args[i]);
                                } else if (param_idx >= 0) {
                                    fprintf(out, "  mov %s, %s       ; arg%d = %s\n",
                                            regs[i], regs[param_idx], i+1, args[i]);
                                }
                            }
                        }
                        
                        fprintf(out, "  call %s\n", func_name);
                    }
                    break;
                }
                
                case STMT_WHILE: {
                    // Parse: while condition body end while
                    static int while_label_counter = 0;
                    int label_num = while_label_counter++;
                    
                    fprintf(out, "  ; while loop\n");
                    fprintf(out, ".while_start_%d:\n", label_num);
                    
                    // Parse condition: variable < value or variable > value etc
                    char left[256], op[8], right[256];
                    char *cond_start = stmt->content + 6;  // Skip "while "
                    
                    // Find the condition part (before first statement keyword)
                    char condition[256] = "";
                    char *ptr = cond_start;
                    while (*ptr && strncmp(ptr, "end", 3) != 0) {
                        // Look for statements that mark end of condition
                        if (strncmp(ptr, "numeric", 7) == 0 || 
                            strncmp(ptr, "text", 4) == 0 ||
                            strncmp(ptr, "print", 5) == 0 ||
                            strncmp(ptr, "return", 6) == 0) {
                            break;
                        }
                        strncat(condition, ptr, 1);
                        ptr++;
                    }
                    
                    // Parse condition
                    if (sscanf(condition, "%s %s %s", left, op, right) == 3) {
                        int left_offset = find_local_offset(func, left);
                        int right_num;
                        
                        fprintf(out, "  mov rax, [rbp%d]     ; load %s\n", left_offset, left);
                        
                        if (sscanf(right, "%d", &right_num) == 1) {
                            fprintf(out, "  cmp rax, %d\n", right_num);
                        } else {
                            int right_offset = find_local_offset(func, right);
                            fprintf(out, "  cmp rax, [rbp%d]\n", right_offset);
                        }
                        
                        // Jump based on comparison operator
                        if (strcmp(op, "<") == 0) {
                            fprintf(out, "  jge .while_end_%d    ; exit if >= \n", label_num);
                        } else if (strcmp(op, ">") == 0) {
                            fprintf(out, "  jle .while_end_%d    ; exit if <=\n", label_num);
                        } else if (strcmp(op, "<=") == 0) {
                            fprintf(out, "  jg .while_end_%d     ; exit if >\n", label_num);
                        } else if (strcmp(op, ">=") == 0) {
                            fprintf(out, "  jl .while_end_%d     ; exit if <\n", label_num);
                        } else if (strcmp(op, "==") == 0) {
                            fprintf(out, "  jne .while_end_%d    ; exit if !=\n", label_num);
                        } else if (strcmp(op, "!=") == 0) {
                            fprintf(out, "  je .while_end_%d     ; exit if ==\n", label_num);
                        }
                        
                        // TODO: Parse and generate body statements here
                        // For now, body is in the stmt->content after condition
                        
                        fprintf(out, "  jmp .while_start_%d   ; loop back\n", label_num);
                        fprintf(out, ".while_end_%d:\n", label_num);
                    }
                    
                    break;
                }
            }
            
            stmt = stmt->next;
        }
        
        // Epilogue
        fprintf(out, "  mov rsp, rbp\n");
        fprintf(out, "  pop rbp\n");
        fprintf(out, "  ret\n");
        fprintf(out, "\n");
        
        func = func->next;
    }
    
    // Add _start entry point
    fprintf(out, "_start:\n");
    fprintf(out, "  call main\n");
    fprintf(out, "  mov rdi, rax     ; exit code from main\n");
    fprintf(out, "  mov rax, 60      ; sys_exit\n");
    fprintf(out, "  syscall\n\n");
    
    // Add print_number helper function
    fprintf(out, "; Helper function to print numbers\n");
    fprintf(out, "print_number:\n");
    fprintf(out, "  push rbp\n");
    fprintf(out, "  mov rbp, rsp\n");
    fprintf(out, "  sub rsp, 32\n");
    fprintf(out, "  mov rax, rdi         ; number to print\n");
    fprintf(out, "  mov rcx, 10          ; divisor\n");
    fprintf(out, "  lea rsi, [rbp-1]     ; start from end of buffer\n");
    fprintf(out, "  mov byte [rsi], 10   ; newline at end\n");
    fprintf(out, "  dec rsi\n");
    fprintf(out, ".convert_loop:\n");
    fprintf(out, "  xor rdx, rdx\n");
    fprintf(out, "  div rcx              ; rax = quotient, rdx = remainder\n");
    fprintf(out, "  add dl, '0'          ; convert to ASCII\n");
    fprintf(out, "  mov [rsi], dl        ; store digit\n");
    fprintf(out, "  dec rsi              ; move backwards\n");
    fprintf(out, "  test rax, rax\n");
    fprintf(out, "  jnz .convert_loop\n");
    fprintf(out, "  ; Now print from rsi+1 to rbp-1\n");
    fprintf(out, "  inc rsi              ; move to first digit\n");
    fprintf(out, "  mov rax, 1           ; sys_write\n");
    fprintf(out, "  mov rdi, 1           ; stdout\n");
    fprintf(out, "  lea rdx, [rbp-1]     ; end pointer\n");
    fprintf(out, "  sub rdx, rsi         ; length = end - start\n");
    fprintf(out, "  add rdx, 1           ; include newline\n");
    fprintf(out, "  syscall\n");
    fprintf(out, "  mov rsp, rbp\n");
    fprintf(out, "  pop rbp\n");
    fprintf(out, "  ret\n");
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    printf("🔧 Functions Module - Complete Implementation\n");
    printf("=============================================\n");
    printf("Input: %s\n\n", argv[1]);
    
    // Read source file
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(sz + 1);
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);
    
    // Remove comments (simple -- comment removal)
    char *cleaned = malloc(sz + 1);
    int ci = 0;
    for (int i = 0; src[i]; i++) {
        if (src[i] == '-' && src[i+1] == '-') {
            // Skip until newline
            while (src[i] && src[i] != '\n') i++;
            if (src[i] == '\n') cleaned[ci++] = '\n';
        } else {
            cleaned[ci++] = src[i];
        }
    }
    cleaned[ci] = 0;
    free(src);
    src = cleaned;
    
    // Create lexer and parse
    Lexer *lexer = lexer_create(src);
    functions_parse(lexer);
    
    // Count functions
    int func_count = 0;
    FunctionDef *func = func_head;
    while (func != NULL) {
        func_count++;
        func = func->next;
    }
    printf("\n  ✓ Functions found: %d\n\n", func_count);
    
    // Generate assembly
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open output file\n");
        lexer_free(lexer);
        free(src);
        return 1;
    }
    
    functions_codegen(out);
    
    fclose(out);
    lexer_free(lexer);
    free(src);
    
    printf("✅ Assembly generated successfully!\n");
    return 0;
}
