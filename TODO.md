# MELP TODO List

**Son Güncelleme:** 29 Kasım 2025  
**Durum:** Phase 12 Complete, Exception Handling Complete

---

## ✅ Tamamlanan Özellikler

### Phase 0-12 (Bootstrap Foundation)
- [x] Variables (numeric, text)
- [x] Arithmetic operations
- [x] Print statements
- [x] If-else conditionals
- [x] While loops
- [x] Do-while loops
- [x] Functions
- [x] Arrays
- [x] Structs
- [x] Pointers
- [x] Lambda expressions
- [x] Closures
- [x] CLI arguments (argc/argv)
- [x] String utilities (length, concat, substring, etc.)
- [x] File operations (read, write, append)
- [x] For loops
- [x] Ternary operator
- [x] Logical operators (&&, ||, !)
- [x] Enum types

### Modern Language Features
- [x] **Exception Handling** (29 Kasım 2025)
  - try-catch-finally blokları
  - Çoklu catch (tip kontrolü)
  - throw statement
  - Yakalanmamış istisna yönetimi
  - İç içe try-catch desteği

---

## 🚀 Sıradaki Özellikler

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
