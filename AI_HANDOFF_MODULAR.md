# 🤖 AI HANDOFF - MELP MODÜLERLEŞTİRME

## ⚠️ DİKKAT: BU BELGE ESKİDİ!

**YENİ BELGE:** `/home/pardus/projeler/MLP/MLP/AI_HANDOFF_CRITICAL.md`

Lütfen önce `AI_HANDOFF_CRITICAL.md` dosyasını oku. O dosya güncel durumu ve yapılması gerekenleri anlatıyor.

---

## 📊 ÖZET

- Mevcut `melp/C/stage0/codegen.c` → 874 satır, MONOLİTİK
- Modüler yapı VAR ama KULLANILMIYOR
- Öncelik: codegen.c'yi parçala, modüllere taşı

---

## ⚡ HIZLI BAŞLANGIÇ (İLK OKU!)

**Sen:** Yeni AI asistanısın. MELP modüler derleyicisi üzerinde çalışıyorsun.

**Durum:** Modülerleştirme %69.7 tamamlandı (23/33 modül aktif). 10 modül isim çakışması nedeniyle bekliyor.

**Şimdi:** Stage 3 Phase 2 - Çakışan modülleri namespace refactoring ile entegre et.

**Nasıl:** Fonksiyon/struct isimlerini modül önekleriyle değiştir (örn: `type_to_string` → `generic_type_to_string`).

---

## 📋 YAPILACAK İŞ (SIRALI)

### ADIM 1: Ortamı Tanı
```bash
cd /home/pardus/projeler/MLP/MLP/melp
ls -la bootstrap/   # C kaynak kodları
ls -la runtime/     # Runtime kütüphanesi
./melp-bootstrap --help  # Derleyiciyi test et
```

### ADIM 2: Mevcut Yapıyı Anla
```
melp/
├── bootstrap/
│   ├── lexer.c      # 683 satır - Token üretici
│   ├── parser.c     # 4698 satır - AST üretici
│   ├── codegen.c    # 4740 satır - Assembly üretici ← EN BÜYÜK!
│   └── main.c       # Giriş noktası
└── runtime/
    └── runtime.c    # 1534 satır - Çalışma zamanı
```

### ADIM 3: Modülleri Oluştur

**Strateji:** `#include` tabanlı bölme. Mevcut çalışan kodu bozmadan modüller oluştur.

**Örnek - codegen.c için:**
```c
// codegen.c (yeni hali)
#include "codegen/types.c"
#include "codegen/variable.c"
#include "codegen/if.c"
#include "codegen/for.c"
// ... diğer modüller
```

### ADIM 4: Test Et
```bash
make clean && make
./melp-bootstrap test.mlp test.s
nasm -f elf64 test.s -o test.o
ld -o test test.o runtime/runtime.o -lc -dynamic-linker /lib64/ld-linux-x86-64.so.2
./test
```

---

## 🎯 TAM MODÜL LİSTESİ

### CODEGEN - `bootstrap/codegen/` (40 modül)

**Grup 1: Temel Yapı**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 1 | `types.c` | VarInfo, FuncInfo, StructInfo, Codegen struct | 10-146 |
| 2 | `core.c` | codegen_create, codegen_emit | 150-210 |
| 3 | `prologue.c` | codegen_emit_prologue, codegen_emit_epilogue | 674-835 |

**Grup 2: Symbol Table (Sembol Tabloları)**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 4 | `variable.c` | codegen_add/find/get_variable | 316-500 |
| 5 | `function.c` | codegen_add/find_function | 566-590 |
| 6 | `struct.c` | codegen_add/find_struct, field | 588-665 |
| 7 | `interface.c` | codegen_add_interface | 642-668 |
| 8 | `enum.c` | codegen_add/find_enum_value | 499-522 |
| 9 | `type_alias.c` | codegen_add/find_type_alias | 523-550 |
| 10 | `string_literal.c` | codegen_add_string | 555-565 |
| 11 | `module_tracker.c` | codegen_module_seen | 188-210 |
| 12 | `generic.c` | codegen_add_generic_func/instance | 237-315 |
| 13 | `operator_overload.c` | codegen_add/find_operator | 212-236 |

**Grup 3: Bildirimler**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 14 | `declaration.c` | codegen_generate_declaration | 836-1130 |
| 15 | `assignment.c` | codegen_generate_assignment | 1131-1263 |
| 16 | `print.c` | codegen_generate_print | 1264-1386 |

**Grup 4: İfadeler (Expressions)**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 17 | `expr_number.c` | EXPR_NUMBER | 1390-1416 |
| 18 | `expr_string.c` | EXPR_STRING | 1417-1421 |
| 19 | `expr_null.c` | EXPR_NULL | 1414-1416 |
| 20 | `expr_variable.c` | EXPR_VARIABLE | 1422-1462 |
| 21 | `expr_binary.c` | EXPR_BINARY_OP (+,-,*,/) | 1463-1696 |
| 22 | `expr_comparison.c` | EXPR_COMPARISON (==,!=,<,>) | 2196-2238 |
| 23 | `expr_ternary.c` | EXPR_TERNARY (?:) | 2239-2283 |
| 24 | `expr_logical.c` | EXPR_LOGICAL_AND/OR/NOT | 2502-2577 |
| 25 | `expr_array.c` | EXPR_ARRAY_INDEX | 2416-2449 |
| 26 | `expr_field.c` | EXPR_FIELD_ACCESS | 2129-2195 |
| 27 | `expr_func_call.c` | EXPR_FUNC_CALL | 1697-2128 |
| 28 | `expr_pointer.c` | EXPR_ADDRESS_OF, EXPR_DEREFERENCE | 2450-2501 |
| 29 | `expr_null_safety.c` | EXPR_OPTIONAL_CHAIN (?.), EXPR_NULL_COALESCE (??) | 2284-2415 |
| 30 | `expr_list.c` | EXPR_LIST_LITERAL () | 2578-2619 |
| 31 | `expr_tuple.c` | EXPR_TUPLE_LITERAL {} | 2620-2648 |
| 32 | `expr_lambda.c` | EXPR_LAMBDA (closure) | 2649-2813 |
| 33 | `expr_await.c` | EXPR_AWAIT (async) | 2814-2822 |
| 34 | `comparison.c` | codegen_generate_comparison | 2823-2963 |

**Grup 5: Kontrol Yapıları**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 35 | `if.c` | codegen_generate_if | 2964-3017 |
| 36 | `for.c` | codegen_generate_for | 3018-3116 |
| 37 | `for_in.c` | codegen_generate_for_in (iterator) | 3117-3275 |
| 38 | `while.c` | codegen_generate_while | 3276-3323 |
| 39 | `do_while.c` | codegen_generate_do_while | 3324-3386 |
| 40 | `switch.c` | codegen_generate_switch | 3387-3542 |

**Grup 6: Fonksiyonlar**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 41 | `function_def.c` | codegen_generate_func_def | 3543-3690 |
| 42 | `return.c` | codegen_generate_return | 3691-3768 |
| 43 | `yield.c` | STMT_YIELD (generator) | 3919-3953 |

**Grup 7: Tip Tanımları**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 44 | `struct_def.c` | STMT_STRUCT_DEF | 3770-3785 |
| 45 | `interface_def.c` | STMT_INTERFACE_DEF | 3786-3792 |
| 46 | `enum_def.c` | STMT_ENUM_DEF | 3793-3799 |
| 47 | `type_alias_def.c` | STMT_TYPE_ALIAS | 3800-3804 |

**Grup 8: Modern Özellikler**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 48 | `try_catch.c` | STMT_TRY_CATCH (exception handling) | 3954-4140 |
| 49 | `throw.c` | STMT_THROW | 4141-4180 |
| 50 | `match.c` | STMT_MATCH (pattern matching) | 4333-4425 |
| 51 | `state.c` | STMT_STATE_DECL (state management) | 4426-4510 |
| 52 | `module_def.c` | STMT_MODULE_DEF | 4202-4240 |
| 53 | `operator_def.c` | STMT_OPERATOR_DEF | 4242-4332 |

**Grup 9: Ana Dispatcher**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 54 | `statement.c` | codegen_generate_statement | 3769-4510 |
| 55 | `generate.c` | codegen_generate (main) | 4512-4741 |

---

### LEXER - `bootstrap/lexer/` (8 modül)

| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 1 | `token_types.c` | TokenType enum (tüm token'lar) | 9-136 |
| 2 | `token.c` | Token struct | 137-145 |
| 3 | `core.c` | Lexer struct, lexer_create | 139-146 |
| 4 | `whitespace.c` | Boşluk, yorum atlama | 147-230 |
| 5 | `keyword.c` | Anahtar kelime tanıma | 231-426 |
| 6 | `literal.c` | Number, string literal parsing | 427-550 |
| 7 | `operator.c` | Operatör token'ları | 550-650 |
| 8 | `scanner.c` | lexer_next_token (ana) | 427-674 |

---

### PARSER - `bootstrap/parser/` (22 modül)

**Grup 1: Temel**
| # | Dosya | Ne Yapar |
|---|-------|----------|
| 1 | `ast_types.c` | VarType, StmtType, ExprType, Expression, Statement |
| 2 | `core.c` | Parser struct, parser_advance |
| 3 | `expression.c` | parser_parse_expression (dispatcher) |
| 4 | `declaration.c` | parser_parse_declaration |

**Grup 2: İfadeler**
| # | Dosya | Ne Yapar |
|---|-------|----------|
| 5 | `expr_primary.c` | parser_parse_primary_expression |
| 6 | `expr_unary.c` | parser_parse_unary_expression |
| 7 | `expr_binary.c` | parser_parse_arithmetic/comparison |
| 8 | `expr_interpolated.c` | parser_parse_interpolated_string |

**Grup 3: Kontrol Yapıları**
| # | Dosya | Ne Yapar |
|---|-------|----------|
| 9 | `if.c` | parser_parse_if_statement |
| 10 | `for.c` | parser_parse_for_statement |
| 11 | `while.c` | parser_parse_while/do_while |
| 12 | `switch.c` | parser_parse_switch_statement |

**Grup 4: Fonksiyonlar & Tipler**
| # | Dosya | Ne Yapar |
|---|-------|----------|
| 13 | `function.c` | parser_parse_func_definition |
| 14 | `struct.c` | parser_parse_struct_definition |
| 15 | `interface.c` | parser_parse_interface_definition |
| 16 | `enum.c` | parser_parse_enum |

**Grup 5: Modern Özellikler**
| # | Dosya | Ne Yapar |
|---|-------|----------|
| 17 | `try_catch.c` | parser_parse_try_catch |
| 18 | `match.c` | parser_parse_match_statement |
| 19 | `state.c` | parser_parse_state_statement |
| 20 | `import.c` | parser_parse_import_statement |
| 21 | `module.c` | parser_parse_module_definition |
| 22 | `operator.c` | parser_parse_operator_definition |

---

### RUNTIME - `runtime/` (17 modül)

**Grup 1: Temel**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 1 | `memory.c` | mlp_malloc, mlp_free | 11-25 |
| 2 | `print.c` | print_number, print_double, print_string | 27-84 |

**Grup 2: String İşlemleri**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 3 | `string.c` | string_length, string_concat, string_equal | 85-175 |
| 4 | `string_utils.c` | mlp_substring, mlp_indexOf, mlp_charAt | 439-565 |
| 5 | `string_advanced.c` | mlp_split, mlp_trim, mlp_replace, mlp_to_upper | 1227-1470 |

**Grup 3: Veri Yapıları**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 6 | `array.c` | mlp_array_alloc/free/length/resize | 176-265 |
| 7 | `range.c` | mlp_range, mlp_range1, mlp_range2 | 266-308 |

**Grup 4: I/O**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 8 | `file.c` | mlp_file_read/write/exists/append | 314-437 |
| 9 | `input.c` | mlp_input, mlp_input_prompt | 1099-1135 |
| 10 | `cli.c` | mlp_get_argv, mlp_get_argc | 550-580 |

**Grup 5: Modern Özellikler**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 11 | `gc.c` | gc_init/alloc/free/collect/shutdown | 755-985 |
| 12 | `exception.c` | mlp_exception_*, mlp_throw | 584-700 |
| 13 | `null_safety.c` | mlp_is_null, mlp_coalesce, mlp_safe_deref | 706-750 |

**Grup 6: Yardımcılar**
| # | Dosya | Ne Yapar | Satırlar |
|---|-------|----------|----------|
| 14 | `conversion.c` | mlp_to_numeric, mlp_to_string, mlp_to_decimal | 988-1095 |
| 15 | `math.c` | mlp_sin, mlp_cos, mlp_tan, mlp_sqrt, mlp_pow... | 1139-1220 |
| 16 | `assert.c` | mlp_assert | 1471-1488 |
| 17 | `bitwise.c` | mlp_band, mlp_bor, mlp_bxor, mlp_shl, mlp_shr | 1489-1534 |

---

## 📊 ÖZET

| Bileşen | Modül Sayısı |
|---------|--------------|
| CODEGEN | 55 |
| LEXER | 8 |
| PARSER | 22 |
| RUNTIME | 17 |
| **TOPLAM** | **102** |

---

## ⚠️ KRİTİK KURALLAR

1. **`string` KULLAN, `text` KULLANMA!** (PMLP standardı)
2. **Her değişiklikten sonra test et!**
3. **Çalışan kodu bozma** - önce yedekle
4. **Küçük adımlarla ilerle** - bir modül bitir, test et, sonrakine geç

---

## 🔧 YARDIMCI KOMUTLAR

```bash
# Tüm fonksiyonları bul
grep "^void codegen_" bootstrap/codegen.c

# Belirli satırları oku
sed -n '2964,3017p' bootstrap/codegen.c  # if.c için

# Derle ve test et
make clean && make && ./melp-bootstrap test.mlp test.s

# Git durumu
git status
git diff
```

---

## 📊 İLERLEME TAKİBİ

Her modülü tamamladığında işaretle:

```
CODEGEN:
[ ] types.c
[ ] core.c
[ ] variable.c
[ ] function.c
[ ] struct.c
[ ] declaration.c
[ ] assignment.c
[ ] print.c
[ ] expression.c
[ ] comparison.c
[ ] if.c
[ ] for.c
[ ] for_in.c
[ ] while.c
[ ] do_while.c
[ ] switch.c
[ ] function_def.c
[ ] return.c
[ ] try_catch.c
[ ] throw.c
[ ] async.c
[ ] lambda.c
[ ] generic.c
[ ] operator_def.c
```

---

## 💡 İPUCU

Eğer takılırsan:
1. `bootstrap/codegen.c` dosyasını aç
2. İlgili satır aralığını oku
3. O kısmı yeni dosyaya kopyala
4. Ana dosyadan `#include` ile dahil et
5. Test et

**Başarılar!** 🚀
