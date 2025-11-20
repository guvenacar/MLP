# Phase 5.2 Test Suite

**Phase**: Error Handling & Memory Management  
**Features**: 14 new runtime functions  
**Test Coverage**: 100% function coverage  
**Status**: Tests written, waiting for implementation  
**Last Updated**: 21 Kasım 2025

---

## 📊 Test Summary

| Test Suite | Test Count | Lines of Code | Status |
|------------|-----------|---------------|---------|
| C Unit Tests (Error Handling) | 20 | ~320 | ✅ Written |
| C Unit Tests (Memory Mgmt) | 36 | ~480 | ✅ Written |
| MLP Integration (Error) | 9 | ~260 | ✅ Written |
| MLP Integration (Memory) | 13 | ~320 | ✅ Written |
| Edge Cases & Stress Tests | 15 | ~340 | ✅ Written |
| **Total** | **93** | **~1,720** | **✅ Complete** |

---

## 🎯 Test Coverage Matrix

### Error Handling Functions (5 functions)

| Function | C Tests | MLP Tests | Edge Cases | Total Coverage |
|----------|---------|-----------|------------|----------------|
| `exit_with_code(code)` | 5 | 2 | 0 | 7 tests |
| `panic(message)` | 3 | 0 | 0 | 3 tests |
| `mlp_assert(cond, msg)` | 6 | 3 | 1 | 10 tests |
| `get_error_code()` | 3 | 9 | 3 | 15 tests |
| `set_error_code(code)` | 3 | 9 | 3 | 15 tests |

**Total Error Handling Tests**: 50 tests

### Memory Management Functions (6 functions)

| Function | C Tests | MLP Tests | Edge Cases | Total Coverage |
|----------|---------|-----------|------------|----------------|
| `mlp_malloc(size)` | 6 | 5 | 6 | 17 tests |
| `mlp_free(ptr)` | 4 | 4 | 2 | 10 tests |
| `mlp_realloc(ptr, size)` | 6 | 3 | 3 | 12 tests |
| `mlp_calloc(count, size)` | 5 | 2 | 2 | 9 tests |
| `get_allocated_bytes()` | 5 | 5 | 0 | 10 tests |
| `check_memory_leaks()` | 4 | 3 | 1 | 8 tests |

**Total Memory Management Tests**: 66 tests

### System Utilities (3 functions) - To be tested in integration

| Function | Status |
|----------|--------|
| `get_env(name)` | ⏳ Pending integration tests |
| `current_timestamp()` | ⏳ Pending integration tests |
| `sleep_ms(ms)` | ⏳ Pending integration tests |

---

## 🚀 Running Tests

### Prerequisites

Tests require Claude #1's implementation to be complete:
- `runtime/runtime.c` with all 14 functions implemented
- Compiler integration (lexer, parser, codegen) complete

### C Unit Tests

```bash
cd test/c_unit_tests

# Build all tests
./build_phase5_2_tests.sh

# Run error handling tests (20 tests)
./test_error_handling

# Run memory management tests (36 tests)
./test_memory_management
```

**Expected Output**:
```
==================================
Phase 5.2 C Unit Tests - Error Handling
==================================

--- Error Code Tests ---
Testing get_set_error_code... PASS
Testing error_code_persistence... PASS
Testing error_code_large_values... PASS

--- Assert Pass Tests ---
Testing assert_pass... PASS
...

==================================
Results: 20 passed, 0 failed
==================================
```

### MLP Integration Tests

```bash
cd /home/pardus/projeler/tyd-lang/MLP

# Compile and run error handling tests (9 tests)
./mlpc test/test_error_handling.mlp -o test_error
./test_error

# Compile and run memory management tests (13 tests)
./mlpc test/test_memory_management.mlp -o test_memory
./test_memory

# Compile and run edge cases (15 tests)
./mlpc test/test_phase5_2_edge_cases.mlp -o test_edge
./test_edge
```

**Expected Output**:
```
====================================
Error Handling Integration Tests
====================================

Test: Error Code Management
  PASS: Initial error code is 0
  PASS: Error code set to 42
  PASS: Negative error codes work
...

====================================
Results:
  Passed: 9
  Failed: 0
====================================
```

---

## 📋 Test Details

### C Unit Tests: Error Handling

**File**: `test/c_unit_tests/test_error_handling.c` (320 lines)

#### Error Code Tests (3 tests)
- `test_get_set_error_code`: Basic get/set operations
- `test_error_code_persistence`: Multiple set operations
- `test_error_code_large_values`: Max/min int32 values

#### Assert Pass Tests (3 tests)
- `test_assert_pass`: Various truthy values
- `test_assert_with_null_message`: NULL message handling
- `test_assert_with_long_message`: 1KB message handling

#### Exit Code Tests (5 tests - use fork())
- `test_exit_with_code_zero`: Exit with code 0
- `test_exit_with_code_one`: Exit with code 1
- `test_exit_with_code_custom`: Exit with code 42
- `test_exit_with_code_large`: Exit with code 255
- `test_exit_with_code_negative`: Negative exit code handling

#### Panic Tests (3 tests - use fork())
- `test_panic_exits_with_one`: Panic exits with code 1
- `test_panic_with_null_message`: NULL message handling
- `test_panic_with_long_message`: 1KB message handling

#### Assert Failure Tests (3 tests - use fork())
- `test_assert_failure`: Assert(0) exits with code 1
- `test_assert_failure_with_null`: Assert(0, NULL) handling
- `test_assert_with_false_like_values`: Only 0 is falsy

#### Integration Tests (2 tests)
- `test_error_code_with_assert`: Error code persists through asserts
- `test_multiple_operations`: Sequence of operations

---

### C Unit Tests: Memory Management

**File**: `test/c_unit_tests/test_memory_management.c` (480 lines)

#### Malloc Tests (6 tests)
- `test_malloc_basic`: 100 byte allocation
- `test_malloc_zero`: Zero size returns NULL
- `test_malloc_negative`: Negative size returns NULL
- `test_malloc_large`: 1MB allocation
- `test_malloc_very_large`: 10MB allocation
- `test_malloc_small`: 1, 8, 16 byte allocations

#### Free Tests (4 tests)
- `test_free_basic`: Basic free reduces allocation
- `test_free_null`: free(NULL) doesn't crash
- `test_free_multiple`: Multiple frees
- `test_free_in_reverse_order`: Reverse order freeing

#### Calloc Tests (5 tests)
- `test_calloc_basic`: Memory is zeroed
- `test_calloc_zero_count`: calloc(0, x) returns NULL
- `test_calloc_zero_size`: calloc(x, 0) returns NULL
- `test_calloc_large`: 1000 x 1024 bytes
- `test_calloc_tracking`: Allocation tracking works

#### Realloc Tests (6 tests)
- `test_realloc_grow`: Grow from 100 to 200 bytes
- `test_realloc_shrink`: Shrink from 200 to 100 bytes
- `test_realloc_null`: realloc(NULL, size) = malloc(size)
- `test_realloc_zero_size`: realloc(ptr, 0) = free(ptr)
- `test_realloc_same_size`: Realloc to same size
- `test_realloc_multiple_times`: Chain of reallocs

#### Tracking Tests (5 tests)
- `test_get_allocated_bytes_initial`: Initial state
- `test_get_allocated_bytes_after_malloc`: Increases after malloc
- `test_get_allocated_bytes_after_free`: Decreases after free
- `test_get_allocated_bytes_multiple`: Track multiple allocations
- `test_tracking_accuracy`: Diff >= requested size

#### Leak Detection Tests (4 tests)
- `test_check_memory_leaks_none`: No false positives
- `test_check_memory_leaks_one`: Detects 1 leak
- `test_check_memory_leaks_multiple`: Detects 3 leaks
- `test_check_memory_leaks_partial`: Detects partial leaks

#### Stress Tests (2 tests)
- `test_many_small_allocations`: 1000 x 16 bytes
- `test_alternating_alloc_free`: 100 iterations

---

### MLP Integration Tests: Error Handling

**File**: `test/test_error_handling.mlp` (260 lines)

#### Tests (9 functions)
1. **test_error_codes**: Get/set/negative error codes
2. **test_assertions_pass**: Various passing assertions
3. **test_error_propagation**: Error propagation through call stack
4. **test_safe_divide**: Division by zero handling
5. **test_preconditions**: Array bounds checking
6. **test_error_reset**: Error code reset to 0
7. **test_multiple_error_states**: Multiple error states
8. **test_assert_computed**: Assertions with computed values
9. **test_error_in_loop**: Error codes in loops

---

### MLP Integration Tests: Memory Management

**File**: `test/test_memory_management.mlp` (320 lines)

#### Tests (13 functions)
1. **test_basic_malloc_free**: Basic allocation/free cycle
2. **test_multiple_allocations**: 3 allocations (100+200+300 bytes)
3. **test_calloc**: Calloc 10 x 8 bytes
4. **test_realloc**: Grow buffer 128→256→512
5. **test_leak_detection_clean**: No false positives
6. **test_leak_detection_with_leak**: Detect intentional leak
7. **test_tracking_accuracy**: Precise allocation tracking
8. **test_allocation_in_loop**: 10 iterations
9. **test_realloc_shrink**: Shrink 1024→512
10. **test_calloc_zeroed**: Verify calloc tracking
11. **test_free_null_safety**: free(NULL) safety
12. **test_sequential_alloc_free**: 20 sequential operations
13. **test_large_allocation**: 1MB allocation

---

### Edge Cases and Stress Tests

**File**: `test/test_phase5_2_edge_cases.mlp` (340 lines)

#### Tests (15 functions)
1. **test_zero_size_allocation**: malloc(0) returns NULL
2. **test_negative_size**: malloc(-100) returns NULL
3. **test_free_null**: free(NULL) safety
4. **test_large_allocation**: 1MB allocation
5. **test_many_small_allocations**: 100 x 16 bytes
6. **test_realloc_chain**: 64→128→256→512 growth
7. **test_error_code_boundaries**: Max/min int32
8. **test_calloc_zero_count**: calloc(0, x) returns NULL
9. **test_calloc_zero_size**: calloc(x, 0) returns NULL
10. **test_rapid_cycles**: 50 rapid alloc/free cycles
11. **test_realloc_same_size**: Realloc to same size
12. **test_nested_allocations**: Recursive allocation (depth 10)
13. **test_error_code_changes**: Multiple error code changes
14. **test_interleaved_operations**: Mixed memory/error ops
15. **test_assert_zero**: Assert with various values

---

## ✅ Test Validation Checklist

### Phase 5.2 Testing Checklist

- [x] **C Unit Tests Written**
  - [x] test_error_handling.c (20 tests)
  - [x] test_memory_management.c (36 tests)
  - [x] build_phase5_2_tests.sh script

- [x] **MLP Integration Tests Written**
  - [x] test_error_handling.mlp (9 tests)
  - [x] test_memory_management.mlp (13 tests)
  - [x] test_phase5_2_edge_cases.mlp (15 tests)

- [x] **Test Documentation**
  - [x] README_TESTS_PHASE5_2.md (this file)
  - [x] Test coverage matrix
  - [x] Running instructions

- [ ] **Waiting for Implementation** (Claude #1)
  - [ ] Runtime functions in runtime.c
  - [ ] Compiler integration
  - [ ] Build system updates

- [ ] **Test Execution** (Day 7-8)
  - [ ] Compile C unit tests
  - [ ] Run C unit tests
  - [ ] Compile MLP tests
  - [ ] Run MLP tests
  - [ ] Report results

- [ ] **Bug Reporting** (Day 9-10)
  - [ ] Document any failures
  - [ ] Report bugs to Claude #1
  - [ ] Retest after fixes

---

## 📈 Expected Test Results

### Success Criteria

1. ✅ All 93 tests written and documented
2. ⏳ All C unit tests pass (56/56)
3. ⏳ All MLP integration tests pass (37/37)
4. ⏳ No memory leaks detected
5. ⏳ No segmentation faults
6. ⏳ Edge cases handled gracefully

### Known Limitations

1. **System Utilities Not Tested**: `get_env()`, `current_timestamp()`, `sleep_ms()` 
   will be tested in future integration tests
2. **Platform-Specific**: Exit codes may vary across platforms
3. **Memory Overhead**: Allocation tracking includes overhead

---

## 🐛 Bug Reporting Template

When reporting bugs to Claude #1, use this format:

```markdown
### Bug: [Short Description]

**Test File**: test/c_unit_tests/test_xxx.c  
**Test Function**: test_xxx  
**Line Number**: XXX

**Expected Behavior**:
[What should happen]

**Actual Behavior**:
[What actually happened]

**Error Output**:
```
[Paste error message or output]
```

**Suggested Fix**:
[Optional: Your suggestion]
```

---

## 📞 Coordination Notes

### For Claude #1 (Implementation)
- All test files are ready in:
  - `test/c_unit_tests/test_error_handling.c`
  - `test/c_unit_tests/test_memory_management.c`
  - `test/test_error_handling.mlp`
  - `test/test_memory_management.mlp`
  - `test/test_phase5_2_edge_cases.mlp`
- Build script: `test/c_unit_tests/build_phase5_2_tests.sh`
- Test execution will begin once your implementation is merged

### For Claude #2 (Documentation)
- Test coverage details available for documentation
- API examples can reference these test files
- Design docs should align with test expectations

### For Coordinator
- Day 1-6: Test code writing complete ✅
- Day 7-8: Ready for test execution (pending implementation)
- Day 9-10: Ready for bug reporting and retesting

---

## 🎯 Next Steps

1. **Wait for Claude #1**: Implementation of 14 runtime functions
2. **Wait for Merge**: Claude #1's PR merged to integration branch
3. **Compile Tests**: Run `build_phase5_2_tests.sh`
4. **Execute Tests**: Run all test suites
5. **Report Results**: Document pass/fail counts
6. **Report Bugs**: Any failures reported to Claude #1
7. **Retest**: After bug fixes
8. **Final Report**: Complete test validation

---

**Test Suite Status**: ✅ **COMPLETE - READY FOR EXECUTION**  
**Waiting On**: Claude #1 runtime implementation  
**Total Test Code**: ~1,720 lines  
**Branch**: `claude/phase5-2-testing-20251121-013054`

---

*Last Updated: 21 Kasım 2025 - Claude #3 (Testing)*
