# Phase 5.6 - Additional Language Features

**Status**: Planned
**Goal**: Add char literals, null keyword, and const support

---

## 1. Character Literals

### Syntax
```mlp
int ch = 'A';           -- ASCII value 65
int newline = '\n';     -- ASCII value 10
int tab = '\t';         -- ASCII value 9
int backslash = '\\';   -- ASCII value 92
```

### Supported Escape Sequences
| Escape | Value | Description |
|--------|-------|-------------|
| `\n` | 10 | Newline |
| `\t` | 9 | Tab |
| `\r` | 13 | Carriage return |
| `\\` | 92 | Backslash |
| `\'` | 39 | Single quote |
| `\"` | 34 | Double quote |
| `\0` | 0 | Null character |

### Implementation

**Lexer**: Parse `'x'` as TOKEN_CHAR with integer value

```c
if (current_char == '\'') {
    // Single character literal
    advance();
    int char_value;
    if (current_char == '\\') {
        // Escape sequence
        advance();
        switch (current_char) {
            case 'n': char_value = 10; break;
            case 't': char_value = 9; break;
            case 'r': char_value = 13; break;
            case '\\': char_value = 92; break;
            case '\'': char_value = 39; break;
            case '0': char_value = 0; break;
            default: char_value = current_char;
        }
    } else {
        char_value = current_char;
    }
    advance();
    expect('\'');
    return createToken(TOKEN_SAYI, int_to_string(char_value));
}
```

**Parser**: Treat as integer constant (no changes needed)

**Generator**: Use integer value directly

---

## 2. Null Keyword

### Syntax
```mlp
int ptr = null;         -- Equivalent to 0
if ptr == null then
    print "Pointer is null"
end
```

### Implementation

**Lexer**: Add TOKEN_NULL keyword

```c
{"null", TOKEN_NULL},
```

**Parser**: Create AST_SAYI node with value "0"

**Generator**: Load 0 into RAX

---

## 3. Const Keyword

### Syntax
```mlp
const int MAX_SIZE = 100;
const string VERSION = "1.0.0";

-- Error: Cannot reassign const
MAX_SIZE = 200;  -- Compile error!
```

### Implementation

**Lexer**: Add TOKEN_CONST keyword

**Parser**:
- Parse as variable declaration with const flag
- Store in symbol table with is_const = true

**Semantic Check**:
- Error if const variable is reassigned

**Generator**: Same as regular variables

---

## 4. Test Cases

### test_char_literals.mlp
```mlp
-- Test character literals
int a = 'A';
print "ASCII of 'A': "
print a

int newline = '\n';
print "ASCII of newline: "
print newline

int tab = '\t';
print "ASCII of tab: "
print tab

-- Character comparison
if a == 65 then
    print "'A' equals 65"
end
```

### test_null.mlp
```mlp
-- Test null keyword
int ptr = null;

if ptr == null then
    print "Pointer is null"
end

if ptr == 0 then
    print "null equals 0"
end
```

### test_const.mlp
```mlp
-- Test const keyword
const int MAX = 100;
const string NAME = "MLP";

print "Max value: "
print MAX

print "Name: "
print NAME

-- This should cause a compile error:
-- MAX = 200;
```

---

## 5. Priority

| Feature | Priority | Difficulty | Self-hosting Need |
|---------|----------|------------|-------------------|
| char literals | High | Easy | Critical (lexer) |
| null keyword | Medium | Very Easy | Useful |
| const keyword | Low | Medium | Nice to have |

---

## 6. Files to Modify

| File | Changes |
|------|---------|
| `c_lexer.h` | Add TOKEN_NULL, TOKEN_CONST |
| `c_lexer.c` | Parse char literals, add keywords |
| `c_parser.c` | Handle const declarations |
| `c_generator.c` | Generate code for char literals |

---

**Status**: Ready for implementation
**Assignee**: Claude #2
