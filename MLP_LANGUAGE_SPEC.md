# MLP (Modern Language Project) - Complete Language Specification

**Version:** 1.0  
**Date:** 30 Kasım 2025  
**Status:** Living Document - Self-Hosting Öncesi

---

## 📋 İçindekiler

1. [Genel Bakış](#1-genel-bakış)
2. [Mimari](#2-mimari)
3. [Tip Sistemi](#3-tip-sistemi)
4. [Söz Dizimi (Syntax)](#4-söz-dizimi-syntax)
5. [Operatörler](#5-operatörler)
6. [Kontrol Yapıları](#6-kontrol-yapıları)
7. [Fonksiyonlar](#7-fonksiyonlar)
8. [Veri Yapıları](#8-veri-yapıları)
9. [Modül Sistemi](#9-modül-sistemi)
10. [Hata Yönetimi](#10-hata-yönetimi)
11. [Bellek Yönetimi](#11-bellek-yönetimi)
12. [Built-in Fonksiyonlar](#12-built-in-fonksiyonlar)
13. [Gelecek Özellikler](#13-gelecek-özellikler)
14. [Compiler Mimarisi](#14-compiler-mimarisi)

---

## 1. Genel Bakış

### 1.1 MLP Nedir?

MLP, kullanıcı dostu söz dizimi ile yüksek performansı birleştiren, **Transparent Type Optimization (TTO)** kullanan modern bir programlama dilidir.

### 1.2 Tasarım Felsefesi

1. **Basitlik:** Kullanıcı sadece `numeric` ve `text` görür
2. **Performans:** Arka planda int64/double/BigDecimal ve SSO/heap otomatik seçilir
3. **Güvenlik:** Null safety, exception handling
4. **Modülerlik:** Import/export sistemi
5. **Self-hosting:** Compiler kendi dilinde yazılabilir

### 1.3 Hedef Platform

- **Assembly:** x86-64 (Intel syntax, NASM)
- **OS:** Linux (ELF64)
- **Runtime:** C runtime (libc, libm)

---

## 2. Mimari

### 2.1 Derleme Süreci

```
┌─────────────────────────────────────────────────────────────────┐
│                     MLP Derleme Süreci                          │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  [Kaynak Kod]  →  [Lexer]  →  [Parser]  →  [Codegen]  →  [ASM]  │
│     .mlp           Tokens      AST         x86-64        .s     │
│                                                                 │
│  [ASM]  →  [NASM]  →  [Linker]  →  [Executable]                 │
│   .s        .o         + runtime      binary                    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 Dosya Yapısı

```
melp/
├── bootstrap/           # C ile yazılmış bootstrap compiler
│   ├── lexer.c         # Tokenizer
│   ├── parser.c        # Parser (AST oluşturur)
│   ├── codegen.c       # x86-64 assembly üretir
│   └── main.c          # Entry point
├── runtime/            # C runtime (self-hosting'de de kullanılır)
│   └── runtime.c       # Built-in fonksiyonlar, memory, I/O
├── selfhost/           # MLP ile yazılmış compiler (hedef)
│   ├── lexer.mlp
│   ├── parser.mlp
│   └── codegen.mlp
└── stdlib/             # Standart kütüphane (MLP ile yazılmış)
    ├── string.mlp
    ├── math.mlp
    └── io.mlp
```

### 2.3 Çok Dilli Destek (Gelecek)

```
[Kullanıcı Kodu]     [diller.json]      [syntax.json]
     ↓                    ↓                  ↓
     └──────────→ [Normalizer] ←────────────┘
                       ↓
              [English + MLP Base Syntax]
                       ↓
                   [Lexer]
                       ↓
                   [Parser]
                       ↓
                  [Codegen]
```

---

## 3. Tip Sistemi

### 3.1 Kullanıcı Görünür Tipler

| Tip | Açıklama | Örnek |
|-----|----------|-------|
| `numeric` | Sayısal değer (TTO ile optimize) | `42`, `3.14`, `123_456` |
| `text` | Metin değeri (SSO ile optimize) | `"Merhaba"` |
| `boolean` | Mantıksal değer | `true`, `false` |
| `decimal` | Ondalık sayı (legacy, numeric'e yönlendir) | `3.14159` |

### 3.2 Transparent Type Optimization (TTO)

Kullanıcı sadece `numeric` yazar, arka planda:

```
┌─────────────────────────────────────────────────────────────┐
│                    TTO Numeric Hiyerarşisi                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   numeric x = 42                                            │
│        ↓                                                    │
│   ┌─────────────────────────────────────────────┐           │
│   │  Değer analizi:                             │           │
│   │  • Tam sayı ve |x| ≤ 2^63-1 → int64         │           │
│   │  • Ondalık veya büyük → double              │           │
│   │  • Çok büyük/hassas → BigDecimal            │           │
│   └─────────────────────────────────────────────┘           │
│        ↓                                                    │
│   ┌─────────────────────────────────────────────┐           │
│   │  Otomatik promotion:                        │           │
│   │  • int64 overflow → double                  │           │
│   │  • double precision loss → BigDecimal       │           │
│   └─────────────────────────────────────────────┘           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**Assembly Temsili:**

```nasm
; int64 için
mov rax, 42

; double için
movsd xmm0, [double_constant]

; BigDecimal için
call bigdecimal_create
```

### 3.3 Small String Optimization (SSO)

```
┌─────────────────────────────────────────────────────────────┐
│                    SSO Text Hiyerarşisi                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   text s = "Ali"                                            │
│        ↓                                                    │
│   ┌─────────────────────────────────────────────┐           │
│   │  Uzunluk kontrolü:                          │           │
│   │  • len ≤ 23 byte → Stack/Register (inline)  │           │
│   │  • len > 23 byte → Heap (pointer + length)  │           │
│   └─────────────────────────────────────────────┘           │
│                                                             │
│   SSO Layout (24 byte):                                     │
│   ┌────────────────────────┬────┐                           │
│   │   23 byte data         │ len│                           │
│   └────────────────────────┴────┘                           │
│                                                             │
│   Heap Layout:                                              │
│   ┌────────┬────────┬──────────────────┐                    │
│   │ ptr    │ length │ capacity         │                    │
│   └────────┴────────┴──────────────────┘                    │
│       ↓                                                     │
│   ┌──────────────────────────────────────┐                  │
│   │ Heap allocated string data...        │                  │
│   └──────────────────────────────────────┘                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.4 Nullable Types

```mlp
numeric? maybe_num = null
text? maybe_text = null

-- Null check
if maybe_num != null then
    print(maybe_num)
end if

-- Null coalescing
numeric value = maybe_num ?? 0

-- Optional chaining
text result = obj?.field?.subfield ?? "default"
```

### 3.5 Type Aliases

```mlp
type PersonId = numeric
type Name = text
type Callback = func(numeric) returns numeric
```

---

## 4. Söz Dizimi (Syntax)

### 4.1 Yorumlar

```mlp
-- Tek satır yorum

--- 
Çok satırlı
yorum bloğu
---
```

### 4.2 Değişken Tanımlama

```mlp
-- Temel tanımlama
numeric x = 42
text name = "Ali"
boolean flag = true

-- Çoklu tanımlama (Phase 21)
numeric a, b, c = 1, 2, 3
numeric, text x, y = 42, "hello"

-- Büyük sayı formatları (Planlanan)
numeric big = 123_456_789        -- Python style
numeric big2 = 123.456.789       -- Türkçe style (. binlik ayracı)
numeric decimal_tr = 123,45      -- Türkçe ondalık (, ondalık ayracı)
```

### 4.3 Sabitler

```mlp
-- State ile global değişken
state numeric counter = 0
state text app_name = "MyApp"

-- Shared state (thread-safe, planlanan)
shared state numeric global_counter = 0
```

### 4.4 String Literals

```mlp
-- Basit string
text s1 = "Merhaba Dünya"

-- Escape sequences
text s2 = "Satır 1\nSatır 2"
text s3 = "Tab:\tdeğer"
text s4 = "Tırnak: \"alıntı\""
text s5 = "Backslash: \\"

-- String interpolation
text name = "Ali"
numeric age = 25
text message = $"Merhaba {name}, yaşın {age}"

-- Multi-line string (Planlanan)
text long = """
Bu çok satırlı
bir metin
"""
```

---

## 5. Operatörler

### 5.1 Aritmetik Operatörler

| Operatör | Açıklama | Örnek |
|----------|----------|-------|
| `+` | Toplama | `a + b` |
| `-` | Çıkarma | `a - b` |
| `*` | Çarpma | `a * b` |
| `/` | Bölme | `a / b` |
| `mod` | Modulo | `a mod b` |
| `-` | Unary minus | `-x`, `-3.14` |

### 5.2 Karşılaştırma Operatörleri

| Operatör | Açıklama | Örnek |
|----------|----------|-------|
| `==` | Eşitlik | `a == b` |
| `!=` | Eşit değil | `a != b` |
| `<` | Küçük | `a < b` |
| `>` | Büyük | `a > b` |
| `<=` | Küçük eşit | `a <= b` |
| `>=` | Büyük eşit | `a >= b` |

### 5.3 Mantıksal Operatörler

| Operatör | Açıklama | Örnek |
|----------|----------|-------|
| `and` | VE | `a and b` |
| `or` | VEYA | `a or b` |
| `not` | DEĞİL | `not a` |

### 5.4 Bitwise Operatörler

| Operatör | Açıklama | Örnek |
|----------|----------|-------|
| `band` | Bitwise AND | `a band b` |
| `bor` | Bitwise OR | `a bor b` |
| `bxor` | Bitwise XOR | `a bxor b` |
| `bnot` | Bitwise NOT | `bnot a` |
| `shl` | Shift left | `a shl 2` |
| `shr` | Shift right | `a shr 2` |

### 5.5 Null Operatörleri

| Operatör | Açıklama | Örnek |
|----------|----------|-------|
| `??` | Null coalescing | `a ?? default` |
| `?.` | Optional chaining | `obj?.field` |

### 5.6 Operatör Önceliği (Yüksekten Düşüğe)

1. `()` - Parantez
2. `.`, `?.`, `[]` - Erişim
3. `not`, `bnot`, `-` (unary) - Unary
4. `*`, `/`, `mod` - Çarpma/Bölme
5. `+`, `-` - Toplama/Çıkarma
6. `shl`, `shr` - Bit kaydırma
7. `<`, `>`, `<=`, `>=` - Karşılaştırma
8. `==`, `!=` - Eşitlik
9. `band` - Bitwise AND
10. `bxor` - Bitwise XOR
11. `bor` - Bitwise OR
12. `and` - Mantıksal AND
13. `or` - Mantıksal OR
14. `??` - Null coalescing
15. `=` - Atama

---

## 6. Kontrol Yapıları

### 6.1 If-Else

```mlp
if condition then
    -- kod
end if

if condition then
    -- kod
else
    -- kod
end if

if condition1 then
    -- kod
else if condition2 then
    -- kod
else
    -- kod
end if
```

### 6.2 While Döngüsü

```mlp
while condition
    -- kod
end while
```

### 6.3 Do-While Döngüsü

```mlp
do
    -- kod
while condition
```

### 6.4 For Döngüsü

```mlp
-- Basit for
for i = 0 to 10
    print(i)
end for

-- Step ile
for i = 0 to 100 step 10
    print(i)
end for

-- For-in (iterator)
for item in collection
    print(item)
end for

-- Range ile
for i in range(10)
    print(i)
end for

for i in range(5, 10)
    print(i)
end for

for i in range(0, 100, 5)
    print(i)
end for
```

### 6.5 Switch-Case (Match)

```mlp
-- Temel switch
switch value
    case 1
        print("bir")
    case 2, 3
        print("iki veya üç")
    case 4 to 10
        print("4-10 arası")
    default
        print("diğer")
end switch

-- Pattern matching (Phase 17)
match value
    case 0 => print("sıfır")
    case n if n > 0 => print("pozitif")
    case _ => print("negatif")
end match
```

### 6.6 Döngü Kontrolü

```mlp
-- Exit: Döngüden çık
for i = 0 to 100
    if i == 50 then
        exit
    end if
end for

-- Continue: Sonraki iterasyona geç
for i = 0 to 10
    if i == 5 then
        continue
    end if
    print(i)
end for
```

---

## 7. Fonksiyonlar

### 7.1 Fonksiyon Tanımlama

```mlp
-- Parametresiz
func sayHello()
    print("Merhaba!")
end func

-- Parametreli
func add(numeric a, numeric b) returns numeric
    return a + b
end func

-- Çoklu return
func minmax(numeric a, numeric b) returns numeric, numeric
    if a < b then
        return a, b
    else
        return b, a
    end if
end func

-- Çoklu return kullanımı
numeric min, max = minmax(10, 5)
```

### 7.2 Lambda/Closure

```mlp
-- Lambda tanımlama
numeric list() numbers = [1, 2, 3, 4, 5]
numbers.map(lambda x => x * 2)
numbers.filter(lambda x => x > 2)

-- Closure
func makeCounter() returns func() returns numeric
    numeric count = 0
    return lambda () =>
        count = count + 1
        return count
    end lambda
end func

numeric counter = makeCounter()
print(counter())  -- 1
print(counter())  -- 2
```

### 7.3 Generator

```mlp
func fibonacci(numeric n) yields numeric
    numeric a = 0
    numeric b = 1
    for i = 0 to n
        yield a
        numeric temp = a
        a = b
        b = temp + b
    end for
end func

for num in fibonacci(10)
    print(num)
end for
```

### 7.4 Async/Await (Temel)

```mlp
async func fetchData() returns text
    text data = await http_get("https://api.example.com")
    return data
end func
```

---

## 8. Veri Yapıları

### 8.1 Arrays

```mlp
-- Array literal
numeric list() numbers = [1, 2, 3, 4, 5]

-- Boş array
numeric list() empty = []

-- Dinamik array (Planlanan)
list() mixed = ["Ali", 10, true]  -- Heterojen

-- Array işlemleri
numbers[0] = 100        -- Index erişim
numeric len = length(numbers)
array_push(numbers, 6)
numeric last = array_pop(numbers)
```

### 8.2 Structs

```mlp
struct Person
    text name
    numeric age
end struct

-- Kullanım
Person p
p.name = "Ali"
p.age = 25

-- Struct literal (Planlanan)
Person p2 = Person { name: "Veli", age: 30 }
```

### 8.3 Enums

```mlp
enum Status
    Pending = 0
    Active = 1
    Completed = 2
end enum

Status current = Status.Active

if current == Status.Active then
    print("Aktif")
end if
```

### 8.4 Generic Types

```mlp
-- Generic struct
struct Box<T>
    T value
end struct

Box<numeric> numBox
numBox.value = 42

-- Generic function
func identity<T>(T value) returns T
    return value
end func

numeric x = identity<numeric>(42)
text s = identity<text>("hello")
```

---

## 9. Modül Sistemi

### 9.1 Import

```mlp
-- Dosya import
import "math_utils.mlp"

-- Modül import
import math
import string as str

-- Kullanım
numeric result = math.sqrt(16)
text upper = str.to_upper("hello")
```

### 9.2 Module Tanımlama

```mlp
module MathUtils
    export func add(numeric a, numeric b) returns numeric
        return a + b
    end func
    
    export func multiply(numeric a, numeric b) returns numeric
        return a * b
    end func
    
    -- Private (export edilmemiş)
    func helper() returns numeric
        return 42
    end func
end module
```

---

## 10. Hata Yönetimi

### 10.1 Try-Catch-Finally

```mlp
try
    numeric result = riskyOperation()
catch error
    print($"Hata: {error}")
finally
    cleanup()
end try
```

### 10.2 Throw

```mlp
func divide(numeric a, numeric b) returns numeric
    if b == 0 then
        throw "Division by zero"
    end if
    return a / b
end func
```

---

## 11. Bellek Yönetimi

### 11.1 Garbage Collection (Phase 19)

MLP, mark-and-sweep garbage collector kullanır:

```
┌─────────────────────────────────────────────────────────────┐
│                    GC Mimarisi                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  [Stack/Registers]  →  [Root Set]                           │
│         ↓                  ↓                                │
│  ┌─────────────────────────────────────────┐                │
│  │           Mark Phase                    │                │
│  │  • Root'lardan başla                    │                │
│  │  • Erişilebilir nesneleri işaretle      │                │
│  └─────────────────────────────────────────┘                │
│         ↓                                                   │
│  ┌─────────────────────────────────────────┐                │
│  │           Sweep Phase                   │                │
│  │  • İşaretsiz nesneleri serbest bırak    │                │
│  │  • Belleği geri al                      │                │
│  └─────────────────────────────────────────┘                │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 11.2 GC Built-in Fonksiyonları

```mlp
gc_collect()              -- Manuel GC tetikle
gc_full_collect()         -- Tam GC
numeric count = gc_get_object_count()
numeric bytes = gc_get_total_bytes()
```

---

## 12. Built-in Fonksiyonlar

### 12.1 I/O

| Fonksiyon | Açıklama | Örnek |
|-----------|----------|-------|
| `print(value)` | Ekrana yaz | `print("Hello")` |
| `input()` | Kullanıcıdan oku | `text s = input()` |
| `input(prompt)` | Prompt ile oku | `text s = input("İsim: ")` |

### 12.2 String İşlemleri

| Fonksiyon | Açıklama | Örnek |
|-----------|----------|-------|
| `str_length(s)` | Uzunluk | `numeric len = str_length("Ali")` |
| `substring(s, start, len)` | Alt string | `substring("Hello", 0, 2)` |
| `indexOf(s, search)` | Arama | `indexOf("Hello", "ll")` |
| `charAt(s, index)` | Karakter al | `charAt("Hello", 0)` |
| `to_upper(s)` | Büyük harf | `to_upper("hello")` |
| `to_lower(s)` | Küçük harf | `to_lower("HELLO")` |
| `trim(s)` | Boşluk temizle | `trim("  hi  ")` |
| `split(s, delim)` | Böl | `split("a,b,c", ",")` |
| `replace(s, old, new)` | Değiştir | `replace("hello", "l", "L")` |

### 12.3 Tip Dönüşümleri

| Fonksiyon | Açıklama | Örnek |
|-----------|----------|-------|
| `to_numeric(s)` | String → Sayı | `to_numeric("42")` |
| `to_text(n)` | Sayı → String | `to_text(42)` |

### 12.4 Matematik

| Fonksiyon | Açıklama |
|-----------|----------|
| `abs(x)` | Mutlak değer |
| `floor(x)` | Aşağı yuvarla |
| `ceil(x)` | Yukarı yuvarla |
| `round(x)` | Yuvarla |
| `sqrt(x)` | Karekök |
| `pow(x, y)` | Üs alma |
| `sin(x)`, `cos(x)`, `tan(x)` | Trigonometri |
| `log(x)`, `log10(x)` | Logaritma |
| `exp(x)` | e^x |
| `min(a, b)`, `max(a, b)` | Min/Max |
| `random()` | 0-1 arası rastgele |

### 12.5 Array İşlemleri

| Fonksiyon | Açıklama |
|-----------|----------|
| `length(arr)` | Uzunluk |
| `array_push(arr, val)` | Sona ekle |
| `array_pop(arr)` | Sondan çıkar |
| `array_get(arr, idx)` | Index ile al |
| `array_set(arr, idx, val)` | Index ile ata |
| `array_resize(arr, size)` | Boyut değiştir |

### 12.6 Dosya İşlemleri

| Fonksiyon | Açıklama |
|-----------|----------|
| `read_file(path)` | Dosya oku |
| `write_file(path, content)` | Dosya yaz |
| `append_file(path, content)` | Dosyaya ekle |
| `file_exists(path)` | Dosya var mı |

### 12.7 Sistem

| Fonksiyon | Açıklama |
|-----------|----------|
| `get_argc()` | Argüman sayısı |
| `get_argv(index)` | Argüman al |
| `exit(code)` | Programı sonlandır |
| `assert(cond, msg)` | Doğrulama |

---

## 13. Gelecek Özellikler

### 13.1 Sayı Formatları (Yüksek Öncelik)

MLP, farklı sayı formatlarını destekleyecektir:

#### 13.1.1 Binlik Ayracı (Underscore)

Python benzeri underscore ayracı:

```mlp
numeric büyük = 1_000_000_000        -- Okunabilirlik için
numeric hex = 0xFF_FF_FF_FF          -- Hex'te de geçerli
numeric binary = 0b1010_1010_1010    -- Binary'de de geçerli
```

#### 13.1.2 Türkçe Sayı Formatı

Türkçe/Avrupa sayı formatı desteği:

```mlp
-- Virgül ondalık ayracı, nokta binlik ayracı
numeric maaş = 12.345.678,90         -- 12345678.90
numeric oran = 3,14                  -- 3.14
numeric büyük = 1.000.000            -- 1000000

-- Karışık format desteği (locale-aware)
numeric a = 123_456.78               -- Underscore (Programcı)
numeric b = 123.456,78               -- Türkçe format (Kullanıcı)
```

#### 13.1.3 Format Dönüşümü

```mlp
-- Sayı → Formatlı string
text s1 = format_number(12345.67, "tr")     -- "12.345,67"
text s2 = format_number(12345.67, "en")     -- "12,345.67"
text s3 = format_number(12345.67, "_")      -- "12_345.67"

-- Formatlı string → Sayı
numeric n1 = parse_number("12.345,67", "tr") -- 12345.67
```

### 13.2 Veri Yapıları: Array, List, Tuple (Yüksek Öncelik)

MLP'de üç temel koleksiyon tipi vardır. Her birinin kendine özgü syntax'ı ve kullanım amacı vardır:

| Syntax | Tip | Homojen? | Mutable? | Bellek | Kullanım Amacı |
|--------|-----|----------|----------|--------|----------------|
| `[]` | **Array** | ✅ Evet | ✅ Evet | Stack/Heap | Aynı tipte veri dizisi |
| `()` | **List** | ❌ Hayır | ✅ Evet | Heap | Heterojen dinamik koleksiyon |
| `<>` | **Tuple** | ❌ Hayır | ❌ Hayır | Stack ⚡ | Immutable kayıt, hızlı |

#### 13.2.1 Array `[]` - Homojen, Mutable

```mlp
-- Array tanımlama: Tüm elemanlar aynı tipte
numeric[] sayılar = [1, 2, 3, 4, 5]
text[] isimler = ["Ali", "Veli", "Ayşe"]

-- Eleman erişimi ve değiştirme
sayılar[0] = 100                    -- ✅ OK, mutable
print(sayılar[0])                   -- 100

-- Dinamik işlemler
array_push(sayılar, 6)              -- ✅ OK, eleman ekle
numeric son = array_pop(sayılar)    -- ✅ OK, eleman çıkar

-- Boş array
numeric[] boş = []
```

#### 13.2.2 List `()` - Heterojen, Mutable

```mlp
-- List tanımlama: Farklı tipler bir arada
kişi() = ("Ali", 25, true, 3.14)

-- Eleman erişimi (runtime tip kontrolü)
text isim = kişi[0]                 -- "Ali"
numeric yaş = kişi[1]               -- 25
boolean aktif = kişi[2]             -- true

-- Eleman değiştirme
kişi[0] = "Veli"                    -- ✅ OK, mutable
kişi[1] = 30                        -- ✅ OK

-- Dinamik işlemler
kişi.add("yeni eleman")             -- ✅ OK, eleman ekle
kişi.remove(0)                      -- ✅ OK, eleman sil

-- Boş list
boş() = ()

-- Tip güvenli erişim
if type_of(kişi[0]) == "text" then
    text ad = kişi[0] as text
end if
```

#### 13.2.3 Tuple `<>` - Heterojen, Immutable, Stack-allocated

```mlp
-- Tuple tanımlama: Farklı tipler, değiştirilemez
koordinat<> = <10, 20, "point">
renk<> = <255, 128, 0>

-- Eleman erişimi (sadece okuma)
print(koordinat[0])                 -- 10
print(koordinat[2])                 -- "point"

-- ❌ Tuple immutable - değiştirilemez!
koordinat[0] = 99                   -- ❌ HATA! Compile error
koordinat.add(5)                    -- ❌ HATA! Boyut sabit

-- Fonksiyondan tuple döndürme (stack'te, hızlı!)
func minmax(numeric[] arr) returns <numeric, numeric>
    return <min(arr), max(arr)>
end func

sonuç<> = minmax([3, 1, 4, 1, 5])
print(sonuç[0])                     -- 1 (min)
print(sonuç[1])                     -- 5 (max)

-- Tuple destructuring
<min_val, max_val> = minmax([3, 1, 4, 1, 5])
print(min_val)                      -- 1
print(max_val)                      -- 5

-- Swap işlemi (tuple ile)
<a, b> = <b, a>
```

#### 13.2.4 Karşılaştırma ve Kullanım Senaryoları

| Senaryo | Önerilen Tip | Neden? |
|---------|--------------|--------|
| Sayı dizisi işleme | `numeric[]` Array | Homojen, hızlı indeksleme |
| Veritabanı kaydı | `kişi()` List | Farklı tipler, değiştirilebilir |
| Fonksiyon çoklu return | `<a, b>` Tuple | Stack'te, allocation yok |
| Koordinat, RGB | `<x, y, z>` Tuple | Sabit yapı, immutable |
| Dinamik koleksiyon | `items()` List | Eleman ekle/çıkar |

### 13.3 Çoklu Değişken Tanımlama (Orta Öncelik)

```mlp
-- Aynı tipte çoklu tanımlama
numeric a, b, c = 1, 2, 3

-- Farklı değerlerle
numeric x = 0, y = 0, z = 0

-- Swap (tuple ile)
<a, b> = <b, a>

-- Fonksiyondan çoklu dönüş (tuple destructuring)
<text ad, numeric yaş> = get_user_info()
```

### 13.4 Default ve Named Parametreler (Orta Öncelik)

```mlp
-- Default parametreler
func selamla(text isim = "Dünya", text mesaj = "Merhaba")
    print(mesaj + " " + isim + "!")
end func

selamla()                        -- "Merhaba Dünya!"
selamla("Ali")                   -- "Merhaba Ali!"

-- Named parametreler
func kayıt_oluştur(text isim, numeric yaş, boolean aktif = true)
    -- ...
end func

kayıt_oluştur(yaş: 25, isim: "Ali")  -- Sıralama önemli değil
```

### 13.5 Spread Operatörü (Düşük Öncelik)

```mlp
-- Array spread
numeric[] a = [1, 2, 3]
numeric[] b = [0, ...a, 4, 5]    -- [0, 1, 2, 3, 4, 5]

-- Fonksiyon çağrısında
func toplam(numeric x, numeric y, numeric z) returns numeric
    return x + y + z
end func

numeric[] args = [1, 2, 3]
print(toplam(...args))           -- 6
```

### 13.6 Planlanan Diğer Özellikler

| Özellik | Açıklama | Öncelik |
|---------|----------|---------|
| Multi-line strings | `"""..."""` | Yüksek |
| String interpolation güçlendirme | `f"x={x:2d}"` | Orta |
| Async iterators | `async for item in stream` | Düşük |
| Generator functions | `yield` keyword | Düşük |
| Regex literals | `/pattern/flags` | Düşük |
| Decorator/Annotation | `@cached` | Düşük |

### 13.7 Eklenmeyecekler

| Özellik | Neden |
|---------|-------|
| Class/Inheritance | struct + functions yeterli |
| Multiple inheritance | Karmaşıklık |
| Operator precedence override | Kafa karıştırıcı |
| Implicit type coercion | Hata kaynağı |
| Macros | Hata ayıklamayı zorlaştırır |
| Pointer arithmetic | Güvenlik riski |

---

## 14. Compiler Mimarisi

### 14.1 Modüler C Yapısı (Mevcut → Hedef)

**Mevcut (Monolitik):**
```
bootstrap/
├── lexer.c      (~500 satır)
├── parser.c     (~4500 satır) 
├── codegen.c    (~4600 satır)
└── main.c       (~300 satır)
```

**Hedef (Modüler):**
```
bootstrap/
├── lexer/
│   └── lexer.c
├── parser/
│   ├── parser_core.c      # Temel parser altyapısı
│   ├── parser_expr.c      # Expression parsing
│   ├── parser_stmt.c      # Statement parsing  
│   ├── parser_decl.c      # Declaration parsing
│   └── parser_type.c      # Type parsing
├── codegen/
│   ├── codegen_core.c     # Temel codegen altyapısı
│   ├── codegen_expr.c     # Expression code generation
│   ├── codegen_stmt.c     # Statement code generation
│   ├── codegen_if.c       # If/else generation
│   ├── codegen_loop.c     # For/while generation
│   ├── codegen_func.c     # Function generation
│   ├── codegen_struct.c   # Struct generation
│   ├── codegen_match.c    # Pattern matching
│   ├── codegen_async.c    # Async/await
│   └── codegen_tto.c      # TTO optimizations
├── runtime/
│   └── runtime.c          # C runtime (değişmez)
└── main.c
```

### 14.2 Self-Hosted MLP Yapısı

```
selfhost/
├── lexer/
│   ├── lexer.mlp          # Tokenizer
│   └── tokens.mlp         # Token tanımları
├── parser/
│   ├── parser.mlp         # Ana parser
│   ├── ast.mlp            # AST node tanımları
│   ├── expr.mlp           # Expression parser
│   └── stmt.mlp           # Statement parser
├── codegen/
│   ├── codegen.mlp        # Ana codegen
│   ├── emit.mlp           # Assembly emit
│   ├── expr.mlp           # Expression codegen
│   ├── stmt.mlp           # Statement codegen
│   └── optimize.mlp       # TTO optimizations
└── main.mlp               # Compiler entry point
```

### 14.3 Token Listesi

```mlp
-- Literals
TOKEN_NUMBER      -- 123, 3.14
TOKEN_STRING      -- "hello"
TOKEN_TRUE        -- true
TOKEN_FALSE       -- false
TOKEN_NULL        -- null

-- Types
TOKEN_NUMERIC     -- numeric
TOKEN_DECIMAL     -- decimal
TOKEN_BOOLEAN     -- boolean
TOKEN_TEXT        -- text

-- Keywords - Control
TOKEN_IF          -- if
TOKEN_THEN        -- then
TOKEN_ELSE        -- else
TOKEN_END         -- end
TOKEN_WHILE       -- while
TOKEN_DO          -- do
TOKEN_FOR         -- for
TOKEN_TO          -- to
TOKEN_STEP        -- step
TOKEN_IN          -- in
TOKEN_SWITCH      -- switch
TOKEN_CASE        -- case
TOKEN_DEFAULT     -- default
TOKEN_MATCH       -- match

-- Keywords - Functions
TOKEN_FUNC        -- func
TOKEN_RETURN      -- return
TOKEN_RETURNS     -- returns
TOKEN_LAMBDA      -- lambda
TOKEN_YIELD       -- yield
TOKEN_YIELDS      -- yields
TOKEN_ASYNC       -- async
TOKEN_AWAIT       -- await

-- Keywords - Data
TOKEN_STRUCT      -- struct
TOKEN_ENUM        -- enum
TOKEN_TYPE        -- type
TOKEN_LIST        -- list
TOKEN_INTERFACE   -- interface
TOKEN_IMPLEMENTS  -- implements

-- Keywords - Modules
TOKEN_IMPORT      -- import
TOKEN_MODULE      -- module
TOKEN_EXPORT      -- export
TOKEN_AS          -- as

-- Keywords - Control Flow
TOKEN_EXIT        -- exit
TOKEN_CONTINUE    -- continue
TOKEN_TRY         -- try
TOKEN_CATCH       -- catch
TOKEN_FINALLY     -- finally
TOKEN_THROW       -- throw

-- Keywords - State
TOKEN_STATE       -- state
TOKEN_SHARED      -- shared

-- Operators
TOKEN_PLUS        -- +
TOKEN_MINUS       -- -
TOKEN_MULTIPLY    -- *
TOKEN_DIVIDE      -- /
TOKEN_MOD         -- mod
TOKEN_ASSIGN      -- =
TOKEN_EQUAL       -- ==
TOKEN_NOT_EQUAL   -- !=
TOKEN_LESS        -- <
TOKEN_GREATER     -- >
TOKEN_LESS_EQ     -- <=
TOKEN_GREATER_EQ  -- >=
TOKEN_AND         -- and
TOKEN_OR          -- or
TOKEN_NOT         -- not
TOKEN_BAND        -- band
TOKEN_BOR         -- bor
TOKEN_BXOR        -- bxor
TOKEN_BNOT        -- bnot
TOKEN_SHL         -- shl
TOKEN_SHR         -- shr
TOKEN_ARROW       -- =>
TOKEN_QUESTION    -- ?
TOKEN_COALESCE    -- ??
TOKEN_OPTIONAL    -- ?.

-- Delimiters
TOKEN_LPAREN      -- (
TOKEN_RPAREN      -- )
TOKEN_LBRACKET    -- [
TOKEN_RBRACKET    -- ]
TOKEN_LBRACE      -- {
TOKEN_RBRACE      -- }
TOKEN_COMMA       -- ,
TOKEN_DOT         -- .
TOKEN_COLON       -- :
TOKEN_SEMICOLON   -- ;

-- Special
TOKEN_EOF         -- End of file
TOKEN_IDENTIFIER  -- Değişken/fonksiyon isimleri
TOKEN_NEWLINE     -- Satır sonu
TOKEN_COMMENT     -- Yorum
```

---

## Appendix A: Örnek Programlar

### A.1 Hello World

```mlp
print("Merhaba Dünya!")
```

### A.2 Fibonacci

```mlp
func fibonacci(numeric n) returns numeric
    if n <= 1 then
        return n
    end if
    return fibonacci(n - 1) + fibonacci(n - 2)
end func

for i = 0 to 10
    print(fibonacci(i))
end for
```

### A.3 FizzBuzz

```mlp
for i = 1 to 100
    if i mod 15 == 0 then
        print("FizzBuzz")
    else if i mod 3 == 0 then
        print("Fizz")
    else if i mod 5 == 0 then
        print("Buzz")
    else
        print(i)
    end if
end for
```

### A.4 Struct Kullanımı

```mlp
struct Point
    numeric x
    numeric y
end struct

func distance(Point p1, Point p2) returns numeric
    numeric dx = p2.x - p1.x
    numeric dy = p2.y - p1.y
    return sqrt(dx * dx + dy * dy)
end func

Point a
a.x = 0
a.y = 0

Point b
b.x = 3
b.y = 4

print(distance(a, b))  -- 5
```

---

## Appendix B: Hata Mesajları

| Hata Kodu | Mesaj | Açıklama |
|-----------|-------|----------|
| E001 | Unexpected token | Beklenmeyen token |
| E002 | Undefined variable | Tanımsız değişken |
| E003 | Type mismatch | Tip uyumsuzluğu |
| E004 | Division by zero | Sıfıra bölme |
| E005 | Index out of bounds | Geçersiz index |
| E006 | Null pointer | Null referans |
| E007 | Stack overflow | Yığın taşması |
| E008 | Out of memory | Bellek yetersiz |

---

## Appendix C: Versiyon Geçmişi

| Versiyon | Tarih | Değişiklikler |
|----------|-------|---------------|
| 0.1 | 28 Kasım 2025 | İlk çalışan compiler |
| 0.5 | 29 Kasım 2025 | Functions, structs, arrays |
| 0.10 | 29 Kasım 2025 | Exceptions, modules |
| 0.15 | 29 Kasım 2025 | Null safety, operators |
| 0.19 | 30 Kasım 2025 | GC, pattern matching |
| 0.20 | 30 Kasım 2025 | TTO başlangıç, built-ins |
| 0.21 | 30 Kasım 2025 | Mixed declarations, list syntax |
| 1.0 | TBD | Self-hosting complete |

---

**Son Güncelleme:** 30 Kasım 2025  
**Yazar:** MLP Development Team
