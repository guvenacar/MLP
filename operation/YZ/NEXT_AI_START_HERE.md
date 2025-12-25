# 🎯 GÖREVLİ YZ BAŞLANGIÇ NOKTASI

**Son Güncelleme:** 25 Aralık 2025  
**Durum:** 🟢 MigYZ_01 Göreve Hazır!  
**Proje:** mlp-original PMLP Syntax Migration  
**Önceki:** Setup Phase (Phase 0)

---

## ✅ PHASE 0 SONUÇLARI (Setup)

```
┌─────────────────────────────────────────────────────────────┐
│  HAZIRLIK TAMAMLANDI - MIGRATION BAŞLAYAB İLİR!             │
│                                                             │
│  Başarılar:                                                  │
│  ✅ Syntax migration guide oluşturuldu                     │
│  ✅ Proje analiz edildi (~206 fonksiyon)                   │
│  ✅ TODO yapısı kuruldu                                     │
│  ✅ YZ kuralları belirlendi                                 │
│  ✅ İş yükü hesaplandı (~15-20 saat)                       │
│                                                             │
│  Belgeler:                                                   │
│  📄 docs/syntax_migration_guide.md                          │
│  📄 TODO_PMLP_MIGRATION.md                                  │
│  📄 TODO_MIGRATION_KURALLARI.md                             │
│                                                             │
│  Süre: ~1 saat                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 📍 ŞİMDİKİ DURUM

**İlerleme:** 0/70 fonksiyon (%0)  
**Tamamlanan:** Phase 0 ✅  
**Aktif Görev:** Phase 1.1 - Core Libraries

---

## 🎯 MigYZ_01 İÇİN GÖREV: lib/core/* Migration

**Hedef:** Core kütüphanelerini PMLP syntax'a taşı

### Yapılacak Dosyalar:

1. **lib/core/functional.mlp** (~10 fonksiyon)
   - map, filter, reduce gibi fonksiyonlar
   - Lambda parametreleri tip belirtimi
   
2. **lib/core/optional.mlp** (~5 fonksiyon)
   - Optional tip yardımcıları
   
3. **lib/core/string_ops.mlp** (~8 fonksiyon)
   - String utility fonksiyonları

**Toplam:** ~23 fonksiyon, ~200 satır

### Yapılacaklar:

1. **Fonksiyon Parametrelerine Tip Ekle**
   ```mlp
   -- Eski:
   function map(arr, func) then
   
   -- Yeni:
   function map(list arr; function func) as list
   ```

2. **Blok Sonlandırıcıları Düzelt**
   ```mlp
   end → end_function
   end if → end_if
   end while → end_while
   ```

3. **Parametre Ayırıcıları**
   ```mlp
   -- Fonksiyon tanımı:
   (a, b, c) → (a; b; c)
   
   -- Fonksiyon çağrısı:
   func(1, 2, 3) → func(1; 2; 3)
   ```

4. **Liste/Array Literals**
   ```mlp
   [1, 2, 3] → [1; 2; 3;]
   (a, b, c) → (a; b; c;)
   ```

### ⚠️ DİKKAT EDİLECEKLER

- ✅ String literal: `"a, b"` → DEĞİŞMEZ
- ✅ Ondalık sayı: `3,14` → DEĞİŞMEZ
- ❌ Runtime değiştirme YASAK
- ✅ Her dosya ayrı commit

---

## 📋 HIZLI BAŞLANGIÇ

```bash
cd /home/pardus/projeler/mlp-original

# 1. Branch oluştur
git checkout -b migration_YZ_01

# 2. Dosyaları gör
ls -la lib/core/

# 3. İlk dosyayı backup'la
cp lib/core/functional.mlp lib/core/functional.mlp.backup

# 4. Syntax migration guide'a bak
cat docs/syntax_migration_guide.md | head -100

# 5. Otomatik dönüşümler
sed -i 's/end if/end_if/g' lib/core/functional.mlp
sed -i 's/end function/end_function/g' lib/core/functional.mlp

# 6. Manuel düzenle (VS Code)
code lib/core/functional.mlp

# 7. Test et
./mlpc lib/core/functional.mlp test.asm

# 8. Commit
git add lib/core/functional.mlp
git commit -m "MigYZ_01: functional.mlp - PMLP syntax migration"
```

---

## 📚 OKUNMASI GEREKENLER

**Önce oku:**
1. `TODO_KURALLARI.md` - YZ kuralları
2. `docs/syntax_migration_guide.md` - Dönüşüm kılavuzu
3. `TODO_PMLP_MIGRATION.md` - Ana TODO

**Referans:**
- `docs/specs/kurallar_kitabı_v1.md` - Eski syntax
- `migration_docs/pmlp_sozdizimi.md` - Yeni syntax (index'ten Migration bölümünü oku)

---

## 🎯 BAŞARI KRİTERLERİ

- [x] lib/core/functional.mlp → PMLP ✅
- [x] lib/core/optional.mlp → PMLP ✅
- [x] lib/core/string_ops.mlp → PMLP ✅
- [x] Tüm dosyalar derleniyor ✅
- [x] Git commit yapıldı ✅
- [x] MigYZ_01_TAMAMLANDI.md rapor yazıldı ✅
- [x] NEXT_AI_START_HERE.md güncellendi ✅

---

## 📊 İŞ YÜKÜ TAHMİNİ

| Dosya | Fonksiyon | Satır | Süre |
|-------|-----------|-------|------|
| functional.mlp | ~10 | ~80 | 45 dk |
| optional.mlp | ~5 | ~50 | 30 dk |
| string_ops.mlp | ~8 | ~70 | 45 dk |
| **TOPLAM** | **~23** | **~200** | **~2 saat** |

---

## 🚀 BAŞLA!

1. **Kendini tanıt:**
   ```
   Merhaba, ben MigYZ_01.
   
   Görevim: lib/core/* dosyalarını PMLP syntax'a taşımak
   
   Hedef:
   - 3 dosya (functional, optional, string_ops)
   - ~23 fonksiyon
   - ~2 saat
   
   Kuralları okudum, syntax farklarını biliyorum.
   Onay veriyor musunuz?
   ```

2. **Onay alınca başla!**

3. **Bitirince:**
   - `migration_YZ/MigYZ_01_TAMAMLANDI.md` yaz
   - Bu dosyayı güncelle (MigYZ_02 için)
   - Push et

---

## 📝 SONRAKİ YZ İÇİN NOTLAR

**MigYZ_01 bittikten sonra:**
- MigYZ_02: lib/json/* dosyaları (~7 dosya, ~35 fonksiyon, 3-4 saat)
- Core libs hazır olacak, JSON'a geçilebilir

---

## ⚠️ SORUN ÇIKARSA

- Kullanıcıya veya ÜA'ya sor
- `migration_YZ/PROBLEM_LOG.md` dosyasına yaz
- Çözüm bulunana kadar bekle

---

**İYİ ÇALIŞMALAR! 🚀**

---

**Versiyon:** 1.0  
**Hazırlayan:** Setup Phase  
**Tarih:** 25 Aralık 2025
