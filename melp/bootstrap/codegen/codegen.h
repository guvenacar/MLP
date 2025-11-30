// MELP Modular Codegen - Header
// x86-64 Assembly code generation

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations from parser
typedef struct Expression Expression;
typedef struct Statement Statement;
typedef struct Declaration Declaration;
typedef struct AST AST;
typedef struct MethodSignature MethodSignature;
typedef enum VarType VarType;
typedef enum InternalNumericType InternalNumericType;
typedef enum InternalStringType InternalStringType;

// ============================================================================
// Data Structures
// ============================================================================

typedef struct VarInfo {
    char* name;
    int stack_offset;
    VarType type;
    char* struct_name;
    int is_dynamic_array;
    int is_pointer;
    int is_closure;
    int is_array;
    int array_size;
    int is_global;
    int is_state;
    Expression* init_expr;
    InternalNumericType internal_numeric_type;
    InternalStringType internal_string_type;
    int is_double;
    struct VarInfo* next;
} VarInfo;

typedef struct FuncInfo {
    char* name;
    int param_count;
    VarType* param_types;
    struct FuncInfo* next;
} FuncInfo;

typedef struct StructInfo {
    char* name;
    int field_count;
    VarType* field_types;
    char** field_names;
    int* field_offsets;
    int total_size;
    char** implements;
    int implements_count;
    struct StructInfo* next;
} StructInfo;

typedef struct InterfaceInfo {
    char* name;
    MethodSignature** methods;
    int method_count;
    struct InterfaceInfo* next;
} InterfaceInfo;

typedef struct EnumValue {
    char* enum_name;
    char* member_name;
    long value;
    struct EnumValue* next;
} EnumValue;

typedef struct TypeAlias {
    char* alias_name;
    int base_type;
    char* struct_name;
    struct TypeAlias* next;
} TypeAlias;

typedef struct StringLiteral {
    int id;
    char* value;
    struct StringLiteral* next;
} StringLiteral;

typedef struct ModuleTracker {
    char* module_name;
    struct ModuleTracker* next;
} ModuleTracker;

typedef struct GenericFunc {
    char* name;
    Statement* func_stmt;
    char** type_params;
    int type_param_count;
    struct GenericFunc* next;
} GenericFunc;

typedef struct GenericInstance {
    char* original_name;
    char* mangled_name;
    VarType* type_args;
    int type_arg_count;
    int generated;
    struct GenericInstance* next;
} GenericInstance;

typedef struct OperatorOverload {
    char* op;
    char* type_name;
    char* func_name;
    struct OperatorOverload* next;
} OperatorOverload;

typedef struct StateVarInfo {
    char* name;
    VarType type;
    int is_global;
    int is_state;
    Expression* init_expr;
} StateVarInfo;

#define MAX_VARS 256
#define MAX_STATE_VARS 64

typedef struct Codegen {
    FILE* out;
    int stack_offset;
    VarInfo* variables;
    FuncInfo* functions;
    StructInfo* structs;
    InterfaceInfo* interfaces;
    EnumValue* enums;
    TypeAlias* type_aliases;
    StringLiteral* strings;
    ModuleTracker* modules_seen;
    GenericFunc* generic_funcs;
    GenericInstance* generic_instances;
    OperatorOverload* operator_overloads;
    int label_counter;
    int loop_start_label;
    int loop_continue_label;
    int loop_end_label;
    int in_loop;
    int in_function;
    int in_operator_overload;
    int in_generator;
    int string_counter;
    char* current_module;
    StateVarInfo global_vars[MAX_STATE_VARS];
    int global_var_count;
} Codegen;

// ============================================================================
// codegen_core.c - Core Functions
// ============================================================================

Codegen* codegen_create(const char* output_file);
void codegen_free(Codegen* gen);
void codegen_emit(Codegen* gen, const char* line);
int codegen_add_string(Codegen* gen, const char* value);

// ============================================================================
// codegen_variable.c - Variable Management
// ============================================================================

void codegen_add_variable(Codegen* gen, const char* name, int offset, VarType type, const char* struct_name);
void codegen_add_variable_tto(Codegen* gen, const char* name, int offset, VarType type, InternalNumericType internal_type);
void codegen_add_variable_tto_string(Codegen* gen, const char* name, int offset, VarType type, InternalStringType str_type);
void codegen_add_dynamic_array(Codegen* gen, const char* name, int offset, VarType type);
void codegen_add_pointer_variable(Codegen* gen, const char* name, int offset, VarType type);
void codegen_add_struct_variable(Codegen* gen, const char* name, int offset, const char* struct_name);
void codegen_add_stack_array(Codegen* gen, const char* name, int offset, VarType type, int array_size);
int codegen_find_variable(Codegen* gen, const char* name);
VarInfo* codegen_get_variable(Codegen* gen, const char* name);

// ============================================================================
// codegen_function.c - Function Table Management
// ============================================================================

void codegen_add_function(Codegen* gen, const char* name, int param_count, VarType* param_types);
FuncInfo* codegen_find_function(Codegen* gen, const char* name);

// ============================================================================
// codegen_struct.c - Struct Management
// ============================================================================

void codegen_add_struct(Codegen* gen, const char* name, int field_count, 
                        VarType* field_types, char** field_names, 
                        char** implements, int implements_count);
StructInfo* codegen_find_struct(Codegen* gen, const char* name);
int codegen_get_field_offset(Codegen* gen, const char* struct_name, const char* field_name);
VarType codegen_get_field_type(Codegen* gen, const char* struct_name, const char* field_name);

// ============================================================================
// codegen_interface.c - Interface Management
// ============================================================================

void codegen_add_interface(Codegen* gen, const char* name, MethodSignature** methods, int method_count);
InterfaceInfo* codegen_find_interface(Codegen* gen, const char* name);

// ============================================================================
// codegen_enum.c - Enum Management
// ============================================================================

void codegen_add_enum_value(Codegen* gen, const char* enum_name, const char* member_name, long value);
long codegen_find_enum_value(Codegen* gen, const char* enum_name, const char* member_name);

// ============================================================================
// codegen_type.c - Type Alias & Generic Management
// ============================================================================

void codegen_add_type_alias(Codegen* gen, const char* alias_name, int base_type, const char* struct_name);
TypeAlias* codegen_find_type_alias(Codegen* gen, const char* alias_name);
void codegen_add_generic_func(Codegen* gen, const char* name, Statement* stmt);
GenericFunc* codegen_find_generic_func(Codegen* gen, const char* name);
GenericInstance* codegen_find_generic_instance(Codegen* gen, const char* name, VarType* type_args, int type_arg_count);
char* codegen_mangle_generic_name(const char* name, VarType* type_args, int type_arg_count);
void codegen_add_generic_instance(Codegen* gen, const char* original_name, const char* mangled_name, VarType* type_args, int type_arg_count);
const char* codegen_type_name(VarType type);

// ============================================================================
// codegen_module.c - Module Tracking
// ============================================================================

int codegen_module_seen(Codegen* gen, const char* module_name);
void codegen_mark_module_seen(Codegen* gen, const char* module_name);

// ============================================================================
// codegen_operator.c - Operator Overloading
// ============================================================================

void codegen_add_operator_overload(Codegen* gen, const char* op, const char* type_name, const char* func_name);
OperatorOverload* codegen_find_operator_overload(Codegen* gen, const char* op, const char* type_name);

// ============================================================================
// codegen_prologue.c - Assembly Prologue/Epilogue
// ============================================================================

void codegen_emit_prologue(Codegen* gen);
void codegen_emit_epilogue(Codegen* gen);

// ============================================================================
// codegen_decl.c - Declaration Generation
// ============================================================================

void codegen_generate_declaration(Codegen* gen, Declaration* decl);

// ============================================================================
// codegen_assign.c - Assignment Generation
// ============================================================================

void codegen_generate_assignment(Codegen* gen, char* var_name, char* field_name, 
                                  Expression* array_index, Expression* dereference_target, 
                                  Expression* value);

// ============================================================================
// codegen_print.c - Print Generation
// ============================================================================

void codegen_generate_print(Codegen* gen, Expression* expr);

// ============================================================================
// codegen_expr.c - Expression Generation
// ============================================================================

void codegen_generate_expression_value(Codegen* gen, Expression* expr);

// ============================================================================
// codegen_compare.c - Comparison Generation
// ============================================================================

void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);

// ============================================================================
// codegen_if.c - If/Else Generation
// ============================================================================

void codegen_generate_if(Codegen* gen, Statement* stmt);

// ============================================================================
// codegen_for.c - For Loop Generation
// ============================================================================

void codegen_generate_for(Codegen* gen, Statement* stmt);
void codegen_generate_for_in(Codegen* gen, Statement* stmt);

// ============================================================================
// codegen_while.c - While/Do-While Generation
// ============================================================================

void codegen_generate_while(Codegen* gen, Statement* stmt);
void codegen_generate_do_while(Codegen* gen, Statement* stmt);

// ============================================================================
// codegen_switch.c - Switch/Case Generation
// ============================================================================

void codegen_generate_switch(Codegen* gen, Statement* stmt);

// ============================================================================
// codegen_funcdef.c - Function Definition Generation
// ============================================================================

void codegen_generate_func_def(Codegen* gen, Statement* stmt);

// ============================================================================
// codegen_return.c - Return Generation
// ============================================================================

void codegen_generate_return(Codegen* gen, Statement* stmt);

// ============================================================================
// codegen_stmt.c - Statement Dispatcher
// ============================================================================

void codegen_generate_statement(Codegen* gen, Statement* stmt);

// ============================================================================
// codegen_main.c - Main Generate Function
// ============================================================================

void codegen_generate(Codegen* gen, AST* ast);

#endif // CODEGEN_H
