#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

// Core modules (no conflicts)
#include "modules/variable/variable_parser.h"
#include "modules/variable/variable_codegen.h"
#include "modules/arithmetic/arithmetic_parser.h"
#include "modules/arithmetic/arithmetic_codegen.h"
#include "modules/comparison/comparison_parser.h"
#include "modules/comparison/comparison_codegen.h"
#include "modules/logical/logical_parser.h"
#include "modules/logical/logical_codegen.h"
#include "modules/string_ops/string_ops_parser.h"
#include "modules/string_ops/string_ops_codegen.h"
#include "modules/array/array_parser.h"
#include "modules/array/array_codegen.h"
#include "modules/struct/struct_parser.h"
#include "modules/struct/struct_codegen.h"
#include "modules/control_flow/control_flow_parser.h"
#include "modules/control_flow/control_flow_codegen.h"
#include "modules/function/function_parser.h"
#include "modules/function/function_codegen.h"
#include "modules/pointer/pointer_parser.h"
#include "modules/pointer/pointer_codegen.h"
#include "modules/memory/memory_parser.h"
#include "modules/memory/memory_codegen.h"
#include "modules/cli_io/cli_io_parser.h"
#include "modules/cli_io/cli_io_codegen.h"
#include "modules/file_io/file_io_parser.h"
#include "modules/file_io/file_io_codegen.h"
// #include "modules/null_safety/null_safety_parser.h"  // Conflicts with pointer module
// #include "modules/null_safety/null_safety_codegen.h"
#include "modules/preprocessor/preprocessor_parser.h"
#include "modules/preprocessor/preprocessor_codegen.h"
#include "modules/reflection/reflection_parser.h"
#include "modules/reflection/reflection_codegen.h"
#include "modules/debug_features/debug_features_parser.h"
#include "modules/debug_features/debug_features_codegen.h"
#include "modules/concurrency/concurrency_parser.h"
#include "modules/concurrency/concurrency_codegen.h"
#include "modules/state_management/state_management_parser.h"
#include "modules/state_management/state_management_codegen.h"

// Advanced modules (Stage 1 - conflicts with each other, include separately)
// #include "modules/lambda/lambda_parser.h"
// #include "modules/generator/generator_parser.h"
// #include "modules/async/async_parser.h"
// #include "modules/exception_handling/exception_handling_parser.h"
// #include "modules/module_system/module_system_parser.h"
// #include "modules/interface_trait/interface_trait_parser.h"
// #include "modules/enum/enum_parser.h"
// #include "modules/pattern_matching/pattern_matching_parser.h"
// #include "modules/generic_types/generic_types_parser.h"
// #include "modules/switch_match/switch_match_parser.h"
// #include "modules/operator_overloading/operator_overloading_parser.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.s>\n", argv[0]);
        return 1;
    }
    
    // Read source file
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open input file\n");
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    // Compile
    Lexer* lexer = lexer_create(source);
    Parser* parser = parser_create(lexer);
    Program* program = parser_parse(parser);
    
    if (program) {
        codegen_generate(program, argv[2]);
        printf("Compilation successful: %s -> %s\n", argv[1], argv[2]);
        program_free(program);
    } else {
        fprintf(stderr, "Compilation failed\n");
        parser_free(parser);
        lexer_free(lexer);
        free(source);
        return 1;
    }
    
    parser_free(parser);
    lexer_free(lexer);
    free(source);
    
    return 0;
}
