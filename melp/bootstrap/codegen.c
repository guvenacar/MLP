// MELP Phase 1 - Bootstrap Compiler  
// Codegen: x86-64 Assembly üretir (declarations + assignments + print)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.c"

typedef struct VarInfo {
    char* name;
    int stack_offset;
    VarType type;  // Variable type
    char* struct_name; // NULL for non-struct, struct type name for struct instances
    int is_dynamic_array; // 1 if dynamic array (pointer), 0 otherwise
    int is_pointer;       // Phase 10: 1 if pointer type, 0 otherwise
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
    int* field_offsets;  // Offset of each field in bytes
    int total_size;      // Total size of struct in bytes
    struct StructInfo* next;
} StructInfo;

typedef struct EnumValue {
    char* enum_name;    // Enum type name (e.g., "Status")
    char* member_name;  // Member name (e.g., "Active")
    long value;         // Member value (e.g., 1)
    struct EnumValue* next;
} EnumValue;

typedef struct TypeAlias {
    char* alias_name;     // e.g., "PersonId"
    int base_type;        // VarType (TYPE_NUMERIC, etc.)
    char* struct_name;    // NULL for primitives, struct name for struct aliases
    struct TypeAlias* next;
} TypeAlias;

typedef struct StringLiteral {
    int id;
    char* value;
    struct StringLiteral* next;
} StringLiteral;

typedef struct {
    FILE* out;
    int stack_offset;
    VarInfo* variables;      // Symbol table
    FuncInfo* functions;     // Function table
    StructInfo* structs;     // Struct type table
    EnumValue* enums;        // Enum values table
    TypeAlias* type_aliases; // Type aliases table
    StringLiteral* strings;  // String literals
    int label_counter;       // For unique labels
    int loop_start_label;    // Current loop start (condition check)
    int loop_continue_label; // Current loop continue (increment/next iteration)
    int loop_end_label;      // Current loop end for exit
    int in_loop;             // Are we inside a loop?
    int in_function;         // Are we inside a function?
    int string_counter;      // For unique string labels
    char* current_module;    // Current module name (NULL if not in module)
} Codegen;

Codegen* codegen_create(const char* output_file) {
    Codegen* gen = malloc(sizeof(Codegen));
    if (!gen) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    
    gen->out = fopen(output_file, "w");
    if (!gen->out) {
        fprintf(stderr, "Error: Cannot open output file '%s'\n", output_file);
        perror("fopen");
        exit(1);
    }
    
    gen->stack_offset = 0;
    gen->variables = NULL;
    gen->functions = NULL;
    gen->structs = NULL;
    gen->enums = NULL;
    gen->type_aliases = NULL;
    gen->strings = NULL;
    gen->label_counter = 0;
    gen->loop_start_label = -1;
    gen->loop_continue_label = -1;
    gen->loop_end_label = -1;
    gen->in_loop = 0;
    gen->in_function = 0;
    gen->string_counter = 0;
    gen->current_module = NULL;
    return gen;
}

void codegen_add_variable(Codegen* gen, const char* name, int offset, VarType type) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->next = gen->variables;
    gen->variables = var;
}

void codegen_add_dynamic_array(Codegen* gen, const char* name, int offset, VarType type) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 1;
    var->is_pointer = 0;
    var->next = gen->variables;
    gen->variables = var;
}

// Phase 10: Add pointer variable
void codegen_add_pointer_variable(Codegen* gen, const char* name, int offset, VarType type) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->stack_offset = offset;
    var->type = type;
    var->struct_name = NULL;
    var->is_dynamic_array = 0;
    var->is_pointer = 1;
    var->next = gen->variables;
    gen->variables = var;
}

void codegen_add_struct_variable(Codegen* gen, const char* name, int offset, const char* struct_name) {
    VarInfo* var = malloc(sizeof(VarInfo));
    var->name = malloc(strlen(name) + 1);
    strcpy(var->name, name);
    var->is_dynamic_array = 0;
    var->is_pointer = 0;
    var->stack_offset = offset;
    var->type = TYPE_NUMERIC; // Placeholder
    var->struct_name = malloc(strlen(struct_name) + 1);
    strcpy(var->struct_name, struct_name);
    var->next = gen->variables;
    gen->variables = var;
}

int codegen_find_variable(Codegen* gen, const char* name) {
    VarInfo* current = gen->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current->stack_offset;
        }
        current = current->next;
    }
    fprintf(stderr, "Codegen error: Undefined variable '%s'\n", name);
    exit(1);
}

VarInfo* codegen_get_variable(Codegen* gen, const char* name) {
    VarInfo* current = gen->variables;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    fprintf(stderr, "Codegen error: Undefined variable '%s'\n", name);
    exit(1);
}

void codegen_add_enum_value(Codegen* gen, const char* enum_name, const char* member_name, long value) {
    EnumValue* ev = malloc(sizeof(EnumValue));
    ev->enum_name = malloc(strlen(enum_name) + 1);
    strcpy(ev->enum_name, enum_name);
    ev->member_name = malloc(strlen(member_name) + 1);
    strcpy(ev->member_name, member_name);
    ev->value = value;
    ev->next = gen->enums;
    gen->enums = ev;
}

long codegen_find_enum_value(Codegen* gen, const char* enum_name, const char* member_name) {
    EnumValue* current = gen->enums;
    while (current) {
        if (strcmp(current->enum_name, enum_name) == 0 &&
            strcmp(current->member_name, member_name) == 0) {
            return current->value;
        }
        current = current->next;
    }
    fprintf(stderr, "Codegen error: Undefined enum value '%s.%s'\n", enum_name, member_name);
    exit(1);
}

void codegen_add_type_alias(Codegen* gen, const char* alias_name, int base_type, const char* struct_name) {
    TypeAlias* alias = malloc(sizeof(TypeAlias));
    alias->alias_name = malloc(strlen(alias_name) + 1);
    strcpy(alias->alias_name, alias_name);
    alias->base_type = base_type;
    
    if (struct_name) {
        alias->struct_name = malloc(strlen(struct_name) + 1);
        strcpy(alias->struct_name, struct_name);
    } else {
        alias->struct_name = NULL;
    }
    
    alias->next = gen->type_aliases;
    gen->type_aliases = alias;
}

TypeAlias* codegen_find_type_alias(Codegen* gen, const char* alias_name) {
    TypeAlias* current = gen->type_aliases;
    while (current) {
        if (strcmp(current->alias_name, alias_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Not found, might be a regular type
}

void codegen_emit(Codegen* gen, const char* line) {
    fprintf(gen->out, "%s\n", line);
}

int codegen_add_string(Codegen* gen, const char* value) {
    int id = gen->string_counter++;
    StringLiteral* str = malloc(sizeof(StringLiteral));
    str->id = id;
    str->value = malloc(strlen(value) + 1);
    strcpy(str->value, value);
    str->next = gen->strings;
    gen->strings = str;
    return id;
}

void codegen_add_function(Codegen* gen, const char* name, int param_count, VarType* param_types) {
    FuncInfo* func = malloc(sizeof(FuncInfo));
    func->name = malloc(strlen(name) + 1);
    strcpy(func->name, name);
    func->param_count = param_count;
    func->param_types = malloc(sizeof(VarType) * param_count);
    memcpy(func->param_types, param_types, sizeof(VarType) * param_count);
    func->next = gen->functions;
    gen->functions = func;
}

FuncInfo* codegen_find_function(Codegen* gen, const char* name) {
    FuncInfo* current = gen->functions;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void codegen_add_struct(Codegen* gen, const char* name, int field_count, 
                       VarType* field_types, char** field_names) {
    StructInfo* st = malloc(sizeof(StructInfo));
    st->name = malloc(strlen(name) + 1);
    strcpy(st->name, name);
    st->field_count = field_count;
    
    // Allocate and copy field info
    st->field_types = malloc(sizeof(VarType) * field_count);
    st->field_names = malloc(sizeof(char*) * field_count);
    st->field_offsets = malloc(sizeof(int) * field_count);
    
    int offset = 0;
    for (int i = 0; i < field_count; i++) {
        st->field_types[i] = field_types[i];
        st->field_names[i] = malloc(strlen(field_names[i]) + 1);
        strcpy(st->field_names[i], field_names[i]);
        
        // Calculate offset (all types are 8 bytes for now)
        st->field_offsets[i] = offset;
        offset += 8;
    }
    
    st->total_size = offset;
    st->next = gen->structs;
    gen->structs = st;
}

StructInfo* codegen_find_struct(Codegen* gen, const char* name) {
    StructInfo* current = gen->structs;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Forward declaration
void codegen_generate_statement(Codegen* gen, Statement* stmt);
void codegen_generate_expression_value(Codegen* gen, Expression* expr);
void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);

void codegen_emit_prologue(Codegen* gen) {
    codegen_emit(gen, "section .data");
    codegen_emit(gen, "    ; String literals will be added here");
    codegen_emit(gen, "");
    codegen_emit(gen, "section .text");
    codegen_emit(gen, "extern print_number");
    codegen_emit(gen, "extern print_string");
    codegen_emit(gen, "extern string_length");
    codegen_emit(gen, "extern string_concat");
    codegen_emit(gen, "extern string_equal");
    codegen_emit(gen, "extern string_not_equal");
    codegen_emit(gen, "extern int_to_string");
    codegen_emit(gen, "extern malloc");
    codegen_emit(gen, "extern free");
    codegen_emit(gen, "extern mlp_array_alloc");
    codegen_emit(gen, "extern mlp_array_free");
    codegen_emit(gen, "extern mlp_array_length");
    codegen_emit(gen, "extern mlp_array_resize");
    codegen_emit(gen, "extern mlp_file_read");
    codegen_emit(gen, "extern mlp_file_write");
    codegen_emit(gen, "extern mlp_file_exists");
    codegen_emit(gen, "extern mlp_file_append");
    codegen_emit(gen, "extern mlp_substring");
    codegen_emit(gen, "extern mlp_indexOf");
    codegen_emit(gen, "extern mlp_charAt");
    codegen_emit(gen, "extern mlp_string_length");
    codegen_emit(gen, "extern mlp_get_argv");
    codegen_emit(gen, "extern mlp_get_argc");
    codegen_emit(gen, "extern setjmp");
    codegen_emit(gen, "extern strcmp");
    codegen_emit(gen, "extern mlp_exception_push");
    codegen_emit(gen, "extern mlp_exception_pop");
    codegen_emit(gen, "extern mlp_throw");
    codegen_emit(gen, "extern mlp_exception_type");
    codegen_emit(gen, "extern mlp_exception_message");
    codegen_emit(gen, "extern mlp_exception_code");
    codegen_emit(gen, "extern mlp_exception_has_handler");
    codegen_emit(gen, "extern mlp_exception_has_parent_handler");
    codegen_emit(gen, "global _start");
}

// Helper function to check if a function name is a builtin/runtime function
int is_builtin_function(const char* name) {
    static const char* builtins[] = {
        "print_number", "print_string", "string_length", "string_concat",
        "string_equal", "string_not_equal", "int_to_string",
        "malloc", "free",
        "mlp_array_alloc", "mlp_array_free", "mlp_array_length", "mlp_array_resize",
        "mlp_file_read", "mlp_file_write", "mlp_file_exists", "mlp_file_append",
        "mlp_substring", "mlp_indexOf", "mlp_charAt", "mlp_string_length",
        "mlp_get_argv", "mlp_get_argc",
        "setjmp", "strcmp",
        "mlp_exception_push", "mlp_exception_pop", "mlp_throw",
        "mlp_exception_type", "mlp_exception_message", "mlp_exception_code",
        "mlp_exception_has_handler", "mlp_exception_has_parent_handler",
        NULL
    };
    
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void codegen_emit_epilogue(Codegen* gen) {
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Exit program");
    codegen_emit(gen, "    mov rax, 60        ; sys_exit");
    codegen_emit(gen, "    xor rdi, rdi       ; exit code 0");
    codegen_emit(gen, "    syscall");
}

void codegen_generate_declaration(Codegen* gen, Declaration* decl) {
    char buffer[256];
    
    // Phase 11: Union types - currently treated as first type (no runtime type checking)
    if (decl->is_union) {
        snprintf(buffer, sizeof(buffer), 
                 "    ; Union type variable '%s' (types: ", decl->name);
        codegen_emit(gen, buffer);
        
        for (int i = 0; i < decl->union_count; i++) {
            const char* type_str = decl->union_types[i] == TYPE_NUMERIC ? "numeric" :
                                   decl->union_types[i] == TYPE_DECIMAL ? "decimal" :
                                   decl->union_types[i] == TYPE_BOOLEAN ? "boolean" : "text";
            snprintf(buffer, sizeof(buffer), "    ;   - %s", type_str);
            codegen_emit(gen, buffer);
        }
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Note: Union types use first type for storage (runtime checking not implemented)");
        codegen_emit(gen, buffer);
    }
    
    // Resolve type alias if present
    const char* actual_struct_name = decl->struct_name;
    VarType actual_type = decl->type;
    
    if (decl->struct_name) {
        // Check if this is a type alias
        TypeAlias* alias = codegen_find_type_alias(gen, decl->struct_name);
        if (alias) {
            // Resolve alias to actual type
            actual_type = alias->base_type;
            actual_struct_name = alias->struct_name; // May be NULL (primitive) or another struct name
        }
    }
    
    // Check if this is a struct instance (after alias resolution)
    if (actual_struct_name) {
        // Struct instance declaration
        StructInfo* struct_info = codegen_find_struct(gen, actual_struct_name);
        if (!struct_info) {
            fprintf(stderr, "Codegen error: Undefined struct type '%s'\n", actual_struct_name);
            exit(1);
        }
        
        // Allocate space for entire struct
        gen->stack_offset += struct_info->total_size;
        codegen_add_struct_variable(gen, decl->name, gen->stack_offset, actual_struct_name);
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Declaration: %s %s (struct, size=%d bytes)", 
                 actual_struct_name, decl->name, struct_info->total_size);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, %d         ; Allocate space for struct %s",
                 struct_info->total_size, decl->name);
        codegen_emit(gen, buffer);
        
        // Initialize fields to zero
        for (int i = 0; i < struct_info->field_count; i++) {
            int field_offset = gen->stack_offset - struct_info->field_offsets[i];
            snprintf(buffer, sizeof(buffer),
                     "    mov qword [rbp-%d], 0   ; Initialize %s.%s",
                     field_offset, decl->name, struct_info->field_names[i]);
            codegen_emit(gen, buffer);
        }
    } else if (decl->is_array) {
        // Array declaration
        int element_size = 8; // All types are 8 bytes for simplicity
        
        if (decl->array_size == 0) {
            // Dynamic array - store pointer (8 bytes)
            gen->stack_offset += 8;
            codegen_add_dynamic_array(gen, decl->name, gen->stack_offset, decl->type);
            
            const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                                   decl->type == TYPE_DECIMAL ? "decimal" :
                                   decl->type == TYPE_BOOLEAN ? "boolean" : "text";
            
            snprintf(buffer, sizeof(buffer), 
                     "    ; Declaration: %s[] %s (dynamic array, pointer)", 
                     type_str, decl->name);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer),
                     "    sub rsp, 8         ; Allocate space for pointer %s",
                     decl->name);
            codegen_emit(gen, buffer);
            
            // Initialize pointer
            if (decl->init_value) {
                // init_value should be malloc(size) call
                codegen_generate_expression_value(gen, decl->init_value);
                snprintf(buffer, sizeof(buffer),
                         "    mov [rbp-%d], rax   ; Store array pointer to %s",
                         gen->stack_offset, decl->name);
                codegen_emit(gen, buffer);
            } else {
                // Initialize to NULL
                snprintf(buffer, sizeof(buffer),
                         "    mov qword [rbp-%d], 0   ; Initialize %s to NULL",
                         gen->stack_offset, decl->name);
                codegen_emit(gen, buffer);
            }
        } else {
            // Fixed-size array - allocate on stack
            int total_size = decl->array_size * element_size;
            
            gen->stack_offset += total_size;
            codegen_add_variable(gen, decl->name, gen->stack_offset, decl->type);
            
            const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                                   decl->type == TYPE_DECIMAL ? "decimal" :
                                   decl->type == TYPE_BOOLEAN ? "boolean" : "text";
            
            snprintf(buffer, sizeof(buffer), 
                     "    ; Declaration: %s[] %s (array, size=%d, total=%d bytes)", 
                     type_str, decl->name, decl->array_size, total_size);
            codegen_emit(gen, buffer);
            
            snprintf(buffer, sizeof(buffer),
                     "    sub rsp, %d         ; Allocate space for array %s",
                     total_size, decl->name);
            codegen_emit(gen, buffer);
            
            // Initialize array elements
            if (decl->array_init) {
                // Initialize from array literal
                for (int i = 0; i < decl->array_init_count; i++) {
                    codegen_generate_expression_value(gen, decl->array_init[i]);
                    int elem_offset = gen->stack_offset - (i * element_size);
                    snprintf(buffer, sizeof(buffer),
                             "    mov [rbp-%d], rax   ; Initialize %s[%d]",
                             elem_offset, decl->name, i);
                    codegen_emit(gen, buffer);
                }
                
                // Zero-initialize remaining elements
                for (int i = decl->array_init_count; i < decl->array_size; i++) {
                    int elem_offset = gen->stack_offset - (i * element_size);
                    snprintf(buffer, sizeof(buffer),
                             "    mov qword [rbp-%d], 0   ; Initialize %s[%d]",
                             elem_offset, decl->name, i);
                    codegen_emit(gen, buffer);
                }
            } else {
                // Zero-initialize all elements
                for (int i = 0; i < decl->array_size; i++) {
                    int elem_offset = gen->stack_offset - (i * element_size);
                    snprintf(buffer, sizeof(buffer),
                             "    mov qword [rbp-%d], 0   ; Initialize %s[%d]",
                             elem_offset, decl->name, i);
                    codegen_emit(gen, buffer);
                }
            }
        }
    } else if (decl->is_pointer) {
        // Phase 10: Pointer variable declaration
        gen->stack_offset += 8;  // Pointers are 8 bytes (64-bit addresses)
        codegen_add_pointer_variable(gen, decl->name, gen->stack_offset, decl->type);
        
        const char* type_str = decl->type == TYPE_NUMERIC ? "numeric" :
                               decl->type == TYPE_DECIMAL ? "decimal" :
                               decl->type == TYPE_BOOLEAN ? "boolean" : "text";
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Declaration: %s* %s (pointer)", 
                 type_str, decl->name);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, 8         ; Allocate space for pointer %s",
                 decl->name);
        codegen_emit(gen, buffer);
        
        // Initialize pointer
        if (decl->init_value) {
            codegen_generate_expression_value(gen, decl->init_value);
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rax   ; Initialize %s",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        } else {
            // Initialize to NULL (0)
            snprintf(buffer, sizeof(buffer),
                     "    mov qword [rbp-%d], 0   ; Initialize %s to NULL",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        }
    } else {
        // Regular variable declaration (may be through type alias)
        gen->stack_offset += 8;
        codegen_add_variable(gen, decl->name, gen->stack_offset, actual_type);
        
        snprintf(buffer, sizeof(buffer), 
                 "    ; Declaration: %s %s", 
                 actual_type == TYPE_NUMERIC ? "numeric" :
                 actual_type == TYPE_DECIMAL ? "decimal" : 
                 actual_type == TYPE_BOOLEAN ? "boolean" : "text",
                 decl->name);
        codegen_emit(gen, buffer);
        
        snprintf(buffer, sizeof(buffer),
                 "    sub rsp, 8         ; Allocate space for %s",
                 decl->name);
        codegen_emit(gen, buffer);
        
        // Initialize if value provided
        if (decl->init_value) {
            codegen_generate_expression_value(gen, decl->init_value);
            snprintf(buffer, sizeof(buffer),
                     "    mov [rbp-%d], rax   ; Initialize %s",
                     gen->stack_offset, decl->name);
            codegen_emit(gen, buffer);
        }
    }
}

void codegen_generate_assignment(Codegen* gen, char* var_name, char* field_name, Expression* array_index, Expression* dereference_target, Expression* value) {
    char buffer[256];
    
    if (dereference_target != NULL) {
        // Phase 10: Pointer dereference assignment: *ptr = value
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Pointer dereference assignment: *ptr = ...");
        
        // Evaluate value first
        codegen_generate_expression_value(gen, value);
        codegen_emit(gen, "    push rax            ; Save value");
        
        // Evaluate pointer expression (get address)
        codegen_generate_expression_value(gen, dereference_target);
        codegen_emit(gen, "    mov rbx, rax        ; Move address to rbx");
        codegen_emit(gen, "    pop rax             ; Restore value");
        codegen_emit(gen, "    mov [rbx], rax      ; Store value at address");
    } else if (array_index != NULL) {
        // Array assignment: arr[index] = value
        VarInfo* var_info = codegen_get_variable(gen, var_name);
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Array assignment: %s[...] = ...", var_name);
        codegen_emit(gen, buffer);
        
        // Evaluate index expression
        codegen_generate_expression_value(gen, array_index);
        codegen_emit(gen, "    push rax            ; Save index");
        
        // Evaluate value expression
        codegen_generate_expression_value(gen, value);
        codegen_emit(gen, "    mov rbx, rax        ; Save value in rbx");
        codegen_emit(gen, "    pop rax             ; Restore index");
        
        if (var_info->is_dynamic_array) {
            // Dynamic array: load pointer, then store at offset
            codegen_emit(gen, "    push rbx            ; Save value");
            snprintf(buffer, sizeof(buffer), "    mov rcx, [rbp-%d]   ; Load array pointer", var_info->stack_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    pop rbx             ; Restore value");
            codegen_emit(gen, "    mov rdx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rdx       ; index * 8");
            codegen_emit(gen, "    add rcx, rax        ; ptr + (index * 8)");
            codegen_emit(gen, "    mov [rcx], rbx      ; Store value to array[index]");
        } else {
            // Fixed-size array: stack-based addressing
            int base_offset = var_info->stack_offset;
            
            // Calculate element address: rbp - (base - index*8)
            codegen_emit(gen, "    mov rcx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rcx       ; index * 8");
            snprintf(buffer, sizeof(buffer), "    mov rcx, %d         ; Base offset", base_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    sub rcx, rax        ; base - (index * 8)");
            codegen_emit(gen, "    neg rcx             ; -(base - index*8)");
            codegen_emit(gen, "    mov [rbp + rcx], rbx ; Store value to array[index]");
        }
    } else if (field_name == NULL) {
        // Regular variable assignment
        int offset = codegen_find_variable(gen, var_name);
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Assignment: %s = ...", var_name);
        codegen_emit(gen, buffer);
        
        // Evaluate expression and store result
        codegen_generate_expression_value(gen, value);
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store to %s",
                 offset, var_name);
        codegen_emit(gen, buffer);
    } else {
        // Field assignment: object.field = value
        int offset = codegen_find_variable(gen, var_name);
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Field assignment: %s.%s = ...", var_name, field_name);
        codegen_emit(gen, buffer);
        
        // Find variable info to get struct name
        VarInfo* var_info = codegen_get_variable(gen, var_name);
        
        if (!var_info || !var_info->struct_name) {
            fprintf(stderr, "Codegen error: Variable '%s' is not a struct\n", var_name);
            exit(1);
        }
        
        // Find struct definition
        StructInfo* struct_info = codegen_find_struct(gen, var_info->struct_name);
        if (!struct_info) {
            fprintf(stderr, "Codegen error: Struct '%s' not found\n", var_info->struct_name);
            exit(1);
        }
        
        // Find field offset
        int field_offset = -1;
        for (int i = 0; i < struct_info->field_count; i++) {
            if (strcmp(struct_info->field_names[i], field_name) == 0) {
                field_offset = struct_info->field_offsets[i];
                break;
            }
        }
        
        if (field_offset < 0) {
            fprintf(stderr, "Codegen error: Field '%s' not found in struct '%s'\n", 
                    field_name, var_info->struct_name);
            exit(1);
        }
        
        // Evaluate expression and store to field
        codegen_generate_expression_value(gen, value);
        int actual_offset = offset - field_offset;
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store to %s.%s",
                 actual_offset, var_name, field_name);
        codegen_emit(gen, buffer);
    }
}

void codegen_generate_print(Codegen* gen, Expression* expr) {
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Print statement");
    
    // Evaluate expression to rax
    codegen_generate_expression_value(gen, expr);
    
    // Move result to rdi (first argument)
    codegen_emit(gen, "    mov rdi, rax");
    
    // Call appropriate print function based on expression type
    if (expr->type == EXPR_STRING) {
        codegen_emit(gen, "    call print_string");
    } else if (expr->type == EXPR_VARIABLE) {
        // Check variable type
        VarInfo* var = codegen_get_variable(gen, expr->var_name);
        if (var->type == TYPE_STRING) {
            codegen_emit(gen, "    call print_string");
        } else {
            codegen_emit(gen, "    call print_number");
        }
    } else {
        codegen_emit(gen, "    call print_number");
    }
}

// Helper: Determine if expression is string type
int is_string_expression(Codegen* gen, Expression* expr) {
    if (expr->type == EXPR_STRING) {
        return 1;
    }
    if (expr->type == EXPR_VARIABLE) {
        VarInfo* var = codegen_get_variable(gen, expr->var_name);
        return var->type == TYPE_STRING;
    }
    // For binary operations, check operands recursively
    if (expr->type == EXPR_BINARY_OP) {
        return is_string_expression(gen, expr->binary_op.left) || 
               is_string_expression(gen, expr->binary_op.right);
    }
    return 0;
}

void codegen_generate_expression_value(Codegen* gen, Expression* expr) {
    char buffer[256];
    
    if (expr->type == EXPR_NUMBER) {
        snprintf(buffer, sizeof(buffer), "    mov rax, %ld", expr->number_value);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_NULL) {
        // null literal - represented as 0
        codegen_emit(gen, "    mov rax, 0");
    } else if (expr->type == EXPR_STRING) {
        // String literal - add to data section and reference it
        int str_id = codegen_add_string(gen, expr->string_value);
        snprintf(buffer, sizeof(buffer), "    mov rax, str_%d", str_id);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_VARIABLE) {
        VarInfo* var = codegen_get_variable(gen, expr->var_name);
        
        if (var->stack_offset < 0 && var->stack_offset <= -1000) {
            // Captured variable from closure environment
            // offset = -(1000 + index), so index = -(offset + 1000)
            int capture_index = -(var->stack_offset + 1000);
            int env_offset = 8;  // Closure env pushed first (push rdi)
            
            snprintf(buffer, sizeof(buffer), 
                    "    ; Load captured variable %s from environment[%d]", 
                    expr->var_name, capture_index);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov r13, [rbp-%d]   ; Environment pointer", env_offset);
            codegen_emit(gen, buffer);
            snprintf(buffer, sizeof(buffer), "    mov rax, [r13+%d]", capture_index * 8);
            codegen_emit(gen, buffer);
        } else {
            // Regular stack variable
            snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", var->stack_offset);
            codegen_emit(gen, buffer);
        }
    } else if (expr->type == EXPR_BINARY_OP) {
        // Check if this is string concatenation
        if (expr->binary_op.op == BIN_OP_ADD && 
            (is_string_expression(gen, expr->binary_op.left) || 
             is_string_expression(gen, expr->binary_op.right))) {
            // String concatenation
            int left_is_string = is_string_expression(gen, expr->binary_op.left);
            int right_is_string = is_string_expression(gen, expr->binary_op.right);
            
            // Evaluate left side
            codegen_generate_expression_value(gen, expr->binary_op.left);
            // If left is numeric, convert to string
            if (!left_is_string) {
                codegen_emit(gen, "    mov rdi, rax");
                codegen_emit(gen, "    call int_to_string");
            }
            codegen_emit(gen, "    push rax");
            
            // Evaluate right side
            codegen_generate_expression_value(gen, expr->binary_op.right);
            // If right is numeric, convert to string
            if (!right_is_string) {
                codegen_emit(gen, "    mov rdi, rax");
                codegen_emit(gen, "    call int_to_string");
            }
            codegen_emit(gen, "    mov rsi, rax");  // second arg
            codegen_emit(gen, "    pop rdi");       // first arg
            
            // Call string_concat(rdi, rsi) -> returns new string in rax
            codegen_emit(gen, "    call string_concat");
            // Result pointer is in rax
        } else {
            // Numeric operations
            // Evaluate left side
            codegen_generate_expression_value(gen, expr->binary_op.left);
            codegen_emit(gen, "    push rax");
            
            // Evaluate right side
            codegen_generate_expression_value(gen, expr->binary_op.right);
            codegen_emit(gen, "    mov rbx, rax");
            codegen_emit(gen, "    pop rax");
            
            // Perform operation
            switch (expr->binary_op.op) {
                case BIN_OP_ADD:
                    codegen_emit(gen, "    add rax, rbx");
                    break;
                case BIN_OP_SUB:
                    codegen_emit(gen, "    sub rax, rbx");
                    break;
                case BIN_OP_MUL:
                    codegen_emit(gen, "    imul rax, rbx");
                    break;
                case BIN_OP_DIV:
                    codegen_emit(gen, "    cqo");  // Sign extend rax to rdx:rax
                    codegen_emit(gen, "    idiv rbx");
                    break;
            }
        }
    } else if (expr->type == EXPR_FUNC_CALL) {
        // Function call
        char buffer[256];
        
        // Check for built-in functions
        int is_builtin = 0;
        const char* builtin_name = NULL;
        
        if (strcmp(expr->func_call.func_name, "length") == 0) {
            is_builtin = 1;
            builtin_name = "string_length";
            
            // Check if argument is array (use mlp_array_length) or string
            // For now, assume string_length - array version will be mlp_array_length
            if (expr->func_call.arg_count > 0) {
                // TODO: Determine if arg is array or string at compile time
                // For simplicity, use string_length for now
            }
        } else if (strcmp(expr->func_call.func_name, "malloc") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_array_alloc";
        } else if (strcmp(expr->func_call.func_name, "free") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_array_free";
        } else if (strcmp(expr->func_call.func_name, "resize") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_array_resize";
        } else if (strcmp(expr->func_call.func_name, "read_file") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_read";
        } else if (strcmp(expr->func_call.func_name, "write_file") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_write";
        } else if (strcmp(expr->func_call.func_name, "file_exists") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_exists";
        } else if (strcmp(expr->func_call.func_name, "append_file") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_file_append";
        } else if (strcmp(expr->func_call.func_name, "substring") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_substring";
        } else if (strcmp(expr->func_call.func_name, "indexOf") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_indexOf";
        } else if (strcmp(expr->func_call.func_name, "charAt") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_charAt";
        } else if (strcmp(expr->func_call.func_name, "str_length") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_string_length";
        } else if (strcmp(expr->func_call.func_name, "get_argc") == 0 ||
                   strcmp(expr->func_call.func_name, "mlp_get_argc") == 0) {
            is_builtin = 1;
            builtin_name = "mlp_get_argc";
        }
        
        if (is_builtin) {
            // Built-in function - use System V AMD64 ABI calling convention
            // Arguments go in: rdi, rsi, rdx, rcx, r8, r9
            // FIXED: Push all args to stack first to avoid overwriting registers
            
            // Evaluate all arguments and push to stack
            for (int i = 0; i < expr->func_call.arg_count; i++) {
                codegen_generate_expression_value(gen, expr->func_call.args[i]);
                codegen_emit(gen, "    push rax");
            }
            
            // Now pop them into the correct registers (in reverse order)
            if (expr->func_call.arg_count > 2) {
                codegen_emit(gen, "    pop rdx");  // Third arg
            }
            if (expr->func_call.arg_count > 1) {
                codegen_emit(gen, "    pop rsi");  // Second arg
            }
            if (expr->func_call.arg_count > 0) {
                codegen_emit(gen, "    pop rdi");  // First arg
            }
            
            snprintf(buffer, sizeof(buffer), "    call %s", builtin_name);
            codegen_emit(gen, buffer);
            // Result is in rax
        } else {
            // User-defined function OR lambda call
            // Check if func_name is a variable (lambda)
            VarInfo* lambda_var = NULL;
            VarInfo* v = gen->variables;
            while (v) {
                if (strcmp(v->name, expr->func_call.func_name) == 0) {
                    lambda_var = v;
                    break;
                }
                v = v->next;
            }
            
            if (lambda_var) {
                // Lambda call: variable holds closure structure OR function pointer
                codegen_emit(gen, "    ; Lambda/closure call");
                
                // Load closure pointer/function pointer
                snprintf(buffer, sizeof(buffer), "    mov r15, [rbp-%d]   ; Load closure/lambda", 
                         lambda_var->stack_offset);
                codegen_emit(gen, buffer);
                
                // Check if r15 points to closure structure (has env pointer)
                // For simplicity, assume if lambda was created with closures, first qword is func ptr
                codegen_emit(gen, "    ; Assume closure structure: [func_ptr, env_ptr]");
                
                // Push arguments to stack (System V ABI: rdi, rsi, rdx, rcx)
                const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx"};
                
                // Save r15 (closure pointer)
                codegen_emit(gen, "    push r15");
                
                // Evaluate arguments and save to temp storage
                for (int i = 0; i < expr->func_call.arg_count; i++) {
                    codegen_generate_expression_value(gen, expr->func_call.args[i]);
                    codegen_emit(gen, "    push rax");
                }
                
                // Restore closure pointer
                snprintf(buffer, sizeof(buffer), "    mov r15, [rsp+%d]", expr->func_call.arg_count * 8);
                codegen_emit(gen, buffer);
                
                // Load function pointer and environment pointer
                codegen_emit(gen, "    mov r14, [r15]      ; Function pointer");
                codegen_emit(gen, "    mov rdi, [r15+8]    ; Environment pointer (first arg)");
                
                // Pop arguments into registers (skip rdi, it's for environment)
                for (int i = expr->func_call.arg_count - 1; i >= 0; i--) {
                    if (i < 3) {  // rsi, rdx, rcx available (rdi used for env)
                        snprintf(buffer, sizeof(buffer), "    pop %s", arg_regs[i + 1]);
                        codegen_emit(gen, buffer);
                    } else {
                        codegen_emit(gen, "    pop rax");  // Clean stack
                    }
                }
                
                // Remove saved closure pointer from stack
                codegen_emit(gen, "    add rsp, 8");
                
                // Call function
                codegen_emit(gen, "    call r14");
                
                // Result is in rax
            } else {
                // Regular user-defined or builtin function
                
                // Check if it's a builtin function
                int is_builtin = 0;
                char* func_label = NULL;
                
                if (strchr(expr->func_call.func_name, '.') != NULL) {
                    // Module qualified call: Math.add -> Math_add
                    func_label = malloc(strlen(expr->func_call.func_name) + 1);
                    strcpy(func_label, expr->func_call.func_name);
                    // Replace dot with underscore
                    for (char* p = func_label; *p; p++) {
                        if (*p == '.') *p = '_';
                    }
                } else if (is_builtin_function(expr->func_call.func_name)) {
                    // Builtin/runtime function - use as-is without func_ prefix
                    func_label = malloc(strlen(expr->func_call.func_name) + 1);
                    strcpy(func_label, expr->func_call.func_name);
                    is_builtin = 1;
                } else {
                    // Regular user function call: func_name -> func_func_name
                    func_label = malloc(strlen(expr->func_call.func_name) + 6);
                    sprintf(func_label, "func_%s", expr->func_call.func_name);
                }
                
                if (is_builtin) {
                    // Builtin functions use System V ABI (rdi, rsi, rdx, rcx, r8, r9)
                    // Generate arguments and store in registers
                    static const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                    
                    // Evaluate arguments right-to-left and store in temp locations
                    for (int i = expr->func_call.arg_count - 1; i >= 0; i--) {
                        codegen_generate_expression_value(gen, expr->func_call.args[i]);
                        codegen_emit(gen, "    push rax");
                    }
                    
                    // Pop arguments into registers (left-to-right)
                    for (int i = 0; i < expr->func_call.arg_count && i < 6; i++) {
                        snprintf(buffer, sizeof(buffer), "    pop %s", arg_regs[i]);
                        codegen_emit(gen, buffer);
                    }
                    
                    // Call function
                    snprintf(buffer, sizeof(buffer), "    call %s", func_label);
                    codegen_emit(gen, buffer);
                    
                    // Result is in rax
                } else {
                    // User-defined function - push arguments in reverse order (right to left)
                    for (int i = expr->func_call.arg_count - 1; i >= 0; i--) {
                        codegen_generate_expression_value(gen, expr->func_call.args[i]);
                        codegen_emit(gen, "    push rax");
                    }
                    
                    // Call function
                    snprintf(buffer, sizeof(buffer), "    call %s", func_label);
                    codegen_emit(gen, buffer);
                    
                    // Clean up stack (pop arguments)
                    if (expr->func_call.arg_count > 0) {
                        snprintf(buffer, sizeof(buffer), "    add rsp, %d", expr->func_call.arg_count * 8);
                        codegen_emit(gen, buffer);
                    }
                }
                
                free(func_label);
                
                // Result is in rax
            }
        }
    } else if (expr->type == EXPR_FIELD_ACCESS) {
        // Field access: object.field OR Enum.Member OR Module.function
        // First check if it's an enum value
        EnumValue* ev = gen->enums;
        int found_enum = 0;
        while (ev) {
            if (strcmp(ev->enum_name, expr->field_access.object_name) == 0 &&
                strcmp(ev->member_name, expr->field_access.field_name) == 0) {
                snprintf(buffer, sizeof(buffer), "    mov rax, %ld   ; %s.%s", 
                         ev->value, expr->field_access.object_name, expr->field_access.field_name);
                codegen_emit(gen, buffer);
                found_enum = 1;
                break;
            }
            ev = ev->next;
        }
        
        if (!found_enum) {
            // Check if it's a module function reference (e.g., Math.add)
            // Module function references are just identifiers, not actual values
            // They will be used in function calls like: Math.add(5, 3)
            // For now, we can't load a function as a value, so this is likely
            // used in a function call expression which will be handled separately
            
            // Try to find as struct field access
            VarInfo* var_info = codegen_get_variable(gen, expr->field_access.object_name);
            
            if (!var_info) {
                // Not a variable, could be a module name
                // For now, emit a comment and leave rax unchanged (module.func will be handled in EXPR_FUNC_CALL)
                snprintf(buffer, sizeof(buffer), "    ; Module reference: %s.%s (used in function call)", 
                         expr->field_access.object_name, expr->field_access.field_name);
                codegen_emit(gen, buffer);
                // This case shouldn't normally be reached as module.func is parsed as part of function call
            } else if (!var_info->struct_name) {
                fprintf(stderr, "Codegen error: '%s' is not a struct or enum\n", expr->field_access.object_name);
                exit(1);
            } else {
                // Struct field access
                StructInfo* struct_info = codegen_find_struct(gen, var_info->struct_name);
                if (!struct_info) {
                    fprintf(stderr, "Codegen error: Undefined struct type '%s'\n", var_info->struct_name);
                    exit(1);
                }
                
                // Find field in struct
                int field_index = -1;
                for (int i = 0; i < struct_info->field_count; i++) {
                    if (strcmp(struct_info->field_names[i], expr->field_access.field_name) == 0) {
                        field_index = i;
                        break;
                    }
                }
                
                if (field_index == -1) {
                    fprintf(stderr, "Codegen error: Struct '%s' has no field '%s'\n", 
                            var_info->struct_name, expr->field_access.field_name);
                    exit(1);
                }
                
                // Calculate actual offset: base_offset - field_offset
                int actual_offset = var_info->stack_offset - struct_info->field_offsets[field_index];
                snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]   ; Load %s.%s", 
                         actual_offset, expr->field_access.object_name, expr->field_access.field_name);
                codegen_emit(gen, buffer);
            }
        }
    } else if (expr->type == EXPR_COMPARISON) {
        // Comparison expression - evaluate to 1 (true) or 0 (false)
        codegen_generate_expression_value(gen, expr->comparison.left);
        codegen_emit(gen, "    push rax");
        codegen_generate_expression_value(gen, expr->comparison.right);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        codegen_emit(gen, "    cmp rax, rbx");
        
        // Use conditional move to set rax to 1 or 0
        int label = gen->label_counter++;
        switch (expr->comparison.op) {
            case CMP_EQUAL:
                codegen_emit(gen, "    mov rax, 0      ; Default false");
                codegen_emit(gen, "    mov rbx, 1      ; True value");
                codegen_emit(gen, "    cmove rax, rbx  ; If equal, set rax=1");
                break;
            case CMP_NOT_EQUAL:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovne rax, rbx");
                break;
            case CMP_LESS:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovl rax, rbx");
                break;
            case CMP_LESS_EQUAL:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovle rax, rbx");
                break;
            case CMP_GREATER:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovg rax, rbx");
                break;
            case CMP_GREATER_EQUAL:
                codegen_emit(gen, "    mov rax, 0");
                codegen_emit(gen, "    mov rbx, 1");
                codegen_emit(gen, "    cmovge rax, rbx");
                break;
        }
    } else if (expr->type == EXPR_TERNARY) {
        // Ternary operator: condition ? true_expr : false_expr
        int false_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Ternary operator");
        
        // Evaluate condition
        if (expr->ternary.condition->type == EXPR_COMPARISON) {
            // Use comparison generation
            codegen_generate_comparison(gen, expr->ternary.condition, false_label);
            
            // True branch
            codegen_generate_expression_value(gen, expr->ternary.true_expr);
            snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
            codegen_emit(gen, buffer);
            
            // False branch
            snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
            codegen_emit(gen, buffer);
            codegen_generate_expression_value(gen, expr->ternary.false_expr);
            
            snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
            codegen_emit(gen, buffer);
        } else {
            // Non-comparison condition (treat 0 as false, non-zero as true)
            codegen_generate_expression_value(gen, expr->ternary.condition);
            codegen_emit(gen, "    test rax, rax");
            snprintf(buffer, sizeof(buffer), "    jz .L%d", false_label);
            codegen_emit(gen, buffer);
            
            // True branch
            codegen_generate_expression_value(gen, expr->ternary.true_expr);
            snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
            codegen_emit(gen, buffer);
            
            // False branch
            snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
            codegen_emit(gen, buffer);
            codegen_generate_expression_value(gen, expr->ternary.false_expr);
            
            snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
            codegen_emit(gen, buffer);
        }
    } else if (expr->type == EXPR_ARRAY_INDEX) {
        // Array indexing: arr[index]
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Array indexing");
        
        // Get array variable info
        VarInfo* var_info = codegen_get_variable(gen, expr->array_index.array_name);
        
        // Evaluate index expression
        codegen_generate_expression_value(gen, expr->array_index.index);
        
        if (var_info->is_dynamic_array) {
            // Dynamic array: load pointer, then index into it
            codegen_emit(gen, "    push rax            ; Save index");
            snprintf(buffer, sizeof(buffer), "    mov rbx, [rbp-%d]   ; Load array pointer", var_info->stack_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    pop rax             ; Restore index");
            codegen_emit(gen, "    mov rcx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rcx       ; index * 8");
            codegen_emit(gen, "    add rbx, rax        ; ptr + (index * 8)");
            codegen_emit(gen, "    mov rax, [rbx]      ; Load array[index]");
        } else {
            // Fixed-size array: stack-based addressing
            int base_offset = var_info->stack_offset;
            
            // Calculate element address: rbp - (base - index*8)
            codegen_emit(gen, "    mov rbx, 8          ; Element size");
            codegen_emit(gen, "    imul rax, rbx       ; index * 8");
            snprintf(buffer, sizeof(buffer), "    mov rbx, %d         ; Base offset", base_offset);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    sub rbx, rax        ; base - (index * 8)");
            codegen_emit(gen, "    neg rbx             ; -(base - index*8)");
            codegen_emit(gen, "    mov rax, [rbp + rbx] ; Load array[index]");
        }
    } else if (expr->type == EXPR_ADDRESS_OF) {
        // Phase 10: Address-of operator: &variable
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Address-of operator");
        
        if (expr->unary_operand->type == EXPR_VARIABLE) {
            // Get address of variable
            int offset = codegen_find_variable(gen, expr->unary_operand->var_name);
            snprintf(buffer, sizeof(buffer), "    lea rax, [rbp-%d]   ; &%s", 
                     offset, expr->unary_operand->var_name);
            codegen_emit(gen, buffer);
        } else if (expr->unary_operand->type == EXPR_ARRAY_INDEX) {
            // Get address of array element: &arr[index]
            VarInfo* var_info = codegen_get_variable(gen, expr->unary_operand->array_index.array_name);
            
            // Evaluate index expression
            codegen_generate_expression_value(gen, expr->unary_operand->array_index.index);
            
            if (var_info->is_dynamic_array) {
                // Dynamic array: load pointer, then calculate element address
                codegen_emit(gen, "    push rax            ; Save index");
                snprintf(buffer, sizeof(buffer), "    mov rbx, [rbp-%d]   ; Load array pointer", var_info->stack_offset);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    pop rax             ; Restore index");
                codegen_emit(gen, "    mov rcx, 8          ; Element size");
                codegen_emit(gen, "    imul rax, rcx       ; index * 8");
                codegen_emit(gen, "    add rax, rbx        ; ptr + (index * 8) = element address");
            } else {
                // Fixed-size array: calculate stack address
                int base_offset = var_info->stack_offset;
                codegen_emit(gen, "    mov rbx, 8          ; Element size");
                codegen_emit(gen, "    imul rax, rbx       ; index * 8");
                snprintf(buffer, sizeof(buffer), "    mov rbx, %d         ; Base offset", base_offset);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    sub rbx, rax        ; base - (index * 8)");
                codegen_emit(gen, "    neg rbx             ; -(base - index*8)");
                codegen_emit(gen, "    lea rax, [rbp + rbx] ; Calculate element address");
            }
        } else {
            fprintf(stderr, "Codegen error: Address-of operator can only be applied to variables or array elements\n");
            exit(1);
        }
    } else if (expr->type == EXPR_DEREFERENCE) {
        // Phase 10: Dereference operator: *pointer
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Dereference operator");
        
        // Evaluate pointer expression (should result in an address)
        codegen_generate_expression_value(gen, expr->unary_operand);
        
        // Load value at that address
        codegen_emit(gen, "    mov rax, [rax]      ; Dereference pointer");
    } else if (expr->type == EXPR_LOGICAL_AND) {
        // Logical AND with short-circuit evaluation
        int false_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Logical AND");
        
        // Evaluate left side
        codegen_generate_expression_value(gen, expr->logical_binary.left);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d          ; Short-circuit if false", false_label);
        codegen_emit(gen, buffer);
        
        // Evaluate right side (only if left was true)
        codegen_generate_expression_value(gen, expr->logical_binary.right);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d          ; Check right side", false_label);
        codegen_emit(gen, buffer);
        
        // Both true
        codegen_emit(gen, "    mov rax, 1          ; Result: true");
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
        
        // At least one false
        snprintf(buffer, sizeof(buffer), ".L%d:", false_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    mov rax, 0          ; Result: false");
        
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_LOGICAL_OR) {
        // Logical OR with short-circuit evaluation
        int true_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Logical OR");
        
        // Evaluate left side
        codegen_generate_expression_value(gen, expr->logical_binary.left);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d          ; Short-circuit if true", true_label);
        codegen_emit(gen, buffer);
        
        // Evaluate right side (only if left was false)
        codegen_generate_expression_value(gen, expr->logical_binary.right);
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d          ; Check right side", true_label);
        codegen_emit(gen, buffer);
        
        // Both false
        codegen_emit(gen, "    mov rax, 0          ; Result: false");
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
        
        // At least one true
        snprintf(buffer, sizeof(buffer), ".L%d:", true_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    mov rax, 1          ; Result: true");
        
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    } else if (expr->type == EXPR_LOGICAL_NOT) {
        // Logical NOT
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Logical NOT");
        
        // Evaluate operand
        codegen_generate_expression_value(gen, expr->logical_not_operand);
        
        // Flip boolean: 0 -> 1, non-zero -> 0
        codegen_emit(gen, "    test rax, rax");
        codegen_emit(gen, "    setz al             ; Set AL to 1 if zero, 0 otherwise");
        codegen_emit(gen, "    movzx rax, al       ; Zero-extend AL to RAX");
    } else if (expr->type == EXPR_LAMBDA) {
        // Lambda functions: inline generation with JMP skip
        char buffer[256];
        char lambda_name[64];
        char skip_label_name[64];
        int skip_label = gen->label_counter++;
        
        snprintf(lambda_name, sizeof(lambda_name), "__lambda_%d", gen->label_counter++);
        snprintf(skip_label_name, sizeof(skip_label_name), "__lambda_skip_%d", skip_label);
        
        // Jump over lambda definition
        snprintf(buffer, sizeof(buffer), "    jmp %s     ; Skip lambda definition", skip_label_name);
        codegen_emit(gen, buffer);
        
        // Store current context
        int old_in_function = gen->in_function;
        VarInfo* old_variables = gen->variables;
        int old_stack_offset = gen->stack_offset;
        
        // Lambda function definition
        gen->in_function = 1;
        gen->variables = NULL;
        gen->stack_offset = 0;
        
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "; Lambda function: %s (captures %d variables)", 
                lambda_name, expr->lambda.captured_count);
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "%s:", lambda_name);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    push rbp");
        codegen_emit(gen, "    mov rbp, rsp");
        
        // If lambda has closure, first parameter (rdi) is closure environment pointer
        int param_offset = 0;
        const char* param_regs[] = {"rdi", "rsi", "rdx", "rcx"};
        
        if (expr->lambda.captured_count > 0) {
            // Store closure environment pointer
            codegen_emit(gen, "    ; Store closure environment pointer");
            gen->stack_offset += 8;
            codegen_emit(gen, "    push rdi    ; closure environment");
            int closure_offset = gen->stack_offset;
            
            // Add captured variables to symbol table
            for (int i = 0; i < expr->lambda.captured_count; i++) {
                VarInfo* var = malloc(sizeof(VarInfo));
                var->name = malloc(strlen(expr->lambda.captured_vars[i]) + 1);
                strcpy(var->name, expr->lambda.captured_vars[i]);
                var->stack_offset = -(1000 + i);  // Special marker: negative 1000+ index
                var->type = TYPE_NUMERIC;
                var->struct_name = NULL;
                var->is_dynamic_array = 0;
                var->is_pointer = 0;
                var->next = gen->variables;
                gen->variables = var;
                
                snprintf(buffer, sizeof(buffer), 
                        "    ; Captured variable: %s at environment[%d]", 
                        expr->lambda.captured_vars[i], i);
                codegen_emit(gen, buffer);
            }
            
            param_offset = 1;  // First actual parameter is in rsi, not rdi
        }
        
        // Allocate stack for parameters
        int total_stack = expr->lambda.param_count * 8;
        if (total_stack > 0) {
            snprintf(buffer, sizeof(buffer), "    sub rsp, %d", total_stack);
            codegen_emit(gen, buffer);
        }
        
        // Store parameters from registers
        for (int i = 0; i < expr->lambda.param_count && (i + param_offset) < 4; i++) {
            gen->stack_offset += 8;
            VarInfo* var = malloc(sizeof(VarInfo));
            var->name = malloc(strlen(expr->lambda.param_names[i]) + 1);
            strcpy(var->name, expr->lambda.param_names[i]);
            var->stack_offset = gen->stack_offset;
            var->type = TYPE_NUMERIC;
            var->struct_name = NULL;
            var->is_dynamic_array = 0;
            var->is_pointer = 0;
            var->next = gen->variables;
            gen->variables = var;
            
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], %s", 
                    gen->stack_offset, param_regs[i + param_offset]);
            codegen_emit(gen, buffer);
        }
        
        // Generate lambda body
        codegen_emit(gen, "    ; Lambda body");
        codegen_generate_expression_value(gen, expr->lambda.body);
        
        // Return
        codegen_emit(gen, "    leave");
        codegen_emit(gen, "    ret");
        
        // Skip label - closure creation code runs HERE in outer function scope
        snprintf(buffer, sizeof(buffer), "%s:", skip_label_name);
        codegen_emit(gen, buffer);
        
        // Create closure if needed (BEFORE restoring context, while captured vars accessible)
        if (expr->lambda.captured_count > 0) {
            codegen_emit(gen, "");
            codegen_emit(gen, "    ; Allocate closure environment");
            int env_size = expr->lambda.captured_count * 8;
            snprintf(buffer, sizeof(buffer), "    mov rdi, %d", env_size);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    call malloc");
            codegen_emit(gen, "    mov r12, rax    ; Save environment pointer");
            
            // Copy captured variable values to environment (use OLD context)
            for (int i = 0; i < expr->lambda.captured_count; i++) {
                VarInfo* var = old_variables;
                while (var) {
                    if (strcmp(var->name, expr->lambda.captured_vars[i]) == 0) {
                        snprintf(buffer, sizeof(buffer), 
                                "    ; Store %s in environment[%d]", 
                                expr->lambda.captured_vars[i], i);
                        codegen_emit(gen, buffer);
                        
                        snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", var->stack_offset);
                        codegen_emit(gen, buffer);
                        snprintf(buffer, sizeof(buffer), "    mov [r12+%d], rax", i * 8);
                        codegen_emit(gen, buffer);
                        break;
                    }
                    var = var->next;
                }
            }
            
            // Create closure: allocate structure with function pointer + environment
            codegen_emit(gen, "    ; Create closure structure (func_ptr + env_ptr)");
            codegen_emit(gen, "    mov rdi, 16     ; 2 pointers");
            codegen_emit(gen, "    call malloc");
            snprintf(buffer, sizeof(buffer), "    lea r13, [rel %s]", lambda_name);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    mov [rax], r13      ; Store function pointer");
            codegen_emit(gen, "    mov [rax+8], r12    ; Store environment pointer");
            codegen_emit(gen, "    ; rax now holds closure pointer");
        } else {
            // No closures: just load lambda address into rax
            snprintf(buffer, sizeof(buffer), "    lea rax, [rel %s]", lambda_name);
            codegen_emit(gen, buffer);
        }
        
        // Restore context AFTER closure creation
        gen->in_function = old_in_function;
        VarInfo* temp_vars = gen->variables;
        gen->variables = old_variables;
        gen->stack_offset = old_stack_offset;
        
        // Free lambda variables
        while (temp_vars) {
            VarInfo* next = temp_vars->next;
            free(temp_vars->name);
            free(temp_vars);
            temp_vars = next;
        }
    } else if (expr->type == EXPR_AWAIT) {
        // Await expression: for now just evaluate the awaited expression
        // Full async runtime would suspend here and resume later
        codegen_emit(gen, "    ; Await expression (compiled as sync call for now)");
        codegen_generate_expression_value(gen, expr->await_expr.awaited_expr);
        // Result is already in rax
    }
}

void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label) {
    char buffer[256];
    
    // Check if condition is a comparison expression
    if (condition->type != EXPR_COMPARISON) {
        // Not a comparison - evaluate as boolean (0 = false, non-zero = true)
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Evaluate boolean condition");
        codegen_generate_expression_value(gen, condition);
        codegen_emit(gen, "    cmp rax, 0");
        snprintf(buffer, sizeof(buffer), "    je .L%d          ; Jump if false", false_label);
        codegen_emit(gen, buffer);
        return;
    }
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Evaluate comparison");
    
    // Check if this is a string comparison
    int is_string_comp = is_string_expression(gen, condition->comparison.left) || 
                         is_string_expression(gen, condition->comparison.right);
    
    if (is_string_comp) {
        // String comparison - use runtime functions
        
        // Load left operand (string pointer) into rdi
        codegen_generate_expression_value(gen, condition->comparison.left);
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    push rdi");
        
        // Load right operand (string pointer) into rsi
        codegen_generate_expression_value(gen, condition->comparison.right);
        codegen_emit(gen, "    mov rsi, rax");
        codegen_emit(gen, "    pop rdi");
        
        // Call appropriate string comparison function
        if (condition->comparison.op == CMP_EQUAL) {
            codegen_emit(gen, "    call string_equal");
        } else if (condition->comparison.op == CMP_NOT_EQUAL) {
            codegen_emit(gen, "    call string_not_equal");
        } else {
            fprintf(stderr, "Error: Unsupported string comparison operator\n");
            exit(1);
        }
        
        // Result is in rax (1 = true, 0 = false)
        // Jump to false_label if result is 0
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jz .L%d", false_label);
        codegen_emit(gen, buffer);
    } else {
        // Numeric comparison - use cmp instruction
        
        // Load left side into rax
        codegen_generate_expression_value(gen, condition->comparison.left);
        codegen_emit(gen, "    push rax");
        
        // Load right side into rbx
        codegen_generate_expression_value(gen, condition->comparison.right);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        
        // Compare
        codegen_emit(gen, "    cmp rax, rbx");
        
        // Jump to false label based on comparison
        const char* jump_instr;
        switch (condition->comparison.op) {
            case CMP_EQUAL:         jump_instr = "jne"; break;
            case CMP_NOT_EQUAL:     jump_instr = "je"; break;
            case CMP_LESS:          jump_instr = "jge"; break;
            case CMP_LESS_EQUAL:    jump_instr = "jg"; break;
            case CMP_GREATER:       jump_instr = "jle"; break;
            case CMP_GREATER_EQUAL: jump_instr = "jl"; break;
            default: jump_instr = "jmp"; break;
        }
        
        snprintf(buffer, sizeof(buffer), "    %s .L%d", jump_instr, false_label);
        codegen_emit(gen, buffer);
    }
}

void codegen_generate_if(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int else_label = gen->label_counter++;
    int end_label = gen->label_counter++;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; If statement");
    
    // Generate condition check
    if (stmt->if_stmt.condition->type == EXPR_COMPARISON) {
        codegen_generate_comparison(gen, stmt->if_stmt.condition, else_label);
    }
    
    // Then body
    codegen_emit(gen, "    ; Then body");
    for (int i = 0; i < stmt->if_stmt.then_count; i++) {
        codegen_generate_statement(gen, stmt->if_stmt.then_body[i]);
    }
    
    if (stmt->if_stmt.else_count > 0) {
        snprintf(buffer, sizeof(buffer), "    jmp .L%d", end_label);
        codegen_emit(gen, buffer);
    }
    
    // Else label
    snprintf(buffer, sizeof(buffer), ".L%d:", else_label);
    codegen_emit(gen, buffer);
    
    // Else body (if exists)
    if (stmt->if_stmt.else_count > 0) {
        codegen_emit(gen, "    ; Else body");
        for (int i = 0; i < stmt->if_stmt.else_count; i++) {
            codegen_generate_statement(gen, stmt->if_stmt.else_body[i]);
        }
        
        snprintf(buffer, sizeof(buffer), ".L%d:", end_label);
        codegen_emit(gen, buffer);
    }
}

void codegen_generate_for(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int loop_start = gen->label_counter++;
    int loop_continue = gen->label_counter++;
    int loop_end = gen->label_counter++;
    
    // Save previous loop context
    int prev_start = gen->loop_start_label;
    int prev_continue = gen->loop_continue_label;
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    // Set current loop context
    gen->loop_start_label = loop_start;
    gen->loop_continue_label = loop_continue;
    gen->loop_end_label = loop_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; For loop");
    
    // Allocate loop variable on stack
    gen->stack_offset += 8;
    codegen_add_variable(gen, stmt->for_stmt.var_name, gen->stack_offset, TYPE_NUMERIC);
    snprintf(buffer, sizeof(buffer), "    sub rsp, 8         ; Allocate %s", 
             stmt->for_stmt.var_name);
    codegen_emit(gen, buffer);
    
    // Initialize loop variable with start value
    codegen_generate_expression_value(gen, stmt->for_stmt.start);
    int var_offset = codegen_find_variable(gen, stmt->for_stmt.var_name);
    snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Initialize %s",
             var_offset, stmt->for_stmt.var_name);
    codegen_emit(gen, buffer);
    
    // Loop start label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; loop_start", loop_start);
    codegen_emit(gen, buffer);
    
    // Check condition: loop_var <= end_value
    snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]   ; Load %s",
             var_offset, stmt->for_stmt.var_name);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    push rax");
    
    codegen_generate_expression_value(gen, stmt->for_stmt.end);
    codegen_emit(gen, "    mov rbx, rax");
    codegen_emit(gen, "    pop rax");
    codegen_emit(gen, "    cmp rax, rbx");
    
    snprintf(buffer, sizeof(buffer), "    jg .L%d           ; Exit if i > end", loop_end);
    codegen_emit(gen, buffer);
    
    // Loop body
    codegen_emit(gen, "    ; Loop body");
    for (int i = 0; i < stmt->for_stmt.body_count; i++) {
        codegen_generate_statement(gen, stmt->for_stmt.body[i]);
    }
    
    // Loop continue label (for continue statement)
    snprintf(buffer, sizeof(buffer), ".L%d:  ; loop_continue", loop_continue);
    codegen_emit(gen, buffer);
    
    // Increment loop variable
    codegen_emit(gen, "    ; Increment loop variable");
    snprintf(buffer, sizeof(buffer), "    mov rax, [rbp-%d]", var_offset);
    codegen_emit(gen, buffer);
    
    if (stmt->for_stmt.step) {
        // Custom step value
        codegen_emit(gen, "    push rax");
        codegen_generate_expression_value(gen, stmt->for_stmt.step);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax");
        codegen_emit(gen, "    add rax, rbx");
    } else {
        // Default step = 1
        codegen_emit(gen, "    add rax, 1");
    }
    
    snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", var_offset);
    codegen_emit(gen, buffer);
    
    // Jump back to loop start
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; loop_end", loop_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_start_label = prev_start;
    gen->loop_continue_label = prev_continue;
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
}

void codegen_generate_while(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int loop_start = gen->label_counter++;
    int loop_end = gen->label_counter++;
    
    // Save previous loop context
    int prev_start = gen->loop_start_label;
    int prev_continue = gen->loop_continue_label;
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    // Set current loop context (while: continue = start)
    gen->loop_start_label = loop_start;
    gen->loop_continue_label = loop_start;
    gen->loop_end_label = loop_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; While loop");
    
    // Loop start label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; while_start", loop_start);
    codegen_emit(gen, buffer);
    
    // Evaluate condition and jump to end if false
    codegen_generate_comparison(gen, stmt->while_stmt.condition, loop_end);
    
    // Loop body
    codegen_emit(gen, "    ; While body");
    for (int i = 0; i < stmt->while_stmt.body_count; i++) {
        codegen_generate_statement(gen, stmt->while_stmt.body[i]);
    }
    
    // Jump back to loop start
    snprintf(buffer, sizeof(buffer), "    jmp .L%d", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; while_end", loop_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_start_label = prev_start;
    gen->loop_continue_label = prev_continue;
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
}

void codegen_generate_do_while(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int loop_start = gen->label_counter++;
    int loop_continue = gen->label_counter++;
    int loop_end = gen->label_counter++;
    
    // Save previous loop context
    int prev_start = gen->loop_start_label;
    int prev_continue = gen->loop_continue_label;
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    // Set current loop context
    gen->loop_start_label = loop_start;
    gen->loop_continue_label = loop_continue;
    gen->loop_end_label = loop_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Do-while loop");
    
    // Loop start label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; do_while_start", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop body
    codegen_emit(gen, "    ; Do-while body");
    for (int i = 0; i < stmt->do_while_stmt.body_count; i++) {
        codegen_generate_statement(gen, stmt->do_while_stmt.body[i]);
    }
    
    // Continue label (for continue statement)
    snprintf(buffer, sizeof(buffer), ".L%d:  ; do_while_continue", loop_continue);
    codegen_emit(gen, buffer);
    
    // DEBUG: Check if condition exists
    if (stmt->do_while_stmt.condition == NULL) {
        fprintf(stderr, "ERROR: do-while condition is NULL!\n");
        exit(1);
    }
    
    // Evaluate condition - if FALSE jump to end, if TRUE jump back to start
    codegen_emit(gen, "    ; Check condition");
    
    // codegen_generate_comparison jumps to given label if condition is FALSE
    // So we pass loop_end - if condition FALSE, exit loop
    codegen_generate_comparison(gen, stmt->do_while_stmt.condition, loop_end);
    
    // If we reach here, condition was TRUE, so loop again
    snprintf(buffer, sizeof(buffer), "    jmp .L%d         ; Condition true, loop", loop_start);
    codegen_emit(gen, buffer);
    
    // Loop end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; do_while_end", loop_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_start_label = prev_start;
    gen->loop_continue_label = prev_continue;
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
}

void codegen_generate_switch(Codegen* gen, Statement* stmt) {
    char buffer[256];
    int switch_end = gen->label_counter++;
    
    // Save previous loop context (switch acts like a loop for exit)
    int prev_end = gen->loop_end_label;
    int prev_in_loop = gen->in_loop;
    
    // Set switch context for 'exit' to work
    gen->loop_end_label = switch_end;
    gen->in_loop = 1;
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Switch statement");
    
    // Evaluate switch expression and store in rax
    codegen_generate_expression_value(gen, stmt->switch_stmt.value);
    codegen_emit(gen, "    push rax         ; Save switch value");
    
    // Generate labels for each case
    int* case_labels = malloc(sizeof(int) * stmt->switch_stmt.case_count);
    for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
        case_labels[i] = gen->label_counter++;
    }
    int default_label = (stmt->switch_stmt.default_body != NULL) ? gen->label_counter++ : switch_end;
    
    // Generate comparison jumps for each case
    for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
        codegen_emit(gen, "    ; Case comparison");
        codegen_emit(gen, "    pop rax          ; Restore switch value");
        codegen_emit(gen, "    push rax         ; Save again for next comparison");
        
        // Evaluate case value
        codegen_generate_expression_value(gen, stmt->switch_stmt.cases[i]->value);
        codegen_emit(gen, "    mov rbx, rax");
        codegen_emit(gen, "    pop rax          ; Get switch value");
        codegen_emit(gen, "    push rax         ; Save again");
        
        // Compare and jump if equal
        codegen_emit(gen, "    cmp rax, rbx");
        snprintf(buffer, sizeof(buffer), "    je .L%d          ; Jump to case %d", case_labels[i], i);
        codegen_emit(gen, buffer);
    }
    
    // No match - jump to default or end
    codegen_emit(gen, "    pop rax          ; Clean up switch value");
    snprintf(buffer, sizeof(buffer), "    jmp .L%d         ; Jump to default/end", default_label);
    codegen_emit(gen, buffer);
    
    // Generate case bodies
    for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
        snprintf(buffer, sizeof(buffer), ".L%d:  ; case %d", case_labels[i], i);
        codegen_emit(gen, buffer);
        
        for (int j = 0; j < stmt->switch_stmt.cases[i]->body_count; j++) {
            codegen_generate_statement(gen, stmt->switch_stmt.cases[i]->body[j]);
        }
        
        // Fall through to end (no automatic break - user must use exit)
        snprintf(buffer, sizeof(buffer), "    jmp .L%d         ; Fall through to end", switch_end);
        codegen_emit(gen, buffer);
    }
    
    // Generate default body if present
    if (stmt->switch_stmt.default_body != NULL) {
        snprintf(buffer, sizeof(buffer), ".L%d:  ; default", default_label);
        codegen_emit(gen, buffer);
        
        for (int i = 0; i < stmt->switch_stmt.default_count; i++) {
            codegen_generate_statement(gen, stmt->switch_stmt.default_body[i]);
        }
    }
    
    // Switch end label
    snprintf(buffer, sizeof(buffer), ".L%d:  ; switch_end", switch_end);
    codegen_emit(gen, buffer);
    
    // Restore previous loop context
    gen->loop_end_label = prev_end;
    gen->in_loop = prev_in_loop;
    
    free(case_labels);
}

void codegen_generate_func_def(Codegen* gen, Statement* stmt) {
    char buffer[256];
    
    // Generate function name with module prefix if inside module
    char func_name[256];
    if (gen->current_module) {
        snprintf(func_name, sizeof(func_name), "%s_%s", gen->current_module, stmt->func_def.func_name);
    } else {
        snprintf(func_name, sizeof(func_name), "func_%s", stmt->func_def.func_name);
    }
    
    // Register function
    codegen_add_function(gen, stmt->func_def.func_name, 
                        stmt->func_def.param_count, 
                        stmt->func_def.param_types);
    
    codegen_emit(gen, "");
    
    // Export visibility: Add global directive if exported
    if (stmt->func_def.is_exported) {
        snprintf(buffer, sizeof(buffer), "global %s", func_name);
        codegen_emit(gen, buffer);
    }
    
    // Note: Async functions are compiled as regular functions for now
    // Full async runtime with state machines would require complex transformation
    if (stmt->func_def.is_async) {
        snprintf(buffer, sizeof(buffer), "; Async function: %s (compiled as sync for now)", stmt->func_def.func_name);
        codegen_emit(gen, buffer);
    }
    
    snprintf(buffer, sizeof(buffer), "%s:", func_name);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    
    // Save previous state
    int prev_stack_offset = gen->stack_offset;
    VarInfo* prev_variables = gen->variables;
    int prev_in_function = gen->in_function;
    
    gen->stack_offset = 0;
    gen->variables = NULL;
    gen->in_function = 1;
    
    // Parameters are passed on stack (above rbp)
    // rbp+16 = first param, rbp+24 = second param, etc.
    for (int i = 0; i < stmt->func_def.param_count; i++) {
        int param_offset = 16 + (i * 8);
        
        // Load parameter from stack above rbp
        snprintf(buffer, sizeof(buffer), "    mov rax, [rbp+%d]   ; Load param %s",
                 param_offset, stmt->func_def.param_names[i]);
        codegen_emit(gen, buffer);
        
        // Allocate local space for parameter
        gen->stack_offset += 8;
        codegen_add_variable(gen, stmt->func_def.param_names[i], gen->stack_offset, stmt->func_def.param_types[i]);
        
        snprintf(buffer, sizeof(buffer), "    sub rsp, 8");
        codegen_emit(gen, buffer);
        snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; Store param %s locally",
                 gen->stack_offset, stmt->func_def.param_names[i]);
        codegen_emit(gen, buffer);
    }
    
    // Generate function body
    int has_return = 0;
    for (int i = 0; i < stmt->func_def.body_count; i++) {
        if (stmt->func_def.body[i]->type == STMT_RETURN) {
            has_return = 1;
        }
        codegen_generate_statement(gen, stmt->func_def.body[i]);
    }
    
    // Default return (if no explicit return)
    if (!has_return) {
        codegen_emit(gen, "    mov rsp, rbp");
        codegen_emit(gen, "    pop rbp");
        codegen_emit(gen, "    ret");
    }    
    // Restore previous state
    gen->stack_offset = prev_stack_offset;
    gen->variables = prev_variables;
    gen->in_function = prev_in_function;
}

void codegen_generate_return(Codegen* gen, Statement* stmt) {
    if (!gen->in_function) {
        fprintf(stderr, "Codegen error: 'return' outside function\n");
        exit(1);
    }
    
    if (stmt->return_stmt.value_count > 0) {
        // Generate code for multiple return values
        // For now, support single return value (multiple returns will use stack)
        if (stmt->return_stmt.value_count == 1) {
            // Single return value - put in rax
            codegen_generate_expression_value(gen, stmt->return_stmt.values[0]);
        } else {
            // Multiple return values - store on stack
            // Return values will be accessed by caller via stack offsets
            // For simplicity: rax = first value, rdx = second value, rest on stack
            char buffer[256];
            
            // First return value in rax
            codegen_generate_expression_value(gen, stmt->return_stmt.values[0]);
            
            if (stmt->return_stmt.value_count >= 2) {
                // Second return value in rdx
                codegen_emit(gen, "    push rax           ; Save first return value");
                codegen_generate_expression_value(gen, stmt->return_stmt.values[1]);
                codegen_emit(gen, "    mov rdx, rax       ; Second return in rdx");
                codegen_emit(gen, "    pop rax            ; Restore first return in rax");
            }
            
            // Additional values (if any) would go in rcx, r8, r9, etc.
            // For now, we'll support up to 2 return values
            if (stmt->return_stmt.value_count > 2) {
                fprintf(stderr, "Codegen warning: More than 2 return values not fully supported yet\n");
            }
        }
    } else {
        // Void return
        codegen_emit(gen, "    xor rax, rax");
    }
    
    // Function epilogue
    codegen_emit(gen, "    mov rsp, rbp");
    codegen_emit(gen, "    pop rbp");
    codegen_emit(gen, "    ret");
}

void codegen_generate_statement(Codegen* gen, Statement* stmt) {
    if (stmt->type == STMT_STRUCT_DEF) {
        // Register struct type (no assembly code generated)
        VarType* field_types = malloc(sizeof(VarType) * stmt->struct_def.field_count);
        char** field_names = malloc(sizeof(char*) * stmt->struct_def.field_count);
        
        for (int i = 0; i < stmt->struct_def.field_count; i++) {
            field_types[i] = stmt->struct_def.fields[i].type;
            field_names[i] = stmt->struct_def.fields[i].name;
        }
        
        codegen_add_struct(gen, stmt->struct_def.struct_name, 
                          stmt->struct_def.field_count, field_types, field_names);
        
        free(field_types);
        free(field_names);
    } else if (stmt->type == STMT_ENUM_DEF) {
        // Register enum values (no assembly code generated)
        for (int i = 0; i < stmt->enum_def.member_count; i++) {
            codegen_add_enum_value(gen, stmt->enum_def.enum_name,
                                  stmt->enum_def.members[i].name,
                                  stmt->enum_def.members[i].value);
        }
    } else if (stmt->type == STMT_TYPE_ALIAS) {
        // Register type alias (no assembly code generated)
        codegen_add_type_alias(gen, stmt->type_alias.alias_name,
                              stmt->type_alias.base_type,
                              stmt->type_alias.struct_name);
    } else if (stmt->type == STMT_FUNC_DEF) {
        codegen_generate_func_def(gen, stmt);
    } else if (stmt->type == STMT_RETURN) {
        codegen_generate_return(gen, stmt);
    } else if (stmt->type == STMT_DECLARATION) {
        codegen_generate_declaration(gen, stmt->declaration);
    } else if (stmt->type == STMT_ASSIGNMENT) {
        codegen_generate_assignment(gen, stmt->assignment.var_name, stmt->assignment.field_name, stmt->assignment.array_index, stmt->assignment.dereference_target, stmt->assignment.value);
    } else if (stmt->type == STMT_MULTI_ASSIGNMENT) {
        // Multi-assignment: a, b = func()
        if (!stmt->multi_assignment.func_call) {
            fprintf(stderr, "Codegen error: NULL function call in multi-assignment\n");
            exit(1);
        }
        
        // Call function (result in rax for first value, rdx for second)
        codegen_generate_expression_value(gen, stmt->multi_assignment.func_call);
        
        // Assign values to variables
        // First variable gets rax
        if (stmt->multi_assignment.var_count >= 1) {
            int offset = codegen_find_variable(gen, stmt->multi_assignment.var_names[0]);
            if (offset == -1) {
                fprintf(stderr, "Codegen error: Variable '%s' not found\n", stmt->multi_assignment.var_names[0]);
                exit(1);
            }
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax   ; %s = first return value",
                     offset, stmt->multi_assignment.var_names[0]);
            codegen_emit(gen, buffer);
        }
        
        // Second variable gets rdx
        if (stmt->multi_assignment.var_count >= 2) {
            int offset = codegen_find_variable(gen, stmt->multi_assignment.var_names[1]);
            if (offset == -1) {
                fprintf(stderr, "Codegen error: Variable '%s' not found\n", stmt->multi_assignment.var_names[1]);
                exit(1);
            }
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rdx   ; %s = second return value",
                     offset, stmt->multi_assignment.var_names[1]);
            codegen_emit(gen, buffer);
        }
        
        // Additional variables (if supported in future)
        if (stmt->multi_assignment.var_count > 2) {
            fprintf(stderr, "Codegen warning: More than 2 values in multi-assignment not fully supported\n");
        }
    } else if (stmt->type == STMT_PRINT) {
        codegen_generate_print(gen, stmt->print_stmt.expr);
    } else if (stmt->type == STMT_IF) {
        codegen_generate_if(gen, stmt);
    } else if (stmt->type == STMT_FOR) {
        codegen_generate_for(gen, stmt);
    } else if (stmt->type == STMT_WHILE) {
        codegen_generate_while(gen, stmt);
    } else if (stmt->type == STMT_DO_WHILE) {
        codegen_generate_do_while(gen, stmt);
    } else if (stmt->type == STMT_SWITCH) {
        codegen_generate_switch(gen, stmt);
    } else if (stmt->type == STMT_TRY_CATCH) {
        // Real exception handling with setjmp/longjmp
        char buffer[256];
        int try_label = gen->label_counter++;
        int end_label = gen->label_counter++;
        int finally_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Try-catch-finally block (setjmp/longjmp)");
        
        // Push exception handler
        codegen_emit(gen, "    ; Push exception handler");
        codegen_emit(gen, "    call mlp_exception_push");
        codegen_emit(gen, "    mov r15, rax        ; Save handler pointer");
        
        // setjmp(handler->jump_buffer)
        codegen_emit(gen, "    ; setjmp - returns 0 on first call, 1 on longjmp");
        codegen_emit(gen, "    mov rdi, r15");
        codegen_emit(gen, "    call setjmp");
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d_catch    ; Jump to catch if exception thrown", try_label);
        codegen_emit(gen, buffer);
        
        // Try body
        snprintf(buffer, sizeof(buffer), ".L%d_try:", try_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Try body");
        for (int i = 0; i < stmt->try_catch.try_count; i++) {
            codegen_generate_statement(gen, stmt->try_catch.try_body[i]);
        }
        
        // No exception - pop handler and jump to finally
        codegen_emit(gen, "    ; Try succeeded - pop handler");
        codegen_emit(gen, "    call mlp_exception_pop");
        if (stmt->try_catch.finally_count > 0) {
            snprintf(buffer, sizeof(buffer), "    jmp .L%d_finally", finally_label);
        } else {
            snprintf(buffer, sizeof(buffer), "    jmp .L%d_end", end_label);
        }
        codegen_emit(gen, buffer);
        
        // Catch blocks
        snprintf(buffer, sizeof(buffer), ".L%d_catch:", try_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Catch block(s)");
        
        int rethrow_label = gen->label_counter++;
        
        for (int c = 0; c < stmt->try_catch.catch_count; c++) {
            int next_catch_label = (c < stmt->try_catch.catch_count - 1) ? 
                                    gen->label_counter++ : rethrow_label;
            
            codegen_emit(gen, "");
            snprintf(buffer, sizeof(buffer), "    ; Catch block %d", c);
            codegen_emit(gen, buffer);
            
            // Type check if specified
            if (stmt->try_catch.catch_blocks[c].exception_type) {
                int str_id = codegen_add_string(gen, stmt->try_catch.catch_blocks[c].exception_type);
                
                codegen_emit(gen, "    ; Check exception type");
                codegen_emit(gen, "    call mlp_exception_type");
                snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d    ; Expected type", str_id);
                codegen_emit(gen, buffer);
                codegen_emit(gen, "    mov rsi, rax       ; Actual type");
                codegen_emit(gen, "    call strcmp");
                codegen_emit(gen, "    test rax, rax");
                snprintf(buffer, sizeof(buffer), "    jnz .L%d          ; Type mismatch, try next catch", next_catch_label);
                codegen_emit(gen, buffer);
            }
            
            // Store exception variable if specified
            if (stmt->try_catch.catch_blocks[c].exception_var) {
                codegen_emit(gen, "    ; Store exception in variable");
                gen->stack_offset += 8;
                
                VarInfo* var = malloc(sizeof(VarInfo));
                var->name = malloc(strlen(stmt->try_catch.catch_blocks[c].exception_var) + 1);
                strcpy(var->name, stmt->try_catch.catch_blocks[c].exception_var);
                var->stack_offset = gen->stack_offset;
                var->type = TYPE_STRING;  // Exception is string pointer
                var->struct_name = NULL;
                var->is_dynamic_array = 0;
                var->is_pointer = 0;
                var->next = gen->variables;
                gen->variables = var;
                
                codegen_emit(gen, "    call mlp_exception_message");
                snprintf(buffer, sizeof(buffer), "    mov [rbp-%d], rax", gen->stack_offset);
                codegen_emit(gen, buffer);
            }
            
            // Generate catch body
            for (int i = 0; i < stmt->try_catch.catch_blocks[c].body_count; i++) {
                codegen_generate_statement(gen, stmt->try_catch.catch_blocks[c].body[i]);
            }
            
            // Pop handler after successful catch
            codegen_emit(gen, "    ; Caught - pop handler");
            codegen_emit(gen, "    call mlp_exception_pop");
            
            // Jump to finally or end
            if (stmt->try_catch.finally_count > 0) {
                snprintf(buffer, sizeof(buffer), "    jmp .L%d_finally", finally_label);
            } else {
                snprintf(buffer, sizeof(buffer), "    jmp .L%d_end", end_label);
            }
            codegen_emit(gen, buffer);
            
            // Next catch label
            if (c < stmt->try_catch.catch_count - 1) {
                snprintf(buffer, sizeof(buffer), ".L%d:", next_catch_label);
                codegen_emit(gen, buffer);
            }
        }
        
        // No catch matched - re-throw or uncaught
        snprintf(buffer, sizeof(buffer), ".L%d:  ; Re-throw/Uncaught", rethrow_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; No catch matched - check if parent handler exists (before popping)");
        
        // Check for parent handler WITHOUT popping yet
        codegen_emit(gen, "    call mlp_exception_has_parent_handler");
        codegen_emit(gen, "    test rax, rax");
        snprintf(buffer, sizeof(buffer), "    jnz .L%d_do_rethrow", rethrow_label);
        codegen_emit(gen, buffer);
        
        // No parent handler - uncaught exception, terminate (DON'T pop, data still valid)
        snprintf(buffer, sizeof(buffer), ".L%d_uncaught:", rethrow_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Uncaught exception - print and exit");
        
        // Print "Uncaught exception: " prefix
        int uncaught_str = codegen_add_string(gen, "Uncaught exception: ");
        snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d", uncaught_str);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    call print_string");
        
        // Print exception type
        codegen_emit(gen, "    call mlp_exception_type");
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    call print_string");
        
        // Print " - "
        int sep_str = codegen_add_string(gen, " - ");
        snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d", sep_str);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    call print_string");
        
        // Print exception message
        codegen_emit(gen, "    call mlp_exception_message");
        codegen_emit(gen, "    mov rdi, rax");
        codegen_emit(gen, "    call print_string");
        
        codegen_emit(gen, "    mov rax, 60       ; sys_exit");
        codegen_emit(gen, "    mov rdi, 1        ; error code");
        codegen_emit(gen, "    syscall");
        
        // Has parent handler - pop and re-throw
        snprintf(buffer, sizeof(buffer), ".L%d_do_rethrow:", rethrow_label);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    ; Re-throw to parent handler");
        codegen_emit(gen, "    call mlp_exception_type");
        codegen_emit(gen, "    push rax");
        codegen_emit(gen, "    call mlp_exception_message");
        codegen_emit(gen, "    push rax");
        codegen_emit(gen, "    call mlp_exception_code");
        codegen_emit(gen, "    mov rdx, rax");
        codegen_emit(gen, "    pop rsi");
        codegen_emit(gen, "    pop rdi");
        codegen_emit(gen, "    call mlp_exception_pop    ; Pop AFTER saving data");
        codegen_emit(gen, "    call mlp_throw");
        
        // Finally block
        if (stmt->try_catch.finally_count > 0) {
            snprintf(buffer, sizeof(buffer), ".L%d_finally:", finally_label);
            codegen_emit(gen, buffer);
            codegen_emit(gen, "    ; Finally block");
            for (int i = 0; i < stmt->try_catch.finally_count; i++) {
                codegen_generate_statement(gen, stmt->try_catch.finally_body[i]);
            }
        }
        
        // End label
        snprintf(buffer, sizeof(buffer), ".L%d_end:", end_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_THROW) {
        // Real throw with mlp_throw runtime
        char buffer[256];
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Throw exception");
        
        // Get exception type (or default "Error")
        if (stmt->throw_stmt.error_type) {
            int str_id = codegen_add_string(gen, stmt->throw_stmt.error_type);
            snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d    ; Exception type", str_id);
            codegen_emit(gen, buffer);
        } else {
            int str_id = codegen_add_string(gen, "Error");
            snprintf(buffer, sizeof(buffer), "    mov rdi, str_%d    ; Default type: Error", str_id);
            codegen_emit(gen, buffer);
        }
        
        // Evaluate message expression
        if (stmt->throw_stmt.error_message) {
            codegen_emit(gen, "    push rdi           ; Save type");
            codegen_generate_expression_value(gen, stmt->throw_stmt.error_message);
            codegen_emit(gen, "    mov rsi, rax       ; Message in rsi");
            codegen_emit(gen, "    pop rdi            ; Restore type");
        } else {
            int str_id = codegen_add_string(gen, "Unknown error");
            snprintf(buffer, sizeof(buffer), "    mov rsi, str_%d    ; Default message", str_id);
            codegen_emit(gen, buffer);
        }
        
        // Exception code (default 1)
        codegen_emit(gen, "    mov rdx, 1         ; Error code");
        
        // Call mlp_throw (does not return - longjmp to handler)
        codegen_emit(gen, "    call mlp_throw");
    } else if (stmt->type == STMT_EXIT) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'exit' outside loop\n");
            exit(1);
        }
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; exit", gen->loop_end_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_CONTINUE) {
        if (!gen->in_loop) {
            fprintf(stderr, "Codegen error: 'continue' outside loop\n");
            exit(1);
        }
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .L%d           ; continue", gen->loop_continue_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_EXPR_STMT) {
        // Expression statement (e.g., function call for side effects)
        codegen_generate_expression_value(gen, stmt->print_stmt.expr);
        // Result in rax, but we don't use it
    } else if (stmt->type == STMT_MODULE_DEF) {
        // Module definition: Generate code for module body with namespace prefix
        codegen_emit(gen, "");
        char comment[256];
        snprintf(comment, sizeof(comment), "; Module: %s", stmt->module_def.module_name);
        codegen_emit(gen, comment);
        
        // Set current module context
        gen->current_module = stmt->module_def.module_name;
        
        // Generate code for all statements in module body
        for (int i = 0; i < stmt->module_def.body_count; i++) {
            codegen_generate_statement(gen, stmt->module_def.body[i]);
        }
        
        // Clear module context
        gen->current_module = NULL;
        
        codegen_emit(gen, "; End of module");
        codegen_emit(gen, "");
    } else if (stmt->type == STMT_DEBUG_LABEL) {
        // Debug label: Generate assembly label
        char label[256];
        snprintf(label, sizeof(label), ".debug_%s:", stmt->debug_label.label_name);
        codegen_emit(gen, "");
        codegen_emit(gen, label);
    } else if (stmt->type == STMT_DEBUG_GOTO) {
        // Debug goto: Generate jump to debug label
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "    jmp .debug_%s           ; debug goto", 
                 stmt->debug_goto.target_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_DEBUG_IF) {
        // Debug if: Conditional debug block
        int end_label = gen->label_counter++;
        
        codegen_emit(gen, "");
        codegen_emit(gen, "    ; Debug if block");
        
        // Generate condition check (similar to normal if)
        if (stmt->debug_if.condition->type == EXPR_COMPARISON) {
            codegen_generate_comparison(gen, stmt->debug_if.condition, end_label);
        } else {
            // For non-comparison expressions, check if result is non-zero
            codegen_generate_expression_value(gen, stmt->debug_if.condition);
            codegen_emit(gen, "    cmp rax, 0");
            char buffer[256];
            snprintf(buffer, sizeof(buffer), "    je .L%d", end_label);
            codegen_emit(gen, buffer);
        }
        
        // Generate debug if body
        for (int i = 0; i < stmt->debug_if.body_count; i++) {
            codegen_generate_statement(gen, stmt->debug_if.body[i]);
        }
        
        // End label
        char buffer[256];
        snprintf(buffer, sizeof(buffer), ".L%d:  ; End debug if", end_label);
        codegen_emit(gen, buffer);
    } else if (stmt->type == STMT_DEBUG_PAUSE) {
        // Debug pause: Generate breakpoint (int3 instruction)
        codegen_emit(gen, "    int3                ; debug pause (breakpoint)");
    } else if (stmt->type == STMT_IMPORT) {
        // Import statement: Currently a no-op in codegen
        // In future, this could generate extern declarations
        // For now, we assume all modules are compiled together and linked
        char comment[256];
        if (stmt->import_stmt.alias) {
            snprintf(comment, sizeof(comment), "; Import: %s as %s", 
                     stmt->import_stmt.module_name, stmt->import_stmt.alias);
        } else {
            snprintf(comment, sizeof(comment), "; Import: %s", stmt->import_stmt.module_name);
        }
        codegen_emit(gen, comment);
    }
}

void codegen_generate(Codegen* gen, AST* ast) {
    codegen_emit_prologue(gen);
    
    // Pre-pass: Collect all string literals (traverse AST)
    // For simplicity, we'll collect them during code generation
    // and emit data section at the end before rewriting
    // Actually, let's use a simpler approach: generate code first,
    // then emit string data at the top
    
    // We'll need to reorganize output. For now, let's use memory buffer approach
    // Store current file position, generate code, then insert strings at top
    
    // Simple workaround: Close file, generate to temp, rewrite with strings
    // Even simpler: Just emit strings before code section
    
    codegen_emit(gen, "");
    
    // First pass: Register struct types, generate function definitions, and module definitions
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type == STMT_STRUCT_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_FUNC_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        } else if (ast->statements[i]->type == STMT_MODULE_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        }
    }
    
    // Generate main code
    codegen_emit(gen, "");
    codegen_emit(gen, "_start:");
    
    // Get argc and argv from stack BEFORE modifying stack
    // At program start: [rsp] = argc, [rsp+8] = argv[0], [rsp+16] = argv[1], ...
    codegen_emit(gen, "    ; Get argc and argv from stack");
    codegen_emit(gen, "    pop rdi               ; argc (first item on stack)");
    codegen_emit(gen, "    mov rsi, rsp          ; argv (pointer to argv[0])");
    codegen_emit(gen, "    call mlp_get_argv     ; Convert to MLP string array");
    codegen_emit(gen, "");
    
    // Now setup stack frame
    codegen_emit(gen, "    push rbp");
    codegen_emit(gen, "    mov rbp, rsp");
    codegen_emit(gen, "");
    
    // Check if main function exists and call it
    int has_main = 0;
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type == STMT_FUNC_DEF &&
            strcmp(ast->statements[i]->func_def.func_name, "main") == 0) {
            has_main = 1;
            break;
        }
    }
    
    if (has_main) {
        codegen_emit(gen, "    ; Call main function");
        codegen_emit(gen, "    call func_main");
    }
    
    // Second pass: Generate non-function, non-struct, non-module statements
    for (int i = 0; i < ast->count; i++) {
        if (ast->statements[i]->type != STMT_FUNC_DEF && 
            ast->statements[i]->type != STMT_STRUCT_DEF &&
            ast->statements[i]->type != STMT_MODULE_DEF) {
            codegen_generate_statement(gen, ast->statements[i]);
        }
    }
    
    codegen_emit_epilogue(gen);
    
    // Now emit string data section at the end
    // We need to reorganize - strings should be at top after section .data
    // For now, let's just add them at the end as data section
    if (gen->strings) {
        codegen_emit(gen, "");
        codegen_emit(gen, "section .data");
        StringLiteral* str = gen->strings;
        while (str) {
            char buffer[512];
            snprintf(buffer, sizeof(buffer), "str_%d: db \"%s\", 0", str->id, str->value);
            codegen_emit(gen, buffer);
            str = str->next;
        }
    }
}

void codegen_free(Codegen* gen) {
    fclose(gen->out);
    free(gen);
}
