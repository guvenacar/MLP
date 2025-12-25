# MLP Test Düzeltme Kılavuzu

**Tarih:** 24 Aralık 2025  
**Amaç:** Test dosyalarındaki yaygın hataları düzeltmek için rehber

---

## 📋 Yaygın Hatalar ve Çözümleri

### 1. **Syntax Hatası: `int` Kullanımı**

**Hata:**
```mlp
int i = 0;
```

**Çözüm:**
```mlp
numeric i = 0
```

**Açıklama:**
- MLP'de `int` tipi yoktur, `numeric` kullanılmalıdır
- Satır sonunda noktalı virgül kullanılmaz

**Etkilenen Testler:**
- ✅ `test_break_simple.mlp`
- ✅ `test_while.mlp`
- ✅ `test_while2.mlp`
- ✅ `test_goto_simple.mlp`
- ✅ `test_goto_loop.mlp`

---

### 2. **Syntax Hatası: Noktalı Virgül**

**Hata:**
```mlp
string name = "test";
numeric x = 42;
```

**Çözüm:**
```mlp
string name = "test"
numeric x = 42
```

**Açıklama:**
- MLP sözdizimi satır sonunda noktalı virgül kullanmaz
- Sadece değişken bildirimi ve atama ifadeleri

**Pattern:**
```bash
# Hatalı pattern bul
grep -n ";\s*$" test_file.mlp

# Otomatik düzeltme (dikkatli kullan!)
sed -i 's/;\s*$//' test_file.mlp
```

---

### 3. **Tip Dönüşümleri**

**MLP Tip Sistemi:**
```mlp
numeric x = 10      # Sayısal değerler (int, float)
string s = "text"   # Metinler
boolean b = true    # Boolean değerler
```

**C-style → MLP Dönüşüm Tablosu:**
```
int     → numeric
float   → numeric
double  → numeric
char*   → string
bool    → boolean
void    → (function without return)
```

---

## 🔧 Test Düzeltme İş Akışı

### Adım 1: Hatayı Belirle
```bash
cd /home/pardus/projeler/mlp-original
build/bin/mlpc tests/unit/failing/TEST_NAME.mlp /tmp/test.asm 2>&1
```

### Adım 2: Düzeltme Yap

**Basit sed komutu (tek satır):**
```bash
sed -i 's/int /numeric /g' tests/unit/failing/TEST_NAME.mlp
sed -i 's/;$//' tests/unit/failing/TEST_NAME.mlp
```

**Manuel düzeltme:**
- Editörde dosyayı aç
- `int` → `numeric` değiştir
- Satır sonu `;` karakterlerini kaldır

### Adım 3: Test Et
```bash
build/bin/mlpc tests/unit/failing/TEST_NAME.mlp /tmp/test.asm && \
nasm -f elf64 /tmp/test.asm -o /tmp/test.o && \
gcc -no-pie /tmp/test.o compiler/runtime/src/minimal_runtime.c -o /tmp/test_out -lm && \
/tmp/test_out
```

### Adım 4: Başarılıysa Taşı
```bash
mv tests/unit/failing/TEST_NAME.mlp tests/unit/
echo "✅ TEST_NAME.mlp düzeltildi!"
```

---

## 🤖 Toplu Düzeltme Script'i

Benzer hataları toplu olarak düzeltmek için:

```bash
#!/bin/bash
# fix_common_syntax.sh - Yaygın syntax hatalarını düzelt

FAILING_DIR="tests/unit/failing"

# int -> numeric dönüşümü
for file in $FAILING_DIR/*.mlp; do
    if grep -q "int " "$file"; then
        echo "Düzeltiliyor: $(basename $file)"
        
        # Yedek al
        cp "$file" "$file.bak"
        
        # Değişiklikleri uygula
        sed -i 's/\bint\s\+/numeric /g' "$file"
        sed -i 's/;\s*$//' "$file"
        
        # Test et
        if build/bin/mlpc "$file" /tmp/test.asm 2>/dev/null; then
            echo "  ✅ Derleme başarılı"
            
            # Tam test
            if nasm -f elf64 /tmp/test.asm -o /tmp/test.o 2>/dev/null && \
               gcc -no-pie /tmp/test.o compiler/runtime/src/minimal_runtime.c -o /tmp/test_out -lm 2>/dev/null && \
               timeout 2s /tmp/test_out >/dev/null 2>&1; then
                echo "  ✅ Çalıştırma başarılı - Taşınıyor..."
                mv "$file" tests/unit/
                rm "$file.bak"
            else
                echo "  ❌ Çalıştırma başarısız - Geri alınıyor..."
                mv "$file.bak" "$file"
            fi
        else
            echo "  ❌ Derleme başarısız - Geri alınıyor..."
            mv "$file.bak" "$file"
        fi
    fi
done
```

---

## 📊 Düzeltme İstatistikleri

### İlk 5 Düzeltme (24 Aralık 2025)

| Test Dosyası | Değişiklik | Sonuç |
|--------------|------------|--------|
| `test_break_simple.mlp` | `int i = 0;` → `numeric i = 0` | ✅ PASS |
| `test_while.mlp` | `int i = 0;` → `numeric i = 0` | ✅ PASS |
| `test_while2.mlp` | `int i = 0;` → `numeric i = 0` | ✅ PASS |
| `test_goto_simple.mlp` | `int x = 0;` → `numeric x = 0` | ✅ PASS |
| `test_goto_loop.mlp` | `int n = 0;` → `numeric n = 0` | ✅ PASS |

**İlerleme:** 10/91 → 15/91 (%10 → %16.5)

---

## 🚧 Bilinen Sorunlar (Henüz Çözülmedi)

### Lambda İfadeleri
```mlp
print lambda(x, y) => x + y  # HATA: Lambda desteklenmiyor
```

### Closure'lar
```mlp
function outer(x)
    return lambda(y) => x + y  # HATA: Closure yakalaması
end
```

### İleri Seviye Özellikler
- `const` keyword
- `++` / `--` operatörleri
- Ternary operator (`? :`)
- Line continuation (`\`)
- String interpolation

### Linking Hataları
- Directory operations
- Time functions
- Process control
- Bazı file I/O operasyonları

---

## 💡 İpuçları

1. **Önce basit testleri düzelt** - Karmaşık testler daha fazla özellik gerektiriyor
2. **Her düzeltmeden sonra test et** - Geri alması kolay olsun
3. **Benzer testleri grupla** - Aynı hatayı tek seferde düzelt
4. **Yedek al** - `.bak` dosyası oluştur
5. **İlerlemeyi kaydet** - Her 5-10 düzeltmeden sonra commit yap

---

## 📝 Şablon: Yeni Hata Eklemek

Yeni bir yaygın hata keşfettiğinde:

```markdown
### X. **Hata Başlığı**

**Hata:**
```mlp
[hatalı kod örneği]
```

**Çözüm:**
```mlp
[düzeltilmiş kod örneği]
```

**Açıklama:**
- Hatanın nedeni
- Neden bu şekilde düzeltilmeli

**Etkilenen Testler:**
- [ ] `test_name1.mlp`
- [ ] `test_name2.mlp`
```

---

*Bu kılavuz test düzeltmeleri sırasında sürekli güncellenmektedir.*
