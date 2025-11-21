# Phase 5.4 - Language Features for Self-Hosting

**Status**: In Progress
**Goal**: Add language features required for MLP self-hosting compiler

---

## Overview

Phase 5.4 adds core language features needed to write the MLP compiler in MLP itself.

---

## 1. Logical Operators: `and`, `or`, `not`

### Syntax
```mlp
if x > 0 and x < 100 then
    print "Valid"
end

if is_empty or is_null then
    return 0
end

if not found then
    print "Not found"
end
```

### Implementation

**Lexer (c_lexer.c)**:
```c
{"and",  TOKEN_OP_AND},
{"or",   TOKEN_OP_OR},
{"not",  TOKEN_OP_NOT},
```

**Parser (c_parser.c)**:
- `and`/`or` → binary operators (lower precedence than comparison)
- `not` → unary operator

**Generator (c_generator.c)**:
```asm
; a and b
mov rax, [a]
test rax, rax
jz .and_false
mov rax, [b]
test rax, rax
jz .and_false
mov rax, 1
jmp .and_end
.and_false:
xor rax, rax
.and_end:
```

---

## 2. Modulo Operator: `%`

### Syntax
```mlp
int remainder = 17 % 5    -- Result: 2
int is_even = num % 2     -- 0 if even
```

### Implementation

**Lexer**: Add `TOKEN_OP_MOD` for `%`

**Generator**:
```asm
; a % b
mov rax, [a]
cqo              ; Sign extend rax to rdx:rax
mov rcx, [b]
idiv rcx         ; rax = quotient, rdx = remainder
mov rax, rdx     ; Result in rax
```

---

## 3. Bitwise Operators

### Syntax
```mlp
int flags = FLAG_A | FLAG_B    -- OR
int masked = value & 0xFF      -- AND
int toggled = bits ^ mask      -- XOR
int shifted = num << 2         -- Left shift
int divided = num >> 1         -- Right shift
```

### Implementation

**Lexer Tokens**:
```c
TOKEN_OP_BIT_AND,   // &
TOKEN_OP_BIT_OR,    // |
TOKEN_OP_BIT_XOR,   // ^
TOKEN_OP_SHL,       // <<
TOKEN_OP_SHR,       // >>
```

**Generator**:
```asm
; a & b
mov rax, [a]
and rax, [b]

; a | b
mov rax, [a]
or rax, [b]

; a ^ b
mov rax, [a]
xor rax, [b]

; a << n
mov rax, [a]
mov cl, [n]
shl rax, cl

; a >> n
mov rax, [a]
mov cl, [n]
sar rax, cl    ; Arithmetic shift (signed)
```

---

## 4. Enum Support

### Syntax
```mlp
enum TokenType then
    TOKEN_INT = 0
    TOKEN_STRING = 1
    TOKEN_IF = 2
    TOKEN_THEN = 3
    TOKEN_END = 4
end

TokenType current = TOKEN_IF
if current == TOKEN_IF then
    print "Found if"
end
```

### Implementation

**AST Node**: `AST_ENUM_DEF`, `AST_ENUM_VALUE`

**Parser**:
1. Parse `enum Name then ... end`
2. Store enum values in symbol table
3. Auto-increment if no value specified

**Generator**:
- Enum values are compile-time constants
- Replace enum references with integer values

---

## 5. Switch/Case Statement

### Syntax
```mlp
switch token_type then
    case TOKEN_INT then
        parse_int()
    end
    case TOKEN_STRING then
        parse_string()
    end
    case TOKEN_IF then
        parse_if()
    end
    default then
        error("Unknown token")
    end
end
```

### Implementation

**AST Nodes**: `AST_SWITCH`, `AST_CASE`, `AST_DEFAULT`

**Generator** (jump table for dense cases):
```asm
; switch(value)
mov rax, [value]
cmp rax, MAX_CASE
ja .default
lea rbx, [.jump_table]
jmp [rbx + rax*8]

.jump_table:
    dq .case_0
    dq .case_1
    dq .case_2
    ...

.case_0:
    ; case 0 code
    jmp .switch_end
.case_1:
    ; case 1 code
    jmp .switch_end
.default:
    ; default code
.switch_end:
```

---

## 6. Priority Order

| Priority | Feature | Difficulty | Self-hosting Need |
|----------|---------|------------|-------------------|
| 1 | `and`/`or`/`not` | Easy | Critical |
| 2 | `%` modulo | Easy | Critical |
| 3 | Bitwise ops | Easy | High |
| 4 | `enum` | Medium | High |
| 5 | `switch/case` | Medium | Medium |

---

## 7. Test Cases

### test_logical_ops.mlp
```mlp
-- Test and/or/not
int a = 5
int b = 10

if a > 0 and b > 0 then
    print "Both positive"
end

if a > 100 or b > 5 then
    print "At least one true"
end

if not (a == b) then
    print "Not equal"
end
```

### test_modulo.mlp
```mlp
-- Test modulo
int i = 0
while i < 10
    if i % 2 == 0 then
        print i
        print " is even"
    end
    i = i + 1
end
```

### test_bitwise.mlp
```mlp
-- Test bitwise
int flags = 0
flags = flags | 1    -- Set bit 0
flags = flags | 4    -- Set bit 2

if flags & 1 then
    print "Bit 0 is set"
end

int shifted = 8 >> 2  -- Result: 2
print shifted
```

### test_enum.mlp
```mlp
-- Test enum
enum Color then
    RED = 0
    GREEN = 1
    BLUE = 2
end

Color c = GREEN
if c == GREEN then
    print "Color is green"
end
```

### test_switch.mlp
```mlp
-- Test switch
int day = 3

switch day then
    case 1 then
        print "Monday"
    end
    case 2 then
        print "Tuesday"
    end
    case 3 then
        print "Wednesday"
    end
    default then
        print "Other day"
    end
end
```

---

## 8. Files to Modify

| File | Changes |
|------|---------|
| `c_lexer.c` | Add new tokens |
| `c_parser.c` | Add parsing for new constructs |
| `c_generator.c` | Add assembly generation |
| `mlp_types.h` | Add new token/AST types |

---

**Status**: Ready for implementation
**Assignee**: Claude #3
