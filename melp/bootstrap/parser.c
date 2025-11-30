// MELP Phase 1 - Bootstrap Compiler
// Parser: Deklarasyonlar + Assignment + Print

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.c"

// User-visible types (what user writes in code)
typedef enum {
    TYPE_NUMERIC,
    TYPE_DECIMAL,
    TYPE_BOOLEAN,
    TYPE_STRING  // Phase 5: String type
} VarType;

// TTO: Internal numeric representation (transparent to user)
typedef enum {
    INTERNAL_INT64,      // Small integers: -2^63 to 2^63-1
    INTERNAL_DOUBLE,     // Floating point: ~15 digits precision
    INTERNAL_BIGDECIMAL  // Arbitrary precision (future)
} InternalNumericType;

// TTO: Internal string representation (transparent to user)
typedef enum {
    INTERNAL_SSO,        // Small String Optimization: ≤23 bytes inline
    INTERNAL_HEAP,       // Heap allocated: >23 bytes
    INTERNAL_RODATA      // Read-only data section: string literals
} InternalStringType;

typedef enum {
    STMT_DECLARATION,
    STMT_MULTI_DECLARATION, // Phase 21: Multiple typed declarations (numeric, string a, b)
    STMT_ASSIGNMENT,
    STMT_MULTI_ASSIGNMENT,  // Multiple variable assignment (a, b = func())
    STMT_DESTRUCTURE,       // Destructuring assignment (a, b, c = arr)
    STMT_PRINT,
    STMT_IF,
    STMT_FOR,
    STMT_WHILE,
    STMT_DO_WHILE,
    STMT_SWITCH,
    STMT_EXIT,
    STMT_CONTINUE,
    STMT_FUNC_DEF,
    STMT_RETURN,
    STMT_EXPR_STMT,  // Expression statement (e.g., function call)
    STMT_STRUCT_DEF,  // Phase 6: Struct definition
    STMT_INTERFACE_DEF, // Interface definition
    STMT_ENUM_DEF,    // Phase 10: Enum definition
    STMT_TYPE_ALIAS,  // Phase 11: Type alias
    STMT_TRY_CATCH,   // Phase 12: Try-catch block
    STMT_THROW,       // Phase 12: Throw statement
    STMT_YIELD,       // Phase 14: Yield statement (generator)
    STMT_FOR_IN,      // Phase 14: For-in loop (iterator)
    STMT_DEBUG_LABEL, // Debug label definition
    STMT_DEBUG_GOTO,  // Debug goto statement
    STMT_DEBUG_IF,    // Debug conditional block
    STMT_DEBUG_PAUSE, // Debug pause (breakpoint)
    STMT_IMPORT,      // Import statement (import math, import string as str)
    STMT_MODULE_DEF,  // Module definition (module MyModule...end module)
    STMT_OPERATOR_DEF, // Phase 16: Operator overloading definition
    STMT_MATCH,       // Phase 17: Pattern matching (match...end match)
    STMT_STATE_DECL   // Phase 18: State declaration (shared state)
} StmtType;

typedef enum {
    EXPR_NUMBER,
    EXPR_VARIABLE,
    EXPR_COMPARISON,
    EXPR_BINARY_OP,
    EXPR_FUNC_CALL,
    EXPR_STRING,     // Phase 5: String literal
    EXPR_NULL,       // Phase 10: null literal
    EXPR_FIELD_ACCESS, // Phase 6: Struct field access (e.g., person.name)
    EXPR_TERNARY,    // Ternary operator (condition ? true_val : false_val)
    EXPR_ARRAY_INDEX, // Phase 8: Array indexing (e.g., arr[0])
    EXPR_ADDRESS_OF, // Phase 10: Address-of operator (&var)
    EXPR_DEREFERENCE, // Phase 10: Dereference operator (*ptr)
    EXPR_LOGICAL_AND, // Logical AND (&&)
    EXPR_LOGICAL_OR,  // Logical OR (||)
    EXPR_LOGICAL_NOT, // Logical NOT (!)
    EXPR_LAMBDA,      // Phase 12: Lambda function (inline anonymous function)
    EXPR_AWAIT,       // Phase 12: Await expression (await async_call())
    EXPR_ARRAY_LITERAL, // Array literal: [10, 20, 30]
    // Phase 15: Null safety
    EXPR_OPTIONAL_CHAIN, // ?. (optional chaining: obj?.field)
    EXPR_NULL_COALESCE,  // ?? (null coalescing: value ?? default)
    // Phase 22: List and Tuple
    EXPR_LIST_LITERAL,   // List literal: (10, "hello", true)
    EXPR_TUPLE_LITERAL   // Tuple literal: <10, "hello", true>
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
        char* string_value;  // Phase 5: String literal value
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
        struct {
            char* func_name;
            struct Expression** args;
            int arg_count;
            // Phase 13: Generics
            VarType* type_args;         // Type arguments (e.g., [TYPE_NUMERIC] for max<numeric>)
            int type_arg_count;         // Number of type arguments
        } func_call;
        struct {
            char* object_name;  // Variable name (e.g., "person")
            char* field_name;   // Field name (e.g., "name")
        } field_access;
        struct {
            struct Expression* condition;
            struct Expression* true_expr;
            struct Expression* false_expr;
        } ternary;
        struct {
            char* array_name;           // Array variable name
            struct Expression* index;   // Index expression
        } array_index;
        struct Expression* unary_operand;  // Phase 10: For & and * operators
        struct {
            struct Expression* left;   // For AND/OR
            struct Expression* right;  // For AND/OR
        } logical_binary;
        struct Expression* logical_not_operand;  // For NOT
        struct {
            char** param_names;        // Parameter names
            int param_count;           // Number of parameters
            struct Expression* body;   // Lambda body (single expression)
            char** captured_vars;      // Captured variable names (for closures)
            int captured_count;        // Number of captured variables
        } lambda;
        struct {
            struct Expression* awaited_expr;  // Expression to await (usually func call)
        } await_expr;
        struct {
            struct Expression** elements;  // Array literal elements
            int count;                      // Number of elements
        } array_literal;
        // Phase 22: List and Tuple literals
        struct {
            struct Expression** elements;  // List literal elements (heterojen, mutable)
            int count;                      // Number of elements
        } list_literal;
        struct {
            struct Expression** elements;  // Tuple literal elements (heterojen, immutable)
            int count;                      // Number of elements
        } tuple_literal;
        // Phase 15: Null safety
        struct {
            struct Expression* object;      // Object expression (e.g., person in person?.name)
            char* field_name;               // Field name (e.g., "name")
        } optional_chain;
        struct {
            struct Expression* left;        // Value that might be null
            struct Expression* right;       // Default value if null
        } null_coalesce;
    };
    // TTO: Internal type information (transparent to user)
    InternalNumericType internal_numeric_type;  // For EXPR_NUMBER
    InternalStringType internal_string_type;    // For EXPR_STRING
    double double_value;                        // For INTERNAL_DOUBLE numbers
    int has_decimal_point;                      // 1 if literal had decimal point
} Expression;

typedef struct {
    VarType type;
    char* name;
    char* struct_name;       // NULL for non-struct types, struct type name for struct instances
    int is_array;            // 1 if array type, 0 otherwise
    int is_list;             // Phase 21: 1 if list type (dynamic), 0 otherwise
    int array_size;          // Array size (0 = dynamic/unknown)
    Expression** array_init; // Array literal values (NULL if no literal)
    int array_init_count;    // Number of elements in array_init
    int is_pointer;          // Phase 10: 1 if pointer type, 0 otherwise
    int is_nullable;         // Phase 10: 1 if nullable type (type?), 0 otherwise
    int is_union;            // Phase 11: 1 if union type (type1 | type2), 0 otherwise
    VarType* union_types;    // Phase 11: Array of union types (NULL if not union)
    int union_count;         // Phase 11: Number of types in union
    Expression* init_value;  // NULL if no initialization
    int is_exported;         // Phase 9: 1 if export, 0 if private (default export in modules)
    VarType* struct_type_args;  // Phase 13: Generic type arguments for struct (e.g., Box<numeric>)
    int struct_type_arg_count;  // Phase 13: Number of type arguments
} Declaration;

// Phase 6: Struct field definition
typedef struct {
    VarType type;
    char* name;
    int is_generic;         // Phase 13: Is this field a generic type parameter?
    int generic_index;      // Phase 13: Index into type_params if generic
} StructField;

// Interface method signature
typedef struct MethodSignature {
    char* name;             // Method name
    VarType* param_types;   // Parameter types
    char** param_names;     // Parameter names
    int param_count;        // Number of parameters
    VarType return_type;    // Return type
    int has_return;         // 1 if returns value, 0 if void
} MethodSignature;

// Phase 10: Enum member definition
typedef struct {
    char* name;     // Member name (e.g., "Active")
    long value;     // Member value (e.g., 1)
} EnumMember;

// Switch case definition
typedef struct SwitchCase {
    Expression* value;      // Case value to match (NULL for range or type matching)
    Expression* guard;      // Guard condition (if clause) - NULL if no guard
    Expression* range_start; // Range start (for case 1..10) - NULL if not range
    Expression* range_end;   // Range end (for case 1..10) - NULL if not range
    int is_type_check;      // 1 if this is "case is Type", 0 otherwise
    char* type_name;        // Type name for type checking (NULL if not type check)
    struct Statement** body;  // Case body statements
    int body_count;
} SwitchCase;

typedef struct Statement {
    StmtType type;
    union {
        Declaration* declaration;
        struct {
            char* var_name;
            char* field_name;      // NULL for regular assignment, field name for field assignment
            Expression* array_index; // NULL for regular assignment, index expr for array assignment
            Expression* dereference_target; // Phase 10: NULL for regular, ptr expr for *ptr = value
            Expression* value;
        } assignment;
        struct {
            char** var_names;      // Array of variable names (e.g., ["a", "b", "c"])
            int var_count;         // Number of variables
            Expression* func_call; // Function call that returns multiple values
        } multi_assignment;
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
        struct {
            Expression* condition;
            struct Statement** body;
            int body_count;
        } do_while_stmt;
        struct {
            Expression* value;      // Switch expression to evaluate
            struct SwitchCase** cases;  // Array of cases
            int case_count;
            struct Statement** default_body;  // Default case body (NULL if no default)
            int default_count;
        } switch_stmt;
        struct {
            char* func_name;
            VarType* param_types;       // Parameter types
            char** param_names;         // Parameter names
            char** param_struct_names;  // Struct type names (NULL for primitives)
            int param_count;
            VarType* return_types;      // Return types (NULL if no return type specified)
            int return_count;           // Number of return values (0 if void)
            struct Statement** body;
            int body_count;
            int is_exported;            // Phase 9: 1 if export, 0 if private
            int is_async;               // Phase 12: 1 if async, 0 if sync
            int is_generator;           // Phase 14: 1 if generator (yields), 0 if regular
            // Phase 13: Generics
            char** type_params;         // Type parameter names (e.g., ["T", "U"])
            int type_param_count;       // Number of type parameters
            int* param_is_generic;      // 1 if param uses type param, 0 otherwise
            int* param_generic_index;   // Index into type_params for generic params
            int return_is_generic;      // 1 if return type uses type param
            int return_generic_index;   // Index into type_params for return type
        } func_def;
        struct {
            Expression** values;    // Return values (multiple for multi-return)
            int value_count;        // Number of return values
        } return_stmt;
        struct {
            char* struct_name;
            StructField* fields;
            int field_count;
            char** implements;      // Implemented interfaces
            int implements_count;   // Number of interfaces
            // Phase 13: Generics
            char** type_params;     // Type parameter names (e.g., ["T"])
            int type_param_count;   // Number of type parameters
        } struct_def;
        struct {
            char* interface_name;
            struct MethodSignature** methods;
            int method_count;
        } interface_def;
        struct {
            char* enum_name;
            EnumMember* members;
            int member_count;
        } enum_def;
        struct {
            char* alias_name;  // e.g., "PersonId"
            VarType base_type; // e.g., TYPE_NUMERIC
            char* struct_name; // NULL for primitive types, struct name for TYPE_STRUCT
        } type_alias;
        struct {
            struct Statement** try_body;
            int try_count;
            // Multiple catch blocks
            struct {
                char* exception_type;      // Exception type (NULL = catch all)
                char* exception_var;       // Variable name to hold exception
                struct Statement** body;
                int body_count;
            }* catch_blocks;
            int catch_count;
            // Finally block
            struct Statement** finally_body;
            int finally_count;
        } try_catch;
        struct {
            char* error_type;          // Exception type
            Expression* error_message; // Error message expression
        } throw_stmt;
        struct {
            char* label_name;          // Debug label name
        } debug_label;
        struct {
            char* target_label;        // Debug goto target label
        } debug_goto;
        struct {
            Expression* condition;     // Debug if condition
            struct Statement** body;   // Debug if body
            int body_count;
        } debug_if;
        // No data needed for debug pause - just a statement marker
        struct {
            char* module_name;         // Module name to import (e.g., "math")
            char* alias;               // Alias (e.g., "str" for "import string as str"), NULL if no alias
        } import_stmt;
        struct {
            VarType var_type;          // Type of variables (all must be same type)
            char** var_names;          // Array of variable names
            int var_count;             // Number of variables
            Expression* source;        // Source expression (array or struct)
        } destructure;
        // Phase 14: Yield statement
        struct {
            Expression* value;         // Value to yield
        } yield_stmt;
        // Phase 14: For-in loop
        struct {
            char* var_name;            // Loop variable name
            VarType var_type;          // Loop variable type
            Expression* iterable;      // Iterable expression (generator call or array)
            struct Statement** body;   // Loop body
            int body_count;
        } for_in;
        struct {
            char* module_name;         // Module name (e.g., "MyModule")
            struct Statement** body;   // Module body (functions, structs, etc.)
            int body_count;
        } module_def;
        // Phase 16: Operator Overloading
        struct {
            char* op;                  // Operator symbol (+, -, *, /, ==, etc.)
            char* type_name;           // Type name this operator is for
            char* param1_name;         // First parameter name
            char* param2_name;         // Second parameter name (NULL for unary)
            VarType return_type;       // Return type
            struct Statement** body;   // Operator body
            int body_count;
        } operator_def;
        // Phase 17: Pattern Matching
        struct {
            Expression* value;         // Value to match against
            struct {
                Expression** patterns; // Pattern expressions (number, variable, range)
                int pattern_count;     // Number of patterns in this case (for 1, 2, 3 =>)
                int is_wildcard;       // 1 if this is _ (default case)
                int is_range;          // 1 if range pattern (1 to 10)
                Expression* range_start; // Start of range (if is_range)
                Expression* range_end;   // End of range (if is_range)
                struct Statement** body; // Case body statements
                int body_count;
            }* cases;
            int case_count;
        } match_stmt;
        // Phase 18: State Management
        struct {
            char* name;                // State variable name
            VarType type;              // State variable type
            Expression* initial_value; // Initial value (optional)
            int is_shared;             // 1 if shared (thread-safe)
        } state_decl;
        // Phase 21: Multiple typed declarations (numeric, string, boolean a, b, c)
        struct {
            Declaration** declarations; // Array of declarations
            int count;                  // Number of declarations
        } multi_decl;
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
    // TTO: Default to int64 for integer values
    expr->internal_numeric_type = INTERNAL_INT64;
    expr->has_decimal_point = 0;
    expr->double_value = 0.0;
    return expr;
}

// TTO: Create a double number expression
Expression* expression_create_double(double value) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_NUMBER;
    expr->double_value = value;
    expr->number_value = (long)value;  // Keep integer approximation
    // TTO: Use double for floating point
    expr->internal_numeric_type = INTERNAL_DOUBLE;
    expr->has_decimal_point = 1;
    return expr;
}

Expression* expression_create_variable(const char* name) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_VARIABLE;
    expr->var_name = malloc(strlen(name) + 1);
    strcpy(expr->var_name, name);
    return expr;
}

Expression* expression_create_string(const char* value) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_STRING;
    expr->string_value = malloc(strlen(value) + 1);
    strcpy(expr->string_value, value);
    // TTO: Determine internal string type based on length
    size_t len = strlen(value);
    if (len <= 23) {
        expr->internal_string_type = INTERNAL_SSO;  // Small string - stack
    } else {
        expr->internal_string_type = INTERNAL_RODATA;  // Literal goes to .rodata
    }
    return expr;
}

Expression* expression_create_field_access(const char* object_name, const char* field_name) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_FIELD_ACCESS;
    expr->field_access.object_name = malloc(strlen(object_name) + 1);
    strcpy(expr->field_access.object_name, object_name);
    expr->field_access.field_name = malloc(strlen(field_name) + 1);
    strcpy(expr->field_access.field_name, field_name);
    return expr;
}

Expression* expression_create_ternary(Expression* condition, Expression* true_expr, Expression* false_expr) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_TERNARY;
    expr->ternary.condition = condition;
    expr->ternary.true_expr = true_expr;
    expr->ternary.false_expr = false_expr;
    return expr;
}

// Phase 15: Null safety - Optional chaining
Expression* expression_create_optional_chain(Expression* object, const char* field_name) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_OPTIONAL_CHAIN;
    expr->optional_chain.object = object;
    expr->optional_chain.field_name = malloc(strlen(field_name) + 1);
    strcpy(expr->optional_chain.field_name, field_name);
    return expr;
}

// Phase 15: Null safety - Null coalescing
Expression* expression_create_null_coalesce(Expression* left, Expression* right) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_NULL_COALESCE;
    expr->null_coalesce.left = left;
    expr->null_coalesce.right = right;
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

Expression* expression_create_func_call(const char* func_name, Expression** args, int arg_count) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_FUNC_CALL;
    expr->func_call.func_name = malloc(strlen(func_name) + 1);
    strcpy(expr->func_call.func_name, func_name);
    expr->func_call.args = args;
    expr->func_call.arg_count = arg_count;
    expr->func_call.type_args = NULL;
    expr->func_call.type_arg_count = 0;
    return expr;
}

// Phase 10: Create address-of expression (&var)
Expression* expression_create_address_of(Expression* operand) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_ADDRESS_OF;
    expr->unary_operand = operand;
    return expr;
}

// Phase 10: Create dereference expression (*ptr)
Expression* expression_create_dereference(Expression* operand) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_DEREFERENCE;
    expr->unary_operand = operand;
    return expr;
}

// Logical operators: AND, OR, NOT
Expression* expression_create_logical_and(Expression* left, Expression* right) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_LOGICAL_AND;
    expr->logical_binary.left = left;
    expr->logical_binary.right = right;
    return expr;
}

Expression* expression_create_logical_or(Expression* left, Expression* right) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_LOGICAL_OR;
    expr->logical_binary.left = left;
    expr->logical_binary.right = right;
    return expr;
}

Expression* expression_create_logical_not(Expression* operand) {
    Expression* expr = malloc(sizeof(Expression));
    expr->type = EXPR_LOGICAL_NOT;
    expr->logical_not_operand = operand;
    return expr;
}

// Forward declarations
Expression* parser_parse_expression(Parser* parser);
Expression* parser_parse_and_expression(Parser* parser);
Expression* parser_parse_comparison_expression(Parser* parser);
Expression* parser_parse_arithmetic_expression(Parser* parser);
Statement* parser_parse_else_if_chain(Parser* parser);  // Helper for else-if chains

// Parse interpolated string: $"text {expr} text"
// String value contains: TEXT\1EXPR\2TEXT format
// Returns concatenation expression
Expression* parser_parse_interpolated_string(Parser* parser, const char* str_value) {
    Expression* result = NULL;
    int i = 0;
    int len = strlen(str_value);
    
    while (i < len) {
        // Find next expression marker or end
        int text_start = i;
        while (i < len && str_value[i] != '\1') {
            i++;
        }
        
        // Add text literal if any
        if (i > text_start) {
            int text_len = i - text_start;
            char* text = malloc(text_len + 1);
            strncpy(text, str_value + text_start, text_len);
            text[text_len] = '\0';
            
            Expression* str_expr = expression_create_string(text);
            free(text);
            
            if (result == NULL) {
                result = str_expr;
            } else {
                result = expression_create_binary_op(result, str_expr, BIN_OP_ADD);
            }
        }
        
        // If we found expression marker
        if (i < len && str_value[i] == '\1') {
            i++;  // Skip \1
            
            // Find expression end marker \2
            int expr_start = i;
            while (i < len && str_value[i] != '\2') {
                i++;
            }
            
            if (i > expr_start) {
                int expr_len = i - expr_start;
                char* expr_code = malloc(expr_len + 1);
                strncpy(expr_code, str_value + expr_start, expr_len);
                expr_code[expr_len] = '\0';
                
                // Parse the expression by creating a temporary lexer/parser
                Lexer* expr_lexer = lexer_create(expr_code);
                Parser* expr_parser = parser_create(expr_lexer);
                
                Expression* expr = parser_parse_expression(expr_parser);
                
                // Free temporary parser/lexer
                free(expr_parser);
                free(expr_lexer);
                free(expr_code);
                
                // TODO: Convert numeric expressions to string
                // For now, we'll handle this in codegen
                
                if (result == NULL) {
                    result = expr;
                } else {
                    result = expression_create_binary_op(result, expr, BIN_OP_ADD);
                }
            }
            
            if (i < len && str_value[i] == '\2') {
                i++;  // Skip \2
            }
        }
    }
    
    return result != NULL ? result : expression_create_string("");
}

// Helper: Find variables used in an expression (for closure capture)
void find_variables_in_expression(Expression* expr, char*** vars, int* count, int* capacity) {
    if (!expr) return;
    
    if (expr->type == EXPR_VARIABLE) {
        // Check if already in list
        for (int i = 0; i < *count; i++) {
            if (strcmp((*vars)[i], expr->var_name) == 0) {
                return;  // Already captured
            }
        }
        
        // Add to list
        if (*count >= *capacity) {
            *capacity *= 2;
            *vars = realloc(*vars, sizeof(char*) * (*capacity));
        }
        (*vars)[*count] = malloc(strlen(expr->var_name) + 1);
        strcpy((*vars)[*count], expr->var_name);
        (*count)++;
    } else if (expr->type == EXPR_BINARY_OP) {
        find_variables_in_expression(expr->binary_op.left, vars, count, capacity);
        find_variables_in_expression(expr->binary_op.right, vars, count, capacity);
    } else if (expr->type == EXPR_COMPARISON) {
        find_variables_in_expression(expr->comparison.left, vars, count, capacity);
        find_variables_in_expression(expr->comparison.right, vars, count, capacity);
    } else if (expr->type == EXPR_FUNC_CALL) {
        for (int i = 0; i < expr->func_call.arg_count; i++) {
            find_variables_in_expression(expr->func_call.args[i], vars, count, capacity);
        }
    } else if (expr->type == EXPR_TERNARY) {
        find_variables_in_expression(expr->ternary.condition, vars, count, capacity);
        find_variables_in_expression(expr->ternary.true_expr, vars, count, capacity);
        find_variables_in_expression(expr->ternary.false_expr, vars, count, capacity);
    } else if (expr->type == EXPR_LOGICAL_AND || expr->type == EXPR_LOGICAL_OR) {
        find_variables_in_expression(expr->logical_binary.left, vars, count, capacity);
        find_variables_in_expression(expr->logical_binary.right, vars, count, capacity);
    } else if (expr->type == EXPR_LOGICAL_NOT) {
        find_variables_in_expression(expr->logical_not_operand, vars, count, capacity);
    } else if (expr->type == EXPR_ARRAY_INDEX) {
        find_variables_in_expression(expr->array_index.index, vars, count, capacity);
    } else if (expr->type == EXPR_FIELD_ACCESS) {
        // Field access: object_name is a variable
        for (int i = 0; i < *count; i++) {
            if (strcmp((*vars)[i], expr->field_access.object_name) == 0) {
                return;
            }
        }
        if (*count >= *capacity) {
            *capacity *= 2;
            *vars = realloc(*vars, sizeof(char*) * (*capacity));
        }
        (*vars)[*count] = malloc(strlen(expr->field_access.object_name) + 1);
        strcpy((*vars)[*count], expr->field_access.object_name);
        (*count)++;
    }
}

Expression* parser_parse_primary_expression(Parser* parser) {
    // Check for await expression
    if (parser->current_token->type == TOKEN_AWAIT) {
        parser_advance(parser);  // skip 'await'
        
        Expression* expr = malloc(sizeof(Expression));
        expr->type = EXPR_AWAIT;
        expr->await_expr.awaited_expr = parser_parse_primary_expression(parser);
        return expr;
    }
    
    if (parser->current_token->type == TOKEN_FUNC) {
        // Lambda function: func(x, y) = x + y
        parser_advance(parser); // skip 'func'
        
        if (parser->current_token->type != TOKEN_LPAREN) {
            fprintf(stderr, "Parser error: Expected '(' after 'func' in lambda at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip '('
        
        // Parse parameters
        char** param_names = malloc(sizeof(char*) * 10);
        int param_count = 0;
        int param_capacity = 10;
        
        if (parser->current_token->type != TOKEN_RPAREN) {
            while (1) {
                if (parser->current_token->type != TOKEN_IDENTIFIER) {
                    fprintf(stderr, "Parser error: Expected parameter name in lambda at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                
                if (param_count >= param_capacity) {
                    param_capacity *= 2;
                    param_names = realloc(param_names, sizeof(char*) * param_capacity);
                }
                
                param_names[param_count] = malloc(strlen(parser->current_token->value) + 1);
                strcpy(param_names[param_count], parser->current_token->value);
                param_count++;
                
                parser_advance(parser);
                
                if (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser);
                } else {
                    break;
                }
            }
        }
        
        if (parser->current_token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Parser error: Expected ')' after lambda parameters at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip ')'
        
        // Expect '='
        if (parser->current_token->type != TOKEN_ASSIGN) {
            fprintf(stderr, "Parser error: Expected '=' after lambda parameters at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip '='
        
        // Parse body expression
        Expression* body = parser_parse_expression(parser);
        
        // Create lambda expression
        Expression* lambda = malloc(sizeof(Expression));
        lambda->type = EXPR_LAMBDA;
        lambda->lambda.param_names = param_names;
        lambda->lambda.param_count = param_count;
        lambda->lambda.body = body;
        
        // Detect captured variables (closure)
        char** all_vars = malloc(sizeof(char*) * 10);
        int all_vars_count = 0;
        int all_vars_capacity = 10;
        find_variables_in_expression(body, &all_vars, &all_vars_count, &all_vars_capacity);
        
        // Filter out parameters to get only captured variables
        char** captured = malloc(sizeof(char*) * all_vars_count);
        int captured_count = 0;
        
        for (int i = 0; i < all_vars_count; i++) {
            int is_param = 0;
            for (int j = 0; j < param_count; j++) {
                if (strcmp(all_vars[i], param_names[j]) == 0) {
                    is_param = 1;
                    break;
                }
            }
            
            if (!is_param) {
                captured[captured_count] = all_vars[i];
                captured_count++;
            } else {
                free(all_vars[i]);
            }
        }
        
        free(all_vars);
        lambda->lambda.captured_vars = captured;
        lambda->lambda.captured_count = captured_count;
        
        return lambda;
    } else if (parser->current_token->type == TOKEN_NUMBER) {
        // TTO: Check if the number has a decimal point
        const char* num_str = parser->current_token->value;
        int has_decimal = (strchr(num_str, '.') != NULL);
        
        if (has_decimal) {
            // TTO: Parse as double
            double value = strtod(num_str, NULL);
            parser_advance(parser);
            return expression_create_double(value);
        } else {
            // TTO: Parse as int64
            long value = atol(num_str);
            parser_advance(parser);
            return expression_create_number(value);
        }
    } else if (parser->current_token->type == TOKEN_NULL) {
        // null literal
        parser_advance(parser);
        Expression* expr = malloc(sizeof(Expression));
        expr->type = EXPR_NULL;
        return expr;
    } else if (parser->current_token->type == TOKEN_LBRACKET) {
        // Array literal: [10, 20, 30, 40, 50]
        parser_advance(parser); // skip '['
        
        Expression* expr = malloc(sizeof(Expression));
        expr->type = EXPR_ARRAY_LITERAL;
        expr->array_literal.elements = malloc(sizeof(Expression*) * 10);
        expr->array_literal.count = 0;
        int capacity = 10;
        
        if (parser->current_token->type != TOKEN_RBRACKET) {
            while (1) {
                if (expr->array_literal.count >= capacity) {
                    capacity *= 2;
                    expr->array_literal.elements = realloc(expr->array_literal.elements,
                                                          sizeof(Expression*) * capacity);
                }
                expr->array_literal.elements[expr->array_literal.count++] = 
                    parser_parse_expression(parser);
                
                if (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser);
                } else {
                    break;
                }
            }
        }
        
        if (parser->current_token->type != TOKEN_RBRACKET) {
            fprintf(stderr, "Parser error: Expected ']' after array literal at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip ']'
        
        return expr;
    } else if (parser->current_token->type == TOKEN_INTERPOLATED_STRING) {
        // Interpolated string: $"text {expr} text"
        char* str_value = malloc(strlen(parser->current_token->value) + 1);
        strcpy(str_value, parser->current_token->value);
        parser_advance(parser);
        Expression* expr = parser_parse_interpolated_string(parser, str_value);
        free(str_value);
        return expr;
    } else if (parser->current_token->type == TOKEN_STRING) {
        // String literal
        char* str_value = malloc(strlen(parser->current_token->value) + 1);
        strcpy(str_value, parser->current_token->value);
        parser_advance(parser);
        Expression* expr = expression_create_string(str_value);
        free(str_value);
        return expr;
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        char* name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(name, parser->current_token->value);
        parser_advance(parser);
        
        // Phase 13: Check for generic type arguments <type1, type2, ...>
        VarType* type_args = NULL;
        int type_arg_count = 0;
        
        if (parser->current_token->type == TOKEN_LESS) {
            // Could be generic call or comparison - peek ahead
            // If we see a type keyword, it's generic; if number, it's comparison
            
            // Save state BEFORE consuming '<'
            // Note: When current_token is '<', lexer->pos is AFTER '<'
            // So we need to go back 1 character to re-read '<'
            int saved_pos = parser->lexer->pos - 1;  // Position OF '<'
            int saved_line = parser->lexer->line;
            
            parser_advance(parser); // consume '<', get next token
            
            // Check if this looks like a generic call (type keyword) or comparison
            int is_generic = 0;
            if (parser->current_token->type == TOKEN_NUMERIC ||
                parser->current_token->type == TOKEN_DECIMAL ||
                parser->current_token->type == TOKEN_BOOLEAN ||
                parser->current_token->type == TOKEN_TEXT) {
                // Type keyword after < means generic
                is_generic = 1;
            }
            // If it's a number (like 0.5) or other non-type token, it's a comparison
            
            if (!is_generic) {
                // Backtrack: restore lexer position and re-lex '<'
                token_free(parser->current_token);
                parser->lexer->pos = saved_pos;
                parser->lexer->line = saved_line;
                parser->current_token = lexer_next_token(parser->lexer);
                // Now current_token should be '<' again
            } else {
                // Generic call - continue parsing type arguments
                type_args = malloc(sizeof(VarType) * 10);
                int type_arg_capacity = 10;
                
                while (1) {
                    VarType arg_type;
                    if (parser->current_token->type == TOKEN_NUMERIC) {
                        arg_type = TYPE_NUMERIC;
                    } else if (parser->current_token->type == TOKEN_DECIMAL) {
                        arg_type = TYPE_DECIMAL;
                    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                        arg_type = TYPE_BOOLEAN;
                    } else if (parser->current_token->type == TOKEN_TEXT) {
                        arg_type = TYPE_STRING;
                    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
                        // Could be a struct type
                        arg_type = TYPE_NUMERIC; // Placeholder
                    } else {
                        // Not a valid type - this wasn't a generic call
                        // TODO: backtrack properly
                        break;
                    }
                    
                    if (type_arg_count >= type_arg_capacity) {
                        type_arg_capacity *= 2;
                        type_args = realloc(type_args, sizeof(VarType) * type_arg_capacity);
                    }
                    type_args[type_arg_count++] = arg_type;
                    parser_advance(parser);
                    
                    if (parser->current_token->type == TOKEN_COMMA) {
                        parser_advance(parser);
                    } else {
                        break;
                    }
                }
                
                if (parser->current_token->type == TOKEN_GREATER) {
                    parser_advance(parser); // skip '>'
                } else {
                    // Not a valid generic - free and reset
                    free(type_args);
                    type_args = NULL;
                    type_arg_count = 0;
                }
            }
        }
        
        // Check for function call
        if (parser->current_token->type == TOKEN_LPAREN) {
            parser_advance(parser); // skip '('
            
            // Parse arguments
            Expression** args = malloc(sizeof(Expression*) * 10);
            int arg_count = 0;
            int arg_capacity = 10;
            
            if (parser->current_token->type != TOKEN_RPAREN) {
                while (1) {
                    if (arg_count >= arg_capacity) {
                        arg_capacity *= 2;
                        args = realloc(args, sizeof(Expression*) * arg_capacity);
                    }
                    args[arg_count++] = parser_parse_expression(parser);
                    
                    if (parser->current_token->type == TOKEN_COMMA) {
                        parser_advance(parser);
                    } else {
                        break;
                    }
                }
            }
            
            if (parser->current_token->type != TOKEN_RPAREN) {
                fprintf(stderr, "Parser error: Expected ')' after function arguments at line %d\\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip ')'
            
            Expression* expr = expression_create_func_call(name, args, arg_count);
            // Set generic type arguments if present
            expr->func_call.type_args = type_args;
            expr->func_call.type_arg_count = type_arg_count;
            free(name);
            return expr;
        } else if (parser->current_token->type == TOKEN_LBRACKET) {
            // Array indexing: arr[index]
            parser_advance(parser); // skip '['
            
            Expression* index = parser_parse_expression(parser);
            
            if (parser->current_token->type != TOKEN_RBRACKET) {
                fprintf(stderr, "Parser error: Expected ']' after array index at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip ']'
            
            Expression* expr = malloc(sizeof(Expression));
            expr->type = EXPR_ARRAY_INDEX;
            expr->array_index.array_name = name; // Takes ownership
            expr->array_index.index = index;
            return expr;
        } else if (parser->current_token->type == TOKEN_QUESTION_DOT) {
            // Phase 15: Optional chaining: object?.field
            parser_advance(parser); // skip '?.'
            
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected field name after '?.' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            char* field_name = malloc(strlen(parser->current_token->value) + 1);
            strcpy(field_name, parser->current_token->value);
            parser_advance(parser);
            
            // Create variable expression first
            Expression* var_expr = expression_create_variable(name);
            free(name);
            
            // Create optional chain expression
            Expression* expr = expression_create_optional_chain(var_expr, field_name);
            free(field_name);
            return expr;
        } else if (parser->current_token->type == TOKEN_DOT) {
            // Field access OR module qualified function call: object.field OR Module.func()
            parser_advance(parser); // skip '.'
            
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected field name after '.' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            char* field_name = malloc(strlen(parser->current_token->value) + 1);
            strcpy(field_name, parser->current_token->value);
            parser_advance(parser);
            
            // Check if it's a function call: Module.func()
            if (parser->current_token->type == TOKEN_LPAREN) {
                // Module qualified function call
                parser_advance(parser); // skip '('
                
                // Create qualified function name: Module_func
                char* qualified_name = malloc(strlen(name) + strlen(field_name) + 2);
                sprintf(qualified_name, "%s.%s", name, field_name);
                
                // Parse arguments
                int arg_count = 0;
                int arg_capacity = 4;
                Expression** args = malloc(sizeof(Expression*) * arg_capacity);
                
                if (parser->current_token->type != TOKEN_RPAREN) {
                    while (1) {
                        if (arg_count >= arg_capacity) {
                            arg_capacity *= 2;
                            args = realloc(args, sizeof(Expression*) * arg_capacity);
                        }
                        
                        args[arg_count++] = parser_parse_expression(parser);
                        
                        if (parser->current_token->type == TOKEN_COMMA) {
                            parser_advance(parser); // skip ','
                        } else {
                            break;
                        }
                    }
                }
                
                if (parser->current_token->type != TOKEN_RPAREN) {
                    fprintf(stderr, "Parser error: Expected ')' after arguments at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                parser_advance(parser); // skip ')'
                
                Expression* expr = expression_create_func_call(qualified_name, args, arg_count);
                free(name);
                free(field_name);
                free(qualified_name);
                return expr;
            } else {
                // Regular field access
                Expression* expr = expression_create_field_access(name, field_name);
                free(name);
                free(field_name);
                return expr;
            }
        } else {
            // Variable
            Expression* expr = expression_create_variable(name);
            free(name);
            return expr;
        }
    } else if (parser->current_token->type == TOKEN_LPAREN) {
        // Could be:
        // 1. Parenthesized expression: (expr)
        // 2. List literal: (expr, expr, ...) - heterojen, mutable
        // 3. Empty list: ()
        parser_advance(parser); // skip '('
        
        // Check for empty list ()
        if (parser->current_token->type == TOKEN_RPAREN) {
            parser_advance(parser); // skip ')'
            Expression* expr = malloc(sizeof(Expression));
            expr->type = EXPR_LIST_LITERAL;
            expr->list_literal.elements = NULL;
            expr->list_literal.count = 0;
            return expr;
        }
        
        Expression* first = parser_parse_expression(parser);
        
        // Check if this is a list literal (has comma)
        if (parser->current_token->type == TOKEN_COMMA) {
            // List literal: (expr, expr, ...)
            Expression* expr = malloc(sizeof(Expression));
            expr->type = EXPR_LIST_LITERAL;
            expr->list_literal.elements = malloc(sizeof(Expression*) * 10);
            expr->list_literal.count = 0;
            int capacity = 10;
            
            // Add first element
            expr->list_literal.elements[expr->list_literal.count++] = first;
            
            // Parse remaining elements
            while (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
                
                if (expr->list_literal.count >= capacity) {
                    capacity *= 2;
                    expr->list_literal.elements = realloc(expr->list_literal.elements,
                                                          sizeof(Expression*) * capacity);
                }
                expr->list_literal.elements[expr->list_literal.count++] = 
                    parser_parse_expression(parser);
            }
            
            if (parser->current_token->type != TOKEN_RPAREN) {
                fprintf(stderr, "Parser error: Expected ')' after list literal at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip ')'
            return expr;
        } else {
            // Parenthesized expression: (expr)
            if (parser->current_token->type != TOKEN_RPAREN) {
                fprintf(stderr, "Parser error: Expected ')' after expression at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip ')'
            return first;
        }
    } else if (parser->current_token->type == TOKEN_LESS) {
        // Could be:
        // 1. Tuple literal: <expr, expr, ...> - heterojen, immutable
        // 2. Empty tuple: <>
        // Note: This is only reached when < is at start of expression (not comparison)
        parser_advance(parser); // skip '<'
        
        // Check for empty tuple <>
        if (parser->current_token->type == TOKEN_GREATER) {
            parser_advance(parser); // skip '>'
            Expression* expr = malloc(sizeof(Expression));
            expr->type = EXPR_TUPLE_LITERAL;
            expr->tuple_literal.elements = NULL;
            expr->tuple_literal.count = 0;
            return expr;
        }
        
        // Parse tuple elements
        Expression* expr = malloc(sizeof(Expression));
        expr->type = EXPR_TUPLE_LITERAL;
        expr->tuple_literal.elements = malloc(sizeof(Expression*) * 10);
        expr->tuple_literal.count = 0;
        int capacity = 10;
        
        while (1) {
            if (expr->tuple_literal.count >= capacity) {
                capacity *= 2;
                expr->tuple_literal.elements = realloc(expr->tuple_literal.elements,
                                                        sizeof(Expression*) * capacity);
            }
            expr->tuple_literal.elements[expr->tuple_literal.count++] = 
                parser_parse_expression(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
            } else {
                break;
            }
        }
        
        if (parser->current_token->type != TOKEN_GREATER) {
            fprintf(stderr, "Parser error: Expected '>' after tuple literal at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip '>'
        return expr;
    } else {
        fprintf(stderr, "Parser error: Expected expression at line %d\n",
                parser->current_token->line);
        exit(1);
    }
}

// Phase 10: Parse unary expressions (& and * prefix operators)
// TTO: Also handles unary minus for negative numbers (-3.14, -5)
Expression* parser_parse_unary_expression(Parser* parser) {
    // Check for logical NOT operator: not expression
    if (parser->current_token->type == TOKEN_NOT) {
        parser_advance(parser); // skip 'not'
        Expression* operand = parser_parse_unary_expression(parser); // Recursive for multiple nots
        return expression_create_logical_not(operand);
    }
    
    // TTO: Check for unary minus operator: -expression
    if (parser->current_token->type == TOKEN_MINUS) {
        parser_advance(parser); // skip '-'
        Expression* operand = parser_parse_unary_expression(parser); // Recursive for --x
        
        // Optimization: If operand is a number literal, negate it directly
        if (operand->type == EXPR_NUMBER) {
            if (operand->has_decimal_point || operand->internal_numeric_type == INTERNAL_DOUBLE) {
                operand->double_value = -operand->double_value;
            } else {
                operand->number_value = -operand->number_value;
            }
            return operand;
        }
        
        // Otherwise, create a subtraction from 0: 0 - operand
        Expression* zero = expression_create_number(0);
        return expression_create_binary_op(zero, operand, BIN_OP_SUB);
    }
    
    // Check for address-of operator: &variable
    if (parser->current_token->type == TOKEN_AMPERSAND) {
        parser_advance(parser); // skip '&'
        Expression* operand = parser_parse_primary_expression(parser);
        return expression_create_address_of(operand);
    }
    
    // Check for dereference operator: *pointer
    if (parser->current_token->type == TOKEN_MULTIPLY) {
        parser_advance(parser); // skip '*'
        Expression* operand = parser_parse_primary_expression(parser);
        return expression_create_dereference(operand);
    }
    
    // Otherwise, parse as primary expression
    return parser_parse_primary_expression(parser);
}

Expression* parser_parse_expression(Parser* parser) {
    // Parse OR level (lowest precedence)
    Expression* left = parser_parse_and_expression(parser);
    
    while (parser->current_token->type == TOKEN_OR) {
        parser_advance(parser);
        Expression* right = parser_parse_and_expression(parser);
        left = expression_create_logical_or(left, right);
    }
    
    return left;
}

// Parse AND expressions (higher precedence than OR)
Expression* parser_parse_and_expression(Parser* parser) {
    Expression* left = parser_parse_comparison_expression(parser);
    
    while (parser->current_token->type == TOKEN_AND) {
        parser_advance(parser);
        Expression* right = parser_parse_comparison_expression(parser);
        left = expression_create_logical_and(left, right);
    }
    
    return left;
}

// Parse comparison and arithmetic expressions
Expression* parser_parse_comparison_expression(Parser* parser) {
    Expression* left = parser_parse_arithmetic_expression(parser);
    
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
        Expression* right = parser_parse_arithmetic_expression(parser);
        left = expression_create_comparison(left, right, op);
    }
    
    // Phase 15: Check for null coalescing operator (??)
    while (parser->current_token->type == TOKEN_NULL_COALESCE) {
        parser_advance(parser);  // skip '??'
        Expression* right = parser_parse_arithmetic_expression(parser);
        left = expression_create_null_coalesce(left, right);
    }
    
    // Check for ternary operator (? :)
    if (parser->current_token->type == TOKEN_QUESTION) {
        parser_advance(parser);
        Expression* true_expr = parser_parse_expression(parser);
        
        if (parser->current_token->type != TOKEN_COLON) {
            fprintf(stderr, "Parser error: Expected ':' in ternary expression at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
        
        Expression* false_expr = parser_parse_expression(parser);
        return expression_create_ternary(left, true_expr, false_expr);
    }
    
    return left;
}

// Parse arithmetic expressions (+, -, *, /)
Expression* parser_parse_arithmetic_expression(Parser* parser) {
    Expression* left = parser_parse_unary_expression(parser);
    
    // Check for arithmetic operators (+, -, *, /) in a loop for left-associativity
    while (1) {
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
            Expression* right = parser_parse_unary_expression(parser);
            left = expression_create_binary_op(left, right, bin_op);
        } else {
            break;
        }
    }
    
    return left;
}

Declaration* parser_parse_declaration(Parser* parser) {
    Declaration* decl = malloc(sizeof(Declaration));
    decl->struct_name = NULL;
    decl->is_array = 0;
    decl->is_list = 0;               // Phase 21: Dynamic list flag
    decl->array_size = 0;
    decl->array_init = NULL;
    decl->array_init_count = 0;
    decl->is_pointer = 0;
    decl->is_nullable = 0;
    decl->is_union = 0;
    decl->union_types = NULL;
    decl->union_count = 0;
    decl->init_value = NULL;
    decl->is_exported = 1;  // Default: exported (public)
    decl->struct_type_args = NULL;     // Phase 13: Generic type args
    decl->struct_type_arg_count = 0;   // Phase 13: Generic type arg count
    
    // Type keyword or struct name
    if (parser->current_token->type == TOKEN_NUMERIC) {
        decl->type = TYPE_NUMERIC;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_DECIMAL) {
        decl->type = TYPE_DECIMAL;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        decl->type = TYPE_BOOLEAN;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_TEXT) {
        decl->type = TYPE_STRING;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        // Struct instance (e.g., Person p or Box<numeric> b)
        decl->struct_name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(decl->struct_name, parser->current_token->value);
        decl->type = TYPE_NUMERIC; // Placeholder, actual type is struct
        parser_advance(parser);
        
        // Phase 13: Check for generic type arguments: Box<numeric>
        if (parser->current_token->type == TOKEN_LESS) {
            parser_advance(parser); // skip '<'
            
            decl->struct_type_args = malloc(sizeof(VarType) * 10);
            int capacity = 10;
            
            while (1) {
                VarType type_arg;
                if (parser->current_token->type == TOKEN_NUMERIC) {
                    type_arg = TYPE_NUMERIC;
                } else if (parser->current_token->type == TOKEN_DECIMAL) {
                    type_arg = TYPE_DECIMAL;
                } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                    type_arg = TYPE_BOOLEAN;
                } else if (parser->current_token->type == TOKEN_TEXT) {
                    type_arg = TYPE_STRING;
                } else {
                    fprintf(stderr, "Parser error: Expected type in generic arguments at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                parser_advance(parser);
                
                if (decl->struct_type_arg_count >= capacity) {
                    capacity *= 2;
                    decl->struct_type_args = realloc(decl->struct_type_args, sizeof(VarType) * capacity);
                }
                decl->struct_type_args[decl->struct_type_arg_count++] = type_arg;
                
                if (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser); // skip ','
                } else {
                    break;
                }
            }
            
            if (parser->current_token->type != TOKEN_GREATER) {
                fprintf(stderr, "Parser error: Expected '>' after generic arguments at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip '>'
        }
    } else {
        fprintf(stderr, "Parser error: Expected type keyword at line %d\n", 
                parser->current_token->line);
        exit(1);
    }
    
    // Phase 11: Check for union type: numeric | string | boolean
    if (parser->current_token->type == TOKEN_PIPE) {
        decl->is_union = 1;
        int capacity = 10;
        decl->union_types = malloc(sizeof(VarType) * capacity);
        decl->union_count = 0;
        
        // First type already parsed
        decl->union_types[decl->union_count++] = decl->type;
        
        // Parse remaining types
        while (parser->current_token->type == TOKEN_PIPE) {
            parser_advance(parser); // skip '|'
            
            VarType union_type;
            if (parser->current_token->type == TOKEN_NUMERIC) {
                union_type = TYPE_NUMERIC;
                parser_advance(parser);
            } else if (parser->current_token->type == TOKEN_DECIMAL) {
                union_type = TYPE_DECIMAL;
                parser_advance(parser);
            } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                union_type = TYPE_BOOLEAN;
                parser_advance(parser);
            } else if (parser->current_token->type == TOKEN_TEXT) {
                union_type = TYPE_STRING;
                parser_advance(parser);
            } else {
                fprintf(stderr, "Parser error: Expected type after '|' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            if (decl->union_count >= capacity) {
                capacity *= 2;
                decl->union_types = realloc(decl->union_types, sizeof(VarType) * capacity);
            }
            
            decl->union_types[decl->union_count++] = union_type;
        }
    }
    
    // Phase 10: Check for pointer type: *
    if (parser->current_token->type == TOKEN_MULTIPLY) {
        decl->is_pointer = 1;
        parser_advance(parser); // skip '*'
    }
    
    // Phase 10: Check for nullable type: ?
    if (parser->current_token->type == TOKEN_QUESTION) {
        decl->is_nullable = 1;
        parser_advance(parser); // skip '?'
    }
    
    // Check for array type: [] or list type: list()
    if (parser->current_token->type == TOKEN_LBRACKET) {
        decl->is_array = 1;
        parser_advance(parser); // skip '['
        
        // Check for sized array: [size]
        if (parser->current_token->type == TOKEN_NUMBER) {
            decl->array_size = atoi(parser->current_token->value);
            parser_advance(parser);
        }
        
        if (parser->current_token->type != TOKEN_RBRACKET) {
            fprintf(stderr, "Parser error: Expected ']' after '[' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip ']'
    } else if (parser->current_token->type == TOKEN_LIST) {
        // Phase 21: List type - numeric list() x
        parser_advance(parser); // skip 'list'
        
        if (parser->current_token->type != TOKEN_LPAREN) {
            fprintf(stderr, "Parser error: Expected '(' after 'list' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip '('
        
        if (parser->current_token->type != TOKEN_RPAREN) {
            fprintf(stderr, "Parser error: Expected ')' after 'list(' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip ')'
        
        decl->is_list = 1;
        decl->is_array = 1;  // Lists are dynamic arrays internally
        decl->array_size = 0; // Dynamic size
    }
    
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
    if (parser->current_token->type == TOKEN_ASSIGN) {
        parser_advance(parser);
        
        // Phase 21: Check for list() initialization
        if (decl->is_list && parser->current_token->type == TOKEN_LIST) {
            parser_advance(parser); // skip 'list'
            if (parser->current_token->type == TOKEN_LPAREN) {
                parser_advance(parser); // skip '('
                if (parser->current_token->type == TOKEN_RPAREN) {
                    parser_advance(parser); // skip ')'
                    // Empty list initialization - no init_value needed
                    // The list is already marked as dynamic via is_list
                } else {
                    fprintf(stderr, "Parser error: Expected ')' after 'list(' at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
            } else {
                fprintf(stderr, "Parser error: Expected '(' after 'list' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
        }
        // Check for array literal: [1, 2, 3]
        else if (decl->is_array && parser->current_token->type == TOKEN_LBRACKET) {
            parser_advance(parser); // skip '['
            
            // Parse array elements
            int capacity = 10;
            decl->array_init = malloc(sizeof(Expression*) * capacity);
            decl->array_init_count = 0;
            
            if (parser->current_token->type != TOKEN_RBRACKET) {
                while (1) {
                    if (decl->array_init_count >= capacity) {
                        capacity *= 2;
                        decl->array_init = realloc(decl->array_init, sizeof(Expression*) * capacity);
                    }
                    
                    decl->array_init[decl->array_init_count++] = parser_parse_expression(parser);
                    
                    if (parser->current_token->type == TOKEN_COMMA) {
                        parser_advance(parser);
                    } else {
                        break;
                    }
                }
            }
            
            if (parser->current_token->type != TOKEN_RBRACKET) {
                fprintf(stderr, "Parser error: Expected ']' after array elements at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip ']'
            
            // Set array size from literal if not specified
            if (decl->array_size == 0) {
                decl->array_size = decl->array_init_count;
            }
        } else {
            decl->init_value = parser_parse_expression(parser);
        }
    }
    
    return decl;
}

Statement* parser_parse_statement(Parser* parser);  // Forward declaration
Statement* parser_parse_try_catch(Parser* parser);  // Forward declaration
Statement* parser_parse_import_statement(Parser* parser);  // Forward declaration
Statement* parser_parse_module_definition(Parser* parser);  // Forward declaration
Statement* parser_parse_debug_statement(Parser* parser);  // Forward declaration

// Helper function to parse else-if chain recursively
Statement* parser_parse_else_if_chain(Parser* parser) {
    Statement* nested_if = malloc(sizeof(Statement));
    nested_if->type = STMT_IF;
    
    // Parse condition (we're already past 'else if')
    nested_if->if_stmt.condition = parser_parse_expression(parser);
    
    // Expect 'then'
    if (parser->current_token->type != TOKEN_THEN) {
        fprintf(stderr, "Parser error: Expected 'then' after else if condition at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse then body
    nested_if->if_stmt.then_body = malloc(sizeof(Statement*) * 10);
    nested_if->if_stmt.then_count = 0;
    int then_capacity = 10;
    
    while (parser->current_token->type != TOKEN_ELSE && 
           parser->current_token->type != TOKEN_ELSE_IF &&
           parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* body_stmt = parser_parse_statement(parser);
        
        if (nested_if->if_stmt.then_count >= then_capacity) {
            then_capacity *= 2;
            nested_if->if_stmt.then_body = realloc(nested_if->if_stmt.then_body,
                                                  sizeof(Statement*) * then_capacity);
        }
        nested_if->if_stmt.then_body[nested_if->if_stmt.then_count++] = body_stmt;
    }
    
    // Check for more else if or final else
    nested_if->if_stmt.else_body = NULL;
    nested_if->if_stmt.else_count = 0;
    
    if (parser->current_token->type == TOKEN_ELSE_IF) {
        parser_advance(parser); // Skip 'else if'
        
        // Recursively parse the next else if
        nested_if->if_stmt.else_body = malloc(sizeof(Statement*) * 1);
        nested_if->if_stmt.else_count = 1;
        nested_if->if_stmt.else_body[0] = parser_parse_else_if_chain(parser);
        
    } else if (parser->current_token->type == TOKEN_ELSE) {
        parser_advance(parser); // Skip 'else'
        
        // Parse final else body
        nested_if->if_stmt.else_body = malloc(sizeof(Statement*) * 10);
        int else_capacity = 10;
        
        while (parser->current_token->type != TOKEN_END &&
               parser->current_token->type != TOKEN_EOF) {
            
            Statement* body_stmt = parser_parse_statement(parser);
            
            if (nested_if->if_stmt.else_count >= else_capacity) {
                else_capacity *= 2;
                nested_if->if_stmt.else_body = realloc(nested_if->if_stmt.else_body,
                                                      sizeof(Statement*) * else_capacity);
            }
            nested_if->if_stmt.else_body[nested_if->if_stmt.else_count++] = body_stmt;
        }
    }
    
    return nested_if;
}

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
           parser->current_token->type != TOKEN_ELSE_IF &&
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
    
    // Check for else if or else
    stmt->if_stmt.else_body = NULL;
    stmt->if_stmt.else_count = 0;
    
    if (parser->current_token->type == TOKEN_ELSE_IF) {
        parser_advance(parser); // Skip 'else if'
        
        // Use helper function to handle else if chain recursively
        stmt->if_stmt.else_body = malloc(sizeof(Statement*) * 1);
        stmt->if_stmt.else_count = 1;
        stmt->if_stmt.else_body[0] = parser_parse_else_if_chain(parser);
        
    } else if (parser->current_token->type == TOKEN_ELSE) {
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
    
    // Skip 'for'
    parser_advance(parser);
    
    // Parse loop variable
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected identifier after 'for' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    char* var_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(var_name, parser->current_token->value);
    parser_advance(parser);
    
    // Phase 14: Check for 'in' (for-in loop) vs '=' (traditional for loop)
    if (parser->current_token->type == TOKEN_IN) {
        // for x in iterable do ... end for
        stmt->type = STMT_FOR_IN;
        stmt->for_in.var_name = var_name;
        stmt->for_in.var_type = TYPE_NUMERIC; // Default, will be inferred from iterator
        
        parser_advance(parser); // skip 'in'
        
        // Parse iterable expression (generator call or array)
        stmt->for_in.iterable = parser_parse_expression(parser);
        
        // Optional 'do' keyword
        if (parser->current_token->type == TOKEN_DO) {
            parser_advance(parser);
        }
        
        // Parse body
        stmt->for_in.body = malloc(sizeof(Statement*) * 10);
        stmt->for_in.body_count = 0;
        int body_capacity = 10;
        
        while (parser->current_token->type != TOKEN_END &&
               parser->current_token->type != TOKEN_EOF) {
            
            Statement* body_stmt = parser_parse_statement(parser);
            
            if (stmt->for_in.body_count >= body_capacity) {
                body_capacity *= 2;
                stmt->for_in.body = realloc(stmt->for_in.body,
                                            sizeof(Statement*) * body_capacity);
            }
            stmt->for_in.body[stmt->for_in.body_count++] = body_stmt;
        }
        
        // Expect 'end' 'for'
        if (parser->current_token->type != TOKEN_END) {
            fprintf(stderr, "Parser error: Expected 'end' after for-in loop body at line %d\n",
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
    
    // Traditional for loop: for i = start to end [step s] do ... end for
    stmt->type = STMT_FOR;
    stmt->for_stmt.var_name = var_name;
    
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

Statement* parser_parse_do_while_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_DO_WHILE;
    
    // Skip 'do'
    parser_advance(parser);
    
    // Parse body
    stmt->do_while_stmt.body = malloc(sizeof(Statement*) * 10);
    stmt->do_while_stmt.body_count = 0;
    int body_capacity = 10;
    
    while (parser->current_token->type != TOKEN_WHILE &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* body_stmt = parser_parse_statement(parser);
        
        if (stmt->do_while_stmt.body_count >= body_capacity) {
            body_capacity *= 2;
            stmt->do_while_stmt.body = realloc(stmt->do_while_stmt.body,
                                              sizeof(Statement*) * body_capacity);
        }
        stmt->do_while_stmt.body[stmt->do_while_stmt.body_count++] = body_stmt;
    }
    
    // Expect 'while'
    if (parser->current_token->type != TOKEN_WHILE) {
        fprintf(stderr, "Parser error: Expected 'while' after do block at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse condition
    stmt->do_while_stmt.condition = parser_parse_expression(parser);
    
    return stmt;
}

Statement* parser_parse_switch_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_SWITCH;
    
    // Skip 'switch'
    parser_advance(parser);
    
    // Parse switch expression
    stmt->switch_stmt.value = parser_parse_expression(parser);
    
    // Initialize cases array
    stmt->switch_stmt.cases = malloc(sizeof(SwitchCase*) * 10);
    stmt->switch_stmt.case_count = 0;
    int case_capacity = 10;
    
    stmt->switch_stmt.default_body = NULL;
    stmt->switch_stmt.default_count = 0;
    
    // Parse cases until we hit 'end'
    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        if (parser->current_token->type == TOKEN_CASE) {
            parser_advance(parser); // Skip 'case'
            
            // Allocate new case
            SwitchCase* switch_case = malloc(sizeof(SwitchCase));
            switch_case->value = NULL;
            switch_case->guard = NULL;
            switch_case->range_start = NULL;
            switch_case->range_end = NULL;
            switch_case->is_type_check = 0;
            switch_case->type_name = NULL;
            
            // Check for type matching: case is null
            if (parser->current_token->type == TOKEN_IDENTIFIER &&
                strcmp(parser->current_token->value, "is") == 0) {
                parser_advance(parser); // Skip 'is'
                switch_case->is_type_check = 1;
                
                // Check for "null" keyword
                if (parser->current_token->type == TOKEN_NULL) {
                    switch_case->type_name = malloc(5);
                    strcpy(switch_case->type_name, "null");
                    parser_advance(parser);
                } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
                    switch_case->type_name = malloc(strlen(parser->current_token->value) + 1);
                    strcpy(switch_case->type_name, parser->current_token->value);
                    parser_advance(parser);
                } else {
                    fprintf(stderr, "Parser error: Expected type name or 'null' after 'is' at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
            } else {
                // Parse case value (could be start of range)
                switch_case->value = parser_parse_expression(parser);
                
                // Check for range: case 1 to 10
                if (parser->current_token->type == TOKEN_TO) {
                    // Found 'to' for range
                    parser_advance(parser); // Skip 'to'
                    
                    switch_case->range_start = switch_case->value;
                    switch_case->range_end = parser_parse_expression(parser);
                    switch_case->value = NULL; // Clear value since we're using range
                }
            }
            
            // Check for guard condition: case x if x > 10
            if (parser->current_token->type == TOKEN_IF) {
                parser_advance(parser); // Skip 'if'
                switch_case->guard = parser_parse_expression(parser);
            }
            
            switch_case->body = malloc(sizeof(Statement*) * 10);
            switch_case->body_count = 0;
            int body_capacity = 10;
            
            // Parse case body until next case, default, or end
            while (parser->current_token->type != TOKEN_CASE &&
                   parser->current_token->type != TOKEN_DEFAULT &&
                   parser->current_token->type != TOKEN_END &&
                   parser->current_token->type != TOKEN_EOF) {
                
                Statement* body_stmt = parser_parse_statement(parser);
                
                if (switch_case->body_count >= body_capacity) {
                    body_capacity *= 2;
                    switch_case->body = realloc(switch_case->body,
                                               sizeof(Statement*) * body_capacity);
                }
                switch_case->body[switch_case->body_count++] = body_stmt;
            }
            
            // Add case to cases array
            if (stmt->switch_stmt.case_count >= case_capacity) {
                case_capacity *= 2;
                stmt->switch_stmt.cases = realloc(stmt->switch_stmt.cases,
                                                  sizeof(SwitchCase*) * case_capacity);
            }
            stmt->switch_stmt.cases[stmt->switch_stmt.case_count++] = switch_case;
            
        } else if (parser->current_token->type == TOKEN_DEFAULT) {
            parser_advance(parser); // Skip 'default'
            
            // Parse default body
            stmt->switch_stmt.default_body = malloc(sizeof(Statement*) * 10);
            int default_capacity = 10;
            
            while (parser->current_token->type != TOKEN_CASE &&
                   parser->current_token->type != TOKEN_DEFAULT &&
                   parser->current_token->type != TOKEN_END &&
                   parser->current_token->type != TOKEN_EOF) {
                
                Statement* body_stmt = parser_parse_statement(parser);
                
                if (stmt->switch_stmt.default_count >= default_capacity) {
                    default_capacity *= 2;
                    stmt->switch_stmt.default_body = realloc(stmt->switch_stmt.default_body,
                                                            sizeof(Statement*) * default_capacity);
                }
                stmt->switch_stmt.default_body[stmt->switch_stmt.default_count++] = body_stmt;
            }
        } else {
            fprintf(stderr, "Parser error: Expected 'case' or 'default' in switch at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
    }
    
    // Expect 'end' 'switch'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after switch block at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_SWITCH) {
        fprintf(stderr, "Parser error: Expected 'switch' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

Statement* parser_parse_try_catch(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_TRY_CATCH;
    
    // Skip 'try'
    parser_advance(parser);
    
    // Parse try body
    stmt->try_catch.try_body = malloc(sizeof(Statement*) * 10);
    stmt->try_catch.try_count = 0;
    int try_capacity = 10;
    
    while (parser->current_token->type != TOKEN_CATCH &&
           parser->current_token->type != TOKEN_FINALLY &&
           parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* body_stmt = parser_parse_statement(parser);
        
        if (stmt->try_catch.try_count >= try_capacity) {
            try_capacity *= 2;
            stmt->try_catch.try_body = realloc(stmt->try_catch.try_body,
                                               sizeof(Statement*) * try_capacity);
        }
        stmt->try_catch.try_body[stmt->try_catch.try_count++] = body_stmt;
    }
    
    // Parse multiple catch blocks
    stmt->try_catch.catch_blocks = NULL;
    stmt->try_catch.catch_count = 0;
    int catch_capacity = 2;
    
    while (parser->current_token->type == TOKEN_CATCH) {
        parser_advance(parser); // skip 'catch'
        
        // Allocate catch blocks array
        if (stmt->try_catch.catch_blocks == NULL) {
            stmt->try_catch.catch_blocks = malloc(sizeof(*stmt->try_catch.catch_blocks) * catch_capacity);
        }
        
        if (stmt->try_catch.catch_count >= catch_capacity) {
            catch_capacity *= 2;
            stmt->try_catch.catch_blocks = realloc(stmt->try_catch.catch_blocks,
                                                    sizeof(*stmt->try_catch.catch_blocks) * catch_capacity);
        }
        
        int catch_idx = stmt->try_catch.catch_count++;
        stmt->try_catch.catch_blocks[catch_idx].exception_type = NULL;
        stmt->try_catch.catch_blocks[catch_idx].exception_var = NULL;
        
        // Parse exception type (optional)
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Could be: catch ErrorType e OR catch e
            // Look ahead to distinguish
            char* first_id = malloc(strlen(parser->current_token->value) + 1);
            strcpy(first_id, parser->current_token->value);
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_IDENTIFIER) {
                // catch ErrorType e
                stmt->try_catch.catch_blocks[catch_idx].exception_type = first_id;
                stmt->try_catch.catch_blocks[catch_idx].exception_var = 
                    malloc(strlen(parser->current_token->value) + 1);
                strcpy(stmt->try_catch.catch_blocks[catch_idx].exception_var, 
                       parser->current_token->value);
                parser_advance(parser);
            } else {
                // catch e (no type)
                stmt->try_catch.catch_blocks[catch_idx].exception_var = first_id;
            }
        }
        
        // Parse catch body
        stmt->try_catch.catch_blocks[catch_idx].body = malloc(sizeof(Statement*) * 10);
        stmt->try_catch.catch_blocks[catch_idx].body_count = 0;
        int body_capacity = 10;
        
        while (parser->current_token->type != TOKEN_CATCH &&
               parser->current_token->type != TOKEN_FINALLY &&
               parser->current_token->type != TOKEN_END &&
               parser->current_token->type != TOKEN_EOF) {
            
            Statement* body_stmt = parser_parse_statement(parser);
            
            if (stmt->try_catch.catch_blocks[catch_idx].body_count >= body_capacity) {
                body_capacity *= 2;
                stmt->try_catch.catch_blocks[catch_idx].body = 
                    realloc(stmt->try_catch.catch_blocks[catch_idx].body,
                            sizeof(Statement*) * body_capacity);
            }
            stmt->try_catch.catch_blocks[catch_idx].body[
                stmt->try_catch.catch_blocks[catch_idx].body_count++] = body_stmt;
        }
    }
    
    // Parse finally block (optional)
    stmt->try_catch.finally_body = NULL;
    stmt->try_catch.finally_count = 0;
    
    if (parser->current_token->type == TOKEN_FINALLY) {
        parser_advance(parser); // skip 'finally'
        
        stmt->try_catch.finally_body = malloc(sizeof(Statement*) * 10);
        int finally_capacity = 10;
        
        while (parser->current_token->type != TOKEN_END &&
               parser->current_token->type != TOKEN_EOF) {
            
            Statement* body_stmt = parser_parse_statement(parser);
            
            if (stmt->try_catch.finally_count >= finally_capacity) {
                finally_capacity *= 2;
                stmt->try_catch.finally_body = realloc(stmt->try_catch.finally_body,
                                                       sizeof(Statement*) * finally_capacity);
            }
            stmt->try_catch.finally_body[stmt->try_catch.finally_count++] = body_stmt;
        }
    }
    
    // Expect 'end'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after try-catch at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser); // skip 'end'
    
    // Expect 'try' (end try)
    if (parser->current_token->type != TOKEN_TRY) {
        fprintf(stderr, "Parser error: Expected 'try' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

// Parse import statement: import math or import string as str
Statement* parser_parse_import_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_IMPORT;
    
    parser_advance(parser); // skip 'import'
    
    // Expect module name (identifier)
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected module name after 'import' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    stmt->import_stmt.module_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->import_stmt.module_name, parser->current_token->value);
    parser_advance(parser);
    
    // Check for 'as' alias
    stmt->import_stmt.alias = NULL;
    if (parser->current_token->type == TOKEN_AS) {
        parser_advance(parser); // skip 'as'
        
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Parser error: Expected alias name after 'as' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        
        stmt->import_stmt.alias = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->import_stmt.alias, parser->current_token->value);
        parser_advance(parser);
    }
    
    return stmt;
}

// Parse module definition: module MyModule ... end module
Statement* parser_parse_module_definition(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_MODULE_DEF;
    
    parser_advance(parser); // skip 'module'
    
    // Expect module name (identifier)
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected module name after 'module' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    stmt->module_def.module_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->module_def.module_name, parser->current_token->value);
    parser_advance(parser);
    
    // Parse module body (functions, structs, etc.) until 'end module'
    int body_capacity = 10;
    stmt->module_def.body = malloc(sizeof(Statement*) * body_capacity);
    stmt->module_def.body_count = 0;
    
    while (parser->current_token->type != TOKEN_END && 
           parser->current_token->type != TOKEN_EOF) {
        
        if (stmt->module_def.body_count >= body_capacity) {
            body_capacity *= 2;
            stmt->module_def.body = realloc(stmt->module_def.body,
                                           sizeof(Statement*) * body_capacity);
        }
        
        stmt->module_def.body[stmt->module_def.body_count++] = parser_parse_statement(parser);
    }
    
    // Expect 'end' 'module'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after module body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_MODULE) {
        fprintf(stderr, "Parser error: Expected 'module' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

// Phase 16: Parse operator overloading definition
// Syntax: operator +(Vector a, Vector b) returns Vector ... end operator
Statement* parser_parse_operator_definition(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_OPERATOR_DEF;
    
    parser_advance(parser); // skip 'operator'
    
    // Get operator symbol (+, -, *, /, ==, !=, <, >, <=, >=, etc.)
    // Can be a single or double char operator
    char* op = NULL;
    if (parser->current_token->type == TOKEN_PLUS) {
        op = "+";
    } else if (parser->current_token->type == TOKEN_MINUS) {
        op = "-";
    } else if (parser->current_token->type == TOKEN_MULTIPLY) {
        op = "*";
    } else if (parser->current_token->type == TOKEN_DIVIDE) {
        op = "/";
    } else if (parser->current_token->type == TOKEN_EQUAL) {
        op = "==";
    } else if (parser->current_token->type == TOKEN_NOT_EQUAL) {
        op = "!=";
    } else if (parser->current_token->type == TOKEN_LESS) {
        op = "<";
    } else if (parser->current_token->type == TOKEN_GREATER) {
        op = ">";
    } else if (parser->current_token->type == TOKEN_LESS_EQUAL) {
        op = "<=";
    } else if (parser->current_token->type == TOKEN_GREATER_EQUAL) {
        op = ">=";
    } else {
        fprintf(stderr, "Parser error: Expected operator symbol after 'operator' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    stmt->operator_def.op = malloc(strlen(op) + 1);
    strcpy(stmt->operator_def.op, op);
    parser_advance(parser);
    
    // Expect '('
    if (parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Parser error: Expected '(' after operator symbol at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // First parameter: TypeName param_name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected type name in operator definition at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    stmt->operator_def.type_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->operator_def.type_name, parser->current_token->value);
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected parameter name in operator definition at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    stmt->operator_def.param1_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->operator_def.param1_name, parser->current_token->value);
    parser_advance(parser);
    
    // Check for comma (binary operator) or close paren (unary operator)
    stmt->operator_def.param2_name = NULL;
    if (parser->current_token->type == TOKEN_COMMA) {
        parser_advance(parser); // skip ','
        
        // Second parameter: TypeName param_name
        // Skip the type name (should be same as first)
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Parser error: Expected type name for second parameter at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip type name
        
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Parser error: Expected parameter name for second parameter at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        
        stmt->operator_def.param2_name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->operator_def.param2_name, parser->current_token->value);
        parser_advance(parser);
    }
    
    // Expect ')'
    if (parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Parser error: Expected ')' after operator parameters at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Expect 'returns' or 'döndürür'
    if (parser->current_token->type != TOKEN_RETURNS) {
        fprintf(stderr, "Parser error: Expected 'returns' after operator parameters at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Return type
    if (parser->current_token->type == TOKEN_NUMERIC) {
        stmt->operator_def.return_type = TYPE_NUMERIC;
    } else if (parser->current_token->type == TOKEN_TEXT) {
        stmt->operator_def.return_type = TYPE_STRING;
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        stmt->operator_def.return_type = TYPE_BOOLEAN;
    } else if (parser->current_token->type == TOKEN_DECIMAL) {
        stmt->operator_def.return_type = TYPE_DECIMAL;
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        // Custom type - for now treat as numeric (struct support later)
        stmt->operator_def.return_type = TYPE_NUMERIC;
    } else {
        fprintf(stderr, "Parser error: Expected return type after 'returns' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse operator body until 'end operator'
    int body_capacity = 10;
    stmt->operator_def.body = malloc(sizeof(Statement*) * body_capacity);
    stmt->operator_def.body_count = 0;
    
    while (parser->current_token->type != TOKEN_END && 
           parser->current_token->type != TOKEN_EOF) {
        
        if (stmt->operator_def.body_count >= body_capacity) {
            body_capacity *= 2;
            stmt->operator_def.body = realloc(stmt->operator_def.body,
                                             sizeof(Statement*) * body_capacity);
        }
        
        stmt->operator_def.body[stmt->operator_def.body_count++] = parser_parse_statement(parser);
    }
    
    // Expect 'end' 'operator'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after operator body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_OPERATOR) {
        fprintf(stderr, "Parser error: Expected 'operator' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

// Phase 17: Parse pattern matching statement
// Syntax: match value
//           case 1 => ...
//           case 2, 3 => ...
//           case 4 to 10 => ...
//           case _ => ...
//         end match
Statement* parser_parse_match_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_MATCH;
    
    parser_advance(parser); // skip 'match'
    
    // Parse the value to match
    stmt->match_stmt.value = parser_parse_expression(parser);
    
    // Initialize cases array
    int case_capacity = 10;
    stmt->match_stmt.cases = malloc(sizeof(*stmt->match_stmt.cases) * case_capacity);
    stmt->match_stmt.case_count = 0;
    
    // Parse cases until 'end match'
    while (parser->current_token->type == TOKEN_CASE) {
        if (stmt->match_stmt.case_count >= case_capacity) {
            case_capacity *= 2;
            stmt->match_stmt.cases = realloc(stmt->match_stmt.cases,
                                            sizeof(*stmt->match_stmt.cases) * case_capacity);
        }
        
        int idx = stmt->match_stmt.case_count;
        stmt->match_stmt.cases[idx].patterns = NULL;
        stmt->match_stmt.cases[idx].pattern_count = 0;
        stmt->match_stmt.cases[idx].is_wildcard = 0;
        stmt->match_stmt.cases[idx].is_range = 0;
        stmt->match_stmt.cases[idx].range_start = NULL;
        stmt->match_stmt.cases[idx].range_end = NULL;
        stmt->match_stmt.cases[idx].body = NULL;
        stmt->match_stmt.cases[idx].body_count = 0;
        
        parser_advance(parser); // skip 'case'
        
        // Check for wildcard pattern (_)
        if (parser->current_token->type == TOKEN_UNDERSCORE) {
            stmt->match_stmt.cases[idx].is_wildcard = 1;
            parser_advance(parser);
        } else {
            // Parse pattern(s) - can be comma-separated or range
            int pattern_capacity = 5;
            stmt->match_stmt.cases[idx].patterns = malloc(sizeof(Expression*) * pattern_capacity);
            
            // First pattern
            Expression* first_pattern = parser_parse_expression(parser);
            
            // Check for range pattern (to keyword)
            if (parser->current_token->type == TOKEN_TO) {
                stmt->match_stmt.cases[idx].is_range = 1;
                stmt->match_stmt.cases[idx].range_start = first_pattern;
                parser_advance(parser); // skip 'to'
                stmt->match_stmt.cases[idx].range_end = parser_parse_expression(parser);
            } else {
                // Regular pattern(s)
                stmt->match_stmt.cases[idx].patterns[0] = first_pattern;
                stmt->match_stmt.cases[idx].pattern_count = 1;
                
                // Check for comma-separated patterns
                while (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser); // skip ','
                    
                    if (stmt->match_stmt.cases[idx].pattern_count >= pattern_capacity) {
                        pattern_capacity *= 2;
                        stmt->match_stmt.cases[idx].patterns = realloc(
                            stmt->match_stmt.cases[idx].patterns,
                            sizeof(Expression*) * pattern_capacity);
                    }
                    
                    stmt->match_stmt.cases[idx].patterns[stmt->match_stmt.cases[idx].pattern_count++] =
                        parser_parse_expression(parser);
                }
            }
        }
        
        // Expect '=>'
        if (parser->current_token->type != TOKEN_ARROW) {
            fprintf(stderr, "Parser error: Expected '=>' after pattern at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
        
        // Parse case body - single statement or multiple until next case/end
        int body_capacity = 10;
        stmt->match_stmt.cases[idx].body = malloc(sizeof(Statement*) * body_capacity);
        
        while (parser->current_token->type != TOKEN_CASE &&
               parser->current_token->type != TOKEN_END &&
               parser->current_token->type != TOKEN_EOF) {
            
            if (stmt->match_stmt.cases[idx].body_count >= body_capacity) {
                body_capacity *= 2;
                stmt->match_stmt.cases[idx].body = realloc(
                    stmt->match_stmt.cases[idx].body,
                    sizeof(Statement*) * body_capacity);
            }
            
            stmt->match_stmt.cases[idx].body[stmt->match_stmt.cases[idx].body_count++] =
                parser_parse_statement(parser);
        }
        
        stmt->match_stmt.case_count++;
    }
    
    // Expect 'end' 'match'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after match cases at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_MATCH) {
        fprintf(stderr, "Parser error: Expected 'match' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

// Phase 18: State Management
Statement* parser_parse_state_statement(Parser* parser, int is_shared) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_STATE_DECL;
    stmt->state_decl.is_shared = is_shared;
    stmt->state_decl.initial_value = NULL;
    
    if (is_shared) {
        parser_advance(parser); // skip 'shared'
    }
    parser_advance(parser); // skip 'state'
    
    // Parse type
    if (parser->current_token->type == TOKEN_NUMERIC) {
        stmt->state_decl.type = TYPE_NUMERIC;
    } else if (parser->current_token->type == TOKEN_TEXT) {
        stmt->state_decl.type = TYPE_STRING;
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        stmt->state_decl.type = TYPE_BOOLEAN;
    } else if (parser->current_token->type == TOKEN_DECIMAL) {
        stmt->state_decl.type = TYPE_DECIMAL;
    } else {
        fprintf(stderr, "Parser error: Expected type after 'state' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse state name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected state name at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    stmt->state_decl.name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->state_decl.name, parser->current_token->value);
    parser_advance(parser);
    
    // Optional initial value
    if (parser->current_token->type == TOKEN_ASSIGN) {
        parser_advance(parser); // skip '='
        stmt->state_decl.initial_value = parser_parse_expression(parser);
    }
    
    return stmt;
}

Statement* parser_parse_debug_statement(Parser* parser) {
    parser_advance(parser); // skip 'debug'
    
    Statement* stmt = malloc(sizeof(Statement));
    
    // Check what debug statement it is
    if (parser->current_token->type == TOKEN_IDENTIFIER) {
        // debug mylabel - Label definition
        stmt->type = STMT_DEBUG_LABEL;
        stmt->debug_label.label_name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->debug_label.label_name, parser->current_token->value);
        parser_advance(parser);
    }
    else if (parser->current_token->type == TOKEN_GOTO) {
        // debug goto mylabel
        stmt->type = STMT_DEBUG_GOTO;
        parser_advance(parser); // skip 'goto'
        
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Parser error: Expected label name after debug goto at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        
        stmt->debug_goto.target_label = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->debug_goto.target_label, parser->current_token->value);
        parser_advance(parser);
    }
    else if (parser->current_token->type == TOKEN_IF) {
        // debug if condition then ... end if
        stmt->type = STMT_DEBUG_IF;
        parser_advance(parser); // skip 'if'
        
        stmt->debug_if.condition = parser_parse_expression(parser);
        
        if (parser->current_token->type != TOKEN_THEN) {
            fprintf(stderr, "Parser error: Expected 'then' after debug if condition at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip 'then'
        
        // Parse debug if body
        stmt->debug_if.body = malloc(sizeof(Statement*) * 10);
        stmt->debug_if.body_count = 0;
        int capacity = 10;
        
        // Inside debug if block, parse statements until we see "debug end if"
        while (parser->current_token->type != TOKEN_EOF) {
            // Check if this is "debug end if"
            if (parser->current_token->type == TOKEN_DEBUG) {
                // Peek ahead to see if next token is 'end'
                // We need to check if this is "debug end if" or another debug statement
                int saved_pos = parser->lexer->pos;
                Token* next_token = lexer_next_token(parser->lexer);
                int is_end = (next_token->type == TOKEN_END);
                token_free(next_token);
                parser->lexer->pos = saved_pos; // Restore position
                
                if (is_end) {
                    // This is "debug end if", stop parsing body
                    break;
                }
            }
            
            Statement* body_stmt = parser_parse_statement(parser);
            
            if (stmt->debug_if.body_count >= capacity) {
                capacity *= 2;
                stmt->debug_if.body = realloc(stmt->debug_if.body,
                                              sizeof(Statement*) * capacity);
            }
            stmt->debug_if.body[stmt->debug_if.body_count++] = body_stmt;
        }
        
        // Expect 'debug' (for "debug end if")
        if (parser->current_token->type != TOKEN_DEBUG) {
            fprintf(stderr, "Parser error: Expected 'debug end if' to close debug if block at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip 'debug'
        
        // Expect 'end'
        if (parser->current_token->type != TOKEN_END) {
            fprintf(stderr, "Parser error: Expected 'end' after 'debug' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip 'end'
        
        // Expect 'if'
        if (parser->current_token->type != TOKEN_IF) {
            fprintf(stderr, "Parser error: Expected 'if' after 'debug end' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
    }
    else if (parser->current_token->type == TOKEN_PAUSE) {
        // debug pause
        stmt->type = STMT_DEBUG_PAUSE;
        parser_advance(parser); // skip 'pause'
    }
    else {
        fprintf(stderr, "Parser error: Unknown debug statement type at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    return stmt;
}

Statement* parser_parse_func_definition(Parser* parser, int is_exported, int is_async) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_FUNC_DEF;
    stmt->func_def.is_exported = is_exported;  // Set visibility
    stmt->func_def.is_async = is_async;        // Set async flag
    stmt->func_def.is_generator = 0;           // Phase 14: Default not a generator
    
    // Initialize generic fields
    stmt->func_def.type_params = NULL;
    stmt->func_def.type_param_count = 0;
    stmt->func_def.param_is_generic = NULL;
    stmt->func_def.param_generic_index = NULL;
    stmt->func_def.return_is_generic = 0;
    stmt->func_def.return_generic_index = -1;
    
    // Skip 'func'
    parser_advance(parser);
    
    // Parse function name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected function name after 'func' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    stmt->func_def.func_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->func_def.func_name, parser->current_token->value);
    parser_advance(parser);
    
    // Phase 13: Check for generic type parameters <T, U, ...>
    if (parser->current_token->type == TOKEN_LESS) {
        parser_advance(parser); // skip '<'
        
        stmt->func_def.type_params = malloc(sizeof(char*) * 10);
        int type_param_capacity = 10;
        
        while (1) {
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected type parameter name at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            if (stmt->func_def.type_param_count >= type_param_capacity) {
                type_param_capacity *= 2;
                stmt->func_def.type_params = realloc(stmt->func_def.type_params,
                                                     sizeof(char*) * type_param_capacity);
            }
            
            stmt->func_def.type_params[stmt->func_def.type_param_count] = 
                malloc(strlen(parser->current_token->value) + 1);
            strcpy(stmt->func_def.type_params[stmt->func_def.type_param_count],
                   parser->current_token->value);
            stmt->func_def.type_param_count++;
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
            } else {
                break;
            }
        }
        
        if (parser->current_token->type != TOKEN_GREATER) {
            fprintf(stderr, "Parser error: Expected '>' after type parameters at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip '>'
    }
    
    // Expect '('
    if (parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Parser error: Expected '(' after function name at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse parameters
    stmt->func_def.param_types = malloc(sizeof(VarType) * 10);
    stmt->func_def.param_names = malloc(sizeof(char*) * 10);
    stmt->func_def.param_struct_names = malloc(sizeof(char*) * 10);
    stmt->func_def.param_is_generic = malloc(sizeof(int) * 10);
    stmt->func_def.param_generic_index = malloc(sizeof(int) * 10);
    stmt->func_def.param_count = 0;
    int param_capacity = 10;
    
    if (parser->current_token->type != TOKEN_RPAREN) {
        while (1) {
            // Parse parameter type
            VarType param_type;
            char* struct_name = NULL;  // For struct type parameters
            int is_generic_param = 0;
            int generic_param_index = -1;
            
            if (parser->current_token->type == TOKEN_NUMERIC) {
                param_type = TYPE_NUMERIC;
                parser_advance(parser);
            } else if (parser->current_token->type == TOKEN_DECIMAL) {
                param_type = TYPE_DECIMAL;
                parser_advance(parser);
            } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                param_type = TYPE_BOOLEAN;
                parser_advance(parser);
            } else if (parser->current_token->type == TOKEN_TEXT) {
                param_type = TYPE_STRING;
                parser_advance(parser);
            } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
                // Check if it's a generic type parameter (T, U, etc.)
                char* type_name = parser->current_token->value;
                int found_generic = 0;
                
                for (int i = 0; i < stmt->func_def.type_param_count; i++) {
                    if (strcmp(type_name, stmt->func_def.type_params[i]) == 0) {
                        is_generic_param = 1;
                        generic_param_index = i;
                        found_generic = 1;
                        break;
                    }
                }
                
                if (found_generic) {
                    param_type = TYPE_NUMERIC;  // Placeholder, will be resolved at instantiation
                } else {
                    // Could be a struct type (e.g., Vector2D)
                    param_type = TYPE_NUMERIC;  // Placeholder
                    struct_name = malloc(strlen(parser->current_token->value) + 1);
                    strcpy(struct_name, parser->current_token->value);
                }
                parser_advance(parser);
            } else {
                fprintf(stderr, "Parser error: Expected parameter type at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            // Phase 10: Check for pointer type (*)
            if (parser->current_token->type == TOKEN_MULTIPLY) {
                parser_advance(parser); // skip '*'
                // For now, pointer types are stored the same as regular types
                // In future, we may need to track this differently
            }
            
            // Parse parameter name
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected parameter name at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            if (stmt->func_def.param_count >= param_capacity) {
                param_capacity *= 2;
                stmt->func_def.param_types = realloc(stmt->func_def.param_types,
                                                    sizeof(VarType) * param_capacity);
                stmt->func_def.param_names = realloc(stmt->func_def.param_names,
                                                     sizeof(char*) * param_capacity);
                stmt->func_def.param_struct_names = realloc(stmt->func_def.param_struct_names,
                                                           sizeof(char*) * param_capacity);
                stmt->func_def.param_is_generic = realloc(stmt->func_def.param_is_generic,
                                                          sizeof(int) * param_capacity);
                stmt->func_def.param_generic_index = realloc(stmt->func_def.param_generic_index,
                                                             sizeof(int) * param_capacity);
            }
            
            stmt->func_def.param_types[stmt->func_def.param_count] = param_type;
            stmt->func_def.param_names[stmt->func_def.param_count] = malloc(strlen(parser->current_token->value) + 1);
            strcpy(stmt->func_def.param_names[stmt->func_def.param_count], parser->current_token->value);
            stmt->func_def.param_struct_names[stmt->func_def.param_count] = struct_name;  // NULL for primitives
            stmt->func_def.param_is_generic[stmt->func_def.param_count] = is_generic_param;
            stmt->func_def.param_generic_index[stmt->func_def.param_count] = generic_param_index;
            stmt->func_def.param_count++;
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser);
            } else {
                break;
            }
        }
    }
    
    // Expect ')'
    if (parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Parser error: Expected ')' after parameters at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse return type(s) if present (returns type1, type2, ...)
    // Or yield type for generators (yields type)
    stmt->func_def.return_types = NULL;
    stmt->func_def.return_count = 0;
    
    if (parser->current_token->type == TOKEN_RETURNS || 
        parser->current_token->type == TOKEN_YIELDS) {
        
        // Phase 14: Check if it's a generator function
        if (parser->current_token->type == TOKEN_YIELDS) {
            stmt->func_def.is_generator = 1;
        }
        
        parser_advance(parser); // skip 'returns' or 'yields'
        
        stmt->func_def.return_types = malloc(sizeof(VarType) * 10);
        int return_capacity = 10;
        
        // Parse first return type
        VarType ret_type;
        if (parser->current_token->type == TOKEN_NUMERIC) {
            ret_type = TYPE_NUMERIC;
            parser_advance(parser);
        } else if (parser->current_token->type == TOKEN_DECIMAL) {
            ret_type = TYPE_DECIMAL;
            parser_advance(parser);
        } else if (parser->current_token->type == TOKEN_BOOLEAN) {
            ret_type = TYPE_BOOLEAN;
            parser_advance(parser);
        } else if (parser->current_token->type == TOKEN_TEXT) {
            ret_type = TYPE_STRING;
            parser_advance(parser);
        } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Check if it's a generic type parameter
            char* type_name = parser->current_token->value;
            int found_generic = 0;
            
            for (int i = 0; i < stmt->func_def.type_param_count; i++) {
                if (strcmp(type_name, stmt->func_def.type_params[i]) == 0) {
                    stmt->func_def.return_is_generic = 1;
                    stmt->func_def.return_generic_index = i;
                    found_generic = 1;
                    break;
                }
            }
            
            if (!found_generic) {
                fprintf(stderr, "Parser error: Unknown return type '%s' at line %d\n",
                        type_name, parser->current_token->line);
                exit(1);
            }
            ret_type = TYPE_NUMERIC;  // Placeholder
            parser_advance(parser);
        } else {
            fprintf(stderr, "Parser error: Expected return type after 'returns' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        stmt->func_def.return_types[stmt->func_def.return_count++] = ret_type;
        
        // Parse additional return types if comma-separated
        while (parser->current_token->type == TOKEN_COMMA) {
            parser_advance(parser); // skip ','
            
            if (stmt->func_def.return_count >= return_capacity) {
                return_capacity *= 2;
                stmt->func_def.return_types = realloc(stmt->func_def.return_types,
                                                     sizeof(VarType) * return_capacity);
            }
            
            if (parser->current_token->type == TOKEN_NUMERIC) {
                ret_type = TYPE_NUMERIC;
            } else if (parser->current_token->type == TOKEN_DECIMAL) {
                ret_type = TYPE_DECIMAL;
            } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                ret_type = TYPE_BOOLEAN;
            } else if (parser->current_token->type == TOKEN_TEXT) {
                ret_type = TYPE_STRING;
            } else {
                fprintf(stderr, "Parser error: Expected return type after ',' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            stmt->func_def.return_types[stmt->func_def.return_count++] = ret_type;
            parser_advance(parser);
        }
    }
    
    // Parse function body
    stmt->func_def.body = malloc(sizeof(Statement*) * 10);
    stmt->func_def.body_count = 0;
    int body_capacity = 10;
    
    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        Statement* body_stmt = parser_parse_statement(parser);
        
        if (stmt->func_def.body_count >= body_capacity) {
            body_capacity *= 2;
            stmt->func_def.body = realloc(stmt->func_def.body,
                                         sizeof(Statement*) * body_capacity);
        }
        stmt->func_def.body[stmt->func_def.body_count++] = body_stmt;
    }
    
    // Expect 'end' 'func'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after function body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_FUNC) {
        fprintf(stderr, "Parser error: Expected 'func' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

Statement* parser_parse_struct_definition(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_STRUCT_DEF;
    
    // Initialize generic fields
    stmt->struct_def.type_params = NULL;
    stmt->struct_def.type_param_count = 0;
    
    // Skip 'struct'
    parser_advance(parser);
    
    // Parse struct name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected struct name after 'struct' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    stmt->struct_def.struct_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->struct_def.struct_name, parser->current_token->value);
    parser_advance(parser);
    
    // Phase 13: Check for generic type parameters <T, U, ...>
    if (parser->current_token->type == TOKEN_LESS) {
        parser_advance(parser); // skip '<'
        
        stmt->struct_def.type_params = malloc(sizeof(char*) * 10);
        int type_param_capacity = 10;
        
        while (1) {
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected type parameter name at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            if (stmt->struct_def.type_param_count >= type_param_capacity) {
                type_param_capacity *= 2;
                stmt->struct_def.type_params = realloc(stmt->struct_def.type_params,
                                                       sizeof(char*) * type_param_capacity);
            }
            
            stmt->struct_def.type_params[stmt->struct_def.type_param_count] = 
                malloc(strlen(parser->current_token->value) + 1);
            strcpy(stmt->struct_def.type_params[stmt->struct_def.type_param_count],
                   parser->current_token->value);
            stmt->struct_def.type_param_count++;
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
            } else {
                break;
            }
        }
        
        if (parser->current_token->type != TOKEN_GREATER) {
            fprintf(stderr, "Parser error: Expected '>' after type parameters at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip '>'
    }
    
    // Check for 'implements' keyword
    stmt->struct_def.implements = NULL;
    stmt->struct_def.implements_count = 0;
    
    if (parser->current_token->type == TOKEN_IMPLEMENTS) {
        parser_advance(parser);
        
        // Parse interface names (comma-separated)
        stmt->struct_def.implements = malloc(sizeof(char*) * 10);
        int implements_capacity = 10;
        
        while (1) {
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected interface name after 'implements' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            if (stmt->struct_def.implements_count >= implements_capacity) {
                implements_capacity *= 2;
                stmt->struct_def.implements = realloc(stmt->struct_def.implements,
                                                     sizeof(char*) * implements_capacity);
            }
            
            stmt->struct_def.implements[stmt->struct_def.implements_count] = 
                malloc(strlen(parser->current_token->value) + 1);
            strcpy(stmt->struct_def.implements[stmt->struct_def.implements_count],
                   parser->current_token->value);
            stmt->struct_def.implements_count++;
            parser_advance(parser);
            
            // Check for comma (multiple interfaces)
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser);
            } else {
                break;
            }
        }
    }
    
    // Parse struct fields (no 'then' keyword, unlike if statement)
    stmt->struct_def.fields = malloc(sizeof(StructField) * 10);
    stmt->struct_def.field_count = 0;
    int field_capacity = 10;
    
    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        // Parse field type
        VarType field_type;
        int field_is_generic = 0;
        int field_generic_index = -1;
        
        if (parser->current_token->type == TOKEN_NUMERIC) {
            field_type = TYPE_NUMERIC;
        } else if (parser->current_token->type == TOKEN_DECIMAL) {
            field_type = TYPE_DECIMAL;
        } else if (parser->current_token->type == TOKEN_BOOLEAN) {
            field_type = TYPE_BOOLEAN;
        } else if (parser->current_token->type == TOKEN_TEXT) {
            field_type = TYPE_STRING;
        } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Check if it's a type parameter (e.g., T, U)
            int found_type_param = 0;
            for (int i = 0; i < stmt->struct_def.type_param_count; i++) {
                if (strcmp(parser->current_token->value, stmt->struct_def.type_params[i]) == 0) {
                    field_is_generic = 1;
                    field_generic_index = i;
                    field_type = TYPE_NUMERIC; // Placeholder, will be substituted at instantiation
                    found_type_param = 1;
                    break;
                }
            }
            if (!found_type_param) {
                // Must be 'end' keyword or unknown type
                break;
            }
        } else {
            // Must be 'end' keyword
            break;
        }
        parser_advance(parser);
        
        // Parse field name
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Parser error: Expected field name at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        
        if (stmt->struct_def.field_count >= field_capacity) {
            field_capacity *= 2;
            stmt->struct_def.fields = realloc(stmt->struct_def.fields,
                                             sizeof(StructField) * field_capacity);
        }
        
        stmt->struct_def.fields[stmt->struct_def.field_count].type = field_type;
        stmt->struct_def.fields[stmt->struct_def.field_count].name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->struct_def.fields[stmt->struct_def.field_count].name, parser->current_token->value);
        stmt->struct_def.fields[stmt->struct_def.field_count].is_generic = field_is_generic;
        stmt->struct_def.fields[stmt->struct_def.field_count].generic_index = field_generic_index;
        stmt->struct_def.field_count++;
        parser_advance(parser);
    }
    
    // Expect 'end' 'struct'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after struct body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_STRUCT) {
        fprintf(stderr, "Parser error: Expected 'struct' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

Statement* parser_parse_interface_definition(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_INTERFACE_DEF;
    
    // Skip 'interface'
    parser_advance(parser);
    
    // Parse interface name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected interface name after 'interface' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    stmt->interface_def.interface_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->interface_def.interface_name, parser->current_token->value);
    parser_advance(parser);
    
    // Parse method signatures
    stmt->interface_def.methods = malloc(sizeof(MethodSignature*) * 10);
    stmt->interface_def.method_count = 0;
    int method_capacity = 10;
    
    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        // Must be 'yaz' (func keyword)
        if (parser->current_token->type != TOKEN_FUNC) {
            break;
        }
        parser_advance(parser);
        
        // Parse method name
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Parser error: Expected method name at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        
        if (stmt->interface_def.method_count >= method_capacity) {
            method_capacity *= 2;
            stmt->interface_def.methods = realloc(stmt->interface_def.methods,
                                                 sizeof(MethodSignature*) * method_capacity);
        }
        
        MethodSignature* sig = malloc(sizeof(MethodSignature));
        sig->name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(sig->name, parser->current_token->value);
        parser_advance(parser);
        
        // Parse parameters: ( ... )
        if (parser->current_token->type != TOKEN_LPAREN) {
            fprintf(stderr, "Parser error: Expected '(' after method name at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
        
        sig->param_types = malloc(sizeof(VarType) * 10);
        sig->param_names = malloc(sizeof(char*) * 10);
        sig->param_count = 0;
        int param_capacity = 10;
        
        while (parser->current_token->type != TOKEN_RPAREN &&
               parser->current_token->type != TOKEN_EOF) {
            // Parse parameter type
            VarType param_type;
            if (parser->current_token->type == TOKEN_NUMERIC) {
                param_type = TYPE_NUMERIC;
            } else if (parser->current_token->type == TOKEN_DECIMAL) {
                param_type = TYPE_DECIMAL;
            } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                param_type = TYPE_BOOLEAN;
            } else if (parser->current_token->type == TOKEN_TEXT) {
                param_type = TYPE_STRING;
            } else {
                fprintf(stderr, "Parser error: Expected parameter type at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            if (sig->param_count >= param_capacity) {
                param_capacity *= 2;
                sig->param_types = realloc(sig->param_types, sizeof(VarType) * param_capacity);
                sig->param_names = realloc(sig->param_names, sizeof(char*) * param_capacity);
            }
            
            sig->param_types[sig->param_count] = param_type;
            sig->param_names[sig->param_count] = NULL; // No names in interface signatures
            sig->param_count++;
            parser_advance(parser);
            
            // Check for comma
            if (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser);
            }
        }
        parser_advance(parser); // Skip ')'
        
        // Parse return type: -> type
        sig->has_return = 0;
        if (parser->current_token->type == TOKEN_RETURNS) {
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_NUMERIC) {
                sig->return_type = TYPE_NUMERIC;
                sig->has_return = 1;
            } else if (parser->current_token->type == TOKEN_DECIMAL) {
                sig->return_type = TYPE_DECIMAL;
                sig->has_return = 1;
            } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                sig->return_type = TYPE_BOOLEAN;
                sig->has_return = 1;
            } else if (parser->current_token->type == TOKEN_TEXT) {
                sig->return_type = TYPE_STRING;
                sig->has_return = 1;
            } else if (strcmp(parser->current_token->value, "void") == 0) {
                sig->has_return = 0;
            } else {
                fprintf(stderr, "Parser error: Expected return type after '->' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser);
        }
        
        stmt->interface_def.methods[stmt->interface_def.method_count] = sig;
        stmt->interface_def.method_count++;
    }
    
    // Expect 'end' 'interface'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after interface body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_INTERFACE) {
        fprintf(stderr, "Parser error: Expected 'interface' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}


Statement* parser_parse_type_alias(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_TYPE_ALIAS;
    
    // Skip 'type'
    parser_advance(parser);
    
    // Parse alias name (e.g., PersonId)
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected alias name after 'type' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    stmt->type_alias.alias_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->type_alias.alias_name, parser->current_token->value);
    parser_advance(parser);
    
    // Expect '='
    if (parser->current_token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Parser error: Expected '=' after alias name at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    // Parse base type (numeric, text, boolean, struct Name, etc.)
    stmt->type_alias.struct_name = NULL; // Default for primitive types
    
    if (parser->current_token->type == TOKEN_NUMERIC) {
        stmt->type_alias.base_type = TYPE_NUMERIC;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_DECIMAL) {
        stmt->type_alias.base_type = TYPE_DECIMAL;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_TEXT) {
        stmt->type_alias.base_type = TYPE_STRING;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_BOOLEAN) {
        stmt->type_alias.base_type = TYPE_BOOLEAN;
        parser_advance(parser);
    } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        // Assume it's a struct type or another alias (e.g., type PersonRef = Person)
        // We'll store as TYPE_NUMERIC placeholder and use struct_name
        stmt->type_alias.base_type = TYPE_NUMERIC;
        stmt->type_alias.struct_name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->type_alias.struct_name, parser->current_token->value);
        parser_advance(parser);
    } else {
        fprintf(stderr, "Parser error: Expected type name after '=' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    
    return stmt;
}

Statement* parser_parse_enum_definition(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    stmt->type = STMT_ENUM_DEF;
    
    // Skip 'enum'
    parser_advance(parser);
    
    // Parse enum name
    if (parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Parser error: Expected enum name after 'enum' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    stmt->enum_def.enum_name = malloc(strlen(parser->current_token->value) + 1);
    strcpy(stmt->enum_def.enum_name, parser->current_token->value);
    parser_advance(parser);
    
    // Parse enum members
    stmt->enum_def.members = malloc(sizeof(EnumMember) * 10);
    stmt->enum_def.member_count = 0;
    int member_capacity = 10;
    
    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_EOF) {
        
        // Parse member name
        if (parser->current_token->type != TOKEN_IDENTIFIER) {
            // Must be 'end' keyword
            break;
        }
        
        if (stmt->enum_def.member_count >= member_capacity) {
            member_capacity *= 2;
            stmt->enum_def.members = realloc(stmt->enum_def.members,
                                             sizeof(EnumMember) * member_capacity);
        }
        
        stmt->enum_def.members[stmt->enum_def.member_count].name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(stmt->enum_def.members[stmt->enum_def.member_count].name, parser->current_token->value);
        parser_advance(parser);
        
        // Expect '=' and value
        if (parser->current_token->type != TOKEN_ASSIGN) {
            fprintf(stderr, "Parser error: Expected '=' after enum member name at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser);
        
        if (parser->current_token->type != TOKEN_NUMBER) {
            fprintf(stderr, "Parser error: Expected number after '=' in enum definition at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        stmt->enum_def.members[stmt->enum_def.member_count].value = atol(parser->current_token->value);
        stmt->enum_def.member_count++;
        parser_advance(parser);
    }
    
    // Expect 'end' 'enum'
    if (parser->current_token->type != TOKEN_END) {
        fprintf(stderr, "Parser error: Expected 'end' after enum body at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    if (parser->current_token->type != TOKEN_ENUM) {
        fprintf(stderr, "Parser error: Expected 'enum' after 'end' at line %d\n",
                parser->current_token->line);
        exit(1);
    }
    parser_advance(parser);
    
    return stmt;
}

Statement* parser_parse_statement(Parser* parser) {
    Statement* stmt = malloc(sizeof(Statement));
    
    // Check for async modifier
    int is_async = 0;
    if (parser->current_token->type == TOKEN_ASYNC) {
        is_async = 1;
        parser_advance(parser);  // skip 'async'
        
        // Async must be followed by func
        if (parser->current_token->type != TOKEN_FUNC) {
            fprintf(stderr, "Parser error: 'async' must be followed by 'func' at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
    }
    
    // Check for export/private visibility modifiers
    int is_exported = 1;  // Default: exported (public)
    if (parser->current_token->type == TOKEN_EXPORT) {
        is_exported = 1;
        parser_advance(parser);  // skip 'export'
    } else if (parser->current_token->type == TOKEN_PRIVATE) {
        is_exported = 0;
        parser_advance(parser);  // skip 'private'
    }
    
    // Check for import
    if (parser->current_token->type == TOKEN_IMPORT) {
        free(stmt);
        return parser_parse_import_statement(parser);
    }
    // Check for module
    else if (parser->current_token->type == TOKEN_MODULE) {
        free(stmt);
        return parser_parse_module_definition(parser);
    }
    // Phase 16: Check for operator overloading
    else if (parser->current_token->type == TOKEN_OPERATOR) {
        free(stmt);
        return parser_parse_operator_definition(parser);
    }
    // Phase 17: Check for pattern matching
    else if (parser->current_token->type == TOKEN_MATCH) {
        free(stmt);
        return parser_parse_match_statement(parser);
    }
    // Phase 18: Check for state declaration
    else if (parser->current_token->type == TOKEN_STATE) {
        free(stmt);
        return parser_parse_state_statement(parser, 0); // not shared
    }
    else if (parser->current_token->type == TOKEN_SHARED) {
        free(stmt);
        return parser_parse_state_statement(parser, 1); // shared
    }
    // Check for func
    else if (parser->current_token->type == TOKEN_FUNC) {
        free(stmt);
        return parser_parse_func_definition(parser, is_exported, is_async);
    }
    // Check for struct
    else if (parser->current_token->type == TOKEN_STRUCT) {
        free(stmt);
        return parser_parse_struct_definition(parser);
    }
    // Check for interface
    else if (parser->current_token->type == TOKEN_INTERFACE) {
        free(stmt);
        return parser_parse_interface_definition(parser);
    }
    // Check for type alias
    else if (parser->current_token->type == TOKEN_TYPE) {
        free(stmt);
        return parser_parse_type_alias(parser);
    }
    // Check for enum
    else if (parser->current_token->type == TOKEN_ENUM) {
        free(stmt);
        return parser_parse_enum_definition(parser);
    }
    // Check for return
    else if (parser->current_token->type == TOKEN_RETURN) {
        stmt->type = STMT_RETURN;
        parser_advance(parser);
        
        // Parse return value(s) - support multiple values separated by comma
        stmt->return_stmt.values = NULL;
        stmt->return_stmt.value_count = 0;
        
        if (parser->current_token->type != TOKEN_EOF && 
            parser->current_token->type != TOKEN_END) {
            
            stmt->return_stmt.values = malloc(sizeof(Expression*) * 10);
            int value_capacity = 10;
            
            // Parse first return value
            stmt->return_stmt.values[stmt->return_stmt.value_count++] = parser_parse_expression(parser);
            
            // Parse additional return values if comma-separated
            while (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
                
                if (stmt->return_stmt.value_count >= value_capacity) {
                    value_capacity *= 2;
                    stmt->return_stmt.values = realloc(stmt->return_stmt.values,
                                                       sizeof(Expression*) * value_capacity);
                }
                
                stmt->return_stmt.values[stmt->return_stmt.value_count++] = parser_parse_expression(parser);
            }
        }
        return stmt;
    }
    // Check for if
    else if (parser->current_token->type == TOKEN_IF) {
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
    // Check for do-while loop
    else if (parser->current_token->type == TOKEN_DO) {
        free(stmt);
        return parser_parse_do_while_statement(parser);
    }
    // Check for switch statement
    else if (parser->current_token->type == TOKEN_SWITCH) {
        free(stmt);
        return parser_parse_switch_statement(parser);
    }
    // Check for try-catch
    else if (parser->current_token->type == TOKEN_TRY) {
        free(stmt);
        return parser_parse_try_catch(parser);
    }
    // Check for throw
    else if (parser->current_token->type == TOKEN_THROW) {
        stmt->type = STMT_THROW;
        parser_advance(parser); // skip 'throw'
        
        // Parse exception type (optional identifier before expression)
        stmt->throw_stmt.error_type = NULL;
        stmt->throw_stmt.error_message = NULL;
        
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Look ahead: throw ErrorType("message") or throw "message"
            // If identifier followed by '(', it's a type
            char* first_id = malloc(strlen(parser->current_token->value) + 1);
            strcpy(first_id, parser->current_token->value);
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_LPAREN) {
                // throw ErrorType(message)
                stmt->throw_stmt.error_type = first_id;
                parser_advance(parser); // skip '('
                stmt->throw_stmt.error_message = parser_parse_expression(parser);
                
                if (parser->current_token->type != TOKEN_RPAREN) {
                    fprintf(stderr, "Parser error: Expected ')' after throw message at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                parser_advance(parser); // skip ')'
            } else {
                // Just a variable, treat as message
                Expression* var_expr = malloc(sizeof(Expression));
                var_expr->type = EXPR_VARIABLE;
                var_expr->var_name = first_id;
                stmt->throw_stmt.error_message = var_expr;
            }
        } else {
            // throw expression (no type)
            stmt->throw_stmt.error_message = parser_parse_expression(parser);
        }
        
        return stmt;
    }
    // Phase 14: Check for yield statement
    else if (parser->current_token->type == TOKEN_YIELD) {
        stmt->type = STMT_YIELD;
        parser_advance(parser); // skip 'yield'
        
        // Parse yield value
        stmt->yield_stmt.value = parser_parse_expression(parser);
        
        return stmt;
    }
    // Check for debug statements
    else if (parser->current_token->type == TOKEN_DEBUG) {
        free(stmt);
        return parser_parse_debug_statement(parser);
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
    // Check for type keywords (declaration or destructuring)
    else if (parser->current_token->type == TOKEN_NUMERIC ||
        parser->current_token->type == TOKEN_DECIMAL ||
        parser->current_token->type == TOKEN_BOOLEAN ||
        parser->current_token->type == TOKEN_TEXT) {
        
        // Try to parse as declaration first
        // But we need to detect destructuring pattern: type id, id, id = expr
        // vs normal declaration: type id or type id = expr or type[] id
        // Phase 21: Also detect mixed type declaration: numeric, string, boolean a, b, c
        
        Token* type_token = parser->current_token;
        TokenType saved_type = type_token->type;  // Save type before advancing
        
        parser_advance(parser);
        
        // Phase 21: Check for mixed type declaration: numeric, string, boolean a, b, c
        if (parser->current_token->type == TOKEN_COMMA) {
            // Mixed type declaration!
            // Collect all types first
            VarType types[100];
            int type_count = 0;
            
            // First type
            if (saved_type == TOKEN_NUMERIC) types[type_count++] = TYPE_NUMERIC;
            else if (saved_type == TOKEN_DECIMAL) types[type_count++] = TYPE_DECIMAL;
            else if (saved_type == TOKEN_BOOLEAN) types[type_count++] = TYPE_BOOLEAN;
            else types[type_count++] = TYPE_STRING;
            
            // Parse remaining types (comma was already seen to get here)
            // We're at the comma position - check what comes after
            while (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
                
                // Check if this is another type or start of identifiers
                if (parser->current_token->type == TOKEN_NUMERIC) {
                    types[type_count++] = TYPE_NUMERIC;
                    parser_advance(parser);
                } else if (parser->current_token->type == TOKEN_DECIMAL) {
                    types[type_count++] = TYPE_DECIMAL;
                    parser_advance(parser);
                } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                    types[type_count++] = TYPE_BOOLEAN;
                    parser_advance(parser);
                } else if (parser->current_token->type == TOKEN_TEXT) {
                    types[type_count++] = TYPE_STRING;
                    parser_advance(parser);
                } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
                    // This is the first identifier - handle it below
                    break;
                } else {
                    fprintf(stderr, "Parser error: Expected type or identifier at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
            }
            
            // Now parse variable names - current token should be first identifier
            char* names[100];
            int name_count = 0;
            
            // First identifier
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected identifier in mixed declaration at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            names[name_count] = malloc(strlen(parser->current_token->value) + 1);
            strcpy(names[name_count], parser->current_token->value);
            name_count++;
            parser_advance(parser);
            
            // Parse remaining names
            while (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
                
                if (parser->current_token->type != TOKEN_IDENTIFIER) {
                    fprintf(stderr, "Parser error: Expected identifier after ',' at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                names[name_count] = malloc(strlen(parser->current_token->value) + 1);
                strcpy(names[name_count], parser->current_token->value);
                name_count++;
                parser_advance(parser);
            }
            
            // Validate: type count must equal name count
            if (type_count != name_count) {
                fprintf(stderr, "Parser error: Mixed declaration has %d types but %d names at line %d\n",
                        type_count, name_count, parser->current_token->line);
                exit(1);
            }
            
            // Create multi-declaration statement
            stmt->type = STMT_MULTI_DECLARATION;
            stmt->multi_decl.declarations = malloc(sizeof(Declaration*) * type_count);
            stmt->multi_decl.count = type_count;
            
            for (int i = 0; i < type_count; i++) {
                Declaration* decl = malloc(sizeof(Declaration));
                decl->type = types[i];
                decl->name = names[i];
                decl->struct_name = NULL;
                decl->is_array = 0;
                decl->is_list = 0;
                decl->array_size = 0;
                decl->array_init = NULL;
                decl->array_init_count = 0;
                decl->is_pointer = 0;
                decl->is_nullable = 0;
                decl->is_union = 0;
                decl->union_types = NULL;
                decl->union_count = 0;
                decl->init_value = NULL;
                decl->is_exported = is_exported;
                decl->struct_type_args = NULL;
                decl->struct_type_arg_count = 0;
                
                stmt->multi_decl.declarations[i] = decl;
            }
            
            return stmt;
        }
        
        // Check for array syntax, pointer syntax, or list syntax
        if (parser->current_token->type == TOKEN_LBRACKET ||
            parser->current_token->type == TOKEN_MULTIPLY ||
            parser->current_token->type == TOKEN_LIST) {
            // Normal declaration with array or pointer - rewind and parse normally
            // We need to go back - save current state
            int is_array = (parser->current_token->type == TOKEN_LBRACKET);
            int is_pointer = (parser->current_token->type == TOKEN_MULTIPLY);
            int is_list = (parser->current_token->type == TOKEN_LIST);
            
            // For now, just parse as normal declaration
            // This is a simplification - ideally we'd rewind properly
            stmt->type = STMT_DECLARATION;
            
            VarType var_type;
            if (saved_type == TOKEN_NUMERIC) var_type = TYPE_NUMERIC;
            else if (saved_type == TOKEN_DECIMAL) var_type = TYPE_DECIMAL;
            else if (saved_type == TOKEN_BOOLEAN) var_type = TYPE_BOOLEAN;
            else var_type = TYPE_STRING;
            
            Declaration* decl = malloc(sizeof(Declaration));
            decl->type = var_type;
            decl->struct_name = NULL;
            decl->is_nullable = 0;
            decl->is_exported = is_exported;
            decl->is_array = 0;
            decl->is_list = 0;
            decl->array_size = 0;
            decl->array_init = NULL;
            decl->array_init_count = 0;
            decl->is_pointer = 0;
            decl->is_union = 0;
            decl->union_types = NULL;
            decl->union_count = 0;
            decl->init_value = NULL;
            decl->struct_type_args = NULL;
            decl->struct_type_arg_count = 0;
            
            if (is_array) {
                parser_advance(parser); // skip '['
                decl->is_array = 1;
                if (parser->current_token->type == TOKEN_NUMBER) {
                    decl->array_size = atoi(parser->current_token->value);
                    parser_advance(parser);
                } else {
                    decl->array_size = 0; // Dynamic
                }
                if (parser->current_token->type != TOKEN_RBRACKET) {
                    fprintf(stderr, "Parser error: Expected ']' in array declaration\n");
                    exit(1);
                }
                parser_advance(parser);
            } else if (is_list) {
                // Phase 21: List type - numeric list() x
                parser_advance(parser); // skip 'list'
                if (parser->current_token->type != TOKEN_LPAREN) {
                    fprintf(stderr, "Parser error: Expected '(' after 'list'\n");
                    exit(1);
                }
                parser_advance(parser); // skip '('
                if (parser->current_token->type != TOKEN_RPAREN) {
                    fprintf(stderr, "Parser error: Expected ')' after 'list('\n");
                    exit(1);
                }
                parser_advance(parser); // skip ')'
                decl->is_list = 1;
                decl->is_array = 1;
                decl->array_size = 0;
            } else if (is_pointer) {
                parser_advance(parser); // skip '*'
                decl->is_pointer = 1;
            }
            
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected identifier\n");
                exit(1);
            }
            decl->name = malloc(strlen(parser->current_token->value) + 1);
            strcpy(decl->name, parser->current_token->value);
            parser_advance(parser);
            
            decl->init_value = NULL;
            if (parser->current_token->type == TOKEN_ASSIGN) {
                parser_advance(parser);
                
                // Phase 21: Check for list() initialization
                if (decl->is_list && parser->current_token->type == TOKEN_LIST) {
                    parser_advance(parser); // skip 'list'
                    if (parser->current_token->type == TOKEN_LPAREN) {
                        parser_advance(parser); // skip '('
                        if (parser->current_token->type == TOKEN_RPAREN) {
                            parser_advance(parser); // skip ')'
                            // Empty list - no init value needed
                        }
                    }
                }
                // Check for array literal: [1, 2, 3]
                else if (decl->is_array && parser->current_token->type == TOKEN_LBRACKET) {
                    parser_advance(parser); // skip '['
                    
                    // Parse array elements
                    int capacity = 10;
                    decl->array_init = malloc(sizeof(Expression*) * capacity);
                    decl->array_init_count = 0;
                    
                    if (parser->current_token->type != TOKEN_RBRACKET) {
                        while (1) {
                            if (decl->array_init_count >= capacity) {
                                capacity *= 2;
                                decl->array_init = realloc(decl->array_init, sizeof(Expression*) * capacity);
                            }
                            
                            decl->array_init[decl->array_init_count++] = parser_parse_expression(parser);
                            
                            if (parser->current_token->type == TOKEN_COMMA) {
                                parser_advance(parser);
                            } else {
                                break;
                            }
                        }
                    }
                    
                    if (parser->current_token->type != TOKEN_RBRACKET) {
                        fprintf(stderr, "Parser error: Expected ']' after array elements at line %d\n",
                                parser->current_token->line);
                        exit(1);
                    }
                    parser_advance(parser); // skip ']'
                    
                    // Set array size from literal if not specified
                    if (decl->array_size == 0) {
                        decl->array_size = decl->array_init_count;
                    }
                } else {
                    decl->init_value = parser_parse_expression(parser);
                }
            }
            
            stmt->declaration = decl;
            
        } else if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // Could be: numeric a, b = arr OR numeric a = 5 OR numeric a
            // Save first identifier
            char* first_name = malloc(strlen(parser->current_token->value) + 1);
            strcpy(first_name, parser->current_token->value);
            parser_advance(parser);
            
            if (parser->current_token->type == TOKEN_COMMA) {
                // Destructuring: numeric a, b, c = arr
                stmt->type = STMT_DESTRUCTURE;
                
                VarType var_type;
                if (saved_type == TOKEN_NUMERIC) var_type = TYPE_NUMERIC;
                else if (saved_type == TOKEN_DECIMAL) var_type = TYPE_DECIMAL;
                else if (saved_type == TOKEN_BOOLEAN) var_type = TYPE_BOOLEAN;
                else var_type = TYPE_STRING;
                
                stmt->destructure.var_type = var_type;
                stmt->destructure.var_names = malloc(sizeof(char*) * 10);
                stmt->destructure.var_count = 0;
                int capacity = 10;
                
                // Add first name
                stmt->destructure.var_names[stmt->destructure.var_count++] = first_name;
                
                // Parse remaining names
                while (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser); // skip comma
                    
                    if (parser->current_token->type != TOKEN_IDENTIFIER) {
                        fprintf(stderr, "Parser error: Expected identifier in destructuring at line %d\n",
                                parser->current_token->line);
                        exit(1);
                    }
                    
                    if (stmt->destructure.var_count >= capacity) {
                        capacity *= 2;
                        stmt->destructure.var_names = realloc(stmt->destructure.var_names,
                                                             sizeof(char*) * capacity);
                    }
                    
                    stmt->destructure.var_names[stmt->destructure.var_count] = 
                        malloc(strlen(parser->current_token->value) + 1);
                    strcpy(stmt->destructure.var_names[stmt->destructure.var_count],
                           parser->current_token->value);
                    stmt->destructure.var_count++;
                    
                    parser_advance(parser);
                }
                
                // Expect '='
                if (parser->current_token->type != TOKEN_ASSIGN) {
                    fprintf(stderr, "Parser error: Expected '=' in destructuring at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                parser_advance(parser);
                
                // Parse source expression
                stmt->destructure.source = parser_parse_expression(parser);
                
            } else {
                // Normal declaration: numeric a = 5 OR numeric a
                stmt->type = STMT_DECLARATION;
                
                VarType var_type;
                if (saved_type == TOKEN_NUMERIC) var_type = TYPE_NUMERIC;
                else if (saved_type == TOKEN_DECIMAL) var_type = TYPE_DECIMAL;
                else if (saved_type == TOKEN_BOOLEAN) var_type = TYPE_BOOLEAN;
                else var_type = TYPE_STRING;
                
                Declaration* decl = malloc(sizeof(Declaration));
                decl->type = var_type;
                decl->name = first_name;
                decl->struct_name = NULL;
                decl->is_array = 0;
                decl->is_list = 0;
                decl->array_size = 0;
                decl->array_init = NULL;
                decl->array_init_count = 0;
                decl->is_pointer = 0;
                decl->is_nullable = 0;
                decl->is_union = 0;
                decl->union_types = NULL;
                decl->union_count = 0;
                decl->is_exported = is_exported;
                decl->init_value = NULL;
                
                if (parser->current_token->type == TOKEN_ASSIGN) {
                    parser_advance(parser);
                    decl->init_value = parser_parse_expression(parser);
                }
                
                stmt->declaration = decl;
            }
        } else {
            fprintf(stderr, "Parser error: Unexpected token after type keyword at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
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
    // Phase 10: Check for pointer dereference assignment: *ptr = value
    else if (parser->current_token->type == TOKEN_MULTIPLY) {
        parser_advance(parser); // skip '*'
        
        // Parse the pointer expression (for now, just support identifiers)
        Expression* ptr_expr = parser_parse_unary_expression(parser);
        
        if (parser->current_token->type != TOKEN_ASSIGN) {
            fprintf(stderr, "Parser error: Expected '=' after dereference at line %d\n",
                    parser->current_token->line);
            exit(1);
        }
        parser_advance(parser); // skip '='
        
        stmt->type = STMT_ASSIGNMENT;
        stmt->assignment.var_name = NULL;
        stmt->assignment.field_name = NULL;
        stmt->assignment.array_index = NULL;
        stmt->assignment.dereference_target = ptr_expr;
        stmt->assignment.value = parser_parse_expression(parser);
    }
    // Check for assignment or function call (identifier)
    else if (parser->current_token->type == TOKEN_IDENTIFIER) {
        char* name = malloc(strlen(parser->current_token->value) + 1);
        strcpy(name, parser->current_token->value);
        parser_advance(parser);
        
        // Phase 13: Check for generic struct instance (e.g., Box<numeric> b)
        if (parser->current_token->type == TOKEN_LESS) {
            parser_advance(parser); // skip '<'
            
            // Parse type arguments
            VarType* type_args = malloc(sizeof(VarType) * 10);
            int type_arg_count = 0;
            int type_arg_capacity = 10;
            
            while (1) {
                VarType type_arg;
                if (parser->current_token->type == TOKEN_NUMERIC) {
                    type_arg = TYPE_NUMERIC;
                } else if (parser->current_token->type == TOKEN_DECIMAL) {
                    type_arg = TYPE_DECIMAL;
                } else if (parser->current_token->type == TOKEN_BOOLEAN) {
                    type_arg = TYPE_BOOLEAN;
                } else if (parser->current_token->type == TOKEN_TEXT) {
                    type_arg = TYPE_STRING;
                } else {
                    fprintf(stderr, "Parser error: Expected type in generic arguments at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                parser_advance(parser);
                
                if (type_arg_count >= type_arg_capacity) {
                    type_arg_capacity *= 2;
                    type_args = realloc(type_args, sizeof(VarType) * type_arg_capacity);
                }
                type_args[type_arg_count++] = type_arg;
                
                if (parser->current_token->type == TOKEN_COMMA) {
                    parser_advance(parser); // skip ','
                } else {
                    break;
                }
            }
            
            if (parser->current_token->type != TOKEN_GREATER) {
                fprintf(stderr, "Parser error: Expected '>' after generic arguments at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip '>'
            
            // Now expect variable name
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected variable name after generic type at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            stmt->type = STMT_DECLARATION;
            Declaration* decl = malloc(sizeof(Declaration));
            decl->struct_name = name; // Generic struct name (e.g., Box)
            decl->type = TYPE_NUMERIC; // Placeholder
            decl->name = malloc(strlen(parser->current_token->value) + 1);
            strcpy(decl->name, parser->current_token->value);
            decl->is_array = 0;
            decl->is_list = 0;
            decl->array_size = 0;
            decl->array_init = NULL;
            decl->array_init_count = 0;
            decl->is_pointer = 0;
            decl->is_nullable = 0;
            decl->is_union = 0;
            decl->union_types = NULL;
            decl->union_count = 0;
            decl->is_exported = 1;
            decl->init_value = NULL;
            decl->struct_type_args = type_args;
            decl->struct_type_arg_count = type_arg_count;
            parser_advance(parser);
            
            // Check for initialization
            if (parser->current_token->type == TOKEN_ASSIGN) {
                parser_advance(parser);
                decl->init_value = parser_parse_expression(parser);
            }
            
            stmt->declaration = decl;
        }
        // Check if it's a struct/type alias instance declaration (e.g., "Person p" or "PersonId id")
        else if (parser->current_token->type == TOKEN_IDENTIFIER) {
            // This is a struct/alias instance: "TypeName varName"
            // name = type name (struct or alias), current_token = variable name
            stmt->type = STMT_DECLARATION;
            Declaration* decl = malloc(sizeof(Declaration));
            decl->struct_name = name; // Type name (struct or alias)
            decl->type = TYPE_NUMERIC; // Placeholder
            decl->name = malloc(strlen(parser->current_token->value) + 1);
            strcpy(decl->name, parser->current_token->value);
            decl->is_array = 0;
            decl->is_list = 0;
            decl->array_size = 0;
            decl->array_init = NULL;
            decl->array_init_count = 0;
            decl->is_pointer = 0;
            decl->is_nullable = 0;
            decl->is_union = 0;
            decl->union_types = NULL;
            decl->union_count = 0;
            decl->is_exported = 1;
            decl->init_value = NULL;
            decl->struct_type_args = NULL;
            decl->struct_type_arg_count = 0;
            parser_advance(parser);
            
            // Check for initialization
            if (parser->current_token->type == TOKEN_ASSIGN) {
                parser_advance(parser);
                decl->init_value = parser_parse_expression(parser);
            }
            
            stmt->declaration = decl;
        }
        // Check if it's a function call (followed by '(')
        else if (parser->current_token->type == TOKEN_LPAREN) {
            parser_advance(parser); // skip '('
            
            // Parse arguments
            Expression** args = malloc(sizeof(Expression*) * 10);
            int arg_count = 0;
            int arg_capacity = 10;
            
            if (parser->current_token->type != TOKEN_RPAREN) {
                while (1) {
                    if (arg_count >= arg_capacity) {
                        arg_capacity *= 2;
                        args = realloc(args, sizeof(Expression*) * arg_capacity);
                    }
                    args[arg_count++] = parser_parse_expression(parser);
                    
                    if (parser->current_token->type == TOKEN_COMMA) {
                        parser_advance(parser);
                    } else {
                        break;
                    }
                }
            }
            
            if (parser->current_token->type != TOKEN_RPAREN) {
                fprintf(stderr, "Parser error: Expected ')' after function arguments at line %d\\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip ')'
            
            // Statement-level function call (discard return value)
            stmt->type = STMT_EXPR_STMT;
            stmt->print_stmt.expr = expression_create_func_call(name, args, arg_count);
            free(name);
        } else if (parser->current_token->type == TOKEN_LBRACKET) {
            // Array assignment: arr[index] = value
            parser_advance(parser); // skip '['
            
            Expression* index = parser_parse_expression(parser);
            
            if (parser->current_token->type != TOKEN_RBRACKET) {
                fprintf(stderr, "Parser error: Expected ']' after array index at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip ']'
            
            if (parser->current_token->type != TOKEN_ASSIGN) {
                fprintf(stderr, "Parser error: Expected '=' after array index at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip '='
            
            stmt->type = STMT_ASSIGNMENT;
            stmt->assignment.var_name = name;
            stmt->assignment.field_name = NULL;
            stmt->assignment.array_index = index;  // Set array index
            stmt->assignment.dereference_target = NULL;
            stmt->assignment.value = parser_parse_expression(parser);
        } else if (parser->current_token->type == TOKEN_DOT) {
            // Field assignment: object.field = value
            parser_advance(parser); // skip '.'
            
            if (parser->current_token->type != TOKEN_IDENTIFIER) {
                fprintf(stderr, "Parser error: Expected field name after '.' at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            
            char* field_name = malloc(strlen(parser->current_token->value) + 1);
            strcpy(field_name, parser->current_token->value);
            parser_advance(parser);
            
            if (parser->current_token->type != TOKEN_ASSIGN) {
                fprintf(stderr, "Parser error: Expected '=' after field name at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser); // skip '='
            
            stmt->type = STMT_ASSIGNMENT;
            stmt->assignment.var_name = name;
            stmt->assignment.field_name = field_name;  // Set field name
            stmt->assignment.array_index = NULL;
            stmt->assignment.dereference_target = NULL;
            stmt->assignment.value = parser_parse_expression(parser);
        } else if (parser->current_token->type == TOKEN_COMMA) {
            // Multi-assignment: a, b, c = func()
            stmt->type = STMT_MULTI_ASSIGNMENT;
            stmt->multi_assignment.var_names = malloc(sizeof(char*) * 10);
            stmt->multi_assignment.var_count = 0;
            int var_capacity = 10;
            
            // First variable (already parsed)
            stmt->multi_assignment.var_names[stmt->multi_assignment.var_count++] = name;
            
            // Parse remaining variables
            while (parser->current_token->type == TOKEN_COMMA) {
                parser_advance(parser); // skip ','
                
                if (parser->current_token->type != TOKEN_IDENTIFIER) {
                    fprintf(stderr, "Parser error: Expected variable name after ',' at line %d\n",
                            parser->current_token->line);
                    exit(1);
                }
                
                if (stmt->multi_assignment.var_count >= var_capacity) {
                    var_capacity *= 2;
                    stmt->multi_assignment.var_names = realloc(stmt->multi_assignment.var_names,
                                                              sizeof(char*) * var_capacity);
                }
                
                stmt->multi_assignment.var_names[stmt->multi_assignment.var_count] = 
                    malloc(strlen(parser->current_token->value) + 1);
                strcpy(stmt->multi_assignment.var_names[stmt->multi_assignment.var_count],
                       parser->current_token->value);
                stmt->multi_assignment.var_count++;
                parser_advance(parser);
            }
            
            // Expect '='
            if (parser->current_token->type != TOKEN_ASSIGN) {
                fprintf(stderr, "Parser error: Expected '=' after variable list at line %d\n",
                        parser->current_token->line);
                exit(1);
            }
            parser_advance(parser);
            
            // Parse right-hand side (should be a function call)
            stmt->multi_assignment.func_call = parser_parse_expression(parser);
        } else if (parser->current_token->type == TOKEN_ASSIGN) {
            // Assignment
            parser_advance(parser);
            stmt->type = STMT_ASSIGNMENT;
            stmt->assignment.var_name = name;
            stmt->assignment.field_name = NULL;  // Regular variable assignment
            stmt->assignment.array_index = NULL;
            stmt->assignment.dereference_target = NULL;
            stmt->assignment.value = parser_parse_expression(parser);
        } else {
            fprintf(stderr, "Parser error: Expected '=' or '(' after identifier at line %d\n",
                    parser->current_token->line);
            free(name);
            exit(1);
        }
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
