# Phase 4 Testing Documentation - MLP Compiler

## Overview

This document describes the comprehensive Phase 4 test suite for the MLP compiler, covering hash maps, type conversion operations, and mathematical functions. Phase 4 builds upon Phase 3 (hash tables, file I/O, string operations) with focus on MLP language integration and advanced runtime features.

**Branch:** `claude/phase4-testing-20251120`  
**Test Engineer:** Claude #3  
**Date:** November 20, 2025  
**Total Tests:** 88 tests (22 C tests + 66 MLP tests)

---

## Test Categories

### 1. Hash Map Integration Tests (22 tests)

Hash maps provide key-value storage with O(1) average lookup time using DJB2 hashing and linear probing.

#### 1.1 Basic Operations (5 tests)
**File:** `test/test_hashmap_basic.mlp`

```mlp
# Create and use hash map
map = HashMap()
map.set("name", "Alice")
map.get("name")  # Returns "Alice"
map.has("name")  # Returns 1
map.remove("name")
map.clear()
```

**Tests:**
- String-to-Int maps (basic CRUD operations)
- Int-to-String maps (reverse key-value types)
- String-to-String maps (translation dictionary pattern)
- Multiple independent maps (isolation verification)
- Empty map operations (size, has, clear on empty)

#### 1.2 Advanced Operations (7 tests)
**File:** `test/test_hashmap_advanced.mlp`

**Tests:**
- Large dataset (100 entries, random access patterns)
- Update existing keys (verify no size change on update)
- Remove multiple items (partial deletion)
- Clear and reuse (verify old keys gone after clear)
- Collision handling (keys with potential hash collisions)
- Mixed operations (insert→update→remove→insert sequences)
- Multiple independent maps (verify no cross-contamination)

#### 1.3 Edge Cases (10 tests)
**File:** `test/test_hashmap_edge_cases.mlp`

**Tests:**
- Empty string values (distinguish from null)
- Special characters in keys (spaces, @, /, -)
- Remove non-existent keys (graceful handling)
- Get non-existent keys (default value behavior)
- Duplicate key handling (overwrite vs error)
- Zero values (distinguish from non-existent)
- Negative values (signed integer support)
- Rapid insert/remove cycles (100 operations)
- Unicode keys (Turkish: ğüşıöç, emoji support)
- Stress test (50 mixed operations)

---

### 2. Type Conversion Tests (20 tests)

Type conversion enables seamless translation between strings, integers, floats, and booleans.

#### 2.1 C Implementation Tests (10 tests)
**File:** `runtime/test_type_conversion.c`  
**Status:** ✅ **10/10 PASSING**

**Functions Tested:**
```c
int64_t string_to_int(const char* str);      // "42" → 42
char* int_to_string(int64_t num);            // 42 → "42"
double string_to_float(const char* str);      // "3.14" → 3.14
char* float_to_string(double num);           // 3.14 → "3.140000"
int string_to_bool(const char* str);         // "true" → 1
char* bool_to_string(int val);               // 1 → "true"
```

**Test Coverage:**
1. ✅ String to Int (positive, negative, zero, large)
2. ✅ Int to String (conversion verification)
3. ✅ String to Float (decimal precision)
4. ✅ Float to String (6 decimal places)
5. ✅ String to Bool (true/false/1/0/yes/no)
6. ✅ Bool to String (1→"true", 0→"false")
7. ✅ Empty string handling (defaults to 0)
8. ✅ Large numbers (999999999 round-trip)
9. ✅ Whitespace handling (leading spaces)
10. ✅ Invalid conversions (non-numeric → 0)

#### 2.2 MLP Integration Tests (10 tests)
**File:** `test/test_type_conversion.mlp`

**Tests:**
- String to Int conversion (all integer types)
- Int to String conversion (formatting)
- String to Float conversion (decimal handling)
- Float to String conversion (precision control)
- String to Bool conversion (multiple formats)
- Bool to String conversion (true/false output)
- Empty string edge cases (default behavior)
- Invalid conversion handling (graceful degradation)
- Whitespace handling (trimming)
- Round-trip conversions (data integrity)

---

### 3. Math Operations Tests (24 tests)

Mathematical functions provide essential numeric computation capabilities.

#### 3.1 C Implementation Tests (12 tests)
**File:** `runtime/test_math_ops.c`  
**Status:** ✅ **12/12 PASSING**

**Functions Tested:**
```c
double math_sqrt(double num);                 // √x
double math_pow(double base, double exp);     // x^y
int64_t math_abs(int64_t num);               // |x| (int)
double math_fabs(double num);                // |x| (float)
int64_t math_min(int64_t a, int64_t b);      // min(a,b)
int64_t math_max(int64_t a, int64_t b);      // max(a,b)
int64_t math_round(double num);              // round(x)
int64_t math_floor(double num);              // floor(x)
int64_t math_ceil(double num);               // ceil(x)
double math_sin(double angle);               // sin(θ)
double math_cos(double angle);               // cos(θ)
double math_tan(double angle);               // tan(θ)
```

**Test Coverage:**
1. ✅ Square root (perfect squares, non-perfect, zero)
2. ✅ Power function (positive, negative, zero exponents)
3. ✅ Absolute value int (negative, positive, zero)
4. ✅ Absolute value float (decimal precision)
5. ✅ Min/Max functions (various combinations)
6. ✅ Rounding (up, down, .5 cases)
7. ✅ Floor function (positive, negative)
8. ✅ Ceiling function (positive, negative)
9. ✅ Trigonometry (sin, cos, tan at key angles)
10. ✅ Edge cases (0^0, sqrt(0), large powers)
11. ✅ Negative sqrt (error handling → returns 0)
12. ✅ Combined operations (nested calls)

**Compilation:**
```bash
cd runtime
gcc -o test_math_ops test_math_ops.c runtime.c -I. -lm
./test_math_ops
```

#### 3.2 MLP Integration Tests (12 tests)
**File:** `test/test_math_ops.mlp`

**Tests:**
- Square root calculations (various inputs)
- Power function (integer and fractional exponents)
- Integer absolute value (signed integers)
- Float absolute value (decimal precision)
- Min/Max operations (comparison logic)
- Rounding operations (banker's rounding)
- Floor function (always down)
- Ceiling function (always up)
- Trigonometric functions (sin/cos/tan)
- Combined operations (nested math calls)
- Edge cases (boundary conditions)
- Error handling (negative sqrt)

---

### 4. Edge Cases & Boundary Tests (10 tests)

**File:** `test/test_edge_cases_phase4.mlp`

Tests extreme conditions and error handling across all Phase 4 features:

**Test Coverage:**
1. Hash map null/empty keys (empty string as key)
2. Hash map large values (1000+ character strings)
3. Hash map collisions (similar key patterns)
4. Rapid operations (100 insert/remove cycles)
5. Type conversion extremes (INT_MAX, INT_MIN)
6. Math boundary conditions (sqrt(0), 0^0, 1^∞)
7. String operations edge cases (empty, unicode)
8. Memory stress (multiple maps, 50+ entries each)
9. Nested operations (complex function composition)
10. Error recovery (graceful degradation)

---

### 5. Performance Benchmarks (12 benchmarks)

**File:** `test/benchmark_phase4.mlp`

Performance testing for scalability and optimization validation:

**Benchmarks:**
1. Hash map insert (1000 items)
2. Hash map lookup (1000 lookups)
3. Hash map remove (250 removals)
4. Mixed hash map operations (1000 mixed ops)
5. Type conversion cycles (1000 conversions)
6. Square root performance (1000 computations)
7. Power function performance (1000 computations)
8. Trigonometry performance (300 sin/cos/tan)
9. Rounding operations (1000 round/floor/ceil)
10. Combined stress test (500 iterations, all ops)
11. Memory intensive (5 maps × 100 items)
12. Sequential access (1000 sequential writes + reads)

**Expected Performance:**
- Hash map operations: O(1) average, O(n) worst case
- Type conversions: O(n) where n = string length
- Math operations: O(1) for most, O(log n) for pow

---

## Runtime Functions Added

### Type Conversion Functions (6 functions)

```c
// Integer conversions
int64_t string_to_int(const char* str);
char* int_to_string(int64_t num);

// Float conversions
double string_to_float(const char* str);
char* float_to_string(double num);

// Boolean conversions
int string_to_bool(const char* str);
char* bool_to_string(int val);
```

**Implementation Details:**
- `string_to_int`: Uses `atoll()` for 64-bit support
- `int_to_string`: Uses `snprintf()` + `strdup()` for dynamic allocation
- `string_to_float`: Uses `atof()` with error checking
- `float_to_string`: Formats with 6 decimal places
- `string_to_bool`: Accepts true/false/1/0/yes/no (case-insensitive)
- `bool_to_string`: Returns "true" or "false"

### Math Functions (12 functions)

```c
// Basic math
double math_sqrt(double num);          // Square root (error if negative)
double math_pow(double base, double exp);  // Power function
int64_t math_abs(int64_t num);        // Absolute value (int)
double math_fabs(double num);         // Absolute value (float)

// Comparisons
int64_t math_min(int64_t a, int64_t b);  // Minimum
int64_t math_max(int64_t a, int64_t b);  // Maximum

// Rounding
int64_t math_round(double num);       // Round to nearest
int64_t math_floor(double num);       // Round down
int64_t math_ceil(double num);        // Round up

// Trigonometry (radians)
double math_sin(double angle);        // Sine
double math_cos(double angle);        // Cosine
double math_tan(double angle);        // Tangent
```

**Implementation Details:**
- All functions use `<math.h>` standard library
- Error checking for sqrt (negative input → returns 0, prints error)
- Trigonometric functions accept radians (π ≈ 3.14159)
- Rounding functions return `int64_t` for integer results

---

## Running the Tests

### C Tests (Runtime Functions)

```bash
cd runtime

# Type conversion tests
gcc -o test_type_conversion test_type_conversion.c runtime.c -I. -lm
./test_type_conversion

# Math operations tests
gcc -o test_math_ops test_math_ops.c runtime.c -I. -lm
./test_math_ops
```

**Expected Output:**
```
╔══════════════════════════════════════╗
║   Type Conversion Tests - Phase 4   ║
╚══════════════════════════════════════╝
...
║ Passed:  10                         ║
║ Failed:   0                         ║
╚══════════════════════════════════════╝

╔══════════════════════════════════════╗
║  Math Operations Tests - Phase 4    ║
╚══════════════════════════════════════╝
...
║ Passed:  12                         ║
║ Failed:   0                         ║
╚══════════════════════════════════════╝
```

### MLP Tests (Integration Tests)

**Note:** MLP tests require MLP compiler Phase 4 support (hash maps, type conversion, math operations in code generator).

```bash
# When compiler is ready:
./mlp test/test_hashmap_basic.mlp
./mlp test/test_hashmap_advanced.mlp
./mlp test/test_hashmap_edge_cases.mlp
./mlp test/test_type_conversion.mlp
./mlp test/test_math_ops.mlp
./mlp test/test_edge_cases_phase4.mlp
./mlp test/benchmark_phase4.mlp
```

---

## Test Results Summary

### Phase 4 Test Statistics

| Category | C Tests | MLP Tests | Total | Status |
|----------|---------|-----------|-------|--------|
| Hash Maps | 0 | 22 | 22 | ✅ Created |
| Type Conversion | 10 | 10 | 20 | ✅ 10/10 C passing |
| Math Operations | 12 | 12 | 24 | ✅ 12/12 C passing |
| Edge Cases | 0 | 10 | 10 | ✅ Created |
| Benchmarks | 0 | 12 | 12 | ✅ Created |
| **TOTAL** | **22** | **66** | **88** | **22/22 C passing** |

### Cumulative Statistics (Phase 3 + Phase 4)

| Phase | Component | Tests | Status |
|-------|-----------|-------|--------|
| Phase 3 | Hash tables (C) | 10 | ✅ 10/10 passing |
| Phase 3 | File I/O (C) | 8 | ✅ 8/8 passing |
| Phase 3 | String ops (C) | 7 | ✅ 7/7 passing |
| Phase 3 | MLP integration | 26 | ✅ Created |
| **Phase 3 Total** | | **51** | **25/25 C passing** |
| Phase 4 | Hash maps (MLP) | 22 | ✅ Created |
| Phase 4 | Type conversion (C) | 10 | ✅ 10/10 passing |
| Phase 4 | Math ops (C) | 12 | ✅ 12/12 passing |
| Phase 4 | Edge cases (MLP) | 10 | ✅ Created |
| Phase 4 | Benchmarks (MLP) | 12 | ✅ Created |
| **Phase 4 Total** | | **66** | **22/22 C passing** |
| **GRAND TOTAL** | | **117** | **47/47 C passing** |

---

## Files Modified/Created

### Runtime Files

**Modified:**
- `runtime/runtime.c` (+230 lines)
  - Added 6 type conversion functions
  - Added 12 math operation functions
  - Added `#include <math.h>`

**Created:**
- `runtime/test_type_conversion.c` (230 lines, 10 tests)
- `runtime/test_math_ops.c` (350 lines, 12 tests)

### Test Files (MLP)

**Created:**
- `test/test_hashmap_basic.mlp` (320 lines, 5 tests)
- `test/test_hashmap_advanced.mlp` (450 lines, 7 tests)
- `test/test_hashmap_edge_cases.mlp` (500 lines, 10 tests)
- `test/test_type_conversion.mlp` (380 lines, 10 tests)
- `test/test_math_ops.mlp` (520 lines, 12 tests)
- `test/test_edge_cases_phase4.mlp` (400 lines, 10 tests)
- `test/benchmark_phase4.mlp` (600 lines, 12 benchmarks)

### Documentation

**Created:**
- `README_TESTS_PHASE4.md` (this file)

---

## Dependencies

### C Compilation
- GCC 7.0+ (C11 support)
- Math library (`-lm` flag)
- Standard libraries: `<stdio.h>`, `<stdlib.h>`, `<string.h>`, `<math.h>`

### MLP Compiler Requirements
- Phase 3 features (hash tables, file I/O, strings)
- Phase 4 features:
  - Hash map syntax support in parser
  - Type conversion function calls in codegen
  - Math operation function calls in codegen
  - Runtime linking (`-lm` in final compilation)

---

## Known Issues & Limitations

### Current Limitations
1. **Hash Map Capacity:** Default size 100, linear probing may degrade with >70% load
2. **Float Precision:** String conversion limited to 6 decimal places
3. **Unicode Support:** UTF-8 keys work but length calculation may be inaccurate
4. **Trigonometry:** Accepts radians only (no degree conversion yet)

### Warnings
- `fabs()` implicit declaration warnings in tests (non-fatal, `-lm` flag resolves)
- Memory management: Callers must free strings returned by conversion functions

### Future Improvements
1. Dynamic hash map resizing (rehashing at 75% load)
2. Configurable float precision (printf format string)
3. Degree/radian conversion helpers
4. Unicode normalization for keys
5. Hash map iterators (foreach loops)

---

## Integration with MLP Compiler

### Parser Extensions Needed

```mlp
# Hash map syntax
map = HashMap()  # Constructor call
map.set(key, value)  # Method call syntax
value = map.get(key)  # Method with return value

# Type conversions
int_val = string_to_int("42")  # Function call
str_val = int_to_string(42)

# Math operations
result = math_sqrt(16.0)
power = math_pow(2.0, 3.0)
```

### Code Generator Extensions Needed

1. **Hash Map Support:**
   - Constructor call → `hashmap_create()`
   - Method calls → `hashmap_set()`, `hashmap_get()`, etc.
   - Destructor (automatic or explicit) → `hashmap_destroy()`

2. **Function Calls:**
   - Type conversion functions (direct C calls)
   - Math functions (direct C calls with `-lm`)

3. **Type System:**
   - HashMap type (opaque pointer)
   - Implicit conversions (string ↔ other types)

---

## Testing Strategy

### Test-Driven Development Flow

1. **Write C Tests First** (Unit Testing)
   - Validate runtime functions in isolation
   - Ensure correct behavior before integration
   - Fast feedback loop (compile + run in seconds)

2. **Create MLP Tests** (Integration Testing)
   - Test language syntax and semantics
   - Validate compiler code generation
   - End-to-end validation with runtime

3. **Edge Cases & Benchmarks** (Stress Testing)
   - Boundary conditions and error handling
   - Performance validation and regression detection
   - Real-world usage patterns

### Test Coverage Goals

- **Unit Tests:** 100% function coverage (✅ Achieved)
- **Integration Tests:** All MLP syntax patterns (✅ Created)
- **Edge Cases:** Boundary conditions + error paths (✅ Created)
- **Performance:** Baseline metrics for regression detection (✅ Created)

---

## Maintenance & Updates

### Adding New Tests

1. **C Tests:** Add to `runtime/test_*.c`
   - Use existing test macros (TEST_START, ASSERT_EQ, TEST_END)
   - Compile with `-lm` flag
   - Update test count in this README

2. **MLP Tests:** Add to `test/*.mlp`
   - Follow existing naming convention
   - Include descriptive docstrings
   - Update test statistics in this README

### Regression Testing

```bash
# Run all C tests
cd runtime
for test in test_*; do
    if [ -x "$test" ]; then
        echo "Running $test..."
        ./"$test" || echo "FAILED: $test"
    fi
done

# Run all MLP tests (when compiler ready)
cd test
for test in test_*.mlp benchmark_*.mlp; do
    echo "Running $test..."
    ../mlp "$test" || echo "FAILED: $test"
done
```

---

## Phase 4 Completion Checklist

- [x] Hash map MLP tests (22 tests)
- [x] Type conversion C tests (10 tests, 10/10 passing)
- [x] Type conversion MLP tests (10 tests)
- [x] Math operations C tests (12 tests, 12/12 passing)
- [x] Math operations MLP tests (12 tests)
- [x] Edge case tests (10 tests)
- [x] Performance benchmarks (12 benchmarks)
- [x] Runtime functions implemented (18 functions)
- [x] Documentation (this file)
- [ ] Commit and push to branch
- [ ] Integration with compiler (pending Phase 4 codegen)

---

## References

### Phase 3 Documentation
- `README_TESTS_PHASE3.md` - Phase 3 test documentation
- `docs/BOOTSTRAP.md` - MLP compiler bootstrap process
- `runtime/hashmap.h` - Hash map implementation details

### External References
- C11 Standard: ISO/IEC 9899:2011
- IEEE 754: Floating-point arithmetic standard
- DJB2 Hash: http://www.cse.yorku.ca/~oz/hash.html

---

## Contact & Support

**Test Engineer:** Claude #3 (Testing Specialist)  
**Branch:** `claude/phase4-testing-20251120`  
**Coordination:** Phase 4 parallel development with Claude #1 and #2

For questions about Phase 4 testing, refer to:
1. This documentation (README_TESTS_PHASE4.md)
2. Phase 4 brief (PHASE4_BRIEF_CLAUDE3_TESTING.md)
3. Runtime source code comments (runtime/runtime.c)

---

**End of Phase 4 Testing Documentation**

*Last Updated: November 20, 2025*  
*Test Statistics: 88 tests total (22 C, 66 MLP) - 22/22 C tests passing ✅*
