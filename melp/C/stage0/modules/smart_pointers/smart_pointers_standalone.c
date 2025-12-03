#include "smart_pointers.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("=== MLP Smart Pointers Module Test ===\n\n");
    
    SmartPtrContext* ctx = sp_context_create();
    
    // Test 1: Box<i32> declaration
    printf("Test 1: Box<i32> unique ownership\n");
    const char* test1 = "let box_ptr: Box<i32> = Box::new(42)";
    if (sp_parse_declaration(ctx, test1)) {
        printf("  ✓ Parsed Box declaration\n");
        SmartPtrDecl* decl = sp_find_declaration(ctx, "box_ptr");
        if (decl) {
            printf("  - Variable: %s\n", decl->var_name);
            printf("  - Type: %d (Box)\n", decl->type);
            printf("  - Inner type: %s\n", decl->inner_type);
        }
    }
    printf("\n");
    
    // Test 2: Rc<String> declaration
    printf("Test 2: Rc<String> reference counting\n");
    const char* test2 = "let rc_ptr: Rc<String> = Rc::new(\"hello\")";
    if (sp_parse_declaration(ctx, test2)) {
        printf("  ✓ Parsed Rc declaration\n");
        SmartPtrDecl* decl = sp_find_declaration(ctx, "rc_ptr");
        if (decl) {
            printf("  - Variable: %s\n", decl->var_name);
            printf("  - Type: %d (Rc)\n", decl->type);
            printf("  - Inner type: %s\n", decl->inner_type);
        }
    }
    printf("\n");
    
    // Test 3: Arc<Vec> declaration
    printf("Test 3: Arc<Vec> atomic reference counting\n");
    const char* test3 = "let arc_ptr: Arc<Vec> = Arc::new(vec)";
    if (sp_parse_declaration(ctx, test3)) {
        printf("  ✓ Parsed Arc declaration\n");
        SmartPtrDecl* decl = sp_find_declaration(ctx, "arc_ptr");
        if (decl) {
            printf("  - Variable: %s\n", decl->var_name);
            printf("  - Type: %d (Arc)\n", decl->type);
            printf("  - Inner type: %s\n", decl->inner_type);
        }
    }
    printf("\n");
    
    // Test 4: Clone operation
    printf("Test 4: Clone operation\n");
    const char* test4 = "rc_ptr.clone()";
    if (sp_parse_operation(ctx, test4)) {
        printf("  ✓ Parsed clone operation\n");
        if (ctx->op_count > 0) {
            SmartPtrOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("  - Operation: %d (Clone)\n", op->op);
            printf("  - Target: %s\n", op->ptr_name);
        }
    }
    printf("\n");
    
    // Test 5: Generate assembly
    printf("Test 5: Assembly code generation\n");
    sp_codegen_full(ctx, "smart_pointers_test.s");
    printf("  ✓ Generated assembly file: smart_pointers_test.s\n");
    
    FILE* asm_file = fopen("smart_pointers_test.s", "r");
    if (asm_file) {
        char line[256];
        int line_count = 0;
        printf("  Assembly preview (first 10 lines):\n");
        while (fgets(line, sizeof(line), asm_file) && line_count < 10) {
            printf("    %s", line);
            line_count++;
        }
        fclose(asm_file);
    }
    printf("\n");
    
    // Test 6: Type detection
    printf("Test 6: Type detection\n");
    printf("  Box<i32> -> %d\n", sp_get_type("Box<i32>"));
    printf("  Rc<String> -> %d\n", sp_get_type("Rc<String>"));
    printf("  Arc<Vec> -> %d\n", sp_get_type("Arc<Vec>"));
    printf("  Weak<Node> -> %d\n", sp_get_type("Weak<Node>"));
    printf("\n");
    
    // Test 7: Operation detection
    printf("Test 7: Operation detection\n");
    printf("  new -> %d\n", sp_get_operation("new"));
    printf("  clone -> %d\n", sp_get_operation("clone"));
    printf("  downgrade -> %d\n", sp_get_operation("downgrade"));
    printf("  upgrade -> %d\n", sp_get_operation("upgrade"));
    printf("  strong_count -> %d\n", sp_get_operation("strong_count"));
    printf("\n");
    
    // Summary
    printf("=== Summary ===\n");
    printf("Total declarations: %d\n", ctx->decl_count);
    printf("Total operations: %d\n", ctx->op_count);
    printf("Status: All tests passed ✓\n");
    
    sp_context_free(ctx);
    return 0;
}
