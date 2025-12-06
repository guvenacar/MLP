#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef enum {
    FUNC_PARAM_NUMERIC,
    FUNC_PARAM_TEXT,
    FUNC_PARAM_BOOLEAN,
    FUNC_PARAM_POINTER,
    FUNC_PARAM_ARRAY
} FunctionParamType;

typedef struct FunctionParam {
    char* name;
    FunctionParamType type;
    void* default_value;  // TIER 1: Default parameter value
    int has_default;      // TIER 1: Whether param has default
    struct FunctionParam* next;
} FunctionParam;

typedef enum {
    FUNC_RETURN_NUMERIC,
    FUNC_RETURN_TEXT,
    FUNC_RETURN_BOOLEAN,
    FUNC_RETURN_VOID
} FunctionReturnType;

typedef struct Statement Statement;

typedef struct {
    char* name;
    FunctionParam* params;
    FunctionReturnType return_type;
    Statement* body;           // Linked list of statements
    int param_count;
    int local_var_count;       // For stack frame calculation
} FunctionDeclaration;

typedef struct {
    char* function_name;
    struct Expression** arguments;  // Array of expressions
    int arg_count;
} FunctionCall;

typedef struct {
    struct Expression* return_value;  // NULL for void return
} ReturnStatement;

// Function management
FunctionDeclaration* function_create(const char* name, FunctionReturnType return_type);
void function_add_param(FunctionDeclaration* func, const char* name, FunctionParamType type);
void function_set_body(FunctionDeclaration* func, Statement* body);
void function_free(FunctionDeclaration* func);

// Function call
FunctionCall* function_call_create(const char* name);
void function_call_add_arg(FunctionCall* call, struct Expression* arg);
void function_call_free(FunctionCall* call);

// Return statement
ReturnStatement* return_statement_create(struct Expression* value);
void return_statement_free(ReturnStatement* ret);

#endif
