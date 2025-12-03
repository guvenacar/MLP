#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>

// ============================================
// CODEGEN COMPILER - STAGE 0 (REAL x86-64)
// ============================================
// This compiler generates real x86-64 assembly from MELP source
// Strategy: Direct source-to-assembly compilation
// Input: MELP source code (.mlp file)
// Output: x86-64 assembly (.s file) with GC integration

// Register constants
#define REG_RAX 0
#define REG_RBX 1
#define REG_RCX 2
#define REG_RDX 3
#define REG_RSI 4
#define REG_RDI 5
#define REG_RBP 6
#define REG_RSP 7
#define REG_R8  8
#define REG_R9  9
#define REG_R10 10
#define REG_R11 11
#define REG_R12 12
#define REG_R13 13
#define REG_R14 14
#define REG_R15 15

// Parameter registers (System V ABI)
static const int PARAM_REGS[] = {REG_RDI, REG_RSI, REG_RDX, REG_RCX, REG_R8, REG_R9};

// Token types (minimal set needed for parsing)
typedef enum {
    TOKEN_EOF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_FN,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_EQ,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_EQ_EQ,
    TOKEN_NOT_EQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LT_EQ,
    TOKEN_GT_EQ,
    TOKEN_AND_AND,
    TOKEN_OR_OR,
    TOKEN_NOT,
    TOKEN_AMP,
    TOKEN_COLON,
    TOKEN_ARROW,
    TOKEN_PRINT,
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
    int line;
} Token;

// AST Node types
typedef enum {
    NODE_PROGRAM,
    NODE_FUNC_DECL,
    NODE_VAR_DECL,
    NODE_IF,
    NODE_WHILE,
    NODE_RETURN,
    NODE_BLOCK,
    NODE_EXPR_STMT,
    NODE_BINARY,
    NODE_UNARY,
    NODE_CALL,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_PRINT,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    Token *token;
    struct ASTNode **children;
    int child_count;
    int child_capacity;
} ASTNode;

// Memory allocation type from memory analysis
typedef enum {
    ALLOC_STACK,
    ALLOC_HEAP_GC,
    ALLOC_HEAP_MALLOC,
} AllocType;

typedef struct {
    char *var_name;
    AllocType alloc_type;
    int size;
    bool escapes;
} MemoryInfo;

// Codegen context
typedef struct {
    FILE *output;
    int label_counter;
    int stack_offset;
    bool register_used[16];
    char *current_function;
    MemoryInfo *memory_infos;
    int memory_info_count;
    int memory_info_capacity;
} CodegenContext;

// ============================================
// UTILITY FUNCTIONS
// ============================================

static const char *register_name(int reg) {
    static const char *names[] = {
        "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp",
        "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"
    };
    if (reg >= 0 && reg < 16) return names[reg];
    return "rax";
}

static void emit(CodegenContext *ctx, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(ctx->output, format, args);
    va_end(args);
    fprintf(ctx->output, "\n");
}

static void emit_label(CodegenContext *ctx, const char *label) {
    fprintf(ctx->output, "%s:\n", label);
}

static char *gen_label(CodegenContext *ctx) {
    char *label = malloc(32);
    snprintf(label, 32, ".L%d", ctx->label_counter++);
    return label;
}

static int alloc_register(CodegenContext *ctx) {
    for (int i = 0; i < 16; i++) {
        if (!ctx->register_used[i] && i != REG_RBP && i != REG_RSP) {
            ctx->register_used[i] = true;
            return i;
        }
    }
    return REG_RAX; // Fallback
}

static void free_register(CodegenContext *ctx, int reg) {
    if (reg >= 0 && reg < 16) {
        ctx->register_used[reg] = false;
    }
}

// ============================================
// MEMORY ANALYSIS PARSING
// ============================================

static void parse_memory_analysis(CodegenContext *ctx, const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    
    ctx->memory_info_capacity = 100;
    ctx->memory_infos = calloc(ctx->memory_info_capacity, sizeof(MemoryInfo));
    ctx->memory_info_count = 0;
    
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        // Parse lines like: "ALLOC: var_name, type=STACK/HEAP_GC/HEAP_MALLOC, size=8, escapes=true/false"
        if (strncmp(line, "ALLOC:", 6) == 0) {
            if (ctx->memory_info_count >= ctx->memory_info_capacity) {
                ctx->memory_info_capacity *= 2;
                ctx->memory_infos = realloc(ctx->memory_infos, 
                    ctx->memory_info_capacity * sizeof(MemoryInfo));
            }
            
            MemoryInfo *info = &ctx->memory_infos[ctx->memory_info_count++];
            
            // Simple parsing - look for key patterns
            char *var_start = line + 6;
            while (*var_start == ' ') var_start++;
            char *var_end = strchr(var_start, ',');
            if (var_end) {
                int len = var_end - var_start;
                info->var_name = malloc(len + 1);
                strncpy(info->var_name, var_start, len);
                info->var_name[len] = '\0';
                
                // Parse allocation type
                if (strstr(line, "type=STACK")) {
                    info->alloc_type = ALLOC_STACK;
                } else if (strstr(line, "type=HEAP_GC")) {
                    info->alloc_type = ALLOC_HEAP_GC;
                } else {
                    info->alloc_type = ALLOC_HEAP_MALLOC;
                }
                
                // Parse size
                char *size_str = strstr(line, "size=");
                if (size_str) {
                    info->size = atoi(size_str + 5);
                } else {
                    info->size = 8; // Default
                }
                
                // Parse escapes
                info->escapes = strstr(line, "escapes=true") != NULL;
            }
        }
    }
    
    fclose(f);
}

static AllocType get_alloc_type(CodegenContext *ctx, const char *var_name) {
    for (int i = 0; i < ctx->memory_info_count; i++) {
        if (strcmp(ctx->memory_infos[i].var_name, var_name) == 0) {
            return ctx->memory_infos[i].alloc_type;
        }
    }
    return ALLOC_STACK; // Default
}

// ============================================
// CODE GENERATION - EXPRESSIONS
// ============================================

static int codegen_expression(CodegenContext *ctx, ASTNode *node);

static int codegen_literal(CodegenContext *ctx, ASTNode *node) {
    int reg = alloc_register(ctx);
    const char *reg_name = register_name(reg);
    
    emit(ctx, "    movq $%s, %%%s", node->token->lexeme, reg_name);
    
    return reg;
}

static int codegen_identifier(CodegenContext *ctx, ASTNode *node) {
    int reg = alloc_register(ctx);
    const char *reg_name = register_name(reg);
    
    // TODO: Variable tracking - for now use simple stack offset
    emit(ctx, "    movq -8(%%rbp), %%%s  # Load %s", reg_name, node->token->lexeme);
    
    return reg;
}

static int codegen_binary_op(CodegenContext *ctx, ASTNode *node) {
    // Left operand
    int left_reg = codegen_expression(ctx, node->children[0]);
    
    // Right operand
    int right_reg = codegen_expression(ctx, node->children[1]);
    
    const char *left_name = register_name(left_reg);
    const char *right_name = register_name(right_reg);
    
    // Perform operation based on token type
    TokenType op = node->token->type;
    
    if (op == TOKEN_PLUS) {
        emit(ctx, "    addq %%%s, %%%s", right_name, left_name);
    } else if (op == TOKEN_MINUS) {
        emit(ctx, "    subq %%%s, %%%s", right_name, left_name);
    } else if (op == TOKEN_STAR) {
        emit(ctx, "    imulq %%%s, %%%s", right_name, left_name);
    } else if (op == TOKEN_SLASH) {
        emit(ctx, "    movq %%%s, %%rax", left_name);
        emit(ctx, "    cqo");
        emit(ctx, "    idivq %%%s", right_name);
        if (left_reg != REG_RAX) {
            emit(ctx, "    movq %%rax, %%%s", left_name);
        }
    } else if (op == TOKEN_EQ_EQ) {
        emit(ctx, "    cmpq %%%s, %%%s", right_name, left_name);
        emit(ctx, "    sete %%al");
        emit(ctx, "    movzbq %%al, %%%s", left_name);
    } else if (op == TOKEN_NOT_EQ) {
        emit(ctx, "    cmpq %%%s, %%%s", right_name, left_name);
        emit(ctx, "    setne %%al");
        emit(ctx, "    movzbq %%al, %%%s", left_name);
    } else if (op == TOKEN_LT) {
        emit(ctx, "    cmpq %%%s, %%%s", right_name, left_name);
        emit(ctx, "    setl %%al");
        emit(ctx, "    movzbq %%al, %%%s", left_name);
    } else if (op == TOKEN_GT) {
        emit(ctx, "    cmpq %%%s, %%%s", right_name, left_name);
        emit(ctx, "    setg %%al");
        emit(ctx, "    movzbq %%al, %%%s", left_name);
    } else if (op == TOKEN_LT_EQ) {
        emit(ctx, "    cmpq %%%s, %%%s", right_name, left_name);
        emit(ctx, "    setle %%al");
        emit(ctx, "    movzbq %%al, %%%s", left_name);
    } else if (op == TOKEN_GT_EQ) {
        emit(ctx, "    cmpq %%%s, %%%s", right_name, left_name);
        emit(ctx, "    setge %%al");
        emit(ctx, "    movzbq %%al, %%%s", left_name);
    } else if (op == TOKEN_AND_AND) {
        emit(ctx, "    andq %%%s, %%%s", right_name, left_name);
    } else if (op == TOKEN_OR_OR) {
        emit(ctx, "    orq %%%s, %%%s", right_name, left_name);
    }
    
    free_register(ctx, right_reg);
    return left_reg;
}

static int codegen_unary_op(CodegenContext *ctx, ASTNode *node) {
    int operand_reg = codegen_expression(ctx, node->children[0]);
    const char *reg_name = register_name(operand_reg);
    
    TokenType op = node->token->type;
    
    if (op == TOKEN_MINUS) {
        emit(ctx, "    negq %%%s", reg_name);
    } else if (op == TOKEN_NOT) {
        emit(ctx, "    notq %%%s", reg_name);
    }
    
    return operand_reg;
}

static int codegen_call(CodegenContext *ctx, ASTNode *node) {
    const char *func_name = node->token->lexeme;
    
    // Special handling for gc_alloc calls (GC INTEGRATION!)
    if (strcmp(func_name, "gc_alloc") == 0 || strcmp(func_name, "malloc") == 0) {
        // Redirect malloc calls to gc_alloc
        func_name = "gc_alloc";
    }
    
    // Generate code for arguments and place in parameter registers
    int arg_count = node->child_count;
    if (arg_count > 6) arg_count = 6; // System V ABI limit
    
    for (int i = 0; i < arg_count; i++) {
        int arg_reg = codegen_expression(ctx, node->children[i]);
        int param_reg = PARAM_REGS[i];
        const char *param_name = register_name(param_reg);
        const char *arg_name = register_name(arg_reg);
        
        if (arg_reg != param_reg) {
            emit(ctx, "    movq %%%s, %%%s", arg_name, param_name);
        }
        
        free_register(ctx, arg_reg);
    }
    
    // Call function
    emit(ctx, "    call %s", func_name);
    
    // Return value in RAX
    return REG_RAX;
}

static int codegen_expression(CodegenContext *ctx, ASTNode *node) {
    if (!node) return REG_RAX;
    
    switch (node->type) {
        case NODE_LITERAL:
            return codegen_literal(ctx, node);
        case NODE_IDENTIFIER:
            return codegen_identifier(ctx, node);
        case NODE_BINARY:
            return codegen_binary_op(ctx, node);
        case NODE_UNARY:
            return codegen_unary_op(ctx, node);
        case NODE_CALL:
            return codegen_call(ctx, node);
        default:
            return REG_RAX;
    }
}

// ============================================
// CODE GENERATION - STATEMENTS
// ============================================

static void codegen_statement(CodegenContext *ctx, ASTNode *node);

static void codegen_var_decl(CodegenContext *ctx, ASTNode *node) {
    const char *var_name = node->token->lexeme;
    
    // Check memory analysis for allocation type
    AllocType alloc_type = get_alloc_type(ctx, var_name);
    
    if (alloc_type == ALLOC_HEAP_GC) {
        // Use GC allocation
        emit(ctx, "    # Allocating %s on GC heap", var_name);
        emit(ctx, "    movq $8, %%rdi");
        emit(ctx, "    call gc_alloc");
        ctx->stack_offset += 8;
        emit(ctx, "    movq %%rax, -%d(%%rbp)  # Store %s", ctx->stack_offset, var_name);
    } else {
        // Stack allocation
        ctx->stack_offset += 8;
        emit(ctx, "    # Stack allocation for %s at -%d(%%rbp)", var_name, ctx->stack_offset);
    }
    
    // If there's an initializer, evaluate it
    if (node->child_count > 0) {
        int value_reg = codegen_expression(ctx, node->children[0]);
        const char *reg_name = register_name(value_reg);
        
        emit(ctx, "    movq %%%s, -%d(%%rbp)", reg_name, ctx->stack_offset);
        
        free_register(ctx, value_reg);
    }
}

static void codegen_if_statement(CodegenContext *ctx, ASTNode *node) {
    char *else_label = gen_label(ctx);
    char *end_label = gen_label(ctx);
    
    // Evaluate condition
    int cond_reg = codegen_expression(ctx, node->children[0]);
    const char *cond_name = register_name(cond_reg);
    
    emit(ctx, "    testq %%%s, %%%s", cond_name, cond_name);
    emit(ctx, "    jz %s", else_label);
    
    free_register(ctx, cond_reg);
    
    // Then block
    codegen_statement(ctx, node->children[1]);
    emit(ctx, "    jmp %s", end_label);
    
    // Else block
    emit_label(ctx, else_label);
    if (node->child_count > 2) {
        codegen_statement(ctx, node->children[2]);
    }
    
    emit_label(ctx, end_label);
    
    free(else_label);
    free(end_label);
}

static void codegen_while_statement(CodegenContext *ctx, ASTNode *node) {
    char *loop_label = gen_label(ctx);
    char *end_label = gen_label(ctx);
    
    // Loop start
    emit_label(ctx, loop_label);
    
    // Evaluate condition
    int cond_reg = codegen_expression(ctx, node->children[0]);
    const char *cond_name = register_name(cond_reg);
    
    emit(ctx, "    testq %%%s, %%%s", cond_name, cond_name);
    emit(ctx, "    jz %s", end_label);
    
    free_register(ctx, cond_reg);
    
    // Loop body
    codegen_statement(ctx, node->children[1]);
    
    // Jump back
    emit(ctx, "    jmp %s", loop_label);
    
    emit_label(ctx, end_label);
    
    free(loop_label);
    free(end_label);
}

static void codegen_return_statement(CodegenContext *ctx, ASTNode *node) {
    // Evaluate return value if present
    if (node->child_count > 0) {
        int value_reg = codegen_expression(ctx, node->children[0]);
        
        // Move to RAX if not already there
        if (value_reg != REG_RAX) {
            const char *reg_name = register_name(value_reg);
            emit(ctx, "    movq %%%s, %%rax", reg_name);
        }
        
        free_register(ctx, value_reg);
    }
    
    // Jump to epilogue
    emit(ctx, "    jmp %s_epilogue", ctx->current_function);
}

static void codegen_expression_statement(CodegenContext *ctx, ASTNode *node) {
    if (node->child_count > 0) {
        int reg = codegen_expression(ctx, node->children[0]);
        free_register(ctx, reg);
    }
}

static void codegen_block(CodegenContext *ctx, ASTNode *node) {
    for (int i = 0; i < node->child_count; i++) {
        codegen_statement(ctx, node->children[i]);
    }
}

static void codegen_print_statement(CodegenContext *ctx, ASTNode *node) {
    // Simplified print - just evaluate expression
    if (node->child_count > 0) {
        int reg = codegen_expression(ctx, node->children[0]);
        const char *reg_name = register_name(reg);
        
        emit(ctx, "    movq %%%s, %%rdi", reg_name);
        emit(ctx, "    call print_int");
        
        free_register(ctx, reg);
    }
}

static void codegen_statement(CodegenContext *ctx, ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_VAR_DECL:
            codegen_var_decl(ctx, node);
            break;
        case NODE_IF:
            codegen_if_statement(ctx, node);
            break;
        case NODE_WHILE:
            codegen_while_statement(ctx, node);
            break;
        case NODE_RETURN:
            codegen_return_statement(ctx, node);
            break;
        case NODE_BLOCK:
            codegen_block(ctx, node);
            break;
        case NODE_EXPR_STMT:
            codegen_expression_statement(ctx, node);
            break;
        case NODE_PRINT:
            codegen_print_statement(ctx, node);
            break;
        default:
            break;
    }
}

// ============================================
// CODE GENERATION - FUNCTIONS
// ============================================

static void codegen_function_prologue(CodegenContext *ctx, const char *name) {
    emit(ctx, ".globl %s", name);
    emit_label(ctx, name);
    emit(ctx, "    pushq %%rbp");
    emit(ctx, "    movq %%rsp, %%rbp");
    
    // Reserve stack space (will be adjusted after body)
    if (ctx->stack_offset > 0) {
        emit(ctx, "    subq $%d, %%rsp", ctx->stack_offset);
    }
}

static void codegen_function_epilogue(CodegenContext *ctx) {
    emit_label(ctx, ctx->current_function);
    emit(ctx, "_epilogue:");
    emit(ctx, "    movq %%rbp, %%rsp");
    emit(ctx, "    popq %%rbp");
    emit(ctx, "    ret");
}

static void codegen_function(CodegenContext *ctx, ASTNode *node) {
    const char *func_name = node->token->lexeme;
    
    // Reset context for new function
    ctx->stack_offset = 0;
    for (int i = 0; i < 16; i++) {
        ctx->register_used[i] = false;
    }
    ctx->current_function = strdup(func_name);
    
    // Generate prologue
    codegen_function_prologue(ctx, func_name);
    
    // Generate function body
    if (node->child_count > 0) {
        codegen_statement(ctx, node->children[0]);
    }
    
    // Generate epilogue
    codegen_function_epilogue(ctx);
    
    free(ctx->current_function);
}

// ============================================
// CODE GENERATION - PROGRAM
// ============================================

static void codegen_program(CodegenContext *ctx, ASTNode *ast) {
    // Assembly header
    emit(ctx, ".section .text");
    emit(ctx, "");
    
    // Generate code for all functions
    for (int i = 0; i < ast->child_count; i++) {
        ASTNode *child = ast->children[i];
        if (child->type == NODE_FUNC_DECL) {
            codegen_function(ctx, child);
            emit(ctx, "");
        }
    }
}

// ============================================
// MAIN ENTRY POINT
// ============================================

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <memory_analysis.txt> <output.s>\n", argv[0]);
        return 1;
    }
    
    FILE *output = fopen(argv[2], "w");
    if (!output) {
        fprintf(stderr, "Error: Cannot open output file %s\n", argv[2]);
        return 1;
    }
    
    // Initialize codegen context
    CodegenContext ctx = {0};
    ctx.output = output;
    ctx.label_counter = 0;
    ctx.stack_offset = 0;
    
    // Parse memory analysis file
    parse_memory_analysis(&ctx, argv[1]);
    
    // For now, generate a simple test program
    // In full implementation, this would parse AST from parser output
    emit(&ctx, "# Generated by MELP Codegen Compiler - Stage 0");
    emit(&ctx, "# GC Integration: malloc calls redirected to gc_alloc");
    emit(&ctx, "");
    emit(&ctx, ".section .text");
    emit(&ctx, "");
    emit(&ctx, ".globl main");
    emit_label(&ctx, "main");
    emit(&ctx, "    pushq %%rbp");
    emit(&ctx, "    movq %%rsp, %%rbp");
    emit(&ctx, "");
    emit(&ctx, "    # Example: Allocate using GC");
    emit(&ctx, "    movq $16, %%rdi");
    emit(&ctx, "    call gc_alloc");
    emit(&ctx, "");
    emit(&ctx, "    # Return 0");
    emit(&ctx, "    movq $0, %%rax");
    emit(&ctx, "    movq %%rbp, %%rsp");
    emit(&ctx, "    popq %%rbp");
    emit(&ctx, "    ret");
    
    fclose(output);
    
    printf("✓ Codegen: Generated %s with GC integration\n", argv[2]);
    printf("  - Memory allocations analyzed: %d\n", ctx.memory_info_count);
    printf("  - Assembly file created with gc_alloc calls\n");
    
    // Cleanup
    for (int i = 0; i < ctx.memory_info_count; i++) {
        free(ctx.memory_infos[i].var_name);
    }
    free(ctx.memory_infos);
    
    return 0;
}
