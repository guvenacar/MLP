# MLP Self-Hosting & Modülerleştirme Planı

**Başlangıç:** 30 Kasım 2025  
**Hedef:** Modüler C compiler → Modüler MLP self-hosted compiler

---

## 📋 Genel Bakış

### Neden Modülerleştirme?

1. **Bakım kolaylığı** - Her dosya tek bir konuya odaklanır
2. **Paralel geliştirme** - Farklı modüller bağımsız çalışılabilir
3. **Test kolaylığı** - Her modül izole test edilebilir
4. **Aşamalı self-hosting** - Modül modül MLP'ye çevrilebilir
5. **AI uyumluluğu** - Gelecek AI'lar daha kolay anlayabilir

### Strateji

```
Faz 1: C modülerleştirme (mevcut monolitik → modüler C)
Faz 2: Self-hosting (modüler C → modüler MLP)
Faz 3: TTO & Optimizasyon
```

---

## 🎯 Faz 1: C Modülerleştirme

### 1.1 Lexer (Hazır - değişmeyecek)

```
bootstrap/
└── lexer.c (~500 satır) ✅ Yeterince küçük
```

### 1.2 Parser Modülleri

| # | Modül | Kaynak | Satır | Durum | Açıklama |
|---|-------|--------|-------|-------|----------|
| 1.2.1 | `parser_core.c` | parser.c | ~200 | ⬜ | Parser struct, utility fonksiyonlar |
| 1.2.2 | `parser_expr.c` | parser.c | ~800 | ⬜ | Expression parsing (binary, unary, primary) |
| 1.2.3 | `parser_stmt.c` | parser.c | ~600 | ⬜ | Statement parsing (if, while, for, switch) |
| 1.2.4 | `parser_decl.c` | parser.c | ~500 | ⬜ | Declaration parsing (func, struct, enum) |
| 1.2.5 | `parser_type.c` | parser.c | ~300 | ⬜ | Type parsing (generic, nullable) |
| 1.2.6 | `parser.h` | - | ~200 | ⬜ | Shared header |

### 1.3 Codegen Modülleri

| # | Modül | Kaynak | Satır | Durum | Açıklama |
|---|-------|--------|-------|-------|----------|
| 1.3.1 | `codegen_core.c` | codegen.c | ~400 | ⬜ | Codegen struct, emit, strings |
| 1.3.2 | `codegen_expr.c` | codegen.c | ~800 | ⬜ | Expression codegen |
| 1.3.3 | `codegen_stmt.c` | codegen.c | ~400 | ⬜ | Temel statement codegen |
| 1.3.4 | `codegen_if.c` | codegen.c | ~200 | ⬜ | If/else codegen |
| 1.3.5 | `codegen_loop.c` | codegen.c | ~400 | ⬜ | For/while/do-while codegen |
| 1.3.6 | `codegen_func.c` | codegen.c | ~500 | ⬜ | Function codegen |
| 1.3.7 | `codegen_struct.c` | codegen.c | ~300 | ⬜ | Struct codegen |
| 1.3.8 | `codegen_match.c` | codegen.c | ~200 | ⬜ | Pattern matching codegen |
| 1.3.9 | `codegen_async.c` | codegen.c | ~300 | ⬜ | Async/await codegen |
| 1.3.10 | `codegen_tto.c` | codegen.c | ~200 | ⬜ | TTO optimizations |
| 1.3.11 | `codegen.h` | - | ~300 | ⬜ | Shared header |

### 1.4 Modülerleştirme Sırası

```
Hafta 1:
├── [ ] 1. codegen_core.c - Temel altyapı
├── [ ] 2. codegen.h - Shared types
├── [ ] 3. codegen_expr.c - Expression (en büyük)
└── [ ] 4. Test: Mevcut testler geçmeli

Hafta 2:
├── [ ] 5. codegen_stmt.c - Statements
├── [ ] 6. codegen_if.c - If/else
├── [ ] 7. codegen_loop.c - Loops
└── [ ] 8. Test

Hafta 3:
├── [ ] 9. codegen_func.c - Functions
├── [ ] 10. codegen_struct.c - Structs
├── [ ] 11. codegen_match.c - Pattern matching
├── [ ] 12. codegen_async.c - Async
└── [ ] 13. Test

Hafta 4:
├── [ ] 14. parser_core.c
├── [ ] 15. parser_expr.c
├── [ ] 16. parser_stmt.c
├── [ ] 17. parser_decl.c
├── [ ] 18. parser_type.c
└── [ ] 19. Final test
```

---

## 🎯 Faz 2: Self-Hosting

### 2.1 Önkoşullar (C'de tamamlanmalı)

| # | Özellik | Durum | Açıklama |
|---|---------|-------|----------|
| 2.1.1 | State fonksiyon içi erişim | ✅ | Bug düzeltildi |
| 2.1.2 | `true`/`false` literals | ⬜ | Parser'da keyword olmalı |
| 2.1.3 | `print(func_call())` | ⬜ | Function return → print |
| 2.1.4 | String karşılaştırma | ✅ | `==` çalışıyor |
| 2.1.5 | TTO numeric | ⬜ | int64/double seçimi |
| 2.1.6 | TTO text (SSO) | ⬜ | Inline/heap seçimi |

### 2.2 MLP Modülleri

| # | Modül | C Karşılığı | Durum | Açıklama |
|---|-------|-------------|-------|----------|
| 2.2.1 | `tokens.mlp` | lexer.c | ⬜ | Token tanımları |
| 2.2.2 | `lexer.mlp` | lexer.c | 🔄 | Tokenizer (başlandı) |
| 2.2.3 | `ast.mlp` | parser.c | ⬜ | AST node tanımları |
| 2.2.4 | `parser.mlp` | parser_*.c | ⬜ | Parser |
| 2.2.5 | `emit.mlp` | codegen_core.c | ⬜ | Assembly emit |
| 2.2.6 | `codegen.mlp` | codegen_*.c | ⬜ | Code generator |
| 2.2.7 | `main.mlp` | main.c | ⬜ | Entry point |

### 2.3 Self-Hosting Sırası

```
1. [ ] tokens.mlp - Token enum tanımları
2. [ ] lexer.mlp - Tokenizer (devam)
3. [ ] ast.mlp - AST types
4. [ ] parser.mlp - Parser
5. [ ] emit.mlp - Assembly emit helpers
6. [ ] codegen.mlp - Code generator
7. [ ] main.mlp - Compiler entry
8. [ ] Test: lexer.mlp kendini tokenize edebilmeli
9. [ ] Test: parser.mlp kendini parse edebilmeli
10. [ ] Test: Tam self-hosting
```

---

## 🎯 Faz 3: TTO & Yeni Özellikler

### 3.1 TTO Numeric

| # | Özellik | Durum | Açıklama |
|---|---------|-------|----------|
| 3.1.1 | int64 temel | ✅ | Mevcut |
| 3.1.2 | double temel | ✅ | Mevcut |
| 3.1.3 | Literal analizi | ⬜ | `42` → int64, `3.14` → double |
| 3.1.4 | Runtime promotion | ⬜ | Overflow → double |
| 3.1.5 | BigDecimal | ⬜ | Çok büyük sayılar |

### 3.2 TTO Text (SSO)

| # | Özellik | Durum | Açıklama |
|---|---------|-------|----------|
| 3.2.1 | SSO struct | ⬜ | 24-byte inline |
| 3.2.2 | Heap fallback | ⬜ | >23 byte |
| 3.2.3 | String ops | ⬜ | concat, substring SSO-aware |

### 3.3 Yeni Sözdizimi

| # | Özellik | Durum | Açıklama |
|---|---------|-------|----------|
| 3.3.1 | `numeric a = 123_456` | ⬜ | Binlik ayracı (Python) |
| 3.3.2 | `numeric a = 123.456,78` | ⬜ | Türkçe format |
| 3.3.3 | `list() a = ["Ali", 10]` | ⬜ | Heterojen liste |
| 3.3.4 | `(a, b) = func()` | ⬜ | Tuple destructuring |
| 3.3.5 | `numeric a, b = 1, 2` | ⬜ | Çoklu tanımlama |

---

## 📊 İlerleme Takibi

### Haftalık Hedefler

```
Hafta 1 (2-8 Aralık):
├── Codegen modülerleştirme başlangıç
└── codegen_core.c, codegen_expr.c

Hafta 2 (9-15 Aralık):
├── Codegen modülerleştirme devam
└── codegen_stmt.c, codegen_if.c, codegen_loop.c

Hafta 3 (16-22 Aralık):
├── Codegen modülerleştirme tamamla
└── codegen_func.c, codegen_struct.c, etc.

Hafta 4 (23-29 Aralık):
├── Parser modülerleştirme
└── parser_*.c dosyaları

Hafta 5 (30 Aralık - 5 Ocak):
├── Self-hosting başlangıç
└── tokens.mlp, lexer.mlp

Hafta 6+ (Ocak 2025):
├── Self-hosting devam
└── parser.mlp, codegen.mlp
```

### Durum Açıklamaları

- ⬜ Başlanmadı
- 🔄 Devam ediyor
- ✅ Tamamlandı
- ❌ Bloke/Sorunlu

---

## 📝 Notlar

### Önemli Kararlar

1. **Runtime C'de kalacak** - Self-hosting sadece lexer/parser/codegen
2. **Modüler önce** - C modülerleşmeden MLP'ye geçme
3. **Test odaklı** - Her modül sonrası testler geçmeli
4. **Geriye uyumluluk** - Mevcut .mlp dosyaları çalışmaya devam etmeli

### Bilinen Sorunlar

1. `true`/`false` keyword değil identifier olarak parse ediliyor
2. `print(func())` düzgün çalışmıyor
3. String interpolation içinde state değişken sorunu

### Referanslar

- `MLP_LANGUAGE_SPEC.md` - Dil spesifikasyonu
- `melp/bootstrap/` - Mevcut C compiler
- `melp/selfhost/` - MLP compiler (WIP)
- `melp/runtime/` - C runtime

---

**Son Güncelleme:** 30 Kasım 2025
