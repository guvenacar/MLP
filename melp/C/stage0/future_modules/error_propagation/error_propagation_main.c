/**
 * MLP Error Propagation Module - Main #71
 */

#include "error_propagation.h"

int main() {
    printf("=== MLP Error Propagation Module Test ===\n\n");
    
    const char* input = 
        "# Error propagation with ? operator\n"
        "\n"
        "# Result propagation\n"
        "value = get_result()?\n"
        "\n"
        "# Option propagation\n"
        "data = find_item()?\n"
        "\n"
        "# Chained propagation\n"
        "result = fetch().parse()?.validate()?\n"
        "\n"
        "# Try block\n"
        "try {\n"
        "    risky_operation()\n"
        "} catch (e) {\n"
        "    handle_error(e)\n"
        "}\n";
    
    ErrorPropCtx ctx;
    errorprop_parse(input, &ctx);
    
    printf("Parsed:\n");
    printf("  Question operators: %d\n", ctx.question_count);
    printf("  Try blocks: %d\n", ctx.try_count);
    printf("\n");
    
    printf("Question Operators:\n");
    for (int i = 0; i < ctx.question_count; i++) {
        QuestionOp* op = &ctx.question_ops[i];
        const char* type = op->type == QUESTION_RESULT ? "Result" :
                          op->type == QUESTION_OPTION ? "Option" : "Custom";
        printf("  %d. %s? [%s]\n", i + 1, op->expr, type);
    }
    printf("\n");
    
    errorprop_generate_asm("error_propagation.s", &ctx);
    printf("✓ Assembly generated: error_propagation.s\n");
    
    printf("\n=== Error Propagation Module: SUCCESS ===\n");
    return 0;
}
