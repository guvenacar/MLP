# TODO: Stage0-C PMLP Syntax Desteği

**Tarih:** 25 Aralık 2025  
**Güncelleme:** Mastermind tarafından sadeleştirildi  
**Proje:** mlp-original (guvenacar/mlp)  
**Amaç:** Stage0-C compiler'a PMLP sözdizimi desteği ekle

---

## 🎯 HEDEF

Stage0-C compiler'ı (mlp_compiler.c) PMLP syntax ile uyumlu hale getirmek.

**⚠️ ÖNEMLİ:** Eski syntax'ı SİLMİYORUZ, PMLP syntax DESTEĞI EKLİYORUZ!
- Eski syntax (`end if`, virgülle parametre) → Çalışmaya devam edecek
- Yeni PMLP syntax (`end_if`, noktalı virgülle parametre) → Ekleniyor
- **Geriye dönük uyumluluk korunacak!**

**Referans:** `TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md`

---

## 📊 MEVCUT DURUM

### ✅ Zaten Var:
- `end_if`, `end_while`, `end_for`, `end_function`, `end_struct`, `end_enum`, `end_switch`
- `continue` keyword
- `TOKEN_SEMICOLON` (tanımlı ama separator olarak kullanılmıyor)

### ❌ Eksik:
- `exit` (generic)
- `exit_for`, `exit_while`, `exit_if`, `exit_function`, `exit_switch`
- `continue_for`, `continue_while`
- Semicolon separator (parametrelerde `,` → `;`)

---

## 📋 GÖREVLER

### **PHASE 1: Exit/Continue Tokenları** (1-2 saat)

**Durum:** ✅ TAMAMLANDI (STAGE0_YZ_01) - 25 Aralık 2025

**İlerleme:**
- [x] Task 1.1: Token Enum'a Ekle
- [x] Task 1.2: Keyword Table'a Ekle
- [x] Task 1.3: Parser'da Handle Et
- [x] Task 1.4: Codegen
- [x] Task 1.5: Test

**Dosya:** `compiler/stage1-mlp/mlp_compiler.c`

#### Task 1.1: Token Enum'a Ekle (~line 120-220)
```c
TOKEN_EXIT,
TOKEN_EXIT_FOR,
TOKEN_EXIT_WHILE,
TOKEN_EXIT_IF,
TOKEN_EXIT_FUNCTION,
TOKEN_EXIT_SWITCH,
TOKEN_CONTINUE_FOR,
TOKEN_CONTINUE_WHILE,
```

#### Task 1.2: Keyword Table'a Ekle (~line 1350-1450)
```c
{"exit",           TOKEN_EXIT},
{"exit_for",       TOKEN_EXIT_FOR},
{"exit_while",     TOKEN_EXIT_WHILE},
{"exit_if",        TOKEN_EXIT_IF},
{"exit_function",  TOKEN_EXIT_FUNCTION},
{"exit_switch",    TOKEN_EXIT_SWITCH},
{"continue_for",   TOKEN_CONTINUE_FOR},
{"continue_while", TOKEN_CONTINUE_WHILE},
```

#### Task 1.3: Parser'da Handle Et
- `parse_statement()` içinde exit/continue token'larını işle
- Loop context tracking ekle (hangi loop'tan çıkılacak)

#### Task 1.4: Codegen
- `jmp` instruction ile loop_end veya loop_start label'ına atla

**Test:**
```bash
./build/bin/mlpc tests/pmlp/test_exit.mlp /tmp/test.asm
```

#### ✅ PHASE 1 TEST SONUÇLARI (STAGE0_YZ_01)

**Tamamlama Tarihi:** 25 Aralık 2025  
**Süre:** ~1.5 saat  
**Test Durumu:** 10/10 Başarılı ✅

**Test Dosyaları ve Sonuçları:**
```bash
=== FINAL TEST SUMMARY ===
Testing: test_comprehensive.mlp ... ✅ PASSED
Testing: test_continue_for.mlp ... ✅ PASSED
Testing: test_continue.mlp ... ✅ PASSED
Testing: test_continue_simple.mlp ... ✅ PASSED
Testing: test_exit_for.mlp ... ✅ PASSED
Testing: test_exit_function.mlp ... ✅ PASSED
Testing: test_exit.mlp ... ✅ PASSED
Testing: test_exit_simple.mlp ... ✅ PASSED
Testing: test_generic_exit.mlp ... ✅ PASSED
Testing: test_regression.mlp ... ✅ PASSED
```

**Assembly Doğrulama:**
- `exit_while` → `jmp .L1` (loop end) ✅
- `exit_for` → `jmp .L2` (loop end) ✅
- `continue_while` → `jmp .L0` (loop start) ✅
- `continue_for` → `jmp .L1` (loop start) ✅
- `exit_function` → `leave; ret` ✅
- Generic `exit` → `jmp .L[aktif_dongu_son]` ✅

**Yapılan Değişiklikler:**
1. Token enum'a 8 yeni token eklendi (line ~150-157)
2. Keyword table'a 8 yeni keyword eklendi (line ~1387-1394)
3. Parser'da 8 yeni case eklendi (line ~4200-4260)
4. Codegen'de 8 yeni visit fonksiyonu eklendi (line ~6585-6678)
5. AST enum'a 8 yeni node tipi eklendi (line ~456-463)

**Geriye Dönük Uyumluluk:**
- ✅ Eski syntax (`end_if`, `continue`, `break`) hala çalışıyor
- ✅ Mevcut test dosyaları bozulmadı
- ✅ Regression test başarılı

**Compiler Durumu:**
- ✅ Recompile başarılı (gcc hatasız)
- ✅ Hiç pseudo/stub kod yok
- ✅ Tüm jmp instruction'lar doğru label'lara işaret ediyor

---

### **PHASE 2: Semicolon Separator** (2-3 saat)

**Durum:** ✅ TAMAMLANDI (STAGE0_YZ_02) - 26 Aralık 2025

**İlerleme:**
- [x] Task 2.1: Fonksiyon Parametreleri
- [x] Task 2.2: Fonksiyon Çağrıları
- [x] Task 2.3: Array Literals
- [x] Task 2.4: List Literals
- [x] Task 2.5: Test

#### ✅ PHASE 2 TEST SONUÇLARI (STAGE0_YZ_02)

**Tamamlama Tarihi:** 26 Aralık 2025  
**Karar:** Semicolon-ONLY (hibrit değil)  
**Test Durumu:** 13/13 Başarılı ✅

**Yapılan Değişiklikler:**
- 10 lokasyonda `TOKEN_COMMA || TOKEN_SEMICOLON` → `TOKEN_SEMICOLON`
- Virgül artık SADECE ondalık ayırıcı (3,14 = 3.14)
- Semicolon SADECE parametre/eleman ayırıcı

**⚠️ BREAKING CHANGE:**
- Eski virgül kullanımı ARTIK ÇALIŞMAZ
- `func(a, b)` → Hata (kullan: `func(a; b)`)
- `[1, 2, 3]` → Hata (kullan: `[1; 2; 3;]`)

**Neden Hibrit Değil:**
- Türk formatında virgül ondalık: `123,45` = 123.45
- `(123,45, 44)` ambiguity sorunu çözülemez
- Mastermind kararı: Tek ayırıcı = net syntax

**Kritik:** Bu değişiklik tüm parametre/list/array parsing'i etkiler!

#### Task 2.1: Fonksiyon Parametreleri
```c
// Eski: function add(a, b)
// Yeni: function add(a; b)

// parse_function_declaration() içinde:
// TOKEN_COMMA → TOKEN_SEMICOLON
```

#### Task 2.2: Fonksiyon Çağrıları
```c
// Eski: add(1, 2)
// Yeni: add(1; 2)

// parse_function_call() içinde:
// TOKEN_COMMA → TOKEN_SEMICOLON
```

#### Task 2.3: Array Literals
```c
// Eski: [1, 2, 3]
// Yeni: [1; 2; 3;]  (trailing semicolon!)

// parse_array_literal() içinde değişiklik
```

#### Task 2.4: List Literals
```c
// Eski: (1, 2, 3)
// Yeni: (1; 2; 3;)  (trailing semicolon!)

// parse_list_literal() içinde değişiklik
```

**Not:** `TOKEN_SEMICOLON` zaten tanımlı, sadece kullanım yerlerini güncelle.

---

### **PHASE 3: Test & Migration** (1-2 saat)

**Durum:** ✅ TAMAMLANDI (STAGE0_YZ_03) - 25 Aralık 2025

**İlerleme:**
- [x] Task 3.1: Migration Script
- [x] Task 3.2: Test Dosyaları
- [x] Task 3.3: Regression
- [x] Task 3.4: Final Doğrulama

#### ✅ PHASE 3 TEST SONUÇLARI (STAGE0_YZ_03)

**Tamamlama Tarihi:** 25 Aralık 2025  
**Süre:** ~1 saat  
**Test Durumu:** 10/10 Başarılı ✅

**Oluşturulan Dosyalar:**
```bash
dev/scripts/migrate_to_pmlp.sh          # Migration script (bash)
dev/scripts/run_pmlp_tests.sh           # Test runner script
tests/pmlp/test_semicolon.mlp.phase2    # Phase 2 için hazır (semicolon)
tests/pmlp/test_semicolon_advanced.mlp.phase2  # Phase 2 advanced
tests/pmlp/test_full_pmlp.mlp.phase2    # Phase 1+2 combined test
```

**Regression Test Sonuçları:**
```bash
=== Test Summary ===
Total:  10
Passed: 10
Failed: 0
Success rate: 100%
```

**Test Edilen Dosyalar:**
- test_comprehensive.mlp ✅
- test_continue_for.mlp ✅
- test_continue.mlp ✅
- test_continue_simple.mlp ✅
- test_exit_for.mlp ✅
- test_exit_function.mlp ✅
- test_exit.mlp ✅
- test_exit_simple.mlp ✅
- test_generic_exit.mlp ✅
- test_regression.mlp ✅

**Doğrulama Özeti:**
- ✅ Phase 1 (exit/continue) testleri 100% geçti
- ✅ Eski syntax (regression) hala çalışıyor
- ✅ Assembly dosyaları doğru üretiliyor
- ✅ Migration script hazır ve çalışıyor
- ✅ Test runner script hazır
- ✅ Phase 2 test dosyaları hazır (.phase2 uzantısıyla)

**Not:** Phase 2 (semicolon separator) henüz implementasyonu yapılmadığı için Phase 2 testleri `.phase2` uzantısıyla işaretlendi ve STAGE0_YZ_02 tarafından kullanılmak üzere hazır tutuldu.

---

#### Task 3.1: Migration Script
```bash
#!/bin/bash
# migrate_to_pmlp.sh

# Parametre virgüllerini noktalı virgüle çevir (dikkatli!)
# String literal içindekiler korunmalı

find tests -name "*.mlp" -exec sed -i \
    -e 's/end if/end_if/g' \
    -e 's/end while/end_while/g' \
    -e 's/end for/end_for/g' \
    -e 's/end function/end_function/g' \
    {} \;
```

#### Task 3.2: Test Dosyaları
- `tests/pmlp/test_exit.mlp` - exit testleri
- `tests/pmlp/test_continue.mlp` - continue testleri
- `tests/pmlp/test_semicolon.mlp` - semicolon separator testi

#### Task 3.3: Regression
- Mevcut çalışan testlerin hala geçtiğini doğrula

---

## 👥 YZ GÖREV DAĞILIMI

| YZ | Phase | Süre |
|----|-------|------|
| STAGE0_YZ_01 | Phase 1 (Exit/Continue) | 1-2h |
| STAGE0_YZ_02 | Phase 2 (Semicolon) | 2-3h |
| STAGE0_YZ_03 | Phase 3 (Test/Migration) | 1-2h |

**Toplam: 4-7 saat**

---

## ✅ BAŞARI KRİTERLERİ

**Yeni PMLP Syntax:**
- [x] `exit`, `exit_for`, `exit_while` çalışıyor
- [x] `continue_for`, `continue_while` çalışıyor
- [x] `func(a; b; c)` syntax derleniyor
- [x] `[1; 2; 3;]` array literal çalışıyor
- [x] `(1; 2; 3;)` list literal çalışıyor
- [x] En az 5 PMLP test dosyası geçiyor (13/13)

**Geriye Dönük Uyumluluk:**
- [x] Eski blok syntax (`end_if` vb.) hala çalışıyor
- [x] Exit/continue eski syntax (`break`, `continue`) hala çalışıyor
- [ ] ~~Virgül separator~~ **KALDIRILDI** (Mastermind kararı)

---

## 📚 REFERANSLAR

- `TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md` - PMLP syntax referansı
- `TODO_STAGE0_PMLP/docs/syntax_migration_guide.md` - Migration rehberi
- `compiler/stage1-mlp/mlp_compiler.c` - Hedef dosya (10,303 satır)

---

## 📊 İLERLEME TAKİBİ

**Son Güncelleme:** 26 Aralık 2025

| Phase | Durum | İlerleme | Sorumlu |
|-------|-------|----------|----------|
| Phase 1 | ✅ Tamamlandı | 5/5 task | STAGE0_YZ_01 |
| Phase 2 | ✅ Tamamlandı | 5/5 task | STAGE0_YZ_02 |
| Phase 3 | ✅ Tamamlandı | 4/4 task | STAGE0_YZ_03 |

**Toplam İlerleme:** 14/14 task (%100)

---

**İlerleme:** %100 ✅ TODO TAMAMLANDI
