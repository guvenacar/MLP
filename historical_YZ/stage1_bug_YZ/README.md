# 🐛 stage1_bug_YZ - LLVM Öncesi Bug Fix Dizini

**Oluşturulma:** 20 Aralık 2025  
**Üst Akıl:** YZ_ÜA_04  
**Durum:** 🔥 AKTİF

---

## 📋 Amaç

LLVM backend'e geçmeden önce tüm Stage 1 bug'larını temizlemek.

**Hedef:** Sıfır bug ile LLVM'e geçiş!

---

## 📁 Yapı

```
stage1_bug_YZ/
├── README.md           ← Bu dosya
├── BUG_LIST.md         ← Tüm bug'ların listesi
├── YZ_00.md            ← İlk bug fix YZ görevi
├── YZ_01.md            ← ...
└── ...
```

---

## 🐛 Bilinen Bug'lar

### BUG-001: Fonksiyon Body Boş Üretiliyor (ÖNCELİK: YÜKSEK)

**Sorun:**
```mlp
function main() as int {
    const x = 88
    return(x)
}
```

**Beklenen:** `const x = 88` ve `return(x)` için assembly üretilmeli  
**Gerçek:** Fonksiyon body tamamen boş

**Muhtemel Sebep:** `const` keyword parse edilirken body parsing erkenden sonlanıyor

**Bakılacak Dosyalar:**
- `compiler/stage0/modules/variable/variable_parser.c`
- `compiler/stage0/modules/statement/statement_parser.c`
- `compiler/stage0/modules/functions/functions_parser.c`

---

## ✅ Tamamlanan Bug Fix'ler

(Henüz yok)

---

## 📊 İlerleme

| Bug ID | Açıklama | Durum | YZ |
|--------|----------|-------|-----|
| BUG-001 | Body boş üretiliyor | ⏳ Bekliyor | YZ_00 |

---

**Üst Akıl:** YZ_ÜA_04
