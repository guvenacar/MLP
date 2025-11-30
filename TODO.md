# MELP TODO List

**Son Güncelleme:** 30 Kasım 2025  
**Durum:** Phase 20 Complete - Built-in Functions 🎉

---

## 🚨 SELF-HOSTING ÖNCESİ YAPILACAKLAR (YENİ PLANLAMA)

**Önem:** Bu özellikler self-hosting compiler yazılmadan ÖNCE eklenmelidir!

### 📌 Öncelik 0: Transparent Type Optimization (TTO) - KRİTİK

**Felsefe:** Kullanıcı sadece `numeric` ve `text` görür, arka planda otomatik optimizasyon yapılır.

| # | Özellik | Zorluk | Süre | Durum | Açıklama |
|---|---------|--------|------|-------|----------|
| 0.1 | **Numeric TTO** | Orta | 1 gün | ⏳ | int64 (küçük) → double (ondalık) → BigDecimal (büyük) |
| 0.2 | **Text TTO (SSO)** | Orta | 1 gün | ⏳ | ≤23 byte stack, >23 byte heap, sabit .rodata |
| 0.3 | **Overflow Handling** | Orta | 0.5 gün | ⏳ | int64 taşarsa otomatik BigDecimal'e promote |
| 0.4 | **Tip-aware Codegen** | Orta | 0.5 gün | ⏳ | Her dahili tip için optimize assembly |

**TTO Toplam Süre:** ~3 gün

### 📌 Öncelik 1: Temel Fonksiyonlar - ✅ TAMAMLANDI (Phase 20)

| # | Özellik | Zorluk | Süre | Durum | Açıklama |
|---|---------|--------|------|-------|----------|
| 1 | **String ↔ Number dönüşümü** | Kolay | 1 saat | ✅ | `to_numeric("42")`, `to_text(42)` |
| 2 | **Input fonksiyonu** | Kolay | 1 saat | ✅ | `input()`, `input(prompt)` |
| 3 | **Math kütüphanesi** | Kolay | 2-3 saat | ✅ | sin, cos, sqrt, pow, abs, floor, ceil, log, exp |
| 4 | **String fonksiyonları** | Kolay | 2 saat | ✅ | split, trim, replace, to_upper, to_lower, starts_with, ends_with, contains |
| 5 | **Bitwise operatörler** | Kolay | 2-3 saat | ✅ | band, bor, bxor, bnot, shl, shr, ushr |
| 6 | **Assert** | Kolay | 1 saat | ✅ | assert(condition, "message") |
| 7 | **Union Types** | Orta | 1-2 gün | ⏳ | `type Result = numeric \| text` - Tip güvenliği |

### 🎯 KALAN TAHMİNİ SÜRE: ~4-5 gün (TTO + Union Types)

### 📝 Önerilen Uygulama Sırası:
1. ⏳ **TTO Numeric** - int64/double/BigDecimal seçimi
2. ⏳ **TTO Text** - SSO implementasyonu
3. ⏳ **Union Types** - En karmaşık

**Not:** Class/Inheritance EKLENMEYECEK - struct+functions yaklaşımı tercih edildi.

---

## ✅ Tam Çalışan Özellikler

### Temel Özellikler
- [x] **Yorum Satırları** - `--` tek satır, `---` çoklu satır
- [x] **Değişkenler** - numeric, text, boolean
- [x] **Aritmetik İşlemler** - +, -, *, /
- [x] **Print Statement** - print()
- [x] **If-Else** - if/then/else/end if
- [x] **Else-If Chains** - else if zincirleri
- [x] **While Döngüsü** - while/end while
- [x] **Do-While Döngüsü** - do/while
- [x] **For Döngüsü** - for/to/step/end for
- [x] **Exit/Continue** - döngü kontrolü
- [x] **Switch-Case** - switch/case/default/end switch
- [x] **Fonksiyonlar** - func/return/end func
- [x] **Pointers** - &var, *ptr
- [x] **Lambda/Closure** - func(x) = x + 10
- [x] **String İşlemleri** - str_length, substring, indexOf, charAt
- [x] **String Interpolation** - $"Hello {name}"
- [x] **File I/O** - read_file, write_file, append_file, file_exists
- [x] **CLI Arguments** - get_argc()
- [x] **Ternary Operator** - x > y ? x : y
- [x] **Logical Operators** - `and`, `or`, `not` (NOT: &&, ||, ! değil!)
- [x] **Enum Types** - enum/end enum, Enum.Value
- [x] **Destructuring** - numeric a, b, c = arr

### Modern Özellikler
- [x] **Exception Handling** - try/catch/throw/end try
- [x] **Async/Await** - async func, await
- [x] **Module System (Multi-file)** - import ModuleName (ayrı .mlp dosyası)
- [x] **Generics** - func max<T>(T a, T b), struct Pair<T, U>
- [x] **Iterators/Generators** - for x in arr, range(), yields/yield

---

## ⚠️ Kısmi Çalışan Özellikler (Bug'lu)

### Arrays
- ✅ Dynamic: `numeric[] arr = malloc(5)` + `arr[0] = value`
- ✅ Static literals: `numeric[5] arr = [10, 20, 30, 40, 50]` - ÇALIŞıYOR

### Structs  
- ✅ Struct tanımlama ve field erişimi
- ✅ Struct return bug: DÜZELTİLDİ (rax + rdx ile multi-register return)

### Module System
- ✅ Multi-file import çalışıyor
- ✅ Aynı dosya içinde module tanımlama ve kullanma - ÇALIŞIYOR

---

## ❌ Çalışmayan Özellikler (Kritik Buglar)

### Pattern Matching - DÜZELTİLDİ ✅
- `case 1 to 5` (range matching) → ÇALIŞıYOR
- Guard conditions `case x if x > 10` → ÇALIŞıYOR
- **Düzeltme:** Declaration struct field initialization eklendi

### Interface - ÇALIŞIYOR ✅
- İngilizce syntax ile test edildi
- `struct Circle implements Drawable` → ÇALIŞIYOR

---

## 🔧 Düzeltilecekler (Öncelik Sırası)

1. ~~**[ACİL]** Pattern Matching SEGFAULT düzelt~~ ✅ DÜZELTİLDİ
2. ~~**[YÜKSEK]** Struct return bug düzelt~~ ✅ DÜZELTİLDİ
3. ~~**[YÜKSEK]** Interface İngilizce syntax testi~~ ✅ ÇALIŞIYOR
4. ~~**[ORTA]** Static array literals desteği~~ ✅ ÇALIŞIYOR
5. ~~**[DÜŞÜK]** Same-file module desteği~~ ✅ ÇALIŞIYOR

**TÜM KRİTİK BUGLAR DÜZELTİLDİ!** 🎉

---

## 🚀 Gelecek Özellikler

### 1. Modül Sistemi (Module System) - YÜKSEKÖNCELİK
**Amaç:** Kod organizasyonu ve yeniden kullanılabilirlik

**Sözdizimi:**
```mlp
-- math.mlp
module Math
    export func add(numeric a, numeric b) -> numeric
        return a + b
    end func
    
    export numeric PI = 3.14159
end module

-- main.mlp
import Math

func main()
    print(Math.add(5, 3))
    print(Math.PI)
end func
```

**Gereksinimler:**
- [ ] Lexer: `module`, `import`, `export` tokenları
- [ ] Parser: Module bildirimleri, import statements
- [ ] Symbol resolution: Cross-module symbol lookup
- [ ] Codegen: External symbol linking
- [ ] File system: Multiple .mlp file handling
- [ ] Namespace management

**Zorluk:** ORTA - Çoklu dosya yönetimi gerekiyor

---

### 2. Generics / Templates - ORTA ÖNCELİK
**Amaç:** Tip güvenli genel amaçlı kod

**Sözdizimi:**
```mlp
-- Generic function
func max<T>(T a, T b) -> T
    return a > b ? a : b
end func

-- Generic struct
struct Array<T>
    T* data
    numeric size
end struct

-- Usage
numeric x = max<numeric>(5, 10)
text y = max<text>("abc", "xyz")

Array<numeric> nums
Array<text> strings
```

**Gereksinimler:**
- [ ] Lexer: `<>` angle bracket support (conflict with comparison?)
- [ ] Parser: Type parameters, generic declarations
- [ ] Type checking: Template instantiation
- [ ] Codegen: Monomorphization (her tip için kod üret)
- [ ] Symbol table: Generic type resolution

**Zorluk:** YÜKSEK - Tip sistemi büyük değişiklik

---

### 3. Pattern Matching - ORTA ÖNCELİK
**Amaç:** Güçlü kontrol akışı ve destructuring

**Sözdizimi:**
```mlp
match value
    case 0:
        print("Zero")
    case 1, 2, 3:
        print("Small")
    case x if x > 10:
        print("Large")
    default:
        print("Other")
end match

-- Struct destructuring
match person
    case Person{name: "Alice", age: a}:
        print("Alice is ", a, " years old")
    case Person{age: a} if a >= 18:
        print("Adult")
    default:
        print("Other person")
end match
```

**Gereksinimler:**
- [ ] Lexer: `match`, `case`, `default` tokenları
- [ ] Parser: Match expressions, case patterns
- [ ] Pattern matching: Value, range, guard clauses
- [ ] Codegen: Efficient decision tree (jump table?)
- [ ] Destructuring: Struct/array pattern binding

**Zorluk:** YÜKSEK - Pattern matching engine karmaşık

---

### 4. Operator Overloading - DÜŞÜK ÖNCELİK
**Amaç:** Custom türler için operatörler

**Sözdizimi:**
```mlp
struct Vector
    numeric x
    numeric y
    
    -- Operator overload
    operator +(Vector other) -> Vector
        Vector result
        result.x = this.x + other.x
        result.y = this.y + other.y
        return result
    end operator
    
    operator ==(Vector other) -> numeric
        return this.x == other.x && this.y == other.y
    end operator
end struct

-- Usage
Vector v1, v2, v3
v1.x = 1
v1.y = 2
v2.x = 3
v2.y = 4
v3 = v1 + v2  -- Operator overload çağrılır
print(v3.x)   -- 4
```

**Gereksinimler:**
- [ ] Lexer: `operator` keyword
- [ ] Parser: Operator function definitions
- [ ] Type checking: Operator signature validation
- [ ] Codegen: Operator function calls
- [ ] Operator precedence: Existing system ile uyumluluk

**Zorluk:** ORTA - Parser ve codegen değişiklikleri

---

### 5. Interface / Trait System - DÜŞÜK ÖNCELİK
**Amaç:** Polymorphism ve contract programming

**Sözdizimi:**
```mlp
interface Printable
    func toString() -> text
end interface

struct Person implements Printable
    text name
    numeric age
    
    func toString() -> text
        return "Person: " + name
    end func
end struct

func printObject(Printable obj)
    print(obj.toString())
end func
```

**Gereksinimler:**
- [ ] Lexer: `interface`, `implements` keywords
- [ ] Parser: Interface declarations
- [ ] Type checking: Interface compliance
- [ ] Codegen: Virtual method tables (vtable)
- [ ] Dynamic dispatch

**Zorluk:** ÇOK YÜKSEK - Runtime polymorphism gerekiyor

---

## 🔧 İyileştirmeler ve Optimizasyonlar

### Compiler Optimizations
- [ ] Dead code elimination
- [ ] Constant folding
- [ ] Register allocation optimization
- [ ] Tail call optimization
- [ ] Inline functions

### Developer Experience
- [ ] Better error messages (line numbers, context)
- [ ] Warning system
- [ ] LSP (Language Server Protocol) support
- [ ] Syntax highlighter (VS Code extension)
- [ ] Debugger support

### Standard Library
- [ ] Math functions (sin, cos, sqrt, pow)
- [ ] More string utilities
- [ ] HashMap/Dictionary
- [ ] Regex support
- [ ] JSON parsing
- [ ] HTTP client

### Runtime Improvements
- [ ] Memory pooling
- [ ] Garbage collection (reference counting?)
- [ ] Multi-threading support
- [ ] Stack overflow protection

---

## 📊 Öncelik Matrisi

| Özellik | Öncelik | Zorluk | Değer | Notlar |
|---------|---------|--------|-------|--------|
| Modül Sistemi | ⭐⭐⭐⭐⭐ | ORTA | YÜKSEKİ | Kod organizasyonu kritik |
| Generics | ⭐⭐⭐⭐ | YÜKSEK | YÜKSEK | Reusability için önemli |
| Pattern Matching | ⭐⭐⭐ | YÜKSEK | ORTA | Nice-to-have |
| Operator Overload | ⭐⭐ | ORTA | ORTA | Sugar syntax |
| Interface/Trait | ⭐ | ÇOK YÜKSEK | YÜKSEK | Polymorphism gerekirse |

---

## 🎯 Önerilen Sıra

1. **Modül Sistemi** - En pratik ve gerekli özellik
2. **Generics** - Type safety ve reusability için
3. **Pattern Matching** - Modern language feel için
4. **Operator Overloading** - Quality of life
5. **Interface/Trait** - Eğer OOP gerekirse

---

## 📝 Notlar

- Her özellik için ayrı branch kullan
- Testler yazılmadan feature complete değildir
- Backward compatibility koru (mevcut kodlar çalışmalı)
- Dokümantasyon her feature ile beraber güncellenmelidir

**Son Durum:** Exception handling tamamlandı, modül sistemi sırada! 🚀
