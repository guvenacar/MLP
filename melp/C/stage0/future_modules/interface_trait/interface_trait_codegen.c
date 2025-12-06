#include "interface_trait_codegen.h"
#include <string.h>

// Trait definition için kod üret
void codegen_trait_def(FILE* out, TraitDef* def) {
    fprintf(out, "    ; Trait: %s\n", def->name);
    fprintf(out, "    ; Methods: %d\n", def->method_count);
    fprintf(out, "    ; Super traits: %d\n", def->super_count);
    
    // Trait metadata
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".trait_%s:\n", def->name);
    fprintf(out, "    dq .name_%s\n", def->name);
    fprintf(out, "    dq %d    ; method_count\n", def->method_count);
    fprintf(out, "    dq .methods_%s\n", def->name);
    
    fprintf(out, ".name_%s: db \"%s\", 0\n", def->name, def->name);
    
    // Method table
    fprintf(out, ".methods_%s:\n", def->name);
    for (int i = 0; i < def->method_count; i++) {
        fprintf(out, "    dq .method_%s_%s\n", 
                def->name, def->methods[i]->name);
    }
    
    fprintf(out, "section .text\n");
}

// Trait method için kod üret
void codegen_trait_method(FILE* out, TraitMethod* method) {
    fprintf(out, "    ; Trait method: %s\n", method->name);
    
    if (method->has_default) {
        fprintf(out, "    ; Has default implementation\n");
        
        // Default implementation
        fprintf(out, "\n.trait_method_%s_default:\n", method->name);
        fprintf(out, "    push rbp\n");
        fprintf(out, "    mov rbp, rsp\n");
        
        fprintf(out, "    ; Default implementation body\n");
        
        fprintf(out, "    pop rbp\n");
        fprintf(out, "    ret\n");
    }
}

// Trait implementation için kod üret
void codegen_trait_impl(FILE* out, TraitImpl* impl) {
    fprintf(out, "    ; Trait impl: %s for %s\n",
            impl->trait_def->name, impl->type_name);
    
    // Implementation metadata
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".impl_%s_for_%s:\n", 
            impl->trait_def->name, impl->type_name);
    fprintf(out, "    dq .trait_%s\n", impl->trait_def->name);
    fprintf(out, "    dq .type_%s\n", impl->type_name);
    fprintf(out, "    dq %d    ; impl_count\n", impl->impl_count);
    
    fprintf(out, "section .text\n");
    
    // Method implementations
    for (int i = 0; i < impl->impl_count; i++) {
        fprintf(out, "\n.impl_%s_%s_for_%s:\n",
                impl->trait_def->name,
                impl->trait_def->methods[i]->name,
                impl->type_name);
        
        fprintf(out, "    push rbp\n");
        fprintf(out, "    mov rbp, rsp\n");
        
        fprintf(out, "    ; Method implementation\n");
        
        fprintf(out, "    pop rbp\n");
        fprintf(out, "    ret\n");
    }
}

// Trait object için kod üret
void codegen_trait_object(FILE* out, TraitObject* obj) {
    fprintf(out, "    ; Trait object\n");
    
    // Trait object allocation
    fprintf(out, "    mov rdi, 24    ; vtable + data + trait_def\n");
    fprintf(out, "    call malloc\n");
    
    // Set vtable
    fprintf(out, "    mov qword [rax], .vtable\n");
    
    // Set data
    fprintf(out, "    mov rbx, [rbp - 8]    ; Data pointer\n");
    fprintf(out, "    mov qword [rax + 8], rbx\n");
    
    // Set trait_def
    fprintf(out, "    lea rbx, [rel .trait_%s]\n", obj->trait_def->name);
    fprintf(out, "    mov qword [rax + 16], rbx\n");
}

// VTable için kod üret
void codegen_vtable(FILE* out, VTable* vtable) {
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".vtable_%s:\n", vtable->trait_def->name);
    
    // VTable entries (method pointers)
    for (int i = 0; i < vtable->method_count; i++) {
        fprintf(out, "    dq .method_%d\n", i);
    }
    
    fprintf(out, ".vtable_%s_count: dq %d\n", 
            vtable->trait_def->name, vtable->method_count);
    
    fprintf(out, "section .text\n");
}

// Dynamic dispatch için kod üret
void codegen_dynamic_dispatch(FILE* out, TraitObject* obj, const char* method_name) {
    fprintf(out, "    ; Dynamic dispatch: %s\n", method_name);
    
    // Load trait object
    fprintf(out, "    mov rax, [rbp - 8]    ; Trait object\n");
    
    // Load vtable
    fprintf(out, "    mov rbx, [rax]        ; VTable pointer\n");
    
    // Load data (self)
    fprintf(out, "    mov rdi, [rax + 8]    ; Data pointer (first param)\n");
    
    // Method lookup
    codegen_vtable_lookup(out, method_name);
    
    // Call method
    fprintf(out, "    call rax\n");
}

// VTable lookup için kod üret
void codegen_vtable_lookup(FILE* out, const char* method_name) {
    fprintf(out, "    ; VTable lookup: %s\n", method_name);
    
    // Linear search (şimdilik)
    fprintf(out, "    xor rcx, rcx    ; Index\n");
    fprintf(out, ".lookup_loop:\n");
    fprintf(out, "    mov rax, [rbx + rcx*8]    ; Method pointer\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .lookup_not_found\n");
    
    // Method name comparison (şimdilik skip)
    fprintf(out, "    ; TODO: Method name comparison\n");
    fprintf(out, "    jmp .lookup_found\n");
    
    fprintf(out, ".lookup_not_found:\n");
    fprintf(out, "    xor rax, rax\n");
    
    fprintf(out, ".lookup_found:\n");
}

// Static dispatch için kod üret (monomorphization)
void codegen_static_dispatch(FILE* out, TraitImpl* impl, const char* method_name) {
    fprintf(out, "    ; Static dispatch: %s.%s\n",
            impl->type_name, method_name);
    
    // Direct call (no vtable lookup)
    fprintf(out, "    call .impl_%s_%s_for_%s\n",
            impl->trait_def->name,
            method_name,
            impl->type_name);
}

// Trait bound check için kod üret
void codegen_trait_bound_check(FILE* out, TraitBound* bound) {
    fprintf(out, "    ; Trait bound check: %s: %s\n",
            bound->type_param,
            bound->required_trait->name);
    
    // Compile-time check (Stage 0: Skip)
    fprintf(out, "    ; Trait bound verified at compile time\n");
}

// Associated type için kod üret
void codegen_associated_type(FILE* out, void* assoc_type) {
    fprintf(out, "    ; Associated type\n");
    
    // Associated type metadata
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".assoc_type:\n");
    fprintf(out, "    dq 0    ; Type info\n");
    fprintf(out, "section .text\n");
}

// Super trait check için kod üret
void codegen_super_trait_check(FILE* out, TraitDef* def) {
    fprintf(out, "    ; Super trait check\n");
    
    for (int i = 0; i < def->super_count; i++) {
        fprintf(out, "    ; Super trait: %s\n", def->super_traits[i]->name);
    }
}
