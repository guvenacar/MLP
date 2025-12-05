#include <stdio.h>
#include <stdlib.h>
#include "standard_library.h"

int main(int argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("Standard Library Module - Standalone Test\n");
    printf("=========================================\n\n");

    // Create standard library context
    StdLibContext* ctx = stdlib_context_create();

    // Initialize all standard bindings
    stdlib_init_all(ctx);

    // Print available functions
    stdlib_print_functions(ctx);
    stdlib_print_constants(ctx);

    // Test function lookup
    printf("\n\nTesting Function Lookup:\n");
    printf("========================\n");

    const char* test_funcs[] = {"printf", "malloc", "sin", "sqrt"};
    for (int i = 0; i < 4; i++) {
        FunctionBinding* binding = stdlib_lookup_function(ctx, test_funcs[i]);
        if (binding) {
            printf("✓ Found %s -> %s (%s)\n",
                   binding->mlp_name, binding->c_name, binding->return_type);
        } else {
            printf("✗ Not found: %s\n", test_funcs[i]);
        }
    }

    // Test constant lookup
    printf("\n\nTesting Constant Lookup:\n");
    printf("========================\n");

    const char* test_consts[] = {"NULL", "PI", "E"};
    for (int i = 0; i < 3; i++) {
        ConstantBinding* binding = stdlib_lookup_constant(ctx, test_consts[i]);
        if (binding) {
            printf("✓ Found %s -> %s = %s\n",
                   binding->mlp_name, binding->c_name, binding->value);
        } else {
            printf("✗ Not found: %s\n", test_consts[i]);
        }
    }

    // Example code generation
    printf("\n\nExample Code Generation:\n");
    printf("========================\n");
    printf("MLP code: printf(\"Hello, World!\\n\")\n");
    printf("Generated C code:\n");
    const char* args[] = {"\"Hello, World!\\n\""};
    stdlib_generate_function_call(stdout, ctx, "printf", args, 1);

    printf("\n\nStandard Library Features:\n");
    printf("=========================\n");
    printf("  1. C standard library bindings (stdio, stdlib, string)\n");
    printf("  2. Math library bindings (math.h)\n");
    printf("  3. POSIX bindings (pthread, unistd)\n");
    printf("  4. System call wrappers\n");
    printf("  5. Automatic library linking\n");
    printf("  6. Type mapping (MLP types <-> C types)\n");
    printf("  7. Function documentation\n");
    printf("  8. Custom MLP standard library\n");

    printf("\n\nExample MLP syntax:\n");
    printf("==================\n");
    printf("  # Using C stdlib functions\n");
    printf("  import std.io\n");
    printf("  \n");
    printf("  func main() {\n");
    printf("      printf(\"Hello, World!\\n\")\n");
    printf("      let x = sqrt(16.0)\n");
    printf("      println(\"sqrt(16) = {}\", x)\n");
    printf("  }\n");

    // Cleanup
    stdlib_context_destroy(ctx);

    return 0;
}
