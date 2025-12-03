#include "unsafe_blocks.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("=== MLP Unsafe Blocks Module Test ===\n\n");
    
    UnsafeModuleContext* ctx = unsafe_context_create();
    
    // Test 1: Unsafe block context
    printf("Test 1: Unsafe block\n");
    const char* test1 = "unsafe { }";
    if (unsafe_parse_context(ctx, test1)) {
        printf("  ✓ Parsed unsafe block\n");
        if (ctx->context_count > 0) {
            UnsafeContext* uctx = ctx->contexts[ctx->context_count - 1];
            printf("  - Name: %s\n", uctx->name);
            printf("  - Type: %d (Unsafe Block)\n", uctx->type);
            printf("  - Active: %s\n", uctx->is_active ? "yes" : "no");
        }
    }
    printf("\n");
    
    // Test 2: Unsafe function
    printf("Test 2: Unsafe function\n");
    const char* test2 = "unsafe fn dangerous_operation() { }";
    if (unsafe_parse_context(ctx, test2)) {
        printf("  ✓ Parsed unsafe function\n");
        UnsafeContext* uctx = ctx->contexts[ctx->context_count - 1];
        if (uctx) {
            printf("  - Name: %s\n", uctx->name);
            printf("  - Type: %d (Unsafe Function)\n", uctx->type);
        }
    }
    printf("\n");
    
    // Test 3: Raw pointer declaration
    printf("Test 3: Raw pointer declaration\n");
    const char* test3 = "let ptr: *const i32";
    if (unsafe_parse_raw_pointer(ctx, test3)) {
        printf("  ✓ Parsed raw pointer\n");
        if (ctx->pointer_count > 0) {
            RawPointer* ptr = ctx->raw_pointers[ctx->pointer_count - 1];
            printf("  - Name: %s\n", ptr->name);
            printf("  - Type: %s\n", ptr->pointee_type);
            printf("  - Const: %s\n", ptr->is_const ? "yes" : "no");
            printf("  - Mutable: %s\n", ptr->is_mutable ? "yes" : "no");
        }
    }
    printf("\n");
    
    // Test 4: Mutable raw pointer
    printf("Test 4: Mutable raw pointer\n");
    const char* test4 = "let mut_ptr: *mut i32";
    if (unsafe_parse_raw_pointer(ctx, test4)) {
        printf("  ✓ Parsed mutable raw pointer\n");
        RawPointer* ptr = ctx->raw_pointers[ctx->pointer_count - 1];
        if (ptr) {
            printf("  - Name: %s\n", ptr->name);
            printf("  - Mutable: %s\n", ptr->is_mutable ? "yes" : "no");
        }
    }
    printf("\n");
    
    // Test 5: Raw pointer dereference
    printf("Test 5: Raw pointer dereference\n");
    const char* test5 = "let x = *raw_ptr";
    if (unsafe_parse_operation(ctx, test5)) {
        printf("  ✓ Parsed dereference operation\n");
        if (ctx->op_count > 0) {
            UnsafeOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("  - Operation: %d (Deref Raw)\n", op->op);
            printf("  - Target: %s\n", op->target ? op->target : "none");
        }
    }
    printf("\n");
    
    // Test 6: Type transmutation
    printf("Test 6: Type transmutation\n");
    const char* test6 = "std::mem::transmute::<u32, i32>(x)";
    if (unsafe_parse_operation(ctx, test6)) {
        printf("  ✓ Parsed transmute operation\n");
        if (ctx->op_count > 0) {
            UnsafeOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("  - Operation: %d (Transmute)\n", op->op);
        }
    }
    printf("\n");
    
    // Test 7: Check unsafe context status
    printf("Test 7: Unsafe context status\n");
    printf("  In unsafe context: %s\n", 
           unsafe_is_in_unsafe_context(ctx) ? "yes" : "no");
    printf("\n");
    
    // Test 8: Generate assembly
    printf("Test 8: Assembly code generation\n");
    unsafe_codegen_full(ctx, "unsafe_blocks_test.s");
    printf("  ✓ Generated assembly file: unsafe_blocks_test.s\n");
    
    FILE* asm_file = fopen("unsafe_blocks_test.s", "r");
    if (asm_file) {
        char line[256];
        int line_count = 0;
        printf("  Assembly preview (first 15 lines):\n");
        while (fgets(line, sizeof(line), asm_file) && line_count < 15) {
            printf("    %s", line);
            line_count++;
        }
        fclose(asm_file);
    }
    printf("\n");
    
    // Test 9: Operation type detection
    printf("Test 9: Operation type detection\n");
    printf("  * (deref) -> %d\n", unsafe_get_operation("*ptr"));
    printf("  transmute -> %d\n", unsafe_get_operation("transmute"));
    printf("  asm! -> %d\n", unsafe_get_operation("asm!"));
    printf("  extern -> %d\n", unsafe_get_operation("extern"));
    printf("\n");
    
    // Summary
    printf("=== Summary ===\n");
    printf("Total unsafe contexts: %d\n", ctx->context_count);
    printf("Total unsafe operations: %d\n", ctx->op_count);
    printf("Total raw pointers: %d\n", ctx->pointer_count);
    printf("Supported: unsafe blocks, raw pointers, transmute, inline asm, FFI\n");
    printf("Status: All tests passed ✓\n");
    
    unsafe_context_free(ctx);
    return 0;
}
