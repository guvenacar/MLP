# MLP Base Syntax Specification (IR)

**Version:** 1.0
**Purpose:** Intermediate Representation for Multi-Language Programming
**Design:** Simplified Ada/VB Style - Unambiguous, Easy to Parse

---

## Philosophy

MLP Base Syntax is an **Intermediate Representation (IR)** that users typically never see directly. User code in any language/syntax is transformed into this canonical form before compilation.

**Key Design Principles:**
1. **Unambiguous:** Every construct has exactly one meaning
2. **Explicit:** Block endings clearly marked (no `}}}` confusion)
3. **Simple Parser:** Minimal state machine, no indentation tracking
4. **Universal Target:** Any syntax can transform to this IR

---

## 1. Variable Declaration & Assignment

### Variable Declaration
```mlp
int x = 42
string name = "Alice"
bool flag = true
float pi = 3.14159
```

### Assignment
```mlp
x = 100
name = "Bob"
flag = false
```

### Multiple Assignment
```mlp
a = b = c = 0
```

**Notes:**
- Type inference optional: `x = 42` (type inferred as int)
- Decimal separator: `.` (IR standard, transformed from `,` in Turkish)

---

## 2. Conditional Statements

### Simple If
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

### If-Elif-Else
```mlp
if x > 0 then
    print "positive"
else if x == 0 then
    print "zero"
else
    print "negative"
end if
```

**Notes:**
- `then` keyword required after condition
- `else if` (two words) for elif
- Block ends with `end if` (explicit, unambiguous)

---

## 3. Loops

### For Loop (Counter-based)
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

### For Loop (Downward)
```mlp
for i = 10 to 0 step -1
    print i
end for
```

### While Loop (Infinite with Break)
```mlp
while
    if x > 10 then
        break
    end if
    x = x + 1
end while
```

**Notes:**
- For loop: inclusive range (0 to 10 includes both 0 and 10)
- While: infinite loop, use `break` to exit
- Block ends with `end for` or `end while`

---

## 4. Functions

### Function Definition
```mlp
function add(a, b) then
    return a + b
end function
```

### Procedure (No Return)
```mlp
function greet(name) then
    print "Hello"
    print name
end function
```

### Function Call
```mlp
int result = add(5, 3)
greet("Alice")
```

**Notes:**
- `then` keyword required after parameter list
- Return type not specified (inferred or dynamic)
- Block ends with `end function`

---

## 5. Operators

### Arithmetic Operators
```mlp
x = a + b     # Addition
x = a - b     # Subtraction
x = a * b     # Multiplication
x = a / b     # Division
x = a mod b   # Modulo (not %)
```

### Comparison Operators
```mlp
x == y        # Equal
x != y        # Not equal (also <>)
x > y         # Greater than
x < y         # Less than
x >= y        # Greater or equal
x <= y        # Less or equal
```

### Logical Operators
```mlp
a and b       # Logical AND
a or b        # Logical OR
not a         # Logical NOT
```

### Assignment
```mlp
x = 5         # Simple assignment
x = x + 1     # Increment
```

**Notes:**
- Comparison uses `==` (double equals) to distinguish from assignment `=`
- Both `!=` and `<>` accepted for not-equal
- `mod` keyword for modulo (not `%` symbol)

---

## 6. Data Types

### Basic Types
```mlp
int       # Integer
float     # Floating point
string    # Text string
bool      # Boolean (true/false)
```

### Array (Optional - Future)
```mlp
int[] numbers = [1, 2, 3, 4, 5]
string[] names = ["Alice", "Bob", "Charlie"]
```

### Type Examples
```mlp
int count = 42
float pi = 3.14159
string message = "Hello World"
bool active = true
```

**Notes:**
- Type keywords are short: `int`, `float`, `string`, `bool`
- Literals: `true`, `false` (lowercase)
- String literals: double quotes only `"..."`

---

## 7. Input/Output

### Output
```mlp
print "Hello World"
print x
print "Value:" x
```

### Input (Optional - Future)
```mlp
string input = read_line()
int number = read_int()
```

**Notes:**
- `print` statement (no parentheses in IR)
- Multiple values separated by space (implicit concatenation)

---

## 8. Comments

### Single Line Comment
```mlp
-- This is a comment
int x = 42  -- Inline comment
```

### Multi-line Comment (Optional)
```mlp
---
This is a
multi-line comment
---
```

**Notes:**
- Single line: `--` (double dash)
- Multi-line: `---` delimiters (triple dash)

---

## 9. String Operations

### Concatenation
```mlp
string full = first + " " + last
```

### Built-in Functions (Optional)
```mlp
int len = length(text)
string sub = substring(text, 0, 5)
string upper = to_upper(text)
string lower = to_lower(text)
```

---

## 10. Struct/Record (Optional - Future)

### Define Struct
```mlp
struct Person then
    string name
    int age
end struct
```

### Create Instance
```mlp
Person p
p.name = "Alice"
p.age = 25
```

### Access Member
```mlp
print p.name
print p.age
```

**Notes:**
- `then` keyword after struct name
- Block ends with `end struct`

---

## 11. Loop Control

### Break Statement
```mlp
while
    if x > 10 then
        break
    end if
    x = x + 1
end while
```

### Continue Statement (Optional)
```mlp
for i = 0 to 10
    if i == 5 then
        continue
    end if
    print i
end for
```

---

## Block Ending Summary

| Construct | Opening | Closing |
|-----------|---------|---------|
| If | `if ... then` | `end if` |
| For | `for ... to` | `end for` |
| While | `while` | `end while` |
| Function | `function ... then` | `end function` |
| Struct | `struct ... then` | `end struct` |

**Philosophy:** Explicit endings eliminate ambiguity in deeply nested code.

---

## Reserved Keywords

```
int, float, string, bool
if, then, else, end
for, to, step, while, break, continue
function, return
struct
print
true, false
and, or, not, mod
```

---

## Example: Complete Program

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

## Transformation Examples

### From C-style:
```c
if (x > 0) {
    for (int i = 0; i < 10; i++) {
        printf("%d", i);
    }
}
```

### To MLP IR:
```mlp
if x > 0 then
    for i = 0 to 10
        print i
    end for
end if
```

---

### From Python-style:
```python
if x > 0:
    for i in range(10):
        print(i)
```

### To MLP IR:
```mlp
if x > 0 then
    for i = 0 to 10
        print i
    end for
end if
```

---

## Parser Notes

The parser for this IR is intentionally simple:

1. **No indentation sensitivity** - Whitespace for readability only
2. **Explicit keywords** - Every block has clear start/end
3. **Minimal state** - No complex context tracking
4. **Unambiguous** - One syntax, one meaning

This design ensures:
- Fast compilation
- Clear error messages
- Easy debugging
- Maintainable codebase

---

**Status:** Specification Complete ✓
**Next:** Implement transformer from multiple syntaxes to this IR
