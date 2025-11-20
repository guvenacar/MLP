# Math Operations Design Specification

**Phase:** 4 - Advanced Features
**Date:** November 20, 2025
**Status:** Design Complete - Ready for Implementation

---

## 📋 Overview

Phase 4 extends MLP with essential **mathematical utility functions** beyond basic arithmetic operators (`+`, `-`, `*`, `/`, `%`). These functions provide:
- Absolute value calculation
- Min/max selection
- Power and square root (future)
- Essential building blocks for algorithms

---

## 🎯 Design Goals

1. **Simple API** - Easy-to-use functions for common operations
2. **Integer Focus** - Phase 4 targets `int` type (float support in Phase 5+)
3. **Predictable** - Clear behavior, no surprises
4. **Efficient** - Minimal overhead, inline-friendly
5. **Self-Hosting Ready** - Compiler can use these internally

---

## 📚 Core Functions

### `abs(num: int) -> int`
Returns absolute value of integer.

**Parameters:**
- `num` - Integer value (can be negative)

**Returns:**
- Absolute value (non-negative)
- Handles `INT_MIN` gracefully (returns `INT_MAX` to avoid overflow)

**Example:**
```mlp
int positive = abs(42);
print positive;  -- Output: 42

int negative = abs(-100);
print negative;  -- Output: 100

int zero = abs(0);
print zero;  -- Output: 0

-- Use case: Distance calculation
int a = 10;
int b = 25;
int distance = abs(a - b);
print distance;  -- Output: 15
```

**Use Cases:**
- Distance calculations
- Error magnitude
- Ensuring positive values
- Mathematical algorithms

**Implementation:**
```c
long mlp_abs(long num) {
    // Handle INT_MIN edge case
    if (num == LONG_MIN) {
        return LONG_MAX;  // Avoid overflow
    }
    return (num < 0) ? -num : num;
}
```

**Code Generation:**
```nasm
; abs(x)
    pop rax              ; Get argument
    test rax, rax        ; Check sign
    jns .positive        ; Jump if non-negative
    neg rax              ; Negate if negative
.positive:
    push rax             ; Push result
```

---

### `min(a: int, b: int) -> int`
Returns smaller of two integers.

**Parameters:**
- `a` - First integer
- `b` - Second integer

**Returns:**
- The smaller value

**Example:**
```mlp
int smaller = min(10, 20);
print smaller;  -- Output: 10

int same = min(5, 5);
print same;  -- Output: 5

int negative = min(-5, 3);
print negative;  -- Output: -5

-- Use case: Clamping
int value = 150;
int max_allowed = 100;
int clamped = min(value, max_allowed);
print clamped;  -- Output: 100
```

**Use Cases:**
- Value clamping
- Range validation
- Finding minimums in data
- Boundary checking

**Implementation:**
```c
long mlp_min(long a, long b) {
    return (a < b) ? a : b;
}
```

**Code Generation:**
```nasm
; min(a, b)
    pop rbx              ; b
    pop rax              ; a
    cmp rax, rbx         ; Compare a and b
    cmovg rax, rbx       ; If a > b, move b to rax
    push rax             ; Push result
```

---

### `max(a: int, b: int) -> int`
Returns larger of two integers.

**Parameters:**
- `a` - First integer
- `b` - Second integer

**Returns:**
- The larger value

**Example:**
```mlp
int larger = max(10, 20);
print larger;  -- Output: 20

int same = max(5, 5);
print same;  -- Output: 5

int positive = max(-5, 3);
print positive;  -- Output: 3

-- Use case: Ensuring minimum value
int input = 5;
int minimum = 10;
int safe_value = max(input, minimum);
print safe_value;  -- Output: 10
```

**Use Cases:**
- Ensuring minimum values
- Finding maximums
- Peak detection
- Threshold enforcement

**Implementation:**
```c
long mlp_max(long a, long b) {
    return (a > b) ? a : b;
}
```

**Code Generation:**
```nasm
; max(a, b)
    pop rbx              ; b
    pop rax              ; a
    cmp rax, rbx         ; Compare a and b
    cmovl rax, rbx       ; If a < b, move b to rax
    push rax             ; Push result
```

---

### `clamp(value: int, min_val: int, max_val: int) -> int`
Restricts value to specified range.

**Parameters:**
- `value` - Value to clamp
- `min_val` - Minimum allowed value
- `max_val` - Maximum allowed value

**Returns:**
- `min_val` if `value < min_val`
- `max_val` if `value > max_val`
- `value` otherwise

**Example:**
```mlp
int result1 = clamp(15, 0, 100);
print result1;  -- Output: 15 (within range)

int result2 = clamp(-10, 0, 100);
print result2;  -- Output: 0 (clamped to min)

int result3 = clamp(150, 0, 100);
print result3;  -- Output: 100 (clamped to max)

-- Use case: Color channel clamping
int red = clamp(pixel_red, 0, 255);
int green = clamp(pixel_green, 0, 255);
int blue = clamp(pixel_blue, 0, 255);
```

**Use Cases:**
- Input validation
- Color channel clamping (0-255)
- Physics constraints
- Safe array indexing

**Implementation:**
```c
long mlp_clamp(long value, long min_val, long max_val) {
    if (value < min_val) return min_val;
    if (value > max_val) return max_val;
    return value;
}
```

**Alternative Implementation (using min/max):**
```c
long mlp_clamp(long value, long min_val, long max_val) {
    return mlp_min(mlp_max(value, min_val), max_val);
}
```

**Code Generation:**
```nasm
; clamp(value, min, max)
    pop r8               ; max_val
    pop rbx              ; min_val
    pop rax              ; value

    ; Compare with min
    cmp rax, rbx
    cmovl rax, rbx       ; If value < min, value = min

    ; Compare with max
    cmp rax, r8
    cmovg rax, r8        ; If value > max, value = max

    push rax             ; Push result
```

---

## 🔢 Extended Math (Future Phases)

### `power(base: int, exp: int) -> int`
Raises base to exponent (integer power).

**Example:**
```mlp
int result = power(2, 8);
print result;  -- Output: 256

int cube = power(5, 3);
print cube;  -- Output: 125
```

**Implementation:**
```c
long mlp_power(long base, long exp) {
    if (exp < 0) return 0;  // Undefined for negative exp
    if (exp == 0) return 1;

    long result = 1;
    while (exp > 0) {
        if (exp & 1) result *= base;
        base *= base;
        exp >>= 1;
    }
    return result;
}
```

---

### `sqrt(num: int) -> int`
Integer square root (floor of actual square root).

**Example:**
```mlp
int result = sqrt(16);
print result;  -- Output: 4

int approx = sqrt(20);
print approx;  -- Output: 4 (floor of 4.47)
```

**Implementation:**
```c
long mlp_sqrt(long num) {
    if (num < 0) return -1;  // Error: negative
    if (num == 0) return 0;

    // Newton's method
    long x = num;
    long y = (x + 1) / 2;
    while (y < x) {
        x = y;
        y = (x + num / x) / 2;
    }
    return x;
}
```

---

## 🧩 Compiler Integration

### Built-in Function Recognition

In parser (`parse_primary()`):

```c
// Built-in math functions
const char* math_builtins[] = {
    "abs", "min", "max", "clamp",
    "power", "sqrt",  // Future
    NULL
};

int is_math_builtin(const char* name) {
    for (int i = 0; math_builtins[i]; i++) {
        if (strcmp(name, math_builtins[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
```

### AST Node Structure

```c
typedef struct {
    ASTNodeType type;      // AST_BUILTIN_CALL
    char* function_name;   // "abs", "min", "max", "clamp"
    ASTNode** args;        // Argument expressions
    int arg_count;         // Number of arguments
} BuiltinCallNode;
```

### Code Generation Pattern

```c
void generate_math_builtin(BuiltinCallNode* node, FILE* out) {
    // Generate arguments (right-to-left for System V ABI)
    for (int i = node->arg_count - 1; i >= 0; i--) {
        generate_expression(node->args[i], out);
    }

    // Pop arguments into registers
    if (node->arg_count >= 1) fprintf(out, "    pop rdi\n");
    if (node->arg_count >= 2) fprintf(out, "    pop rsi\n");
    if (node->arg_count >= 3) fprintf(out, "    pop rdx\n");

    // Call runtime function
    fprintf(out, "    call mlp_%s\n", node->function_name);

    // Push result
    fprintf(out, "    push rax\n");
}
```

---

## 🎯 Use Cases

### Use Case 1: Input Validation
```mlp
function validate_age(age) then
    -- Clamp age to reasonable range
    int valid_age = clamp(age, 0, 150);

    if valid_age != age then
        print "Warning: Age adjusted to valid range";
    end

    return valid_age
end

int input_age = -5;
int safe_age = validate_age(input_age);
print safe_age;  -- Output: 0
```

### Use Case 2: Array Bounds Safety
```mlp
function safe_get(array, index) then
    int size = array.size();
    int safe_index = clamp(index, 0, size - 1);

    if safe_index != index then
        print "Warning: Index clamped to valid range";
    end

    return array.get(safe_index)
end

list[int] numbers = list();
numbers.add(10);
numbers.add(20);
numbers.add(30);

int value = safe_get(numbers, 100);  -- Clamped to 2
print value;  -- Output: 30
```

### Use Case 3: Distance Calculation
```mlp
function manhattan_distance(x1, y1, x2, y2) then
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    return dx + dy
end

int dist = manhattan_distance(0, 0, 3, 4);
print dist;  -- Output: 7
```

### Use Case 4: Finding Min/Max in List
```mlp
function find_min(numbers) then
    if numbers.size() == 0 then
        return 0;
    end

    int min_val = numbers.get(0);
    int i = 1;
    while i < numbers.size()
        int current = numbers.get(i);
        min_val = min(min_val, current);
        i = i + 1;
    end

    return min_val
end

function find_max(numbers) then
    if numbers.size() == 0 then
        return 0;
    end

    int max_val = numbers.get(0);
    int i = 1;
    while i < numbers.size()
        int current = numbers.get(i);
        max_val = max(max_val, current);
        i = i + 1;
    end

    return max_val
end

list[int] values = list();
values.add(42);
values.add(17);
values.add(99);
values.add(3);

int minimum = find_min(values);
int maximum = find_max(values);

print "Min: ";
print minimum;  -- Output: 3
print "Max: ";
print maximum;  -- Output: 99
```

---

## 🧪 Testing Strategy

### Test 1: Absolute Value
```mlp
-- Positive
if abs(42) == 42 then
    print "PASS: abs(positive)";
end

-- Negative
if abs(-100) == 100 then
    print "PASS: abs(negative)";
end

-- Zero
if abs(0) == 0 then
    print "PASS: abs(zero)";
end
```

### Test 2: Min/Max
```mlp
-- min
if min(10, 20) == 10 then
    print "PASS: min(10, 20)";
end

if min(-5, 3) == -5 then
    print "PASS: min with negative";
end

-- max
if max(10, 20) == 20 then
    print "PASS: max(10, 20)";
end

if max(-5, 3) == 3 then
    print "PASS: max with negative";
end
```

### Test 3: Clamp
```mlp
-- Within range
if clamp(15, 0, 100) == 15 then
    print "PASS: clamp within range";
end

-- Below min
if clamp(-10, 0, 100) == 0 then
    print "PASS: clamp below min";
end

-- Above max
if clamp(150, 0, 100) == 100 then
    print "PASS: clamp above max";
end
```

---

## 📊 Performance Considerations

### Inline Optimization
Math functions are **excellent candidates for inlining**:

```c
// Compiler can inline simple functions
#define MLP_INLINE static inline

MLP_INLINE long mlp_abs(long num) {
    return (num < 0) ? -num : num;
}

MLP_INLINE long mlp_min(long a, long b) {
    return (a < b) ? a : b;
}

MLP_INLINE long mlp_max(long a, long b) {
    return (a > b) ? a : b;
}
```

### Assembly Optimization
Modern CPUs have conditional move instructions (`cmov`) that avoid branches:

```nasm
; Optimized min (branchless)
min:
    cmp rdi, rsi
    cmovg rdi, rsi       ; If a > b, move b to result
    mov rax, rdi
    ret
```

---

## 🔮 Future Enhancements

### Phase 5: Floating Point Math
```mlp
float f = 3.14;
float abs_f = fabs(f);
float min_f = fmin(1.5, 2.5);
float max_f = fmax(1.5, 2.5);
```

### Phase 6: Advanced Math
```mlp
-- Trigonometry
float angle = 45.0;
float sine = sin(angle);
float cosine = cos(angle);

-- Logarithms
float log_val = log(100.0);
float log10_val = log10(100.0);

-- Rounding
float rounded = round(3.7);  -- 4.0
float floored = floor(3.7);  -- 3.0
float ceiled = ceil(3.2);    -- 4.0
```

### Phase 7: Random Numbers
```mlp
-- Random integer in range
int random = random_int(0, 100);

-- Seed random generator
seed_random(42);
```

---

## 📖 Related Documentation

- [TYPE_CONVERSIONS_DESIGN.md](TYPE_CONVERSIONS_DESIGN.md) - Type conversion utilities
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API reference
- [ROADMAP.md](ROADMAP.md) - Future features

---

## ✅ Success Criteria

Phase 4 Math Operations complete when:
- [ ] Core functions implemented (`abs`, `min`, `max`, `clamp`)
- [ ] Runtime functions in `runtime/runtime.c`
- [ ] Parser recognizes all math builtins
- [ ] Code generator emits correct calls
- [ ] All test cases pass
- [ ] Documentation complete with examples
- [ ] Performance acceptable (inline-friendly)

---

**Last Updated:** November 20, 2025
**Status:** Design Complete - Ready for Implementation
**Implementation Priority:** 🟡 MEDIUM (Nice-to-have for Phase 4, essential for algorithms)
