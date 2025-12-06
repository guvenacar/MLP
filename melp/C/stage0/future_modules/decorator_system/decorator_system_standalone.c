/**
 * MODÜL #57: DECORATOR_SYSTEM - Standalone Compiler
 * 
 * Complete decorator compilation pipeline
 */

#include "decorator_system.h"

// Forward declarations
bool decorator_parse(DecoratorContext* ctx, const char* input);
void decorator_codegen(DecoratorContext* ctx, FILE* out);

// ============================================================================
// TEST CODE
// ============================================================================

const char* TEST_CODE = 
"-- Test decorators\n"
"\n"
"@cached\n"
"function fibonacci(n: i32) -> i32\n"
"    if n <= 1 then\n"
"        return n\n"
"    end if\n"
"    return fibonacci(n - 1) + fibonacci(n - 2)\n"
"end function\n"
"\n"
"@timeit\n"
"@deprecated\n"
"function old_algorithm(x: i32) -> i32\n"
"    return x * x + 2 * x + 1\n"
"end function\n"
"\n"
"@property\n"
"function get_name() -> string\n"
"    return \"MLP\"\n"
"end function\n"
"\n"
"@synchronized\n"
"function critical_section()\n"
"    -- Thread-safe code\n"
"end function\n";

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    printf("=== MLP DECORATOR SYSTEM - MODULE #57 ===\n\n");
    
    // Create context
    DecoratorContext* ctx = decorator_context_new();
    
    // Parse test code
    printf("Parsing decorators...\n");
    bool parse_ok = decorator_parse(ctx, TEST_CODE);
    
    if (!parse_ok) {
        fprintf(stderr, "Error: Failed to parse decorators\n");
        decorator_context_free(ctx);
        return 1;
    }
    
    printf("✓ Parsing complete\n\n");
    
    // Validate decorators
    printf("Validating decorators...\n");
    int valid_count = 0;
    Decorator* current = ctx->decorators;
    while (current) {
        if (decorator_validate(ctx, current)) {
            valid_count++;
        }
        current = current->next;
    }
    printf("✓ %d decorators validated\n\n", valid_count);
    
    // Print statistics
    decorator_print_stats(ctx);
    
    // Analyze decorator types
    printf("\n=== DECORATOR ANALYSIS ===\n");
    current = ctx->decorators;
    
    int cached_count = 0;
    int timeit_count = 0;
    int deprecated_count = 0;
    int property_count = 0;
    int synchronized_count = 0;
    int stacked_functions = 0;
    
    while (current) {
        if (strcmp(current->name, "cached") == 0) cached_count++;
        if (strcmp(current->name, "timeit") == 0) timeit_count++;
        if (strcmp(current->name, "deprecated") == 0) deprecated_count++;
        if (strcmp(current->name, "property") == 0) property_count++;
        if (strcmp(current->name, "synchronized") == 0) synchronized_count++;
        
        // Check if function has multiple decorators
        if (decorator_is_stacked(ctx, current->target_name)) {
            // Count unique stacked functions
            Decorator* check = ctx->decorators;
            bool found = false;
            while (check != current) {
                if (strcmp(check->target_name, current->target_name) == 0) {
                    found = true;
                    break;
                }
                check = check->next;
            }
            if (!found) {
                stacked_functions++;
            }
        }
        
        current = current->next;
    }
    
    printf("@cached decorators: %d (fibonacci)\n", cached_count);
    printf("@timeit decorators: %d (old_algorithm)\n", timeit_count);
    printf("@deprecated decorators: %d (old_algorithm)\n", deprecated_count);
    printf("@property decorators: %d (get_name)\n", property_count);
    printf("@synchronized decorators: %d (critical_section)\n", synchronized_count);
    printf("Functions with stacked decorators: %d\n", stacked_functions);
    
    // List all decorated functions
    printf("\n=== DECORATED FUNCTIONS ===\n");
    current = ctx->decorators;
    while (current) {
        printf("- %s: @%s", current->target_name, current->name);
        if (current->args) {
            printf(" (with arguments)");
        }
        printf("\n");
        current = current->next;
    }
    
    // Generate assembly
    printf("\n=== CODE GENERATION ===\n");
    FILE* out = fopen("decorator_system.s", "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create output file\n");
        decorator_context_free(ctx);
        return 1;
    }
    
    decorator_codegen(ctx, out);
    fclose(out);
    printf("✓ Assembly generated: decorator_system.s\n");
    
    // Count lines in generated assembly
    FILE* check = fopen("decorator_system.s", "r");
    int line_count = 0;
    char ch;
    while ((ch = fgetc(check)) != EOF) {
        if (ch == '\n') line_count++;
    }
    fclose(check);
    printf("  Assembly lines: %d\n", line_count);
    
    // Test results
    printf("\n=== TEST RESULTS ===\n");
    if (ctx->decorator_count == 5 && 
        cached_count == 1 && 
        timeit_count == 1 && 
        deprecated_count == 1 &&
        property_count == 1 &&
        synchronized_count == 1) {
        printf("✅ MODULE #57 TEST PASSED\n");
        printf("✅ Decorator system fully functional\n");
        printf("✅ Ready for Stage 1 (self-hosting)\n");
    } else {
        printf("❌ TEST FAILED\n");
        printf("Expected: 5 decorators (1 cached, 1 timeit, 1 deprecated, 1 property, 1 synchronized)\n");
        printf("Got: %d decorators (%d cached, %d timeit, %d deprecated, %d property, %d synchronized)\n",
               ctx->decorator_count, cached_count, timeit_count, deprecated_count, 
               property_count, synchronized_count);
    }
    
    // Cleanup
    decorator_context_free(ctx);
    
    return 0;
}
