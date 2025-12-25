# MLP Test Dosyaları Syntax Düzeltme Görevi

**Tarih:** 24 Aralık 2025  
**Hedef:** Test dosyalarını pmlp_kesin_sozdizimi.md'ye uygun hale getir  
**Görev Sahibi:** Yeni YZ Agent

---

## 📋 GÖREV TANIMI

MLP self-hosting derleyicisi %100 doğru çalışıyor ve pmlp_kesin_sozdizimi.md'ye tam uyumlu. Ancak test dosyaları eski/yanlış syntax kullandığı için başarısız oluyor. Bu test dosyalarını düzelt.

---

## 🎯 DÜZELTME KURALLARI

### Kural 1: Blok Sonlandırıcılar TEK TOKEN, ALT ÇİZGİLİ

#### YANLIŞ → DOĞRU Dönüşümler:
```
end function  →  end_function
end while     →  end_while
end for       →  end_for
end struct    →  end_struct
end if        →  end_if
end           →  end_function (eğer function bloğu içindeyse)
end           →  end_while (eğer while bloğu içindeyse)
end           →  end_for (eğer for bloğu içindeyse)
```

#### Kritik: Boşluk OLMAMALI, alt çizgi OLMALI!

---

## 📝 DÜZELTİLMESİ GEREKEN TEST DOSYALARI

### Kategori 1: `end` Kullanımı (5 dosya)

#### 1. test_minimal.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_minimal.mlp`

**Mevcut Kod:**
```mlp
function test(numeric arr[], numeric x)
    print(x)
end
```

**Düzeltilmiş Kod:**
```mlp
function test(numeric arr[], numeric x)
    print(x)
end_function
```

**Değişiklik:** Satır 3 → `end` yerine `end_function`

---

#### 2. test_array_sum.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_array_sum.mlp`

**Mevcut Kod:**
```mlp
function sum_array(arr[], size)
    numeric total = 0
    numeric i = 0
    while i < size
        total = total + arr[i]
        i = i + 1
    end
    return total
end
```

**Düzeltilmiş Kod:**
```mlp
function sum_array(arr[], size)
    numeric total = 0
    numeric i = 0
    while i < size
        total = total + arr[i]
        i = i + 1
    end_while
    return total
end_function
```

**Değişiklikler:**
- Satır 7 → `end` yerine `end_while`
- Satır 9 → `end` yerine `end_function`

---

#### 3. test_simple_fn.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_simple_fn.mlp`

**Kontrol Et:** Dosyayı aç ve `end` kullanımlarını `end_function` yap

---

#### 4. test_two_funcs.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_two_funcs.mlp`

**Kontrol Et:** Dosyayı aç ve tüm `end` kullanımlarını `end_function` yap

---

#### 5. test_array_only.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_array_only.mlp`

**Kontrol Et:** Dosyayı aç ve tüm `end` kullanımlarını uygun blok sonlandırıcıyla değiştir

---

### Kategori 2: Boşluklu Kullanım (5 dosya)

#### 6. test_while_only.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_while_only.mlp`

**Mevcut Kod:**
```mlp
function test_while()
    numeric i = 0
    while i < 3
        print "Loop"
        i = i + 1
    end while
    return 0
end function
```

**Düzeltilmiş Kod:**
```mlp
function test_while()
    numeric i = 0
    while i < 3
        print "Loop"
        i = i + 1
    end_while
    return 0
end_function
```

**Değişiklikler:**
- Satır 6 → `end while` yerine `end_while` (boşluk kaldır)
- Satır 8 → `end function` yerine `end_function` (boşluk kaldır)

---

#### 7. test_for_only.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_for_only.mlp`

**Mevcut Kod:**
```mlp
function test_for()
    for j = 1 to 5
        print "Loop"
    end for
    return 0
end function
```

**Düzeltilmiş Kod:**
```mlp
function test_for()
    for j = 1 to 5
        print "Loop"
    end_for
    return 0
end_function
```

**Değişiklikler:**
- Satır 4 → `end for` yerine `end_for` (boşluk kaldır)
- Satır 6 → `end function` yerine `end_function` (boşluk kaldır)

---

#### 8. test_import_simple.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_import_simple.mlp`

**Mevcut Kod:**
```mlp
import "mylib.mlp"

function main()
    greet("World")
    return 0
end function

main()
```

**Düzeltilmiş Kod:**
```mlp
import "mylib.mlp"

function main()
    greet("World")
    return 0
end_function

main()
```

**Değişiklik:** Satır 6 → `end function` yerine `end_function` (boşluk kaldır)

---

#### 9. test_import_simple_eng.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_import_simple_eng.mlp`

**Kontrol Et:** test_import_simple.mlp ile aynı düzeltmeyi yap

---

#### 10. test_import_simple_norm.mlp
**Lokasyon:** `/home/pardus/projeler/mlp-original/test_import_simple_norm.mlp`

**Kontrol Et:** test_import_simple.mlp ile aynı düzeltmeyi yap

---

## 🔍 OTOMATIC DÜZELTME KOMUTLARİ

Test dosyalarını toplu düzeltmek için bu komutları kullanabilirsin:

### Komut 1: Boşluklu end_function düzelt
```bash
cd /home/pardus/projeler/mlp-original
sed -i 's/end function/end_function/g' test_*.mlp
```

### Komut 2: Boşluklu end_while düzelt
```bash
sed -i 's/end while/end_while/g' test_*.mlp
```

### Komut 3: Boşluklu end_for düzelt
```bash
sed -i 's/end for/end_for/g' test_*.mlp
```

### Komut 4: Satır sonundaki tek "end" düzelt (Dikkatli!)
```bash
# Bu komutu çalıştırmadan önce manuel kontrol et!
# Hangi dosyalarda "end" var?
grep -n '^end$' test_*.mlp

# Sonra manuel düzelt veya daha spesifik sed kullan
```

---

## ✅ DOĞRULAMA ADIMLARI

Her düzeltmeden sonra:

### Adım 1: Syntax kontrolü
```bash
cd /home/pardus/projeler/mlp-original
./mlpc test_minimal.mlp /tmp/test.asm
```

### Adım 2: Tüm test suite'i çalıştır
```bash
./test_selfhost_suite.sh
```

### Adım 3: Sonuçları kontrol et
```bash
# Başarı oranı artmalı:
# Önce: 8/18 = 44%
# Sonra: ~16/18 = 89%
```

---

## 📊 BEKLENEN SONUÇLAR

### Düzeltme Öncesi (Şu An):
- Toplam Test: 18
- Başarılı: 8 ✅
- Başarısız: 10 ❌
- Başarı Oranı: 44%

### Düzeltme Sonrası (Beklenen):
- Toplam Test: 18
- Başarılı: 15-16 ✅
- Başarısız: 2-3 ❌
- Başarı Oranı: **83-89%** 🎉

### Kalan Başarısızlıklar (Normal):
- Import sistem eksikliği (gerçek implementation sorunu)
- Array handling eksikliği (gerçek implementation sorunu)

---

## 📚 REFERANS BELGELER

### Ana Syntax Referansı:
- **pmlp_kesin_sozdizimi.md** - MLP'nin TEK ve KESİN syntax dokümanı
- Lokasyon: `/home/pardus/projeler/mlp-original/pmlp_kesin_sozdizimi.md`

### Önemli Bölümler:
- Satır 27: "Blok Sonlandırıcı: Alt çizgili tek token"
- Satır 38-48: Tüm end_* tokenların listesi
- Satır 53-60: end_function örneği

---

## 🎯 GÖREV TAMAMLAMA KRİTERLERİ

Görev tamamlanmış sayılır:
1. ✅ 10 test dosyası düzeltildi
2. ✅ `./test_selfhost_suite.sh` çalıştırıldı
3. ✅ Başarı oranı %80+ oldu
4. ✅ Syntax hataları kalmadı (sadece implementation eksiklikleri)

---

## ⚠️ DİKKAT EDİLMESİ GEREKENLER

### 1. Context Korunmalı
Sadece syntax değiştir, logic'i değiştirme:
```
✅ DOĞRU: end function → end_function
❌ YANLIŞ: Kod mantığını değiştirmek
```

### 2. Tüm Dosyayı Oku
Sed komutları tehlikeli olabilir, her dosyayı önce manuel kontrol et:
```bash
# Dosyayı önce oku
cat test_minimal.mlp

# Sonra düzelt
sed -i 's/end function/end_function/g' test_minimal.mlp

# Sonucu kontrol et
cat test_minimal.mlp
```

### 3. Test Et
Her düzeltmeden sonra derleyiciyi test et:
```bash
./mlpc test_minimal.mlp /tmp/test.asm
# Başarılı mı?
```

---

## 📝 RAPOR ŞABLONU

Görev tamamlandığında şu formatta rapor ver:

```markdown
# Test Dosyası Düzeltme Raporu

## Düzeltilen Dosyalar:
1. test_minimal.mlp ✅
2. test_array_sum.mlp ✅
...

## Değişiklik Özeti:
- Toplam değişiklik: X satır
- end function → end_function: Y adet
- end while → end_while: Z adet
...

## Test Sonuçları:
- Başarı oranı: A% → B%
- Yeni başarılı testler: [liste]
- Hala başarısız: [liste] (nedenleri ile)

## Sorunlar:
- [varsa sorunlar]
```

---

## 🚀 BAŞLA

Görev hazır! Başlamak için:

1. Bu belgeyi oku
2. pmlp_kesin_sozdizimi.md'yi incele
3. Test dosyalarını düzelt
4. Test et
5. Rapor ver

**Başarılar!** 🎉
