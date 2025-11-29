# MELP Phase 3: Turing-Complete Bootstrap Compiler

## 🎯 Proje Amacı

MELP, **kademeli self-hosting** stratejisi ile gelişen bir programlama dilidir.  
**Phase 12 tamamlandı** - MELP artık **CLI argümanları, string utilities ve text parametreleri** destekliyor! 🎉

```mlp
-- Text parameters in functions (NEW!)
func greet(text name)
    text msg = "Hello " + name
    print(msg)
end func

-- String utilities (NEW!)
text str = "Hello World"
print(str_length(str))        -- 11
print(substring(str, 0, 5))   -- "Hello"
print(indexOf(str, "World"))  -- 6
print(charAt(str, 0))         -- "H"

-- CLI arguments (NEW!)
func main()
    numeric argc = get_argc()  -- Argument count
    print(argc)
end func

-- Async/Await (NEW! - Compiled as sync)
async func fetchData(numeric id)
    return id * 100
end func

async func processData()
    numeric result = await fetchData(5)
    print(result)  -- 500
end func

-- Debug features (Development mode)
debug basla  -- Debug label
print(x)
debug if x > 10 then
    debug goto basla  -- Intentional infinite loop for testing
debug end if
debug pause  -- Breakpoint (int3)

-- Pointers (Phase 10)
numeric x = 42
numeric* ptr = &x
print(*ptr)  -- 42
*ptr = 100
print(x)     -- 100

-- Structs (Phase 6)
struct Person
    text name
    numeric age
end struct

Person alice
alice.name = "Alice"

-- Ternary operator (Phase 6)
numeric max = a > b ? a : b
text status = age >= 18 ? "adult" : "minor"
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

## 🐛 Debug Features (Development Mode)

MELP, geliştirme sırasında kullanılmak üzere özel debug statement'ları içerir:

```mlp
debug label_name        -- Debug label tanımla
debug goto label_name   -- Debug label'a atla
debug if condition then -- Debug koşullu blok
    -- code
debug end if
debug pause            -- Breakpoint (int3 instruction)
```

**ÖNEMLİ:** Debug statement'ları **kasıtlı olarak** sonsuz döngü oluşturabilir!

```mlp
debug basla
print(x)
debug if x = x then
    debug goto basla  -- Bu SONSUZ DÖNGÜ oluşturur ve bu BEKLENİLEN bir durumdur!
debug end if
```

**Neden böyle?**
- Debug blokları IDE debugger ile adım adım test için tasarlanmıştır
- Geliştirme sırasında belirli kod bölümlerini tekrar tekrar çalıştırmak için kullanılır
- Production build'de (`--release` flag ile) tüm debug statement'ları otomatik olarak çıkarılır
- Debug mode'da sonsuz döngüler **bug değil, feature'dır**!

**Kullanım Senaryosu:**
1. Geliştirme: `./melp-bootstrap --debug mycode.mlp` → Debug blokları aktif
2. Production: `./melp-bootstrap --release mycode.mlp` → Debug blokları stripped

**Not:** `--debug`/`--release` flag implementasyonu henüz tamamlanmamıştır. Şu anda tüm debug statement'ları assembly'e dahil edilir.

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

### ✅ Phase 4 - Functions (TAMAMLANDI!)
- **Function definition**: `func add(numeric a, numeric b) ... end func`
- **Return**: `return a + b`
- **Function calls**: `x = add(5, 3)`
- **Parameters**: Multiple parameters with type checking
- **Call stack**: Proper frame management

### ✅ Phase 5 - Strings (TAMAMLANDI!)
- **String literals**: `text name = "Alice"`
- **String concatenation**: `text msg = "Hello " + name`
- **String comparison**: `name == "Alice"`
- **Built-in functions**: `length(str)`
- **Print support**: `print("Hello")` or `print(name)`

### ✅ Phase 6 - Structs & Ternary (TAMAMLANDI!)
- **Struct definitions**: `struct Person ... end struct`
- **Struct instances**: `Person p`
- **Field access**: `p.name`, `p.age`
- **Ternary operator**: `max = a > b ? a : b`
- **Nested ternary**: Full support for complex conditionals
- **Type-aware**: Works with all types (numeric, text, boolean)

### 🎯 MELP TURING-COMPLETE & PRACTICAL! 
Variables + Conditionals + Loops + Functions + Strings + Structs + I/O = Real-world programming!


### ✅ Phase 7 - String Operations (Kısmen tamamlandı)
- **String concatenation**: `text full = "Hello " + "World"`
- **Comparison operators**: `==`, `!=`, `<`, `>`, `<=`, `>=`

### ✅ Phase 8 - Arrays (TAMAMLANDI!)
- **Array declarations**: `numeric[] arr = malloc(10)`
- **Array indexing**: `arr[0] = 42`, `x = arr[i]`
- **Dynamic allocation**: Arrays created at runtime

### ✅ Phase 9 - File I/O (TAMAMLANDI!)
- **read_file(filename)**: Read entire file as text
- **write_file(filename, content)**: Write text to file
- **append_file(filename, content)**: Append to file

### ✅ Phase 10 - Pointers (TAMAMLANDI!)
- **Pointer types**: `numeric* ptr`
- **Address-of**: `ptr = &variable`
- **Dereference**: `x = *ptr`, `*ptr = 100`
- **Pointer parameters**: `func modify(numeric* ptr)`

### ✅ Phase 11 - String Utilities (TAMAMLANDI!)
- **substring(str, start, length)**: Extract substring
- **indexOf(str, substr)**: Find substring position (-1 if not found)
- **charAt(str, index)**: Get character at index
- **str_length(str)**: Get string length

### ✅ Phase 12 - CLI Arguments (TAMAMLANDI!)
- **get_argc()**: Get argument count
- **mlp_get_argv()**: Convert C argv to MLP string array
- **Stack-based extraction**: argc/argv read from stack at _start
- **Text parameters**: Functions can now accept `text` parameters

### 🔄 Phase 13 - Self-Hosting (İlerliyor)
- **Lexer prototypes**: lexer_simple.mlp, lexer_v2.mlp, lexer_minimal.mlp tested
- **Syntax constraints identified**: Global text/arrays problematic, all functions need return values
- **Next**: Complete lexer, then parser, then codegen in MLP

### ⏳ Gelecek Fazlar
- **Enums**: Numeric constant groups
- **Full self-hosting**: Compiler written entirely in MLP
- **Standard library**: More built-in functions


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
**Versiyon:** 0.6.0 (Phase 6 - Structs & Ternary Operator!)  
**Milestone:** Variables, loops, functions, strings, structs, ternary - MELP is now practical! 🎉
