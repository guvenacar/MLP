#include "array_codegen.h"
#include <stdio.h>

// Generate stack-based array (fixed size)
void codegen_array_stack(FILE* output, Array* arr) {
    fprintf(output, "    ; Stack array: %d elements\n", arr->length);
    
    // Allocate space on stack
    int bytes = arr->length * 8;  // 8 bytes per element (pointer or numeric)
    fprintf(output, "    sub rsp, %d\n", bytes);
    
    // TODO: Initialize elements
    fprintf(output, "    ; TODO: Initialize array elements\n");
}

// Generate heap-based array (dynamic size)
void codegen_array_heap(FILE* output, Array* arr) {
    fprintf(output, "    ; Heap array: capacity %d\n", arr->capacity);
    
    // Call malloc for array allocation
    int bytes = arr->capacity * 8;
    fprintf(output, "    mov rdi, %d  ; Size in bytes\n", bytes);
    fprintf(output, "    extern malloc\n");
    fprintf(output, "    call malloc\n");
    fprintf(output, "    ; Array pointer in rax\n");
    
    // TODO: Initialize elements and store pointer
}

// Generate list creation (always heap)
void codegen_list_create(FILE* output, List* list) {
    fprintf(output, "    ; List creation: capacity %d\n", list->capacity);
    
    // Lists need two allocations:
    // 1. Array of element pointers
    // 2. Array of element types
    
    fprintf(output, "    ; Allocate element pointer array\n");
    int bytes_elements = list->capacity * 8;
    fprintf(output, "    mov rdi, %d\n", bytes_elements);
    fprintf(output, "    extern malloc\n");
    fprintf(output, "    call malloc\n");
    fprintf(output, "    push rax  ; Save element array pointer\n");
    
    fprintf(output, "    ; Allocate type array\n");
    int bytes_types = list->capacity * 4;  // 4 bytes per VarType enum
    fprintf(output, "    mov rdi, %d\n", bytes_types);
    fprintf(output, "    call malloc\n");
    fprintf(output, "    ; Type array in rax, element array on stack\n");
    
    // TODO: Store both pointers in List struct
}

// Generate tuple creation (stack allocation)
void codegen_tuple_create(FILE* output, Tuple* tuple) {
    fprintf(output, "    ; Tuple creation: %d elements (immutable)\n", tuple->length);
    
    // Allocate on stack (like struct)
    int bytes = tuple->length * 8 + tuple->length * 4;  // Elements + types
    fprintf(output, "    sub rsp, %d\n", bytes);
    
    // TODO: Initialize tuple elements
    fprintf(output, "    ; TODO: Initialize tuple elements (immutable)\n");
}

// Generate array index access
void codegen_array_index(FILE* output, const char* array_name, int index) {
    fprintf(output, "    ; Array index access: %s[%d]\n", array_name, index);
    
    // Load array base address
    fprintf(output, "    mov rbx, [%s]  ; Array base address\n", array_name);
    
    // Calculate offset: index * 8 (assuming 8-byte elements)
    int offset = index * 8;
    fprintf(output, "    mov rax, [rbx + %d]  ; Get element at index %d\n", offset, index);
}

// Generate list index access (with runtime type info)
void codegen_list_index(FILE* output, const char* list_name, int index) {
    fprintf(output, "    ; List index access: %s(%d)\n", list_name, index);
    
    // Load list structure
    fprintf(output, "    mov rbx, [%s]  ; List element array\n", list_name);
    fprintf(output, "    mov rcx, [%s+8]  ; List type array\n", list_name);
    
    // Get element
    int offset = index * 8;
    fprintf(output, "    mov rax, [rbx + %d]  ; Element pointer\n", offset);
    
    // Get type info
    int type_offset = index * 4;
    fprintf(output, "    mov edx, [rcx + %d]  ; Element type\n", type_offset);
}
