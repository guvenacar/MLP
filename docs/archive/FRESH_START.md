# 🚀 MLP Project - Fresh Start Guide

**Tarih:** 22 Kasım 2025  
**Branch:** test-phase5-2-with-parser-fix  
**Son Commit:** d140aa9

---

## 📍 Mevcut Durum

### ✅ Tamamlanan İşler

1. **Keyword sistemi düzeltildi:**
   - `numeric`, `string`, `boolean` (int/float/bool değil)
   - KeywordMap ve tüm tokenlar güncellendi

2. **Türkçe → İngilizce çeviri:**
   - Tüm token isimleri: TOKEN_YAPI_* → TOKEN_*
   - Tüm error mesajları İngilizce

3. **Runtime modern wrappers:**
   - 12 Python/VB.NET style fonksiyon eklendi
   - `len()`, `ord()`, `chr()`, `substring()`, vb.

4. **Global değişken tanımlama çalışıyor:**
   ```mlp
   numeric x = 42
   print x
   ```

### 🔴 Bilinen Sorun

**"end function" compound token olarak tanınmıyor:**

```mlp
function test() then
    return 42
end function    -- ❌ Parser hatası veriyor
```

**Workaround:**
```mlp
function test() then
    return 42
end             -- ✅ Çalışıyor
```

**Sorun:** Lexer "end" ve "function" kelimelerini ayrı token olarak üretiyor, compound token ("end function") üretemiyor.

---

## 📚 Kritik Dosyalar

**Zorunlu Okumalar:**
1. `/home/pardus/projeler/tyd-lang/MLP/SPECS.md` - Dil spesifikasyonu
2. `/home/pardus/projeler/tyd-lang/MLP/AI_RULES.md` - AI kuralları (yeni güncellendi)
3. `/home/pardus/projeler/tyd-lang/MLP/TODO.md` - Eksikler listesi

**Önemli Dosyalar:**
- `BASE_LANGUAGE.md` - Temel dil syntax kuralları
- `self_host/mlp_compiler.c` - Bootstrap compiler (6660 satır, tek dosya)
- `runtime/runtime.c` - Runtime fonksiyonları (2828+ satır)

---

## 🎯 Öncelikli İşler (TODO.md'den)

### 1. 🔴 Array/Dizi Desteği (EN KRİTİK!)
**Self-hosting için şart!**

```mlp
numeric numbers[10]
string keywords[5] = ["if", "else", "while", "for", "func"]
numeric x = numbers[0]
```

**Neden gerekli:**
- Token listesi: `Token[] tokens`
- AST node listesi: `ASTNode[] nodes`
- Symbol table: `Degisken[] kapsam_haritasi`

### 2. 🟡 Input/Stdin Okuma
```mlp
string input = read_input()
numeric number = read_int()
```

### 3. 🟢 "end function" Compound Token Sorunu
**Alternatifler:**
- A) "end" kullanımını tercih et (zaten çalışıyor)
- B) Parser'da "end" + next token kontrolü
- C) Lexer'da compound token üretimini düzelt

---

## 🔧 Başlangıç Komutları

```bash
# Proje dizini
cd /home/pardus/projeler/tyd-lang/MLP

# SPECS.md'yi oku
cat SPECS.md

# AI_RULES.md'yi oku
cat AI_RULES.md

# TODO.md'yi oku
cat TODO.md

# Bootstrap compiler'ı derle
gcc -o mlp_bootstrap self_host/mlp_compiler.c -lm

# Basit test
echo 'numeric x = 42
print x' > test.mlp
./mlp_bootstrap test.mlp test.asm
```

---

## 💡 AI için Notlar

### Altın Kurallar

1. **Self-hosting ilkesi:** Tüm yeni özellikler MLP dilinde yazılacak
2. **TODO.md takibi:** Eksikler MLP'de tamamlanacak
3. **SPECS.md kutsal:** Değişiklik için kullanıcı onayı şart
4. **Veri tipleri:** numeric, string, boolean (int/float YOK!)
5. **Noktalı virgül:** Python-style, NO semicolons

### Önerilen Strateji

1. **Önce Array implementasyonu:** Self-hosting için kritik
2. **MLP'de kod yaz:** Her yeni özellik MLP'de test edilir
3. **Test-driven:** Her özellik için test dosyası yaz
4. **Küçük adımlar:** Bir seferde bir özellik

### Debug İpuçları

```bash
# Lexer debug (token'ları göster)
./mlp_bootstrap test.mlp test.asm 2>&1 | grep "DEBUG:"

# Preprocessor çıktısı
python3 dil_cevirici.py test.mlp --verbose

# Assembly çıktısı
cat test.asm
```

---

## 📋 Son Commit Özeti

**Commit:** d140aa9  
**Mesaj:** docs: Update AI_RULES.md - Add TODO.md integration and self-hosting requirements

**Değişiklikler:**
- AI_RULES.md güncellendi (TODO.md entegrasyonu)
- Test dosyaları temizlendi
- 57 dosya değişti (4460 ekleme, 730 silme)

**Yeni Belgeler:**
- BASE_LANGUAGE.md
- CLEANUP_PLAN.md
- docs/ARCHITECTURE_AND_MEMORY_STRATEGY.md
- docs/COMPILER_BACKENDS_COMPARISON.md
- docs/COMPILER_CHAIN_ANALYSIS.md
- docs/HYBRID_BACKEND_ARCHITECTURE.md
- docs/LANGUAGE_EVOLUTION_GUIDE.md

---

## 🎯 Önerilen İlk Görev

**Array implementasyonu ile başla:**

1. **Lexer:** `[` `]` token'larını ekle
2. **Parser:** Array tanımlama syntax'ı (`numeric arr[10]`)
3. **Parser:** Array erişimi syntax'ı (`arr[i]`)
4. **Generator:** Stack'te array allocation
5. **Runtime:** Array helper fonksiyonları
6. **Test:** Array test dosyaları yaz (MLP'de!)

**Tahmini süre:** 2-3 gün

---

## 🚫 Yapılmaması Gerekenler

- ❌ C/Python'da yeni özellik yazmak (sadece MLP!)
- ❌ SPECS.md'yi onaysız değiştirmek
- ❌ "int", "float", "char" kullanmak
- ❌ Noktalı virgül eklemek
- ❌ TODO.md'yi göz ardı etmek

---

## ✅ Başarı Kriterleri

**Array implementasyonu tamamlandığında:**

```mlp
-- Test: Array tanımlama ve erişim
numeric numbers[5] = [10, 20, 30, 40, 50]
numeric x = numbers[2]  -- 30
print x

-- Test: String array
string keywords[3] = ["if", "else", "while"]
string first = keywords[0]
print first  -- "if"

-- Test: Array iteration
numeric i = 0
while i < 5 then
    print numbers[i]
    i = i + 1
end while
```

**Bu çalışırsa self-hosting denemesine başlanabilir!**

---

## 📞 Yardım Gerekirse

1. AI_RULES.md'yi oku (tüm kurallar orada)
2. TODO.md'yi kontrol et (öncelikler)
3. SPECS.md'ye bak (syntax referansı)
4. Git log'a bak (geçmiş değişiklikler)

**Başarılar! 🎉**
