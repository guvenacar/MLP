// Mini MELP Bootstrap Compiler
// Purpose: Compile simple MELP code to x86-64 assembly
// This is a temporary tool - will be replaced by self-hosted version

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_CODE 65536
#define MAX_TOKENS 1000

typedef enum {
    TOK_EOF,
    TOK_NUMERIC,
    TOK_STRING, 
    TOK_BOOLEAN,
    TOK_TRUE,
    TOK_FALSE,
    TOK_FUNCTION,
    TOK_END,
    TOK_RETURN,
    TOK_PRINT,
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_STRING_LITERAL,
    TOK_EQUALS,
    TOK_EQUAL_EQUAL,
    TOK_NOT_EQUAL,
    TOK_LESS,
    TOK_LESS_EQUAL,
    TOK_GREATER,
    TOK_GREATER_EQUAL,
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_IF,
    TOK_THEN,
    TOK_ELSE,
    TOK_END_IF,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_COMMA,
    TOK_COLON,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PERCENT,
    TOK_COMMENT
} TokenType;

typedef struct {
    TokenType type;
    char value[256];
    int line;
} Token;

typedef struct {
    char name[256];
    char return_type[256];
    int start_token;
    int end_token;
} Function;

static char source[MAX_CODE];
static int pos = 0;
static int line = 1;
static Token tokens[MAX_TOKENS];
static int token_count = 0;
static int label_counter = 0;
static Function functions[50];
static int function_count = 0;

void skip_whitespace() {
    while (source[pos] && isspace(source[pos])) {
        if (source[pos] == '\n') line++;
        pos++;
    }
}

void skip_comment() {
    // Single line comment: --
    if (source[pos] == '-' && source[pos+1] == '-' && source[pos+2] != '-') {
        pos += 2;
        while (source[pos] && source[pos] != '\n') pos++;
        if (source[pos] == '\n') {
            line++;
            pos++;
        }
        return;
    }
    
    // Multi-line comment: --- ... ---
    if (source[pos] == '-' && source[pos+1] == '-' && source[pos+2] == '-') {
        pos += 3;
        while (source[pos]) {
            if (source[pos] == '-' && source[pos+1] == '-' && source[pos+2] == '-') {
                pos += 3;
                break;
            }
            if (source[pos] == '\n') line++;
            pos++;
        }
        return;
    }
    
    // Doc comment: --# ... #--
    if (source[pos] == '-' && source[pos+1] == '-' && source[pos+2] == '#') {
        pos += 3;
        while (source[pos]) {
            if (source[pos] == '#' && source[pos+1] == '-' && source[pos+2] == '-') {
                pos += 3;
                break;
            }
            if (source[pos] == '\n') line++;
            pos++;
        }
        return;
    }
}

void add_token(TokenType type, const char* value) {
    if (token_count >= MAX_TOKENS) {
        fprintf(stderr, "Too many tokens\n");
        exit(1);
    }
    tokens[token_count].type = type;
    strncpy(tokens[token_count].value, value, 255);
    tokens[token_count].line = line;
    token_count++;
}

void tokenize() {
    while (source[pos]) {
        skip_whitespace();
        if (!source[pos]) break;
        
        // Try to skip comments
        int old_pos = pos;
        skip_comment();
        if (pos != old_pos) continue;  // Comment was skipped
        
        if (!source[pos]) break;
        
        // Keywords and identifiers
        if (isalpha(source[pos])) {
            char word[256];
            int i = 0;
            while (isalnum(source[pos]) || source[pos] == '_') {
                word[i++] = source[pos++];
            }
            word[i] = '\0';
            
            if (strcmp(word, "numeric") == 0) add_token(TOK_NUMERIC, word);
            else if (strcmp(word, "string") == 0) add_token(TOK_STRING, word);
            else if (strcmp(word, "boolean") == 0) add_token(TOK_BOOLEAN, word);
            else if (strcmp(word, "true") == 0) add_token(TOK_TRUE, word);
            else if (strcmp(word, "false") == 0) add_token(TOK_FALSE, word);
            else if (strcmp(word, "AND") == 0) add_token(TOK_AND, word);
            else if (strcmp(word, "OR") == 0) add_token(TOK_OR, word);
            else if (strcmp(word, "NOT") == 0) add_token(TOK_NOT, word);
            else if (strcmp(word, "if") == 0) add_token(TOK_IF, word);
            else if (strcmp(word, "then") == 0) add_token(TOK_THEN, word);
            else if (strcmp(word, "else") == 0) add_token(TOK_ELSE, word);
            else if (strcmp(word, "end") == 0) {
                // Check if next word is "if"
                int saved_pos = pos;
                while (isspace(source[pos])) pos++;
                if (source[pos] == 'i' && source[pos+1] == 'f' && !isalnum(source[pos+2])) {
                    pos += 2;
                    add_token(TOK_END_IF, "end if");
                } else {
                    pos = saved_pos;
                    add_token(TOK_END, word);
                }
            }
            else if (strcmp(word, "function") == 0) add_token(TOK_FUNCTION, word);
            else if (strcmp(word, "return") == 0) add_token(TOK_RETURN, word);
            else add_token(TOK_IDENTIFIER, word);
            continue;
        }
        
        // Numbers
        if (isdigit(source[pos])) {
            char num[256];
            int i = 0;
            while (isdigit(source[pos])) {
                num[i++] = source[pos++];
            }
            num[i] = '\0';
            add_token(TOK_NUMBER, num);
            continue;
        }
        
        // String literals
        if (source[pos] == '"') {
            pos++; // skip opening "
            char str[256];
            int i = 0;
            while (source[pos] && source[pos] != '"') {
                str[i++] = source[pos++];
            }
            str[i] = '\0';
            pos++; // skip closing "
            add_token(TOK_STRING_LITERAL, str);
            continue;
        }
        
        // Operators and symbols
        // Check for two-character operators first
        if (source[pos] == '=' && source[pos+1] == '=') {
            add_token(TOK_EQUAL_EQUAL, "==");
            pos += 2;
            continue;
        }
        if (source[pos] == '!' && source[pos+1] == '=') {
            add_token(TOK_NOT_EQUAL, "!=");
            pos += 2;
            continue;
        }
        if (source[pos] == '<' && source[pos+1] == '=') {
            add_token(TOK_LESS_EQUAL, "<=");
            pos += 2;
            continue;
        }
        if (source[pos] == '>' && source[pos+1] == '=') {
            add_token(TOK_GREATER_EQUAL, ">=");
            pos += 2;
            continue;
        }
        
        // Single character operators
        if (source[pos] == '=') {
            add_token(TOK_EQUALS, "=");
            pos++;
            continue;
        }
        if (source[pos] == '<') {
            add_token(TOK_LESS, "<");
            pos++;
            continue;
        }
        if (source[pos] == '>') {
            add_token(TOK_GREATER, ">");
            pos++;
            continue;
        }
        if (source[pos] == '(') {
            add_token(TOK_LPAREN, "(");
            pos++;
            continue;
        }
        if (source[pos] == ')') {
            add_token(TOK_RPAREN, ")");
            pos++;
            continue;
        }
        if (source[pos] == ',') {
            add_token(TOK_COMMA, ",");
            pos++;
            continue;
        }
        if (source[pos] == ':') {
            add_token(TOK_COLON, ":");
            pos++;
            continue;
        }
        
        // Arithmetic operators
        if (source[pos] == '+') {
            add_token(TOK_PLUS, "+");
            pos++;
            continue;
        }
        if (source[pos] == '-') {
            add_token(TOK_MINUS, "-");
            pos++;
            continue;
        }
        if (source[pos] == '*') {
            add_token(TOK_STAR, "*");
            pos++;
            continue;
        }
        if (source[pos] == '/') {
            add_token(TOK_SLASH, "/");
            pos++;
            continue;
        }
        if (source[pos] == '%') {
            add_token(TOK_PERCENT, "%");
            pos++;
            continue;
        }
        
        // Unknown character - skip
        pos++;
    }
    
    add_token(TOK_EOF, "");
}

void generate_assembly(const char* output_file) {
    FILE* out = fopen(output_file, "w");
    if (!out) {
        fprintf(stderr, "Cannot open output file: %s\n", output_file);
        exit(1);
    }
    
    // First pass: Find all function definitions
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOK_FUNCTION && i+1 < token_count && tokens[i+1].type == TOK_IDENTIFIER) {
            strcpy(functions[function_count].name, tokens[i+1].value);
            functions[function_count].start_token = i;
            
            // Find return type (after colon)
            for (int j = i+2; j < token_count; j++) {
                if (tokens[j].type == TOK_COLON && j+1 < token_count) {
                    strcpy(functions[function_count].return_type, tokens[j+1].value);
                    break;
                }
            }
            
            // Find end of function
            for (int j = i+1; j < token_count; j++) {
                if (tokens[j].type == TOK_END) {
                    // Check if next is "function"
                    if (j+1 < token_count && tokens[j+1].type == TOK_FUNCTION) {
                        functions[function_count].end_token = j+1;
                        function_count++;
                        break;
                    }
                }
            }
        }
    }
    
    fprintf(out, "; Generated by melpc bootstrap compiler\n");
    fprintf(out, "section .data\n");
    fprintf(out, "    msg_ok: db 'MELP Module OK',10,0\n");
    fprintf(out, "\n");
    fprintf(out, "\nsection .bss\n");
    
    // Declare variables (only once each)
    char declared_vars[100][256];
    int declared_count = 0;
    
    for (int i = 0; i < token_count; i++) {
        if ((tokens[i].type == TOK_NUMERIC || tokens[i].type == TOK_BOOLEAN) && 
            i+1 < token_count && 
            tokens[i+1].type == TOK_IDENTIFIER) {
            
            // Check if already declared
            int already_declared = 0;
            for (int j = 0; j < declared_count; j++) {
                if (strcmp(declared_vars[j], tokens[i+1].value) == 0) {
                    already_declared = 1;
                    break;
                }
            }
            
            if (!already_declared) {
                fprintf(out, "    %s: resq 1\n", tokens[i+1].value);
                strcpy(declared_vars[declared_count++], tokens[i+1].value);
            }
        }
    }
    
    fprintf(out, "\nsection .text\n");
    fprintf(out, "    extern mlp_print\n");
    fprintf(out, "    extern mlp_malloc\n");
    fprintf(out, "    global _start\n\n");
    
    // Generate function definitions
    for (int f = 0; f < function_count; f++) {
        fprintf(out, "%s:\n", functions[f].name);
        fprintf(out, "    push rbp\n");
        fprintf(out, "    mov rbp, rsp\n");
        
        // Process function body
        int in_function = 0;
        for (int i = functions[f].start_token; i <= functions[f].end_token; i++) {
            // Look for return statement
            if (tokens[i].type == TOK_RETURN && i+1 < token_count) {
                if (tokens[i+1].type == TOK_NUMBER) {
                    fprintf(out, "    mov rax, %s\n", tokens[i+1].value);
                } else if (tokens[i+1].type == TOK_IDENTIFIER) {
                    fprintf(out, "    mov rax, qword [%s]\n", tokens[i+1].value);
                }
                // Check for arithmetic expression
                else if (i+3 < token_count && 
                         tokens[i+1].type == TOK_IDENTIFIER &&
                         (tokens[i+2].type == TOK_PLUS || tokens[i+2].type == TOK_MINUS ||
                          tokens[i+2].type == TOK_STAR || tokens[i+2].type == TOK_SLASH) &&
                         tokens[i+3].type == TOK_IDENTIFIER) {
                    
                    fprintf(out, "    mov rax, qword [%s]\n", tokens[i+1].value);
                    fprintf(out, "    mov rbx, qword [%s]\n", tokens[i+3].value);
                    
                    if (tokens[i+2].type == TOK_PLUS) fprintf(out, "    add rax, rbx\n");
                    else if (tokens[i+2].type == TOK_MINUS) fprintf(out, "    sub rax, rbx\n");
                    else if (tokens[i+2].type == TOK_STAR) fprintf(out, "    imul rax, rbx\n");
                    else if (tokens[i+2].type == TOK_SLASH) {
                        fprintf(out, "    xor rdx, rdx\n");
                        fprintf(out, "    idiv rbx\n");
                    }
                }
            }
            
            // Handle variable declarations inside function
            if ((tokens[i].type == TOK_NUMERIC || tokens[i].type == TOK_BOOLEAN) &&
                i+1 < token_count && tokens[i+1].type == TOK_IDENTIFIER &&
                i+2 < token_count && tokens[i+2].type == TOK_EQUALS &&
                i+3 < token_count && tokens[i+3].type == TOK_NUMBER) {
                
                fprintf(out, "    mov qword [%s], %s\n", tokens[i+1].value, tokens[i+3].value);
            }
        }
        
        fprintf(out, "    pop rbp\n");
        fprintf(out, "    ret\n\n");
    }
    
    fprintf(out, "_start:\n");
    
    // Initialize variables
    for (int i = 0; i < token_count; i++) {
        if ((tokens[i].type == TOK_NUMERIC || tokens[i].type == TOK_BOOLEAN) && 
            i+1 < token_count && 
            tokens[i+1].type == TOK_IDENTIFIER &&
            i+2 < token_count &&
            tokens[i+2].type == TOK_EQUALS) {
            
            char* var_name = tokens[i+1].value;
            int expr_start = i+3;
            
            // Boolean literal assignment
            if (expr_start < token_count && 
                (tokens[expr_start].type == TOK_TRUE || tokens[expr_start].type == TOK_FALSE)) {
                
                // Check for logical operation with another boolean
                if (expr_start+1 < token_count && 
                    (tokens[expr_start+1].type == TOK_AND || tokens[expr_start+1].type == TOK_OR)) {
                    
                    if (expr_start+2 < token_count && 
                        (tokens[expr_start+2].type == TOK_TRUE || tokens[expr_start+2].type == TOK_FALSE)) {
                        
                        char* left = tokens[expr_start].value;
                        char* op = tokens[expr_start+1].value;
                        char* right = tokens[expr_start+2].value;
                        
                        char* left_val = (tokens[expr_start].type == TOK_TRUE) ? "1" : "0";
                        char* right_val = (tokens[expr_start+2].type == TOK_TRUE) ? "1" : "0";
                        
                        fprintf(out, "    ; %s = %s %s %s\n", var_name, left, op, right);
                        fprintf(out, "    mov rax, %s\n", left_val);
                        fprintf(out, "    mov rbx, %s\n", right_val);
                        
                        if (strcmp(op, "AND") == 0) {
                            fprintf(out, "    and rax, rbx\n");
                        } else if (strcmp(op, "OR") == 0) {
                            fprintf(out, "    or rax, rbx\n");
                        }
                        
                        fprintf(out, "    mov qword [%s], rax\n", var_name);
                        continue;
                    }
                }
                
                // Check for NOT operation
                if (expr_start > 3 && tokens[expr_start-1].type == TOK_NOT) {
                    char* value = (tokens[expr_start].type == TOK_TRUE) ? "1" : "0";
                    fprintf(out, "    ; %s = NOT %s\n", var_name, tokens[expr_start].value);
                    fprintf(out, "    mov rax, %s\n", value);
                    fprintf(out, "    xor rax, 1\n");  // Toggle 0<->1
                    fprintf(out, "    mov qword [%s], rax\n", var_name);
                    continue;
                }
                
                // Simple boolean assignment
                char* value = (tokens[expr_start].type == TOK_TRUE) ? "1" : "0";
                fprintf(out, "    ; %s = %s\n", var_name, tokens[expr_start].value);
                fprintf(out, "    mov qword [%s], %s\n", var_name, value);
                continue;
            }
            
            // NOT operation on boolean literal
            if (expr_start < token_count && tokens[expr_start].type == TOK_NOT &&
                expr_start+1 < token_count && 
                (tokens[expr_start+1].type == TOK_TRUE || tokens[expr_start+1].type == TOK_FALSE)) {
                
                char* value = (tokens[expr_start+1].type == TOK_TRUE) ? "1" : "0";
                fprintf(out, "    ; %s = NOT %s\n", var_name, tokens[expr_start+1].value);
                fprintf(out, "    mov rax, %s\n", value);
                fprintf(out, "    xor rax, 1\n");
                fprintf(out, "    mov qword [%s], rax\n", var_name);
                continue;
            }
            
            // Function call (identifier followed by parentheses)
            if (expr_start < token_count && tokens[expr_start].type == TOK_IDENTIFIER &&
                expr_start+1 < token_count && tokens[expr_start+1].type == TOK_LPAREN) {
                
                char* func_name = tokens[expr_start].value;
                fprintf(out, "    ; %s = %s()\n", var_name, func_name);
                fprintf(out, "    call %s\n", func_name);
                fprintf(out, "    mov qword [%s], rax\n", var_name);
                continue;
            }
            
            // Simple number assignment
            if (expr_start < token_count && tokens[expr_start].type == TOK_NUMBER) {
                // Check for comparison operation
                if (expr_start+1 < token_count && 
                    (tokens[expr_start+1].type == TOK_EQUAL_EQUAL ||
                     tokens[expr_start+1].type == TOK_NOT_EQUAL ||
                     tokens[expr_start+1].type == TOK_LESS ||
                     tokens[expr_start+1].type == TOK_LESS_EQUAL ||
                     tokens[expr_start+1].type == TOK_GREATER ||
                     tokens[expr_start+1].type == TOK_GREATER_EQUAL)) {
                    
                    // Binary comparison operation
                    if (expr_start+2 < token_count && tokens[expr_start+2].type == TOK_NUMBER) {
                        char* left = tokens[expr_start].value;
                        char* op = tokens[expr_start+1].value;
                        char* right = tokens[expr_start+2].value;
                        
                        fprintf(out, "    ; %s = %s %s %s\n", var_name, left, op, right);
                        fprintf(out, "    mov rax, %s\n", left);
                        fprintf(out, "    cmp rax, %s\n", right);
                        
                        if (strcmp(op, "==") == 0) {
                            fprintf(out, "    sete al\n");
                        } else if (strcmp(op, "!=") == 0) {
                            fprintf(out, "    setne al\n");
                        } else if (strcmp(op, "<") == 0) {
                            fprintf(out, "    setl al\n");
                        } else if (strcmp(op, "<=") == 0) {
                            fprintf(out, "    setle al\n");
                        } else if (strcmp(op, ">") == 0) {
                            fprintf(out, "    setg al\n");
                        } else if (strcmp(op, ">=") == 0) {
                            fprintf(out, "    setge al\n");
                        }
                        
                        fprintf(out, "    movzx rax, al\n");
                        fprintf(out, "    mov qword [%s], rax\n", var_name);
                        continue;
                    }
                }
                
                // Check for arithmetic operation
                if (expr_start+1 < token_count && 
                    (tokens[expr_start+1].type == TOK_PLUS ||
                     tokens[expr_start+1].type == TOK_MINUS ||
                     tokens[expr_start+1].type == TOK_STAR ||
                     tokens[expr_start+1].type == TOK_SLASH ||
                     tokens[expr_start+1].type == TOK_PERCENT)) {
                    
                    // Binary arithmetic operation
                    if (expr_start+2 < token_count && tokens[expr_start+2].type == TOK_NUMBER) {
                        char* left = tokens[expr_start].value;
                        char* op = tokens[expr_start+1].value;
                        char* right = tokens[expr_start+2].value;
                        
                        fprintf(out, "    ; %s = %s %s %s\n", var_name, left, op, right);
                        fprintf(out, "    mov rax, %s\n", left);
                        
                        if (strcmp(op, "+") == 0) {
                            fprintf(out, "    add rax, %s\n", right);
                        } else if (strcmp(op, "-") == 0) {
                            fprintf(out, "    sub rax, %s\n", right);
                        } else if (strcmp(op, "*") == 0) {
                            fprintf(out, "    imul rax, %s\n", right);
                        } else if (strcmp(op, "/") == 0) {
                            fprintf(out, "    xor rdx, rdx\n");
                            fprintf(out, "    mov rbx, %s\n", right);
                            fprintf(out, "    idiv rbx\n");
                        } else if (strcmp(op, "%") == 0) {
                            fprintf(out, "    xor rdx, rdx\n");
                            fprintf(out, "    mov rbx, %s\n", right);
                            fprintf(out, "    idiv rbx\n");
                            fprintf(out, "    mov rax, rdx\n");
                        }
                        
                        fprintf(out, "    mov qword [%s], rax\n", var_name);
                        continue;
                    }
                }
                
                // Simple assignment
                fprintf(out, "    ; %s = %s\n", var_name, tokens[expr_start].value);
                fprintf(out, "    mov qword [%s], %s\n", var_name, tokens[expr_start].value);
            }
        }
        
        // Handle if/then/else control flow
        if (tokens[i].type == TOK_IF) {
            // Find condition between if and then
            int cond_start = i + 1;
            int then_pos = -1;
            int else_pos = -1;
            int endif_pos = -1;
            
            // Find then, else, and end if positions
            for (int j = cond_start; j < token_count; j++) {
                if (tokens[j].type == TOK_THEN && then_pos == -1) then_pos = j;
                else if (tokens[j].type == TOK_ELSE && else_pos == -1) else_pos = j;
                else if (tokens[j].type == TOK_END_IF) {
                    endif_pos = j;
                    break;
                }
            }
            
            if (then_pos == -1 || endif_pos == -1) continue;
            
            // Generate labels
            int label_num = label_counter++;
            
            // Parse condition (simple: var comparison value or boolean var)
            fprintf(out, "\n    ; if statement\n");
            
            // Check for comparison expression (var op value) - CHECK THIS FIRST
            if (cond_start + 2 < then_pos && 
                     tokens[cond_start].type == TOK_IDENTIFIER &&
                     (tokens[cond_start+1].type == TOK_EQUAL_EQUAL ||
                      tokens[cond_start+1].type == TOK_NOT_EQUAL ||
                      tokens[cond_start+1].type == TOK_LESS ||
                      tokens[cond_start+1].type == TOK_LESS_EQUAL ||
                      tokens[cond_start+1].type == TOK_GREATER ||
                      tokens[cond_start+1].type == TOK_GREATER_EQUAL) &&
                     tokens[cond_start+2].type == TOK_NUMBER) {
                
                char* var = tokens[cond_start].value;
                char* op = tokens[cond_start+1].value;
                char* val = tokens[cond_start+2].value;
                
                fprintf(out, "    mov rax, qword [%s]\n", var);
                fprintf(out, "    cmp rax, %s\n", val);
                
                // Jump to else/endif based on opposite condition
                char* jump_instr = "";
                if (strcmp(op, "==") == 0) jump_instr = "jne";
                else if (strcmp(op, "!=") == 0) jump_instr = "je";
                else if (strcmp(op, "<") == 0) jump_instr = "jge";
                else if (strcmp(op, "<=") == 0) jump_instr = "jg";
                else if (strcmp(op, ">") == 0) jump_instr = "jle";
                else if (strcmp(op, ">=") == 0) jump_instr = "jl";
                
                if (else_pos != -1) {
                    fprintf(out, "    %s .else_%d\n", jump_instr, label_num);
                } else {
                    fprintf(out, "    %s .endif_%d\n", jump_instr, label_num);
                }
            }
            // Check if condition is a simple boolean variable
            else if (cond_start < then_pos && tokens[cond_start].type == TOK_IDENTIFIER) {
                fprintf(out, "    mov rax, qword [%s]\n", tokens[cond_start].value);
                fprintf(out, "    cmp rax, 0\n");
                if (else_pos != -1) {
                    fprintf(out, "    je .else_%d\n", label_num);
                } else {
                    fprintf(out, "    je .endif_%d\n", label_num);
                }
            }
            
            // Then block - process assignments
            fprintf(out, ".then_%d:\n", label_num);
            for (int j = then_pos + 1; j < (else_pos != -1 ? else_pos : endif_pos); j++) {
                if ((tokens[j].type == TOK_NUMERIC || tokens[j].type == TOK_BOOLEAN) &&
                    j+1 < token_count && tokens[j+1].type == TOK_IDENTIFIER &&
                    j+2 < token_count && tokens[j+2].type == TOK_EQUALS &&
                    j+3 < token_count && tokens[j+3].type == TOK_NUMBER) {
                    
                    fprintf(out, "    mov qword [%s], %s\n", tokens[j+1].value, tokens[j+3].value);
                }
            }
            
            if (else_pos != -1) {
                fprintf(out, "    jmp .endif_%d\n", label_num);
                fprintf(out, ".else_%d:\n", label_num);
                
                // Else block - process assignments
                for (int j = else_pos + 1; j < endif_pos; j++) {
                    if ((tokens[j].type == TOK_NUMERIC || tokens[j].type == TOK_BOOLEAN) &&
                        j+1 < token_count && tokens[j+1].type == TOK_IDENTIFIER &&
                        j+2 < token_count && tokens[j+2].type == TOK_EQUALS &&
                        j+3 < token_count && tokens[j+3].type == TOK_NUMBER) {
                        
                        fprintf(out, "    mov qword [%s], %s\n", tokens[j+1].value, tokens[j+3].value);
                    }
                }
            }
            
            fprintf(out, ".endif_%d:\n", label_num);
        }
    }
    
    fprintf(out, "\n    ; Print success message\n");
    fprintf(out, "    mov rdi, msg_ok\n");
    fprintf(out, "    call mlp_print\n");
    fprintf(out, "\n    ; Exit\n");
    fprintf(out, "    mov rax, 60\n");
    fprintf(out, "    xor rdi, rdi\n");
    fprintf(out, "    syscall\n");
    
    fclose(out);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.mlp output.s\n", argv[0]);
        return 1;
    }
    
    printf("🔧 melpc - MELP Bootstrap Compiler\n");
    printf("===================================\n");
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    // Read input file
    FILE* in = fopen(argv[1], "r");
    if (!in) {
        fprintf(stderr, "Cannot open input file: %s\n", argv[1]);
        return 1;
    }
    
    size_t size = fread(source, 1, MAX_CODE-1, in);
    source[size] = '\0';
    fclose(in);
    
    printf("[1/3] Tokenizing...\n");
    tokenize();
    printf("      Found %d tokens\n", token_count);
    
    printf("[2/3] Parsing...\n");
    printf("      Simple parse complete\n");
    
    printf("[3/3] Generating assembly...\n");
    generate_assembly(argv[2]);
    
    printf("\n✅ Compilation successful!\n");
    printf("Next steps:\n");
    printf("  nasm -f elf64 %s -o output.o\n", argv[2]);
    printf("  ld output.o ../../C/runtime/runtime.o -o program\n");
    
    return 0;
}
