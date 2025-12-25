# 🚀 STAGE1_MIGRATION - START HERE!

**Son Session:** STAGE1_YZ_01 - Phase 1 Tamamlandı  
**Tarih:** 26 Aralık 2025  
**Agent:** GitHub Copilot (Claude Sonnet 4.5)  
**Status:** ✅ Phase 1 Complete → ⏳ Phase 2 Ready

---

## 🤖 YZ HIZLI BAŞLANGIÇ (İLK OKUYACAĞIN!)

**Yeni YZ geldiğinde kullanıcıya selam vermeden önce yapılacaklar (5 dakika):**

1. **Bu dosyayı oku** → Mevcut durum nedir?
2. **TODO dosyalarını oku:**
   - [`TODO_KURALLARI.md`](TODO_KURALLARI.md) → Genel YZ kuralları
   - [`2-TODO_STAGE1_MIGRATION.md`](2-TODO_STAGE1_MIGRATION.md) → Detaylı görev listesi
3. **Son raporları oku:**
   - `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/` → Önceki YZ ne yaptı?
4. **Referans dokümanlara bak:**
   - [`docs/specs/pmlp_kesin_sozdizimi.md`](docs/specs/pmlp_kesin_sozdizimi.md) → PMLP syntax referansı

**Session bittiğinde yapılacaklar (15 dakika):**

1. **YZ raporu yaz** → `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/STAGE1_YZ_0X_TAMAMLANDI.md`
2. **TODO dosyalarını güncelle:**
   - `2-TODO_STAGE1_MIGRATION.md` → Tamamlananı [x] işaretle
3. **Bu dosyayı güncelle** → Status, "Son Session" bilgisi
4. **Git commit & push**
5. **KULLANICIYA SOR** → "Görev tamamlandı. Sırada ne yapmak istersin?"

---

## 🎯 AKTİF TODO: STAGE1_MIGRATION

**Amaç:** Stage1-MLP compiler modüllerini PMLP syntax'a migrate etmek.

**SORUN:** 
```bash
# Mevcut hata:
./build/bin/mlpc compiler/stage1-mlp/compiler_core/codegen.mlp /tmp/test.asm
# HATA: "," beklenmeyen token - çünkü virgül artık ondalık ayırıcı!
```

**ÇÖZÜM:** Tüm `func(a, b)` → `func(a; b)` dönüşümü yapılmalı.

---

## 📊 PROJE DURUMU

### TODO Sırası:
| # | TODO | Durum |
|---|------|-------|
| 1 | TODO_STAGE0_PMLP_SYNTAX | ✅ TAMAMLANDI |
| 2 | **TODO_STAGE1_MIGRATION** | ⏳ **AKTİF** |
| 3 | TODO_SELFHOST_VERIFY | ⏸️ Bekliyor |
| 4 | TODO_RUNTIME_INTEGRATION | ⏸️ Bekliyor |
| 5 | TODO_GCC_TO_LLVM_BACKEND | ⏸️ Bekliyor |

### Stage1 Migration Phase'leri:
- **Phase 1:** ⏳ Analiz ve Planlama (30 dk)
- **Phase 2:** ⏳ Dosya Migration (1.5-2 saat)
- **Phase 3:** ⏳ Doğrulama (30 dk)

---

## 📁 MİGRATE EDİLECEK DOSYALAR

```
compiler/stage1-mlp/compiler_core/
├── codegen.mlp               (809 satır) ⬅️ En büyük
├── codegen_functions.mlp     
├── codegen_control.mlp       
├── codegen_structures.mlp    
├── codegen_async.mlp         
├── codegen_lambda.mlp        
├── lexer.mlp                 
├── parser.mlp                
├── parser_advanced.mlp       
├── parser_functions.mlp      
├── parser_structures.mlp     
├── scope_manager.mlp         
├── function_registry.mlp     
├── gc_integration.mlp        
├── import_handler.mlp        
├── token_types.mlp           
├── mlp_compiler_main.mlp     
└── ast/ast_nodes.mlp
```

**Toplam:** ~17 dosya

---

## 🔄 DEĞİŞTİRİLECEK PATTERN'LER

| Eski Syntax | Yeni PMLP Syntax | Açıklama |
|-------------|------------------|----------|
| `func(a, b)` | `func(a; b)` | Fonksiyon çağrısı |
| `function f(a, b)` | `function f(a; b)` | Fonksiyon tanımı |
| `[1, 2, 3]` | `[1; 2; 3;]` | Liste/array literal |

**⚠️ DİKKAT - DOKUNMA:**
- `3,14` → Ondalık sayı, değişmemeli!
- String içindeki virgüller → Korunmalı!

---

## 🔍 İLK ÖNCE: MEVCUT DURUMU KONTROL ET!

**GÖREVE BAŞLAMADAN ÖNCE:**

```bash
# Kaç dosyada virgül var?
grep -rn "function.*(.*, " compiler/stage1-mlp/compiler_core/ | wc -l

# Örnek görmek için:
grep -n "function.*(.*, " compiler/stage1-mlp/compiler_core/codegen.mlp | head -10
```

---

## 🎯 PHASE 1: ANALİZ VE PLANLAMA (ŞİMDİ!)

**Görev:** STAGE1_YZ_01  
**Süre:** 30 dakika  
**Durum:** ⏳ Başlamadı

### Task 1.1: Dosyaları tara
- [ ] `grep` ile tüm virgüllü fonksiyon tanımlarını bul
- [ ] `grep` ile tüm virgüllü fonksiyon çağrılarını bul
- [ ] Array literal kullanımlarını bul

### Task 1.2: Ondalık sayı tespiti
- [ ] `\d,\d` pattern'i ile ondalık sayıları tespit et
- [ ] Bu sayıların korunacağından emin ol

### Task 1.3: Migration planı
- [ ] Hangi dosya önce migrate edilecek (bağımlılık sırası)
- [ ] Script mi manuel mi karar ver

**Başarı Kriterleri:**
- ✅ Tüm değiştirilecek satırlar listelenmiş
- ✅ Ondalık sayılar tespit edilmiş
- ✅ Migration sırası belirlenmiş

---

## 💪 TAAHHÜTLER

- ✅ Önce mevcut durumu kontrol edeceğim
- ✅ Ondalık sayılara dokunmayacağım
- ✅ String içindeki virgülleri koruyacağım
- ✅ Her dosya sonrası `./build/bin/mlpc` ile test edeceğim
- ✅ Git commit yapacağım

---

## 📚 REFERANSLAR

| Belge | Amaç |
|-------|------|
| [`2-TODO_STAGE1_MIGRATION.md`](2-TODO_STAGE1_MIGRATION.md) | Detaylı görev listesi |
| [`1-TODO_STAGE0_PMLP_SYNTAX.md`](1-TODO_STAGE0_PMLP_SYNTAX.md) | PMLP syntax değişiklikleri |
| [`docs/specs/pmlp_kesin_sozdizimi.md`](docs/specs/pmlp_kesin_sozdizimi.md) | PMLP syntax referansı |
| [`tests/pmlp/`](tests/pmlp/) | PMLP test suite (13 test) |

---

## 🚦 BAŞLA!

1. Yukarıdaki belgeleri oku
2. `grep` ile mevcut durumu kontrol et
3. Onay ver ve göreve başla!

**Soru varsa:** Üst Akıl'a sor!
