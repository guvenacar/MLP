# Stage 1 Modül Analiz Raporu

**Tarih:** 20 Aralık 2025  
**YZ:** YZ_ÜA_02  
**Amaç:** Başarısız derlenen Stage 1 modüllerinin kök neden analizi

---

## 📊 Özet

| Durum | Sayı | Oran |
|-------|------|------|
| ✅ Başarılı | 30+ | ~80% |
| ❌ Başarısız | 8 | ~20% |

---

## 🔴 Sorunlu Modüller ve Kök Nedenler

### 1. `token_types.mlp` ✅ (Aslında başarılı!)
**Dosya:** `core/token_types.mlp`  
**Durum:** ✅ **SORUN YOK** - 91 const başarıyla tanımlandı  
**Not:** Sadece const içeriyor, fonksiyon yok - bu beklenen davranış.

---

### 2. `control_flow_parser.mlp` / `control_flow_codegen.mlp`
**Dosya:** `control_flow/control_flow_parser.mlp:79`  
**Hata:** `error [Parser]: Failed to parse array element`

**Sorunlu Kod:**
```pmpl
if token_type != T_IF
    return ([;), pos]  -- Not an if statement
end_if
```

**Problem:** `([;)` geçersiz syntax - köşeli parantez `]` beklenirken `)` bulunuyor.

**Analiz:**
Kodun amacı: Boş list + pos değeri döndürmek (tuple olarak)

**PMPL Kuralları (docs_tr/kurallar_kitabı_v1.md ve pmlp_kesin_sozdizimi.md):**
- Boş list: `()` 
- Tek elemanlı list: `(elem;)`  
- Liste tanımlama: `list a() = (;)` veya `list a() = ()`

**Stage 1'de Doğru Kullanım (variables_parser.mlp):**
```pmpl
return (;)  -- Boş list döndür
```

**Sorunun Kaynağı:**
`return ([;), pos]` kodunda:
- `[;)` kısmı ne array ne list sözdizimi
- `]` köşeli parantez ile `]` arasında tutarsızlık

**Olası Çözümler:**
1. Tuple kullan: `return <(;); pos>` - Boş list ve pos tuple olarak
2. List kullan: `return ((;); pos;)` - Her şey list içinde
3. Sadece boş: `return (;)` - Eğer pos gerekmiyorsa

---

### 3. `operators_parser.mlp`
**Dosya:** `operators/operators_parser.mlp:316`  
**Hata:** `error [Parser]: Expected ']' to close array`

**Sorunlu Kod:**
```pmpl
if token_type == T_NUMBER then
    list expr = (EXPR_LITERAL; token_value; 0; 0; 0;)
    return [expr, pos + 1]
end_if
```

**Problem:** `return [expr, pos + 1]` - köşeli parantez `[]` kullanılmış!

**Analiz:**
- PMPL'de array: `arr[i]` ile erişim
- PMPL'de tuple: `<elem1; elem2>` ile tanımlama
- **`[expr, pos + 1]` ne Python ne PMPL sözdizimi!**

**Çözüm:**
```pmpl
return <expr; pos + 1>   -- Tuple syntax
-- veya --
return (expr; pos + 1;)  -- List syntax
```

---

### 4. `enums_parser.mlp` / `enums_codegen.mlp`
**Dosya:** `enums/enums_parser.mlp:72`  
**Hata:** `error [Parser]: Expected 'function' keyword`

**Sorunlu Kod:**
```pmpl
while current_pos < len(tokens)
    numeric token_type = tokens[current_pos]
```

**Problem:** `tokens[current_pos]` - köşeli parantez array erişimi

**Analiz:**
- PMPL'de array erişimi: `arr[i]` ✅ bu doğru!
- AMA `len(tokens)` fonksiyonu tanımlı mı?
- Sorun muhtemelen **import chain** - önceki modüllerdeki hatalar bu modüle yayılıyor

**Kontrol:**
- `enums_parser.mlp` hangi modülleri import ediyor?
- Import edilen modüller başarıyla derleniyor mu?

---

### 5. `test_literals.mlp`
**Dosya:** `literals/test_literals.mlp:18`  
**Hata:** `error [Parser]: Expected 'function' keyword`

**Sorunlu Kod:**
```pmpl
numeric test_count = 0
numeric test_passed = 0
numeric test_failed = 0
```

**Problem:** Global değişken tanımı!

**Analiz:**
- PMPL'de global değişken destekleniyor mu?
- Eğer destekleniyorsa: Parser sorunu
- Eğer desteklenmiyorsa: Kod yeniden yazılmalı

**PMPL Kuralı:** Tüm değişkenler fonksiyon içinde olmalı (stateless prensip)

---

### 6. `test_variables.mlp`
**Dosya:** `variables/test_variables.mlp:26`  
**Hata:** `Module 'compiler/stage1/modules/core/token_types.mlp' not found`

**Problem:** Import yolu yanlış!

**Sorunlu Kod:**
```pmpl
import "compiler/stage1/modules/core/token_types.mlp"
```

**Analiz:**
- Dosyalar `archive/stage1_api_attempt/modules/` altında
- Ama import yolu `compiler/stage1/modules/` diyor
- **Yol uyuşmazlığı!**

**Çözüm:** Import yollarını düzelt:
```pmpl
import "core/token_types.mlp"  -- Relative path
-- veya --
import "archive/stage1_api_attempt/modules/core/token_types.mlp"  -- Full path
```

---

### 7. `test_operators.mlp`
**Durum:** `operators_parser.mlp`'deki hatalardan etkileniyor (import chain)

---

### 8. `test_enums.mlp`
**Durum:** `enums_parser.mlp`'deki hatalardan etkileniyor (import chain)

---

## 🔍 Kök Neden Kategorileri

| Kategori | Modüller | Öncelik |
|----------|----------|---------|
| **A: Tuple/List Syntax** | `control_flow_parser`, `operators_parser` | 🔴 Yüksek |
| **B: Import Yolu** | `test_variables` | 🟡 Orta |
| **C: Global Değişken** | `test_literals` | 🟡 Orta |
| **D: Import Chain** | `test_operators`, `test_enums`, `test_control_flow` | ⚪ Düşük (A çözülünce düzelir) |

---

## 💡 Önerilen Düzeltme Sırası

### Adım 1: Tuple Return Syntax Fix (YZ_112)
**Etki:** 4+ modül düzelir  
**Dosyalar:**
- `control_flow_parser.mlp` - `return ([;), pos]` → `return <(;); pos>`
- `operators_parser.mlp` - `return [expr, pos + 1]` → `return <expr; pos + 1>`

**Sayı:** ~22+ satır düzeltme gerekiyor

**Otomatik Düzeltme Scripti:**
```bash
# Python tuple [x, y] → PMPL tuple <x; y>
sed -i 's/return \[\([^,]*\), \([^\]]*\)\]/return <\1; \2>/g' file.mlp

# Boş list return ([;), x] → <(;); x>
sed -i "s/return (\[;\), \([^]]*\)]/return <(;); \1>/g" file.mlp
```

### Adım 2: Import Yolları (YZ_113)
**Etki:** 1+ modül düzelir  
**İş:** Tüm import yollarını standardize et

### Adım 3: Global Değişken → Fonksiyon İçi (YZ_114)
**Etki:** 1 modül düzelir  
**İş:** Test dosyalarında global değişkenleri fonksiyon içine al

---

## 📝 PMPL Syntax Hatırlatması

```pmpl
-- List (parentez + noktalı virgül)
list mylist = (1; 2; 3;)
mylist(0)  -- Erişim

-- Array (köşeli parantez)
numeric[5] arr
arr[0] = 10  -- Erişim

-- Tuple (açılı parantez)
tuple mytuple = <10; "hello"; true>
mytuple<0>  -- Erişim

-- Return multiple values
return <value1; value2>  -- Tuple döndür
```

---

## ⚠️ Kritik Soru

**`return [x, y]` Python-style syntax Stage 1 kodlarında yaygın mı?**

### Bulgu:
```
return [x, y]     → 12 kullanım (operators_parser.mlp)
return ([;), pos] → 10+ kullanım (control_flow_parser.mlp)
```

### Etkilenen Dosyalar:
1. `operators_parser.mlp` - 12 satır
2. `control_flow_parser.mlp` - 10+ satır

### Çözüm Önerisi:
**Otomatik dönüşüm scripti:**
```bash
# [x, y] → <x; y>
sed -i 's/return \[\([^,]*\), \([^\]]*\)\]/return <\1; \2>/g' file.mlp

# ([;), x] → <(); x>
sed -i 's/return (\[;\), \([^]]*\)]/return <(); \1>/g' file.mlp
```

---

*Rapor Sonu - YZ_ÜA_02*
