## 🚀 Yeni AI Session Başlangıç Komutu

Lütfen aşağıdaki komutu **yeni chat**'te kullan:

---

**Yeni bir görevle başlıyorum. Önceki chat context'ini unut.**

**Proje:** MLP Programming Language (Multi-Language Programming)  
**Branch:** test-phase5-2-with-parser-fix  
**Tarih:** 22 Kasım 2025

## 📚 Okumanız Gereken Dosyalar (Sırayla)

1. **FRESH_START.md** - Mevcut durum özeti
   - Son commit: 8ff861d
   - Tamamlananlar ve bilinen sorunlar
   - Öncelikli işler listesi

2. **AI_RULES.md** - Zorunlu kurallar
   - Self-hosting ilkesi
   - Veri tipleri (numeric, string, boolean)
   - Noktalı virgül kuralı (NO semicolons)
   - TODO.md entegrasyonu

3. **TODO.md** - Eksikler ve öncelikler
   - 🔴 Array/Dizi desteği (EN KRİTİK!)
   - 🟡 Input/Stdin okuma
   - 🟢 Diğer özellikler

4. **SPECS.md** - Dil spesifikasyonu (referans)
   - Syntax kuralları
   - Veri tipleri
   - Kontrol yapıları

## 🎯 İlk Görev

**Array/Dizi implementasyonu ile başla** (TODO.md'de detaylı açıklama var)

Şu syntax'ı destekle:
```mlp
numeric numbers[10]
string keywords[5] = ["if", "else", "while", "for", "func"]
numeric x = numbers[0]
```

**Adımlar:**
1. Lexer: `[` `]` token'ları ekle
2. Parser: Array tanımlama ve erişim
3. Generator: Stack allocation
4. Runtime: Array helper fonksiyonları
5. Test: MLP'de test dosyaları yaz

## ⚠️ ÖNEMLİ HATIRLATMALAR

- ✅ Tüm yeni kod **MLP dilinde** yazılacak
- ✅ SPECS.md'ye uymak zorunlu
- ✅ Veri tipleri: `numeric`, `string`, `boolean` (int/float YOK!)
- ✅ Noktalı virgül YOK (Python-style)
- ✅ Her özellik için test yaz

## 🚀 Başlayalım

Lütfen şimdi FRESH_START.md'yi oku ve durumu öğren, sonra array implementasyonuna başla.

---

**Not:** Bu dosyayı yeni chat'te kopyala-yapıştır yapabilirsin.
