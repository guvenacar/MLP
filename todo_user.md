# MELP/MELP-DECL - Keyword Roadmap

**Amaç:** Basit deklarasyondan modern tam özellikli dile kademeli geçiş

---

## 🎯 Turing Complete Minimum Set

Bir dil Turing complete olması için minimum gereksinimler:

### 1. ✅ Variables (Phase 0)
```decl
numeric x
```

### 2. ⏳ Assignment (Phase 1)
```decl
numeric x = 5
x = 10
```

### 3. ⏳ Arithmetic (Phase 1)
```decl
numeric result = a + b * c
```

### 4. ⏳ Conditional (Phase 2)
```decl
if x > 5 then
    print("büyük")
end if

tek satır if:
if a = b then c=10 else d=10 

```

### 5. ⏳ Loop (Phase 3)
```decl
while x < 10
    x = x + 1
end while
```

**✅ Bu 5 özellik = Turing Complete!**

---

## 📊 Phase Roadmap

### Phase 0: Minimal ✅ TAMAMLANDI
```decl
numeric a
decimal b
boolean c

a,b,c = 5, "Ali", false -- akıllı değişken tanımlama var.
veya 
numeric a, string b, boolean c = 5, "Ali", false


```

### Phase 1: Expressions ⏳ DEVAM EDİYOR
```decl
-- Comments (tek satır)
---
Multi-line
comments
---

numeric a = 5
numeric b = a + 10 * 2
string name = "Ali"
print(a)
print(name)

-- String line continuation
string long = "Bu uzun \
bir string"
```

**Keywords:**
- `=` (assignment)
- `+`, `-`, `*`, `/` (arithmetic)
- `print()` (output)
- `--` (single-line comment)
- `---` (multi-line comment)
- `\` (line continuation in strings)

### Phase 2: Control Flow
```decl
if x > 5 then
    print("büyük")
else if x == 5 then
    print("eşit")
else
    print("küçük")
end if
```

**Keywords:**
- `if`, `then`, `else if`, `else`, `end if`
- `>`, `<`, `>=`, `<=`, `==`, `!=` (comparison)
- `and`, `or`, `not` (logical)

### Phase 3: Loops
```decl
-- For loop
for i = 0 to 10
    print(i)
end for

for i = 0 to 20 step 2
    print(i)
end for

-- While loop
while x < 100
    x = x * 2
end while

-- Loop control (Smart exit - Hibrit sistem)
for i = 0 to 100
    if i == 3 then
        continue      -- Iteration atla, döngüye devam
    end if
    
    if i == 50 then
        exit          -- for'dan çık (if atlanır - smart!)
    end if
    
    -- exit if: Gereksiz else'den kurtarır! (Early return pattern)
    if i == 60 then
        exit if       -- if'ten çık, for devam eder
    end if
    -- buraya gelir (gereksiz else yok!)
    
    print(i)
end for
-- exit veya exit for buraya getirir

-- Nested loops
for i = 0 to 10
    for j = 0 to 10
        if j == 5 then
            exit       -- İçteki for'dan çık (smart)
        end if
        
        if i == 5 and j == 3 then
            exit for   -- Explicit: içteki for'dan çık (aynı)
        end if
    end for
end for

-- Debug Features (Production'da silinir)
debug mylabel              -- Label tanımı
numeric x = 0
x = x + 1

debug pause                -- Breakpoint (debugger durur)

if x < 10 then
    debug goto mylabel     -- Debug-only jump
end if

debug if x < 0 then        -- Debug-only block
    print(999)
    debug goto mylabel
debug end if
```

**Keywords:**
- `for`, `to`, `step`, `end for`
- `while`, `end while`
- `exit` (smart: loop/function'dan çık, if atlanır)
- `exit for`, `exit while` (explicit loop control)
- `exit if` (explicit conditional control - benzersiz!)
- `continue` (iteration atla)
- `break` → `exit` (C/Java/Python normalization)
- `debug label` (debug-only label, production'da silinir)
- `debug goto label` (debug-only jump, production'da silinir)
- `debug pause` (breakpoint, production'da silinir)

### Phase 4: Functions
```decl
function add(numeric a, numeric b)
    return a + b
end function

function greet(string name)
    print("Merhaba " + name)
end function

-- Function call
numeric result = add(5, 10)
greet("Ali")
```

**Keywords:**
- `function`, `end function`
- `return`
- `exit function` (early return)

### Phase 5: Data Structures
```decl
-- Structs
struct Person then
    string name
    numeric age
end struct

Person p
p.name = "Ali"
p.age = 25

-- Lists
numeric list() numbers = list()
numbers.add(10)
numbers.add(20)
numeric first = numbers.get(0)
numeric count = numbers.size()
```

**Keywords:**
- `struct`, `end struct`
- `list()` (constructor)
- `.` (member access)

---

## 🚀 Modern Language Features

### Phase 6: Advanced Control
```decl
-- Switch/Match
switch value
    case 1
        print("bir")
    case 2
        print("iki")
    default
        print("diğer")
end switch

-- Do-while
do
    x = x + 1
while x < 10

-- Exit anywhere (VB.NET style)
exit if
exit switch
exit function
stop  -- exit program
```

**Keywords:**
- `switch`, `case`, `default`, `end switch`
- `do`, `while` (do-while variant)
- `exit if`, `exit switch`, `stop`

### Phase 7: Advanced Functions
```decl
-- Optional parameters
function greet(string name, string title = "Mr.")
    print(title + " " + name)
end function

-- Multiple return
function minmax(numeric a, numeric b)
    if a < b then
        return a, b
    else
        return b, a
    end if
end function

numeric min, max = minmax(10, 5)

-- Lambda/Anonymous functions
numeric list() numbers = list(1, 2, 3, 4, 5)
numbers.map(lambda x then return x * 2)
```

**Keywords:**
- `lambda`, `then`
- Default parameters (implicit)

### Phase 8: Error Handling
```decl
try
    numeric result = divide(10, 0)
catch error as e
    print("Hata: " + e.message)
finally
    print("Temizlik")
end try
```

**Keywords:**
- `try`, `catch`, `finally`, `end try`
- `throw` (raise exception)
- `as` (type alias)

### Phase 9: Modules & Imports
```decl
-- Import
import math
import string as str

-- Use
numeric result = math.sqrt(16)
string upper = str.uppercase("hello")

-- Module definition
module MyModule
    function helper()
        return 42
    end function
end module
```

**Keywords:**
- `import`, `as`
- `module`, `end module`
- `export` (public)
- `private` (internal)

### Phase 10: Advanced Types
```decl
-- Nullable types
numeric? maybe = null
if maybe != null then
    print(maybe)
end if

-- Type alias
type PersonId = numeric
type Result = numeric | string  -- Union type

-- Enums
enum Status
    Pending = 0
    Active = 1
    Done = 2
end enum

Status current = Status.Active
```

**Keywords:**
- `null`
- `type`
- `enum`, `end enum`
- `|` (union type)
- `?` (nullable suffix)

### Phase 11: Advanced OOP (Optional)
```decl
-- Classes
class Animal
    string name
    
    function init(string n)
        name = n
    end function
    
    function speak()
        print(name + " makes a sound")
    end function
end class

class Dog extends Animal
    function speak()
        print(name + " barks")
    end function
end class

-- Interface
interface Drawable
    function draw()
end interface
```

**Keywords:**
- `class`, `end class`
- `extends` (inheritance)
- `interface`, `end interface`
- `implements`
- `this`, `super`
- `new` (constructor call)

### Phase 12: Concurrency
```decl
-- Async functions
async function fetchData()
    string data = await http.get("api.example.com")
    return data
end function

-- Parallel
parallel
    task1()
    task2()
    task3()
end parallel
```

**Keywords:**
- `async`, `await`
- `parallel`, `end parallel`
- `spawn` (thread/goroutine)

---

## 📝 Complete Keyword List (Alphabetical)

### Core (Turing Complete)
- `=` - assignment
- `+`, `-`, `*`, `/` - arithmetic
- `if`, `then`, `else`, `end if` - conditional
- `while`, `end while` - loop
- `numeric`, `decimal`, `boolean`, `string` - types

### Control Flow
- `for`, `to`, `step`, `end for`
- `switch`, `case`, `default`, `end switch`
- `do`, `while`
- `exit for`, `exit while`, `exit if`, `exit function`, `exit switch`
- `stop`

### Operators
- `>`, `<`, `>=`, `<=`, `==`, `!=` - comparison
- `and`, `or`, `not` - logical
- `.` - member access
- `?` - nullable
- `|` - union type

### Functions & Structures
- `function`, `end function`
- `return`
- `struct`, `end struct`
- `lambda`

### Collections
- `list()` - list constructor
- `.add()`, `.get()`, `.size()` - list methods

### Error Handling
- `try`, `catch`, `finally`, `end try`
- `throw`
- `as`

### Modules
- `import`
- `module`, `end module`
- `export`, `private`

### Advanced Types
- `null`
- `type`
- `enum`, `end enum`

### OOP (Optional)
- `class`, `end class`
- `extends`, `implements`
- `interface`, `end interface`
- `this`, `super`
- `new`

### Concurrency (Optional)
- `async`, `await`
- `parallel`, `end parallel`
- `spawn`

### Comments
- `--` - single-line comment
- `---` ... `---` - multi-line comment
- `\` - line continuation

### I/O
- `print()` - output
- `input()` - input (future)

---

## 🎯 Priority Order

1. **✅ Phase 0-1:** Variables, assignment, arithmetic, print → **Practical scripts**
2. **Phase 2-3:** if/for/while → **Turing complete**
3. **Phase 4-5:** Functions, structs → **Real programs**
4. **Phase 6+:** Advanced features → **Modern language**

---

**Strateji:** Her phase self-hosting olmalı!  
**İlke:** Compiler her phase'de kendi dilinde yeniden yazılır.

**Son Güncelleme:** 28 Kasım 2025  
**Mevcut Phase:** 1 (Assignment & Expressions)
