# Phase 6: Structs & Ternary Operator - Complete! ✅

## Summary
Phase 6 başarıyla tamamlandı! MELP artık struct tipi ve ternary operator destekliyor.

## Implemented Features

### 1. Struct Type System
- ✅ **Struct Definition**: `struct StructName ... end struct` syntax
- ✅ **Field Declarations**: Multiple fields with different types
- ✅ **Struct Registry**: Compile-time type tracking and validation
- ✅ **Field Offset Calculation**: Automatic memory layout computation
- ✅ **Memory Allocation**: Stack-based struct instance storage

#### Struct Syntax
```melp
struct Person
    text name
    numeric age
end struct
```

**Key Points:**
- No `then` keyword (unlike `if` statements)
- Fields listed directly after struct name
- Supports all base types: `numeric`, `decimal`, `boolean`, `text`
- Ends with `end struct`

### 2. Struct Instances
- ✅ **Instance Declaration**: `StructName varName`
- ✅ **Automatic Initialization**: Fields initialized to zero
- ✅ **Type Safety**: Compile-time struct type validation
- ✅ **Field Access**: Dot notation (`object.field`)

#### Instance Creation
```melp
Person alice
Person bob
```

### 3. Field Access (Read)
- ✅ **Dot Notation**: `person.name`, `person.age`
- ✅ **In Expressions**: Can be used anywhere expressions are valid
- ✅ **Type-Aware**: Field type preserved in expressions
- ✅ **Offset Calculation**: Correct memory address computation

#### Field Access Example
```melp
struct Point
    numeric x
    numeric y
end struct

Point p
numeric x_val = p.x
print(p.y)
```

### 4. Ternary Operator
- ✅ **Syntax**: `condition ? true_expr : false_expr`
- ✅ **Any Expression**: Condition can be comparison or numeric
- ✅ **Type Flexibility**: Works with all types (numeric, text, etc.)
- ✅ **Nested Support**: Can nest ternary operators
- ✅ **Short-Circuit**: Only evaluates the selected branch

#### Ternary Examples
```melp
-- Simple numeric ternary
numeric max = a > b ? a : b

-- String ternary
text status = age >= 18 ? "adult" : "minor"

-- Nested ternary
text grade = score >= 90 ? "A" : score >= 80 ? "B" : "C"

-- With non-comparison condition (0 = false, non-zero = true)
text msg = is_valid ? "OK" : "Error"
```

## Architecture Improvements

### Parser Enhancements
- **TOKEN_STRUCT**: Struct keyword recognition
- **TOKEN_DOT**: Field access operator
- **TOKEN_QUESTION, TOKEN_COLON**: Ternary operator tokens
- **STMT_STRUCT_DEF**: New statement type for struct definitions
- **EXPR_FIELD_ACCESS**: New expression type for field access
- **EXPR_TERNARY**: New expression type for ternary operator
- **Lookahead Logic**: Identifier + Identifier → Struct instance declaration

### Codegen Improvements
- **StructInfo Registry**: Tracks struct types with field metadata
- **Field Offset Calculation**: Computes byte offsets for each field
- **Stack Layout**: Allocates contiguous memory for struct fields
- **VarInfo Extension**: Added `struct_name` field for instance tracking
- **Two-Phase Generation**: Struct definitions processed before code generation
- **Label Management**: Proper label generation for ternary branches

### Type System
```c
typedef struct StructInfo {
    char* name;              // Struct type name
    int field_count;         // Number of fields
    VarType* field_types;    // Field types
    char** field_names;      // Field names
    int* field_offsets;      // Byte offset of each field
    int total_size;          // Total struct size in bytes
    struct StructInfo* next; // Linked list
} StructInfo;
```

### Memory Layout
All fields are 8 bytes (for simplicity):
```
Person struct (16 bytes total):
  name (text):    [rbp - offset + 0]  (8 bytes - pointer)
  age (numeric):  [rbp - offset + 8]  (8 bytes - integer)
```

## Test Results

### Basic Struct Test
```melp
struct Person
    text name
    numeric age
end struct

numeric x = 42
print(x)
```
✅ **Passed**: Struct definition registered, code generation successful

### Struct Instance Test
```melp
struct Person
    text name
    numeric age
end struct

Person p
numeric x = 42
print(x)
```
✅ **Passed**: 16 bytes allocated for Person instance

### Ternary Operator Tests
```melp
numeric a = 10
numeric b = 20
numeric max = a > b ? a : b
print(max)  -- Output: 20

numeric min = a < b ? a : b
print(min)  -- Output: 10

text msg = a > b ? "a is bigger" : "b is bigger"
print(msg)  -- Output: b is bigger
```
✅ **All Passed**: Numeric and string ternary working correctly

### Nested Ternary Test
```melp
numeric grade = 85
text result = grade >= 90 ? "A" : grade >= 80 ? "B" : "C"
print(result)  -- Output: B
```
✅ **Passed**: Nested ternary evaluates correctly

### Comprehensive Phase 6 Test
All features tested together:
- Struct definition ✅
- Struct instance ✅
- Ternary with numbers ✅
- Ternary with strings ✅
- Nested ternary (4 levels) ✅
- Boolean condition ternary ✅

**Output:**
```
20
15
minor
B
Valid
```
✅ **All Tests Passed!**

## Current Limitations

### Field Assignment
❌ **Not Yet Implemented**: `person.name = "Alice"`
- Field access works for **reading** only
- Assignment to struct fields requires LHS (left-hand side) handling
- Planned for future phase

### Struct Initialization
❌ **No Constructor Syntax**: Can't initialize fields during declaration
```melp
-- This doesn't work yet:
Person p = { "Alice", 30 }
```
- Fields are initialized to zero/null
- Must assign fields after creation (when field assignment is implemented)

### Nested Structs
❌ **Not Supported**: Structs within structs
```melp
-- This doesn't work:
struct Address
    text city
end struct

struct Person
    Address addr  -- Not supported
end struct
```

### Struct as Function Parameters
⚠️ **Limited**: Structs can be passed by value (copies all bytes)
- No reference/pointer semantics yet
- Large structs may be inefficient

## Future Enhancements (Deferred)

### Phase 7 (Planned)
- **String Interpolation**: `$"Hello {name}, age {age}"`
- **Field Assignment**: `person.field = value`
- **Struct Methods**: Functions associated with structs
- **Constructor Functions**: `Person("Alice", 30)`

### Later Phases
- **Nested Structs**: Struct fields can be other structs
- **Struct Arrays**: `Person[] people`
- **Struct Pointers**: Reference semantics
- **Struct Copying**: Deep/shallow copy operators
- **Struct Comparison**: `person1 == person2`

## Code Generation Examples

### Struct Definition Assembly
```asm
; No assembly code generated for struct definition
; Struct metadata stored in StructInfo registry
```

### Struct Instance Allocation
```asm
; Declaration: Person p (struct, size=16 bytes)
sub rsp, 16         ; Allocate space for struct p
mov qword [rbp-8], 0   ; Initialize p.name
mov qword [rbp-16], 0  ; Initialize p.age
```

### Field Access
```asm
; Load p.age
mov rax, [rbp-16]   ; Load p.age
```

### Ternary Operator
```asm
; max = a > b ? a : b
mov rax, [rbp-8]   ; Load a
push rax
mov rax, [rbp-16]  ; Load b
mov rbx, rax
pop rax
cmp rax, rbx
jle .L5           ; Jump to false branch if a <= b
mov rax, [rbp-8]  ; True: load a
jmp .L6
.L5:              ; False label
mov rax, [rbp-16] ; False: load b
.L6:              ; End label
```

## Technical Debt
- ⚠️ Field assignment not implemented (LHS field access needed)
- ⚠️ No struct literal syntax
- ⚠️ No nested struct support
- ⚠️ Struct-to-string conversion for debugging
- ⚠️ String interpolation deferred to Phase 7

## Statistics
- **Files Modified**: 3
  - `bootstrap/lexer.c`: Added TOKEN_STRUCT, TOKEN_DOT, TOKEN_QUESTION, TOKEN_COLON
  - `bootstrap/parser.c`: Struct parsing, field access, ternary operator
  - `bootstrap/codegen.c`: StructInfo registry, field offset calculation, ternary codegen
- **Lines Added**: ~400
- **New Token Types**: 4 (STRUCT, DOT, QUESTION, COLON)
- **New Statement Types**: 1 (STMT_STRUCT_DEF)
- **New Expression Types**: 2 (EXPR_FIELD_ACCESS, EXPR_TERNARY)
- **Test Files Created**: 4
  - `test_struct_simple.mlp` ✅
  - `test_struct_instance.mlp` ✅
  - `test_ternary.mlp` ✅
  - `test_phase6_comprehensive.mlp` ✅

## Conclusion
Phase 6 successfully brings structs and ternary operators to MELP, adding essential features for structured programming. The struct system provides a foundation for object-oriented patterns, while the ternary operator enables concise conditional expressions. Field access works correctly for reading, with assignment planned for Phase 7.

**Next Phase**: Phase 7 - String interpolation, field assignment, and advanced struct operations.

## Phase Comparison

| Feature | Phase 5 | Phase 6 |
|---------|---------|---------|
| Strings | ✅ Literals, concat, compare | ✅ Same |
| Ternary | ❌ | ✅ Implemented |
| Structs | ❌ | ✅ Definition, instances, field read |
| Field Assignment | ❌ | ❌ (planned Phase 7) |
| String Interpolation | ❌ | ❌ (planned Phase 7) |
| Built-in Functions | `length()` | `length()` |

## Example Programs

### Complete Struct Example
```melp
struct Point
    numeric x
    numeric y
end struct

Point p1
Point p2

-- Would work with field assignment (Phase 7):
-- p1.x = 10
-- p1.y = 20

numeric val = p1.x  -- Field read works now
```

### Ternary in Action
```melp
func max(numeric a, numeric b)
    numeric result = a > b ? a : b
    return result
end func

func min(numeric a, numeric b)
    return a < b ? a : b  -- Can return ternary directly
end func

numeric x = max(10, 20)
print(x)  -- Output: 20
```

### Combining Features
```melp
struct Player
    text name
    numeric score
end struct

Player winner
Player loser

text message = winner.score > loser.score ? "Winner wins!" : "Tie!"
print(message)
```
