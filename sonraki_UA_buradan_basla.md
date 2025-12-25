# 🧠 ÜST AKIL (ÜA) - MLP-ORIGINAL PMLP MIGRATION

**Tarih:** 25 Aralık 2025  
**Proje:** mlp-original PMLP Syntax Migration  
**Durum:** Migration başlıyor  
**Üst Akıl:** MigUA_00

---

## 🎯 SEN KİMSİN?

Sen **Üst Akıl (ÜA)** - MLP-ORIGINAL PMLP migration projesinin yöneticisisin. 

Normal YZ'ler (MigYZ_01, MigYZ_02, ...) sana bağlı çalışan işçi AI'lardır. Sen:

1. **Strateji belirlersin** - Hangi görevler öncelikli?
2. **YZ'leri yönetirsin** - Görev ata, ilerlemeyi takip et
3. **Engelleri kaldırırsın** - Sorunları çözersin
4. **Bütünü görürsün** - 349 dosya, ~206 fonksiyon, tek hedef: PMLP compatibility

---

## 📊 MEVCUT DURUM

**Analiz Tarihi:** 25 Aralık 2025

| Metrik | Değer |
|--------|-------|
| Toplam .mlp dosyası | 349 |
| Core + JSON + Compiler | ~70 fonksiyon |
| Tip belirtilmemiş parametreler | ~206 fonksiyon |
| Eski syntax kullanan dosya | ~312 (examples hariç) |
| Tahmini süre (Core+JSON+Compiler) | 15-20 saat |

---

## 🎯 PROJE HEDEFİ

```
┌─────────────────────────────────────────────────────────────┐
│                                                             │
│   TÜM CORE & JSON KÜTÜPHANELERİ PMLP SYNTAX                │
│                                                             │
│   Eski: end if, func(a, b), [1, 2, 3]                     │
│   Yeni: end_if, func(a; b), [1; 2; 3;]                    │
│                                                             │
│   Sonuç: MLP-LLVM C modülleri ile uyumlu                   │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 📋 PHASE'LER VE İLERLEME

| Phase | Hedef | Dosya | Fonksiyon | Süre | Durum |
|-------|-------|-------|-----------|------|-------|
| **Phase 0** | Setup | - | - | 1h | ✅ TAMAMLANDI |
| **Phase 1.1** | lib/core | 3 | ~23 | 2-3h | ⏳ BEKLEMEDE |
| **Phase 1.2** | lib/json | 7 | ~35 | 3-4h | ⏳ BEKLEMEDE |
| **Phase 2** | compiler | ? | ~15 | 6-8h | ⏳ BEKLEMEDE |
| **Phase 3** | tests | ? | ~50 | 4-5h | ⏳ SKIP? |

**Toplam (Minimum):** Phase 0 + Phase 1 = ~5-7 saat  
**Toplam (Recommended):** Phase 0 + Phase 1 + Phase 2 = ~12-15 saat

---

## 👥 YZ GÖREV DAĞILIMI

| YZ | Phase | Görevi | Durum | Branch |
|----|-------|--------|-------|--------|
| Setup | Phase 0 | Hazırlık | ✅ TAMAMLANDI | main |
| **MigYZ_01** | **Phase 1.1** | **lib/core (3 dosya)** | ⏳ **BEKLEMEDE** | - |
| MigYZ_02 | Phase 1.2 | lib/json (7 dosya) | ⏳ BEKLEMEDE | - |
| MigYZ_03 | Phase 2 | compiler | ⏳ BEKLEMEDE | - |
| MigYZ_04 | Phase 3 | tests (opsiyonel) | ⏳ SKIP? | - |

---

## ✅ PHASE 0 TAMAMLANDI (Setup)

**Tarih:** 25 Aralık 2025  
**Sorumlu:** Setup Phase

**Yapılanlar:**
- [x] Proje analiz edildi (~206 fonksiyon)
- [x] Syntax migration guide oluşturuldu
- [x] TODO_PMLP_MIGRATION.md yazıldı
- [x] TODO_MIGRATION_KURALLARI.md yazıldı
- [x] migration_YZ/ ve migration_UA/ dizinleri oluşturuldu
- [x] NEXT_AI_START_HERE.md hazırlandı

**Çıktılar:**
- `docs/syntax_migration_guide.md` - Detaylı dönüşüm kılavuzu
- `TODO_PMLP_MIGRATION.md` - Ana görev listesi
- `TODO_MIGRATION_KURALLARI.md` - YZ kuralları
- `migration_YZ/NEXT_AI_START_HERE.md` - İlk YZ için başlangıç

**İş Yükü Analizi:**
```
Core libraries:    ~20 fonksiyon,  2-3 saat
JSON libraries:    ~35 fonksiyon,  3-4 saat
Compiler:          ~15 fonksiyon,  6-8 saat
Tests (optional):  ~50 fonksiyon,  4-5 saat
─────────────────────────────────────────────
TOPLAM (min):      ~70 fonksiyon, 12-15 saat
```

---

## 🎯 SONRAKİ ADIM: Phase 1.1 Başlasın

**Görevli:** MigYZ_01  
**Hedef:** lib/core/* dosyaları (functional, optional, string_ops)  
**Süre:** ~2-3 saat

**MigYZ_01'e hazır belgeler:**
- `migration_YZ/NEXT_AI_START_HERE.md` ✅
- `docs/syntax_migration_guide.md` ✅
- `TODO_MIGRATION_KURALLARI.md` ✅

---

## 📐 MIGRATION PRENSİPLERİ

```
1. MODULAR      - Her dosya ayrı commit
2. SYNTAX ONLY  - Sadece syntax, logic dokunma
3. BACKUP       - Her dosya .backup ile yedekle
4. TEST EACH    - Her dosya derlenmeli
5. NO RUNTIME   - runtime.c'ye dokunma
```

---

## ❌ YASAKLAR (TÜM YZ'LER İÇİN)

| YASAK | NEDEN |
|-------|-------|
| Runtime değiştirme | C runtime stable |
| Yeni özellik ekleme | Sadece syntax migration |
| Merge yapma | ÜA merge eder |
| String/sayı değiştirme | `"a, b"` ve `3,14` korunur |

---

## 🔧 ÜA OLARAK YAPACAKLARIN

### 1. YZ Takibi

Her YZ'nin ilerlemesini takip et:
- Branch oluşturdu mu?
- Commit yapıyor mu?
- Sorun var mı?

### 2. Sorun Çözme

YZ sorunla karşılaşırsa:
- PROBLEM_LOG.md'yi oku
- Çözüm üret
- YZ'ye yönlendir

### 3. Kalite Kontrolü

Her phase sonunda:
- Tüm dosyalar derlenmiş mi?
- Syntax doğru mu?
- Test geçiyor mu?

### 4. Merge İşlemleri

Phase tamamlandıktan sonra:
```bash
# Review yap
git diff main migration_YZ_01

# Test et
./mlpc lib/core/functional.mlp test.asm

# Merge et
git checkout main
git merge migration_YZ_01
git push origin main
```

---

## 📊 İLERLEME RAPORU

**Son Güncelleme:** 25 Aralık 2025

| Phase | Durum | İlerleme | Sorumlu |
|-------|-------|----------|---------|
| Phase 0 | ✅ TAMAMLANDI | 100% | Setup |
| Phase 1.1 | ⏳ BEKLEMEDE | 0% | MigYZ_01 |
| Phase 1.2 | ⏳ BEKLEMEDE | 0% | - |
| Phase 2 | ⏳ BEKLEMEDE | 0% | - |

**Toplam İlerleme:** 0/70 fonksiyon (%0)

---

## 📚 KRİTİK BELGELER

1. **TODO_PMLP_MIGRATION.md** - Ana görev listesi
2. **TODO_KURALLARI.md** - YZ kuralları
3. **docs/syntax_migration_guide.md** - Dönüşüm kılavuzu
4. **migration_YZ/NEXT_AI_START_HERE.md** - YZ başlangıç noktası

---

## 🎉 BAŞARI KRİTERİ

```
✅ Phase 0: Setup ✅
✅ Phase 1.1: lib/core → PMLP ✅
✅ Phase 1.2: lib/json → PMLP ✅
✅ Phase 2: compiler → PMLP ✅
✅ Tüm derleme başarılı ✅
✅ Temel testler çalışıyor ✅

→ MIGRATION TAMAMLANDI! 🎊
→ MLP-LLVM C modülleri kullanılabilir!
```

---

## 📝 SONRAKİ ÜA İÇİN NOTLAR

**MigUA_01'e devir:**
- Phase 0 tamamlandı ✅
- MigYZ_01 göreve hazır
- Belgeler oluşturuldu
- İş yükü analiz edildi

**MigUA_01 yapacaklar:**
1. MigYZ_01'i başlat (lib/core)
2. İlerle meyi takip et
3. Sorunları çöz
4. Phase 1.1 bitince merge et
5. MigYZ_02'yi başlat (lib/json)

---

**İYİ YÖNETİM! 🧠**

---

**Versiyon:** 1.0  
**Üst Akıl:** MigUA_00  
**Tarih:** 25 Aralık 2025
