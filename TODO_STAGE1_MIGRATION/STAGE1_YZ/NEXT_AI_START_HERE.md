# 🚀 STAGE1_MIGRATION - START HERE!

**Son Session:** STAGE1_YZ_01 - Phase 1 Tamamlandı  
**Tarih:** 26 Aralık 2025  
**Agent:** GitHub Copilot (Claude Sonnet 4.5)  
**Status:** ✅ Phase 1 Complete → ⏳ Phase 2 Ready

---

## 🤖 YZ HIZLI BAŞLANGIÇ (İLK OKUYACAĞIN!)

**Yeni YZ geldiğinde yapılacaklar (5 dakika):**

1. **Bu dosyayı oku** → Mevcut durum nedir?
2. **TODO dosyalarını oku:**
   - [`TODO_KURALLARI.md`](../../TODO_KURALLARI.md) → Genel YZ kuralları
   - [`2-TODO_STAGE1_MIGRATION.md`](../../2-TODO_STAGE1_MIGRATION.md) → Detaylı görev listesi
3. **Son raporları oku:**
   - `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/STAGE1_YZ_0X_TAMAMLANDI.md` → Önceki YZ ne yaptı?
4. **Referans dokümanlara bak:**
   - `TODO_STAGE1_MIGRATION/docs/` → (Sadece ilgili bölüm!)

**Session bittiğinde yapılacaklar (15 dakika):**

1. **YZ raporu yaz** → `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/STAGE1_YZ_0X_TAMAMLANDI.md` (görev, değişiklikler, testler, sonuçlar)
2. **TODO dosyalarını güncelle:**
   - `2-TODO_STAGE1_MIGRATION.md` → Tamamlananı [x] işaretle, test sonuçlarını ekle
3. **Bu dosyayı güncelle** → Status, "Son Session" bilgisi
4. **Git commit & push** → Değişiklikleri kendi dalına commit et ve push et
5. **KULLANICIYA SOR** → "Görev tamamlandı. Sırada ne yapmak istersin?"

**🔧 Git Workflow:**
```bash
# 1. Yeni dal oluştur (görev adı + YZ numarası)
# Format: task-description_STAGE1_YZ_XX
git checkout -b task-description_STAGE1_YZ_XX

# 2. Değişiklikleri ekle
git add compiler/ tests/ docs/ TODO_STAGE1_MIGRATION/STAGE1_YZ/*.md 2-TODO_STAGE1_MIGRATION.md NEXT_AI_START_HERE.md

# 3. Commit et (detaylı mesajla)
git commit -m "STAGE1_YZ_XX: Task Description

✅ Completed Tasks:
- Task 1
- Task 2

📊 Test Results:
- Tests: X/X passing

Status: Complete"

# 4. Push et (merge YAPMA!)
git push origin task-description_STAGE1_YZ_XX
```

**⚠️ ÖNEMLİ:** Session bittiğinde mutlaka TODO.md + NEXT_AI_START_HERE.md + YZ raporu + GIT COMMIT yapılmalı!

---

## 🔍 İLK ÖNCE: MEVCUT DURUMU KONTROL ET!

**⚠️ GÖREVE BAŞLAMADAN ÖNCE MUTLAKA YAP:**

### Adım 1: Kod İncele

```bash
# Görevinle ilgili kodun zaten yapılıp yapılmadığını kontrol et
grep -rn "TODO_İLE_İLGİLİ_KEYWORD" compiler/stage1-mlp/mlp_compiler.c
```

### Adım 2: Mevcut Durumu Tespit Et

**Eğer ilgili kod zaten varsa:**
- ✅ "ZATEN YAPILMIŞ!" de
- 🧪 Test et, çalışıyor mu kontrol et
- 📝 Sadece dokümantasyon işlerini yap:
  - TODO güncelleme
  - Test sonuçları ekleme
  - Rapor yazma
  - Commit atma

**Eğer kod yoksa:**
- 🔨 O zaman implementasyon yap
- Her task sonunda test et
- Çalışmazsa düzelt (3 deneme hakkın var)

### Adım 3: Test Et

```bash
# Basit bir test yaz ve çalıştır
./build/bin/mlpc tests/test.mlp /tmp/test.asm
```

**Çalışıyorsa:** Sadece dokümantasyon yap, koda dokunma!  
**Çalışmıyorsa:** Implementasyon yapman gerekiyor.

---

## 📊 PROJE DURUMU

**STAGE1_MIGRATION:**
- **Phase 1:** ✅ TAMAMLANDI - STAGE1_YZ_01 (Analiz & Plan)
- **Phase 2:** ⏳ HAZIR - STAGE1_YZ_02 (Dosya Migration)
- **Phase 3:** ⏸️ Durduruldu (Phase 2 bitene kadar)

**✅ Phase 1 Sonuçları:**
- 18 .mlp dosyası tespit edildi
- 137 fonksiyon tanımı + 740 fonksiyon çağrısı dönüştürülecek
- Migration script hazır: `tools/migrate_stage1_to_pmlp.py`
- Ondalık sayı riski yok (0 kullanım)
- Test başarılı (dry-run on token_types.mlp)

**⚠️ YZ Directory Structure:**
- `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/` - Tamamlanan YZ raporları
  - ✅ STAGE1_YZ_01_TAMAMLANDI.md
- `TODO_STAGE1_MIGRATION/docs/` - Dokümantasyon
- `compiler/stage1-mlp/compiler_core/` - Migration hedef klasörü
- `tools/migrate_stage1_to_pmlp.py` - Migration script

---

## 🎯 PHASE 2: Dosya Migration (ŞİMDİ!)

**Görev:** STAGE1_YZ_02  
**Süre:** 1.5-2 saat  
**Durum:** ⏳ Hazır Başlamaya

**Ne yapılacak:**

### Task 2.1: Core Modüller (30 dakika)
- [ ] token_types.mlp - migration script çalıştır ve test et
- [ ] scope_manager.mlp - migration script çalıştır ve test et
- [ ] function_registry.mlp - migration script çalıştır ve test et
- [ ] ast/ast_nodes.mlp - migration script çalıştır ve test et

### Task 2.2: Lexer/Parser (45 dakika)
- [ ] lexer.mlp
- [ ] parser.mlp
- [ ] parser_advanced.mlp
- [ ] parser_functions.mlp
- [ ] parser_structures.mlp

### Task 2.3: Codegen (45 dakika)
- [ ] codegen_control.mlp
- [ ] codegen_functions.mlp
- [ ] codegen_structures.mlp
- [ ] codegen_async.mlp
- [ ] codegen_lambda.mlp
- [ ] codegen.mlp (en büyük, 809 satır)

### Task 2.4: Diğerleri (15 dakika)
- [ ] gc_integration.mlp
- [ ] import_handler.mlp
- [ ] mlp_compiler_main.mlp

**Başarı Kriterleri:**
- ✅ Her dosya migration script ile dönüştürüldü
- ✅ Her dosya ./build/bin/mlpc ile derleniyor
- ✅ Backup dosyaları (.backup_migration) oluşturuldu
- ✅ String literal'ler korundu
- ✅ Ondalık sayılar korundu

**Migration Komutları:**
```bash
# Tek dosya migration
python3 tools/migrate_stage1_to_pmlp.py compiler/stage1-mlp/compiler_core/token_types.mlp

# Syntax kontrolü
./build/bin/mlpc compiler/stage1-mlp/compiler_core/token_types.mlp /tmp/test.asm

# Hata varsa backup'tan dön ve manuel düzelt
cp compiler/stage1-mlp/compiler_core/token_types.mlp.backup_migration compiler/stage1-mlp/compiler_core/token_types.mlp
```

---

## ✅ PHASE 1 TAMAMLANDI (STAGE1_YZ_01)

**Tamamlanan Görevler:**
- [x] Task 1.1: Dosya tarama ve analiz
  - 18 dosya tespit edildi
  - 137 fonksiyon tanımı + 740 çağrı
  - Ondalık sayı riski yok
- [x] Task 1.2: Migration script hazırlama
  - Script: `tools/migrate_stage1_to_pmlp.py`
  - Dry-run test başarılı
  - Backup mekanizması var

**Test Sonuçları:**
```bash
python3 tools/migrate_stage1_to_pmlp.py compiler/stage1-mlp/compiler_core/token_types.mlp --dry-run
Sonuç: ✅ BAŞARILI - 6 satır tespit edildi
```

**Rapor:** `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/STAGE1_YZ_01_TAMAMLANDI.md`

---

## 🎤 PHASE 2 İÇİN: ONAY VER (ZORUNLU!)

**Yukarıdaki dökümanları okuduktan VE mevcut durumu kontrol ettikten sonra, göreve başlamadan ÖNCE şu formatla onay ver:**

```markdown
🤖 STAGE1_YZ_0X GÖREVE HAZIRIM

**Phase:** Phase X - [Kısa görev adı]
**Süre:** [Tahmini süre]
**Ana Dosya:** [Hangi dosyada çalışacaksın]
**Mevcut Durum:** [Kontrol sonucu: "Yeni implementasyon" veya "Zaten yapılmış, sadece dokümantasyon"]

---

## 💪 TAAHHÜTLER (SÖZ VERİYORUM):

- ✅ Önce mevcut durumu kontrol ettim
- ✅ Pseudo/stub/hack yazmayacağım - Her şey gerçek, çalışır kod
- ✅ Her task sonunda test yapacağım
- ✅ %100 çalışır kod üreteceğim
- ✅ Test başarısız olursa düzeltip tekrar deneyeceğim (3 deneme)
- ✅ Git workflow'a harfiyen uyacağım
- ✅ Merge yapmayacağım

**⚠️ GÖREV BİTİŞİNDE MUTLAKA YAPACAĞIM:**
- ✅ Ana TODO'da görevli olduğum bölümü [x] işaretleyeceğim
- ✅ Ana TODO'da test sonuçlarını ekleyeceğim
- ✅ NEXT_AI'ı güncelleyeceğim (Phase status + Son Session)
- ✅ Rapor yazacağım (STAGE1_YZ_0X_TAMAMLANDI.md)
- ✅ Git commit + push yapacağım

---

## 🚀 ONAY BEKLİYORUM

Üst Akıl, onay verirsen göreve başlıyorum.
```

**⚠️ ÖNEMLİ:** Bu onay formatını kullanmadan göreve başlama! Üst Akıl'ın onayını bekle.

---

## 📚 ÖNEMLİ DÖKÜMANLAR

**Mutlaka oku:**
1. [`TODO_KURALLARI.md`](../../TODO_KURALLARI.md) - Genel YZ kuralları
2. [`2-TODO_STAGE1_MIGRATION.md`](../../2-TODO_STAGE1_MIGRATION.md) - Detaylı task listesi
3. `TODO_STAGE1_MIGRATION/docs/` - İlgili dokümantasyon (sadece ilgili bölüm)

**Referans:**
- `compiler/stage1-mlp/mlp_compiler.c` - Ana kod
- `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/STAGE1_YZ_0X_TAMAMLANDI.md` - Önceki raporlar

---

## 🚦 BAŞLA!

**STAGE1_YZ_01:** Yukarıdaki dökümanları oku, **mevcut durumu kontrol et**, onay formatını kullan, sonra görevlere başla!

**Soru varsa:** Üst Akıl'a sor, tahmin etme!
