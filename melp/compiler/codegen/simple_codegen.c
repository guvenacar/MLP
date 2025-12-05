#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>

// Simple MELP to x86-64 codegen - Proof of concept
// Compiles very simple MELP programs to working x86-64 assembly

// Global debug flag
static bool g_debug = false;

typedef struct {
    char *source;
    int pos;
    int line;
    char current;
} Lexer;

typedef struct {
    char *name;
    int offset;  // Stack offset from %rbp
} Variable;

typedef struct {
    FILE *out;
    int label_count;
    int string_count;
    char *strings[100];  // Store strings for rodata section
    Variable vars[100];  // Local variables
    int var_count;
    int stack_offset;    // Current stack offset
} Codegen;

// ===== LEXER FUNCTIONS =====

static void lexer_init(Lexer *lex, char *source) {
    lex->source = source;
    lex->pos = 0;
    lex->line = 1;
    lex->current = source[0];
}

static void lexer_advance(Lexer *lex) {
    if (lex->current == '\n') lex->line++;
    lex->current = lex->source[++lex->pos];
}

static void lexer_skip_whitespace(Lexer *lex) {
    while (lex->current == ' ' || lex->current == '\t' || 
           lex->current == '\n' || lex->current == '\r') {
        lexer_advance(lex);
    }
}

static void lexer_skip_comment(Lexer *lex) {
    // MELP comment: -- single line
    if (lex->current == '-' && lex->source[lex->pos + 1] == '-') {
        while (lex->current != '\n' && lex->current != '\0') {
            lexer_advance(lex);
        }
    }
    // Legacy C-style comment: // single line
    else if (lex->current == '/' && lex->source[lex->pos + 1] == '/') {
        while (lex->current != '\n' && lex->current != '\0') {
            lexer_advance(lex);
        }
    }
}

static bool lexer_match(Lexer *lex, const char *keyword) {
    int len = strlen(keyword);
    if (strncmp(&lex->source[lex->pos], keyword, len) == 0) {
        if (!isalnum(lex->source[lex->pos + len]) && lex->source[lex->pos + len] != '_') {
            for (int i = 0; i < len; i++) lexer_advance(lex);
            return true;
        }
    }
    return false;
}

static char *lexer_read_identifier(Lexer *lex) {
    int start = lex->pos;
    while (isalnum(lex->current) || lex->current == '_') {
        lexer_advance(lex);
    }
    int len = lex->pos - start;
    char *id = malloc(len + 1);
    strncpy(id, &lex->source[start], len);
    id[len] = '\0';
    return id;
}

static char *lexer_read_string(Lexer *lex) {
    lexer_advance(lex); // Skip opening "
    int start = lex->pos;
    while (lex->current != '"' && lex->current != '\0') {
        if (lex->current == '\\') lexer_advance(lex);
        lexer_advance(lex);
    }
    int len = lex->pos - start;
    char *str = malloc(len + 1);
    strncpy(str, &lex->source[start], len);
    str[len] = '\0';
    lexer_advance(lex); // Skip closing "
    return str;
}

static int lexer_read_number(Lexer *lex) {
    int value = 0;
    while (isdigit(lex->current)) {
        value = value * 10 + (lex->current - '0');
        lexer_advance(lex);
    }
    return value;
}

// ===== CODEGEN FUNCTIONS =====

static void emit(Codegen *gen, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(gen->out, format, args);
    va_end(args);
    fprintf(gen->out, "\n");
}

static void emit_string_literal(Codegen *gen, const char *str) {
    emit(gen, ".str%d:", gen->string_count);
    emit(gen, "    .string \"%s\"", str);
    gen->string_count++;
}

// Variable management
static int find_variable(Codegen *gen, const char *name) {
    for (int i = 0; i < gen->var_count; i++) {
        if (strcmp(gen->vars[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static void add_variable(Codegen *gen, const char *name) {
    gen->stack_offset += 8;  // 8 bytes for int64
    gen->vars[gen->var_count].name = strdup(name);
    gen->vars[gen->var_count].offset = gen->stack_offset;
    gen->var_count++;
}

// Forward declarations
static void compile_expression(Codegen *gen, Lexer *lex);
static void compile_statement(Codegen *gen, Lexer *lex);
static void compile_block(Codegen *gen, Lexer *lex);

// Compile primary expression (literal or identifier)
static void compile_primary(Codegen *gen, Lexer *lex) {
    lexer_skip_whitespace(lex);
    
    if (isdigit(lex->current)) {
        int value = lexer_read_number(lex);
        emit(gen, "    movq $%d, %%rax", value);
    }
    else if (isalpha(lex->current) || lex->current == '_') {
        char *id = lexer_read_identifier(lex);
        
        lexer_skip_whitespace(lex);
        
        // Check if it's a function call
        if (lex->current == '(') {
            // Function call: id(arg1, arg2, ...)
            const char *param_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
            int arg_count = 0;
            
            lexer_advance(lex);  // Skip (
            lexer_skip_whitespace(lex);
            
            // Parse arguments and place in registers
            while (lex->current != ')' && lex->current != '\0') {
                // Compile argument expression
                compile_expression(gen, lex);
                
                // Move result to parameter register
                if (arg_count < 6) {
                    emit(gen, "    movq %%rax, %%%s  # arg %d", param_regs[arg_count], arg_count);
                }
                
                arg_count++;
                
                lexer_skip_whitespace(lex);
                if (lex->current == ',') {
                    lexer_advance(lex);
                    lexer_skip_whitespace(lex);
                }
            }
            
            if (lex->current == ')') lexer_advance(lex);
            
            // Call function
            emit(gen, "    call %s", id);
            // Result is in RAX
        } else {
            // Variable load
            int var_idx = find_variable(gen, id);
            if (var_idx >= 0) {
                emit(gen, "    movq -%d(%%rbp), %%rax  # load %s", 
                     gen->vars[var_idx].offset, id);
            } else {
                emit(gen, "    # Unknown variable: %s", id);
                emit(gen, "    movq $0, %%rax");
            }
        }
        free(id);
    }
}

// Compile binary expression (e.g., x + y)
static void compile_expression(Codegen *gen, Lexer *lex) {
    compile_primary(gen, lex);
    
    lexer_skip_whitespace(lex);
    
    // Check for comparison operators (two chars)
    if (lex->current == '>' || lex->current == '<' || lex->current == '=' || lex->current == '!') {
        char op1 = lex->current;
        char op2 = lex->source[lex->pos + 1];
        
        if (op1 == '>' && op2 == '=') {
            lexer_advance(lex); lexer_advance(lex);
            emit(gen, "    pushq %%rax  # save left");
            compile_primary(gen, lex);
            emit(gen, "    movq %%rax, %%rbx");
            emit(gen, "    popq %%rax");
            emit(gen, "    cmpq %%rbx, %%rax  # compare");
            emit(gen, "    setge %%al  # >=");
            emit(gen, "    movzbq %%al, %%rax  # zero extend");
        }
        else if (op1 == '<' && op2 == '=') {
            lexer_advance(lex); lexer_advance(lex);
            emit(gen, "    pushq %%rax");
            compile_primary(gen, lex);
            emit(gen, "    movq %%rax, %%rbx");
            emit(gen, "    popq %%rax");
            emit(gen, "    cmpq %%rbx, %%rax");
            emit(gen, "    setle %%al  # <=");
            emit(gen, "    movzbq %%al, %%rax");
        }
        else if (op1 == '=' && op2 == '=') {
            lexer_advance(lex); lexer_advance(lex);
            emit(gen, "    pushq %%rax");
            compile_primary(gen, lex);
            emit(gen, "    movq %%rax, %%rbx");
            emit(gen, "    popq %%rax");
            emit(gen, "    cmpq %%rbx, %%rax");
            emit(gen, "    sete %%al  # ==");
            emit(gen, "    movzbq %%al, %%rax");
        }
        else if (op1 == '!' && op2 == '=') {
            lexer_advance(lex); lexer_advance(lex);
            emit(gen, "    pushq %%rax");
            compile_primary(gen, lex);
            emit(gen, "    movq %%rax, %%rbx");
            emit(gen, "    popq %%rax");
            emit(gen, "    cmpq %%rbx, %%rax");
            emit(gen, "    setne %%al  # !=");
            emit(gen, "    movzbq %%al, %%rax");
        }
        else if (op1 == '>') {
            lexer_advance(lex);
            emit(gen, "    pushq %%rax");
            compile_primary(gen, lex);
            emit(gen, "    movq %%rax, %%rbx");
            emit(gen, "    popq %%rax");
            emit(gen, "    cmpq %%rbx, %%rax");
            emit(gen, "    setg %%al  # >");
            emit(gen, "    movzbq %%al, %%rax");
        }
        else if (op1 == '<') {
            lexer_advance(lex);
            emit(gen, "    pushq %%rax");
            compile_primary(gen, lex);
            emit(gen, "    movq %%rax, %%rbx");
            emit(gen, "    popq %%rax");
            emit(gen, "    cmpq %%rbx, %%rax");
            emit(gen, "    setl %%al  # <");
            emit(gen, "    movzbq %%al, %%rax");
        }
        return;
    }
    
    // Arithmetic operators
    char op = lex->current;
    
    if (op == '+' || op == '-' || op == '*' || op == '/') {
        lexer_advance(lex);
        emit(gen, "    pushq %%rax  # save left operand");
        compile_primary(gen, lex);
        emit(gen, "    movq %%rax, %%rbx  # right operand in rbx");
        emit(gen, "    popq %%rax  # restore left operand");
        
        switch (op) {
            case '+': emit(gen, "    addq %%rbx, %%rax"); break;
            case '-': emit(gen, "    subq %%rbx, %%rax"); break;
            case '*': emit(gen, "    imulq %%rbx, %%rax"); break;
            case '/': 
                emit(gen, "    cqto  # sign extend rax to rdx:rax");
                emit(gen, "    idivq %%rbx");
                break;
        }
    }
}

static void compile_print(Codegen *gen, Lexer *lex) {
    // Called after "print" or "print_int" keyword consumed
    // print("string") -> puts call
    // print_int(expr) -> printf call
    lexer_skip_whitespace(lex);
    
    if (lex->current != '(') {
        fprintf(stderr, "Error: Expected '(' after print\n");
        exit(1);
    }
    lexer_advance(lex); // Skip (
    lexer_skip_whitespace(lex);
    
    if (lex->current == '"') {
        // print("string")
        char *str = lexer_read_string(lex);
        int str_id = gen->string_count;
        gen->strings[gen->string_count++] = strdup(str);
        emit(gen, "    # print(\"%s\")", str);
        emit(gen, "    leaq .str%d(%%rip), %%rdi", str_id);
        emit(gen, "    call puts@PLT");
        free(str);
    }
    else {
        // print_int(expr)
        compile_expression(gen, lex);
        emit(gen, "    # print_int(...)");
        emit(gen, "    movq %%rax, %%rsi  # value to print");
        int fmt_id = gen->string_count;
        gen->strings[gen->string_count++] = strdup("%ld\\n");
        emit(gen, "    leaq .str%d(%%rip), %%rdi", fmt_id);
        emit(gen, "    xorq %%rax, %%rax  # no floating point");
        emit(gen, "    call printf@PLT");
    }
    
    lexer_skip_whitespace(lex);
    if (lex->current != ')') {
        fprintf(stderr, "Error: Expected ')' after print argument\n");
        exit(1);
    }
    lexer_advance(lex); // Skip )
}

static void compile_return(Codegen *gen, Lexer *lex) {
    // return expr -> evaluate expr, movq result to %rax, leave, ret
    lexer_skip_whitespace(lex);
    
    if (lex->current != ';' && lex->current != '}' && lex->current != '\0') {
        // Compile return expression
        compile_expression(gen, lex);
        // Result is already in RAX
        emit(gen, "    # return expression");
    } else {
        // return without value (return 0)
        emit(gen, "    # return (no value)");
        emit(gen, "    movq $0, %%rax");
    }
    
    emit(gen, "    leave");
    emit(gen, "    ret");
}

// Compile a block of statements { ... }
static void compile_block(Codegen *gen, Lexer *lex) {
    lexer_skip_whitespace(lex);
    if (lex->current == '{') {
        lexer_advance(lex);
    }
    
    while (lex->current != '}' && lex->current != '\0') {
        compile_statement(gen, lex);
        lexer_skip_whitespace(lex);
    }
    
    if (lex->current == '}') {
        lexer_advance(lex);
    }
}

// Compile if statement
static void compile_if(Codegen *gen, Lexer *lex) {
    lexer_skip_whitespace(lex);
    
    // Skip (
    if (lex->current == '(') lexer_advance(lex);
    
    // Compile condition
    compile_expression(gen, lex);
    
    // Skip )
    lexer_skip_whitespace(lex);
    if (lex->current == ')') lexer_advance(lex);
    
    // Generate labels
    char *else_label = malloc(32);
    char *end_label = malloc(32);
    snprintf(else_label, 32, ".Lelse%d", gen->label_count++);
    snprintf(end_label, 32, ".Lend%d", gen->label_count++);
    
    // Jump if false (rax == 0)
    emit(gen, "    testq %%rax, %%rax");
    emit(gen, "    jz %s  # if false, jump to else", else_label);
    
    // Then block
    compile_block(gen, lex);
    emit(gen, "    jmp %s  # skip else", end_label);
    
    // Else block
    emit(gen, "%s:", else_label);
    lexer_skip_whitespace(lex);
    if (lexer_match(lex, "else")) {
        compile_block(gen, lex);
    }
    
    emit(gen, "%s:", end_label);
    free(else_label);
    free(end_label);
}

// Compile while loop
static void compile_while(Codegen *gen, Lexer *lex) {
    lexer_skip_whitespace(lex);
    
    // Skip (
    if (lex->current == '(') lexer_advance(lex);
    
    // Generate labels
    char *loop_label = malloc(32);
    char *end_label = malloc(32);
    snprintf(loop_label, 32, ".Lloop%d", gen->label_count++);
    snprintf(end_label, 32, ".Lend%d", gen->label_count++);
    
    emit(gen, "%s:  # while loop start", loop_label);
    
    // Compile condition
    compile_expression(gen, lex);
    
    // Skip )
    lexer_skip_whitespace(lex);
    if (lex->current == ')') lexer_advance(lex);
    
    // Jump if false
    emit(gen, "    testq %%rax, %%rax");
    emit(gen, "    jz %s  # exit loop if false", end_label);
    
    // Loop body
    compile_block(gen, lex);
    emit(gen, "    jmp %s  # loop back", loop_label);
    
    emit(gen, "%s:  # while loop end", end_label);
    free(loop_label);
    free(end_label);
}

static void compile_statement(Codegen *gen, Lexer *lex) {
    lexer_skip_whitespace(lex);
    lexer_skip_comment(lex);
    lexer_skip_whitespace(lex);
    
    // Safety: if at end of block, return
    if (lex->current == '}' || lex->current == '\0') {
        return;
    }
    
    // Check for print_int first (longer match)
    if (strncmp(&lex->source[lex->pos], "print_int", 9) == 0 &&
        !isalnum(lex->source[lex->pos + 9]) && lex->source[lex->pos + 9] != '_') {
        for (int i = 0; i < 9; i++) lexer_advance(lex);
        compile_print(gen, lex);
    }
    else if (lexer_match(lex, "if")) {
        compile_if(gen, lex);
        return; // if handles its own semicolons
    }
    else if (lexer_match(lex, "while")) {
        compile_while(gen, lex);
        return; // while handles its own semicolons
    }
    else if (lexer_match(lex, "numeric") || lexer_match(lex, "string") || 
             lexer_match(lex, "boolean") || lexer_match(lex, "let")) {
        // MELP: numeric var = expr; or let var = expr (legacy)
        lexer_skip_whitespace(lex);
        char *var_name = lexer_read_identifier(lex);
        add_variable(gen, var_name);
        
        lexer_skip_whitespace(lex);
        if (lex->current == '=') {
            lexer_advance(lex);
            compile_expression(gen, lex);
            int var_idx = find_variable(gen, var_name);
            emit(gen, "    movq %%rax, -%d(%%rbp)  # store %s", 
                 gen->vars[var_idx].offset, var_name);
        }
        free(var_name);
    }
    else if (lexer_match(lex, "print")) {
        compile_print(gen, lex);
    } 
    else if (lexer_match(lex, "return")) {
        compile_return(gen, lex);
    }
    else if (isalpha(lex->current) || lex->current == '_') {
        // Assignment: var = expr;
        char *var_name = lexer_read_identifier(lex);
        lexer_skip_whitespace(lex);
        if (lex->current == '=') {
            lexer_advance(lex);
            compile_expression(gen, lex);
            int var_idx = find_variable(gen, var_name);
            if (var_idx >= 0) {
                emit(gen, "    movq %%rax, -%d(%%rbp)  # update %s", 
                     gen->vars[var_idx].offset, var_name);
            }
        }
        free(var_name);
    }
    else {
        // Unknown token - skip to avoid infinite loop
        if (lex->current != '\0' && lex->current != ';' && lex->current != '}') {
            lexer_advance(lex);
        }
    }
    
    // Skip semicolon
    lexer_skip_whitespace(lex);
    if (lex->current == ';') lexer_advance(lex);
}

static void compile_function(Codegen *gen, Lexer *lex) {
    // fn name(param1, param2, ...) { ... }
    lexer_skip_whitespace(lex);
    char *name = lexer_read_identifier(lex);
    
    if (g_debug) {
        fprintf(stderr, "[DEBUG] Compiling function: %s (line %d)\n", name, lex->line);
    }
    
    // Reset function-local state
    gen->var_count = 0;
    gen->stack_offset = 0;
    
    emit(gen, "");
    emit(gen, "# Function: %s", name);
    emit(gen, ".globl %s", name);
    emit(gen, ".type %s, @function", name);
    emit(gen, "%s:", name);
    emit(gen, "    pushq %%rbp");
    emit(gen, "    movq %%rsp, %%rbp");
    
    // Parse parameters and store them on stack
    // System V ABI: first 6 params in RDI, RSI, RDX, RCX, R8, R9
    const char *param_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
    int param_count = 0;
    
    lexer_skip_whitespace(lex);
    if (lex->current == '(') {
        lexer_advance(lex);
        lexer_skip_whitespace(lex);
        
        while (lex->current != ')' && lex->current != '\0') {
            if (isalpha(lex->current) || lex->current == '_') {
                char *param_name = lexer_read_identifier(lex);
                
                // Skip type annotation if present (: i32)
                lexer_skip_whitespace(lex);
                if (lex->current == ':') {
                    lexer_advance(lex);
                    while (isalnum(lex->current) || lex->current == '_') {
                        lexer_advance(lex);
                    }
                }
                
                // Add parameter as variable
                add_variable(gen, param_name);
                
                // Move from parameter register to stack
                if (param_count < 6) {
                    emit(gen, "    movq %%%s, -%d(%%rbp)  # param %s", 
                         param_regs[param_count], gen->vars[gen->var_count - 1].offset, param_name);
                }
                
                free(param_name);
                param_count++;
            }
            
            lexer_skip_whitespace(lex);
            if (lex->current == ',') {
                lexer_advance(lex);
                lexer_skip_whitespace(lex);
            }
        }
        
        if (lex->current == ')') lexer_advance(lex);
    }
    
    // Skip return type if present (-> type)
    lexer_skip_whitespace(lex);
    if (lex->current == '-' && lex->source[lex->pos + 1] == '>') {
        lexer_advance(lex);
        lexer_advance(lex);
        while (isalnum(lex->current) || lex->current == '_') {
            lexer_advance(lex);
        }
    }
    
    lexer_skip_whitespace(lex);
    if (lex->current == '{') lexer_advance(lex);
    
    // Allocate fixed stack space (simplified: 128 bytes)
    emit(gen, "    subq $128, %%rsp  # allocate stack for locals");
    
    // Compile body
    while (lex->current != '}' && lex->current != '\0') {
        compile_statement(gen, lex);
        lexer_skip_whitespace(lex);
    }
    
    if (lex->current == '}') lexer_advance(lex);
    
    emit(gen, "    # end of %s", name);
    emit(gen, ".size %s, .-%s", name, name);
    free(name);
}

static void compile_program(Codegen *gen, Lexer *lex) {
    // Assembly header
    emit(gen, "# Generated by MELP Simple Codegen");
    
    // Initialize strings array
    for (int i = 0; i < 100; i++) gen->strings[i] = NULL;
    
    // First pass: compile functions (collects strings)
    emit(gen, ".text");
    
    while (lex->current != '\0') {
        lexer_skip_whitespace(lex);
        lexer_skip_comment(lex);
        lexer_skip_whitespace(lex);
        
        if (lexer_match(lex, "fn")) {
            compile_function(gen, lex);
        } else if (lex->current != '\0') {
            lexer_advance(lex);
        }
    }
    
    // Second pass: emit strings in rodata section
    emit(gen, "");
    emit(gen, ".section .rodata");
    for (int i = 0; i < gen->string_count; i++) {
        if (gen->strings[i]) {
            emit(gen, ".str%d:", i);
            emit(gen, "    .string \"%s\"", gen->strings[i]);
        }
    }
    
    emit(gen, "");
    emit(gen, ".section .note.GNU-stack,\"\",@progbits");
}

// ===== MAIN =====

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    // Check for --debug flag
    if (argc > 3 && strcmp(argv[3], "--debug") == 0) {
        g_debug = true;
        fprintf(stderr, "[DEBUG] Debug mode enabled\n");
    }
    
    // Read input file
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    if (g_debug) {
        fprintf(stderr, "[DEBUG] Read %ld bytes from %s\n", size, argv[1]);
    }
    
    // Open output file
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(source);
        return 1;
    }
    
    // Compile
    Lexer lex;
    Codegen gen;
    lexer_init(&lex, source);
    gen.out = out;
    gen.label_count = 0;
    gen.string_count = 0;
    
    if (g_debug) {
        fprintf(stderr, "[DEBUG] Starting compilation...\n");
    }
    
    compile_program(&gen, &lex);
    
    fclose(out);
    free(source);
    
    printf("✓ Compiled: %s -> %s\n", argv[1], argv[2]);
    return 0;
}
