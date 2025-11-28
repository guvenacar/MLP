# MELP Phase 3: Turing-Complete Bootstrap Compiler

## 🎯 Proje Amacı

MELP, **kademeli self-hosting** stratejisi ile gelişen bir programlama dilidir.  
**Phase 3 tamamlandı** - MELP artık **Turing-complete**! 🎉

```mlp
-- Variables, conditionals, loops, arithmetic, I/O
numeric sum = 0
for i = 0 to 10 step 2
    if i > 5 then
        exit
    end if
    sum = sum + i
    print(sum)
end for
```

## 📁 Dizin Yapısı

```
melp/
├── bootstrap/          # C'de yazılmış bootstrap compiler
│   ├── lexer.c
│   ├── parser.c
│   ├── codegen.c
│   └── main.c
├── compiler/           # MELP'de yazılmış compiler (self-hosting)
│   └── compiler_state.mlp
├── runtime/            # C runtime (minimal)
│   ├── runtime.c
│   └── runtime.h
├── tests/
│   └── test_simple.mlp
├── syntax.json         # Syntax normalization rules
├── diller.json         # Language translation rules
└── Makefile
```

## 🚀 Hızlı Başlangıç

### 1. Bootstrap Compiler'ı Build Et

```bash
make bootstrap
```

Bu, C'de yazılmış compiler'ı derler: `./melp-bootstrap`

### 2. Test Et

```bash
make test
```

Bu:
- `tests/test_simple.mlp` dosyasını derler
- x86-64 assembly üretir
- Assembly'i derleyip çalıştırır

### 3. Kendi Kodunu Derle

```bash
./melp-bootstrap mycode.mlp mycode.s
nasm -f elf64 mycode.s -o mycode.o
ld mycode.o -o mycode
./mycode
```

## 🔬 Self-Hosting Kanıtı

Bootstrap compiler, kendi dilimizde yazılmış kodu derleyebilir:

```bash
./melp-bootstrap compiler/compiler_state.mlp compiler/compiler_state.s
```

Bu dosya (`compiler_state.mlp`), compiler'ın state değişkenlerini tanımlar!

## 📊 Desteklenen Özellikler

### ✅ Phase 0 - Deklarasyonlar
- Değişken tipleri: `numeric`, `decimal`, `boolean`
- x86-64 Assembly üretimi
- Stack allocation

### ✅ Phase 1 - Assignment ve Print
- Assignment: `x = 42`, `x = y + 10`
- Print: `print(x)`
- Expression evaluation

### ✅ Phase 2 - Conditionals
- If-then-else: `if x > 5 then ... else ... end if`
- Comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Conditional jumps

### ✅ Phase 3 - Loops (TAMAMLANDI!)
- **For loops**: `for i = 0 to 10 step 2 ... end for`
- **While loops**: `while x < 10 ... end while`
- **Exit**: Döngüden erken çıkış
- **Continue**: Sonraki iterasyona geç
- Arithmetic operators: `+`, `-`, `*`, `/`
- Nested loop support

### 🎯 MELP TURING-COMPLETE! 
Variables + Conditionals + Loops + Arithmetic + I/O = Turing Completeness

### ⏳ Phase 4 (SONRAKİ)
- Functions: `func add(numeric a, numeric b) ... end func`
- Return: `return a + b`
- Function calls: `x = add(5, 3)`

### ⏳ Phase 5+ (GELECEK)
- Structs ve OOP
- Arrays
- String manipulation
- Advanced features

## 🎓 Mimari

```
Kullanıcı Kodu (.mlp)
    ↓
[Lexer]  → Tokenization
    ↓
[Parser] → AST oluştur
    ↓
[Codegen] → x86-64 Assembly
    ↓
NASM → Object file
    ↓
LD → Executable
```

## 📚 Öğrenme Kaynakları

- `SELF_HOSTING_STRATEGY.md` - Self-hosting stratejisi
- `kurallar_kitabı.md` (ana dizin) - Genel proje kuralları

## 🤝 Katkıda Bulunma

Her phase kademeli olarak eklenir. Yeni özellik eklerken:
1. Bootstrap compiler'ı güncelle (C)
2. Yeni özelliği test et
3. Self-hosting version'u yaz (MELP)
4. Tekrar test et

## 📝 Lisans

Bu eğitim projesidir.

---

**Son Güncelleme:** 28 Kasım 2025  
**Versiyon:** 0.3.0 (Phase 3 - Turing Complete!)  
**Milestone:** Variables, conditionals, loops, arithmetic, I/O - MELP is now Turing-complete! 🎉
