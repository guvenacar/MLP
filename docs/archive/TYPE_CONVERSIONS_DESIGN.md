# Type Conversions Design Specification

**Phase:** 4 - Advanced Features
**Date:** November 20, 2025
**Status:** Design Complete - Ready for Implementation

---

## 📋 Overview

Phase 4 introduces essential **type conversion functions** that enable seamless conversion between fundamental MLP types. These utilities are critical for:
- User input/output formatting
- String-based data processing
- Interoperability between numeric and text data
- Compiler error message generation

---

## 🎯 Design Goals

1. **Safety First** - Predictable behavior for invalid conversions
2. **Performance** - Efficient implementations without unnecessary allocations
3. **Unicode Ready** - Support UTF-8 strings (future-proof)
4. **Clear Semantics** - No surprises, easy-to-understand behavior
5. **Self-Hosting Ready** - Compiler can use these for code generation

---

## 📚 API Design

### Category 1: Integer ↔ String

#### `int_to_string(num: int) -> string`
Converts integer to decimal string representation.

**Parameters:**
- `num` - Integer value to convert

**Returns:**
- String representation (e.g., `42` → `"42"`)
- Handles negative numbers (e.g., `-42` → `"-42"`)

**Example:**
```mlp
int count = 42;
string message = "Count: " + int_to_string(count);
print message;  -- Output: "Count: 42"

int negative = -100;
string neg_str = int_to_string(negative);
print neg_str;  -- Output: "-100"
```

**Use Cases:**
- Formatting output messages
- Building dynamic strings
- Logging numeric values
- Error message construction

**Implementation Notes:**
```c
char* mlp_int_to_string(long num) {
    // Handle special case: 0
    if (num == 0) return strdup("0");

    // Calculate buffer size
    int is_negative = (num < 0);
    long abs_num = is_negative ? -num : num;
    int digits = 0;
    long temp = abs_num;
    while (temp > 0) {
        digits++;
        temp /= 10;
    }

    // Allocate buffer (sign + digits + null)
    char* buffer = malloc(is_negative + digits + 1);
    buffer[is_negative + digits] = '\0';

    // Fill digits from right to left
    int pos = is_negative + digits - 1;
    while (abs_num > 0) {
        buffer[pos--] = '0' + (abs_num % 10);
        abs_num /= 10;
    }

    // Add sign if negative
    if (is_negative) buffer[0] = '-';

    return buffer;
}
```

---

#### `string_to_int(str: string) -> int`
Parses decimal integer from string.

**Parameters:**
- `str` - String to parse (e.g., `"42"`, `"-123"`)

**Returns:**
- Parsed integer value
- `0` if parsing fails (invalid format)

**Example:**
```mlp
string input = "42";
int value = string_to_int(input);
print value;  -- Output: 42

string negative = "-100";
int neg_value = string_to_int(negative);
print neg_value;  -- Output: -100

string invalid = "abc";
int error = string_to_int(invalid);
print error;  -- Output: 0 (error indicator)
```

**Parsing Rules:**
- Leading/trailing whitespace: **Not trimmed** (use `string_trim()` first)
- Leading `+` or `-` sign: **Allowed**
- Non-digit characters: **Stop parsing, return 0**
- Empty string: **Return 0**
- Overflow: **Undefined behavior** (future: clamp to INT_MIN/INT_MAX)

**Use Cases:**
- Reading user input
- Parsing configuration values
- Processing command-line arguments
- Loading data from text files

**Implementation Notes:**
```c
long mlp_string_to_int(const char* str) {
    if (!str || *str == '\0') return 0;

    long result = 0;
    int sign = 1;
    const char* p = str;

    // Handle sign
    if (*p == '-') {
        sign = -1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    // Parse digits
    while (*p >= '0' && *p <= '9') {
        result = result * 10 + (*p - '0');
        p++;
    }

    // If we didn't parse anything, return 0
    if (p == str || (p == str + 1 && (*str == '+' || *str == '-'))) {
        return 0;
    }

    return sign * result;
}
```

---

### Category 2: Character ↔ String

#### `char_code(ch: string) -> int`
Gets ASCII/Unicode code point of first character.

**Parameters:**
- `ch` - String (only first character used)

**Returns:**
- ASCII/Unicode code point (0-127 for ASCII)
- `0` if string is empty

**Example:**
```mlp
string letter = "A";
int code = char_code(letter);
print code;  -- Output: 65

string digit = "7";
int digit_code = char_code(digit);
print digit_code;  -- Output: 55

-- Check if character is uppercase letter
string ch = "M";
int code = char_code(ch);
if code >= 65 then
    if code <= 90 then
        print "Uppercase letter detected";
    end
end
```

**Use Cases:**
- Character classification (is_digit, is_alpha)
- Case conversion logic
- Lexer character processing
- ASCII table operations

**Implementation Notes:**
```c
long mlp_char_code(const char* str) {
    if (!str || *str == '\0') return 0;
    return (long)(unsigned char)(*str);
}
```

---

#### `char_from_code(code: int) -> string`
Creates single-character string from ASCII/Unicode code point.

**Parameters:**
- `code` - ASCII/Unicode code point (0-127 for ASCII)

**Returns:**
- Single-character string
- Empty string `""` if code is invalid (< 0 or > 127)

**Example:**
```mlp
int code = 65;
string letter = char_from_code(code);
print letter;  -- Output: "A"

int newline_code = 10;
string newline = char_from_code(newline_code);
-- newline now contains "\n"

-- Generate alphabet
int i = 65;
while i <= 90
    string letter = char_from_code(i);
    print letter;
    i = i + 1;
end
-- Output: ABCDEFGHIJKLMNOPQRSTUVWXYZ
```

**Use Cases:**
- Generating characters programmatically
- Building escape sequences
- Character iteration
- Custom string builders

**Implementation Notes:**
```c
char* mlp_char_from_code(long code) {
    // Validate ASCII range (future: extend to Unicode)
    if (code < 0 || code > 127) {
        return strdup("");
    }

    char* result = malloc(2);
    result[0] = (char)code;
    result[1] = '\0';
    return result;
}
```

---

### Category 3: String Concatenation

#### `string_concat(s1: string, s2: string) -> string`
Concatenates two strings (alternative to `+` operator).

**Parameters:**
- `s1` - First string
- `s2` - Second string

**Returns:**
- New string containing `s1` + `s2`

**Example:**
```mlp
string first = "Hello";
string second = "World";
string result = string_concat(first, second);
print result;  -- Output: "HelloWorld"

string with_space = string_concat(first, " ");
string full = string_concat(with_space, second);
print full;  -- Output: "Hello World"

-- Building paths
string dir = "/home/user";
string file = "data.txt";
string path = string_concat(string_concat(dir, "/"), file);
print path;  -- Output: "/home/user/data.txt"
```

**Use Cases:**
- Building complex strings in loops
- When `+` operator is not available
- Explicit string building
- Compiler-generated code

**Implementation Notes:**
```c
char* mlp_string_concat(const char* s1, const char* s2) {
    if (!s1) s1 = "";
    if (!s2) s2 = "";

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    char* result = malloc(len1 + len2 + 1);
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2);
    result[len1 + len2] = '\0';

    return result;
}
```

---

### Category 4: String Inspection

#### `string_length(str: string) -> int`
Returns length of string in characters (already exists in Phase 2).

**Parameters:**
- `str` - String to measure

**Returns:**
- Number of characters (not including null terminator)
- `0` for empty string

**Example:**
```mlp
string text = "Hello";
int len = string_length(text);
print len;  -- Output: 5
```

---

#### `string_char_at(str: string, index: int) -> string`
Gets character at specific index as single-character string.

**Parameters:**
- `str` - String to index
- `index` - Zero-based position

**Returns:**
- Single-character string at position
- Empty string `""` if index out of bounds

**Example:**
```mlp
string text = "Hello";
string first = string_char_at(text, 0);
print first;  -- Output: "H"

string last = string_char_at(text, 4);
print last;  -- Output: "o"

string invalid = string_char_at(text, 100);
-- invalid is empty string ""

-- Iterate through string
int i = 0;
while i < string_length(text)
    string ch = string_char_at(text, i);
    print ch;
    i = i + 1;
end
```

**Use Cases:**
- Character-by-character processing
- String iteration
- Lexer tokenization
- Pattern matching

**Implementation Notes:**
```c
char* mlp_string_char_at(const char* str, long index) {
    if (!str || index < 0 || index >= strlen(str)) {
        return strdup("");
    }

    char* result = malloc(2);
    result[0] = str[index];
    result[1] = '\0';
    return result;
}
```

---

## 🔄 Conversion Matrix

| From ↓ / To → | `int` | `string` | `char` (single) |
|---------------|-------|----------|-----------------|
| **int**       | N/A   | `int_to_string()` | `char_from_code()` |
| **string**    | `string_to_int()` | N/A | `string_char_at()` |
| **char code** | `char_code()` | `char_from_code()` | N/A |

---

## ⚠️ Error Handling Philosophy

### No Exceptions - Return Sentinel Values

MLP type conversions use **sentinel values** to indicate errors:

| Function | Error Indicator | Meaning |
|----------|----------------|---------|
| `string_to_int()` | `0` | Parse failed |
| `char_from_code()` | `""` | Invalid code |
| `string_char_at()` | `""` | Out of bounds |
| `char_code()` | `0` | Empty string |

### Best Practices

```mlp
-- ✅ GOOD: Validate before conversion
string input = "42";
if string_length(input) > 0 then
    int value = string_to_int(input);
    if value != 0 then
        print "Valid number";
    end
end

-- ✅ GOOD: Check bounds
string text = "Hello";
int index = 2;
if index >= 0 then
    if index < string_length(text) then
        string ch = string_char_at(text, index);
        print ch;
    end
end

-- ❌ BAD: Assume conversion succeeds
string bad_input = "not a number";
int value = string_to_int(bad_input);  -- Returns 0
print value;  -- Prints 0, but could be ambiguous
```

---

## 🧩 Compiler Integration

### Lexer (No Changes Required)
Type conversion functions are **built-in functions**, not new syntax. Lexer treats them as `TOKEN_IDENTIFIER`.

### Parser
Built-in functions are handled in `parse_primary()` as function calls:

```c
// In parse_primary()
if (token.type == TOKEN_IDENTIFIER) {
    if (is_builtin_function(token.value)) {
        return parse_builtin_call();
    }
}
```

### AST Node Type
```c
typedef struct {
    ASTNodeType type;  // AST_BUILTIN_CALL
    char* function_name;  // "int_to_string", "string_to_int", etc.
    ASTNode** arguments;   // Array of argument expressions
    int arg_count;
} BuiltinCallNode;
```

### Code Generation

#### Example: `int_to_string(42)`
```nasm
section .text

; Evaluate argument: 42
    mov rax, 42
    push rax

; Call runtime function
    pop rdi              ; First arg in RDI (System V ABI)
    call mlp_int_to_string
    push rax             ; Result (char*) on stack
```

#### Example: `string_to_int("42")`
```nasm
section .data
    str_42 db "42", 0

section .text
    lea rdi, [rel str_42]
    call mlp_string_to_int
    push rax             ; Result (int) on stack
```

#### Example: `char_from_code(65)`
```nasm
    mov rax, 65
    push rax
    pop rdi
    call mlp_char_from_code
    push rax             ; Result (char*) on stack
```

---

## 🎯 Use Cases

### Use Case 1: User Input Processing
```mlp
-- Read user input (hypothetical read_line function)
string input = read_line();
input = string_trim(input);

int age = string_to_int(input);
if age > 0 then
    print "Your age is: ";
    print age;
else
    print "Invalid input";
end
```

### Use Case 2: Compiler Error Messages
```mlp
function report_error(line_num, message) then
    string line_str = int_to_string(line_num);
    string error = "Error at line ";
    error = string_concat(error, line_str);
    error = string_concat(error, ": ");
    error = string_concat(error, message);
    print error;
    return 0
end

report_error(42, "Unexpected token");
-- Output: "Error at line 42: Unexpected token"
```

### Use Case 3: Character Classification
```mlp
function is_digit(ch) then
    int code = char_code(ch);
    if code >= 48 then  -- '0' = 48
        if code <= 57 then  -- '9' = 57
            return 1;
        end
    end
    return 0
end

function is_uppercase(ch) then
    int code = char_code(ch);
    if code >= 65 then  -- 'A' = 65
        if code <= 90 then  -- 'Z' = 90
            return 1;
        end
    end
    return 0
end

string test = "A";
if is_uppercase(test) == 1 then
    print "Uppercase letter!";
end
```

### Use Case 4: String Building in Loops
```mlp
function build_csv(numbers) then
    string result = "";
    int i = 0;
    while i < numbers.size()
        int num = numbers.get(i);
        string num_str = int_to_string(num);
        result = string_concat(result, num_str);

        if i < numbers.size() - 1 then
            result = string_concat(result, ",");
        end

        i = i + 1;
    end
    return result
end

list[int] values = list();
values.add(10);
values.add(20);
values.add(30);

string csv = build_csv(values);
print csv;  -- Output: "10,20,30"
```

---

## 🧪 Testing Strategy

### Test Suite: Type Conversions

#### Test 1: Integer to String
```mlp
-- Positive numbers
if int_to_string(42) == "42" then
    print "PASS: Positive int";
end

-- Negative numbers
if int_to_string(-100) == "-100" then
    print "PASS: Negative int";
end

-- Zero
if int_to_string(0) == "0" then
    print "PASS: Zero";
end
```

#### Test 2: String to Integer
```mlp
-- Valid positive
if string_to_int("42") == 42 then
    print "PASS: Valid positive";
end

-- Valid negative
if string_to_int("-100") == -100 then
    print "PASS: Valid negative";
end

-- Invalid input
if string_to_int("abc") == 0 then
    print "PASS: Invalid returns 0";
end

-- Empty string
if string_to_int("") == 0 then
    print "PASS: Empty returns 0";
end
```

#### Test 3: Character Code Operations
```mlp
-- char_code
if char_code("A") == 65 then
    print "PASS: char_code('A')";
end

-- char_from_code
if char_from_code(65) == "A" then
    print "PASS: char_from_code(65)";
end

-- Round-trip
string original = "Z";
int code = char_code(original);
string restored = char_from_code(code);
if original == restored then
    print "PASS: Round-trip conversion";
end
```

#### Test 4: String Concatenation
```mlp
string a = "Hello";
string b = "World";
string result = string_concat(a, b);
if result == "HelloWorld" then
    print "PASS: Concatenation";
end

-- Empty strings
if string_concat("", "test") == "test" then
    print "PASS: Concat with empty";
end
```

---

## 📊 Performance Considerations

### Memory Allocation
All string-returning functions allocate new memory:
- Caller is responsible for freeing (future: automatic GC)
- Avoid conversions in tight loops when possible

### Optimization Tips
```mlp
-- ❌ BAD: Repeated conversions
int i = 0;
while i < 1000
    string s = int_to_string(i);  -- Allocates each iteration
    print s;
    i = i + 1;
end

-- ✅ BETTER: Batch conversions
list[string] strings = list();
int i = 0;
while i < 1000
    strings.add(int_to_string(i));
    i = i + 1;
end
-- Process batch...
```

---

## 🔮 Future Enhancements

### Phase 5+: Extended Conversions
```mlp
-- Floating point (future)
float f = 3.14;
string f_str = float_to_string(f);  -- "3.14"

-- Boolean (future)
int truth = 1;
string bool_str = bool_to_string(truth);  -- "true"

-- Radix conversion (future)
int num = 255;
string hex = int_to_string_radix(num, 16);  -- "FF"
string binary = int_to_string_radix(num, 2);  -- "11111111"
```

### Phase 6+: Format Strings
```mlp
-- sprintf-style formatting
string msg = string_format("Value: %d, Name: %s", 42, "Test");
-- Output: "Value: 42, Name: Test"
```

---

## 📖 Related Documentation

- [STRING_OPS_DESIGN.md](STRING_OPS_DESIGN.md) - String manipulation
- [MATH_OPERATIONS_DESIGN.md](MATH_OPERATIONS_DESIGN.md) - Math utilities
- [FILE_IO_DESIGN.md](FILE_IO_DESIGN.md) - File I/O operations
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API reference

---

## ✅ Success Criteria

Phase 4 Type Conversions complete when:
- [ ] All 6 functions implemented in runtime
- [ ] Parser recognizes all conversion functions
- [ ] Code generator emits correct runtime calls
- [ ] All test cases pass (int/string/char conversions)
- [ ] Error handling consistent (sentinel values)
- [ ] Documentation complete with examples

---

**Last Updated:** November 20, 2025
**Status:** Design Complete - Ready for Implementation
**Implementation Priority:** 🔴 HIGH (Required for user I/O and self-hosting)
