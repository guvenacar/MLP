# RF_YZ_3 Parser Simplification - HANDOFF

**Tarih:** 14 Aralık 2025  
**Önceki YZ:** YZ_74 (tamamlanmadı, devir)  
**Durum:** 🟡 85% Complete - Integration bug var

---

## 🎯 Görev

Parser'ı PMPL single token'ları kullanacak şekilde refactor et.

**Hedef:** Pattern matching hack'lerini kaldır (TOKEN_END + peek + TOKEN_IF), single token switch-case kullan (TOKEN_END_IF).

---

## ✅ Tamamlanan İşler

### 1. Normalize Layer Entegrasyonu
- `compiler/stage0/modules/functions/functions_standalone.c`
  - Line 13: `#include "../../normalize/normalize.h"` eklendi
  - Line 179-183: `normalize_to_pmpl()` çağrısı eklendi (read_file ile lexer_create arasında)
- `compiler/stage0/modules/functions/Makefile`
  - Line 135-137: NORMALIZE_DIR ve NORMALIZE_SOURCES tanımlandı
  - SOURCES listesine eklendi
- ✅ Test: `test_normalize_output` başarılı ("end if" -> "end_if" dönüşümü çalışıyor)

### 2. statement_parser.c Refactor
- **Line 47-68:** TOKEN_END_* single token handling (switch-case)
  ```c
  case TOKEN_END_IF:
  case TOKEN_END_WHILE:
  case TOKEN_END_FOR:
  // ... 9 terminator
  ```
- **Line 72-76:** TOKEN_ELSE handling (parent'a döndür)
- **Line 78-156:** TOKEN_ELSE_IF handling (yeni eklendi)
  - else_if'i nested if olarak parse ediyor
  - Synthetic TOKEN_ELSE parent'a signal veriyor

### 3. functions_standalone.c Refactor
- **Line 96-109:** skip_to_sync_point - TOKEN_END_FUNCTION single token kullanıyor
- Pattern matching hack'leri kaldırıldı (eski TOKEN_END + peek + TOKEN_FUNCTION yerine)

### 4. Build System
- ✅ Compiler başarıyla build ediliyor
- ✅ Normalize layer Makefile'a entegre
- ✅ Duplicate source listesi temizlendi

---

## ❌ BLOCKER: Variable Declaration Bug

### Problem
Variable declaration'dan sonra statement parsing başarısız oluyor.

**Test Dosyası:** `test_only_var.mlp`
```mlp
function main() returns numeric
    numeric x = 5;
end_function
```

**Hata:**
```
test_only_var.mlp:3: error [Parser]: Expected 'function' keyword
    3 |     (end_function satırı)
      | ^
```

### Gözlemler
1. First pass başarılı (1 function registered)
2. Normalize çalışıyor ("end function" -> "end_function")
3. Second pass sırasında variable declaration parse ediliyor
4. Sonraki statement parse edilmeye çalışılırken statement_parse NULL dönüyor
5. functions_parser body loop'u bitiyor
6. Yeni function aramaya başlıyor, ama "end_function" token'ını görüyor
7. "Expected 'function' keyword" hatası veriyor

### Muhtemel Sebepler
1. Variable parser semicolon'dan sonra fazla token okuyor olabilir
2. statement_parse TOKEN_END_FUNCTION'ı görmeden önce NULL dönüyor
3. Parser temp_parser.current_token yönetimi yanlış olabilir

### Debug Adımları
1. `variable_parser.c` incelenmeli - semicolon sonrası token handling
2. `statement_parser.c` - variable declaration dönüşünde token kontrolü
3. `functions_parser.c` line 218-235 - body parsing loop'u debug edilmeli

---

## 📁 Değiştirilen Dosyalar

```
compiler/stage0/modules/functions/
  ├── functions_standalone.c  (+7 lines: normalize include + call)
  ├── Makefile               (+3 lines: normalize module)
  
compiler/stage0/modules/statement/
  ├── statement_parser.c     (+110 lines, -50 lines: PMPL tokens)
  
compiler/stage0/normalize/
  ├── normalize.c            (RF_YZ_1'de oluşturuldu)
  ├── normalize.h            (RF_YZ_1'de oluşturuldu)
  ├── syntax_rules.h         (RF_YZ_1'de oluşturuldu)
```

---

## 🧪 Test Durumu

### Başarılı Testler
- ✅ `compiler/stage0/normalize/test_normalize` - 26/26 passing
- ✅ `compiler/stage0/modules/lexer/test_lexer` - 28/28 passing
- ✅ `test_normalize_output` - "end if" -> "end_if" transformation
- ✅ Build system - compiler successfully compiles

### Başarısız Testler
- ❌ `test_only_var.mlp` - Variable declaration sonrası parse hatası
- ❌ `test_just_var.mlp` - Variable + return statement parse hatası
- ❌ `test_old_syntax.mlp` - Variable + if statement parse hatası
- ❌ `test_simple_pmpl.mlp` - Sadece print bile çalışmıyor (output yok)

---

## 🎯 Kalan İşler

### 1. CRITICAL: Variable Declaration Bug'ını Çöz
- [ ] `variable_parser.c` token handling'i kontrol et
- [ ] `statement_parser.c` variable return'ünde token state'i debug et
- [ ] `functions_parser.c` body loop'unu debug et
- [ ] Test: `test_only_var.mlp` başarılı compile olmalı

### 2. Integration Testing
- [ ] `test_old_syntax.mlp` (eski syntax: "end if", "end function")
- [ ] `test_pmpl_syntax.mlp` (yeni syntax: "end_if", "end_function")
- [ ] `examples/basics/test_lt_only.mlp` (gerçek örnek)
- [ ] Runtime test: Program çalıştırma ve output kontrolü

### 3. Completion
- [ ] RF_YZ_3.md raporu tamamla
- [ ] REFACTOR_STATUS.md'yi 100% complete yap
- [ ] Git commit: "RF_YZ_3: Parser PMPL single token refactor complete"

---

## 📚 Referanslar

- **TODO:** `TODO_REFACTOR_PMPL.md` - Complete refactor specification
- **Status:** `YZ_refactor/REFACTOR_STATUS.md` - Live progress (67% -> 100%)
- **Reports:** 
  - `YZ_refactor/RF_YZ_1.md` - Normalize layer (complete)
  - `YZ_refactor/RF_YZ_2.md` - Lexer tokens (complete)
  - `YZ_refactor/RF_YZ_3.md` - Parser (in progress - şu dosyayı güncelle)

---

## 🚀 Başlangıç Prompt

```
RF_YZ_3 parser refactor'unu tamamla. 

DURUM: Normalize layer ve lexer tokens hazır ve test edildi. Parser'da 
TOKEN_END_* single token handling eklendi ama variable declaration 
sonrası statement parsing problemi var.

BUG: test_only_var.mlp compile edilemiyor - "numeric x = 5;" sonrası 
"end_function" görünce "Expected 'function' keyword" hatası veriyor.

GÖREV:
1. variable_parser.c token handling'i debug et
2. statement_parser.c variable dönüş noktasını kontrol et  
3. functions_parser.c body loop'u düzelt
4. Integration testleri çalıştır
5. RF_YZ_3.md raporunu tamamla

DOSYALAR: YZ_refactor/RF_YZ_3_HANDOFF.md'de detaylı bilgi var.
```

---

## 💡 İpuçları

1. **Token Ownership:** Parser pattern'inde her token'ın kim tarafından free edileceği net olmalı
2. **current_token:** Parser.current_token NULL olmadığında statement_parse önce onu kullanır
3. **Lexer Position:** variable_parser semicolon sonrası lexer position'ı doğru bırakmalı
4. **Debug:** Printf debug'lar ekle - hangi token okunuyor, kim free ediyor

---

**Son Güncelleme:** 14 Aralık 2025 - YZ_74  
**Sonraki YZ:** Bu dosyayı oku ve bug'ı çöz! 🎯
