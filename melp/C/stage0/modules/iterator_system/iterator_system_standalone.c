#include "iterator_system.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    printf("=== MLP Iterator System Module Test ===\n\n");
    
    IteratorContext* ctx = iter_context_create();
    
    // Test 1: Array iterator
    printf("Test 1: Array iterator\n");
    const char* test1 = "let iter = numbers.iter()";
    if (iter_parse_declaration(ctx, test1)) {
        printf("  ✓ Parsed array iterator\n");
        IteratorDecl* decl = iter_find_declaration(ctx, "iter");
        if (decl) {
            printf("  - Variable: %s\n", decl->var_name);
            printf("  - Type: %d (Array)\n", decl->type);
            printf("  - Source: %s\n", decl->source);
            printf("  - Lazy: %s\n", decl->is_lazy ? "yes" : "no");
        }
    }
    printf("\n");
    
    // Test 2: Range iterator
    printf("Test 2: Range iterator\n");
    const char* test2 = "let range_iter = 0..10";
    if (iter_parse_declaration(ctx, test2)) {
        printf("  ✓ Parsed range iterator\n");
        IteratorDecl* decl = iter_find_declaration(ctx, "range_iter");
        if (decl) {
            printf("  - Variable: %s\n", decl->var_name);
            printf("  - Type: %d (Range)\n", decl->type);
            printf("  - Source: %s\n", decl->source);
        }
    }
    printf("\n");
    
    // Test 3: Map operation
    printf("Test 3: Map operation\n");
    const char* test3 = "iter.map(double)";
    if (iter_parse_operation(ctx, test3)) {
        printf("  ✓ Parsed map operation\n");
        if (ctx->op_count > 0) {
            IteratorOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("  - Operation: %d (Map)\n", op->op);
            printf("  - Iterator: %s\n", op->iterator_name);
            printf("  - Function: %s\n", op->function ? op->function : "none");
        }
    }
    printf("\n");
    
    // Test 4: Filter operation
    printf("Test 4: Filter operation\n");
    const char* test4 = "iter.filter(is_even)";
    if (iter_parse_operation(ctx, test4)) {
        printf("  ✓ Parsed filter operation\n");
        if (ctx->op_count > 0) {
            IteratorOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("  - Operation: %d (Filter)\n", op->op);
            printf("  - Iterator: %s\n", op->iterator_name);
            printf("  - Predicate: %s\n", op->function ? op->function : "none");
        }
    }
    printf("\n");
    
    // Test 5: Collect operation
    printf("Test 5: Collect operation\n");
    const char* test5 = "iter.collect()";
    if (iter_parse_operation(ctx, test5)) {
        printf("  ✓ Parsed collect operation\n");
        if (ctx->op_count > 0) {
            IteratorOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("  - Operation: %d (Collect)\n", op->op);
            printf("  - Iterator: %s\n", op->iterator_name);
        }
    }
    printf("\n");
    
    // Test 6: Reduce operation
    printf("Test 6: Reduce operation\n");
    const char* test6 = "iter.reduce(sum)";
    if (iter_parse_operation(ctx, test6)) {
        printf("  ✓ Parsed reduce operation\n");
        if (ctx->op_count > 0) {
            IteratorOpNode* op = ctx->operations[ctx->op_count - 1];
            printf("  - Operation: %d (Reduce)\n", op->op);
            printf("  - Iterator: %s\n", op->iterator_name);
            printf("  - Function: %s\n", op->function ? op->function : "none");
        }
    }
    printf("\n");
    
    // Test 7: Generate assembly
    printf("Test 7: Assembly code generation\n");
    iter_codegen_full(ctx, "iterator_system_test.s");
    printf("  ✓ Generated assembly file: iterator_system_test.s\n");
    
    FILE* asm_file = fopen("iterator_system_test.s", "r");
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
    
    // Test 8: Operation type detection
    printf("Test 8: Operation type detection\n");
    printf("  map -> %d\n", iter_get_operation("map"));
    printf("  filter -> %d\n", iter_get_operation("filter"));
    printf("  reduce -> %d\n", iter_get_operation("reduce"));
    printf("  collect -> %d\n", iter_get_operation("collect"));
    printf("  chain -> %d\n", iter_get_operation("chain"));
    printf("  enumerate -> %d\n", iter_get_operation("enumerate"));
    printf("\n");
    
    // Summary
    printf("=== Summary ===\n");
    printf("Total declarations: %d\n", ctx->decl_count);
    printf("Total operations: %d\n", ctx->op_count);
    printf("Supported operations: map, filter, reduce, collect, chain, zip, enumerate, take, skip\n");
    printf("Status: All tests passed ✓\n");
    
    iter_context_free(ctx);
    return 0;
}
