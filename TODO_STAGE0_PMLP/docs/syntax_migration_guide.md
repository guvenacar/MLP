# MELP Syntax Migration Guide
## Eski Syntax (kurallar_kitabı_v1.md) → Yeni Syntax (PMLP)

**Tarih:** 19 Aralık 2025  
**Amaç:** mlp-original projelerindeki modülleri PMLP söz dizimine güncellemek

---

## 📊 Önemli Farklar Özeti

| Özellik | Eski Syntax | PMLP Syntax | Neden Değişti? |
|---------|-------------|-------------|----------------|
| **Blok sonlandırıcı** | `end if` (2 kelime) | `end_if` (tek token) | Lexer kolaylığı, parser basitliği |
| **Parametre ayırıcı** | `,` (virgül) | `;` (noktalı virgül) | Türk sayı formatı: `345,95` ondalık sayı |
| **Exit/Continue** | `exit for` / `exit while` | `exit` veya `exit_for` | Standalone + explicit desteklenir |
| **Return type** | `returns numeric` | `as numeric` veya `returns` | `as` daha modern, her ikisi de çalışır |
| **Tip tanımları** | `numeric[] arr` | `numeric[] arr` | **DEĞİŞMEDİ** ✅ |
| **String literal** | `"text"` | `"text"` | **DEĞİŞMEDİ** ✅ |
| **Comment** | `--` tek satır | `--` tek satır | **DEĞİŞMEDİ** ✅ |

---

## 1️⃣ Blok Sonlandırıcılar (KRİTİK DEĞİŞİKLİK)

### ❌ Eski Syntax (2 Kelime)
```mlp
if x > 5 then
    print x
end if

while x < 10
    x = x + 1
end while

for i = 0 to 10
    print i
end for

function add(numeric a, numeric b)
    return a + b
end function

struct Person
    string name
    numeric age
end struct
```

### ✅ Yeni Syntax (Tek Token, Alt Çizgili)
```mlp
if x > 5 then
    print x
end_if

while x < 10
    x = x + 1
end_while

for i = 0 to 10
    print i
end_for

function add(numeric a; numeric b)
    return a + b
end_function

struct Person
    string name
    numeric age
end_struct
```

### 🔄 Dönüştürme Kuralları

| Eski | Yeni | Regex Pattern |
|------|------|---------------|
| `end if` | `end_if` | `end\s+if` → `end_if` |
| `end while` | `end_while` | `end\s+while` → `end_while` |
| `end for` | `end_for` | `end\s+for` → `end_for` |
| `end function` | `end_function` | `end\s+function` → `end_function` |
| `end struct` | `end_struct` | `end\s+struct` → `end_struct` |
| `end enum` | `end_enum` | `end\s+enum` → `end_enum` |
| `end switch` | `end_switch` | `end\s+switch` → `end_switch` |
| `end match` | `end_match` | `end\s+match` → `end_match` |
| `end try` | `end_try` | `end\s+try` → `end_try` |

**Otomatik dönüşüm scripti örneği:**
```bash
sed -i 's/end if/end_if/g' *.mlp
sed -i 's/end while/end_while/g' *.mlp
sed -i 's/end for/end_for/g' *.mlp
sed -i 's/end function/end_function/g' *.mlp
sed -i 's/end struct/end_struct/g' *.mlp
```

---

## 2️⃣ Parametre Ayırıcısı (KRİTİK DEĞİŞİKLİK)

### ❌ Eski Syntax (Virgül)
```mlp
function calculate(numeric x, numeric y, string op)
    return x + y
end function

calculate(10, 20, "add")

-- Liste elemanları
list() = (10, 20, 30)

-- Array elemanları
numeric[] arr = [1, 2, 3, 4, 5]
```

### ✅ Yeni Syntax (Noktalı Virgül)
```mlp
function calculate(numeric x; numeric y; string op)
    return x + y
end_function

calculate(10; 20; "add")

-- Liste elemanları (trailing semicolon zorunlu!)
list data = (10; 20; 30;)

-- Array elemanları (trailing semicolon zorunlu!)
numeric[] arr = [1; 2; 3; 4; 5;]
```

### 🔍 NEDEN NOKTALÝ VĠRGÜL?

**Türk Sayı Formatı:**
```mlp
-- Eski syntax:
numeric x = 345.95      -- Nokta ondalık ayırıcı (İngiliz formatı)

-- PMLP syntax:
numeric x = 345,95      -- Virgül ondalık ayırıcı (Türk formatı)
numeric y = 1.234.567,89  -- Nokta binlik ayırıcı, virgül ondalık

-- Bu yüzden parametre ayırıcı virgül olamaz!
function test(numeric a, numeric b)  -- ❌ KAFA KARIŞTIRICI!
    -- a,b = parametre mi yoksa a=0, b=ondalık_sayı mı?
end function

function test(numeric a; numeric b)  -- ✅ NET VE AÇIK!
end function
```

### 🔄 Dönüştürme Kuralları

**Fonksiyon parametreleri:**
```bash
# Regex pattern: function içindeki virgülleri noktalı virgüle çevir
# DİKKAT: String literal içindeki virgüllere dokunma!
```

**Manuel dönüşüm gerekli çünkü:**
- String literal içindeki virgüller korunmalı: `"Ali, Veli"`
- Ondalık sayılardaki virgüller korunmalı: `3,14`
- Sadece parametre/eleman ayırıcı virgüller değişmeli

---

## 3️⃣ Exit/Continue Komutları

### ❌ Eski Syntax (Çift Kelime)
```mlp
for i = 0 to 10
    if condition then
        exit for
    end if
end for

while running
    if skip then
        continue while
    end if
end while
```

### ✅ Yeni Syntax (Standalone veya Explicit)
```mlp
-- Option 1: Standalone (genel)
for i = 0 to 10
    if condition then
        exit           -- Mevcut bloktan çık
    end_if
end_for

while running
    if skip then
        continue       -- Mevcut döngüye devam
    end_if
end_while

-- Option 2: Explicit (belirli blok)
for i = 0 to 10
    if condition then
        exit_for       -- For döngüsünden çık
    end_if
end_for

while running
    if skip then
        continue_while  -- While döngüsüne devam
    end_if
end_while
```

**Not:** Her iki yöntem de destekleniyor. Standalone daha kısa, explicit daha açık.

---

## 4️⃣ Return Type Belirtimi

### ❌ Eski Syntax
```mlp
function add(numeric a, numeric b) returns numeric
    return a + b
end function
```

### ✅ Yeni Syntax (Tercih: `as`)
```mlp
-- Tercih edilen:
function add(numeric a; numeric b) as numeric
    return a + b
end_function

-- Alternatif (hala destekleniyor):
function add(numeric a; numeric b) returns numeric
    return a + b
end_function
```

**Not:** `as` daha modern ve kısa. Her ikisi de çalışır ama PMLP'de `as` tercih edilir.

---

## 5️⃣ Else-If Syntax

### ❌ Eski Syntax
```mlp
if x > 100 then
    print("large")
else if x > 50 then
    print("medium")
else
    print("small")
end if
```

### ✅ Yeni Syntax
```mlp
if x > 100 then
    print("large")
else_if x > 50 then
    print("medium")
else
    print("small")
end_if
```

**Dönüşüm:**
```bash
sed -i 's/else if/else_if/g' *.mlp
```

---

## 6️⃣ Koleksiyon Literal Syntax

### ❌ Eski Syntax
```mlp
-- Liste (virgül + trailing noktalı virgül)
liste() = (10, 20, 30,)

-- Array (virgül)
numeric[] arr = [1, 2, 3]

-- Tuple
koordinat<> = <10, 20, "point">
```

### ✅ Yeni Syntax
```mlp
-- Liste (noktalı virgül + trailing zorunlu!)
list data = (10; 20; 30;)

-- Array (noktalı virgül + trailing zorunlu!)
numeric[] arr = [1; 2; 3;]

-- Tuple (noktalı virgül + trailing zorunlu!)
tuple<> pos = <10; 20; "point";>
```

**KRİTİK KURAL:** Trailing semicolon (son noktalı virgül) **ZORUNLU**!

```mlp
-- ❌ YANLIŞ:
list a = (1; 2; 3)      -- Eksik trailing semicolon
numeric[] b = [1; 2; 3]  -- Eksik trailing semicolon

-- ✅ DOĞRU:
list a = (1; 2; 3;)
numeric[] b = [1; 2; 3;]
```

---

## 7️⃣ Değişmeyen Özellikler ✅

### String Literals
```mlp
-- Her iki syntax'ta da aynı:
string name = "Ali"
string msg = "Hello, World!"
```

### Comments
```mlp
-- Her iki syntax'ta da aynı:
-- Tek satır yorum

---
Çok satırlı yorum
---
```

### Tip Tanımları
```mlp
-- Her iki syntax'ta da aynı:
numeric x
string name
boolean flag
numeric[] arr
```

### Operatörler
```mlp
-- Her iki syntax'ta da aynı:
+  -  *  /  %  **
==  !=  <  >  <=  >=
and  or  not
```

---

## 🔧 Otomatik Dönüştürme Scripti

```bash
#!/bin/bash
# migrate_to_pmlp.sh

# Kullanım: ./migrate_to_pmlp.sh dosya.mlp

FILE=$1

if [ ! -f "$FILE" ]; then
    echo "Dosya bulunamadı: $FILE"
    exit 1
fi

# Yedek al
cp "$FILE" "$FILE.backup"

# 1. Blok sonlandırıcıları
sed -i 's/end if/end_if/g' "$FILE"
sed -i 's/end while/end_while/g' "$FILE"
sed -i 's/end for/end_for/g' "$FILE"
sed -i 's/end function/end_function/g' "$FILE"
sed -i 's/end struct/end_struct/g' "$FILE"
sed -i 's/end enum/end_enum/g' "$FILE"
sed -i 's/end switch/end_switch/g' "$FILE"
sed -i 's/end match/end_match/g' "$FILE"
sed -i 's/end try/end_try/g' "$FILE"

# 2. Else-if
sed -i 's/else if/else_if/g' "$FILE"

# 3. Exit/Continue (basit replacement)
sed -i 's/exit for/exit_for/g' "$FILE"
sed -i 's/exit while/exit_while/g' "$FILE"
sed -i 's/continue for/continue_for/g' "$FILE"
sed -i 's/continue while/continue_while/g' "$FILE"

# 4. Return type (returns → as)
# NOT: Bu daha karmaşık, manuel kontrol önerilir
# sed -i 's/) returns /) as /g' "$FILE"

echo "✅ Dönüşüm tamamlandı: $FILE"
echo "⚠️  MANUEL KONTROL GEREKLİ:"
echo "   - Parametre virgüllerini noktalı virgüle çevir"
echo "   - Liste/Array literal'lerindeki virgülleri noktalı virgüle çevir"
echo "   - Trailing semicolon ekle: (1; 2; 3;)"
echo "   - String literal ve ondalık sayılardaki virgüllere dokunma!"
```

---

## 📋 Migration Checklist

Bir modülü PMLP'ye taşırken:

- [ ] **Blok sonlandırıcılar**: `end if` → `end_if` (tüm blok türleri)
- [ ] **Else-if**: `else if` → `else_if`
- [ ] **Exit/Continue**: `exit for` → `exit` veya `exit_for`
- [ ] **Fonksiyon parametreleri**: `,` → `;`
- [ ] **Liste literal'leri**: `(a, b, c)` → `(a; b; c;)` (trailing semicolon!)
- [ ] **Array literal'leri**: `[a, b, c]` → `[a; b; c;]` (trailing semicolon!)
- [ ] **Return type**: `returns` → `as` (opsiyonel)
- [ ] **Test et**: Derleme ve çalıştırma testleri yap
- [ ] **String literaller**: İçindeki virgüllere dokunmadığından emin ol
- [ ] **Ondalık sayılar**: `3,14` gibi değerlere dokunmadığından emin ol

---

## 📝 Örnek Dönüşüm

### Eski Kod (kurallar_kitabı syntax):
```mlp
function calculate(numeric x, numeric y) returns numeric
    if x > 100 then
        return x
    else if y > 100 then
        return y
    else
        return x + y
    end if
end function

numeric[] data = [10, 20, 30]
for i = 0 to 2
    print data[i]
end for
```

### Yeni Kod (PMLP syntax):
```mlp
function calculate(numeric x; numeric y) as numeric
    if x > 100 then
        return x
    else_if y > 100 then
        return y
    else
        return x + y
    end_if
end_function

numeric[] data = [10; 20; 30;]
for i = 0 to 2
    print data[i]
end_for
```

---

## ⚠️ DİKKAT EDİLECEK NOKTALAR

### 1. String Literal İçindeki Virgüller
```mlp
-- ❌ YANLIŞ: String içindeki virgüle dokunma!
string msg = "Ali; Veli; Ayşe"  -- BOZULDU!

-- ✅ DOĞRU:
string msg = "Ali, Veli, Ayşe"  -- String içinde virgül korunur
```

### 2. Ondalık Sayılar
```mlp
-- ❌ YANLIŞ: Ondalık ayırıcıya dokunma!
numeric pi = 3;14  -- BOZULDU! (3 ve 14 ayrı şeyler oldu)

-- ✅ DOĞRU:
numeric pi = 3,14  -- Virgül ondalık ayırıcı olarak kalır
```

### 3. Trailing Semicolon
```mlp
-- ❌ YANLIŞ:
list a = (1; 2; 3)       -- Eksik trailing semicolon

-- ✅ DOĞRU:
list a = (1; 2; 3;)      -- Trailing semicolon zorunlu
```

### 4. İç İçe Yapılar
```mlp
-- Dikkatli dönüştürme gerekli:
function test(numeric[] arr, string name)
    for i = 0 to 10
        if arr[i] > 5 then
            print name, arr[i]
        end if
    end for
end function

-- Doğru dönüşüm:
function test(numeric[] arr; string name)
    for i = 0 to 10
        if arr[i] > 5 then
            print name; arr[i]  -- print parametreleri de ; ile
        end_if
    end_for
end_function
```

---

## 🚀 Toplu Dönüşüm İçin

```bash
# Tüm .mlp dosyalarını dönüştür
find . -name "*.mlp" -type f | while read file; do
    echo "Dönüştürülüyor: $file"
    ./migrate_to_pmlp.sh "$file"
done

# Manuel kontrol için diff göster
find . -name "*.mlp.backup" | while read backup; do
    original="${backup%.backup}"
    echo "=== $original ==="
    diff "$backup" "$original" | head -20
done
```

---

## 📚 İlgili Dökümanlar

- **Eski Syntax:** [kurallar_kitabı_v1.md](specs/kurallar_kitabı_v1.md)
- **Yeni Syntax:** [pmlp_sozdizimi.md](../migration_docs/pmlp_sozdizimi.md)
  - **Not:** 1000+ satır, sadece ihtiyacın olan bölümü oku (index'e bak!)
- **C Modüllerini Kullanma:** [melp_llvm_modülleri.md](melp_llvm_modülleri.md)

---

## 🔧 TODO: Tip Belirtilmemiş Parametreler (YÜKSEK ÖNCELİK)

### 📊 Analiz Sonuçları

**Proje İstatistikleri:**
- Toplam .mlp dosyası: **349**
- Toplam fonksiyon tanımı: **974**
- **Tip belirtilmemiş parametreler:** **~206 fonksiyon** ⚠️

### Sorun Nedir?

Eski kodda fonksiyon parametreleri tip belirtmeden yazılabiliyordu:

```mlp
-- ❌ ESKİ: Tip yok
function skip_whitespace(text, pos) then
    int len = string_length(text)
    ...
end function

-- ✅ YENİ: Tip zorunlu
function skip_whitespace(string text; numeric pos) as numeric
    numeric len = string_length(text)
    ...
end_function
```

### Etkilenen Dosyalar (Öncelikli)

**Core Libraries (Kritik):**
- `lib/json/json_lexer*.mlp` (7 dosya)
- `lib/json/json_parser.mlp`
- `lib/core/optional.mlp`
- `lib/core/functional.mlp`

**Compiler Modules:**
- `compiler/stage1-mlp/lexer_v*.mlp`

**Tests:**
- `tests/` altında çeşitli test dosyaları (~20 dosya)

### 🎯 Dönüşüm Stratejisi

#### Adım 1: Core Libraries (Öncelik 1)
Sıra önemli - bağımlılık sırasına göre:

1. **lib/core/functional.mlp** (~10 fonksiyon)
   - map, filter, reduce gibi temel fonksiyonlar
   - Lambda parametreleri tip belirtimi

2. **lib/core/optional.mlp** (~5 fonksiyon)
   - Optional tip yardımcıları

3. **lib/json/json_lexer.mlp** serisini standartlaştır (~30 fonksiyon)
   - skip_whitespace, scan_string, scan_number, tokenize_json
   - Hepsi aynı interface'e sahip olmalı

#### Adım 2: Compiler Modules (Öncelik 2)
Self-hosting için kritik:

- `compiler/stage1-mlp/lexer_v3.mlp` (~6 fonksiyon)
- `compiler/stage1-mlp/lexer_v2.mlp` (~8 fonksiyon)

#### Adım 3: Tests (Öncelik 3)
Test dosyaları son - gerekirse devre dışı bırak:

- `tests/unit/failing/*` → Zaten fail
- `tests/test_*.mlp` → İhtiyaç varsa düzelt

### 📝 Tip Çıkarım Kuralları

Parametre ismine göre tip belirle:

| Parametre İsmi | Muhtemel Tip | Örnek |
|----------------|--------------|-------|
| `text`, `str`, `msg`, `name` | `string` | `string text` |
| `pos`, `index`, `len`, `count`, `i`, `j` | `numeric` | `numeric pos` |
| `flag`, `enabled`, `active` | `boolean` | `boolean flag` |
| `arr`, `list`, `items` | `numeric[]` veya `string[]` | Context'e bağlı |

### 🔧 Örnek Dönüşüm

**Dosya: lib/json/json_lexer.mlp**

```diff
- function skip_whitespace(text, pos) then
+ function skip_whitespace(string text; numeric pos) as numeric
-     int len = string_length(text);
+     numeric len = string_length(text)

      while pos < len
-         string ch = substring(text, pos, 1);
+         string ch = substring(text; pos; 1)
-         if is_whitespace(ch) == 0 then
+         if is_whitespace(ch) == 0 then
              return pos
-         end
+         end_if
          pos = pos + 1
-     end
+     end_while

      return pos
- end
+ end_function

- function scan_string(text, start_pos) then
+ function scan_string(string text; numeric start_pos) as numeric
-     int pos = start_pos + 1;
+     numeric pos = start_pos + 1
      ...
- end
+ end_function
```

### 📈 İş Yükü Tahmini

**Toplam değişiklik sayısı: ~206 fonksiyon**

| Kategori | Fonksiyon | Satır | Süre (Tahmini) |
|----------|-----------|-------|----------------|
| **Core libs** | ~20 | ~200 | 2-3 saat |
| **JSON libs** | ~35 | ~350 | 3-4 saat |
| **Compiler** | ~15 | ~150 | 2-3 saat |
| **Tests** | ~50 | ~500 | 4-5 saat |
| **Examples** | ~86 | ~1000 | **SKIP** (düşük öncelik) |
| **TOPLAM** | **~120** | **~1200** | **~12-15 saat** |

### ⚡ Hızlandırma Yöntemleri

1. **Otomatik Script:**
   ```bash
   # Basit pattern'ler için regex replacement
   sed -i 's/function \([a-z_]*\)(text,/function \1(string text;/g' *.mlp
   ```

2. **LLM/AI Yardımı:**
   - Dosyayı AI'ya ver, "Fonksiyon parametrelerine tip ekle" de
   - Batch processing ile hızlandır

3. **Kademeli Yaklaşım:**
   - Önce sadece core libs
   - Test et, çalıştığından emin ol
   - Sonra diğerleri

### ✅ Dönüşüm Checklist (Her Dosya İçin)

- [ ] Fonksiyon parametrelerine tip ekle (`,` → `;`)
- [ ] Return type ekle (`as numeric` / `as string`)
- [ ] Blok sonlandırıcıları düzelt (`end` → `end_function`, `end_if`)
- [ ] Function call'larda ayırıcıları düzelt (`,` → `;`)
- [ ] Local değişken tanımlarında tip kontrolü (`int` → `numeric`)
- [ ] Derle ve test et
- [ ] Git commit (dosya bazında)

### 🚨 Kritik Notlar

1. **"text" Değişken İsmi Sorun DEĞİL:**
   - `string text = "hello"` → ✅ DOĞRU (text değişken ismi)
   - `text name = "hello"` → ❌ YANLIŞ (text tip ismi olarak kullanılmış)
   - Analiz: Projede `text` TİP olarak kullanım YOK! ✅

2. **int vs numeric:**
   - Eski: `int pos = 0`
   - Yeni: `numeric pos = 0`
   - PMLP'de `int` yok, sadece `numeric`

3. **Virgül → Noktalı Virgül:**
   - Parametre tanımı: `(a, b)` → `(a; b)`
   - Function call: `func(1, 2)` → `func(1; 2)`
   - String içi: `"a, b"` → DEĞİŞMEZ!

---

**Son Güncelleme:** 24 Aralık 2025  
**Durum:** %95 uyumluluk, otomatik + manuel dönüşüm gerekli  
**TODO Durumu:** Tip belirtilmemiş ~206 fonksiyon tespit edildi, ~12-15 saat iş
