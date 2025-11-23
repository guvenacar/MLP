# MLP Language Specification v3.0

**Status:** Production Ready ✅ + Phase 8 (Async/Await) 🚀
**Last Updated:** November 23, 2025
**Compiler Version:** 3.0 + Phase 8.4 (Async/Await Basic Implementation)
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
13. **[NEW: Phase 6 - Literal & Generic Types](#phase-6-literal-and-generic-types)** 🚀
14. **[NEW: Phase 8 - Async/Await & Concurrency](#phase-8-asyncawait--concurrency)** 🚀

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
int x = 5
int y = 10;
int c = 0;

if x == y then
    c = 15
else
    c = 20
end func

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
| `function` | Function definition | `func add(a, b)` |
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

### 🔴 CRITICAL ARCHITECTURE RULE

**THE LEXER MUST NEVER SEE NON-ENGLISH KEYWORDS!**

```
❌ WRONG ARCHITECTURE:
Turkish Source → Lexer (add YAZDIR token) → Parser

✅ CORRECT ARCHITECTURE:
Turkish Source → Preprocessor (YAZDIR→print) → English IR → Lexer → Parser
```

**Why this matters:**
- Lexer/Parser understand ONLY English keywords
- Multi-language support is PREPROCESSOR's responsibility
- Assembly generated from English keywords ONLY
- Adding new language = just edit diller.json (10 minutes)
- Compiler core never changes for new languages

### How It Works

**Preprocessor Pipeline:**

```
Input: Turkish/Russian/Hindi .mlp file
  ↓
1. Detect language from "-- lang: XX-XX" header
  ↓
2. Load keyword mappings from diller.json
  ↓
3. Process line by line with state machine:
   STATE_CODE     → Translate keywords
   STATE_STRING   → Preserve UTF-8 content
   STATE_COMMENT  → Preserve as-is
  ↓
4. Output: English .mlp with preserved UTF-8 strings
  ↓
Compiler sees: English keywords + UTF-8 strings
```

**What gets translated:**
- Keywords: YAZDIR → print, EĞER → if, SAYISAL → numeric
- Type names: METIN → string, MANTIKSAL → boolean

**What gets preserved:**
- String contents: `"Merhaba Dünya"` stays as-is (UTF-8)
- Comments: `-- Bu yorum` stays as-is
- String escape sequences: `\n`, `\t`, `\"`

### State Machine

The preprocessor uses a 3-state machine:

```
STATE_CODE:     Normal code - translate keywords
STATE_STRING:   Inside "..." - preserve UTF-8 as-is
STATE_COMMENT:  Inside comment - preserve as-is
```

### UTF-8 String Handling in Compiler

**Problem:** NASM doesn't accept UTF-8 bytes directly in strings

**Solution:** Byte sequence approach (in visit_Metin() ~line 4883)

```c
// 1. Detect UTF-8
int has_utf8 = 0;
for (const char* p = string; *p; p++) {
    if ((unsigned char)*p >= 128) {  // Non-ASCII byte
        has_utf8 = 1;
        break;
    }
}

// 2. Output format based on content
if (has_utf8) {
    // UTF-8 → Byte sequence
    // "Merhaba" → db 77,101,114,104,97,98,97,0
    sprintf(buffer, "%s: db ", label);
    for (const char* p = string; *p; p++) {
        sprintf(buffer + strlen(buffer), "%d,", (unsigned char)*p);
    }
    strcat(buffer, "0");
} else {
    // ASCII → Quoted string with smart delimiter
    // "Hello" → db "Hello", 0
    // "Say \"Hi\"" → db 'Say "Hi"', 0
    int has_quote = (strchr(string, '"') != NULL);
    char delim = has_quote ? '\'' : '"';
    sprintf(buffer, "%s: db %c%s%c, 0", label, delim, string, delim);
}
```

**Result:**
- Turkish "Merhaba" displays correctly
- Russian "Привет" displays correctly
- Hindi "नमस्ते" displays correctly
- Quotes handled: `"Say \"Hi\""` works

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
end func
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

### Rule 1: No Semicolons (Python-Style)

**MLP uses Python-style syntax - NO semicolons anywhere:**

✅ **Correct:**
```mlp
int x
int y = 10
string name = "Alice"
x = 5
print x
return x + y
```

❌ **Wrong:**
```mlp
int x;           -- NO semicolons!
x = 5;           -- NO semicolons!
print x;         -- NO semicolons!
return x + y;    -- NO semicolons!
```

### Rule 2: Block Termination

**All blocks end with `end`:**

```mlp
func add(a, b)
    return a + b
end func

if x > 0 then
    print "Positive"
else
    print "Negative"
end func

while
    if i >= 10 then
        break
    end
    i = i + 1
end func
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
end func

if condition then
    -- statements
else
    -- statements
end func

-- Nested
if x == 0 then
    print "Zero"
else
    if x > 0 then
        print "Positive"
    else
        print "Negative"
    end
end func
```

### Loop (while)

```mlp
-- Infinite loop
while
    print "Forever"
    break  -- Exit with break
end func

-- Conditional loop
int i = 0;
while
    if i >= 10 then
        break
    end
    print i
    i = i + 1
end func
```

---

## Functions

### Definition

```mlp
func name(param1, param2, ...)
    -- statements
    return value
end func
```

### Default Parameters (Phase 5.11) ⭐ NEW

Functions can have default parameter values:

```mlp
func greet(name = "World")
    print "Hello, " + name
end func

greet()           -- Output: Hello, World
greet("Alice")    -- Output: Hello, Alice
```

**Multiple defaults:**
```mlp
func add(a = 10, b = 20)
    return a + b
end func

print add()       -- 30 (10 + 20)
print add(5)      -- 25 (5 + 20)
print add(5, 7)   -- 12 (5 + 7)
```

**Implementation:**
- Two-pass compilation: functions pre-registered before code generation
- Missing arguments automatically filled with default values
- Default values can be any compile-time expression (literals, constants)

### Examples

**Simple function:**
```mlp
func add(a, b)
    return a + b
end func

int result = add(5, 3);
print result  -- 8
```

**Recursive function:**
```mlp
func factorial(n)
    if n <= 1 then
        return 1
    end
    return n * factorial(n - 1)
end func

print factorial(5)  -- 120
```

**No return value:**
```mlp
func greet(name)
    print "Hello, "
    print name
end func

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

### 🏗️ Self-Hosting Status

**MLP IS FULLY SELF-HOSTING** ✅ (Completed: November 22, 2024)

MLP can now compile itself! The compiler has been rewritten in MLP:
- **Lexer:** `self_host/lexer.mlp` - Tokenization in MLP
- **Parser:** `self_host/parser.mlp` - AST construction in MLP  
- **Generator:** `self_host/generator.mlp` - Assembly generation in MLP
- **Main Compiler:** `self_host/mlpc.mlp` - Complete compiler in MLP

### 🔀 Hybrid Architecture (Two Compilation Paths)

MLP supports two distinct compilation pipelines:

#### Path 1: MLP → Assembly (Direct)

```
Source.mlp → self_host/mlpc.mlp → x86-64 Assembly
```

- Uses MLP-written compiler components
- Direct assembly generation via `generator.mlp`
- Faster compilation time
- Pure MLP implementation

#### Path 2: MLP → C → Assembly (Default) ⭐

```
Source.mlp → c_compiler/mlpc → C Intermediate → x86-64 Assembly
```

- Uses C-based bootstrap compiler
- Generates optimized C code first
- Better performance optimizations
- **Recommended for production use**

**Both paths are fully functional and tested!**

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

If you have old `.mlp` files with Turkish keywords but no language header:

**Option 1: Add Language Header**

Add `-- lang: tr-TR` at the top:

```mlp
-- lang: tr-TR
SAYISAL x = 42;
YAZDIR "Merhaba"
```

**Option 2: Use Migration Tool**

```bash
python3 migrate.py old_program.mlp
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
func fibonacci(n)
    if n <= 1 then
        return n
    end
    return fibonacci(n - 1) + fibonacci(n - 2)
end func

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
end func
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
func faktoriyel(n)
    ...
end func
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

---

## Phase 6 - Literal and Generic Types

### Phase 6.1: NULL and Boolean Literals ✅

**Status:** Completed November 22, 2025

MLP now supports NULL, true, and false literals natively.

**New Keywords:**
- `null` - NULL literal (represented as 0 internally)
- `true` - Boolean true (represented as 1)
- `false` - Boolean false (represented as 0)

**Examples:**

```mlp
-- NULL support
numeric x = null
string name = null

if value == null then
    print "Value is null"
end func

-- Boolean literals
boolean flag = true
boolean active = false

if flag == true then
    print "Flag is true"
end func
```

**NULL Comparison:**
```mlp
-- NULL equals 0
if null == 0 then
    print "null == 0: TRUE"
end func

-- Boolean arithmetic
if true == 1 then
    print "true == 1: TRUE"
end func

if false == 0 then
    print "false == 0: TRUE"
end func
```

**Implementation:**
- Added `TOKEN_NULL`, `TOKEN_TRUE`, `TOKEN_FALSE` to lexer
- Updated parser's `birincil()` function to handle these literals
- NULL represented as 0, true as 1, false as 0 in generated assembly

---

### Phase 6.2: Generic Type System ✅

**Status:** 100% Complete (22 Kasım 2025) - Full Implementation Done!

MLP now supports generic types with `optional<T>` as the first implementation.

**Generic Syntax:**

```mlp
-- Generic type declaration
optional<numeric> maybe_number
optional<string> maybe_text

-- Create with NULL
optional<numeric> x = null

-- Type parameter syntax
generic T
optional<T> create_optional(T value)
    -- implementation
end func
```

**Optional<T> Implementation:**

MLP provides a self-hosted Optional<T> library written in pure MLP:

```mlp
-- Optional<numeric> struct
struct OptionalNumeric
    numeric value
    boolean has_value
end struct

-- Create empty optional
func optional_numeric_none()
    OptionalNumeric opt
    opt.value = 0
    opt.has_value = false
    return opt
end func

-- Create optional with value
func optional_numeric_some(numeric val)
    OptionalNumeric opt
    opt.value = val
    opt.has_value = true
    return opt
end func

-- Check if has value
func optional_numeric_has_value(OptionalNumeric opt)
    return opt.has_value
end func

-- Get value (with error check)
func optional_numeric_value(OptionalNumeric opt)
    if opt.has_value then
        return opt.value
    else
        print "ERROR: Accessing value of empty optional!"
        return 0
    end
end func

-- Get value or default
func optional_numeric_value_or(OptionalNumeric opt, numeric default_val)
    if opt.has_value then
        return opt.value
    else
        return default_val
    end
end func
```

**Usage Example:**

```mlp
-- Create empty optional
OptionalNumeric maybe_num = optional_numeric_none()

if optional_numeric_has_value(maybe_num) then
    print "Has value"
else
    print "No value (correct!)"
end func

-- Get with default
numeric val = optional_numeric_value_or(maybe_num, 42)
print val  -- 42

-- Create optional with value
OptionalNumeric some_num = optional_numeric_some(100)

if optional_numeric_has_value(some_num) then
    numeric actual = optional_numeric_value(some_num)
    print actual  -- 100
end func
```

**Self-Hosting Principle:**

All Optional<T> runtime code is written in MLP itself (`mlp_lib/optional.mlp`), demonstrating:
- MLP can implement complex data structures
- Self-hosting compiler development (dogfooding)
- Library code serves as examples for users
- No Python or C code needed for new features

**Implementation Details:**

1. **Lexer:** Added `TOKEN_OPTIONAL`, `TOKEN_GENERIC`
2. **Parser:**
   - Generic type parameter parsing: `optional<numeric>`
   - AST nodes: `AST_OPTIONAL_TANIMLAMA`, etc.
   - Compound keywords: "end function", "end if", etc.
3. **Code Generator:** Visitor stubs for optional operations
4. **Runtime:** Pure MLP implementation in `mlp_lib/optional.mlp`

**Supported Generic Types:**

Currently implemented:
- `optional<numeric>` - Optional integer/decimal
- `optional<string>` - Optional text
- `optional<boolean>` - Optional boolean

**Future Generic Types:**

Planned for Phase 6.3+:
- `list<T>` - Dynamic arrays
- `map<K, V>` - Hash maps
- `result<T, E>` - Error handling
- Custom generic structs

---

### Phase 5.11: Default Parameters (November 22, 2025) ⭐

**Feature:** Functions can have default parameter values

**Syntax:**
```mlp
function add(a = 10, b = 20)
    print a + b
end function

add()      -- Output: 30 (uses both defaults)
add(5)     -- Output: 25 (uses 5 + 20)
add(5, 7)  -- Output: 12 (uses 5 + 7)
```

**Implementation:**

1. **Two-Pass Compilation:**
   - **Pass 1 (Parse):** Create AST with default values stored
   - **Pass 2 (Pre-scan):** Register all function signatures into registry
   - **Pass 3 (Generate):** Generate code using populated registry

2. **Function Registry:**
   - HashMap-based registry stores all function signatures
   - `FunctionSignature` struct: name + param_count + parameters[]
   - `ParameterInfo` struct: name + default_value AST node

3. **Code Generation:**
   - Function calls check registry for signature
   - Missing arguments filled with default values from AST
   - Default value expressions evaluated at call-site

**Test Results:**
```
Test 1: add() -> should be 30
30
Test 2: add(5) -> should be 25
25
Test 3: add(5, 7) -> should be 12
12
```

**Bug Fix:** Added implicit return (function epilog) for functions without explicit return statements to prevent segmentation faults.

---

### Critical Bug Fixes (November 22, 2025)

**Bug #1: "end function" Not Recognized**

**Problem:** Compound keyword "end function" was being parsed as two separate tokens ("end" and "function"), causing syntax errors.

**Root Cause:** The base "end" keyword was completely missing from KeywordMap. Compound keyword handler existed but couldn't run without the base token.

**Fix:** Added `{"end", TOKEN_END}` to KeywordMap at line 1012 in `mlp_compiler.c`

**Impact:**
- All compound keywords now work: "end function", "end if", "end while"
- Function definitions compile correctly
- Tests pass: `test_just_func.mlp`, `test_func_call.mlp`

**Bug #2: Type Keyword Confusion**

**Problem:** AI assistant attempted to add int/float/bool as type keywords, violating MLP design.

**Root Cause:** Misunderstanding of MLP type system.

**Clarification:**
- MLP ONLY uses: `numeric`, `string`, `boolean`
- NO int, float, char, or bool keywords exist
- ALL numbers are BigDecimal (numeric)
- ALL text is BigString (string)

**Fix:** Removed incorrect aliases, updated AI_RULES.md with CRITICAL warnings

---

### Phase 6.3: Line Continuation (Backslash) ✅

**Status:** 100% Complete (22 Kasım 2025)

MLP now supports Python-style multi-line code using backslash continuation.

**Syntax:**

```mlp
-- Long arithmetic expression
numeric x = 100 + \
            200 + \
            300

-- Function calls with many arguments
numeric sum = add_five(10, \
                       20, \
                       30, \
                       40, \
                       50)

-- Multi-line string concatenation
string message = "This is a very long " + \
                 "message that spans " + \
                 "multiple lines"

-- Complex conditions
if x > 50 and \
   y > 100 and \
   x + y == 300 then
    print "Condition met!"
end if
```

**Implementation:**
- Lexer detects backslash followed by newline
- Automatically skips whitespace after continuation
- Works with all statement types (expressions, function calls, conditions)

**Test Coverage:** `test/test_line_continuation_full.mlp`

---

### Phase 6.4: String Concatenation Operator & Type Inference ✅

**Status:** 100% Complete (22 Kasım 2025)

MLP now features compile-time type inference to support automatic string concatenation with the `+` operator.

**Type Inference System:**

```mlp
-- Automatic string concatenation
string hello = "Hello"
string world = " World"
string result = hello + world  -- Uses string_concat()

-- Numeric addition (unchanged)
numeric a = 10
numeric b = 20
numeric sum = a + b  -- Uses add instruction

-- String literals
string msg = "Hello" + " " + "World"  -- Automatically detected as string
```

**Implementation Details:**

1. **`infer_type()` Function:**
   - Analyzes AST nodes at compile-time
   - Returns "string", "numeric", or "unknown"
   - Checks symbol table for variable types

2. **Smart `+` Operator:**
   - If either operand is string → calls `string_concat()`
   - If both operands are numeric → uses `add rax, rbx`
   - Type decision made at compile-time (zero runtime overhead)

3. **Supported Scenarios:**
   - String literal + string literal
   - String variable + string variable
   - String + numeric (implicit conversion)
   - Multi-line string concatenation with backslash

**Assembly Generation:**

```asm
; String concatenation
mov rdi, rax  ; Sol operand -> arg1
mov rsi, rbx  ; Sağ operand -> arg2
call string_concat
; Sonuç RAX'te

; Numeric addition
add rax, rbx
```

**Test Coverage:** `test/test_string_literal_concat.mlp`, `test/test_line_continuation_full.mlp`

---

## Phase 7: Functional Programming

**Status:** Partial ✅ (Phase 7.6-7.7 Complete)
**Date Added:** November 23, 2025
**Compiler:** `self_host/mlp_compiler.c`

### 7.6 Array Parameters (Pass-by-Reference) ✅

**Purpose:** Enable functions to accept arrays as parameters with pointer semantics

**Syntax:**
```mlp
// Array parameter - brackets after parameter name
function sum_array(arr[], size)
    numeric total = 0
    numeric i = 0
    while i < size
        total = total + arr[i]
        i = i + 1
    end
    return total
end

// Usage
numeric numbers[] = [10, 20, 30, 40, 50]
numeric result = sum_array(numbers, 5)  // 150
```

**Features:**
- **Pass-by-reference:** Arrays passed as pointers (not copied)
- **Modification support:** Changes to `arr[i]` affect original array
- **Size convention:** Array size passed separately (no built-in length)
- **Type safety:** Compile-time detection of array parameters

**Implementation:**

1. **Parser Changes:**
   - Detects `[]` tokens after parameter name
   - Stores array metadata in `parametre_is_array` array
   - Syntax: `function name(arr[], size)`

2. **AST Structure:**
   ```c
   struct {
       Token* ad;
       Token** parametreler;
       int parametre_sayisi;
       int* parametre_is_array;  // NEW: Phase 7.6
       ASTNode** parametre_default_degerleri;
       ASTNode* govde;
   } islec_tanimlama_data;
   ```

3. **Code Generation:**
   - Array parameters use `ARRAY_SAYISAL` type (pointer semantics)
   - Element access: `mov rbx, [rbp-8]` (load pointer value)
   - Local arrays: `lea rbx, [rbp-40]` (compute address)
   - Critical distinction for correct memory access

4. **Scope Management:**
   - Functions enter scope with `kapsam_gir()`
   - Exit scope with `kapsam_cik()`
   - Prevents global variable naming conflicts

**Assembly Example:**
```asm
sum_array:
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi     ; Store array pointer (not address!)
    mov [rbp-16], rsi    ; Store size
    
    ; Access arr[i]:
    mov rax, [rbp-24]    ; Load i
    imul rax, 8          ; i * 8 (element size)
    mov rbx, [rbp-8]     ; Load pointer value (mov, not lea!)
    add rbx, rax         ; base + offset
    mov rax, [rbx]       ; Read element
```

**Test Results:**
```mlp
sum_array([10,20,30,40,50], 5)     → 150 ✅
double_array() modifies original    → 300 ✅  
find_max([20,40,60,80,100], 5)     → 100 ✅
```

**Documentation:** `docs/PHASE7_6_ARRAY_PARAMETERS.md`

### 7.7 Functional Programming Patterns ✅

**Purpose:** Map, filter, reduce operations on arrays using higher-order patterns

**Implementation:** Pure MLP library functions in `mlp_lib/functional.mlp`

#### Reduce Operations

Combine array elements into single value:

```mlp
// Sum all elements
function reduce_sum(arr[], size)
    numeric accumulator = 0
    numeric i = 0
    while i < size
        accumulator = accumulator + arr[i]
        i = i + 1
    end
    return accumulator
end

// Product of all elements
function reduce_product(arr[], size)
    numeric accumulator = 1
    numeric i = 0
    while i < size
        accumulator = accumulator * arr[i]
        i = i + 1
    end
    return accumulator
end

// Find maximum
function reduce_max(arr[], size)
    numeric max_val = arr[0]
    numeric i = 1
    while i < size
        if arr[i] > max_val then
            max_val = arr[i]
        end
        i = i + 1
    end
    return max_val
end

// Usage
numeric numbers[] = [1, 2, 3, 4, 5]
numeric sum = reduce_sum(numbers, 5)        // 15
numeric product = reduce_product(numbers, 5) // 120
numeric max = reduce_max(numbers, 5)        // 5
```

#### Map Operations (In-Place)

Transform each array element:

```mlp
// Double each element (modifies original)
function map_double_inplace(arr[], size)
    numeric i = 0
    while i < size
        arr[i] = arr[i] * 2
        i = i + 1
    end
end

// Usage
numeric data[] = [1, 2, 3, 4, 5]
map_double_inplace(data, 5)
// data is now [2, 4, 6, 8, 10]
```

#### Utility Operations

Predicate-based operations:

```mlp
// Count elements matching condition
function count_positive(arr[], size)
    numeric count = 0
    numeric i = 0
    while i < size
        if arr[i] > 0 then
            count = count + 1
        end
        i = i + 1
    end
    return count
end

// Check if any element matches
function any_positive(arr[], size)
    numeric i = 0
    while i < size
        if arr[i] > 0 then
            return 1  // true
        end
        i = i + 1
    end
    return 0  // false
end

// Check if all elements match
function all_positive(arr[], size)
    numeric i = 0
    while i < size
        if arr[i] <= 0 then
            return 0  // false
        end
        i = i + 1
    end
    return 1  // true
end
```

**Features:**
- **Pure MLP:** No runtime C dependencies
- **Composable:** Chain operations together
- **Efficient:** Direct array access, no overhead
- **Extensible:** Easy to add custom operations

**Test Results:**
```
sum([1,2,3,4,5])          → 15 ✅
product([1,2,3,4,5])      → 120 ✅
max([5,10,15,20,25])      → 25 ✅
min([5,10,15,20,25])      → 5 ✅
count_positive([-2,0,1,2]) → 2 ✅
```

**Library:** `mlp_lib/functional.mlp`
**Tests:** `test/phase7_7_simple.mlp`, `test/phase7_7_map.mlp`

---

## 7 (Phase 7.1-7.4) - Lambda Expressions & Higher-Order Functions ✅

**Status: Complete** (November 23, 2025)

### Overview

Lambda expressions provide anonymous function support with concise syntax. Combined with function pointers and indirect calls, lambdas enable functional programming patterns including map, filter, and reduce operations.

**Phases:**
- **7.1:** Lambda syntax parsing (`lambda(x) => x * 2`)
- **7.2-7.3:** Function pointers and indirect calls
- **7.4:** Higher-order functions (lambdas as parameters)

### 7.1 Lambda Expressions ✅

**Purpose:** Anonymous functions with concise syntax

**Syntax:**
```mlp
lambda(parameters) => expression
```

**Examples:**
```mlp
// Simple lambda
numeric double_fn = lambda(n) => n * 2

// Multiple parameters
numeric add_fn = lambda(x, y) => x + y

// No parameters
numeric get_answer = lambda() => 42
```

**Implementation Details:**

1. **Lexer Tokens:**
   - `TOKEN_LAMBDA` - Keyword `lambda`
   - `TOKEN_ARROW` - Operator `=>`
   - **Critical:** `=>` must be tokenized in multi-char operator section (before single `=`)

2. **Parser:**
   - Recognized in `birincil()` (primary expression)
   - Creates `AST_LAMBDA` node
   - Parses parameter list between `( )`
   - Parses expression body after `=>`

3. **AST Structure:**
   ```c
   struct {
       Token** parametreler;          // Parameter tokens
       int parametre_sayisi;          // Parameter count
       ASTNode* govde;                // Lambda body (expression)
       bool is_expression;            // Always true for lambda
   } lambda_data;
   ```

4. **Code Generation:**
   - Generates anonymous function with unique label `__lambda_N`
   - Returns function address: `lea rax, [rel __lambda_N]`
   - **Critical:** Jump instruction prevents inline execution:
     ```asm
     lea rax, [rel __lambda_0]
     jmp __lambda_0_skip     ; Skip lambda body
     
     __lambda_0:
         push rbp
         mov rbp, rsp
         ; ... lambda code ...
         pop rbp
         ret
     
     __lambda_0_skip:
         ; Continue main execution
     ```

**Assembly Example:**
```asm
; numeric double_fn = lambda(n) => n * 2

lea rax, [rel __lambda_0]      ; Load function address
mov [rbp-8], rax               ; Store in double_fn
jmp __lambda_0_skip            ; Skip lambda body

__lambda_0:                    ; Anonymous function
    push rbp
    mov rbp, rsp
    sub rsp, 256
    mov [rbp-8], rdi           ; Parameter n
    mov rax, [rbp-8]           ; n
    imul rax, 2                ; n * 2
    mov rsp, rbp
    pop rbp
    ret

__lambda_0_skip:               ; Resume main flow
    ; ... rest of code ...
```

**Test Results:**
```mlp
print lambda(x, y) => x + y    ✅ Compiles successfully
print lambda(n) => n * 2       ✅ Compiles successfully
print lambda() => 42           ✅ Compiles successfully
```

**Test File:** `test/phase7_1_lambda.mlp`

### 7.2-7.3 Lambda Invocation & Function Pointers ✅

**Purpose:** Store and call lambda functions via function pointers

**Syntax:**
```mlp
numeric fn = lambda(params) => expression
numeric result = fn(arguments)
```

**Implementation Details:**

1. **Function Pointer Storage:**
   - Lambdas stored as 64-bit addresses in `numeric` variables
   - `lea rax, [rel __lambda_N]` loads function address
   - Stored value is executable function pointer

2. **Indirect Function Calls:**
   - Modified `visit_IslecCagirma()` checks if name is variable
   - If variable: indirect call mechanism
   - If function: direct call (existing behavior)

3. **Indirect Call Mechanism:**
   ```c
   Degisken* var = kapsam_degisken_bul(islec_adi);
   if (var != NULL) {
       // Load function pointer
       sprintf(buffer, "mov r10, [rbp-%d]", var->rbp_offset);
       
       // Prepare arguments in registers (rdi, rsi, rdx, rcx, r8, r9)
       for (int i = 0; i < islec->islec_cagirma_data.arguman_sayisi; i++) {
           visit(islec->islec_cagirma_data.argumanlar[i]);
           const char* arg_regs[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
           sprintf(buffer, "mov %s, rax", arg_regs[i]);
       }
       
       // Indirect call
       sprintf(buffer, "call r10");
   }
   ```
   - **Critical:** Use r10 (not rax) to preserve function pointer during arg setup

4. **System V AMD64 ABI:**
   - First 6 arguments in registers: rdi, rsi, rdx, rcx, r8, r9
   - Return value in rax
   - Stack alignment requirements preserved

**Assembly Example:**
```asm
; numeric double_fn = lambda(n) => n * 2
; numeric result = double_fn(5)

; Load function pointer
mov r10, [rbp-8]           ; double_fn variable

; Prepare argument
mov rax, 5
mov rdi, rax               ; First argument in rdi

; Indirect call
call r10                   ; Call via function pointer

; Store result
mov [rbp-16], rax          ; result = rax
```

**Test Results:**
```mlp
numeric double_fn = lambda(n) => n * 2
numeric result = double_fn(5)
print result                       → 10 ✅

numeric add_fn = lambda(x, y) => x + y
numeric result2 = add_fn(3, 7)
print result2                      → 10 ✅
```

**Test Files:** `test/phase7_2_simple.mlp`, `test/phase7_2_lambda_call.mlp`

### 7.4 Higher-Order Functions ✅

**Purpose:** Pass lambdas as function parameters for functional programming patterns

**Syntax:**
```mlp
function apply(fn, value)
    return fn(value)
end

numeric result = apply(lambda(n) => n * 2, 5)
```

**Implementation Details:**

1. **Function Parameters:**
   - Functions can accept `numeric` parameters (function pointers)
   - No special syntax needed - existing implementation handles it

2. **Inline Lambda Syntax:**
   - Lambda expressions can be passed directly as arguments
   - Parser evaluates lambda → function address → passed as numeric

3. **Indirect Calls:**
   - Inside called function, parameter is treated as function pointer
   - Existing indirect call mechanism handles invocation

**Examples:**

**Basic Higher-Order Function:**
```mlp
function apply(fn, value)
    return fn(value)
end

numeric result = apply(lambda(n) => n * 2, 5)
print result                                    // 10 ✅
```

**Multiple Parameters:**
```mlp
function apply2(fn, a, b)
    return fn(a, b)
end

numeric result = apply2(lambda(x, y) => x + y, 3, 7)
print result                                    // 10 ✅
```

**Stored Lambda:**
```mlp
numeric square_fn = lambda(x) => x * x
numeric result = apply(square_fn, 4)
print result                                    // 16 ✅
```

**Functional Programming Patterns:**

**Map with Lambda:**
```mlp
function map_transform(arr[], size, transform_fn)
    numeric i = 0
    while i < size
        arr[i] = transform_fn(arr[i])
        i = i + 1
    end
end

numeric numbers[] = [1, 2, 3, 4, 5]
map_transform(numbers, 5, lambda(x) => x * 2)
print numbers[0]  // 2  ✅
print numbers[2]  // 6  ✅
print numbers[4]  // 10 ✅
```

**Filter with Lambda:**
```mlp
function count_matching(arr[], size, predicate_fn)
    numeric count = 0
    numeric i = 0
    while i < size
        if predicate_fn(arr[i]) == 1 then
            count = count + 1
        end
        i = i + 1
    end
    return count
end

numeric numbers[] = [1, 2, 3, 4, 5]
numeric even_count = count_matching(numbers, 5, lambda(x) => x % 2 == 0)
print even_count  // 2 ✅
```

**Reduce with Lambda:**
```mlp
function reduce(arr[], size, reducer_fn, initial)
    numeric accumulator = initial
    numeric i = 0
    while i < size
        accumulator = reducer_fn(accumulator, arr[i])
        i = i + 1
    end
    return accumulator
end

numeric numbers[] = [1, 2, 3, 4, 5]
numeric sum = reduce(numbers, 5, lambda(acc, x) => acc + x, 0)
print sum  // 15 ✅
```

**Test Results:**
```mlp
apply(lambda(n) => n*2, 5)                     → 10  ✅
apply2(lambda(a,b) => a+b, 3, 7)               → 10  ✅
apply(square_fn, 4)                            → 16  ✅
map_transform(arr, 5, lambda(x) => x*2)        → [2,4,6,8,10] ✅
count_matching(arr, 5, lambda(x) => x%2==0)    → 2   ✅
reduce(arr, 5, lambda(acc,x) => acc+x, 0)      → 15  ✅
```

**Test Files:** `test/phase7_4_higher_order.mlp`, `test/phase7_4_functional.mlp`

**Summary:**
- ✅ 11/11 tests passing
- ✅ Lambda syntax parsing working
- ✅ Function pointers and indirect calls working
- ✅ Higher-order functions working
- ✅ All functional programming patterns (map/filter/reduce) with lambdas working

---

### 7.8-7.9 Upcoming Features ⏳

**Phase 7.8: Lambda Return Values**
- Functions returning lambdas
- First-class function support
- Function factory patterns

**Phase 7.9: Closures**
- Variable capture from outer scope
- Closure struct generation
- Memory management for captured variables

---

---

## 🎨 GUI Designer Features

### MLP Functions Documentation

**Status:** ✅ Complete (November 23, 2025)

Comprehensive function reference documentation for help menu and IntelliSense:

**File:** `mlp_gui_designer/mlp_functions.json`

**Categories (7 total, 38+ functions):**

1. **String Operations** (8 functions)
   - `string_length`, `string_concat`, `string_replace`, `string_split`
   - `string_substring`, `string_upper`, `string_lower`, `string_trim`

2. **GUI Functions** (7 functions)
   - `GUI_PENCERE_AC`, `GUI_BUTTON_CREATE`, `GUI_LABEL_CREATE`
   - `GUI_POLL_EVENT`, `GUI_GET_CLICKED_BUTTON`, `GUI_GUNCELLE`, `GUI_KAPAT`

3. **I/O Operations** (2 functions)
   - `YAZDIR`, `read_input`

4. **File Operations** (4 functions)
   - `open_file`, `read_file`, `write_file`, `close_file`

5. **Array/List Operations** (5 functions)
   - Array literals, `list.add`, `list.get`, `list.size`, `list.remove`

6. **Type Conversions** (2 functions)
   - `str` (numeric to string), `num` (string to numeric)

7. **Functional Programming** (10 functions) - **NEW Phase 7.6-7.7**
   - **REDUCE:** `sum_array`, `product_array`, `max_array`, `min_array`
   - **MAP:** `map_double_inplace`, `map_square_inplace`, `map_increment_inplace`
   - **UTILITY:** `count_positive`, `any_positive`, `all_positive`

**Function Documentation Format:**

```json
{
  "name": "sum_array",
  "signature": "sum_array(arr[], size: numeric) -> numeric",
  "description": "Array elemanlarının toplamını hesaplar (REDUCE operation)",
  "params": [
    { "name": "arr[]", "type": "numeric[]", "description": "Sayı array'i (pass-by-reference)" },
    { "name": "size", "type": "numeric", "description": "Array boyutu" }
  ],
  "returns": { "type": "numeric", "description": "Toplam değer" },
  "example": "numeric numbers[] = [1, 2, 3, 4, 5]\nnumeric total = sum_array(numbers, 5)\nYAZDIR(total)  -- 15",
  "category": "reduce"
}
```

**Constants (3 GUI events):**

- `GUI_EVENT_NONE` = 0
- `GUI_EVENT_QUIT` = 1
- `GUI_EVENT_BUTTON_CLICK` = 2

### Multi-Language & Multi-Syntax Code Generation

**Status:** ✅ Complete (November 23, 2025)

GUI Designer can generate code in multiple languages and syntax styles:

**Supported Languages:**

- **Turkish (tr-TR):** Native Turkish keywords
- **English (en-US):** English keywords

**Supported Syntax Styles:**

1. **MLP-default:** Native block keywords

   ```mlp
   if condition
       // code
   end if

   while condition
       // code
   end while
   ```

2. **C-style:** Curly braces

   ```mlp
   if condition {
       // code
   }

   while condition {
       // code
   }
   ```

3. **Python-style:** Colon and indentation

   ```mlp
   if condition:
       // code

   while condition:
       // code
   ```

**Code Generator Architecture:**

- `getBlockMarkers()` - Returns syntax-appropriate markers (braces, keywords, colons)
- `translateKeyword()` - Translates keywords based on selected language
- Event handlers and event loops adapt to selected syntax
- Integrated with `configLoader` for runtime configuration

**Example Output (C-style):**

```mlp
-- lang: en-US

func onClick_handler() -> numeric {
    YAZDIR("Button clicked")
    return 0
}

while running == 1 {
    numeric event
    event = GUI_POLL_EVENT()

    if event == GUI_EVENT_QUIT {
        running = 0
    }

    GUI_GUNCELLE()
}
```

---

## Phase 8: Async/Await & Concurrency

**Status:** Phase 8.4 Complete ✅ | Phase 8.5 In Progress 🔄  
**Version:** MLP 3.0 + Phase 8.4  
**Released:** November 23, 2025

### Overview

Phase 8 introduces asynchronous programming capabilities to MLP, enabling non-blocking I/O operations and concurrent execution patterns similar to JavaScript's async/await and Python's asyncio.

### Architecture

**Promise-Based Model:**
- Single-threaded event loop (Node.js style)
- Promise/Future pattern for async operations
- State machine transformation for await points
- Continuation-passing style for resume

**Implementation Phases:**
1. ✅ Phase 8.1: Design (PHASE8_ASYNC_DESIGN.md)
2. ✅ Phase 8.2: Promise runtime (7 functions)
3. ✅ Phase 8.3: Event Loop (8 functions)
4. ✅ Phase 8.4: Basic async/await syntax (blocking)
5. 🔄 Phase 8.5: Non-blocking state machines

### Syntax

#### Async Function Declaration

```mlp
async function fetch_data(id)
    // Async function body
    numeric result = await get_from_api(id)
    return result
end
```

**Key Features:**
- `async function` keyword declares async function
- Returns a Promise implicitly
- Can contain `await` expressions
- Supports all normal function features (params, return, local vars)

#### Await Expression

```mlp
numeric value = await async_operation()
```

**Behavior:**
- **Phase 8.4 (Current):** Blocking wait loop - spins until promise resolves
- **Phase 8.5 (Planned):** Non-blocking - returns control to event loop

**Restrictions:**
- Only usable inside `async function`
- Compiler error if used in sync function

### Examples

#### Basic Async Function (Phase 8.4)

```mlp
// Simple async function
async function get_number()
    return 42
end

function main()
    print("Testing async function...")
    numeric result = await get_number()
    print("Result:")
    print(result)  // Prints: 42
    return 0
end
```

**Output:**
```
Testing async function...
Result:
42
```

### Implementation Status

**✅ Completed (Phase 8.1-8.4):**
- Promise struct and state management
- Event loop with task queue
- `async function` keyword and parsing
- `await` expression parsing
- Promise creation/resolution in async functions
- Blocking await implementation
- Return statement async context handling

**🔄 In Progress (Phase 8.5):**
- AsyncState struct for continuation passing
- Await point detection (count_awaits_in_node)
- State machine generation
- Non-blocking await with event loop integration

**Testing:**
- `test_async_basic.mlp` - ✅ Passing
- `test_async_nonblocking.mlp` - 🔄 Prepared

---

**Last Updated:** November 23, 2025  
**Next Milestone:** Non-blocking state machine generation (Phase 8.5)

---

**© 2025 MLP Project**
**Version:** 3.0 + Phase 7.6-7.7
**Status:** Production Ready ✅ + Array Parameters & Functional Programming 🚀
**License:** MIT
