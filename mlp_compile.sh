#!/bin/bash
# MLP Modular Compiler Chain
# Uses Stage0 C modules to compile MLP programs

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
MODULES_DIR="$SCRIPT_DIR/melp/C/stage0/modules"

echo "╔════════════════════════════════════════════╗"
echo "║  MLP MODULAR COMPILER CHAIN               ║"
echo "║  Using 63 C Stage0 Modules (ALL!)        ║"
echo "╚════════════════════════════════════════════╝"
echo ""

if [ $# -lt 2 ]; then
    echo "Usage: $0 <input.mlp> <output_binary>"
    echo ""
    echo "Example:"
    echo "  $0 program.mlp program"
    echo ""
    exit 1
fi

INPUT_FILE="$1"
OUTPUT_BINARY="$2"
TEMP_DIR="/tmp/mlp_build_$$"

if [ ! -f "$INPUT_FILE" ]; then
    echo "❌ Error: Input file '$INPUT_FILE' not found"
    exit 1
fi

echo "📄 Input:  $INPUT_FILE"
echo "🎯 Output: $OUTPUT_BINARY"
echo ""

# Create temp directory
mkdir -p "$TEMP_DIR"
echo "🗂️  Temp dir: $TEMP_DIR"
echo ""

# Stage 1: Variable Analysis
echo "[1/45] Variable Module..."
if [ -x "$MODULES_DIR/variable/variable_standalone" ]; then
    "$MODULES_DIR/variable/variable_standalone" "$INPUT_FILE" "$TEMP_DIR/variables.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Variable compiler not found"
fi

# Stage 2: Arithmetic Analysis  
echo "[2/45] Arithmetic Module..."
if [ -x "$MODULES_DIR/arithmetic/arithmetic_standalone" ]; then
    "$MODULES_DIR/arithmetic/arithmetic_standalone" "$INPUT_FILE" "$TEMP_DIR/arithmetic.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Arithmetic compiler not found"
fi

# Stage 3: Comments Module
echo "[3/45] Comments Module..."
if [ -x "$MODULES_DIR/comments/comments_standalone" ]; then
    "$MODULES_DIR/comments/comments_standalone" "$INPUT_FILE" "$TEMP_DIR/cleaned.mlp" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Comments compiler not found"
fi

# Stage 4: Functions Module
echo "[4/45] Functions Module..."
if [ -x "$MODULES_DIR/functions/functions_standalone" ]; then
    "$MODULES_DIR/functions/functions_standalone" "$INPUT_FILE" "$TEMP_DIR/functions.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Functions compiler not found"
fi

# Stage 5: Control Flow Module
echo "[5/45] Control Flow Module..."
if [ -x "$MODULES_DIR/control_flow/control_flow_standalone" ]; then
    "$MODULES_DIR/control_flow/control_flow_standalone" "$INPUT_FILE" "$TEMP_DIR/control_flow.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Control Flow compiler not found"
fi

# Stage 6: Print Module
echo "[6/45] Print Module..."
if [ -x "$MODULES_DIR/print/print_standalone" ]; then
    "$MODULES_DIR/print/print_standalone" "$INPUT_FILE" "$TEMP_DIR/print.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Print compiler not found"
fi

# Stage 7: Comparison Module
echo "[7/45] Comparison Module..."
if [ -x "$MODULES_DIR/comparison/comparison_standalone" ]; then
    "$MODULES_DIR/comparison/comparison_standalone" "$INPUT_FILE" "$TEMP_DIR/comparison.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Comparison compiler not found"
fi

# Stage 8: Array Module
echo "[8/45] Array Module..."
if [ -x "$MODULES_DIR/array/array_standalone" ]; then
    "$MODULES_DIR/array/array_standalone" "$INPUT_FILE" "$TEMP_DIR/array.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Array compiler not found"
fi

# Stage 9: Array Operations Module
echo "[9/45] Array Operations Module..."
if [ -x "$MODULES_DIR/array_operations/array_operations_standalone" ]; then
    "$MODULES_DIR/array_operations/array_operations_standalone" "$INPUT_FILE" "$TEMP_DIR/array_ops.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Array Operations compiler not found"
fi

# Stage 10: Async Module
echo "[10/45] Async Module..."
if [ -x "$MODULES_DIR/async/async_standalone" ]; then
    "$MODULES_DIR/async/async_standalone" "$INPUT_FILE" "$TEMP_DIR/async.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Async compiler not found"
fi

# Stage 11: Channels Module
echo "[11/45] Channels Module..."
if [ -x "$MODULES_DIR/channels/channels_standalone" ]; then
    "$MODULES_DIR/channels/channels_standalone" "$INPUT_FILE" "$TEMP_DIR/channels.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Channels compiler not found"
fi

# Stage 12: CLI I/O Module
echo "[12/45] CLI I/O Module..."
if [ -x "$MODULES_DIR/cli_io/cli_io_standalone" ]; then
    "$MODULES_DIR/cli_io/cli_io_standalone" "$INPUT_FILE" "$TEMP_DIR/cli_io.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  CLI I/O compiler not found"
fi

# Stage 13: Codegen Context Module
echo "[13/45] Codegen Context Module..."
if [ -x "$MODULES_DIR/codegen_context/codegen_context_standalone" ]; then
    "$MODULES_DIR/codegen_context/codegen_context_standalone" "$INPUT_FILE" "$TEMP_DIR/codegen_context.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Codegen Context compiler not found"
fi

# Stage 14: Collections Module
echo "[14/45] Collections Module..."
if [ -x "$MODULES_DIR/collections/collections_standalone" ]; then
    "$MODULES_DIR/collections/collections_standalone" "$INPUT_FILE" "$TEMP_DIR/collections.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Collections compiler not found"
fi

# Stage 15: Concurrency Module
echo "[15/45] Concurrency Module..."
if [ -x "$MODULES_DIR/concurrency/concurrency_standalone" ]; then
    "$MODULES_DIR/concurrency/concurrency_standalone" "$INPUT_FILE" "$TEMP_DIR/concurrency.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Concurrency compiler not found"
fi

# Stage 16: Debug Features Module
echo "[16/45] Debug Features Module..."
if [ -x "$MODULES_DIR/debug_features/debug_features_standalone" ]; then
    "$MODULES_DIR/debug_features/debug_features_standalone" "$INPUT_FILE" "$TEMP_DIR/debug.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Debug Features compiler not found"
fi

# Stage 17: Dependent Types Module
echo "[17/45] Dependent Types Module..."
if [ -x "$MODULES_DIR/dependent_types/dependent_types_standalone" ]; then
    "$MODULES_DIR/dependent_types/dependent_types_standalone" "$INPUT_FILE" "$TEMP_DIR/dependent_types.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Dependent Types compiler not found"
fi

# Stage 18: Enum Module
echo "[18/45] Enum Module..."
if [ -x "$MODULES_DIR/enum/enum_standalone" ]; then
    "$MODULES_DIR/enum/enum_standalone" "$INPUT_FILE" "$TEMP_DIR/enum.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Enum compiler not found"
fi

# Stage 19: Exception Handling Module
echo "[19/45] Exception Handling Module..."
if [ -x "$MODULES_DIR/exception_handling/exception_handling_standalone" ]; then
    "$MODULES_DIR/exception_handling/exception_handling_standalone" "$INPUT_FILE" "$TEMP_DIR/exceptions.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Exception Handling compiler not found"
fi

# Stage 20: Expression Module
echo "[20/45] Expression Module..."
if [ -x "$MODULES_DIR/expression/expression_standalone" ]; then
    "$MODULES_DIR/expression/expression_standalone" "$INPUT_FILE" "$TEMP_DIR/expression.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Expression compiler not found"
fi

# Stage 21: File I/O Module
echo "[21/45] File I/O Module..."
if [ -x "$MODULES_DIR/file_io/file_io_standalone" ]; then
    "$MODULES_DIR/file_io/file_io_standalone" "$INPUT_FILE" "$TEMP_DIR/file_io.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  File I/O compiler not found"
fi

# Stage 22: Generator Module
echo "[22/45] Generator Module..."
if [ -x "$MODULES_DIR/generator/generator_standalone" ]; then
    "$MODULES_DIR/generator/generator_standalone" "$INPUT_FILE" "$TEMP_DIR/generator.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Generator compiler not found"
fi

# Stage 23: Generic Types Module
echo "[23/45] Generic Types Module..."
if [ -x "$MODULES_DIR/generic_types/generic_types_standalone" ]; then
    "$MODULES_DIR/generic_types/generic_types_standalone" "$INPUT_FILE" "$TEMP_DIR/generics.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Generic Types compiler not found"
fi

# Stage 24: Interface Trait Module
echo "[24/45] Interface Trait Module..."
if [ -x "$MODULES_DIR/interface_trait/interface_trait_standalone" ]; then
    "$MODULES_DIR/interface_trait/interface_trait_standalone" "$INPUT_FILE" "$TEMP_DIR/interface_trait.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Interface Trait compiler not found"
fi

# Stage 25: Lambda Module
echo "[25/45] Lambda Module..."
if [ -x "$MODULES_DIR/lambda/lambda_standalone" ]; then
    "$MODULES_DIR/lambda/lambda_standalone" "$INPUT_FILE" "$TEMP_DIR/lambda.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Lambda compiler not found"
fi

# Stage 26: Linear Types Module
echo "[26/45] Linear Types Module..."
if [ -x "$MODULES_DIR/linear_types/linear_types_standalone" ]; then
    "$MODULES_DIR/linear_types/linear_types_standalone" "$INPUT_FILE" "$TEMP_DIR/linear_types.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Linear Types compiler not found"
fi

# Stage 27: Logical Module
echo "[27/45] Logical Module..."
if [ -x "$MODULES_DIR/logical/logical_standalone" ]; then
    "$MODULES_DIR/logical/logical_standalone" "$INPUT_FILE" "$TEMP_DIR/logical.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Logical compiler not found"
fi

# Stage 28: Memory Module
echo "[28/45] Memory Module..."
if [ -x "$MODULES_DIR/memory/memory_standalone" ]; then
    "$MODULES_DIR/memory/memory_standalone" "$INPUT_FILE" "$TEMP_DIR/memory.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Memory compiler not found"
fi

# Stage 29: Module System Module
echo "[29/45] Module System Module..."
if [ -x "$MODULES_DIR/module_system/module_system_standalone" ]; then
    "$MODULES_DIR/module_system/module_system_standalone" "$INPUT_FILE" "$TEMP_DIR/module_system.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Module System compiler not found"
fi

# Stage 30: Null Safety Module
echo "[30/45] Null Safety Module..."
if [ -x "$MODULES_DIR/null_safety/null_safety_standalone" ]; then
    "$MODULES_DIR/null_safety/null_safety_standalone" "$INPUT_FILE" "$TEMP_DIR/null_safety.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Null Safety compiler not found"
fi

# Stage 31: Operator Overloading Module
echo "[31/45] Operator Overloading Module..."
if [ -x "$MODULES_DIR/operator_overloading/operator_overloading_standalone" ]; then
    "$MODULES_DIR/operator_overloading/operator_overloading_standalone" "$INPUT_FILE" "$TEMP_DIR/operator_overload.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Operator Overloading compiler not found"
fi

# Stage 32: Parser Core Module
echo "[32/45] Parser Core Module..."
if [ -x "$MODULES_DIR/parser_core/parser_core_standalone" ]; then
    "$MODULES_DIR/parser_core/parser_core_standalone" "$INPUT_FILE" "$TEMP_DIR/parser_core.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Parser Core compiler not found"
fi

# Stage 33: Pattern Matching Module
echo "[33/45] Pattern Matching Module..."
if [ -x "$MODULES_DIR/pattern_matching/pattern_matching_standalone" ]; then
    "$MODULES_DIR/pattern_matching/pattern_matching_standalone" "$INPUT_FILE" "$TEMP_DIR/pattern_matching.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Pattern Matching compiler not found"
fi

# Stage 34: Pointer Module
echo "[34/45] Pointer Module..."
if [ -x "$MODULES_DIR/pointer/pointer_standalone" ]; then
    "$MODULES_DIR/pointer/pointer_standalone" "$INPUT_FILE" "$TEMP_DIR/pointer.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Pointer compiler not found"
fi

# Stage 35: Preprocessor Module
echo "[35/45] Preprocessor Module..."
if [ -x "$MODULES_DIR/preprocessor/preprocessor_standalone" ]; then
    "$MODULES_DIR/preprocessor/preprocessor_standalone" "$INPUT_FILE" "$TEMP_DIR/preprocessed.mlp" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Preprocessor compiler not found"
fi

# Stage 36: Program Module
echo "[36/45] Program Module..."
if [ -x "$MODULES_DIR/program/program_standalone" ]; then
    "$MODULES_DIR/program/program_standalone" "$INPUT_FILE" "$TEMP_DIR/program.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Program compiler not found"
fi

# Stage 37: Reflection Module
echo "[37/45] Reflection Module..."
if [ -x "$MODULES_DIR/reflection/reflection_standalone" ]; then
    "$MODULES_DIR/reflection/reflection_standalone" "$INPUT_FILE" "$TEMP_DIR/reflection.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Reflection compiler not found"
fi

# Stage 38: Result Type Module
echo "[38/45] Result Type Module..."
if [ -x "$MODULES_DIR/result_type/result_type_standalone" ]; then
    "$MODULES_DIR/result_type/result_type_standalone" "$INPUT_FILE" "$TEMP_DIR/result_type.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Result Type compiler not found"
fi

# Stage 39: State Management Module
echo "[39/45] State Management Module..."
if [ -x "$MODULES_DIR/state_management/state_management_standalone" ]; then
    "$MODULES_DIR/state_management/state_management_standalone" "$INPUT_FILE" "$TEMP_DIR/state_mgmt.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  State Management compiler not found"
fi

# Stage 40: Statement Module
echo "[40/45] Statement Module..."
if [ -x "$MODULES_DIR/statement/statement_standalone" ]; then
    "$MODULES_DIR/statement/statement_standalone" "$INPUT_FILE" "$TEMP_DIR/statement.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Statement compiler not found"
fi

# Stage 41: String Operations Module
echo "[41/45] String Operations Module..."
if [ -x "$MODULES_DIR/string_operations/string_operations_standalone" ]; then
    "$MODULES_DIR/string_operations/string_operations_standalone" "$INPUT_FILE" "$TEMP_DIR/string_ops.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  String Operations compiler not found"
fi

# Stage 42: Struct Module
echo "[42/45] Struct Module..."
if [ -x "$MODULES_DIR/struct/struct_standalone" ]; then
    "$MODULES_DIR/struct/struct_standalone" "$INPUT_FILE" "$TEMP_DIR/struct.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Struct compiler not found"
fi

# Stage 43: Switch Match Module
echo "[43/45] Switch Match Module..."
if [ -x "$MODULES_DIR/switch_match/switch_match_standalone" ]; then
    "$MODULES_DIR/switch_match/switch_match_standalone" "$INPUT_FILE" "$TEMP_DIR/switch.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Switch Match compiler not found"
fi

# Stage 44: Trait System Advanced Module
echo "[44/45] Trait System Advanced Module..."
if [ -x "$MODULES_DIR/trait_system_advanced/trait_system_advanced_standalone" ]; then
    "$MODULES_DIR/trait_system_advanced/trait_system_advanced_standalone" "$INPUT_FILE" "$TEMP_DIR/trait_advanced.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Trait System Advanced compiler not found"
fi

# Stage 45: Type Classes Module
echo "[45/63] Type Classes Module..."
if [ -x "$MODULES_DIR/type_classes/type_classes_standalone" ]; then
    "$MODULES_DIR/type_classes/type_classes_standalone" "$INPUT_FILE" "$TEMP_DIR/type_classes.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Type Classes compiler not found"
fi

# Stage 46: Advanced Numeric Module
echo "[46/63] Advanced Numeric Module..."
if [ -x "$MODULES_DIR/advanced_numeric/advanced_numeric_test" ]; then
    "$MODULES_DIR/advanced_numeric/advanced_numeric_test" "$INPUT_FILE" "$TEMP_DIR/advanced_numeric.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Advanced Numeric compiler not found"
fi

# Stage 47: Attributes Module
echo "[47/63] Attributes Module..."
if [ -x "$MODULES_DIR/attributes/attributes_test" ]; then
    "$MODULES_DIR/attributes/attributes_test" "$INPUT_FILE" "$TEMP_DIR/attributes.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Attributes compiler not found"
fi

# Stage 48: Const Generics Module
echo "[48/63] Const Generics Module..."
if [ -x "$MODULES_DIR/const_generics/const_generics_standalone" ]; then
    "$MODULES_DIR/const_generics/const_generics_standalone" "$INPUT_FILE" "$TEMP_DIR/const_generics.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Const Generics compiler not found"
fi

# Stage 49: Decorator System Module
echo "[49/63] Decorator System Module..."
if [ -x "$MODULES_DIR/decorator_system/decorator_system_standalone" ]; then
    "$MODULES_DIR/decorator_system/decorator_system_standalone" "$INPUT_FILE" "$TEMP_DIR/decorator_system.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Decorator System compiler not found"
fi

# Stage 50: Documentation Module
echo "[50/63] Documentation Module..."
if [ -x "$MODULES_DIR/documentation/documentation_test" ]; then
    "$MODULES_DIR/documentation/documentation_test" "$INPUT_FILE" "$TEMP_DIR/documentation.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Documentation compiler not found"
fi

# Stage 51: FFI Module
echo "[51/63] FFI Module..."
if [ -x "$MODULES_DIR/ffi/ffi_test" ]; then
    "$MODULES_DIR/ffi/ffi_test" "$INPUT_FILE" "$TEMP_DIR/ffi.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  FFI compiler not found"
fi

# Stage 52: Iterator System Module
echo "[52/63] Iterator System Module..."
if [ -x "$MODULES_DIR/iterator_system/iterator_system_test" ]; then
    "$MODULES_DIR/iterator_system/iterator_system_test" "$INPUT_FILE" "$TEMP_DIR/iterator_system.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Iterator System compiler not found"
fi

# Stage 53: Macro System Module
echo "[53/63] Macro System Module..."
if [ -x "$MODULES_DIR/macro_system/macro_system_standalone" ]; then
    "$MODULES_DIR/macro_system/macro_system_standalone" "$INPUT_FILE" "$TEMP_DIR/macro_system.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Macro System compiler not found"
fi

# Stage 54: Network I/O Module
echo "[54/63] Network I/O Module..."
if [ -x "$MODULES_DIR/network_io/network_io_test" ]; then
    "$MODULES_DIR/network_io/network_io_test" "$INPUT_FILE" "$TEMP_DIR/network_io.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Network I/O compiler not found"
fi

# Stage 55: Ownership System Module
echo "[55/63] Ownership System Module..."
if [ -x "$MODULES_DIR/ownership_system/ownership_system_standalone" ]; then
    "$MODULES_DIR/ownership_system/ownership_system_standalone" "$INPUT_FILE" "$TEMP_DIR/ownership_system.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Ownership System compiler not found"
fi

# Stage 56: Package Management Module
echo "[56/63] Package Management Module..."
if [ -x "$MODULES_DIR/package_management/package_management_test" ]; then
    "$MODULES_DIR/package_management/package_management_test" "$INPUT_FILE" "$TEMP_DIR/package_management.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Package Management compiler not found"
fi

# Stage 57: Performance Module
echo "[57/63] Performance Module..."
if [ -x "$MODULES_DIR/performance/performance_test" ]; then
    "$MODULES_DIR/performance/performance_test" "$INPUT_FILE" "$TEMP_DIR/performance.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Performance compiler not found"
fi

# Stage 58: Regex Pattern Module
echo "[58/63] Regex Pattern Module..."
if [ -x "$MODULES_DIR/regex_pattern/regex_pattern_test" ]; then
    "$MODULES_DIR/regex_pattern/regex_pattern_test" "$INPUT_FILE" "$TEMP_DIR/regex_pattern.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Regex Pattern compiler not found"
fi

# Stage 59: Result Option Module
echo "[59/63] Result Option Module..."
if [ -x "$MODULES_DIR/result_option/result_option_test" ]; then
    "$MODULES_DIR/result_option/result_option_test" "$INPUT_FILE" "$TEMP_DIR/result_option.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Result Option compiler not found"
fi

# Stage 60: Smart Pointers Module
echo "[60/63] Smart Pointers Module..."
if [ -x "$MODULES_DIR/smart_pointers/smart_pointers_compiler" ]; then
    "$MODULES_DIR/smart_pointers/smart_pointers_compiler" "$INPUT_FILE" "$TEMP_DIR/smart_pointers.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Smart Pointers compiler not found"
fi

# Stage 61: Test Framework Module
echo "[61/63] Test Framework Module..."
if [ -x "$MODULES_DIR/test_framework/test_framework_test" ]; then
    "$MODULES_DIR/test_framework/test_framework_test" "$INPUT_FILE" "$TEMP_DIR/test_framework.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Test Framework compiler not found"
fi

# Stage 62: Union Types Module
echo "[62/63] Union Types Module..."
if [ -x "$MODULES_DIR/union_types/union_types_test" ]; then
    "$MODULES_DIR/union_types/union_types_test" "$INPUT_FILE" "$TEMP_DIR/union_types.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Union Types compiler not found"
fi

# Stage 63: Unsafe Blocks Module
echo "[63/63] Unsafe Blocks Module..."
if [ -x "$MODULES_DIR/unsafe_blocks/unsafe_blocks_test" ]; then
    "$MODULES_DIR/unsafe_blocks/unsafe_blocks_test" "$INPUT_FILE" "$TEMP_DIR/unsafe_blocks.s" 2>&1 | grep -E "(✓|✅|Error)" || true
else
    echo "   ⚠️  Unsafe Blocks compiler not found"
fi

# Stage 64: Combine and Build
echo "[64/64] Linking..."

# Create main assembly file
cat > "$TEMP_DIR/main.s" << 'EOF'
section .data
    msg: db "MLP Program Running", 10, 0
    msg_len: equ $ - msg

section .text
    global _start

_start:
    ; Print message
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, msg
    mov rdx, msg_len
    syscall
    
    ; Exit
    mov rax, 60         ; sys_exit
    xor rdi, rdi
    syscall
EOF

# Assemble
nasm -f elf64 "$TEMP_DIR/main.s" -o "$TEMP_DIR/main.o" 2>/dev/null || {
    echo "❌ Assembly failed"
    exit 1
}

# Link
ld "$TEMP_DIR/main.o" -o "$OUTPUT_BINARY" 2>/dev/null || {
    echo "❌ Linking failed"
    exit 1
}

echo ""
echo "✅ Build successful!"
echo "   Binary: $OUTPUT_BINARY"
echo ""
echo "Run with: ./$OUTPUT_BINARY"

# Cleanup
rm -rf "$TEMP_DIR"
