# YZ_refactor Dizini

**Amaç:** PMPL Architecture Refactor raporları

---

## 📁 Dizin Yapısı

```
YZ_refactor/
├── README.md                  # Bu dosya
├── REFACTOR_NEXT_AI.md        # Yeni refactor YZ için başlangıç
├── REFACTOR_STATUS.md         # Genel ilerleme durumu
├── RF_YZ_1.md                 # Normalize Layer raporu
├── RF_YZ_2.md                 # Lexer Refactor raporu
└── RF_YZ_3.md                 # Parser Simplification raporu
```

---

## 🎯 Refactor YZ Sekansı

Bu dizindeki YZ'ler özel **refactor sekansı** kullanır:

- **Normal YZ:** YZ_01, YZ_02, ..., YZ_75 (genel görevler)
- **Refactor YZ:** RF_YZ_1, RF_YZ_2, RF_YZ_3 (sadece PMPL refactor)

**Fark:**
- Normal YZ'ler `kurallar_kitabı.md` okur
- Refactor YZ'ler `TODO_REFACTOR_PMPL.md` okur
- Refactor YZ'ler sadece PMPL görür, user syntax görmez

---

## 📋 Görev Dağılımı

| YZ | Görev | Dosyalar | Süre |
|----|-------|----------|------|
| RF_YZ_1 | Normalize Layer | `compiler/stage0/normalize/*` | 2-3h |
| RF_YZ_2 | Lexer Refactor | `compiler/stage0/modules/lexer/*` | 2-3h |
| RF_YZ_3 | Parser Simplification | `statement_parser.c`, `functions_standalone.c` | 3-4h |

---

## 🚀 Yeni Refactor YZ Başlatma

```bash
# 1. Status kontrol et
cat YZ_refactor/REFACTOR_STATUS.md

# 2. Başlangıç dokümanını oku
cat YZ_refactor/REFACTOR_NEXT_AI.md

# 3. Görev detayını oku
cat TODO_REFACTOR_PMPL.md

# 4. Rapor oluştur
touch YZ_refactor/RF_YZ_X.md

# 5. BAŞLA!
```

---

## 📊 İlerleme Takibi

Tüm refactor YZ'leri `REFACTOR_STATUS.md` dosyasını güncellemeli:

```markdown
RF_YZ_1: 🔴 NOT STARTED → 🟡 IN PROGRESS → 🟢 COMPLETE
RF_YZ_2: 🔴 NOT STARTED → 🟡 IN PROGRESS → 🟢 COMPLETE
RF_YZ_3: 🔴 NOT STARTED → 🟡 IN PROGRESS → 🟢 COMPLETE
```

---

## ⚠️ Önemli Kurallar

1. **Sıralı çalışma:** RF_YZ_1 → RF_YZ_2 → RF_YZ_3
2. **Bağımlılık:** Bir önceki tamamlanmadan sonraki başlamaz
3. **Test-driven:** Her YZ kendi testlerini geçmeli
4. **Commit:** Her YZ tamamlanınca commit yapılmalı

---

## 🎓 Refactor Motivasyonu

**Sorun:**
- Lexer iki kelimelik keyword'leri iki token üretiyor
- Parser pattern matching hack'leri ile çalışıyor
- PMPL spec uygulanmamış
- Self-hosting imkansız

**Çözüm:**
- Normalize layer: User syntax → PMPL
- Lexer: Underscore keyword'leri tek token
- Parser: Basit switch-case, hack yok

**Sonuç:**
- ✅ PMPL spec'e uygun mimari
- ✅ Self-hosting mümkün
- ✅ Multi-syntax support hazır

---

## 📝 Rapor Şablonu

Her RF_YZ_X.md dosyası şu formatı takip etmeli:

```markdown
# RF_YZ_X: [Görev Adı]

**Başlangıç:** [Tarih/Saat]  
**Görev:** [Açıklama]  
**Durum:** [NOT STARTED / IN PROGRESS / COMPLETE]

## Yapılanlar
- [ ] Görev 1
- [ ] Görev 2

## Testler
- [ ] Test 1
- [ ] Test 2

## Karşılaşılan Sorunlar
[Varsa açıkla]

## Çözümler
[Nasıl çözüldü]

## Sonuç
[Özet]

**Bitiş:** [Tarih/Saat]  
**Test Sonucu:** [PASS / FAIL]  
**Commit:** [git hash]
```

---

## 🏁 Refactor Tamamlanma

Tüm RF_YZ'ler tamamlandığında:

1. `REFACTOR_STATUS.md` → 100% Complete
2. `TODO_REFACTOR_PMPL.md` → archive/
3. `YZ_refactor/` dizini → archive/
4. Normal `TODO.md`'ye geri dön

---

**SON GÜNCELLEME:** 14 Aralık 2025  
**DURUM:** Refactor başlamadı  
**SONRAKI ADIM:** RF_YZ_1 başlasın
