# 🚀 STAGE0_PMLP - START HERE!

**Son Session:** STAGE0_YZ_02 (Semicolon Separator)  
**Tarih:** 25 Aralık 2025  
**Agent:** GitHub Copilot (Claude Sonnet 4.5)  
**Status:** Phase 1 ✅, Phase 2 ⏳, Phase 3 ✅

---

## 🤖 YZ HIZLI BAŞLANGIÇ (İLK OKUYACAĞIN!)

**Yeni YZ geldiğinde yapılacaklar (5 dakika):**

1. **Bu dosyayı oku** → Mevcut durum nedir?
2. **TODO dosyalarını oku:**
   - [`TODO_KURALLARI.md`](TODO_KURALLARI.md) → Genel YZ kuralları
   - [`1-TODO_STAGE0_PMLP_SYNTAX.md`](1-TODO_STAGE0_PMLP_SYNTAX.md) → Detaylı görev listesi
3. **Son raporları oku:**
   - `TODO_STAGE0_PMLP/STAGE0_YZ/completed/STAGE0_YZ_0X_TAMAMLANDI.md` → Önceki YZ ne yaptı?
4. **Syntax referansına bak:**
   - [`TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md`](TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md) → (Sadece ilgili bölüm!)

**Session bittiğinde yapılacaklar (15 dakika):**

1. **YZ raporu yaz** → `TODO_STAGE0_PMLP/STAGE0_YZ/completed/STAGE0_YZ_0X_TAMAMLANDI.md` (görev, değişiklikler, testler, sonuçlar)
2. **TODO dosyalarını güncelle:**
   - `1-TODO_STAGE0_PMLP_SYNTAX.md` → Tamamlananı [x] işaretle, test sonuçlarını ekle
3. **Bu dosyayı güncelle** → Status, "Son Session" bilgisi
4. **Git commit & push** → Değişiklikleri kendi dalına commit et ve push et
5. **KULLANICIYA SOR** → "Görev tamamlandı. Sırada ne yapmak istersin?"

**🔧 Git Workflow:**
```bash
# 1. Yeni dal oluştur (görev adı + YZ numarası)
# Format: task-description_STAGE0_YZ_XX
git checkout -b task-description_STAGE0_YZ_XX

# 2. Değişiklikleri ekle
git add compiler/stage0-c/*.c TODO_STAGE0_PMLP/STAGE0_YZ/*.md 1-TODO_STAGE0_PMLP_SYNTAX.md NEXT_AI_START_HERE.md

# 3. Commit et (detaylı mesajla)
git commit -m "STAGE0_YZ_XX: Task Description

✅ Completed Tasks:
- Task 1
- Task 2

📊 Test Results:
- Tests: X/X passing

Status: Complete"

# 4. Push et (merge YAPMA!)
git push origin task-description_STAGE0_YZ_XX
```

**⚠️ ÖNEMLİ:** Session bittiğinde mutlaka TODO.md + NEXT_AI_START_HERE.md + YZ raporu + GIT COMMIT yapılmalı!

---

## 📊 PROJE DURUMU

**Stage0-C Compiler - PMLP Syntax Desteği:**
- **Phase 1:** ✅ TAMAMLANDI (Exit/Continue Tokens) - STAGE0_YZ_01
- **Phase 2:** ✅ TAMAMLANDI (Semicolon Separator) - STAGE0_YZ_02
- **Phase 2:** ⏳ ŞİMDİ! (Semicolon Separator) - STAGE0_YZ_02
- **Phase 3:** ✅ TAMAMLANDI (Migration & Testing) - STAGE0_YZ_03

**⚠️ YZ Directory Structure:**
- `TODO_STAGE0_PMLP/STAGE0_YZ/completed/` - Tamamlanan YZ raporları
- `TODO_STAGE0_PMLP/docs/` - Syntax dokümantasyonu
- `compiler/stage0-c/` - Stage0-C compiler kodu (mlp_compiler.c)

---

## 🎯 PHASE 3: MIGRATION & TESTING (ŞİMDİ!)

**Görev:** STAGE0_YZ_03  
**Süre:** 1-2 saat  
**Durum:** ⏳ Başlamadı

**Ne yapılacak:**

### Task 3.1: Migration Script (30 dakika)
- `tools/migrate_to_pmlp.py` oluştur
- Eski syntax → Yeni PMLP syntax dönüşümü
- Virgül → Noktalı virgül dönüşümü otomatik
- Dry-run ve backup özellikleri

### Task 3.2: Test Suite (45 dakika)
- `tests/pmlp_syntax/` altında test dosyaları
- Exit/continue token testleri (10 test)
- Semicolon separator testleri (10 test)
- Edge case testleri (5 test)
- Tüm testlerin geçmesi gerekli

### Task 3.3: Final Validation (15 dakika)
- Tüm eski .mlp dosyalarını migrate et
- Hepsinin derlendiğini doğrula
- Backward compatibility testi
- Dokümantasyon güncellemesi

**Başarı Kriterleri:**
- ✅ Migration script çalışıyor ve test edilmiş
- ✅ 25/25 test geçiyor
- ✅ Tüm example dosyaları migrate edilmiş ve çalışıyor
- ✅ Backward compatibility korunmuş
- ✅ Dokümantasyon güncellenmiş

---

## 📁 DOSYA ORGANIZASYONU KURALI - ÖNEMLİ!

**Test Dosyaları:**
- ❌ **ANA DİZİNE TEST DOSYASI BIRAKMA!** (test.mlp, test_*.mlp vb.)
- ✅ Tüm test dosyaları `tests/` klasörüne konulmalı
- ✅ PMLP testleri: `tests/pmlp_syntax/`

**Geçici Dosyalar:**
- ✅ `.s`, `.o`, binary dosyalar → `build/temp/` klasörüne
- ✅ Çalışma notları → `build/temp/notes_*.md`

**Her YZ Session Sonunda:** Ana dizini temizle, test dosyalarını `tests/` altına taşı!

---

## 💪 TAAHHÜTLER (HER YZ OKUYACAK!)

- ✅ Pseudo/stub/hack yazmayacağım - Her şey gerçek, çalışır kod
- ✅ Her task sonunda test yapacağım
- ✅ %100 çalışır kod üreteceğim
- ✅ Git workflow'a uyacağım
- ✅ Test başarısız olursa düzeltip tekrar deneyeceğim (3 deneme hakkı)
- ✅ Merge yapmayacağım - sadece kendi dalıma push

**⚠️ GÖREV BİTİŞİNDE MUTLAKA YAPACAĞIM:**
- ✅ Ana TODO'da benim phase/task'larımı [x] işaretleyeceğim
- ✅ Ana TODO'da test sonuçlarını ekleyeceğim
- ✅ NEXT_AI'ı güncelleyeceğim (Phase status + Son Session bilgisi)
- ✅ Rapor yazacağım (TODO_STAGE0_PMLP/STAGE0_YZ/completed/STAGE0_YZ_0X_TAMAMLANDI.md)
- ✅ Git commit + push yapacağım

---

## 📚 ÖNEMLİ DÖKÜMANLAR

**Mutlaka oku:**
1. [`TODO_KURALLARI.md`](TODO_KURALLARI.md) - Genel YZ kuralları
2. [`1-TODO_STAGE0_PMLP_SYNTAX.md`](1-TODO_STAGE0_PMLP_SYNTAX.md) - Detaylı task listesi
3. [`TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md`](TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md) - PMLP syntax (sadece ilgili bölüm)

**Referans:**
- `compiler/stage0-c/mlp_compiler.c` - Stage0-C compiler (10,303 satır)
- `TODO_STAGE0_PMLP/STAGE0_YZ/completed/STAGE0_YZ_01_TAMAMLANDI.md` - Phase 1 raporu
- `TODO_STAGE0_PMLP/STAGE0_YZ/completed/STAGE0_YZ_02_TAMAMLANDI.md` - Phase 2 raporu

---

## 🚦 BAŞLA!

**STAGE0_YZ_03:** Yukarıdaki dökümanları oku, sonra Phase 3 görevlerine başla!

**Soru varsa:** Üst Akıl'a sor, tahmin etme!
