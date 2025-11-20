# MLP Phase 2 - Dynamic List Testing Suite

## Overview
Comprehensive test suite for MLP's Phase 2 Dynamic List implementation. Tests cover runtime functionality, edge cases, struct integration, and advanced usage patterns.

## Test Organization

### 1. Runtime Tests (`runtime/test_list.c`)
**Purpose:** Validate core C implementation of dynamic lists  
**Language:** C  
**Compile:** `gcc -o test_list test_list.c runtime.c json_parser.c -I.`  
**Run:** `./test_list`

#### Test Coverage:
| Test # | Name | Description | Status |
|--------|------|-------------|--------|
| 1 | Basic creation | Verify list initializes correctly | ✅ Pass |
| 2 | Add single | Add one element, verify size and value | ✅ Pass |
| 3 | Add multiple | Add 5 elements, verify all | ✅ Pass |
| 4 | Capacity expansion | Trigger 8→16 growth, verify integrity | ✅ Pass |
| 5 | Remove element | Remove middle element, verify shifting | ✅ Pass |
| 6 | Clear list | Clear all elements, test reusability | ✅ Pass |
| 7 | String list | Test pointer type (strings) | ✅ Pass |
| 8 | Empty operations | Operations on empty list | ✅ Pass |
| 9 | Large list | 1000 elements, capacity 8→1024 | ✅ Pass |
| 10 | Mixed operations | Complex add/remove sequence | ✅ Pass |

**Results:** ✅ 10/10 Passed | ❌ 0 Failed

---

### 2. Edge Case Tests (`test/test_list_edge_cases.mlp`)
**Purpose:** Test boundary conditions and error handling  
**Language:** MLP  
**Compile:** `cd c_compiler && ./compiler_test ../test/test_list_edge_cases.mlp output.asm`

#### Test Coverage:
| Test # | Description | Key Checks |
|--------|-------------|------------|
| 1 | Empty list size | `size()` returns 0 on new list |
| 2 | Clear on empty | `clear()` on empty doesn't crash |
| 3 | Add after clear | List reusable after clear |
| 4 | Single element | Add/get/remove single element |
| 5 | Boundary access | Index 0 and last index (size-1) |
| 6 | Clear and reuse | Multiple clear cycles |

**Test Count:** 6 edge case scenarios

---

### 3. Integration Tests (`test/test_list_with_struct.mlp`)
**Purpose:** Test list compatibility with custom struct types  
**Language:** MLP  
**Struct:** `Person { string name; int age; }`

#### Test Coverage:
| Test # | Description | Operations |
|--------|-------------|------------|
| 1 | Create struct list | `list[Person]` initialization |
| 2-4 | Add 3 people | Alice(25), Bob(30), Charlie(35) |
| 5-7 | Retrieve people | `get()` and verify struct fields |
| 8 | Remove middle | Remove Bob, verify Alice + Charlie |
| 9 | Add after remove | Add David(40) |
| 10 | Clear struct list | `clear()` on struct list |

**Validation:** Struct field integrity maintained through all operations

---

### 4. Advanced Tests (`test/test_list_advanced.mlp`)
**Purpose:** Complex scenarios and real-world patterns  
**Language:** MLP

#### Test Coverage:
| Test # | Pattern | Description |
|--------|---------|-------------|
| 1 | Multiple lists | Parallel int and string lists |
| 2-3 | While iteration | Forward iteration with `while` loop |
| 4 | Linear search | Find element by value |
| 5 | Reduce/sum | Calculate sum using accumulator |
| 6 | Reverse iteration | Backward traversal pattern |
| 7 | Build with loop | Generate squares (1,4,9,16,25) |
| 8 | Conditional remove | Remove odd numbers (keep evens) |
| 9 | Parallel lists | Synchronized ID/user lists |
| 10 | Stress test | 50 elements, verify capacity growth |

**Real-world patterns:** Search, filter, reduce, parallel data

---

## Test Execution

### Run All Tests

```bash
# 1. Runtime tests
cd runtime
gcc -o test_list test_list.c runtime.c json_parser.c -I.
./test_list

# 2. MLP tests (requires compiler)
cd ../c_compiler

# Edge cases
./compiler_test ../test/test_list_edge_cases.mlp edge_cases.asm
nasm -f elf64 edge_cases.asm -o edge_cases.o
gcc -no-pie edge_cases.o ../runtime/runtime.o -o edge_cases
./edge_cases

# Struct integration
./compiler_test ../test/test_list_with_struct.mlp struct_test.asm
nasm -f elf64 struct_test.asm -o struct_test.o
gcc -no-pie struct_test.o ../runtime/runtime.o -o struct_test
./struct_test

# Advanced patterns
./compiler_test ../test/test_list_advanced.mlp advanced.asm
nasm -f elf64 advanced.asm -o advanced.o
gcc -no-pie advanced.o ../runtime/runtime.o -o advanced
./advanced
```

### Expected Output

#### Runtime Tests
```
═══════════════════════════════════════════════════════
  MLP Dynamic List - Comprehensive Test Suite
═══════════════════════════════════════════════════════
🧪 Test 1: Basic list creation
   ✅ Test passed
...
═══════════════════════════════════════════════════════
  Test Results
═══════════════════════════════════════════════════════
  ✅ Passed: 10
  ❌ Failed: 0
  📊 Total:  10
═══════════════════════════════════════════════════════
```

#### MLP Tests
Each test file outputs structured results:
```
=== MLP List Edge Case Tests ===
Test 1: Empty list operations
Empty list size: 0
✅ Empty list size is 0
...
=== Edge Case Tests Complete ===
```

---

## Coverage Matrix

### Feature Coverage

| Feature | Runtime Test | Edge Case | Integration | Advanced |
|---------|--------------|-----------|-------------|----------|
| `list_create()` | ✅ T1 | ✅ T1 | ✅ T1 | ✅ T1 |
| `list_add()` | ✅ T2,T3 | ✅ T3,T4 | ✅ T2-4 | ✅ T1,T7 |
| `list_get()` | ✅ T2,T3 | ✅ T3,T4,T5 | ✅ T5-7 | ✅ T2,T3 |
| `list_size()` | ✅ T1-T10 | ✅ All | ✅ All | ✅ All |
| `list_remove()` | ✅ T5,T10 | ✅ T4 | ✅ T8 | ✅ T8 |
| `list_clear()` | ✅ T6 | ✅ T2,T6 | ✅ T10 | - |
| Capacity growth | ✅ T4,T9 | - | - | ✅ T10 |
| Value types (int) | ✅ T1-T6,T9-T10 | ✅ All | - | ✅ All |
| Pointer types (string) | ✅ T7 | - | - | ✅ T1,T3 |
| Struct types | - | - | ✅ All | - |
| Empty list | ✅ T8 | ✅ T1,T2 | - | - |
| Large lists (1000+) | ✅ T9 | - | - | ✅ T10 (50) |
| While iteration | - | - | - | ✅ T2,T3,T6 |
| Search pattern | - | - | - | ✅ T4 |
| Reduce pattern | - | - | - | ✅ T5 |
| Filter pattern | - | - | - | ✅ T8 |

### Test Statistics

| Test Suite | Files | Test Cases | Lines of Code | Status |
|------------|-------|------------|---------------|--------|
| Runtime (C) | 1 | 10 | 400+ | ✅ All Pass |
| Edge Cases (MLP) | 1 | 6 | 180+ | ⏳ Pending Compile |
| Integration (MLP) | 1 | 10 | 220+ | ⏳ Pending Compile |
| Advanced (MLP) | 1 | 10 | 280+ | ⏳ Pending Compile |
| **Total** | **4** | **36** | **1080+** | **10/36 Verified** |

---

## Implementation Details

### List Structure (C)
```c
typedef struct {
    void** data;           // Array of element pointers
    int size;              // Current element count
    int capacity;          // Allocated capacity
    size_t element_size;   // Element size in bytes
    int is_pointer_type;   // 0=value, 1=pointer
} DynamicList;
```

### Capacity Growth
- Initial: 8 elements
- Growth: 2x on overflow (8→16→32→64→128→256→512→1024)
- Memory: Automatic realloc with data preservation

### Type Support
- **Value types** (int): Deep copy on add
- **Pointer types** (string): Store reference
- **Struct types**: Store reference (managed externally)

---

## Known Limitations

1. **Bounds checking:** `get()` and `remove()` print error but don't halt execution
2. **Memory leaks:** Strings/structs not freed on `clear()` (user responsibility)
3. **No resize down:** Capacity never shrinks after growth
4. **No insert:** Only add to end (remove from any index)

---

## Future Enhancements

- [ ] `list_insert(list, index, element)` - Insert at position
- [ ] `list_contains(list, element)` - Check existence
- [ ] `list_index_of(list, element)` - Find index
- [ ] `list_resize(list, capacity)` - Manual resize
- [ ] Generic comparison functions for search
- [ ] Iterator pattern support

---

## Compilation Requirements

### Dependencies
- GCC 7.0+ (C11 support)
- NASM 2.14+ (x86-64 assembly)
- MLP Compiler (c_compiler/mlp_compiler)

### Compiler Flags
```bash
# Runtime tests
gcc -o test_list test_list.c runtime.c json_parser.c -I. -std=c11

# MLP programs
nasm -f elf64 program.asm -o program.o
gcc -no-pie program.o runtime.o -o program
```

---

## Test Maintenance

### Adding New Tests

1. **Runtime (C):**
   - Add test function to `test_list.c`
   - Follow `TEST_START`, `ASSERT_*`, `TEST_END` pattern
   - Call from `main()`

2. **MLP:**
   - Create `.mlp` file in `test/` directory
   - Use `# lang: en-US` and `# syntax: mlp` headers
   - Print results with ✅/❌ prefixes
   - Update this README

### Debugging Failed Tests

1. Check stderr output for runtime errors
2. Verify list state with `list_size()` calls
3. Add print statements before assertions
4. Run under valgrind for memory issues:
   ```bash
   valgrind --leak-check=full ./test_list
   ```

---

## Test Results History

| Date | Runtime | Edge | Struct | Advanced | Notes |
|------|---------|------|--------|----------|-------|
| 2025-11-20 | 10/10 ✅ | - | - | - | Initial implementation |

---

## Contact

For test failures or coverage gaps, report to MLP development team.

**Test Suite Version:** 1.0  
**Last Updated:** 2025-11-20  
**Maintainer:** Claude #3 (Test Engineer)
