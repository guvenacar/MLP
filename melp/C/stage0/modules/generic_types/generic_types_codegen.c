#include "generic_types_codegen.h"
#include <string.h>

// Generic type için kod üret
void codegen_generic_type(FILE* out, GenericType* type) {
    fprintf(out, "    ; Generic type: %s\n", type->name);
    
    // Type parameters
    fprintf(out, "    ; Type parameters: %d\n", type->param_count);
    
    for (int i = 0; i < type->param_count; i++) {
        codegen_type_parameter(out, type->type_params[i]);
    }
    
    // Generic type definition (metadata)
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".generic_%s:\n", type->name);
    fprintf(out, "    dq .name_%s\n", type->name);
    fprintf(out, "    dq %d    ; param_count\n", type->param_count);
    fprintf(out, "    dq .params_%s\n", type->name);
    
    fprintf(out, ".name_%s: db \"%s\", 0\n", type->name, type->name);
    
    fprintf(out, "section .text\n");
}

// Type instantiation için kod üret
void codegen_type_instantiation(FILE* out, TypeInstantiation* inst) {
    fprintf(out, "    ; Type instantiation: %s\n", inst->mangled_name);
    
    // Monomorphization: Generic -> Concrete
    codegen_specialize_type(out, inst->generic_type, 
                           inst->type_args, inst->arg_count);
}

// Generic function için kod üret
void codegen_generic_function(FILE* out, GenericFunction* func) {
    fprintf(out, "    ; Generic function: %s\n", func->name);
    
    // Type parameters
    for (int i = 0; i < func->param_count; i++) {
        codegen_type_parameter(out, func->type_params[i]);
    }
    
    // Generic function metadata
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".generic_func_%s:\n", func->name);
    fprintf(out, "    dq .name_%s\n", func->name);
    fprintf(out, "    dq %d    ; param_count\n", func->param_count);
    
    fprintf(out, ".name_%s: db \"%s\", 0\n", func->name, func->name);
    
    fprintf(out, "section .text\n");
}

// Monomorphization için kod üret
void codegen_monomorphization(FILE* out, MonomorphContext* ctx) {
    fprintf(out, "    ; Monomorphization context\n");
    fprintf(out, "    ; Substitutions: %d\n", ctx->subst_count);
    
    for (int i = 0; i < ctx->subst_count; i++) {
        TypeSubstitution* subst = ctx->substitutions[i];
        fprintf(out, "    ; %s -> <type>\n", subst->from);
    }
}

// Type parameter codegen
void codegen_type_parameter(FILE* out, TypeParameter* param) {
    fprintf(out, "    ; Type parameter: %s\n", param->name);
    
    if (param->has_constraint) {
        fprintf(out, "    ; Constraint: yes\n");
    }
    
    if (param->default_type) {
        fprintf(out, "    ; Default type: yes\n");
    }
}

// Type constraint codegen
void codegen_type_constraint(FILE* out, TypeConstraint* constraint) {
    fprintf(out, "    ; Constraint: %s : %s\n",
            constraint->param_name,
            constraint->trait_name);
}

// Type specialization
void codegen_specialize_type(FILE* out, GenericType* type, void** type_args, int arg_count) {
    fprintf(out, "    ; Specialize type: %s\n", type->name);
    
    // Mangled name
    fprintf(out, "\n.type_");
    codegen_mangled_name(out, type->name, type_args, arg_count);
    fprintf(out, ":\n");
    
    // Specialized struct definition
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    fprintf(out, "    ; Specialized type body\n");
    fprintf(out, "    ; Type args: %d\n", arg_count);
    
    // Type-specific code burada
    for (int i = 0; i < arg_count; i++) {
        fprintf(out, "    ; Type arg %d\n", i);
    }
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Function specialization
void codegen_specialize_function(FILE* out, GenericFunction* func, void** type_args, int arg_count) {
    fprintf(out, "    ; Specialize function: %s\n", func->name);
    
    // Mangled name
    fprintf(out, "\n.func_");
    codegen_mangled_name(out, func->name, type_args, arg_count);
    fprintf(out, ":\n");
    
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    fprintf(out, "    ; Specialized function body\n");
    fprintf(out, "    ; Type args: %d\n", arg_count);
    
    // Type-specific function implementation
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Name mangling (List<int> -> List_int)
void codegen_mangled_name(FILE* out, const char* base_name, void** type_args, int arg_count) {
    fprintf(out, "%s", base_name);
    
    for (int i = 0; i < arg_count; i++) {
        fprintf(out, "_T%d", i);
        // Type name burada append (Stage 0: placeholder)
    }
}

// Type info codegen
void codegen_generic_type_info(FILE* out, void* type) {
    fprintf(out, "    ; Type info\n");
    fprintf(out, "    lea rax, [rel .type_info]\n");
}

// Type size codegen
void codegen_type_size(FILE* out, void* type) {
    fprintf(out, "    ; Type size\n");
    fprintf(out, "    mov rax, 8    ; Placeholder size\n");
}
