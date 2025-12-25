# 🔧 REFACTOR NEXT AI - START HERE

**Tarih:** 14 Aralık 2025  
**Refactor Sekansı:** RF_YZ_1, RF_YZ_2, RF_YZ_3  
**Durum:** BAŞLANGIÇ

---

## 🎯 SEN KİMSİN?

Sen **PMPL Architecture Refactor** yapan özel bir YZ'sin.

**ÖZEL GÖREV:** MELP compiler'ın PMPL mimarisini restore et.

---

## 📋 İLK ADIMLAR (5 Dakika)

### 1. TODO Dosyasını Oku
```bash
cat /home/pardus/projeler/MLP/MLP/TODO_REFACTOR_PMPL.md
```

**Bu dosya senin TEK referansın!**
- PMPL syntax kuralları orada
- Görev tanımın orada
- Test kriterlerin orada

### 2. Hangi YZ'sin Anla

**3 Refactor YZ var:**

| YZ | Görev | Süre | Sıra |
|----|-------|------|------|
| **RF_YZ_1** | Normalize Layer | 2-3h | İLK |
| **RF_YZ_2** | Lexer Refactor | 2-3h | İKİNCİ |
| **RF_YZ_3** | Parser Simplification | 3-4h | ÜÇÜNCÜ |

**Duruma Bak:**
```bash
cat /home/pardus/projeler/MLP/MLP/YZ_refactor/REFACTOR_STATUS.md
```

### 3. Rapor Oluştur

**İlk işin:**
```bash
# Rapor dosyası oluştur
touch /home/pardus/projeler/MLP/MLP/YZ_refactor/RF_YZ_X.md
# (X = senin numaran: 1, 2, veya 3)
```

**Rapor şablonu:**
```markdown
# RF_YZ_X: [Görev Adı]

**Başlangıç:** [Tarih/Saat]  
**Görev:** [Kısa açıklama]  
**Durum:** BAŞLADI

## Yapılanlar
- [ ] ...

## Testler
- [ ] ...

## Sonuç
[Tamamlanınca doldur]

**Bitiş:** [Tarih/Saat]
```

---

## 🚨 KRİTİK KURALLAR

### ⛔ YASAKLAR
1. **kurallar_kitabı.md OKUMA!** (Kullanıcı için, sen için değil)
2. **TODO.md OKUMA!** (Normal YZ'ler için, refactor için değil)
3. **YZ dizinindeki eski raporları OKUMA!** (Farklı context)
4. **"MLP user syntax" kullanma!** (PMPL dışında syntax yok)

### ✅ SADECE BUNLARI OKU
1. ✅ `TODO_REFACTOR_PMPL.md` - Ana görev tanımı
2. ✅ `YZ_refactor/REFACTOR_STATUS.md` - Mevcut durum
3. ✅ `YZ_refactor/RF_YZ_*.md` - Önceki refactor raporları (varsa)
4. ✅ Kod dosyaları (sadece değiştireceğin modüller)

---

## 🎯 GÖREV AKIŞI

### RF_YZ_1: Normalize Layer
**Eğer RF_YZ_1'sen:**
1. `TODO_REFACTOR_PMPL.md` → YZ_76 bölümünü oku
2. `compiler/stage0/normalize/` dizini oluştur
3. normalize.c, normalize.h, syntax_rules.h implement et
4. test_normalize.c yaz ve testleri geçir
5. Raporu tamamla (`RF_YZ_1.md`)
6. Commit yap
7. `REFACTOR_STATUS.md` güncelle → RF_YZ_1: ✅ COMPLETE

### RF_YZ_2: Lexer Refactor
**Eğer RF_YZ_2'sen:**
1. **İLK ÖNCE:** `REFACTOR_STATUS.md` kontrol et
   - RF_YZ_1 tamamlanmış mı? ✅
   - Tamamlanmamışsa **BEKLE!**
2. `TODO_REFACTOR_PMPL.md` → YZ_77 bölümünü oku
3. `compiler/stage0/modules/lexer/` dosyalarını refactor et
4. Token tanımları ekle, keyword recognition güncelle
5. test_lexer.c yaz ve testleri geçir
6. Raporu tamamla (`RF_YZ_2.md`)
7. Commit yap
8. `REFACTOR_STATUS.md` güncelle → RF_YZ_2: ✅ COMPLETE

### RF_YZ_3: Parser Simplification
**Eğer RF_YZ_3'sen:**
1. **İLK ÖNCE:** `REFACTOR_STATUS.md` kontrol et
   - RF_YZ_1 ✅ VE RF_YZ_2 ✅ tamamlanmış mı?
   - Değilse **BEKLE!**
2. `TODO_REFACTOR_PMPL.md` → YZ_78 bölümünü oku
3. statement_parser.c, functions_standalone.c refactor et
4. Pattern matching hack'lerini kaldır
5. Mevcut testlerin çalıştığını doğrula
6. Raporu tamamla (`RF_YZ_3.md`)
7. Commit yap
8. `REFACTOR_STATUS.md` güncelle → RF_YZ_3: ✅ COMPLETE

---

## 🧪 TEST PROTOKOLÜ

### Her YZ Kendi Testini Geçmeli

**RF_YZ_1:**
```bash
cd compiler/stage0/normalize
./test_normalize
# Tüm testler ✅ olmalı
```

**RF_YZ_2:**
```bash
cd compiler/stage0/modules/lexer
./test_lexer
# Tüm testler ✅ olmalı
```

**RF_YZ_3:**
```bash
cd /home/pardus/projeler/MLP/MLP
./melp test_adv.mlp test_adv.s
nasm -f elf64 test_adv.s -o test_adv.o
gcc test_adv.o runtime/sto/*.o -o test_adv
./test_adv
# Program çalışmalı ✅
```

---

## 📊 İLERLEME TAKİBİ

**Durumu kontrol et:**
```bash
cat YZ_refactor/REFACTOR_STATUS.md
```

**Raporu güncelle:**
```bash
vim YZ_refactor/RF_YZ_X.md
# İlerlemeyi kaydet
```

---

## 🎓 PMPL HAKKINDAKİ HER ŞEY

**PMPL = Compiler'ın gördüğü TEK syntax**

### Temel Kural
```pmpl
end_if       ← Tek keyword, underscore ile
end_while    ← Tek keyword, underscore ile
else_if      ← Tek keyword, underscore ile
exit_for     ← Tek keyword, underscore ile
```

**Detaylar:** `TODO_REFACTOR_PMPL.md` dosyasında!

---

## 🚦 BİTİŞ PROTOKOLÜ

### Görevini Tamamladıysan

1. **Tüm testler geçiyor mu?**
   ```bash
   # Kendi testlerini çalıştır
   ```

2. **Raporu tamamla:**
   ```bash
   vim YZ_refactor/RF_YZ_X.md
   # Durum: BAŞLADI → Durum: TAMAMLANDI
   # Testler: ❌ → Testler: ✅
   # Sonuç bölümünü doldur
   ```

3. **Commit yap:**
   ```bash
   git add .
   git commit -m "RF_YZ_X complete: [Görev adı]"
   ```

4. **Status güncelle:**
   ```bash
   vim YZ_refactor/REFACTOR_STATUS.md
   # RF_YZ_X: 🔴 → 🟢
   ```

5. **Kullanıcıya bildir:**
   ```
   ✅ RF_YZ_X tamamlandı!
   📊 Testler: PASSED
   📝 Rapor: YZ_refactor/RF_YZ_X.md
   🚀 Sonraki YZ başlayabilir!
   ```

---

## ❓ SORUN ÇIKARSA

### Görev Belirsizse
→ `TODO_REFACTOR_PMPL.md` dosyasını tekrar oku  
→ İlgili kod dosyalarını incele  
→ Kullanıcıya sor

### Test Geçmiyorsa
→ Kodu gözden geçir  
→ Test çıktısını incele  
→ Debug bilgisi ekle  
→ Kullanıcıya rapor et

### Önceki YZ Tamamlanmamışsa
→ **BEKLE!** Sırayla yapılmalı  
→ Kullanıcıya bildir: "RF_YZ_X tamamlanmamış, beklemem gerekiyor"

---

## 🎯 İLK KOMUTIN

```bash
# 1. Durumu kontrol et
cat YZ_refactor/REFACTOR_STATUS.md

# 2. TODO'yu oku
cat TODO_REFACTOR_PMPL.md

# 3. Rapor oluştur
touch YZ_refactor/RF_YZ_X.md
vim YZ_refactor/RF_YZ_X.md

# 4. BAŞLA! 🚀
```

---

**BAŞARILAR!** 🎉

*"PMPL mimarisi restore edilecek, self-hosting mümkün olacak!"*
