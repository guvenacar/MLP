#include "collections_codegen.h"
#include <stdio.h>
#include <string.h>

// Generate code for tuple literal
// Tuples are immutable and stack-allocated for performance
// Layout: [element0:8][element1:8]...[elementN:8]
void codegen_tuple_literal(CodeGen* gen, TupleLiteral* tuple) {
    char buffer[512];
    
    codegen_emit(gen, "");
    snprintf(buffer, sizeof(buffer), "    ; Tuple literal <%d elements> - immutable, stack", 
             tuple->count);
    codegen_emit(gen, buffer);
    
    if (tuple->count == 0) {
        // Empty tuple: return null/0
        codegen_emit(gen, "    xor rax, rax        ; Empty tuple = 0");
        return;
    }
    
    // Allocate stack space for tuple
    int tuple_size = tuple->count * 8;  // Each element is 8 bytes
    snprintf(buffer, sizeof(buffer), "    sub rsp, %d         ; Allocate tuple space", 
             tuple_size);
    codegen_emit(gen, buffer);
    
    // Store each element on stack (in forward order)
    for (int i = 0; i < tuple->count; i++) {
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; Tuple element %d", i);
        codegen_emit(gen, buffer);
        
        // Generate code for element expression
        // This should evaluate expression and leave result in rax
        codegen_generate_expression(gen, tuple->elements[i]);
        
        // Store element at correct offset
        snprintf(buffer, sizeof(buffer), "    mov [rsp+%d], rax   ; tuple<%d> = value", 
                 i * 8, i);
        codegen_emit(gen, buffer);
    }
    
    // Return pointer to tuple (rsp now points to tuple start)
    codegen_emit(gen, "    mov rax, rsp        ; Return tuple pointer");
}

// Generate code for list literal
// Lists are mutable and heap-allocated
// Layout: [count:8][capacity:8][elem0:8][elem1:8]...[elemN:8]
void codegen_list_literal(CodeGen* gen, ListLiteral* list) {
    char buffer[512];
    
    codegen_emit(gen, "");
    snprintf(buffer, sizeof(buffer), "    ; List literal (%d elements) - mutable, heap", 
             list->count);
    codegen_emit(gen, buffer);
    
    if (list->count == 0) {
        // Empty list: allocate header only
        codegen_emit(gen, "    mov rdi, 16         ; List header: count + capacity");
        codegen_emit(gen, "    call gc_alloc       ; Allocate empty list");
        codegen_emit(gen, "    mov qword [rax], 0  ; count = 0");
        codegen_emit(gen, "    mov qword [rax+8], 0 ; capacity = 0");
        return;
    }
    
    // Calculate total size: header (16 bytes) + elements
    int header_size = 16;  // count + capacity
    int data_size = list->count * 8;
    int total_size = header_size + data_size;
    
    snprintf(buffer, sizeof(buffer), "    mov rdi, %d         ; List size: %d header + %d data", 
             total_size, header_size, data_size);
    codegen_emit(gen, buffer);
    codegen_emit(gen, "    call gc_alloc       ; Allocate list on heap");
    codegen_emit(gen, "    push rax            ; Save list pointer");
    
    // Store count
    snprintf(buffer, sizeof(buffer), "    mov qword [rax], %d ; list.count = %d", 
             list->count, list->count);
    codegen_emit(gen, buffer);
    
    // Store capacity (same as count initially)
    snprintf(buffer, sizeof(buffer), "    mov qword [rax+8], %d ; list.capacity = %d", 
             list->count, list->count);
    codegen_emit(gen, buffer);
    
    // Store each element
    for (int i = 0; i < list->count; i++) {
        codegen_emit(gen, "");
        snprintf(buffer, sizeof(buffer), "    ; List element %d", i);
        codegen_emit(gen, buffer);
        
        // Save list pointer
        codegen_emit(gen, "    mov rbx, [rsp]      ; Load list pointer");
        codegen_emit(gen, "    push rbx            ; Save again");
        
        // Generate element expression
        codegen_generate_expression(gen, list->elements[i]);
        
        // Restore list pointer and store element
        codegen_emit(gen, "    pop rbx             ; Restore list pointer");
        snprintf(buffer, sizeof(buffer), "    mov [rbx+%d], rax   ; list(%d) = value", 
                 16 + i * 8, i);
        codegen_emit(gen, buffer);
        codegen_emit(gen, "    mov [rsp], rbx      ; Update saved pointer");
    }
    
    // Return list pointer
    codegen_emit(gen, "    pop rax             ; Return list pointer");
}

// Generate code for tuple access: tuple<index>
void codegen_tuple_access(CodeGen* gen, TupleAccess* access) {
    char buffer[512];
    
    codegen_emit(gen, "");
    snprintf(buffer, sizeof(buffer), "    ; Tuple access: tuple<%d>", access->index);
    codegen_emit(gen, buffer);
    
    // Generate code for tuple expression (result in rax = tuple pointer)
    codegen_generate_expression(gen, access->tuple_expr);
    
    // Bounds check could be added here (compile-time if tuple is literal)
    // For now, trust the index is valid
    
    // Access element at offset
    snprintf(buffer, sizeof(buffer), "    mov rax, [rax+%d]   ; Load tuple<%d>", 
             access->index * 8, access->index);
    codegen_emit(gen, buffer);
}

// Generate code for list access: list(index)
void codegen_list_access(CodeGen* gen, ListAccess* access) {
    char buffer[512];
    
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; List access: list(index)");
    
    // Generate code for list expression (result in rax = list pointer)
    codegen_generate_expression(gen, access->list_expr);
    codegen_emit(gen, "    push rax            ; Save list pointer");
    
    // Generate code for index expression (result in rax = index)
    codegen_generate_expression(gen, access->index_expr);
    
    // Runtime bounds check
    codegen_emit(gen, "    mov rbx, rax        ; index in rbx");
    codegen_emit(gen, "    pop rax             ; Restore list pointer");
    codegen_emit(gen, "    mov rcx, [rax]      ; Load list.count");
    codegen_emit(gen, "    cmp rbx, rcx        ; Compare index with count");
    codegen_emit(gen, "    jae .list_index_error ; Jump if index >= count");
    
    // Calculate offset: 16 (header) + index * 8
    codegen_emit(gen, "    lea rax, [rax+rbx*8+16] ; Calculate element address");
    codegen_emit(gen, "    mov rax, [rax]      ; Load list(index)");
    codegen_emit(gen, "    jmp .list_access_done");
    
    // Error handling
    codegen_emit(gen, ".list_index_error:");
    codegen_emit(gen, "    ; TODO: Throw index out of bounds exception");
    codegen_emit(gen, "    mov rax, 60         ; sys_exit");
    codegen_emit(gen, "    mov rdi, 1          ; error code");
    codegen_emit(gen, "    syscall");
    
    codegen_emit(gen, ".list_access_done:");
}

// Helper function placeholders (to be implemented by expression codegen)
void codegen_generate_expression(CodeGen* gen, void* expr) {
    // This will call the appropriate codegen based on expression type
    // For now, placeholder
    codegen_emit(gen, "    ; TODO: Generate expression code");
    codegen_emit(gen, "    mov rax, 42         ; Placeholder value");
}

void codegen_emit(CodeGen* gen, const char* line) {
    // Emit assembly line
    // Implementation depends on CodeGen structure
    fprintf(gen->output, "%s\n", line);
}
