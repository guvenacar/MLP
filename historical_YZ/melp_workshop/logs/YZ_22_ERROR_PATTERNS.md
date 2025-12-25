# YZ_22: Error Pattern Analysis

**Date:** Prş 18 Ara 2025 22:08:41 +03
**Total Modules Tested:** 42
**Success Rate:** 17/42 (40%)
**Failure Rate:** 25/42 (60%)

## 🔍 Top Error Patterns (Grouped)

### 1️⃣ Parser: Expected 'function' keyword (90 occurrences)

/home/pardus/projeler/MLP/MLP/logs/stage1_errors/arrays_codegen.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/arrays/arrays_codegen.mlp:169: error [Parser]: Expected 'function' keyword
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/control_flow_codegen.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/operators/operators_parser.mlp:306: error [Parser]: Expected 'function' keyword
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/control_flow_parser.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/operators/operators_parser.mlp:306: error [Parser]: Expected 'function' keyword
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/enums_codegen.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/enums/enums_parser.mlp:51:1: error [Parser]: Expected 'function' keyword, got 'pos'
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/enums_parser.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/enums/enums_parser.mlp:51:1: error [Parser]: Expected 'function' keyword, got 'pos'
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/enums_parser.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/enums/enums_parser.mlp:126:1: error [Parser]: Expected 'function' keyword, got 'pos'
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/enums_parser.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/enums/enums_parser.mlp:162:1: error [Parser]: Expected 'function' keyword, got 'pos'
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/enums_parser.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/enums/enums_parser.mlp:207: error [Parser]: Expected 'function' keyword
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/enums_parser.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/enums/enums_parser.mlp:215: error [Parser]: Expected 'function' keyword
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/enums_parser.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/enums/enums_parser.mlp:223: error [Parser]: Expected 'function' keyword

### 2️⃣ Print Statement: Expected '(' after 'print' (22 occurrences)


### 3️⃣ Array Access: Expected ')' after list index (20 occurrences)

/home/pardus/projeler/MLP/MLP/logs/stage1_errors/arrays_codegen.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/arrays/arrays_codegen.mlp:169: error [Parser]: Expected ')' after list index
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/test_arrays.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/arrays/test_arrays.mlp:53: error [Parser]: Expected ')' after list index
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/test_arrays.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/arrays/test_arrays.mlp:293: error [Parser]: Expected ')' after list index
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/test_functions.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/functions/functions_codegen.mlp:212: error [Parser]: Expected ')' after list index
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/test_literals.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/literals/test_literals.mlp:347: error [Parser]: Expected ')' after list index

### 4️⃣ If Statement: Expected 'then' after if condition (17 occurrences)


### 5️⃣ Import System: Module not found (multiple)

/home/pardus/projeler/MLP/MLP/logs/stage1_errors/test_core.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/core/test_core.mlp:7: error [Parser]: Module 'core/token_types' not found in search paths
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/test_core.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/core/test_core.mlp:8: error [Parser]: Module 'core/char_utils' not found in search paths
/home/pardus/projeler/MLP/MLP/logs/stage1_errors/test_core.log:/home/pardus/projeler/MLP/MLP/compiler/stage1/modules/core/test_core.mlp:9: error [Parser]: Module 'core/type_mapper' not found in search paths

### 6️⃣ Arithmetic: Unexpected token in arithmetic expression (7 occurrences)


## 📊 Failed Modules Breakdown

FAIL: arrays/arrays_codegen.mlp
FAIL: arrays/arrays_parser.mlp
FAIL: arrays/test_arrays.mlp
FAIL: control_flow/control_flow_codegen.mlp
FAIL: control_flow/control_flow_parser.mlp
FAIL: control_flow/test_control_flow.mlp
FAIL: core/test_core.mlp
FAIL: enums/enums_codegen.mlp
FAIL: enums/enums_parser.mlp
FAIL: enums/test_enums.mlp
FAIL: functions/functions_codegen.mlp
FAIL: functions/test_functions.mlp
FAIL: literals/literals_codegen.mlp
FAIL: literals/literals_parser.mlp
FAIL: literals/test_literals.mlp
FAIL: operators/operators_codegen.mlp
FAIL: operators/operators_parser.mlp
FAIL: operators/test_operators.mlp
FAIL: structs/structs_codegen.mlp
FAIL: structs/structs_parser.mlp
FAIL: structs/test_structs.mlp
FAIL: variables/test_variables.mlp
FAIL: variables/variables_codegen.mlp

## ✅ Passing Modules

PASS: advanced/string_utils.mlp
PASS: bootstrap_driver.mlp
PASS: bootstrap_minimal.mlp
PASS: bootstrap_test.mlp
PASS: compiler_full.mlp
PASS: compiler_integration.mlp
PASS: compiler.mlp
PASS: core/char_utils.mlp
PASS: core/duplicate_a.mlp
PASS: core/duplicate_b.mlp
PASS: core/math_utils.mlp
PASS: core/test_module.mlp
PASS: core/token_types.mlp
PASS: core/type_mapper.mlp
PASS: functions/functions_parser.mlp
PASS: test/combined_test.mlp
PASS: test/main_multi.mlp
PASS: test/math_utils.mlp
PASS: variables/variables_parser.mlp

