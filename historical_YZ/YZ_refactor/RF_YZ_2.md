# RF_YZ_2: Lexer Refactor - PMPL Keywords

**Başlangıç:** 14 Aralık 2025  
**Görev:** PMPL underscore keyword tokenization  
**Durum:** 🟡 IN PROGRESS

---

## 🎯 Görev Tanımı

Lexer'a PMPL underscore keyword'lerini tanıtmak.

**Amaç:** Normalize edilmiş PMPL input'u tek token'lara çevirmek.

**Örnek:**
```
INPUT:  "end_if"       → TOKEN: TOKEN_END_IF (tek token!)
INPUT:  "else_if"      → TOKEN: TOKEN_ELSE_IF (tek token!)
INPUT:  "exit_for"     → TOKEN: TOKEN_EXIT_FOR (tek token!)
```

---

## 📋 Yapılacaklar

### Faz 1: Token Enum'ları
- [ ] `lexer.h` - TOKEN_END_IF, TOKEN_END_WHILE, vb. ekle
- [ ] Tüm 22 PMPL keyword için token tanımı

### Faz 2: Keyword Recognition
- [ ] `lexer.c` - strcmp ile underscore keyword'leri tanı
- [ ] ESKİ iki-kelimelik pattern matching kodunu KALDIR

### Faz 3: Testing
- [ ] `test_lexer.c` - Unit testler oluştur
- [ ] Tüm PMPL keyword'lerin doğru tokenize edildiğini doğrula

---

## 🔧 İmplementasyon Detayları

### Yeni Token'lar (22 adet)

**Block Terminators:**
- TOKEN_END_IF
- TOKEN_END_WHILE
- TOKEN_END_FOR
- TOKEN_END_FUNCTION
- TOKEN_END_STRUCT
- TOKEN_END_SWITCH
- TOKEN_END_MATCH
- TOKEN_END_OPERATOR
- TOKEN_END_TRY

**Control Flow:**
- TOKEN_ELSE_IF

**Loop Control:**
- TOKEN_EXIT_IF
- TOKEN_EXIT_FOR
- TOKEN_EXIT_WHILE
- TOKEN_EXIT_FUNCTION
- TOKEN_EXIT_SWITCH
- TOKEN_CONTINUE_FOR
- TOKEN_CONTINUE_WHILE

**Debug Keywords:**
- TOKEN_DEBUG_GOTO
- TOKEN_DEBUG_PAUSE
- TOKEN_DEBUG_LABEL
- TOKEN_DEBUG_PRINT

**State Keywords:**
- TOKEN_SHARED_STATE

---

## 🧪 Test Planı

```c
// test_lexer.c taslağı

void test_block_terminators() {
    assert(tokenize("end_if") == TOKEN_END_IF);
    assert(tokenize("end_while") == TOKEN_END_WHILE);
    // ... 9 test
}

void test_control_flow() {
    assert(tokenize("else_if") == TOKEN_ELSE_IF);
}

void test_loop_control() {
    assert(tokenize("exit_for") == TOKEN_EXIT_FOR);
    assert(tokenize("continue_while") == TOKEN_CONTINUE_WHILE);
    // ... 7 test
}

void test_debug_keywords() {
    assert(tokenize("debug_goto") == TOKEN_DEBUG_GOTO);
    // ... 4 test
}

void test_state_keywords() {
    assert(tokenize("shared_state") == TOKEN_SHARED_STATE);
}
```

**Toplam:** ~25 test

---

## 💡 Karşılaşılan Sorunlar ve Çözümler

### Sorun 1: Include Path
**Problem:** test_lexer.c'de `#include "../lexer.h"` hata verdi  
**Çözüm:** `#include "lexer.h"` ve `-I.` flag'i kullanıldı

### Sorun 2: Token Enum Sıralaması
**Problem:** Yeni token'lar nereye eklenmeli?  
**Çözüm:** Mevcut TOKEN_EXIT'ten sonra, PMPL grubu olarak eklendi

---

## 📊 Code Metrics

**Dosyalar:**
- lexer.h: +44 satır (22 token enum)
- lexer.c: +45 satır (22 keyword check)
- test_lexer.c: ~150 satır (28 test)

**Toplam:** ~240 satır yeni kod

---

## ✅ Başarı Kriterleri

- [x] Tüm 22 token tanımı eklendi
- [x] Lexer keyword recognition güncellendi
- [x] Test suite 28/28 geçti
- [x] Mevcut lexer testleri bozulmadı (backward compatible)
- [x] PMPL keywords tek token olarak tanınıyor

---

## 🚀 Sonuç

Lexer artık PMPL underscore keyword'lerini tek token olarak tanıyor!

**Önce:**
```
"end if" → TOKEN_END + TOKEN_IF (iki token, parser hack gerekir)
```

**Sonra:**
```
"end_if" → TOKEN_END_IF (tek token, basit parser!)
```

**Bitiş:** 14 Aralık 2025  
**Test Sonucu:** ✅ PASS (28/28)  
**Commit:** [yakında]  
**Durum:** 🟢 COMPLETE
