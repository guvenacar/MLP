# MLP-ORIGINAL Migration Dizin Yapısı

Bu dizinler PMLP syntax migration projesi için oluşturulmuştur.

---

## 📁 Dizin Yapısı

```
mlp-original/
├── TODO_PMLP_MIGRATION.md              # Ana TODO (görev listesi)
├── TODO_MIGRATION_KURALLARI.md         # YZ kuralları
├── migration_YZ/                        # Görevli YZ dosyaları
│   ├── NEXT_AI_START_HERE.md           # Yeni YZ buradan başlar
│   ├── MigYZ_01_TAMAMLANDI.md          # YZ raporları (oluşturulacak)
│   ├── MigYZ_02_TAMAMLANDI.md
│   └── ...
├── migration_UA/                        # Üst Akıl dosyaları
│   ├── MigUA_00_sonraki_UA_buradan_basla.md
│   └── MigUA_00_raporu.md              # ÜA raporları (oluşturulacak)
└── docs/
    └── syntax_migration_guide.md        # Detaylı dönüşüm kılavuzu
```

---

## 🎯 Roller

### Üst Akıl (ÜA - Mastermind)
- Projeyi yönetir
- YZ'lere görev atar
- İlerlemeyi takip eder
- Merge kararı verir

**Başlangıç:** `migration_UA/MigUA_00_sonraki_UA_buradan_basla.md`

### Görevli YZ (İşçi AI)
- Verilen görevi yapar
- Syntax dönüşümü yapar
- Test eder, commit eder
- Rapor yazar

**Başlangıç:** `migration_YZ/NEXT_AI_START_HERE.md`

---

## 📋 Görev Akışı

```
1. YZ NEXT_AI_START_HERE.md okur
2. YZ TODO_MIGRATION_KURALLARI.md okur
3. YZ kendini tanıtır, onay ister
4. YZ görevi yapar (syntax migration)
5. YZ test eder
6. YZ MigYZ_XX_TAMAMLANDI.md yazar
7. YZ NEXT_AI_START_HERE.md günceller
8. YZ push eder (MERGE YAPMAZ)
9. ÜA review yapar
10. ÜA merge eder
11. Sonraki YZ'ye geçilir
```

---

## ⚠️ KURALLAR

### YZ için:
- ✅ Sadece syntax değiştir
- ✅ Her dosya ayrı commit
- ✅ Test et
- ❌ Runtime'a dokunma
- ❌ Merge yapma
- ❌ String/sayı içindeki virgüllere dokunma

### ÜA için:
- YZ'leri takip et
- Sorunları çöz
- Merge yap
- İlerleme raporla

---

## 📊 İlerleme Takibi

İlerleme şurada takip ediliyor:
- `TODO_PMLP_MIGRATION.md` - Ana TODO
- `migration_UA/MigUA_XX_raporu.md` - ÜA raporları
- `migration_YZ/MigYZ_XX_TAMAMLANDI.md` - YZ raporları

---

## 🚀 Hızlı Başlangıç

### Yeni YZ İçin:
```bash
cd /home/pardus/projeler/mlp-original
cat migration_YZ/NEXT_AI_START_HERE.md
cat TODO_MIGRATION_KURALLARI.md
cat docs/syntax_migration_guide.md
```

### Yeni ÜA İçin:
```bash
cd /home/pardus/projeler/mlp-original
cat migration_UA/MigUA_00_sonraki_UA_buradan_basla.md
cat TODO_PMLP_MIGRATION.md
```

---

## 📚 Önemli Belgeler

| Belge | Amaç | Okuyucu |
|-------|------|---------|
| TODO_PMLP_MIGRATION.md | Ana görev listesi | ÜA + YZ |
| TODO_MIGRATION_KURALLARI.md | YZ kuralları | YZ |
| docs/syntax_migration_guide.md | Dönüşüm kılavuzu | YZ |
| migration_YZ/NEXT_AI_START_HERE.md | YZ başlangıç | YZ |
| migration_UA/MigUA_XX_sonraki_UA_buradan_basla.md | ÜA başlangıç | ÜA |

---

**Versiyon:** 1.0  
**Tarih:** 25 Aralık 2025
