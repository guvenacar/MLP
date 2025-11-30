# MELP/MELP-DECL - Keyword Roadmap

**Amaç:** Basit deklarasyondan modern tam özellikli dile kademeli geçiş

**Durum:** Phase 19 TAMAMLANDI - Garbage Collection 🎉

**Son Güncelleme:** 30 Kasım 2025

---

## 🚨 SELF-HOSTING ÖNCESİ YAPILACAKLAR (YENİ PLANLAMA)

**ÖNEMLİ:** Aşağıdaki özellikler self-hosting compiler yazılmadan ÖNCE tamamlanmalıdır!

### 📌 Öncelik 0: Transparent Type Optimization (TTO) - KRİTİK

| # | Özellik | Zorluk | Süre | Durum | Açıklama |
|---|---------|--------|------|-------|----------|
| 0.1 | **Numeric TTO** | Orta | 1 gün | ⏳ | int64 → double → BigDecimal otomatik seçim |
| 0.2 | **Text TTO (SSO)** | Orta | 1 gün | ⏳ | ≤23 byte inline, >23 byte heap |
| 0.3 | **Overflow Handling** | Orta | 0.5 gün | ⏳ | int64 taşarsa BigDecimal'e promote |
| 0.4 | **Tip-aware Codegen** | Orta | 0.5 gün | ⏳ | Her tip için optimize assembly |

**TTO Toplam:** ~3 gün

### 📌 Öncelik 1: Temel Fonksiyonlar

| # | Özellik | Zorluk | Süre | Durum | Açıklama |
|---|---------|--------|------|-------|----------|
| 1 | **String ↔ Number dönüşümü** | Kolay | 1 saat | ⏳ | `to_numeric("42")`, `to_text(42)` |
| 2 | **Input fonksiyonu** | Kolay | 1 saat | ⏳ | `input()`, `input("prompt: ")` |
| 3 | **Math kütüphanesi** | Kolay | 2-3 saat | ⏳ | sin, cos, sqrt, pow, abs, floor, ceil, round |
| 4 | **String fonksiyonları** | Kolay | 2 saat | ⏳ | split, trim, replace, to_upper, to_lower |
| 5 | **Bitwise operatörler** | Kolay | 2-3 saat | ⏳ | band, bor, bxor, bnot, shl, shr |
| 6 | **Assert** | Kolay | 1 saat | ⏳ | assert(condition, "message") |
| 7 | **Union Types** | Orta | 1-2 gün | ⏳ | `type Result = numeric \| text` |

**Fonksiyonlar Toplam:** ~2-3 gün

### 🎯 Toplam Tahmini Süre: 5-6 gün

### 📝 Önerilen Sıra:
1. **TTO Faz 1** - Numeric (int64/double/BigDecimal)
2. **TTO Faz 2** - Text (SSO)
3. **String ↔ Number** - Lexer için kritik
4. **Input** - Test için gerekli
5. **Math** - Yaygın kullanım
6. **String fonksiyonları** - Lexer parsing için
7. **Bitwise** - Low-level
8. **Assert** - Debug
9. **Union Types** - En karmaşık

**Not:** Class/Inheritance EKLENMEYECEK - struct+functions yaklaşımı kullanılıyor.

---

## 📊 MELP Özellik Durumu Tablosu

| Özellik | Durum | Açıklama |
|---------|-------|----------|
| **Temel Özellikler** | | |
| Değişkenler (numeric, decimal, boolean, text) | ✅ | Tam çalışıyor |
| Aritmetik İşlemler (+, -, *, /, mod) | ✅ | Tam çalışıyor |
| Karşılaştırma (==, !=, <, >, <=, >=) | ✅ | Tam çalışıyor |
| Mantıksal (and, or, not) | ✅ | Tam çalışıyor |
| Print Statement | ✅ | Tam çalışıyor |
| Yorum Satırları (--, ---) | ✅ | Tam çalışıyor |
| **Kontrol Yapıları** | | |
| If/Else/Else If | ✅ | Tam çalışıyor |
| While Döngüsü | ✅ | Tam çalışıyor |
| Do-While Döngüsü | ✅ | Tam çalışıyor |
| For Döngüsü (to/step) | ✅ | Tam çalışıyor |
| For-In Döngüsü | ✅ | Phase 14 |
| Switch-Case | ✅ | Range matching dahil |
| Exit/Continue | ✅ | Tam çalışıyor |
| **Fonksiyonlar** | | |
| Fonksiyon Tanımı (func) | ✅ | Tam çalışıyor |
| Return Statement | ✅ | Çoklu return dahil |
| Lambda/Closure | ✅ | Tam çalışıyor |
| Generator (yields/yield) | ✅ | Phase 14 |
| Async/Await | ✅ | Temel destek |
| **Veri Yapıları** | | |
| Arrays (Static) | ✅ | [1,2,3] literal desteği |
| Arrays (Dynamic) | ✅ | malloc/resize/free |
| Structs | ✅ | Field erişimi çalışıyor |
| Enums | ✅ | Tam çalışıyor |
| Pointers | ✅ | &var, *ptr |
| **Gelişmiş Özellikler** | | |
| Generics | ✅ | Phase 13 - func<T>, struct<T> |
| Exception Handling | ✅ | try/catch/throw |
| Module System | ✅ | import desteği |
| Interface | ✅ | Tam çalışıyor |
| Type Alias | ✅ | Tam çalışıyor |
| Range Fonksiyonu | ✅ | Phase 14 - range(n), range(a,b), range(a,b,step) |
| Null Safety | ✅ | Phase 15 - ??, ?., null literal |
| Operator Overloading | ✅ | Phase 16 - operator +/- for structs |
| Pattern Matching | ✅ | Phase 17 - match/case/=>/_ |
| State Management | ✅ | Phase 18 - state/shared state |
| **String İşlemleri** | | |
| String Literals | ✅ | Tam çalışıyor |
| String Interpolation | ✅ | $"Hello {name}" |
| str_length, substring | ✅ | Tam çalışıyor |
| indexOf, charAt | ✅ | Tam çalışıyor |
| **I/O İşlemleri** | | |
| File Read/Write | ✅ | read_file, write_file |
| File Append/Exists | ✅ | append_file, file_exists |
| CLI Arguments | ✅ | get_argc() |
| **Henüz Yapılmamış** | | |
| Garbage Collection | ✅ | Phase 19 - Otomatik bellek yönetimi |

---

## 🎯 Turing Complete Minimum Set

Bir dil Turing complete olması için minimum gereksinimler:

### 1. ✅ Variables (Phase 0) - TAMAMLANDI
```mlp
numeric x
decimal y
boolean flag
```

### 2. ✅ Assignment (Phase 1) - TAMAMLANDI
```mlp
numeric x = 5
x = 10
x = y + 5
```

### 3. ✅ Arithmetic (Phase 1) - TAMAMLANDI
```mlp
numeric result = a + b * c
x = x - 1
y = a / b
```

### 4. ✅ Conditional (Phase 2) - TAMAMLANDI
```mlp
if x > 5 then
    print(x)
else
    print(0)
end if
```

### 5. ✅ Loop (Phase 3) - TAMAMLANDI
```mlp
while x < 10
    x = x + 1
end while

for i = 0 to 10 step 2
    print(i)
end for
```

**✅ Bu 5 özellik = Turing Complete! - BAŞARILDI!**

---

## 📊 Phase Roadmap

### ✅ Phase 0: Minimal - TAMAMLANDI
```mlp
numeric a
decimal b
boolean c
```

### ✅ Phase 1: Expressions - TAMAMLANDI
```mlp
-- Comments (tek satır)

numeric a = 5
numeric b = a + 10 * 2
print(a)
print(b)
```

### ✅ Phase 2: Conditionals - TAMAMLANDI
```mlp
if x == 5 then
    print(1)
else
    print(0)
end if

-- Comparison operators: ==, !=, <, <=, >, >=
```

### ✅ Phase 3: Loops - TAMAMLANDI (28 Kasım 2025)
```mlp
-- For loop with step
for i = 0 to 10 step 2
    print(i)
end for

-- While loop
numeric x = 0
while x < 5
    x = x + 1
    print(x)
end while

-- Exit (erken çıkış)
for i = 0 to 20
    if i == 8 then
        exit
    end if
    print(i)
end for

-- Continue (iterasyon atla)
for i = 0 to 10
    if i == 3 then
        continue
    end if
    print(i)
end for
```
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
struct Person
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


Logical Operators - and, or, not
Else If - else if (sadece else var)
Debug features - debug goto, debug pause, debug if
Switch/Case - Hiç yok
Do-While - Sadece while var
Lambda functions - Yok
Try/Catch - Error handling yok
Modules/Import - Yok

Async/Await - Concurrency yok
Nullable types (?) - Token var ama parser/codegen yok
Type aliases - Yok
Enums - Yok
Multiple return values - Yok


---

## 📋 Self-Hosting Öncesi Yapılacaklar (Güncel Liste)

### ✅ Tamamlanan Tasarım Kararları (30 Kasım 2025)

#### 1. Veri Yapıları Syntax'ı
| Syntax | Tip | Homojen? | Mutable? | Bellek |
|--------|-----|----------|----------|--------|
| `[]` | **Array** | ✅ Evet | ✅ Evet | Stack/Heap |
| `()` | **List** | ❌ Hayır | ✅ Evet | Heap |
| `<>` | **Tuple** | ❌ Hayır | ❌ Hayır | Stack ⚡ |

```mlp
-- Array: Homojen, mutable
numeric[] sayılar = [1, 2, 3]

-- List: Heterojen, mutable
kişi() = ("Ali", 25, true)

-- Tuple: Heterojen, immutable, stack'te (hızlı!)
koordinat<> = <10, 20, "point">
<min, max> = minmax([3, 1, 4])    -- Destructuring
```

#### 2. Numeric Format Desteği (Lexer'da Normalization)
```mlp
-- Türkçe format (varsayılan): nokta=binlik, virgül=ondalık
numeric maaş = 123.456.789,99       -- → 123456789.99

-- Python style underscore
numeric büyük = 123_456_789         -- → 123456789

-- Her ikisi de dahili olarak aynı değere normalize edilir
```

### ⏳ Implementasyon Bekleyen

1. **Lexer:** Türkçe/Python sayı formatı normalization
2. **Parser:** List `()` ve Tuple `<>` syntax desteği  
3. **Codegen:** Tuple stack allocation
4. **TTO:** int64/double/BigDecimal otomatik seçim
5. **SSO:** Small String Optimization (≤23 byte inline)

---
 