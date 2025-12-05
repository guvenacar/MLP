# MLP (MELP) Complete Syntax Reference

**Version:** 1.0  
**Date:** December 4, 2025  
**Language:** English (PMLP - Pragmatic MLP Base Syntax)  
**Status:** Complete and Comprehensive

---

## Table of Contents

1. [Comments](#1-comments)
2. [Data Types](#2-data-types)
3. [Variables](#3-variables)
4. [Operators](#4-operators)
5. [Expressions](#5-expressions)
6. [Control Flow](#6-control-flow)
7. [Functions](#7-functions)
8. [Collections](#8-collections)
9. [Structures](#9-structures)
10. [Enumerations](#10-enumerations)
11. [Error Handling](#11-error-handling)
12. [Modules](#12-modules)
13. [Advanced Features](#13-advanced-features)
14. [Built-in Functions](#14-built-in-functions)
15. [Reserved Keywords](#15-reserved-keywords)

---

## 1. Comments

### 1.1 Single-Line Comments

```mlp
-- This is a single-line comment
numeric x = 10  -- End-of-line comment
```

**Syntax:**
- Starts with `--` (double hyphen)
- Continues to end of line
- Can appear at end of code line

### 1.2 Multi-Line Comments

```mlp
---
This is a multi-line comment
It can span multiple lines
---

numeric y = 20
```

**Syntax:**
- Starts with `---` (triple hyphen)
- Ends with `---` (triple hyphen)
- Cannot be nested

### 1.3 Documentation Comments

```mlp
--# Single-line documentation comment

---#
Multi-line documentation comment
Used for API documentation
#---

function calculate(numeric x) returns numeric
    return x * 2
end function
```

**Syntax:**
- Single-line: `--#`
- Multi-line: `---#` ... `---#`
- Used for generating documentation

---

## 2. Data Types

### 2.1 Primitive Types

MLP has three user-visible primitive types:

| Type | Description | Example | Internal Representation |
|------|-------------|---------|------------------------|
| `numeric` | All numbers (integers, decimals, big numbers) | `42`, `3.14`, `10^100` | int64/double/BigDecimal (TTO) |
| `text` or `string` | Text values | `"Hello"` | SSO/heap/rodata (TTO) |
| `boolean` | Logical values | `true`, `false` | 1 byte |

**Note:** MLP uses **Transparent Type Optimization (TTO)**:
- `numeric` internally selects int64, double, or BigDecimal
- `text` internally selects SSO (≤23 bytes), heap, or rodata
- User never sees or manages these distinctions

### 2.2 Nullable Types

```mlp
numeric? maybe_num = null
text? maybe_str = null

if maybe_num != null then
    print(maybe_num)
end if
```

**Syntax:**
- Type name followed by `?`
- Can hold `null` value
- Requires null check before use

### 2.3 Type Aliases

```mlp
type PersonId = numeric
type Name = text
type Callback = function(numeric) returns numeric

PersonId id = 12345
Name userName = "Alice"
```

**Syntax:**
- `type NewName = ExistingType`
- Creates alias for existing type
- Improves code readability

---

## 3. Variables

### 3.1 Variable Declaration

```mlp
-- Basic declaration with initialization
numeric x = 10
text name = "Alice"
boolean flag = true

-- Declaration without initialization (default values)
numeric y
text message
boolean status
```

**Rules:**
- Must declare before use
- Type specified explicitly
- Optional initialization

### 3.2 Multiple Variable Declaration

```mlp
-- Same type, multiple variables
numeric a, b, c = 1, 2, 3
text firstName, lastName = "John", "Doe"

-- Mixed types with automatic type inference
a, b, c = 10, "Hello", true
-- a is numeric, b is text, c is boolean
```

**Syntax:**
- Comma-separated variable names
- Values assigned in order
- Type inference supported for mixed declarations

### 3.3 State Variables

```mlp
-- Module-level state
state numeric counter = 100
state text appName = "MyApp"

-- Shared state across files
shared state numeric globalCounter = 1000

-- Using state variables
counter = counter + 1
print(counter)

if counter > 150 then
    print("Counter is high")
end if
```

**State Types:**
- `state`: Module/file scope global variable
- `shared state`: Cross-file shared global variable
- Stored in `.bss` section
- Accessible from all functions

### 3.4 Constants

```mlp
-- Constants use state keyword
state numeric PI = 3.14159
state text APP_VERSION = "1.0.0"

-- Constants cannot be reassigned (convention, not enforced)
-- PI = 3.14  -- Allowed by syntax, but violates convention
```

**Note:** MLP currently has no `const` keyword. Use `state` with uppercase naming convention.

---

## 4. Operators

### 4.1 Arithmetic Operators

| Operator | Description | Example | Result |
|----------|-------------|---------|--------|
| `+` | Addition | `5 + 3` | `8` |
| `-` | Subtraction | `5 - 3` | `2` |
| `*` | Multiplication | `5 * 3` | `15` |
| `/` | Division | `15 / 3` | `5` |
| `mod` or `%` | Modulo | `10 mod 3` | `1` |
| `**` | Exponentiation | `2 ** 3` | `8` |
| `-` | Unary minus | `-5` | `-5` |

**Precedence (high to low):**
1. Unary `-`
2. `**` (exponentiation)
3. `*`, `/`, `mod`
4. `+`, `-`

### 4.2 Comparison Operators

| Operator | Description | Example | Result |
|----------|-------------|---------|--------|
| `==` | Equal | `5 == 5` | `true` |
| `!=` | Not equal | `5 != 3` | `true` |
| `<` | Less than | `3 < 5` | `true` |
| `>` | Greater than | `5 > 3` | `true` |
| `<=` | Less or equal | `5 <= 5` | `true` |
| `>=` | Greater or equal | `5 >= 3` | `true` |

**Note:** All comparisons work with `numeric`, `text`, and `boolean`

### 4.3 Logical Operators

| Operator | Description | Example | Result |
|----------|-------------|---------|--------|
| `and` | Logical AND | `true and false` | `false` |
| `or` | Logical OR | `true or false` | `true` |
| `not` | Logical NOT | `not true` | `false` |

**Short-circuit Evaluation:**
- `and`: Returns false on first false, doesn't evaluate rest
- `or`: Returns true on first true, doesn't evaluate rest

### 4.4 Bitwise Operators

| Operator | Alternative | Description | Example |
|----------|------------|-------------|---------|
| `&` | `and` | Bitwise AND | `5 & 3` → `1` |
| `|` | `or` | Bitwise OR | `5 | 3` → `7` |
| `^` | `xor` | Bitwise XOR | `5 ^ 3` → `6` |
| `bnot` | - | Bitwise NOT | `bnot 5` |
| `shl` | - | Shift left | `5 shl 2` → `20` |
| `shr` | - | Shift right | `20 shr 2` → `5` |

**Note:** 
- Bitwise operators work only on integers
- Text versions are case-insensitive

### 4.5 Null Safety Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `??` | Null coalescing | `value ?? default` |
| `?.` | Optional chaining | `obj?.field?.subfield` |

**Null Coalescing:**
```mlp
numeric? x = null
numeric y = x ?? 10  -- y = 10 (x is null)

-- Chaining
numeric z = a ?? b ?? c ?? 999  -- First non-null value
```

**Optional Chaining:**
```mlp
Person? person = null
text? name = person?.name  -- name is null (person is null)

-- Chaining with fallback
text finalName = person?.name ?? "Unknown"
```

### 4.6 Operator Precedence (Complete)

From highest to lowest:

1. `()` - Parentheses
2. `.`, `?.`, `[]` - Access operators
3. `not`, `bnot`, `-` (unary) - Unary operators
4. `**` - Exponentiation
5. `*`, `/`, `mod` - Multiplication, division, modulo
6. `+`, `-` - Addition, subtraction
7. `shl`, `shr` - Bit shifts
8. `<`, `>`, `<=`, `>=` - Comparison
9. `==`, `!=` - Equality
10. `&`, `and` (bitwise) - Bitwise AND
11. `^`, `xor` - Bitwise XOR
12. `|`, `or` (bitwise) - Bitwise OR
13. `and` (logical) - Logical AND
14. `or` (logical) - Logical OR
15. `??` - Null coalescing
16. `=` - Assignment

---

## 5. Expressions

### 5.1 Arithmetic Expressions

```mlp
numeric result = (5 + 3) * 2 / 4 - 1  -- Result: 3
numeric power = 2 ** 8                 -- Result: 256
numeric remainder = 17 mod 5           -- Result: 2
```

### 5.2 Boolean Expressions

```mlp
boolean condition = (x > 10) and (y < 20)
boolean check = (status == "active") or (count > 0)
boolean negated = not flag
```

### 5.3 Ternary Operator

```mlp
numeric max = a > b ? a : b
text status = age >= 18 ? "adult" : "minor"
```

**Syntax:** `condition ? true_value : false_value`

### 5.4 String Expressions

```mlp
-- String concatenation (via function)
text full = concat("Hello", " ", "World")

-- String interpolation
text name = "Alice"
numeric age = 25
text message = $"Hello {name}, you are {age} years old"
```

---

## 6. Control Flow

### 6.1 If-Then-Else

```mlp
-- Simple if
if condition then
    -- code
end if

-- If-else
if condition then
    -- code when true
else
    -- code when false
end if

-- If-else if-else chain
if condition1 then
    -- code for condition1
else if condition2 then
    -- code for condition2
else if condition3 then
    -- code for condition3
else
    -- default code
end if
```

**Important Keywords:**
- `then` is required after condition
- `end if` (two words) closes the block

### 6.2 While Loop

```mlp
while condition
    -- code
    if break_condition then
        exit while
    end if
    if skip_condition then
        continue while
    end if
end while
```

**Control:**
- `exit while` - Exit loop immediately
- `continue while` - Skip to next iteration

### 6.3 Do-While Loop

```mlp
do
    -- code (executes at least once)
while condition
```

**Note:** 
- No `end do` keyword
- Condition follows `while` at the end

### 6.4 For Loop (Range-Based)

```mlp
-- Basic for loop (0 to 10 inclusive)
for i = 0 to 10
    print(i)
end for

-- For loop with step
for i = 0 to 100 step 10
    print(i)  -- 0, 10, 20, ..., 100
end for

-- Negative step (countdown)
for i = 10 to 0 step -1
    print(i)  -- 10, 9, 8, ..., 0
end for

-- Loop control
for i = 0 to 100
    if i == 50 then
        exit for  -- Exit loop
    end if
    if i mod 2 == 0 then
        continue for  -- Skip even numbers
    end if
    print(i)
end for
```

**Syntax:**
- `for variable = start to end`
- Optional `step increment`
- Default step is 1
- Range is inclusive on both ends

### 6.5 For-Each Loop (Iterator)

```mlp
-- Iterate over array
numeric[] numbers = [1, 2, 3, 4, 5]
for num in numbers
    print(num)
end for

-- Iterate using range() function
for i in range(5)
    print(i)  -- 0, 1, 2, 3, 4
end for

for i in range(2, 8)
    print(i)  -- 2, 3, 4, 5, 6, 7
end for

for i in range(0, 10, 2)
    print(i)  -- 0, 2, 4, 6, 8
end for

-- Iterate over generator
for value in fibonacci(10)
    print(value)
end for
```

**Range Function Signatures:**
- `range(end)` - Start at 0, step 1
- `range(start, end)` - Custom start, step 1
- `range(start, end, step)` - Full control

### 6.6 Switch-Case

```mlp
switch value
    case 1
        print("one")
    case 2, 3
        print("two or three")
    case 4 to 10
        print("four to ten")
    default
        print("other")
end switch
```

**Features:**
- Automatic break (no fall-through)
- Multiple values: `case 1, 2, 3`
- Range: `case 4 to 10`
- Default case

### 6.7 Pattern Matching

```mlp
-- Basic match
match value
    case 1 => print(100)
    case 2 => print(200)
    case 3 => print(300)
    case _ => print(999)
end match

-- Multiple values
match value
    case 1, 2, 3 => print("small")
    case 4, 5, 6 => print("medium")
    case _ => print("large")
end match

-- Range patterns
match value
    case 1 to 5 => print("range 1-5")
    case 6 to 10 => print("range 6-10")
    case _ => print("other")
end match
```

**Pattern Types:**
- Single value: `case 1`
- Multiple values: `case 1, 2, 3`
- Range: `case 1 to 10`
- Wildcard: `case _` (matches anything)

**Syntax:**
- `=>` (fat arrow) separates pattern from action
- No fall-through behavior
- Wildcard `_` should be last case

---

## 7. Functions

### 7.1 Function Declaration

```mlp
-- Function without parameters
function sayHello()
    print("Hello!")
end function

-- Function with parameters
function add(numeric a, numeric b) returns numeric
    return a + b
end function

-- Function with multiple return values
function minmax(numeric a, numeric b) returns numeric, numeric
    if a < b then
        return a, b
    else
        return b, a
    end if
end function

-- Using multiple return values
numeric min, max = minmax(10, 5)
print(min)  -- 5
print(max)  -- 10
```

**Syntax:**
- `function name(parameters) returns type`
- `returns` keyword for return type
- Multiple return types: comma-separated
- `end function` closes the block

### 7.2 Function Calls

```mlp
-- Simple call
sayHello()

-- Call with arguments
numeric result = add(5, 3)

-- Multiple return values
numeric x, y = minmax(100, 50)
```

### 7.3 Recursive Functions

```mlp
function factorial(numeric n) returns numeric
    if n <= 1 then
        return 1
    end if
    return n * factorial(n - 1)
end function

numeric result = factorial(5)  -- 120
```

### 7.4 Lambda Functions (Anonymous Functions)

```mlp
-- Lambda syntax: parameters => expression
lambda x => x * 2
lambda x, y => x + y

-- Using lambdas with arrays
numeric[] numbers = [1, 2, 3, 4, 5]
numeric[] doubled = map(numbers, lambda x => x * 2)
numeric[] filtered = filter(numbers, lambda x => x > 2)

-- Multi-line lambda
lambda x =>
    numeric square = x * x
    return square
end lambda
```

**Syntax:**
- Simple: `lambda params => expression`
- Multi-line: `lambda params => ... end lambda`
- Captures variables from outer scope (closure)

### 7.5 Closures

```mlp
function makeCounter() returns function() returns numeric
    numeric count = 0
    return lambda () =>
        count = count + 1
        return count
    end lambda
end function

-- Using closure
numeric counter = makeCounter()
print(counter())  -- 1
print(counter())  -- 2
print(counter())  -- 3
```

**Note:** Lambda captures variables from enclosing scope.

### 7.6 Generator Functions

```mlp
function fibonacci(numeric n) yields numeric
    numeric a = 0
    numeric b = 1
    for i = 0 to n
        yield a
        numeric temp = a
        a = b
        b = temp + b
    end for
end function

-- Using generator
for num in fibonacci(10)
    print(num)  -- 0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55
end for
```

**Syntax:**
- `function name(params) yields type`
- `yield value` returns value without exiting
- Use in `for-in` loops

### 7.7 Nested Functions

```mlp
function outer(numeric x)
    numeric outerVar = x * 2
    
    function inner(numeric y) returns numeric
        return outerVar + y  -- Access outer variable
    end function
    
    return inner(10)
end function
```

**Note:** Inner functions can access outer function variables.

### 7.8 Async Functions (Basic)

```mlp
async function fetchData() returns text
    text data = await httpGet("https://api.example.com")
    return data
end function

-- Using async
text result = await fetchData()
```

**Syntax:**
- `async function` declares async function
- `await` keyword waits for async result
- Returns promise/future type

---

## 8. Collections

### 8.1 Arrays (Homogeneous, Mutable)

```mlp
-- Array declaration
numeric[] numbers = [1, 2, 3, 4, 5]
text[] names = ["Alice", "Bob", "Charlie"]

-- Empty array
numeric[] empty = []

-- Array access
numbers[0] = 100        -- Set element
numeric first = numbers[0]  -- Get element

-- Array operations
numeric length = length(numbers)
array_push(numbers, 6)     -- Add to end
numeric last = array_pop(numbers)  -- Remove from end
```

**Syntax:**
- `Type[] name = [element1, element2, ...]`
- Square brackets `[]` for type and literals
- Zero-based indexing
- All elements must be same type

### 8.2 Lists (Heterogeneous, Mutable)

```mlp
-- List declaration
person() = ("Alice", 25, true, 3.14)

-- Empty list
empty() = ()

-- List access (runtime type checking required)
text name = person(0)      -- "Alice"
numeric age = person(1)    -- 25
boolean active = person(2) -- true

-- List modification
person(0) = "Bob"          -- Change element
person.add("new item")     -- Add element
person.remove(0)           -- Remove element

-- Type-safe access
if type_of(person(0)) == "text" then
    text userName = person(0) as text
end if
```

**Important Syntax Rules (VB.NET + Pascal Hybrid):**

1. **No Space Between Name and Parenthesis:**
   ```mlp
   list(2)    -- ✅ Correct: index access
   list (2)   -- ❌ Error: space not allowed
   ```

2. **Semicolon Separator + Trailing Semicolon Required:**
   ```mlp
   list() = (10; 20; 30;)   -- ✅ Correct: 3 elements
   list() = (10; 20; 30)    -- ❌ Error: missing trailing semicolon
   list() = (42;)           -- ✅ Correct: 1 element
   list() = (42)            -- ❌ Error: missing trailing semicolon
   list() = ()              -- ✅ Correct: empty list
   list() = (1, 2, 3)       -- ❌ Error: use semicolon, not comma
   ```

3. **Turkish Decimal Format Support:**
   ```mlp
   -- Comma as decimal separator (Turkish format)
   list() = (10,5; 20,3; 100,99;)   -- ✅ OK: [10.5, 20.3, 100.99]
   ```

4. **Normal Variable vs List Variable:**
   ```mlp
   -- Normal (scalar) variable
   a = 5           -- ✅ OK
   a = ()          -- ❌ Error: empty parens meaningless for scalar
   a = (5)         -- ✅ OK: math expression, a = 5
   a = (1; 2; 3;)  -- ❌ Error: use a() for lists
   
   -- List variable
   b() = ()        -- ✅ OK: empty list
   b() = (1;)      -- ✅ OK: 1-element list
   b() = (1; 2; 3;)-- ✅ OK: 3-element list
   b() = 5         -- ❌ Error: list literal required
   ```

5. **Mathematical Parentheses:**
   ```mlp
   num = (8)          -- ✅ OK: num = 8 (math)
   num = (7+3+6)      -- ✅ OK: num = 16 (math)
   x = a * (3 + 2)    -- ✅ OK: a × 5 (space = math)
   x = a(3 + 2)       -- Index access: a[5] (no space)
   ```

### 8.3 Tuples (Heterogeneous, Immutable)

```mlp
-- Tuple declaration
coordinates{} = {10, 20, "point"}
color{} = {255, 128, 0}

-- Tuple access (read-only)
print(coordinates(0))  -- 10
print(coordinates(2))  -- "point"

-- ❌ Tuples are immutable!
coordinates(0) = 99       -- ❌ Error: cannot modify
coordinates.add(5)        -- ❌ Error: cannot add elements

-- Function returning tuple (fast, stack-allocated)
function minmax(numeric[] arr) returns {numeric, numeric}
    return {min(arr), max(arr)}
end function

result{} = minmax([3, 1, 4, 1, 5])
print(result(0))  -- 1 (min)
print(result(1))  -- 5 (max)

-- Tuple destructuring
{min_val, max_val} = minmax([3, 1, 4, 1, 5])
print(min_val)    -- 1
print(max_val)    -- 5

-- Swap using tuples
{a, b} = {b, a}
```

**Syntax:**
- Curly braces `{}` for tuple type and literals
- Comma separator (not semicolon)
- Immutable - cannot change after creation
- Stack-allocated (fast, no heap allocation)
- Used for function multiple returns
- **Note:** `{}` for tuples, `<>` reserved for generics only

### 8.4 Collection Comparison

| Feature | Array `[]` | List `()` | Tuple `{}` |
|---------|-----------|-----------|------------|
| **Syntax** | `Type[] name = [...]` | `name() = (...;)` | `name{} = {...}` |
| **Element Types** | Homogeneous | Heterogeneous | Heterogeneous |
| **Mutable** | Yes | Yes | No |
| **Separator** | `,` comma | `;` semicolon | `,` comma |
| **Trailing Separator** | Optional | **Required** | Optional |
| **Memory** | Stack/Heap | Heap | Stack (fast) |
| **Access Syntax** | `arr[i]` | `list(i)` | `tuple(i)` |
| **Use Case** | Same-type collections | Mixed-type collections | Immutable records, function returns |

---

## 9. Structures

### 9.1 Struct Definition

```mlp
struct Person
    text name
    numeric age
    boolean active
end struct
```

**Syntax:**
- `struct Name` ... `end struct`
- Field declarations inside
- No methods (functions separate)

### 9.2 Struct Usage

```mlp
-- Create struct instance
Person person
person.name = "Alice"
person.age = 30
person.active = true

-- Access fields
print(person.name)  -- "Alice"
print(person.age)   -- 30
```

### 9.3 Nested Structs

```mlp
struct Address
    text street
    text city
end struct

struct Person
    text name
    Address address
end struct

-- Usage
Person person
person.name = "Alice"
person.address.street = "123 Main St"
person.address.city = "New York"

print(person.address.city)  -- "New York"
```

### 9.4 Generic Structs

```mlp
-- Generic struct definition
struct Box<T>
    T value
end struct

-- Usage
Box<numeric> numBox
numBox.value = 42

Box<text> textBox
textBox.value = "Hello"
```

---

## 10. Enumerations

### 10.1 Enum Definition

```mlp
enum Status
    Pending = 0
    Active = 1
    Completed = 2
end enum
```

**Syntax:**
- `enum Name` ... `end enum`
- Values assigned explicitly
- Default values: 0, 1, 2, ...

### 10.2 Enum Usage

```mlp
Status current = Status.Active

if current == Status.Active then
    print("Status is active")
end if

switch current
    case Status.Pending
        print("Waiting...")
    case Status.Active
        print("Running...")
    case Status.Completed
        print("Done!")
end switch
```

---

## 11. Error Handling

### 11.1 Try-Catch-Finally

```mlp
try
    numeric result = riskyOperation()
    print(result)
catch error
    print($"Error occurred: {error}")
finally
    cleanup()
end try
```

**Syntax:**
- `try` ... `catch variable` ... `finally` ... `end try`
- `catch` block handles errors
- `finally` block always executes
- `finally` is optional

### 11.2 Multiple Catch Blocks

```mlp
try
    performOperation()
catch NetworkError e
    print("Network error: " + e.message)
catch FileError e
    print("File error: " + e.message)
catch error
    print("Unknown error: " + error)
finally
    closeResources()
end try
```

### 11.3 Throw

```mlp
function divide(numeric a, numeric b) returns numeric
    if b == 0 then
        throw "Division by zero"
    end if
    return a / b
end function

-- Custom exceptions
throw NetworkError("Connection timeout")
```

**Syntax:**
- `throw message` or `throw ExceptionObject`
- Propagates up call stack
- Can be caught by `catch` block

---

## 12. Modules

### 12.1 Import

```mlp
-- Import file
import "math_utils.mlp"

-- Import module
import math
import string as str

-- Using imported functions
numeric result = math.sqrt(16)
text upper = str.to_upper("hello")
```

**Syntax:**
- `import "filename.mlp"` - Import specific file
- `import moduleName` - Import module
- `import moduleName as alias` - Import with alias

### 12.2 Module Definition

```mlp
module MathUtils
    -- Exported function (public)
    export function add(numeric a, numeric b) returns numeric
        return a + b
    end function
    
    export function multiply(numeric a, numeric b) returns numeric
        return a * b
    end function
    
    -- Private function (not exported)
    function helper() returns numeric
        return 42
    end function
end module
```

**Syntax:**
- `module Name` ... `end module`
- `export` keyword for public functions/structs
- Non-exported items are private

### 12.3 Export

```mlp
-- Export function
export function publicFunction()
    -- code
end function

-- Export struct
export struct Point
    numeric x
    numeric y
end struct

-- Export constant
export state numeric MAX_SIZE = 1000
```

---

## 13. Advanced Features

### 13.1 Generic Functions

```mlp
-- Generic function definition
function identity<T>(T value) returns T
    return value
end function

-- Usage
numeric x = identity<numeric>(42)
text s = identity<text>("hello")
```

**Syntax:**
- `function name<T>(params) returns T`
- Type parameter in angle brackets
- Can have multiple type parameters: `<T, U, V>`

### 13.2 Generic Type Constraints

```mlp
function maximum<T: Comparable>(T a, T b) returns T
    if a > b then
        return a
    else
        return b
    end if
end function
```

**Syntax:**
- `<T: Constraint>` specifies constraint
- Type must satisfy constraint interface

### 13.3 Operator Overloading

```mlp
-- Define struct
struct Vector
    numeric x
    numeric y
end struct

-- Overload + operator
operator +(Vector a, Vector b) returns numeric
    return a.x + b.x + a.y + b.y
end operator

-- Overload - operator
operator -(Vector a, Vector b) returns numeric
    return a.x - b.x + a.y - b.y
end operator

-- Usage
Vector v1
v1.x = 10
v1.y = 20

Vector v2
v2.x = 5
v2.y = 15

numeric sum = v1 + v2   -- Calls operator+(v1, v2) -> 50
numeric diff = v1 - v2  -- Calls operator-(v1, v2) -> 10
```

**Supported Operators:**
- Arithmetic: `+`, `-`, `*`, `/`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`

**Syntax:**
- `operator symbol(Type a, Type b) returns ResultType`
- Must be binary operator (two parameters)
- Only for struct types

### 13.4 Pointers

```mlp
-- Pointer declaration
numeric* ptr

-- Address-of operator
numeric x = 10
ptr = &x

-- Dereference operator
numeric value = *ptr  -- value = 10

-- Pointer arithmetic
numeric* next = ptr + 1
numeric* prev = ptr - 1

-- Null pointer
numeric* nullPtr = null
if nullPtr == null then
    print("Pointer is null")
end if
```

**Operators:**
- `&` - Address-of (get pointer to variable)
- `*` - Dereference (get value from pointer)
- `+`, `-` - Pointer arithmetic

### 13.5 Defer Statement

```mlp
function processFile(text filename)
    File f = openFile(filename)
    defer closeFile(f)  -- Executed at function exit
    
    -- Process file
    text content = readFile(f)
    
    -- closeFile(f) automatically called here
end function
```

**Syntax:**
- `defer statement`
- Executes statement at function exit
- Useful for cleanup (close files, free memory)
- Multiple defers execute in LIFO order

### 13.6 Interfaces and Traits

```mlp
-- Interface definition
interface Drawable
    function draw()
end interface

-- Struct implementing interface
struct Circle implements Drawable
    numeric radius
    
    function draw()
        print("Drawing circle")
    end function
end struct
```

**Syntax:**
- `interface Name` ... `end interface`
- `struct Name implements Interface` ... `end struct`
- Must implement all interface methods

---

## 14. Built-in Functions

### 14.1 I/O Functions

| Function | Description | Example |
|----------|-------------|---------|
| `print(value)` | Print to console | `print("Hello")` |
| `print(value1, value2, ...)` | Print multiple values | `print("Age:", 25)` |
| `input()` | Read line from console | `text line = input()` |
| `input(prompt)` | Read with prompt | `text name = input("Name: ")` |

### 14.2 String Functions

| Function | Description | Example |
|----------|-------------|---------|
| `str_length(s)` | String length | `str_length("Hello")` → `5` |
| `concat(s1, s2, ...)` | Concatenate strings | `concat("Hello", " ", "World")` |
| `substring(s, start, len)` | Extract substring | `substring("Hello", 0, 2)` → `"He"` |
| `indexOf(s, search)` | Find substring | `indexOf("Hello", "ll")` → `2` |
| `charAt(s, index)` | Get character | `charAt("Hello", 0)` → `"H"` |
| `to_upper(s)` | Convert to uppercase | `to_upper("hello")` → `"HELLO"` |
| `to_lower(s)` | Convert to lowercase | `to_lower("HELLO")` → `"hello"` |
| `trim(s)` | Remove whitespace | `trim("  hi  ")` → `"hi"` |
| `split(s, delim)` | Split string | `split("a,b,c", ",")` → `["a","b","c"]` |
| `replace(s, old, new)` | Replace substring | `replace("hello", "l", "L")` → `"heLLo"` |

### 14.3 Type Conversion

| Function | Description | Example |
|----------|-------------|---------|
| `to_numeric(s)` | String to number | `to_numeric("42")` → `42` |
| `to_string(n)` | Number to string | `to_string(42)` → `"42"` |
| `to_boolean(v)` | Convert to boolean | `to_boolean(1)` → `true` |

### 14.4 Math Functions

| Function | Description |
|----------|-------------|
| `abs(x)` | Absolute value |
| `floor(x)` | Round down |
| `ceil(x)` | Round up |
| `round(x)` | Round to nearest |
| `sqrt(x)` | Square root |
| `pow(x, y)` | Power (x^y) |
| `sin(x)`, `cos(x)`, `tan(x)` | Trigonometric functions |
| `log(x)`, `log10(x)` | Logarithms |
| `exp(x)` | Exponential (e^x) |
| `min(a, b)`, `max(a, b)` | Minimum, maximum |
| `random()` | Random number [0, 1) |

### 14.5 Array Functions

| Function | Description |
|----------|-------------|
| `length(arr)` | Array length |
| `array_push(arr, val)` | Add to end |
| `array_pop(arr)` | Remove from end |
| `array_get(arr, idx)` | Get element |
| `array_set(arr, idx, val)` | Set element |
| `array_resize(arr, size)` | Resize array |

### 14.6 List Functions

| Function | Description |
|----------|-------------|
| `list.add(val)` | Add element to list |
| `list.remove(idx)` | Remove element at index |
| `list.get(idx)` | Get element at index |
| `list.size()` | Get list size |
| `list.clear()` | Remove all elements |

### 14.7 File I/O Functions

| Function | Description |
|----------|-------------|
| `read_file(path)` | Read entire file |
| `write_file(path, content)` | Write to file |
| `append_file(path, content)` | Append to file |
| `file_exists(path)` | Check if file exists |

### 14.8 System Functions

| Function | Description |
|----------|-------------|
| `get_argc()` | Get argument count |
| `get_argv(index)` | Get argument by index |
| `exit(code)` | Exit program with code |
| `assert(condition, message)` | Assert condition |

### 14.9 Memory Management

| Function | Description |
|----------|-------------|
| `gc_collect()` | Trigger garbage collection |
| `gc_full_collect()` | Full GC cycle |
| `gc_get_object_count()` | Get tracked object count |
| `gc_get_total_bytes()` | Get total allocated bytes |

### 14.10 Range Function

| Signature | Description | Example |
|-----------|-------------|---------|
| `range(end)` | 0 to end-1, step 1 | `range(5)` → `0,1,2,3,4` |
| `range(start, end)` | start to end-1, step 1 | `range(2, 7)` → `2,3,4,5,6` |
| `range(start, end, step)` | start to end-1, custom step | `range(0, 10, 2)` → `0,2,4,6,8` |

---

## 15. Reserved Keywords

### 15.1 Complete Keyword List

**Type Keywords:**
- `numeric` - Numeric type
- `text`, `string` - Text type
- `boolean` - Boolean type

**Control Flow:**
- `if`, `then`, `else`, `end if`
- `while`, `end while`
- `do`, `while` (do-while)
- `for`, `to`, `step`, `in`, `end for`
- `switch`, `case`, `default`, `end switch`
- `match`, `end match`

**Function Keywords:**
- `function`, `end function`
- `return`, `returns`
- `lambda`, `end lambda`
- `yield`, `yields`
- `async`, `await`

**Data Structure Keywords:**
- `struct`, `end struct`
- `enum`, `end enum`
- `type` (type alias)
- `interface`, `implements`, `end interface`

**Module Keywords:**
- `import`, `module`, `export`, `as`, `end module`

**Error Handling:**
- `try`, `catch`, `finally`, `end try`
- `throw`

**State Keywords:**
- `state`, `shared`

**Control Keywords:**
- `exit` - Exit block (exit for, exit while, etc.)
- `continue` - Continue to next iteration
- `break` - Break out of loop (alternative to exit)

**Operators:**
- `and`, `or`, `not` - Logical operators
- `mod` - Modulo
- `band`, `bor`, `bxor`, `bnot` - Bitwise operators
- `shl`, `shr` - Shift operators

**Special Keywords:**
- `defer` - Defer statement execution
- `operator` - Operator overloading

**Literals:**
- `true`, `false` - Boolean literals
- `null` - Null literal

**Debug Keywords:**
- `debug` - Debug mode operations
- `goto` - Jump to label
- `pause` - Debug pause
- `label` - Define label
- `stop` - Stop program

---

## Appendix A: Quick Reference

### A.1 Comment Styles
```mlp
-- Single line
--- Multi-line ---
--# Doc comment
---# Doc multi-line #---
```

### A.2 Variable Declaration
```mlp
numeric x = 10
text name = "Alice"
boolean flag = true
numeric a, b, c = 1, 2, 3
state numeric counter = 0
```

### A.3 Collections
```mlp
numeric[] array = [1, 2, 3]
list() = (1; 2; 3;)
tuple{} = {1, 2, 3}
```

### A.4 Control Flow
```mlp
if cond then ... end if
while cond ... end while
do ... while cond
for i = 0 to 10 ... end for
for item in collection ... end for
switch val ... case ... end switch
match val ... case ... end match
```

### A.5 Functions
```mlp
function name(params) returns type
    return value
end function

lambda x => x * 2

function gen() yields type
    yield value
end function
```

### A.6 Error Handling
```mlp
try
    -- code
catch error
    -- handle
finally
    -- cleanup
end try
```

---

## Appendix B: Syntax Inconsistencies Found

During documentation creation, the following inconsistencies were found between source documents:

### B.1 List Syntax Conflict
- **kurallar_kitabı.md**: `()` with `;` separator, trailing `;` required → `(1; 2; 3;)`
- **MLP_LANGUAGE_SPEC.md**: `()` with `,` separator → `("Ali", 10, true)`
- **Resolution**: Used kurallar_kitabı.md as authoritative (it has detailed syntax rules)

### B.2 Tuple Syntax Conflict
- **kurallar_kitabı.md**: `<>` angle brackets → `<10, 20, "point">`
- **MLP_LANGUAGE_SPEC.md**: `{}` curly braces → `{10, 20, "point"}`
- **Resolution**: **Changed to `{}` curly braces** to avoid conflict with generics `<T>`
- **Reason**: Angle brackets `<>` are reserved for generic type parameters

### B.3 Multi-line Comment Syntax
- **kurallar_kitabı.md**: `---` block (start and end with `---`)
- **MLP_LANGUAGE_SPEC.md**: Similar but less detailed
- **Resolution**: Used kurallar_kitabı.md format

### B.4 Recommendations
- ✅ Standardized list syntax: `()` with `;` separator
- ✅ **RESOLVED:** Tuple syntax changed to `{}` to avoid generic conflict
- ✅ Multi-line comment syntax: `---` blocks
- 📝 Update source documents (kurallar_kitabı.md, MLP_LANGUAGE_SPEC.md) to reflect `{}` tuple syntax

---

**Document Status:** Complete  
**Conflicts Noted:** Yes (see Appendix B)  
**Priority Authority:** kurallar_kitabı.md  
**Created:** December 4, 2025
