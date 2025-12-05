#include <stdio.h>
#include <stdlib.h>
#include "type_system.h"
#include "type_system_parser.h"
#include "type_system_codegen.h"
#include "../../lexer.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.mlp>\n", argv[0]);
        return 1;
    }

    printf("Type System Module - Standalone Test\n");
    printf("====================================\n\n");

    // Create type context
    TypeContext* ctx = type_context_create();

    // Test basic type creation
    Type* int_type = type_create(TYPE_INT);
    Type* float_type = type_create(TYPE_FLOAT);
    Type* string_type = type_create(TYPE_STRING);

    printf("Basic types created:\n");
    printf("  - int: %s\n", type_to_string(int_type));
    printf("  - float: %s\n", type_to_string(float_type));
    printf("  - string: %s\n", type_to_string(string_type));
    printf("\n");

    // Test type equality
    bool equals = type_equals(int_type, int_type);
    printf("int == int: %s\n\n", equals ? "true" : "false");

    printf("Type system features to implement:\n");
    printf("  1. Type checking (assignment, operations)\n");
    printf("  2. Type inference (from expressions)\n");
    printf("  3. Generic types (parametric polymorphism)\n");
    printf("  4. Type constraints\n");
    printf("  5. Symbol table management\n");
    printf("  6. User-defined types (struct, enum)\n");
    printf("\n");

    printf("Example syntax:\n");
    printf("  let x: int = 42                # Explicit type\n");
    printf("  let y = 3.14                   # Type inference (float)\n");
    printf("  let arr: array[int] = [1,2,3]  # Array type\n");
    printf("  func add(a: int, b: int) -> int { return a + b }\n");

    // Cleanup
    type_free(int_type);
    type_free(float_type);
    type_free(string_type);
    type_context_destroy(ctx);

    return 0;
}
