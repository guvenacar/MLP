# MLP Base Language Specification (OFFICIAL)

**Version:** 3.0  
**Status:** 🟢 PRODUCTION - This is the ONE TRUE SPEC  
**Last Updated:** November 22, 2025  
**Style:** Ada/VB Inspired - Explicit & Unambiguous

---

## 🎯 Purpose

**MLP Base Language** is the canonical intermediate representation (IR) that ALL MLP compilers must accept.

- **User code** (Turkish/Russian/etc.) → **Preprocessor** → **Base Language** → **Compiler**
- **Self-host compiler** reads Base Language directly
- **Language-agnostic:** English keywords, universal syntax

---

## ✅ Core Principles

1. **No Semicolons** - Python-style, newline-terminated statements
2. **Explicit Ends** - Every block has clear `end <keyword>` terminator
3. **`then` Required** - Used after `if` condition and `function` signature
4. **Case-Insensitive Keywords** - `IF`, `if`, `If` all valid
5. **UTF-8 Support** - Comments and strings support all Unicode

---

## 📝 Syntax Rules

### Rule 1: No Semicolons (Python-Style)
```mlp
int x = 42          ✅ CORRECT
string name = "Ada" ✅ CORRECT

int x = 42;         ❌ WRONG - No semicolons!
```

### Rule 2: Explicit Block Endings
```mlp
if x > 0 then
    print "positive"
end if              ✅ Self-documenting end

if x > 0 then
    print "positive"
end                 ⚠️ ALLOWED but less clear
```

### Rule 3: `then` Keyword
```mlp
if x > 0 then       ✅ `then` required after condition
    print x
end if

function add(a, b) then  ✅ `then` required after params
    return a + b
end function

while x < 10        ✅ NO `then` for while!
    x = x + 1
end while
```

### Rule 4: Conditional While (Not Infinite Loop)
```mlp
while x < 10        ✅ Standard: condition-based
    x = x + 1
end while

while               ❌ DEPRECATED: Use `while true` instead
    if x >= 10 then
        break
    end
    x = x + 1
end while
```

---

## 🔤 Data Types

### Basic Types
```mlp
int       -- 64-bit integer
float     -- Double-precision floating point
string    -- UTF-8 text
bool      -- Boolean (true/false)
```

### Examples
```mlp
int count = 42
float pi = 3.14159
string message = "Hello World"
bool active = true
```

---

## 🔀 Control Structures

### If Statement
```mlp
if x > 0 then
    print "positive"
end if
```

### If-Else
```mlp
if x > 0 then
    print "positive"
else
    print "non-positive"
end if
```

### If-Else If-Else
```mlp
if x > 0 then
    print "positive"
else if x == 0 then
    print "zero"
else
    print "negative"
end if
```

### While Loop
```mlp
while x < 10
    print x
    x = x + 1
end while
```

### For Loop
```mlp
for i = 0 to 10
    print i
end for
```

### For Loop with Step
```mlp
for i = 0 to 100 step 5
    print i
end for
```

### Break Statement
```mlp
while x < 100
    if x == 50 then
        break
    end if
    x = x + 1
end while
```

---

## ⚙️ Functions

### Function Definition
```mlp
function add(a, b) then
    return a + b
end function
```

### Function Call
```mlp
int result = add(5, 3)
print result
```

### Procedure (No Return)
```mlp
function greet(name) then
    print "Hello"
    print name
end function

greet("Alice")
```

---

## 🔢 Operators

### Arithmetic
```mlp
x = a + b     -- Addition
x = a - b     -- Subtraction
x = a * b     -- Multiplication
x = a / b     -- Division
x = a mod b   -- Modulo (keyword, not %)
```

### Comparison
```mlp
x == y        -- Equal
x != y        -- Not equal
x > y         -- Greater than
x < y         -- Less than
x >= y        -- Greater or equal
x <= y        -- Less or equal
```

### Logical
```mlp
a and b       -- Logical AND (keyword)
a or b        -- Logical OR (keyword)
not a         -- Logical NOT (keyword)
```

---

## 💬 Comments

### Single-Line
```mlp
-- This is a comment
int x = 42  -- Inline comment
```

### Multi-Line
```mlp
---
This is a
multi-line comment
---
```

---

## 📚 Built-in Functions

### String Operations
```mlp
int len = string_length(text)
string sub = substring(text, 0, 5)
string concat = string_concat(a, b)
```

### I/O
```mlp
print "Hello"
print x
print "Value:" x  -- Multiple args (space-separated)
```

---

## 🏗️ Structs (Future)

```mlp
struct Person then
    string name
    int age
end struct

Person p
p.name = "Alice"
p.age = 25
```

---

## 📋 Reserved Keywords

```
int float string bool
if then else end
for to step while break continue
function return
struct
print
true false
and or not mod
```

---

## 📖 Complete Example

```mlp
-- Fibonacci calculator
function fibonacci(n) then
    if n <= 1 then
        return n
    end if
    return fibonacci(n - 1) + fibonacci(n - 2)
end function

-- Main program
print "Fibonacci sequence:"

for i = 0 to 10
    int result = fibonacci(i)
    print i
    print result
end for

print "Done!"
```

---

## 🔄 Transformation Examples

### From C-Style → Base Language
```c
// C-style input
if (x > 0) {
    for (int i = 0; i < 10; i++) {
        printf("%d\n", i);
    }
}
```

```mlp
-- Base Language output
if x > 0 then
    for i = 0 to 10
        print i
    end for
end if
```

### From Python-Style → Base Language
```python
# Python-style input
if x > 0:
    for i in range(10):
        print(i)
```

```mlp
-- Base Language output
if x > 0 then
    for i = 0 to 10
        print i
    end for
end if
```

---

## ⚠️ Common Mistakes

### ❌ WRONG: Semicolons
```mlp
int x = 42;  -- NO! Remove semicolon
```

### ❌ WRONG: Missing `then`
```mlp
if x > 0     -- NO! Add `then`
    print x
end if
```

### ❌ WRONG: Generic `end`
```mlp
if x > 0 then
    print x
end          -- UNCLEAR! Use `end if`
```

### ❌ WRONG: Infinite `while` without condition
```mlp
while        -- DEPRECATED! Use `while true`
    x = x + 1
end while
```

---

## ✅ Checklist for Compiler Implementation

- [ ] Lexer accepts all keywords (case-insensitive)
- [ ] Parser requires `then` after `if` and `function`
- [ ] Parser accepts `end if`, `end while`, `end for`, `end function`
- [ ] Parser accepts generic `end` as fallback (but warns)
- [ ] No semicolons expected anywhere
- [ ] While loop requires condition (no naked `while`)
- [ ] Comments: `--` single-line, `---` multi-line
- [ ] Operators: `and`/`or`/`not` keywords, `mod` keyword

---

**🎉 This is the official spec. All other documents must align with this.**
