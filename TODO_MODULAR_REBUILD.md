# 📋 MELP MODÜLER YENİDEN YAZIM - TODO

**Başlangıç:** 1 Aralık 2025  
**Son Güncelleme:** 1 Aralık 2025 21:10  
**Strateji:** Her modül = Bir dosya (C + MLP versiyonu)

---

## 🎯 DURUM

### ✅ TAMAMLANDI
- **Stage 0:** 33 modül × 3 dosya = 99 C dosyası oluşturuldu
- **Stage 1:** 10 parser modülü (3,420 satır kod) eklendi
- **Stage 2:** 18 modül entegre edildi (function + memory eklendi)
- **Stage 3 Phase 1:** 5 çakışmasız modül eklendi (23/33 - %69.7)

### 🔄 DEVAM EDİYOR
- **Stage 3 Phase 2:** Çakışan 10 modülün namespace refactoring'i

---

## 📊 STAGE 3 PHASE 1 SONUÇLARI

### Aktif Modüller (23/33 - %69.7)
✅ **Core (18):** variable, arithmetic, comparison, logical, string_ops, array, struct,  
✅ control_flow, function, pointer, memory, cli_io, file_io, preprocessor,  
✅ reflection, debug_features, concurrency, state_management

✅ **Advanced (5):** lambda, generator, async, exception_handling, module_system

### Devre Dışı Modüller (10/33)
❌ interface_trait (type_to_string çakışması - reflection ile)
❌ generic_types (type_to_string, codegen_type_info çakışması - reflection ile)
❌ operator_overloading (codegen_type_check çakışması - reflection ile)
❌ enum (codegen_match_arm çakışması - pattern_matching ile)
❌ pattern_matching (Match struct, codegen_match_arm çakışması)
❌ switch_match (Match struct, codegen_match çakışması)
❌ null_safety (codegen_null_check çakışması - pointer ile)

### İsim Çakışmaları (7 Farklı Conflict)
1. **Reflection Conflicts (3 modül):**
   - `type_to_string()`: reflection ↔ generic_types, interface_trait
   - `codegen_type_info()`: reflection ↔ generic_types
   - `codegen_type_check()`: reflection ↔ operator_overloading

2. **Pattern Matching Conflicts (3 modül):**
   - `struct Match`: pattern_matching ↔ switch_match
   - `codegen_match_arm()`: enum ↔ pattern_matching
   - `codegen_match()`: pattern_matching ↔ switch_match

3. **Null Safety Conflict (1 modül):**
   - `codegen_null_check()`: pointer ↔ null_safety

### Test Durumu
- ✅ 7 test dosyası oluşturuldu (variable, arithmetic, comparison, logical, string, array, struct)
- ✅ Tüm testler derleniyor
- ✅ test_variable çalışıyor (exit 0)
- ✅ Binary boyutu: 467 KB (292 KB → 325 KB → 467 KB)

---

## 🎯 STAGE 3 PHASE 2: NAMESPACE REFACTORING

**Başlangıç:** 1 Aralık 2025 ~22:00  
**Tahmini Süre:** 2.5-3 saat  
**Hedef:** 33/33 modül (%100) - Tüm çakışmaları çöz

### Step 1: Reflection Conflicts (1 saat)
**Modüller:** interface_trait, generic_types, operator_overloading

- [ ] **interface_trait refactor**
  - [ ] `type_to_string()` → `trait_type_to_string()`
  - [ ] interface_trait.h ve .c dosyalarını düzenle
  - [ ] Makefile'a ekle, derle, test et

- [ ] **generic_types refactor**
  - [ ] `type_to_string()` → `generic_type_to_string()`
  - [ ] `codegen_type_info()` → `codegen_generic_type_info()`
  - [ ] generic_types.h, .c ve _codegen.h, _codegen.c düzenle
  - [ ] Makefile'a ekle, derle, test et

- [ ] **operator_overloading refactor**
  - [ ] `codegen_type_check()` → `codegen_operator_type_check()`
  - [ ] operator_overloading_codegen.h ve .c düzenle
  - [ ] Makefile'a ekle, derle, test et

**Progress After:** 23 → 26 modules (78.8%)

### Step 2: Pattern Matching Conflicts (1 saat)
**Modüller:** switch_match, enum, pattern_matching

- [ ] **switch_match refactor**
  - [ ] `struct Match` → `struct SwitchMatch`
  - [ ] `codegen_match()` → `codegen_switch()`
  - [ ] Tüm switch_match dosyalarını düzenle
  - [ ] Makefile'a ekle, derle, test et

- [ ] **enum refactor**
  - [ ] `codegen_match_arm()` → `codegen_enum_match_arm()`
  - [ ] enum_codegen.h ve .c düzenle
  - [ ] Makefile'a ekle, derle, test et

- [ ] **pattern_matching refactor**
  - [ ] `struct Match` → `struct PatternMatch` (opsiyonel)
  - [ ] `codegen_match_arm()` → `codegen_pattern_match_arm()`
  - [ ] pattern_matching_codegen.h ve .c düzenle
  - [ ] Makefile'a ekle, derle, test et

**Progress After:** 26 → 29 modules (87.9%)

### Step 3: Null Safety Conflict (15 dakika)
**Modül:** null_safety

- [ ] **null_safety refactor**
  - [ ] `codegen_null_check()` → `codegen_null_safety_check()`
  - [ ] `codegen_null_check_expr()` → `codegen_null_safety_check_expr()`
  - [ ] null_safety_codegen.h ve .c düzenle
  - [ ] Makefile'a ekle, derle, test et

**Progress After:** 29 → 30 modules (90.9%)

### Step 4: Final Integration & Testing (30 dakika)

- [ ] Tüm 33 modülü main.c'ye ekle
- [ ] Makefile'da MODULE_DIRS = 33 yap
- [ ] `make clean && make` → Derleme kontrolü
- [ ] Binary boyutunu kontrol et (tahmini ~550 KB)
- [ ] Tüm test dosyalarını derle (7 test)
- [ ] Test sonuçlarını kaydet
- [ ] **melp_yedek_stage3_complete** backup oluştur
- [ ] STAGE3_COMPLETE.md dokümantasyonu yaz

**Progress After:** 30 → 33 modules (100%) ✅

---

## 📦 MODÜL LİSTESİ

### LEXER MODÜLLER
| # | C Dosyası | MLP Dosyası | Açıklama |
|---|-----------|-------------|----------|
| L1 | `lexer/token_types.c` | `lexer/token_types.mlp` | TokenType enum |
| L2 | `lexer/token.c` | `lexer/token.mlp` | Token struct |
| L3 | `lexer/lexer_core.c` | `lexer/lexer_core.mlp` | Lexer struct, create, free |
| L4 | `lexer/lexer_whitespace.c` | `lexer/lexer_whitespace.mlp` | Boşluk/yorum atlama |
| L5 | `lexer/lexer_number.c` | `lexer/lexer_number.mlp` | Sayı okuma |
| L6 | `lexer/lexer_string.c` | `lexer/lexer_string.mlp` | String literal okuma |
| L7 | `lexer/lexer_identifier.c` | `lexer/lexer_identifier.mlp` | Identifier/keyword okuma |
| L8 | `lexer/lexer_operator.c` | `lexer/lexer_operator.mlp` | Operatör okuma |
| L9 | `lexer/lexer_next.c` | `lexer/lexer_next.mlp` | lexer_next_token (dispatcher) |

### PARSER MODÜLLER
| # | C Dosyası | MLP Dosyası | Açıklama |
|---|-----------|-------------|----------|
| P1 | `parser/ast_types.c` | `parser/ast_types.mlp` | VarType, ExprType, StmtType |
| P2 | `parser/ast_expr.c` | `parser/ast_expr.mlp` | Expression struct |
| P3 | `parser/ast_stmt.c` | `parser/ast_stmt.mlp` | Statement struct |
| P4 | `parser/ast_decl.c` | `parser/ast_decl.mlp` | Declaration struct |
| P5 | `parser/parser_core.c` | `parser/parser_core.mlp` | Parser struct, create, advance |
| P6 | `parser/parser_expr_primary.c` | `parser/parser_expr_primary.mlp` | Primary expression parsing |
| P7 | `parser/parser_expr_binary.c` | `parser/parser_expr_binary.mlp` | Binary expression parsing |
| P8 | `parser/parser_stmt_decl.c` | `parser/parser_stmt_decl.mlp` | Declaration parsing |
| P9 | `parser/parser_stmt_assign.c` | `parser/parser_stmt_assign.mlp` | Assignment parsing |
| P10 | `parser/parser_stmt_print.c` | `parser/parser_stmt_print.mlp` | Print parsing |
| P11 | `parser/parser_stmt_if.c` | `parser/parser_stmt_if.mlp` | If parsing |
| P12 | `parser/parser_stmt_for.c` | `parser/parser_stmt_for.mlp` | For parsing |
| P13 | `parser/parser_stmt_while.c` | `parser/parser_stmt_while.mlp` | While parsing |
| P14 | `parser/parser_stmt_func.c` | `parser/parser_stmt_func.mlp` | Function parsing |
| P15 | `parser/parser_main.c` | `parser/parser_main.mlp` | parse_program (dispatcher) |

### CODEGEN MODÜLLER
| # | C Dosyası | MLP Dosyası | Açıklama |
|---|-----------|-------------|----------|
| C1 | `codegen/codegen_types.c` | `codegen/codegen_types.mlp` | Codegen struct, VarInfo |
| C2 | `codegen/codegen_core.c` | `codegen/codegen_core.mlp` | create, free |
| C3 | `codegen/codegen_emit.c` | `codegen/codegen_emit.mlp` | emit fonksiyonu |
| C4 | `codegen/codegen_variable.c` | `codegen/codegen_variable.mlp` | add/find variable |
| C5 | `codegen/codegen_string.c` | `codegen/codegen_string.mlp` | add_string |
| C6 | `codegen/codegen_expr_number.c` | `codegen/codegen_expr_number.mlp` | EXPR_NUMBER |
| C7 | `codegen/codegen_expr_string.c` | `codegen/codegen_expr_string.mlp` | EXPR_STRING |
| C8 | `codegen/codegen_expr_variable.c` | `codegen/codegen_expr_variable.mlp` | EXPR_VARIABLE |
| C9 | `codegen/codegen_expr_binary.c` | `codegen/codegen_expr_binary.mlp` | EXPR_BINARY_OP |
| C10 | `codegen/codegen_expr.c` | `codegen/codegen_expr.mlp` | Expression dispatcher |
| C11 | `codegen/codegen_decl.c` | `codegen/codegen_decl.mlp` | generate_declaration |
| C12 | `codegen/codegen_assign.c` | `codegen/codegen_assign.mlp` | generate_assignment |
| C13 | `codegen/codegen_print.c` | `codegen/codegen_print.mlp` | generate_print |
| C14 | `codegen/codegen_compare.c` | `codegen/codegen_compare.mlp` | generate_comparison |
| C15 | `codegen/codegen_if.c` | `codegen/codegen_if.mlp` | generate_if |
| C16 | `codegen/codegen_for.c` | `codegen/codegen_for.mlp` | generate_for |
| C17 | `codegen/codegen_while.c` | `codegen/codegen_while.mlp` | generate_while |
| C18 | `codegen/codegen_func.c` | `codegen/codegen_func.mlp` | function table |
| C19 | `codegen/codegen_funcdef.c` | `codegen/codegen_funcdef.mlp` | generate_func_def |
| C20 | `codegen/codegen_return.c` | `codegen/codegen_return.mlp` | generate_return |
| C21 | `codegen/codegen_stmt.c` | `codegen/codegen_stmt.mlp` | Statement dispatcher |
| C22 | `codegen/codegen_prologue.c` | `codegen/codegen_prologue.mlp` | Prologue/epilogue |
| C23 | `codegen/codegen_main.c` | `codegen/codegen_main.mlp` | codegen_generate |

### RUNTIME MODÜLLER
| # | C Dosyası | Açıklama |
|---|-----------|----------|
| R1 | `runtime/rt_print_number.c` | print_number |
| R2 | `runtime/rt_print_string.c` | print_string |
| R3 | `runtime/rt_memory.c` | malloc/free wrapper |

---

## 🚀 FAZE 1: MİNİMAL ÇALIŞAN SİSTEM

**Hedef:** `numeric x = 42` derlensin ve çalışsın

### Gerekli Modüller:
- [ ] L1: token_types.c
- [ ] L2: token.c
- [ ] L3: lexer_core.c
- [ ] L5: lexer_number.c
- [ ] L7: lexer_identifier.c
- [ ] L9: lexer_next.c
- [ ] P1: ast_types.c
- [ ] P2: ast_expr.c
- [ ] P4: ast_decl.c
- [ ] P5: parser_core.c
- [ ] P6: parser_expr_primary.c
- [ ] P8: parser_stmt_decl.c
- [ ] C1: codegen_types.c
- [ ] C2: codegen_core.c
- [ ] C3: codegen_emit.c
- [ ] C4: codegen_variable.c
- [ ] C6: codegen_expr_number.c
- [ ] C10: codegen_expr.c (dispatcher)
- [ ] C11: codegen_decl.c
- [ ] C21: codegen_stmt.c (dispatcher)
- [ ] C22: codegen_prologue.c
- [ ] C23: codegen_main.c
- [ ] main.c
- [ ] Makefile

### Test:
```
numeric x = 42
-- Program sadece x değişkenini tanımlayıp çıkacak (exit code 0)
```

### FAZE 2: PRINT STATEMENT
**Hedef:** `print(x)` çalışsın

- [ ] `codegen/codegen_expr.c` - EXPR_VARIABLE, EXPR_NUMBER
- [ ] `codegen/codegen_print.c` - generate_print
- [ ] `runtime/runtime.c` - print_number
- [ ] **TEST:** `numeric x = 42` + `print(x)` → "42" yazsın

### FAZE 3: ARİTMETİK İFADELER
**Hedef:** `numeric y = x + 10` çalışsın

- [ ] `codegen/codegen_expr.c` - EXPR_BINARY_OP (+, -, *, /)
- [ ] **TEST:** `numeric x = 10` + `numeric y = x * 2` + `print(y)` → "20"

### FAZE 4: STRING TİPİ
**Hedef:** `string s = "hello"` çalışsın

- [ ] lexer: string literal desteği
- [ ] parser: TYPE_STRING
- [ ] `codegen/codegen_string.c` - string literal, add_string
- [ ] `runtime/runtime.c` - print_string
- [ ] **TEST:** `string s = "hello"` + `print(s)` → "hello"

### FAZE 5: ATAMA (ASSIGNMENT)
**Hedef:** `x = 100` çalışsın

- [ ] parser: STMT_ASSIGNMENT
- [ ] `codegen/codegen_assign.c` - generate_assignment
- [ ] **TEST:** `numeric x = 10` + `x = 20` + `print(x)` → "20"

### FAZE 6: KARŞILAŞTIRMA VE IF
**Hedef:** `if x > 10 then ... end if` çalışsın

- [ ] lexer: >, <, ==, !=, if, then, else, end
- [ ] parser: STMT_IF, EXPR_COMPARISON
- [ ] `codegen/codegen_compare.c` - generate_comparison
- [ ] `codegen/codegen_if.c` - generate_if
- [ ] **TEST:** if-else çalışsın

### FAZE 7: FOR DÖNGÜSÜ
**Hedef:** `for i = 1 to 10 ... end for` çalışsın

- [ ] lexer: for, to, step, end for
- [ ] parser: STMT_FOR
- [ ] `codegen/codegen_for.c` - generate_for
- [ ] **TEST:** 1'den 10'a sayma

### FAZE 8: WHILE DÖNGÜSÜ
**Hedef:** `while ... end while` çalışsın

- [ ] lexer: while
- [ ] parser: STMT_WHILE
- [ ] `codegen/codegen_while.c` - generate_while
- [ ] **TEST:** while döngüsü

### FAZE 9: FONKSİYONLAR
**Hedef:** `function add(a, b) ... end function` çalışsın

- [ ] lexer: function, returns, end function
- [ ] parser: STMT_FUNC_DEF, EXPR_FUNC_CALL
- [ ] `codegen/codegen_function.c` - function table
- [ ] `codegen/codegen_funcdef.c` - generate_func_def
- [ ] `codegen/codegen_return.c` - generate_return
- [ ] `codegen/codegen_expr.c` - EXPR_FUNC_CALL
- [ ] **TEST:** Fonksiyon tanımlama ve çağırma

### FAZE 10: STRUCT
**Hedef:** `struct Point ... end struct` çalışsın

- [ ] lexer: struct
- [ ] parser: STMT_STRUCT_DEF
- [ ] `codegen/codegen_struct.c` - struct management
- [ ] `codegen/codegen_expr.c` - EXPR_FIELD_ACCESS
- [ ] **TEST:** Struct tanımlama ve kullanma

### FAZE 11: ARRAY
**Hedef:** `numeric[] arr = [1, 2, 3]` çalışsın

- [ ] parser: array type, EXPR_ARRAY_LITERAL, EXPR_ARRAY_INDEX
- [ ] `codegen/codegen_expr.c` - array işlemleri
- [ ] `runtime/runtime.c` - array alloc/free
- [ ] **TEST:** Array oluşturma ve erişim

### FAZE 12-20: GELİŞMİŞ ÖZELLİKLER
- [ ] Switch/Case
- [ ] Do-While
- [ ] Module/Import
- [ ] Generic Types
- [ ] Interface
- [ ] Enum
- [ ] Try/Catch
- [ ] Lambda/Closure
- [ ] Pattern Matching (match)
- [ ] State Management
- [ ] Async/Await
- [ ] Operator Overloading

---

## 📁 HEDEF DOSYA YAPISI

```
melp/
├── bootstrap/
│   ├── lexer.c              # Tokenizer
│   ├── parser.h             # Tip tanımları
│   ├── parser.c             # AST builder
│   ├── main.c               # Entry point
│   ├── Makefile
│   └── codegen/
│       ├── codegen.h        # Master header
│       ├── codegen_all.c    # Master include
│       ├── codegen_core.c   # create, free, emit
│       ├── codegen_variable.c
│       ├── codegen_decl.c
│       ├── codegen_assign.c
│       ├── codegen_print.c
│       ├── codegen_expr.c
│       ├── codegen_compare.c
│       ├── codegen_if.c
│       ├── codegen_for.c
│       ├── codegen_while.c
│       ├── codegen_funcdef.c
│       ├── codegen_return.c
│       ├── codegen_struct.c
│       ├── codegen_switch.c
│       └── codegen_main.c
├── runtime/
│   └── runtime.c
├── selfhost/                # MLP ile yazılacak (gelecek)
└── tests/
    ├── test_decl.mlp
    ├── test_print.mlp
    ├── test_expr.mlp
    └── ...
```

---

## 🔴 MEVCUT DURUM

**Aktif Görev:** FAZE 1 - Değişken Tanımlama

**Son Güncelleme:** 1 Aralık 2025

---

## 📝 NOTLAR

- Her faz tamamlandığında bu dosyayı güncelle
- Çalışmayan kod commit etme
- Her modül için test yaz
- Monolitik koddan kopyala-yapıştır yapma, anla ve yeniden yaz
