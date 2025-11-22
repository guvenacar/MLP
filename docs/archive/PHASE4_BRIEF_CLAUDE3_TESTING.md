# Phase 4 - Testing Brief (Claude #3)

**Session**: Phase 4 - Hash Maps & Advanced Features
**Your Role**: Testing & Validation
**Branch**: `claude/phase4-testing-[SESSION_ID]`
**Timeline**: ~5-6 hours

---

## 🎯 Your Mission

Test Phase 4 features comprehensively:
1. Hash map compiler integration tests (MLP)
2. Type conversion tests (C + MLP)
3. Math operation tests (C + MLP)
4. Edge case validation
5. Performance benchmarks
6. Integration with existing features

---

## 📋 Phase 1: Hash Map Compiler Tests (MLP)

### 1.1 Create `test/test_hashmap_basic.mlp`

Basic hash map functionality:

```mlp
-- Basic Hash Map Tests

function test_string_int_map() then
    map[string:int] ages = map();
    int result = 0;
    int size = 0;

    print "=== Test: String-Int Map ===";

    -- Test: Initial size should be 0
    size = ages.size();
    print "Initial size: ";
    print size;

    -- Test: Set values
    ages.set("Alice", 25);
    ages.set("Bob", 30);
    ages.set("Charlie", 35);

    size = ages.size();
    print "After 3 inserts: ";
    print size;

    -- Test: Get values
    result = ages.get("Alice");
    print "Alice's age: ";
    print result;

    result = ages.get("Bob");
    print "Bob's age: ";
    print result;

    result = ages.get("Charlie");
    print "Charlie's age: ";
    print result;

    -- Test: Has key
    result = ages.has("Alice");
    print "Has Alice: ";
    print result;

    result = ages.has("David");
    print "Has David: ";
    print result;

    -- Test: Remove
    ages.remove("Bob");
    size = ages.size();
    print "After remove Bob: ";
    print size;

    result = ages.has("Bob");
    print "Has Bob after remove: ";
    print result;

    -- Test: Clear
    ages.clear();
    size = ages.size();
    print "After clear: ";
    print size;

    return 0;
end

function test_int_string_map() then
    map[int:string] names = map();
    string result = "";
    int size = 0;
    int has = 0;

    print "=== Test: Int-String Map ===";

    -- Test: Set values
    names.set(101, "Alice");
    names.set(102, "Bob");
    names.set(103, "Charlie");

    -- Test: Get values
    result = names.get(101);
    print "Employee 101: ";
    print result;

    result = names.get(102);
    print "Employee 102: ";
    print result;

    -- Test: Has key
    has = names.has(101);
    print "Has 101: ";
    print has;

    has = names.has(999);
    print "Has 999: ";
    print has;

    -- Test: Size
    size = names.size();
    print "Total employees: ";
    print size;

    return 0;
end

function test_string_string_map() then
    map[string:string] config = map();
    string result = "";

    print "=== Test: String-String Map ===";

    -- Test: Configuration storage
    config.set("host", "localhost");
    config.set("port", "8080");
    config.set("protocol", "https");

    result = config.get("host");
    print "Host: ";
    print result;

    result = config.get("port");
    print "Port: ";
    print result;

    result = config.get("protocol");
    print "Protocol: ";
    print result;

    return 0;
end

function test_int_int_map() then
    map[int:int] squares = map();
    int result = 0;
    int i = 0;

    print "=== Test: Int-Int Map ===";

    -- Store squares
    while i < 5 then
        int square = i * i;
        squares.set(i, square);
        i = i + 1;
    end

    -- Retrieve squares
    i = 0;
    while i < 5 then
        result = squares.get(i);
        print "Square of ";
        print i;
        print " is ";
        print result;
        i = i + 1;
    end

    return 0;
end

print "=== Hash Map Compiler Tests ===";
print "";
test_string_int_map();
print "";
test_int_string_map();
print "";
test_string_string_map();
print "";
test_int_int_map();
print "";
print "=== All Tests Complete! ===";
```

**Expected Output**:
- All insertions successful
- All retrievals correct
- Has() returns correct values
- Remove works
- Clear resets map
- Size accurate

**Target**: ~200 lines

---

### 1.2 Create `test/test_hashmap_edge_cases.mlp`

Edge cases and stress testing:

```mlp
-- Hash Map Edge Cases

function test_empty_map_operations() then
    map[string:int] empty = map();
    int result = 0;
    int has = 0;

    print "=== Test: Empty Map Operations ===";

    -- Get from empty map
    result = empty.get("nonexistent");
    print "Get from empty: ";
    print result;  -- Should be 0

    -- Has in empty map
    has = empty.has("nonexistent");
    print "Has in empty: ";
    print has;  -- Should be 0

    -- Remove from empty (should not crash)
    empty.remove("nonexistent");
    print "Remove from empty: OK";

    -- Size of empty
    result = empty.size();
    print "Empty size: ";
    print result;  -- Should be 0

    return 0;
end

function test_overwrite_values() then
    map[string:int] map1 = map();
    int result = 0;

    print "=== Test: Overwrite Values ===";

    -- Set initial value
    map1.set("key", 100);
    result = map1.get("key");
    print "Initial value: ";
    print result;

    -- Overwrite
    map1.set("key", 200);
    result = map1.get("key");
    print "After overwrite: ";
    print result;

    -- Overwrite again
    map1.set("key", 300);
    result = map1.get("key");
    print "After second overwrite: ";
    print result;

    -- Size should still be 1
    result = map1.size();
    print "Size: ";
    print result;

    return 0;
end

function test_many_entries() then
    map[int:int] large_map = map();
    int count = 0;
    int i = 0;
    int result = 0;

    print "=== Test: Many Entries (100) ===";

    -- Insert 100 entries
    while i < 100 then
        large_map.set(i, i * 10);
        i = i + 1;
    end

    count = large_map.size();
    print "Inserted entries: ";
    print count;

    -- Verify some values
    result = large_map.get(0);
    print "Value at 0: ";
    print result;

    result = large_map.get(50);
    print "Value at 50: ";
    print result;

    result = large_map.get(99);
    print "Value at 99: ";
    print result;

    -- Test has
    result = large_map.has(75);
    print "Has 75: ";
    print result;

    result = large_map.has(200);
    print "Has 200: ";
    print result;

    return 0;
end

function test_collision_handling() then
    map[string:int] collision_test = map();
    int result = 0;

    print "=== Test: Collision Handling ===";

    -- These strings may cause collisions depending on hash function
    collision_test.set("abc", 1);
    collision_test.set("bca", 2);
    collision_test.set("cab", 3);

    -- All should be retrievable
    result = collision_test.get("abc");
    print "abc: ";
    print result;

    result = collision_test.get("bca");
    print "bca: ";
    print result;

    result = collision_test.get("cab");
    print "cab: ";
    print result;

    return 0;
end

function test_remove_and_reinsert() then
    map[string:int] map1 = map();
    int result = 0;
    int size = 0;

    print "=== Test: Remove and Reinsert ===";

    -- Insert
    map1.set("key", 100);
    size = map1.size();
    print "After insert, size: ";
    print size;

    -- Remove
    map1.remove("key");
    size = map1.size();
    print "After remove, size: ";
    print size;

    -- Verify not present
    result = map1.has("key");
    print "Has key after remove: ";
    print result;

    -- Reinsert with different value
    map1.set("key", 200);
    size = map1.size();
    print "After reinsert, size: ";
    print size;

    result = map1.get("key");
    print "Value after reinsert: ";
    print result;

    return 0;
end

print "=== Hash Map Edge Cases ===";
print "";
test_empty_map_operations();
print "";
test_overwrite_values();
print "";
test_many_entries();
print "";
test_collision_handling();
print "";
test_remove_and_reinsert();
print "";
print "=== All Edge Case Tests Complete! ===";
```

**Target**: ~250 lines

---

## 📋 Phase 2: Type Conversion Tests

### 2.1 Create `runtime/test_type_conversions.c`

C unit tests for type conversion functions:

```c
// runtime/test_type_conversions.c
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

// Extern declarations
extern char* int_to_string(long num);
extern long string_to_int(const char* str);
extern char* char_to_string(int ch);
extern char* string_concat(const char* s1, const char* s2);

void test_int_to_string() {
    printf("TEST: int_to_string\n");

    // Positive number
    char* s1 = int_to_string(42);
    assert(strcmp(s1, "42") == 0);
    printf("  ✓ int_to_string(42) = \"%s\"\n", s1);
    free(s1);

    // Negative number
    char* s2 = int_to_string(-123);
    assert(strcmp(s2, "-123") == 0);
    printf("  ✓ int_to_string(-123) = \"%s\"\n", s2);
    free(s2);

    // Zero
    char* s3 = int_to_string(0);
    assert(strcmp(s3, "0") == 0);
    printf("  ✓ int_to_string(0) = \"%s\"\n", s3);
    free(s3);

    // Large number
    char* s4 = int_to_string(1000000);
    assert(strcmp(s4, "1000000") == 0);
    printf("  ✓ int_to_string(1000000) = \"%s\"\n", s4);
    free(s4);

    printf("  PASSED\n\n");
}

void test_string_to_int() {
    printf("TEST: string_to_int\n");

    // Valid positive
    long n1 = string_to_int("42");
    assert(n1 == 42);
    printf("  ✓ string_to_int(\"42\") = %ld\n", n1);

    // Valid negative
    long n2 = string_to_int("-123");
    assert(n2 == -123);
    printf("  ✓ string_to_int(\"-123\") = %ld\n", n2);

    // Zero
    long n3 = string_to_int("0");
    assert(n3 == 0);
    printf("  ✓ string_to_int(\"0\") = %ld\n", n3);

    // Invalid string (returns 0)
    long n4 = string_to_int("abc");
    assert(n4 == 0);
    printf("  ✓ string_to_int(\"abc\") = %ld (invalid)\n", n4);

    // Partial number
    long n5 = string_to_int("123abc");
    assert(n5 == 123);
    printf("  ✓ string_to_int(\"123abc\") = %ld (partial)\n", n5);

    // NULL
    long n6 = string_to_int(NULL);
    assert(n6 == 0);
    printf("  ✓ string_to_int(NULL) = %ld\n", n6);

    printf("  PASSED\n\n");
}

void test_char_to_string() {
    printf("TEST: char_to_string\n");

    // Letter
    char* s1 = char_to_string(65); // 'A'
    assert(strcmp(s1, "A") == 0);
    printf("  ✓ char_to_string(65) = \"%s\"\n", s1);
    free(s1);

    // Digit
    char* s2 = char_to_string(48); // '0'
    assert(strcmp(s2, "0") == 0);
    printf("  ✓ char_to_string(48) = \"%s\"\n", s2);
    free(s2);

    // Space
    char* s3 = char_to_string(32); // ' '
    assert(strcmp(s3, " ") == 0);
    printf("  ✓ char_to_string(32) = \" \"\n");
    free(s3);

    // Newline
    char* s4 = char_to_string(10); // '\n'
    assert(strlen(s4) == 1 && s4[0] == '\n');
    printf("  ✓ char_to_string(10) = \"\\n\"\n");
    free(s4);

    printf("  PASSED\n\n");
}

void test_string_concat() {
    printf("TEST: string_concat\n");

    // Normal concatenation
    char* s1 = string_concat("Hello", " World");
    assert(strcmp(s1, "Hello World") == 0);
    printf("  ✓ string_concat(\"Hello\", \" World\") = \"%s\"\n", s1);
    free(s1);

    // Empty strings
    char* s2 = string_concat("", "Test");
    assert(strcmp(s2, "Test") == 0);
    printf("  ✓ string_concat(\"\", \"Test\") = \"%s\"\n", s2);
    free(s2);

    char* s3 = string_concat("Test", "");
    assert(strcmp(s3, "Test") == 0);
    printf("  ✓ string_concat(\"Test\", \"\") = \"%s\"\n", s3);
    free(s3);

    // Both empty
    char* s4 = string_concat("", "");
    assert(strcmp(s4, "") == 0);
    printf("  ✓ string_concat(\"\", \"\") = \"\"\n");
    free(s4);

    // Multiple concatenations
    char* s5 = string_concat("A", "B");
    char* s6 = string_concat(s5, "C");
    assert(strcmp(s6, "ABC") == 0);
    printf("  ✓ Multiple concat = \"%s\"\n", s6);
    free(s5);
    free(s6);

    printf("  PASSED\n\n");
}

void test_roundtrip_conversions() {
    printf("TEST: Roundtrip Conversions\n");

    // int -> string -> int
    long original = 12345;
    char* str = int_to_string(original);
    long result = string_to_int(str);
    assert(result == original);
    printf("  ✓ int(%ld) -> string(\"%s\") -> int(%ld)\n", original, str, result);
    free(str);

    // Negative roundtrip
    long neg = -9876;
    char* str2 = int_to_string(neg);
    long result2 = string_to_int(str2);
    assert(result2 == neg);
    printf("  ✓ int(%ld) -> string(\"%s\") -> int(%ld)\n", neg, str2, result2);
    free(str2);

    printf("  PASSED\n\n");
}

int main() {
    printf("=================================\n");
    printf("Type Conversion Tests\n");
    printf("=================================\n\n");

    test_int_to_string();
    test_string_to_int();
    test_char_to_string();
    test_string_concat();
    test_roundtrip_conversions();

    printf("=================================\n");
    printf("All Type Conversion Tests PASSED!\n");
    printf("=================================\n");

    return 0;
}
```

**Target**: ~200 lines

---

### 2.2 Create `test/test_type_conversions.mlp`

MLP integration tests:

```mlp
-- Type Conversion Integration Tests

function test_int_to_string_basic() then
    string s1 = "";
    string s2 = "";
    string s3 = "";

    print "=== Test: int_to_string ===";

    s1 = int_to_string(42);
    print "int_to_string(42): ";
    print s1;

    s2 = int_to_string(-123);
    print "int_to_string(-123): ";
    print s2;

    s3 = int_to_string(0);
    print "int_to_string(0): ";
    print s3;

    return 0;
end

function test_string_to_int_basic() then
    int n1 = 0;
    int n2 = 0;
    int n3 = 0;

    print "=== Test: string_to_int ===";

    n1 = string_to_int("42");
    print "string_to_int(\"42\"): ";
    print n1;

    n2 = string_to_int("-123");
    print "string_to_int(\"-123\"): ";
    print n2;

    n3 = string_to_int("0");
    print "string_to_int(\"0\"): ";
    print n3;

    return 0;
end

function test_roundtrip() then
    int original = 12345;
    string str = "";
    int result = 0;

    print "=== Test: Roundtrip Conversion ===";

    print "Original: ";
    print original;

    str = int_to_string(original);
    print "As string: ";
    print str;

    result = string_to_int(str);
    print "Back to int: ";
    print result;

    return 0;
end

function test_string_concat_basic() then
    string s1 = "Hello";
    string s2 = " World";
    string result = "";

    print "=== Test: string_concat ===";

    result = string_concat(s1, s2);
    print "Concat result: ";
    print result;

    return 0;
end

function test_number_formatting() then
    int score = 95;
    int total = 100;
    string message = "";
    string score_str = "";
    string total_str = "";

    print "=== Test: Number Formatting ===";

    score_str = int_to_string(score);
    total_str = int_to_string(total);

    message = string_concat("Score: ", score_str);
    message = string_concat(message, "/");
    message = string_concat(message, total_str);

    print message;

    return 0;
end

function test_char_to_string_basic() then
    string a = "";
    string b = "";
    string c = "";

    print "=== Test: char_to_string ===";

    a = char_to_string(65);
    print "char_to_string(65): ";
    print a;

    b = char_to_string(66);
    print "char_to_string(66): ";
    print b;

    c = char_to_string(67);
    print "char_to_string(67): ";
    print c;

    return 0;
end

print "=== Type Conversion Tests ===";
print "";
test_int_to_string_basic();
print "";
test_string_to_int_basic();
print "";
test_roundtrip();
print "";
test_string_concat_basic();
print "";
test_number_formatting();
print "";
test_char_to_string_basic();
print "";
print "=== All Tests Complete! ===";
```

**Target**: ~180 lines

---

## 📋 Phase 3: Math Operation Tests

### 3.1 Create `runtime/test_math_ops.c`

```c
// runtime/test_math_ops.c
#include <stdio.h>
#include <assert.h>

// Extern declarations
extern long math_abs(long n);
extern long math_min(long a, long b);
extern long math_max(long a, long b);
extern long math_pow(long base, long exp);

void test_math_abs() {
    printf("TEST: math_abs\n");

    assert(math_abs(10) == 10);
    printf("  ✓ math_abs(10) = %ld\n", math_abs(10));

    assert(math_abs(-10) == 10);
    printf("  ✓ math_abs(-10) = %ld\n", math_abs(-10));

    assert(math_abs(0) == 0);
    printf("  ✓ math_abs(0) = %ld\n", math_abs(0));

    assert(math_abs(-999) == 999);
    printf("  ✓ math_abs(-999) = %ld\n", math_abs(-999));

    printf("  PASSED\n\n");
}

void test_math_min() {
    printf("TEST: math_min\n");

    assert(math_min(10, 20) == 10);
    printf("  ✓ math_min(10, 20) = %ld\n", math_min(10, 20));

    assert(math_min(20, 10) == 10);
    printf("  ✓ math_min(20, 10) = %ld\n", math_min(20, 10));

    assert(math_min(-5, 5) == -5);
    printf("  ✓ math_min(-5, 5) = %ld\n", math_min(-5, 5));

    assert(math_min(0, 0) == 0);
    printf("  ✓ math_min(0, 0) = %ld\n", math_min(0, 0));

    assert(math_min(-10, -20) == -20);
    printf("  ✓ math_min(-10, -20) = %ld\n", math_min(-10, -20));

    printf("  PASSED\n\n");
}

void test_math_max() {
    printf("TEST: math_max\n");

    assert(math_max(10, 20) == 20);
    printf("  ✓ math_max(10, 20) = %ld\n", math_max(10, 20));

    assert(math_max(20, 10) == 20);
    printf("  ✓ math_max(20, 10) = %ld\n", math_max(20, 10));

    assert(math_max(-5, 5) == 5);
    printf("  ✓ math_max(-5, 5) = %ld\n", math_max(-5, 5));

    assert(math_max(0, 0) == 0);
    printf("  ✓ math_max(0, 0) = %ld\n", math_max(0, 0));

    assert(math_max(-10, -20) == -10);
    printf("  ✓ math_max(-10, -20) = %ld\n", math_max(-10, -20));

    printf("  PASSED\n\n");
}

void test_math_pow() {
    printf("TEST: math_pow\n");

    assert(math_pow(2, 3) == 8);
    printf("  ✓ math_pow(2, 3) = %ld\n", math_pow(2, 3));

    assert(math_pow(5, 2) == 25);
    printf("  ✓ math_pow(5, 2) = %ld\n", math_pow(5, 2));

    assert(math_pow(10, 0) == 1);
    printf("  ✓ math_pow(10, 0) = %ld\n", math_pow(10, 0));

    assert(math_pow(2, 10) == 1024);
    printf("  ✓ math_pow(2, 10) = %ld\n", math_pow(2, 10));

    assert(math_pow(1, 100) == 1);
    printf("  ✓ math_pow(1, 100) = %ld\n", math_pow(1, 100));

    assert(math_pow(3, 4) == 81);
    printf("  ✓ math_pow(3, 4) = %ld\n", math_pow(3, 4));

    printf("  PASSED\n\n");
}

void test_combined_operations() {
    printf("TEST: Combined Operations\n");

    // Clamp function: max(min_val, min(max_val, value))
    long value = 150;
    long min_val = 0;
    long max_val = 100;
    long clamped = math_min(max_val, math_max(min_val, value));
    assert(clamped == 100);
    printf("  ✓ clamp(150, 0, 100) = %ld\n", clamped);

    // Distance: abs(a - b)
    long a = 10;
    long b = 30;
    long dist = math_abs(a - b);
    assert(dist == 20);
    printf("  ✓ distance(10, 30) = %ld\n", dist);

    // Power then min
    long p1 = math_pow(2, 5);  // 32
    long p2 = math_pow(3, 3);  // 27
    long min_pow = math_min(p1, p2);
    assert(min_pow == 27);
    printf("  ✓ min(2^5, 3^3) = %ld\n", min_pow);

    printf("  PASSED\n\n");
}

int main() {
    printf("=================================\n");
    printf("Math Operations Tests\n");
    printf("=================================\n\n");

    test_math_abs();
    test_math_min();
    test_math_max();
    test_math_pow();
    test_combined_operations();

    printf("=================================\n");
    printf("All Math Tests PASSED!\n");
    printf("=================================\n");

    return 0;
}
```

**Target**: ~150 lines

---

### 3.2 Create `test/test_math_ops.mlp`

```mlp
-- Math Operations Integration Tests

function test_abs() then
    int pos = 42;
    int neg = -42;
    int zero = 0;
    int result1 = 0;
    int result2 = 0;
    int result3 = 0;

    print "=== Test: math_abs ===";

    result1 = math_abs(pos);
    print "abs(42): ";
    print result1;

    result2 = math_abs(neg);
    print "abs(-42): ";
    print result2;

    result3 = math_abs(zero);
    print "abs(0): ";
    print result3;

    return 0;
end

function test_min_max() then
    int a = 10;
    int b = 20;
    int min_result = 0;
    int max_result = 0;

    print "=== Test: math_min / math_max ===";

    min_result = math_min(a, b);
    print "min(10, 20): ";
    print min_result;

    max_result = math_max(a, b);
    print "max(10, 20): ";
    print max_result;

    min_result = math_min(b, a);
    print "min(20, 10): ";
    print min_result;

    max_result = math_max(b, a);
    print "max(20, 10): ";
    print max_result;

    return 0;
end

function test_pow() then
    int pow1 = 0;
    int pow2 = 0;
    int pow3 = 0;
    int pow4 = 0;

    print "=== Test: math_pow ===";

    pow1 = math_pow(2, 3);
    print "2^3: ";
    print pow1;

    pow2 = math_pow(5, 2);
    print "5^2: ";
    print pow2;

    pow3 = math_pow(10, 0);
    print "10^0: ";
    print pow3;

    pow4 = math_pow(2, 10);
    print "2^10: ";
    print pow4;

    return 0;
end

function test_clamp() then
    int value = 150;
    int min_val = 0;
    int max_val = 100;
    int clamped = 0;

    print "=== Test: Range Clamping ===";

    print "Original value: ";
    print value;

    -- Clamp: max(min_val, min(max_val, value))
    clamped = math_max(min_val, value);
    clamped = math_min(max_val, clamped);

    print "Clamped to [0, 100]: ";
    print clamped;

    return 0;
end

function test_distance() then
    int x1 = 10;
    int x2 = 30;
    int diff = 0;
    int dist = 0;

    print "=== Test: Distance Calculation ===";

    diff = x2 - x1;
    dist = math_abs(diff);

    print "Distance between 10 and 30: ";
    print dist;

    return 0;
end

function test_power_of_two() then
    int n = 16;
    int power = 1;
    int i = 0;
    int is_pow2 = 0;

    print "=== Test: Power of Two Check ===";

    print "Checking if 16 is power of 2...";

    while power < n then
        power = math_pow(2, i);
        i = i + 1;
    end

    if power == n then
        is_pow2 = 1;
    end

    print "Is power of 2: ";
    print is_pow2;

    return 0;
end

print "=== Math Operations Tests ===";
print "";
test_abs();
print "";
test_min_max();
print "";
test_pow();
print "";
test_clamp();
print "";
test_distance();
print "";
test_power_of_two();
print "";
print "=== All Tests Complete! ===";
```

**Target**: ~180 lines

---

## 📋 Phase 4: Integration Tests

### 4.1 Create `test/test_phase4_complete.mlp`

Comprehensive integration test combining all Phase 4 features:

```mlp
-- Phase 4 Complete Integration Test

function test_symbol_table() then
    map[string:int] symbols = map();
    string var_name = "";
    int var_value = 0;
    int count = 0;

    print "=== Integration: Symbol Table ===";

    -- Store variable addresses/values
    symbols.set("counter", 100);
    symbols.set("total", 200);
    symbols.set("index", 300);

    count = symbols.size();
    print "Symbol table size: ";
    print count;

    var_value = symbols.get("counter");
    print "Variable 'counter': ";
    print var_value;

    return 0;
end

function test_configuration_system() then
    map[string:string] config = map();
    string host = "";
    string port = "";
    string full_url = "";

    print "=== Integration: Configuration ===";

    config.set("host", "localhost");
    config.set("port", "8080");

    host = config.get("host");
    port = config.get("port");

    -- Build URL using string_concat
    full_url = string_concat("http://", host);
    full_url = string_concat(full_url, ":");
    full_url = string_concat(full_url, port);

    print "Server URL: ";
    print full_url;

    return 0;
end

function test_data_processing() then
    list[int] numbers = list();
    int i = 0;
    int sum = 0;
    int min_val = 999999;
    int max_val = -999999;
    int current = 0;
    string summary = "";

    print "=== Integration: Data Processing ===";

    -- Add data
    numbers.add(42);
    numbers.add(17);
    numbers.add(99);
    numbers.add(23);
    numbers.add(5);

    -- Calculate statistics
    i = 0;
    while i < 5 then
        current = numbers.get(i);
        sum = sum + current;
        min_val = math_min(min_val, current);
        max_val = math_max(max_val, current);
        i = i + 1;
    end

    print "Sum: ";
    print sum;
    print "Min: ";
    print min_val;
    print "Max: ";
    print max_val;

    -- Build summary
    summary = string_concat("Stats: min=", int_to_string(min_val));
    summary = string_concat(summary, ", max=");
    summary = string_concat(summary, int_to_string(max_val));

    print summary;

    return 0;
end

function test_cache_with_computation() then
    map[int:int] fibonacci_cache = map();
    int n = 10;
    int i = 0;
    int prev = 0;
    int curr = 1;
    int next = 0;

    print "=== Integration: Fibonacci Cache ===";

    -- Compute and cache fibonacci numbers
    fibonacci_cache.set(0, 0);
    fibonacci_cache.set(1, 1);

    i = 2;
    while i <= n then
        prev = fibonacci_cache.get(i - 2);
        curr = fibonacci_cache.get(i - 1);
        next = prev + curr;
        fibonacci_cache.set(i, next);
        i = i + 1;
    end

    -- Retrieve from cache
    int fib_10 = fibonacci_cache.get(10);
    print "Fibonacci(10) from cache: ";
    print fib_10;

    int cache_size = fibonacci_cache.size();
    print "Cache entries: ";
    print cache_size;

    return 0;
end

function test_all_features_combined() then
    map[string:int] data_map = map();
    list[string] keys = list();
    string key = "";
    int value = 0;
    int total = 0;
    int i = 0;
    string report = "";

    print "=== Integration: All Features ===";

    -- Build data
    data_map.set("apples", 10);
    data_map.set("oranges", 20);
    data_map.set("bananas", 15);

    keys.add("apples");
    keys.add("oranges");
    keys.add("bananas");

    -- Process
    i = 0;
    while i < 3 then
        key = keys.get(i);
        value = data_map.get(key);
        total = total + value;
        i = i + 1;
    end

    -- Report
    report = string_concat("Total items: ", int_to_string(total));
    print report;

    int max_value = 0;
    i = 0;
    while i < 3 then
        key = keys.get(i);
        value = data_map.get(key);
        max_value = math_max(max_value, value);
        i = i + 1;
    end

    print "Maximum quantity: ";
    print max_value;

    return 0;
end

print "=== Phase 4 Complete Integration Tests ===";
print "";
test_symbol_table();
print "";
test_configuration_system();
print "";
test_data_processing();
print "";
test_cache_with_computation();
print "";
test_all_features_combined();
print "";
print "=== All Integration Tests Complete! ===";
```

**Target**: ~250 lines

---

## 📋 Phase 5: Test Compilation & Validation

### 5.1 Create `test/run_phase4_tests.sh`

```bash
#!/bin/bash

# Phase 4 Test Runner

echo "========================================"
echo "Phase 4 Test Suite"
echo "========================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
TOTAL=0
PASSED=0
FAILED=0

# Runtime C tests
echo "=== C Runtime Tests ==="
echo ""

echo "Building runtime tests..."
cd runtime

# Test type conversions
echo "Test: Type Conversions"
gcc -o test_type_conversions test_type_conversions.c runtime.c -lm
if ./test_type_conversions > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASSED${NC}"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC}"
    FAILED=$((FAILED + 1))
fi
TOTAL=$((TOTAL + 1))

# Test math operations
echo "Test: Math Operations"
gcc -o test_math_ops test_math_ops.c runtime.c -lm
if ./test_math_ops > /dev/null 2>&1; then
    echo -e "${GREEN}✓ PASSED${NC}"
    PASSED=$((PASSED + 1))
else
    echo -e "${RED}✗ FAILED${NC}"
    FAILED=$((FAILED + 1))
fi
TOTAL=$((TOTAL + 1))

cd ..
echo ""

# MLP Integration tests
echo "=== MLP Integration Tests ==="
echo ""

cd c_compiler

# Compile tests
TESTS=(
    "test_hashmap_basic"
    "test_hashmap_edge_cases"
    "test_type_conversions"
    "test_math_ops"
    "test_phase4_complete"
)

for test in "${TESTS[@]}"; do
    echo "Test: $test"
    ./compiler_test ../test/${test}.mlp ../test/${test}.asm > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✓ COMPILED${NC}"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}✗ FAILED${NC}"
        FAILED=$((FAILED + 1))
    fi
    TOTAL=$((TOTAL + 1))
done

cd ..
echo ""

# Summary
echo "========================================"
echo "Test Summary"
echo "========================================"
echo "Total:  $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed: $FAILED${NC}"
else
    echo "Failed: $FAILED"
fi
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed.${NC}"
    exit 1
fi
```

**Target**: ~100 lines

---

## 📂 File Structure

Your deliverables:
```
/home/user/MLP/
├── test/
│   ├── test_hashmap_basic.mlp           (NEW: ~200 lines)
│   ├── test_hashmap_edge_cases.mlp      (NEW: ~250 lines)
│   ├── test_type_conversions.mlp        (NEW: ~180 lines)
│   ├── test_math_ops.mlp                (NEW: ~180 lines)
│   ├── test_phase4_complete.mlp         (NEW: ~250 lines)
│   └── run_phase4_tests.sh              (NEW: ~100 lines)
├── runtime/
│   ├── test_type_conversions.c          (NEW: ~200 lines)
│   └── test_math_ops.c                  (NEW: ~150 lines)
└── PHASE4_TESTING_SUMMARY.md            (NEW: ~200 lines)
```

**Total**: ~1,710 lines of tests

---

## ✅ Success Criteria

1. ✅ All 4 map type combinations tested (string:int, int:string, string:string, int:int)
2. ✅ Edge cases covered (empty map, overwrites, 100+ entries, collisions)
3. ✅ Type conversion C tests pass
4. ✅ Type conversion MLP tests compile
5. ✅ Math operation C tests pass
6. ✅ Math operation MLP tests compile
7. ✅ Complete integration test works
8. ✅ Test runner script functional

---

## 🎯 Timeline

**Hour 1-2**: Hash map MLP tests (basic + edge cases)
**Hour 3**: Type conversion tests (C + MLP)
**Hour 4**: Math operation tests (C + MLP)
**Hour 5**: Integration tests
**Hour 6**: Test runner + validation + summary

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase4-testing-[SESSION_ID]

git add test/test_hashmap_*.mlp
git commit -m "test: Add hash map compiler tests"

git add runtime/test_type_conversions.c test/test_type_conversions.mlp
git commit -m "test: Add type conversion tests"

git add runtime/test_math_ops.c test/test_math_ops.mlp
git commit -m "test: Add math operation tests"

git add test/test_phase4_complete.mlp
git commit -m "test: Add Phase 4 integration tests"

git add test/run_phase4_tests.sh
git commit -m "test: Add Phase 4 test runner"

git push -u origin claude/phase4-testing-[SESSION_ID]
```

---

## 🎉 When You're Done

Report:
1. Tests created (X/10)
2. C tests passing (X/2)
3. MLP tests compiling (X/5)
4. Test runner working
5. Ready for coordination

**Branch**: `claude/phase4-testing-[SESSION_ID]`
**Expected**: 6-8 commits

Let's test Phase 4 thoroughly! ✅
