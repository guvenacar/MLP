# MELP Phase 0: Minimal Self-Hosting Compiler

## 🎯 Proje Amacı

MELP, **kademeli self-hosting** stratejisi ile gelişen bir programlama dilidir.  
Phase 0'da **sadece değişken deklarasyonları** desteklenir:

```decl
numeric x
decimal y
boolean flag
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

### Phase 0 (ŞU AN)
- ✅ Deklarasyonlar: `numeric`, `decimal`, `boolean`
- ✅ x86-64 Assembly üretimi
- ✅ Stack allocation
- ✅ Minimal self-hosting (state tanımları)

### Phase 1 (SONRAKI)
- ⏳ Assignment: `x = 42`
- ⏳ Arithmetic: `x = a + b`

### Phase 2 (GELECEK)
- ⏳ If-then: `if x > 5 then ... end if`
- ⏳ Comparison: `>`, `<`, `==`

### Phase 3 (GELECEK)
- ⏳ For loop: `for i = 0 to 10 ... end for`
- ⏳ While loop: `while x < 10 ... end while`

### Phase 4 (GELECEK)
- ⏳ Functions: `function add(numeric a, numeric b) ... end function`
- ⏳ Return: `return a + b`

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
**Versiyon:** 0.1.0 (Phase 0)
