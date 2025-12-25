# MLP-LLVM Modül ve Kütüphane Referansı

**Proje:** MLP-LLVM (LLVM Backend ile MELP Compiler)  
**Konum:** /home/pardus/projeler/MLP/MLP-LLVM/  
**Son Güncelleme:** 24 Aralık 2025  
**Durum:** Aktif Geliştirme - Self-Hosting Stage 1

---

## 📚 Genel Bakış

MLP-LLVM projesi, MELP (Modern Extensible Language Platform) dilinin LLVM backend kullanarak geliştirilmesidir. Bu proje, mlp-original'den çok daha gelişmiş bir mimari sunar ve self-hosting'e odaklanır.

### Temel Özellikler

- **LLVM Backend** - Modern optimizasyon ve code generation
- **Self-Hosting** - MELP ile yazılmış compiler
- **Modüler Mimari** - Ayrı lexer, parser, codegen modülleri
- **STO (Small Type Optimization)** - Performans optimizasyonu
- **Kapsamlı Runtime** - Gelişmiş stdlib desteği

---

## 📁 Proje Yapısı

```
MLP-LLVM/
├── modules/              # MELP ile yazılmış compiler modülleri
│   ├── lexer_mlp/        # Lexer modülleri
│   ├── parser_mlp/       # Parser modülleri
│   ├── codegen_mlp/      # Code generation modülleri
│   └── test/             # Test dosyaları
│
├── compiler/             # Build stages
│   ├── stage0/           # Bootstrap derleyici (Python/C++)
│   ├── stage1/           # MELP ile yazılmış compiler
│   └── stage2/           # Optimize edilmiş compiler
│
├── runtime/              # Runtime kütüphaneleri
│   ├── stdlib/           # Standard library (C)
│   ├── sto/              # Small Type Optimization
│   ├── memory/           # Memory management
│   └── wrapper/          # Entry point wrapper
│
├── examples/             # Örnek kodlar
│   ├── basics/           # Temel örnekler
│   ├── hello/            # Hello World varyasyonları
│   └── llvm/             # LLVM özellikleri
│
├── docs/                 # İngilizce dokümantasyon
├── docs_tr/              # Türkçe dokümantasyon
├── tests/                # Test suite
├── scripts/              # Build/test scriptleri
└── templates/            # Code şablonları
```

**Toplam Kod:** ~12,500 satır MELP kodu (sadece modules/)

---

## �� Lexer Modülleri (modules/lexer_mlp/)

### Ana Modül: lexer.mlp

**Dosya:** `modules/lexer_mlp/lexer.mlp`  
**Durum:** Phase 13 Part 6.6 - COMPLETE  
**Satır Sayısı:** ~300+ satır

Tüm tokenization modüllerini koordine eden ana lexer.

#### Fonksiyonlar

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `create_token(type, value, line, col)` | Token oluştur | `list` |
| `create_position_result(p, c, l)` | Pozisyon bilgisi | `list` |
| `is_digit(ch)` | Rakam kontrolü | `boolean` |
| `is_alpha(ch)` | Harf kontrolü | `boolean` |
| `is_whitespace(ch)` | Boşluk kontrolü | `boolean` |
| `is_operator_start(ch)` | Operatör başlangıcı | `boolean` |

### Yardımcı Modüller

#### 1. char_utils.mlp
Karakter sınıflandırma ve yardımcı fonksiyonlar.

#### 2. tokenize_literals.mlp
Number ve string literal'leri tokenize eder.

| Fonksiyon | Açıklama |
|-----------|----------|
| `tokenize_number()` | Sayı literal'i parse et |
| `tokenize_string()` | String literal'i parse et |
| `tokenize_char()` | Karakter literal'i parse et |

#### 3. tokenize_identifiers.mlp
Identifier ve keyword tokenize.

| Fonksiyon | Açıklama |
|-----------|----------|
| `tokenize_identifier()` | Identifier/keyword parse |
| `is_keyword()` | Keyword kontrolü |
| `keyword_to_token_type()` | Keyword → Token type |

#### 4. tokenize_operators.mlp
Operatör, sembol ve comment tokenize.

| Fonksiyon | Açıklama |
|-----------|----------|
| `tokenize_operator()` | Operatör parse et |
| `skip_line_comment()` | Satır yorumu atla |
| `skip_block_comment()` | Block yorumu atla |

#### 5. token.mlp
Token veri yapısı ve yardımcılar.

```mlp
-- Token: [type, value, line, column]
list token = [T_ID, "x", 1, 5]
```

### Token Tipleri

**Keywords (1-31):**
- `T_FUNCTION = 1`, `T_END = 2`, `T_IF = 3`, `T_THEN = 4`
- `T_ELSE = 5`, `T_WHILE = 6`, `T_DO = 7`, `T_FOR = 17`
- `T_STRUCT = 20`, `T_ENUM = 21`, `T_SWITCH = 22`
- `T_IMPORT = 25`, `T_EXPORT = 26`

**Operators (37-49):**
- `T_PLUS = 37`, `T_MINUS = 38`, `T_MULTIPLY = 39`
- `T_EQ = 42`, `T_NE = 43`, `T_LT = 44`, `T_GT = 45`
- `T_AND = 48`, `T_OR = 49`, `T_NOT = 31`

**Delimiters (50-54):**
- `T_LPAREN = 50`, `T_RPAREN = 51`, `T_COMMA = 52`
- `T_LBRACKET = 53`, `T_RBRACKET = 54`

**Literals:**
- `T_ID = 32`, `T_NUMBER = 61`, `T_STR_LIT = 62`
- `T_EOF = 0`

---

## 🌲 Parser Modülleri (modules/parser_mlp/)

### Ana Modül: parser.mlp

**Dosya:** `modules/parser_mlp/parser.mlp`  
**Durum:** Stage 1 Self-Hosting  
**Satır Sayısı:** ~300+ satır

Tüm parser modüllerini entegre eden ana parser.

#### Global State

```mlp
list g_tokens           -- Token listesi
numeric g_current_pos   -- Mevcut pozisyon
numeric g_error_count   -- Hata sayısı
```

#### Temel Fonksiyonlar

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `init_parser(token_list)` | Parser'ı başlat | `numeric` |
| `current_token()` | Mevcut token'ı al | `list` |
| `peek_token()` | Sonraki token'a bak | `list` |
| `advance()` | Token ilerlet | `void` |
| `expect(token_type)` | Token türü bekle | `boolean` |
| `match(token_type)` | Token eşleşmesi | `boolean` |

### Parser Alt Modülleri

#### 1. parser_expr.mlp (~300 satır)
Expression parsing (precedence climbing).

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_expression()` | Ana expression parser |
| `parse_primary()` | Primary expression |
| `parse_binary_op()` | Binary operator |
| `parse_unary()` | Unary expression |
| `parse_call()` | Function call |
| `parse_index()` | Array indexing |

#### 2. parser_stmt.mlp (~98 satır)
Statement parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_statement()` | Statement parse |
| `parse_var_decl()` | Variable declaration |
| `parse_assignment()` | Assignment |
| `parse_return()` | Return statement |
| `parse_print()` | Print statement |

#### 3. parser_control.mlp (~200 satır)
Control flow parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_if()` | If statement |
| `parse_while()` | While loop |
| `parse_for()` | For loop |
| `parse_break()` | Break statement |
| `parse_continue()` | Continue statement |

#### 4. parser_func.mlp (~250 satır)
Function definition parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_function()` | Function definition |
| `parse_parameters()` | Parameter list |
| `parse_return_type()` | Return type |
| `parse_function_body()` | Function body |

#### 5. parser_struct.mlp (~245 satır)
Struct definition parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_struct()` | Struct definition |
| `parse_struct_member()` | Struct field |
| `parse_struct_init()` | Struct initialization |

#### 6. parser_enum.mlp (~320 satır)
Enum definition parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_enum()` | Enum definition |
| `parse_enum_variant()` | Enum variant |
| `parse_enum_value()` | Enum value |

#### 7. parser_switch.mlp (~332 satır)
Switch/case parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_switch()` | Switch statement |
| `parse_case()` | Case clause |
| `parse_default()` | Default clause |

#### 8. parser_import.mlp (~180 satır)
Import/module system parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_import()` | Import statement |
| `parse_export()` | Export statement |
| `resolve_import()` | Import çözümleme |

#### 9. parser_call.mlp (~220 satır)
Function call parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_call_expr()` | Function call |
| `parse_arguments()` | Argument list |
| `parse_named_args()` | Named arguments |

#### 10. parser_index.mlp (~150 satır)
Array/list indexing parsing.

| Fonksiyon | Açıklama |
|-----------|----------|
| `parse_index_expr()` | Index expression |
| `parse_slice()` | Slice operation |

#### 11. parser_compound.mlp (~200 satır)
Compound expressions (+=, -=, etc.).

#### 12. ast_nodes.mlp (~280 satır)
AST node veri yapıları.

**Node Tipleri:**
- `AST_PROGRAM`, `AST_FUNCTION`, `AST_STRUCT`
- `AST_IF`, `AST_WHILE`, `AST_FOR`
- `AST_VAR_DECL`, `AST_ASSIGNMENT`
- `AST_BINARY_OP`, `AST_CALL`, `AST_INDEX`

#### 13. token_stream.mlp (~219 satır)
Token stream yönetimi.

#### 14. parser_errors.mlp (~150 satır)
Error handling ve reporting.

| Fonksiyon | Açıklama |
|-----------|----------|
| `report_error()` | Hata rapor et |
| `recover_from_error()` | Hatadan kurtar |
| `error_context()` | Hata context'i |

#### 15. parser_pretty.mlp (~336 satır)
AST pretty printing (debug).

| Fonksiyon | Açıklama |
|-----------|----------|
| `print_ast()` | AST yazdır |
| `print_node()` | Node yazdır |
| `indent()` | Indentation |

---

## 🏗️ Code Generation Modülleri (modules/codegen_mlp/)

### Ana Modül: ir_builder.mlp

**Dosya:** `modules/codegen_mlp/ir_builder.mlp`  
**Durum:** Phase 2 Part 1 - Complete  
**Satır Sayısı:** ~200 satır

LLVM IR code generation infrastructure.

#### Temel Fonksiyonlar

| Fonksiyon | Açıklama |
|-----------|----------|
| `emit_module_header()` | LLVM module header |
| `emit_function_start(name, ret_type)` | Function başlat |
| `emit_function_end()` | Function bitir |
| `emit_label(label)` | Basic block label |
| `emit_instr(instruction)` | LLVM instruction |
| `emit_alloca_instr(var, type)` | Alloca instruction |
| `emit_store_instr(val, ptr)` | Store instruction |
| `emit_load_instr(reg, ptr)` | Load instruction |
| `emit_ret_instr(value)` | Return instruction |

### CodeGen Alt Modülleri

#### 1. codegen_literal.mlp
Literal code generation.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_number()` | Sayı literal |
| `codegen_string()` | String literal |
| `codegen_boolean()` | Boolean literal |

#### 2. codegen_variable.mlp
Variable operations.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_var_decl()` | Variable declaration |
| `codegen_var_load()` | Variable load |
| `codegen_var_store()` | Variable store |

#### 3. codegen_arithmetic.mlp
Arithmetic operations.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_add()` | Addition |
| `codegen_sub()` | Subtraction |
| `codegen_mul()` | Multiplication |
| `codegen_div()` | Division |
| `codegen_mod()` | Modulo |

#### 4. codegen_comparison.mlp
Comparison operations.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_eq()` | Equal |
| `codegen_ne()` | Not equal |
| `codegen_lt()` | Less than |
| `codegen_gt()` | Greater than |
| `codegen_le()` | Less or equal |
| `codegen_ge()` | Greater or equal |

#### 5. codegen_logical.mlp
Logical operations.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_and()` | Logical AND |
| `codegen_or()` | Logical OR |
| `codegen_not()` | Logical NOT |

#### 6. codegen_control.mlp
Control flow.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_if()` | If statement |
| `codegen_branch()` | Conditional branch |
| `codegen_jump()` | Unconditional jump |

#### 7. codegen_while.mlp
While loop generation.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_while()` | While loop |
| `codegen_break()` | Break |
| `codegen_continue()` | Continue |

#### 8. codegen_for.mlp
For loop generation.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_for()` | For loop |
| `codegen_range()` | Range expression |

#### 9. codegen_functions.mlp
Function definition generation.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_function()` | Function definition |
| `codegen_call()` | Function call |
| `codegen_return()` | Return statement |

#### 10. codegen_arrays.mlp
Array operations.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_array_init()` | Array initialization |
| `codegen_array_access()` | Array indexing |
| `codegen_array_store()` | Array store |

#### 11. codegen_stmt.mlp
General statement generation.

| Fonksiyon | Açıklama |
|-----------|----------|
| `codegen_statement()` | Statement router |
| `codegen_block()` | Block statement |

#### 12. symbol_table.mlp
Symbol table management.

| Fonksiyon | Açıklama |
|-----------|----------|
| `symbol_table_init()` | Initialize |
| `symbol_add()` | Symbol ekle |
| `symbol_lookup()` | Symbol bul |
| `symbol_scope_push()` | Scope başlat |
| `symbol_scope_pop()` | Scope bitir |

#### 13. type_mapper.mlp
MELP → LLVM type mapping.

| Fonksiyon | Açıklama |
|-----------|----------|
| `map_type()` | Type mapping |
| `is_numeric_type()` | Numeric mi? |
| `is_string_type()` | String mi? |

---

## 🔧 Runtime Kütüphaneleri (runtime/)

### stdlib/ - Standard Library (C Implementation)

#### mlp_io.c / mlp_io.h
Input/Output operations.

| Fonksiyon | Açıklama | Parametreler |
|-----------|----------|--------------|
| `mlp_println_numeric(value, sto_type)` | Sayı yazdır (ln) | `void*`, `uint8_t` |
| `mlp_print_numeric(value, sto_type)` | Sayı yazdır | `void*`, `uint8_t` |
| `mlp_println_string(str)` | String yazdır (ln) | `const char*` |
| `mlp_print_string(str)` | String yazdır | `const char*` |
| `mlp_println_bool(value)` | Boolean yazdır (ln) | `int` |
| `mlp_print_bool(value)` | Boolean yazdır | `int` |

#### mlp_string.c / mlp_string.h
String operations.

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `mlp_string_compare(s1, s2)` | String karşılaştır | `int` |
| `mlp_string_equals(s1, s2)` | Eşit mi? | `int` |
| `mlp_string_not_equals(s1, s2)` | Eşit değil mi? | `int` |
| `mlp_string_length(str)` | String uzunluğu | `size_t` |
| `mlp_string_is_empty(str)` | Boş mu? | `int` |
| `mlp_string_indexOf(str, substr)` | Alt string ara | `int` |
| `mlp_string_free(str)` | String serbest bırak | `void` |
| `mlp_string_split_free(parts, count)` | Split result free | `void` |

#### mlp_list.c / mlp_list.h
Dynamic list (array) operations.

```c
typedef struct {
    void** elements;
    size_t length;
    size_t capacity;
} MelpList;
```

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `melp_list_free(list)` | Liste serbest bırak | `void` |
| `melp_list_length(list)` | Liste uzunluğu | `size_t` |
| `melp_list_is_empty(list)` | Boş mu? | `int` |
| `melp_list_set(list, index, elem)` | Eleman değiştir | `int` |
| `melp_list_append(list, elem)` | Sona ekle | `int` |
| `melp_list_prepend(list, elem)` | Başa ekle | `int` |
| `melp_list_remove(list, index)` | Eleman sil | `int` |
| `melp_list_clear(list)` | Liste temizle | `void` |
| `melp_list_reverse(list)` | Listeyi ters çevir | `void` |
| `melp_list_capacity(list)` | Kapasite | `size_t` |
| `melp_list_reserve(list, cap)` | Kapasite ayır | `int` |
| `melp_list_print_debug(list)` | Debug print | `void` |

#### mlp_map.c / mlp_map.h
HashMap/dictionary implementation.

```c
typedef struct {
    char* key;
    void* value;
    struct MelpMapEntry* next;
} MelpMapEntry;

typedef struct {
    MelpMapEntry** buckets;
    size_t capacity;
    size_t length;
} MelpMap;
```

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `melp_map_free(map)` | Map serbest bırak | `void` |
| `melp_map_insert(map, key, value)` | Anahtar-değer ekle | `int` |
| `melp_map_remove(map, key)` | Eleman sil | `int` |
| `melp_map_has_key(map, key)` | Anahtar var mı? | `int` |
| `melp_map_length(map)` | Map boyutu | `size_t` |
| `melp_map_resize(map, new_cap)` | Kapasiteyi değiştir | `int` |

#### mlp_optional.c / mlp_optional.h
Optional<T> type implementation.

```c
typedef struct {
    void* value;
    int has_value;
    uint8_t value_type;
} MelpOptional;
```

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `melp_optional_get_numeric(opt)` | Numeric değer al | `double` |
| `melp_optional_get_numeric_or(opt, def)` | Değer veya default | `double` |
| `melp_optional_coalesce_numeric(opt, def)` | Coalesce | `double` |
| `melp_optional_free(opt)` | Optional serbest bırak | `void` |
| `melp_optional_free_deep(opt)` | Deep free | `void` |

#### mlp_math.c / mlp_math.h
Math operations (wrapper around C math.h).

#### mlp_panic.c / mlp_panic.h
Error handling ve panic.

| Fonksiyon | Açıklama |
|-----------|----------|
| `mlp_panic_array_bounds(index, len, name)` | Array bounds hatası |
| `mlp_runtime_error(message)` | Runtime error |
| `mlp_panic_division_by_zero()` | Division by zero |

#### mlp_state.c / mlp_state.h
Global state management.

| Fonksiyon | Açıklama |
|-----------|----------|
| `mlp_state_auto_cleanup()` | Auto cleanup |

#### file_io.c / file_io.h
File operations.

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `mlp_file_exists(path)` | Dosya var mı? | `int` |
| `mlp_file_size(path)` | Dosya boyutu | `long` |

### sto/ - Small Type Optimization

**Performans optimizasyonu için küçük değerlerin heap yerine stack'te tutulması.**

#### Modüller:

- **runtime_sto.c / runtime_sto.h** - Ana STO runtime
- **sto_runtime.c / sto_runtime.h** - STO API
- **sto_types.h** - STO tip tanımları
- **sso_string.c** - Small String Optimization
- **bigdecimal.c** - Büyük sayı desteği

### memory/
Memory management sistemi (geliştirilmekte).

### wrapper/
Entry point wrapper (melp_entry.c).

---

## 📦 Bootstrap ve Compiler Modülleri

### Bootstrap Dosyaları

#### 1. bootstrap_minimal.mlp (34 satır)
Minimal bootstrap test.

#### 2. bootstrap_driver.mlp (96 satır)
Bootstrap sürücü.

#### 3. bootstrap_test.mlp (13 satır)
Bootstrap test runner.

### Compiler Integration

#### 1. compiler.mlp (275 satır)
Ana compiler integration.

#### 2. compiler_full.mlp (317 satır)
Full featured compiler.

#### 3. compiler_integration.mlp (273 satır)
Module integration.

#### 4. compiler_working.mlp (32 satır)
Working minimal compiler.

#### 5. compiler_gen1_v2.mlp (30 satır)
Generation 1 v2.

#### 6. compiler_gen1_v3.mlp (47 satır)
Generation 1 v3.

---

## 🧪 Test Modülleri

### Lexer Tests (modules/lexer_mlp/)

- `test_scan_simple.mlp` - Basit scanning
- `test_minimal_token.mlp` - Token testi
- `test_nested_4.mlp` - Nested yapılar
- `test_char_digit.mlp` - Karakter/rakam
- `test_while_no_do.mlp` - While syntax
- `test_escape_quote.mlp` - Escape karakterler
- `test_scan_func_call.mlp` - Function call
- `test_scan_if.mlp` - If statement
- `test_token.mlp` - Token yapısı
- `test_4params.mlp` - Parameter parsing
- `test_arith_compare.mlp` - Arithmetic/comparison
- `test_scan_nested.mlp` - Nested scanning
- `test_token_simple.mlp` - Simple token

### Parser Tests (modules/parser_mlp/)

- `test_basic.mlp` - Temel parser testi

### Test Modülleri (modules/test/)

- `math_utils.mlp` (18 satır) - Math utilities
- `main_multi.mlp` (28 satır) - Multi-module test
- `combined_test.mlp` (35 satır) - Combined test

---

## 📚 Dokümantasyon

### Ana Dokümantasyon

- **ARCHITECTURE.md** (54KB) - Mimari tasarım
- **README.md** (27KB) - Proje README
- **MELP_REFERENCE.md** (24KB) - Dil referansı
- **MELP_VISION.md** (4.7KB) - Proje vizyonu
- **kurallar_kitabı_v1.md** (55KB) - Syntax kuralları
- **pmlp_kesin_sozdizimi.md** (25KB) - Kesin sözdizimi

### İngilizce Dokümantasyon (docs/)

- `docs/api/` - API referansı
- `docs/language/` - Dil özellikleri
- `docs/tutorials/` - Tutorial'lar

### Türkçe Dokümantasyon (docs_tr/)

- `docs_tr/api/` - API referansı (TR)
- `docs_tr/language/` - Dil özellikleri (TR)
- `docs_tr/tutorials/` - Tutorial'lar (TR)

---

## �� Build System

### compiler/ Dizini

#### stage0/ - Bootstrap Compiler
Python/C++ ile yazılmış bootstrap derleyici.

#### stage1/ - Self-Hosted Compiler
MELP ile yazılmış compiler (ilk self-hosting stage).

#### stage2/ - Optimized Compiler
Optimize edilmiş, kendini derleyen compiler.

### Build Scripts (scripts/)

Build ve test scriptleri.

---

## 🎯 Özellik Karşılaştırması: mlp-original vs MLP-LLVM

| Özellik | mlp-original | MLP-LLVM |
|---------|--------------|----------|
| **Backend** | x86-64 Assembly | LLVM IR |
| **Compiler Dili** | C + Python | MELP (self-hosted) |
| **Modül Sayısı** | ~10 dosya | ~70+ dosya |
| **Kod Satırı** | ~3,000 MLP | ~12,500 MLP |
| **Runtime** | runtime.c (3,246 satır) | Modüler stdlib (çoklu dosya) |
| **Optimizasyon** | Minimal | STO, LLVM opt |
| **Tip Sistemi** | Basit | Gelişmiş (STO) |
| **Self-Hosting** | Kısmi | Tam (Stage 1+) |
| **Parser** | Tek dosya | 15 modül |
| **Lexer** | Tek dosya | 5+ modül |
| **Codegen** | Tek dosya | 13 modül |
| **Test Coverage** | %18 | Geliştirilmekte |
| **Dokümantasyon** | Orta | Çok kapsamlı |
| **Lambda/Closure** | Yok | Planlı |
| **Generic Tipler** | Optional only | Planlı |
| **Error Handling** | Basit | Gelişmiş |

---

## 💡 Gelişmiş Özellikler

### STO (Small Type Optimization)

Küçük değerler (int, float, küçük string'ler) heap yerine stack'te tutulur. Performans artışı sağlar.

```c
typedef struct {
    uint8_t type;
    union {
        int64_t i;
        double d;
        char small_str[24];
        void* heap_ptr;
    } data;
} STOValue;
```

### Modüler Mimari

Her özellik ayrı modülde:
- Lexer: 5+ modül (tokenize, char utils, token)
- Parser: 15+ modül (expr, stmt, control, func, struct, enum, etc.)
- Codegen: 13+ modül (literal, var, arithmetic, comparison, control, etc.)

### LLVM Integration

Modern compiler infrastructure:
- Optimizasyon passes
- Target-independent code gen
- JIT compilation desteği
- Debug info generation

---

## 🚀 Self-Hosting Strategy

### Stage 0: Bootstrap
Python/C++ ile yazılmış minimal compiler. MELP kodunu LLVM IR'a çevirir.

### Stage 1: Self-Hosted Compiler
MELP ile yazılmış compiler. Stage 0 ile derlenir.

### Stage 2: Optimized Self-Hosted
Stage 1 kendini derler, optimize eder.

### Stage 3+: Full Self-Hosting
Tüm toolchain MELP ile yazılmış.

---

## 📊 İstatistikler

### Kod Satırları

- **MELP Modülleri:** ~12,500 satır
- **Runtime (C):** ~5,000+ satır
- **Dokümantasyon:** ~160KB Markdown
- **Toplam Dosya:** 70+ MLP dosyası

### Modül Dağılımı

- Lexer: 13 dosya
- Parser: 20 dosya
- Codegen: 13 dosya
- Test: 17 dosya
- Bootstrap: 6 dosya

---

## 🔮 Gelecek Planları

### Kısa Vadeli
- [ ] Stage 1 tamamlanması
- [ ] Test coverage artırma
- [ ] Runtime genişletme
- [ ] Error handling iyileştirme

### Orta Vadeli
- [ ] Lambda/closure desteği
- [ ] Generic tipler
- [ ] Module system
- [ ] Package manager

### Uzun Vadeli
- [ ] Full self-hosting (Stage 3+)
- [ ] JIT compilation
- [ ] IDE tooling
- [ ] Standard library expansion

---

## 📞 Notlar

### mlp-original ile Entegrasyon

Bu iki proje farklı yaklaşımları temsil eder:
- **mlp-original:** Basit, x86-64 assembly, hızlı prototipleme
- **MLP-LLVM:** Gelişmiş, LLVM backend, production-ready

### Modül Import

MLP-LLVM'deki gelişmiş modüller, mlp-original'e taşınabilir, ancak:
- LLVM bağımlılıkları kaldırılmalı
- x86-64 assembly codegen'e çevrilmeli
- Runtime uyumluluğu sağlanmalı

---

**Son Güncelleme:** 24 Aralık 2025  
**Katkıda Bulunanlar:** YZ_57, YZ_09, YZ_06, YZ_22  
**Lisans:** Proje LICENSE dosyasına bakın
