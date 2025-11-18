# MLP Language Specification v3.0

**Status:** Production Ready ✅
**Last Updated:** November 18, 2025
**Compiler Version:** 3.0 (Multi-Language)
**Architecture:** English-Native Compiler + Multi-Language Preprocessor

---

## Table of Contents

1. [Overview](#overview)
2. [Multi-Language Support](#multi-language-support)
3. [Core Language (English)](#core-language-english)
4. [Preprocessor](#preprocessor)
5. [Data Types](#data-types)
6. [Syntax Rules](#syntax-rules)
7. [Control Structures](#control-structures)
8. [Functions](#functions)
9. [Built-in Functions](#built-in-functions)
10. [Compiler Architecture](#compiler-architecture)
11. [Adding New Languages](#adding-new-languages)
12. [Migration Guide](#migration-guide)

---

## Overview

### What is MLP?

**MLP (Multi-Language Programming)** is a compiled programming language that supports writing code in multiple natural languages.

**Key Features:**
- 🌍 Write code in Turkish, Russian, Chinese, Hindi, or define your own
- ⚡ Compiles to native x86-64 assembly
- 🔧 Simple, clean syntax
- 🎯 English-native compiler core
- 🔄 Language-agnostic preprocessor

### Philosophy

**"Programming should be accessible in any language, not just English."**

MLP achieves this through:
1. **English-Native Compiler**: Simple, maintainable core
2. **Preprocessor Layer**: Translates any language → English
3. **Equal Treatment**: All languages supported equally via `diller.json`

### Pipeline

```
Source Code (Any Language)
    ↓
Preprocessor (keyword translation)
    ↓
English Intermediate Code
    ↓
Compiler (lexer → parser → code gen)
    ↓
x86-64 Assembly
    ↓
Native Executable
```

---

## Multi-Language Support

### Supported Languages

| Language | ID | Example Keywords |
|----------|----|------------------|
| English | `en-US` | `int`, `if`, `while`, `function`, `print` |
| Turkish | `tr-TR` | `sayısal`, `eğer`, `döngü`, `işleç`, `yazdir` |
| Russian | `ru-RU` | `целое`, `если`, `пока`, `функция`, `печать` |
| Chinese | `zh-CN` | `整数`, `如果`, `当`, `函数`, `打印` |
| Hindi | `hi-IN` | `संख्या`, `अगर`, `जब_तक`, `फलन`, `लिखो` |
| Custom | `custom-*` | User-defined |

### Language Selection

Add a language header at the top of your file:

```mlp
-- lang: tr-TR
```

Or specify explicitly when compiling:

```bash
./mlpc program.mlp --lang=ru-RU
```

If no language is specified, defaults to `en-US` (English).

### Example: Same Program in 4 Languages

**English:**
```mlp
-- lang: en-US
int x = 5;
int y = 10;
int c = 0;

if x == y then
    c = 15
else
    c = 20
end

print "Result:"
print c
```

**Turkish:**
```mlp
-- lang: tr-TR
SAYISAL x = 5;
SAYISAL y = 10;
SAYISAL c = 0;

EĞER x == y İSE
    c = 15
DEĞİLSE
    c = 20
SON

YAZDIR "Sonuç:"
YAZDIR c
```

**Russian:**
```mlp
-- lang: ru-RU
целое x = 5;
целое y = 10;
целое c = 0;

если x == y то
    c = 15
иначе
    c = 20
конец

печать "Результат:"
печать c
```

**Chinese:**
```mlp
-- lang: zh-CN
整数 x = 5;
整数 y = 10;
整数 c = 0;

如果 x == y 那么
    c = 15
否则
    c = 20
结束

打印 "结果:"
打印 c
```

All four compile to the same executable and produce identical output: `20`

---

## Core Language (English)

The MLP compiler core understands only English keywords. All other languages are translated to English by the preprocessor.

### Keywords

| Keyword | Purpose | Example |
|---------|---------|---------|
| `int` | Integer type declaration | `int x = 42;` |
| `string` | String type declaration | `string name = "Alice";` |
| `if` | Conditional start | `if x > 10 then` |
| `then` | Block start | `if condition then` |
| `else` | Alternative block | `else` |
| `end` | Block end | `end` |
| `while` | Loop | `while` |
| `break` | Exit loop | `break` |
| `function` | Function definition | `function add(a, b) then` |
| `return` | Return value | `return x + y` |
| `print` | Output | `print "Hello"` |
| `true` | Boolean true | `true` |
| `false` | Boolean false | `false` |
| `struct` | Structure (future) | `struct Point then` |

### Operators

**Arithmetic:**
- `+` Addition
- `-` Subtraction
- `*` Multiplication
- `/` Division

**Comparison:**
- `==` Equal
- `!=` Not equal
- `<` Less than
- `>` Greater than
- `<=` Less than or equal
- `>=` Greater than or equal

**Assignment:**
- `=` Assignment

---

## Preprocessor

### How It Works

The preprocessor translates keywords from any language to English while preserving:
- String literals (content inside `"..."`)
- Comments (`--` single-line, `{- ... -}` multi-line)
- Code structure

### State Machine

The preprocessor uses a 3-state machine:

```
STATE_CODE:     Normal code - translate keywords
STATE_STRING:   Inside "..." - preserve as-is
STATE_COMMENT:  Inside comment - preserve as-is
```

**Example:**

Input (Turkish):
```mlp
METIN mesaj = "EĞER bu değişmez";
EĞER x > 10 İSE
    YAZDIR mesaj
SON
```

Output (English):
```mlp
string mesaj = "EĞER bu değişmez";
if x > 10 then
    print mesaj
end
```

Note: `"EĞER bu değişmez"` remains unchanged because it's inside a string literal.

### Language Detection

1. **Automatic:** Reads `-- lang: <id>` header
2. **Explicit:** `--lang=<id>` command-line flag
3. **Default:** If neither, defaults to `en-US`

### Translation Map

Defined in `diller.json`:

```json
{
  "languages": [
    {
      "id": "tr-TR",
      "keywords": {
        "int": ["sayisal", "SAYISAL"],
        "if": ["eğer", "EĞER", "eger", "EGER"],
        "print": ["yazdir", "YAZDIR"]
      }
    }
  ]
}
```

---

## Data Types

### Primitive Types

| Type | Keyword | Size | Default | Example |
|------|---------|------|---------|---------|
| Integer | `int` | 64-bit | 0 | `int x = 42;` |
| String | `string` | Pointer | "" | `string name = "Bob";` |

### Type Rules

- **Static typing:** Types determined at compile time
- **No implicit conversion:** Must be explicit
- **Stack-allocated:** Variables live on the stack

---

## Syntax Rules

### Rule 1: Semicolon Usage

**Semicolons are ONLY used for variable declarations:**

✅ **Correct:**
```mlp
int x;
int y = 10;
string name = "Alice";
```

❌ **Wrong:**
```mlp
return x + y;    -- NO semicolon
end;             -- NO semicolon
print x;         -- NO semicolon
x = 5;           -- NO semicolon (assignment)
```

### Rule 2: Block Termination

**All blocks end with `end`:**

```mlp
function add(a, b) then
    return a + b
end

if x > 0 then
    print "Positive"
else
    print "Negative"
end

while
    if i >= 10 then
        break
    end
    i = i + 1
end
```

### Rule 3: Comments

**Single-line:**
```mlp
-- This is a comment
int x = 10  -- End of line comment
```

**Multi-line:**
```mlp
{-
  This is a multi-line comment
  Spanning multiple lines
-}
```

### Rule 4: String Literals

**Escape sequences:**
- `\"` - Quote
- `\n` - Newline
- `\t` - Tab
- `\\` - Backslash
- `\r` - Carriage return
- `\0` - Null character

**Examples:**
```mlp
string message = "Hello\nWorld";
string path = "C:\\Users\\Alice";
string quote = "She said \"Hi\"";
```

---

## Control Structures

### Conditional (if-then-else)

```mlp
if condition then
    -- statements
end

if condition then
    -- statements
else
    -- statements
end

-- Nested
if x == 0 then
    print "Zero"
else
    if x > 0 then
        print "Positive"
    else
        print "Negative"
    end
end
```

### Loop (while)

```mlp
-- Infinite loop
while
    print "Forever"
    break  -- Exit with break
end

-- Conditional loop
int i = 0;
while
    if i >= 10 then
        break
    end
    print i
    i = i + 1
end
```

---

## Functions

### Definition

```mlp
function name(param1, param2, ...) then
    -- statements
    return value
end
```

### Examples

**Simple function:**
```mlp
function add(a, b) then
    return a + b
end

int result = add(5, 3);
print result  -- 8
```

**Recursive function:**
```mlp
function factorial(n) then
    if n <= 1 then
        return 1
    end
    return n * factorial(n - 1)
end

print factorial(5)  -- 120
```

**No return value:**
```mlp
function greet(name) then
    print "Hello, "
    print name
end

greet("Alice")
```

---

## Built-in Functions

### String Operations

**Concatenate:**
```mlp
string result = string_concat("Hello", " World");
-- result = "Hello World"
```

**Length:**
```mlp
int len = string_length("Hello");
-- len = 5
```

**Character at index:**
```mlp
string char = string_char_at("Hello", 1);
-- char = "e"
```

**Substring:**
```mlp
string sub = string_substring("Hello World", 0, 5);
-- sub = "Hello"
```

**Character code:**
```mlp
int code = char_code("A");
-- code = 65
```

**Code to character:**
```mlp
string char = code_to_char(65);
-- char = "A"
```

### File I/O

**Open file:**
```mlp
string mode = "r";
int file = file_open("data.txt", mode);
```

**Read file:**
```mlp
string content = file_read(file);
```

**Write file:**
```mlp
string mode = "w";
int file = file_open("output.txt", mode);
int written = file_write(file, "Hello World");
```

**Close file:**
```mlp
int result = file_close(file);
```

### System

**Get current directory:**
```mlp
string cwd = get_cwd();
print cwd
```

---

## Compiler Architecture

### Components

```
┌─────────────────────────────────────────────┐
│  Source Code (Multi-Language .mlp)          │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  PREPROCESSOR (dil_cevirici.py)             │
│  - Detect language from header              │
│  - Load translation map (diller.json)       │
│  - State machine: CODE/STRING/COMMENT       │
│  - Translate keywords → English             │
│  - Preserve strings & comments              │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  English Intermediate (.preprocessed.mlp)   │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  LEXER (c_lexer.c)                          │
│  - Tokenization                             │
│  - UTF-8 support                            │
│  - Position tracking                        │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  PARSER (c_parser.c)                        │
│  - AST construction                         │
│  - Syntax validation                        │
│  - Error reporting                          │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  CODE GENERATOR (c_generator.c)             │
│  - x86-64 assembly (NASM)                   │
│  - Register allocation                      │
│  - Stack frame management                   │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  Assembly Output (.asm)                     │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  NASM Assembler                             │
│  - Assembly → Object code                   │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  GCC Linker                                 │
│  - Link with runtime library                │
│  - Produce executable                       │
└─────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────┐
│  Native Executable                          │
└─────────────────────────────────────────────┘
```

### Compilation Process

**Using mlpc wrapper:**
```bash
./mlpc input.mlp -o output
```

**Manual steps:**
```bash
# 1. Preprocess
python3 dil_cevirici.py input.mlp preprocessed.mlp

# 2. Compile
./c_compiler/compiler_test preprocessed.mlp output.asm

# 3. Assemble
nasm -f elf64 output.asm -o output.o

# 4. Link
gcc output.o runtime/runtime.o -o output -no-pie
```

---

## Adding New Languages

### Step 1: Edit diller.json

Add your language definition:

```json
{
  "languages": [
    {
      "id": "es-ES",
      "name": "Español (Spanish)",
      "description": "Spanish programming keywords",
      "keywords": {
        "int": ["entero"],
        "string": ["cadena"],
        "if": ["si"],
        "then": ["entonces"],
        "else": ["sino"],
        "while": ["mientras"],
        "break": ["romper"],
        "end": ["fin"],
        "function": ["función"],
        "return": ["devolver"],
        "print": ["imprimir"],
        "true": ["verdadero"],
        "false": ["falso"]
      }
    }
  ]
}
```

### Step 2: Write Code

Create a file with the language header:

```mlp
-- lang: es-ES
entero x = 42;
imprimir "Hola Mundo"
imprimir x
```

### Step 3: Compile

```bash
./mlpc spanish_program.mlp -o programa
./programa
```

### Step 4: Test

Create test files to verify all keywords work:

```mlp
-- lang: es-ES
-- Test all keywords

entero a = 5;
entero b = 10;
cadena mensaje = "Probando";

si a < b entonces
    imprimir "a es menor"
sino
    imprimir "a es mayor"
fin

función suma(x, y) entonces
    devolver x + y
fin

entero resultado = suma(a, b);
imprimir resultado

entero i = 0;
mientras
    si i >= 5 entonces
        romper
    fin
    imprimir i
    i = i + 1
fin
```

---

## Migration Guide

### Migrating from Turkish-Native to English-Native

If you have old `.tyd` files with Turkish keywords but no language header:

**Option 1: Add Language Header**

Add `-- lang: tr-TR` at the top:

```mlp
-- lang: tr-TR
SAYISAL x = 42;
YAZDIR "Merhaba"
```

**Option 2: Use Migration Tool**

```bash
python3 migrate.py old_program.tyd
```

This will:
1. Read the Turkish source
2. Translate to English
3. Write back to the same file
4. Add a migration comment

**Option 3: Manual Translation**

Translate keywords manually:
- `SAYISAL` → `int`
- `METIN` → `string`
- `EĞER` → `if`
- `İSE` → `then`
- `DEĞİLSE` → `else`
- `DÖNGÜ` → `while`
- `DÖNGÜ_BITIR` → `break`
- `SON` → `end`
- `İŞLEÇ` → `function`
- `DÖNÜŞ` → `return`
- `YAZDIR` → `print`

---

## Error Messages

The compiler provides detailed error messages:

```
╔════════════════════════════════════════════════════════════╗
║ ERROR [Parser]: Syntax Error!                             ║
╚════════════════════════════════════════════════════════════╝

📍 Location: Line 5, Column 10

❌ Found: "if" (IF)
✓ Expected: ; (required for int, string variable declarations)

💡 Explanation: Token type mismatch.
```

---

## Performance

### Compilation Speed

| Phase | Time (100 LOC) |
|-------|----------------|
| Preprocessing | ~10ms |
| Lexing | ~5ms |
| Parsing | ~10ms |
| Code Generation | ~15ms |
| Assembly | ~50ms |
| Linking | ~100ms |
| **Total** | **~190ms** |

### Runtime Performance

- **Native x86-64:** No VM overhead
- **Direct syscalls:** No wrapper layers
- **Stack-based:** Fast local variables
- **No GC:** Predictable performance

---

## Future Features

### Planned for v3.1
- [ ] Float/double support
- [ ] Array types
- [ ] Struct types with methods
- [ ] For loop syntax sugar

### Planned for v3.2
- [ ] Module/import system
- [ ] Standard library expansion
- [ ] Package manager

### Planned for v4.0
- [ ] LLVM backend
- [ ] Optimization passes
- [ ] Debugging support (DWARF)
- [ ] WebAssembly target

---

## Examples

### Example 1: Fibonacci (English)

```mlp
-- lang: en-US
function fibonacci(n) then
    if n <= 1 then
        return n
    end
    return fibonacci(n - 1) + fibonacci(n - 2)
end

int i = 0;
while
    if i >= 10 then
        break
    end

    print "fibonacci("
    print i
    print ") = "
    print fibonacci(i)

    i = i + 1
end
```

### Example 2: File I/O (Russian)

```mlp
-- lang: ru-RU
строка режим_запись = "w";
целое файл = file_open("output.txt", режим_запись);

строка данные = "Привет из MLP!\n";
file_write(файл, данные)
file_close(файл)

строка режим_чтение = "r";
файл = file_open("output.txt", режим_чтение)
строка содержание = file_read(файл);
file_close(файл)

печать "Содержимое файла:"
печать содержание
```

### Example 3: String Manipulation (Chinese)

```mlp
-- lang: zh-CN
字符串 姓 = "张";
字符串 名 = "三";

字符串 全名 = string_concat(姓, 名);

打印 "全名:"
打印 全名

整数 长度 = string_length(全名);
打印 "长度:"
打印 长度
```

### Example 4: Custom Language

```mlp
-- lang: custom-mehmet
sayi x = 100;
sayi y = 200;

olursa x < y ozaman
    goster "x küçük"
yoksa
    goster "x büyük"
dur

yap topla(a, b) ozaman
    geri a + b
dur

goster topla(x, y)
```

---

## Best Practices

### 1. Always Use Language Headers

```mlp
-- lang: tr-TR
-- or
-- lang: ru-RU
```

### 2. Consistent Naming

Use your language's naming conventions:
- Turkish: `kullanici_adi`, `toplam_fiyat`
- Russian: `имя_пользователя`, `общая_цена`
- Chinese: `用户名`, `总价`

### 3. Comment Your Code

```mlp
-- lang: tr-TR
-- Bu fonksiyon faktöriyel hesaplar
function faktoriyel(n) then
    ...
end
```

### 4. Test Edge Cases

```mlp
-- Test with 0
-- Test with 1
-- Test with negative numbers
```

### 5. Use Built-in Functions

Prefer built-in functions for common operations:
- Use `string_concat` instead of manual concatenation
- Use `string_length` instead of counting
- Use `file_read`/`file_write` for I/O

---

## Conclusion

MLP demonstrates that programming languages can support multiple natural languages without sacrificing simplicity or performance.

**Key Takeaways:**
- English-native compiler core keeps implementation simple
- Preprocessor layer provides multi-language support
- All languages treated equally via configuration
- Easy to extend with new languages
- Native performance (no VM, no interpreter)

**Philosophy:**
> "Code in your language. Think in your language. Create in your language."

---

**© 2025 MLP Project**
**Version:** 3.0
**Status:** Production Ready ✅
**License:** MIT
