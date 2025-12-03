// MELP Bootstrap Compiler
// Parser Header - Type definitions for modular compilation
// Generated from parser.c type definitions

#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for self-referential structs
typedef struct Expression Expression;
typedef struct Statement Statement;
typedef struct SwitchCase SwitchCase;
typedef struct MethodSignature MethodSignature;

// ============================================================================
// User-visible types (what user writes in code)
// ============================================================================
typedef enum {
    TYPE_NUMERIC,
    TYPE_DECIMAL,
    TYPE_BOOLEAN,
    TYPE_STRING,  // Phase 5: String type
    TYPE_STRUCT,  // Phase 6: User-defined struct type
    TYPE_VOID,    // For functions with no return
    TYPE_ARRAY,   // Array type
    TYPE_LIST,    // Phase 21: List type (heterojen, mutable)
    TYPE_TUPLE,   // Phase 21: Tuple type (heterojen, immutable)
    TYPE_POINTER, // Phase 10: Pointer type
    TYPE_ANY,     // Phase 11: Any type (for union/dynamic)
    TYPE_ENUM,    // Phase 10: Enum type
    TYPE_GENERIC  // Phase 13: Generic type parameter (T, U, etc.)
} VarType;

// ============================================================================
// TTO: Internal numeric representation (transparent to user)
// ============================================================================
typedef enum {
    INTERNAL_INT64,      // Small integers: -2^63 to 2^63-1
    INTERNAL_DOUBLE,     // Floating point: ~15 digits precision
    INTERNAL_BIGDECIMAL  // Arbitrary precision (future)
} InternalNumericType;

// ============================================================================
// TTO: Internal string representation (transparent to user)
// ============================================================================
typedef enum {
    INTERNAL_SSO,        // Small String Optimization: ≤23 bytes inline
    INTERNAL_HEAP,       // Heap allocated: >23 bytes
    INTERNAL_RODATA      // Read-only data section: string literals
} InternalStringType;

// ============================================================================
// Statement types
// ============================================================================
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

// ============================================================================
// Expression types
// ============================================================================
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

// ============================================================================
// Binary operators
// ============================================================================
typedef enum {
    BIN_OP_ADD,
    BIN_OP_SUB,
    BIN_OP_MUL,
    BIN_OP_DIV,
    BIN_OP_MOD,      // Modulo operator
    BIN_OP_BITAND,   // Bitwise AND
    BIN_OP_BITOR,    // Bitwise OR
    BIN_OP_BITXOR,   // Bitwise XOR
    BIN_OP_SHL,      // Shift left
    BIN_OP_SHR       // Shift right
} BinaryOp;

// ============================================================================
// Comparison operators
// ============================================================================
typedef enum {
    CMP_EQUAL,
    CMP_NOT_EQUAL,
    CMP_LESS,
    CMP_LESS_EQUAL,
    CMP_GREATER,
    CMP_GREATER_EQUAL
} ComparisonOp;

// ============================================================================
// Expression structure
// ============================================================================
struct Expression {
    ExprType type;
    union {
        long number_value;
        char* var_name;
        char* string_value;  // Phase 5: String literal value
        struct {
            Expression* left;
            Expression* right;
            ComparisonOp op;
        } comparison;
        struct {
            Expression* left;
            Expression* right;
            BinaryOp op;
        } binary_op;
        struct {
            char* func_name;
            Expression** args;
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
            Expression* condition;
            Expression* true_expr;
            Expression* false_expr;
        } ternary;
        struct {
            char* array_name;           // Array variable name
            Expression* index;   // Index expression
        } array_index;
        Expression* unary_operand;  // Phase 10: For & and * operators
        struct {
            Expression* left;   // For AND/OR
            Expression* right;  // For AND/OR
        } logical_binary;
        Expression* logical_not_operand;  // For NOT
        struct {
            char** param_names;        // Parameter names
            int param_count;           // Number of parameters
            Expression* body;   // Lambda body (single expression)
            char** captured_vars;      // Captured variable names (for closures)
            int captured_count;        // Number of captured variables
        } lambda;
        struct {
            Expression* awaited_expr;  // Expression to await (usually func call)
        } await_expr;
        struct {
            Expression** elements;  // Array literal elements
            int count;                      // Number of elements
        } array_literal;
        // Phase 22: List and Tuple literals
        struct {
            Expression** elements;  // List literal elements (heterojen, mutable)
            int count;                      // Number of elements
        } list_literal;
        struct {
            Expression** elements;  // Tuple literal elements (heterojen, immutable)
            int count;                      // Number of elements
        } tuple_literal;
        // Phase 15: Null safety
        struct {
            Expression* object;      // Object expression (e.g., person in person?.name)
            char* field_name;               // Field name (e.g., "name")
        } optional_chain;
        struct {
            Expression* left;        // Value that might be null
            Expression* right;       // Default value if null
        } null_coalesce;
    };
    // TTO: Internal type information (transparent to user)
    InternalNumericType internal_numeric_type;  // For EXPR_NUMBER
    InternalStringType internal_string_type;    // For EXPR_STRING
    double double_value;                        // For INTERNAL_DOUBLE numbers
    int has_decimal_point;                      // 1 if literal had decimal point
};

// ============================================================================
// Declaration structure
// ============================================================================
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

// ============================================================================
// Phase 6: Struct field definition
// ============================================================================
typedef struct {
    VarType type;
    char* name;
    int is_generic;         // Phase 13: Is this field a generic type parameter?
    int generic_index;      // Phase 13: Index into type_params if generic
} StructField;

// ============================================================================
// Interface method signature
// ============================================================================
struct MethodSignature {
    char* name;             // Method name
    VarType* param_types;   // Parameter types
    char** param_names;     // Parameter names
    int param_count;        // Number of parameters
    VarType return_type;    // Return type
    int has_return;         // 1 if returns value, 0 if void
};

// ============================================================================
// Phase 10: Enum member definition
// ============================================================================
typedef struct {
    char* name;     // Member name (e.g., "Active")
    long value;     // Member value (e.g., 1)
} EnumMember;

// ============================================================================
// Switch case definition
// ============================================================================
struct SwitchCase {
    Expression* value;      // Case value to match (NULL for range or type matching)
    Expression* guard;      // Guard condition (if clause) - NULL if no guard
    Expression* range_start; // Range start (for case 1..10) - NULL if not range
    Expression* range_end;   // Range end (for case 1..10) - NULL if not range
    int is_type_check;      // 1 if this is "case is Type", 0 otherwise
    char* type_name;        // Type name for type checking (NULL if not type check)
    Statement** body;  // Case body statements
    int body_count;
};

// ============================================================================
// Statement structure
// ============================================================================
struct Statement {
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
            Statement** then_body;
            int then_count;
            Statement** else_body;
            int else_count;
        } if_stmt;
        struct {
            char* var_name;       // Loop variable (e.g., "i")
            Expression* start;     // Start value
            Expression* end;       // End value
            Expression* step;      // Step value (NULL = default 1)
            Statement** body;
            int body_count;
        } for_stmt;
        struct {
            Expression* condition;
            Statement** body;
            int body_count;
        } while_stmt;
        struct {
            Expression* condition;
            Statement** body;
            int body_count;
        } do_while_stmt;
        struct {
            Expression* value;      // Switch expression to evaluate
            SwitchCase** cases;  // Array of cases
            int case_count;
            Statement** default_body;  // Default case body (NULL if no default)
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
            Statement** body;
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
            MethodSignature** methods;
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
            Statement** try_body;
            int try_count;
            // Multiple catch blocks
            struct {
                char* exception_type;      // Exception type (NULL = catch all)
                char* exception_var;       // Variable name to hold exception
                Statement** body;
                int body_count;
            }* catch_blocks;
            int catch_count;
            // Finally block
            Statement** finally_body;
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
            Statement** body;   // Debug if body
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
            Statement** body;   // Loop body
            int body_count;
        } for_in;
        struct {
            char* module_name;         // Module name (e.g., "MyModule")
            Statement** body;   // Module body (functions, structs, etc.)
            int body_count;
        } module_def;
        // Phase 16: Operator Overloading
        struct {
            char* op;                  // Operator symbol (+, -, *, /, ==, etc.)
            char* type_name;           // Type name this operator is for
            char* param1_name;         // First parameter name
            char* param2_name;         // Second parameter name (NULL for unary)
            VarType return_type;       // Return type
            Statement** body;   // Operator body
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
                Statement** body; // Case body statements
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
};

// ============================================================================
// AST structure
// ============================================================================
typedef struct {
    Statement** statements;
    int count;
    int capacity;
} AST;

// ============================================================================
// Parser structure
// ============================================================================
typedef struct {
    void* lexer;          // Lexer* (forward declared to avoid circular dependency)
    void* current_token;  // Token* (forward declared to avoid circular dependency)
} Parser;

// ============================================================================
// Function prototypes - Parser API
// ============================================================================

// Parser creation and management
Parser* parser_create(void* lexer);
void parser_advance(Parser* parser);
void parser_free(Parser* parser);

// AST creation and management
AST* ast_create(void);
void ast_add_statement(AST* ast, Statement* stmt);
void ast_free(AST* ast);

// Expression creation helpers
Expression* expression_create_number(long value);
Expression* expression_create_double(double value);
Expression* expression_create_variable(const char* name);
Expression* expression_create_string(const char* value);
Expression* expression_create_field_access(const char* object_name, const char* field_name);
Expression* expression_create_ternary(Expression* condition, Expression* true_expr, Expression* false_expr);
Expression* expression_create_array_index(const char* array_name, Expression* index);
Expression* expression_create_null(void);

// Parsing functions
Expression* parse_expression(Parser* parser);
Expression* parse_primary(Parser* parser);
Statement* parse_statement(Parser* parser);
AST* parse_program(Parser* parser);

#endif // PARSER_H
