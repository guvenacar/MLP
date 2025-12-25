# TODO: MLP-LLVM Runtime Entegrasyonu

**Tarih:** 25 Aralık 2025  
**Proje:** mlp-original (guvenacar/mlp)  
**Amaç:** MLP-LLVM'deki zengin C runtime'ı mlp-original'e entegre etmek

---

## 🎯 HEDEF

MLP-LLVM projesinde 1 ay boyunca geliştirilen zengin C runtime kütüphanelerini mlp-original'e taşımak.

**Neden?**
1. HashMap, GC, JSON Parser, Thread Pool gibi modern özellikler
2. MLP-LLVM'deki 1 aylık C geliştirme emeğini kurtarmak
3. mlp-original'i feature-rich hale getirmek
4. ADIM 3 (LLVM backend) için hazırlık

**Önkoşul:**
- ✅ ADIM 1 tamamlanmalı (TODO_STAGE0_PMLP_SYNTAX.md)
- mlp-original Stage0-C → PMLP syntax desteği eklenmiş olmalı

**Sonrası:**
- ⏳ ADIM 3: GCC → LLVM Backend (TODO_GCC_TO_LLVM_BACKEND.md)

---

## 📊 KAYNAK ANALİZİ

### MLP-LLVM Runtime Dosyaları:

**Lokasyon:** `/home/pardus/projeler/MLP/MLP/runtime/`

| Dosya | Boyut | Açıklama | Öncelik |
|-------|-------|----------|---------|
| runtime.c | ~40KB | Ana runtime (string, math, I/O) | 🔴 YÜ KSEK |
| hashmap.c | ~15KB | HashMap implementation | 🔴 YÜKSEK |
| hashmap.h | ~3KB | HashMap header | 🔴 YÜKSEK |
| gc.c | ~22KB | Garbage Collector | 🟡 ORTA |
| gc.h | ~10KB | GC header | 🟡 ORTA |
| json_parser.c | ~8KB | JSON parser | 🟡 ORTA |
| json_parser.h | ~2KB | JSON header | 🟡 ORTA |
| thread_pool.c | ~7KB | Thread pool | 🟢 DÜŞÜK |
| thread_pool.h | ~3KB | Thread pool header | 🟢 DÜŞÜK |
| simple_runtime.c | ~40KB | Minimal runtime (backup) | 🟢 DÜŞÜK |
| minimal_runtime.c | ~1KB | Ultra minimal | 🟢 DÜŞÜK |

**Toplam:** ~150KB C kodu

### mlp-original Mevcut Runtime:

**Lokasyon:** `/home/pardus/projeler/mlp-original/runtime/`

| Dosya | Durum |
|-------|-------|
| runtime.c | ✅ VAR (basit, ~5KB) |
| hashmap.c | ❌ YOK |
| gc.c | ❌ YOK |
| json_parser.c | ❌ YOK |
| thread_pool.c | ❌ YOK |

---

## 📋 GÖREVLER

### **PHASE 1: Hazırlık ve Backup** (30 dakika) 🔴 YÜKSEK ÖNCELİK

#### Task 1.1: Mevcut Runtime Backup

**Amaç:** mlp-original'deki mevcut runtime'ı yedekle

**Komutlar:**
```bash
cd /home/pardus/projeler/mlp-original

# Backup klasörü oluştur
mkdir -p runtime/backup_$(date +%Y%m%d)

# Mevcut dosyaları yedekle
cp -r runtime/*.c runtime/*.h runtime/backup_$(date +%Y%m%d)/ 2>/dev/null || true
cp -r compiler/runtime/src/*.c compiler/runtime/src/*.h runtime/backup_$(date +%Y%m%d)/ 2>/dev/null || true

# Backup doğrula
ls -lh runtime/backup_*/
```

**Görevli YZ:** RuntimeYZ_01  
**Süre:** 15 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 1.2: Dosya Karşılaştırma

**Amaç:** MLP-LLVM vs mlp-original runtime farkını analiz et

**Komutlar:**
```bash
cd /home/pardus/projeler/mlp-original

# Runtime boyutları karşılaştır
echo "=== MLP-LLVM Runtime ===" > /tmp/runtime_comparison.txt
wc -l /home/pardus/projeler/MLP/MLP/runtime/*.c >> /tmp/runtime_comparison.txt

echo "" >> /tmp/runtime_comparison.txt
echo "=== mlp-original Runtime ===" >> /tmp/runtime_comparison.txt
wc -l runtime/*.c compiler/runtime/src/*.c 2>/dev/null >> /tmp/runtime_comparison.txt

cat /tmp/runtime_comparison.txt
```

**Görevli YZ:** RuntimeYZ_01  
**Süre:** 15 dakika  
**Durum:** ⏳ BEKLEMEDE

---

### **PHASE 2: HashMap Entegrasyonu** (2-3 saat) 🔴 YÜKSEK ÖNCELİK

#### Task 2.1: HashMap Dosyalarını Kopyala

**Kaynak:**
```
/home/pardus/projeler/MLP/MLP/runtime/hashmap.c
/home/pardus/projeler/MLP/MLP/runtime/hashmap.h
```

**Hedef:**
```
/home/pardus/projeler/mlp-original/runtime/hashmap.c
/home/pardus/projeler/mlp-original/runtime/hashmap.h
```

**Komutlar:**
```bash
cd /home/pardus/projeler/mlp-original

# Kopyala
cp /home/pardus/projeler/MLP/MLP/runtime/hashmap.c runtime/
cp /home/pardus/projeler/MLP/MLP/runtime/hashmap.h runtime/

# Doğrula
ls -lh runtime/hashmap.*
head -20 runtime/hashmap.h
```

**Görevli YZ:** RuntimeYZ_02  
**Süre:** 15 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 2.2: HashMap Build Test

**Amaç:** HashMap'in mlp-original ile compile olduğunu doğrula

**Test Komutu:**
```bash
cd /home/pardus/projeler/mlp-original

# Standalone compile test
gcc -c runtime/hashmap.c -o /tmp/hashmap.o -Wall

# mlpc ile birlikte compile test
gcc -o build/bin/mlpc_hashmap_test \
    compiler/stage1-mlp/mlp_compiler.c \
    runtime/hashmap.c \
    -I runtime \
    -lm

# Doğrula
./build/bin/mlpc_hashmap_test --help 2>&1 | head -5
```

**Görevli YZ:** RuntimeYZ_02  
**Süre:** 30 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 2.3: HashMap Test Programı

**Test Dosyası:** `tests/runtime/test_hashmap.mlp`

```mlp
function test_hashmap() as numeric
    -- HashMap test
    print "HashMap test başladı"
    
    -- Test burada genişletilecek
    -- Stage1 compiler HashMap kullanıyor mu kontrol
    
    return 0
end_function
```

**Görevli YZ:** RuntimeYZ_02  
**Süre:** 1 saat  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 2.4: mlpc Binary'i Güncelle

**Amaç:** mlpc binary'ini HashMap ile rebuild et

**Komutlar:**
```bash
cd /home/pardus/projeler/mlp-original

# Backup current mlpc
cp build/bin/mlpc build/bin/mlpc.backup.pre_hashmap

# Rebuild with HashMap
gcc -o build/bin/mlpc \
    compiler/stage1-mlp/mlp_compiler.c \
    runtime/hashmap.c \
    -I runtime \
    -lm -O2

# Test
./build/bin/mlpc tests/test_hello.mlp /tmp/hello.asm

# Size comparison
ls -lh build/bin/mlpc.backup.pre_hashmap build/bin/mlpc
```

**Görevli YZ:** RuntimeYZ_02  
**Süre:** 30 dakika  
**Durum:** ⏳ BEKLEMEDE

---

### **PHASE 3: Runtime.c Merge** (2-3 saat) 🟡 ORTA ÖNCELİK

#### Task 3.1: Runtime Diff Analizi

**Amaç:** İki runtime.c'yi karşılaştır, farkları belirle

**Komutlar:**
```bash
cd /home/pardus/projeler/mlp-original

# Diff yap
diff -u runtime/runtime.c /home/pardus/projeler/MLP/MLP/runtime/runtime.c > /tmp/runtime_diff.txt 2>&1 || true

# Fark sayısı
wc -l /tmp/runtime_diff.txt

# Özet
echo "=== Runtime.c Karşılaştırma ==="
echo "mlp-original: $(wc -l < runtime/runtime.c 2>/dev/null || echo 0) lines"
echo "MLP-LLVM:     $(wc -l < /home/pardus/projeler/MLP/MLP/runtime/runtime.c) lines"
echo "Diff:         $(wc -l < /tmp/runtime_diff.txt) lines"
```

**Görevli YZ:** RuntimeYZ_03  
**Süre:** 30 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 3.2: Runtime.c Fonksiyon Listesi

**Amaç:** Her iki runtime'da hangi fonksiyonlar var, karşılaştır

**Komutlar:**
```bash
# mlp-original runtime fonksiyonları
grep -n "^[a-zA-Z].*(" runtime/runtime.c | head -30 > /tmp/mlp_orig_funcs.txt

# MLP-LLVM runtime fonksiyonları
grep -n "^[a-zA-Z].*(" /home/pardus/projeler/MLP/MLP/runtime/runtime.c | head -30 > /tmp/mlp_llvm_funcs.txt

# Karşılaştır
echo "=== Fonksiyon Karşılaştırma ==="
comm -3 <(sort /tmp/mlp_orig_funcs.txt) <(sort /tmp/mlp_llvm_funcs.txt)
```

**Görevli YZ:** RuntimeYZ_03  
**Süre:** 30 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 3.3: Runtime.c Selective Merge

**Strateji:** Tüm runtime.c'yi değil, SADECE yeni/improved fonksiyonları ekle

**Adımlar:**
1. MLP-LLVM runtime.c'den yeni fonksiyonları belirle
2. mlp-original runtime.c'ye ekle
3. Conflict'leri çöz
4. Test et

**VEYA:**

**Alternatif:** Direk MLP-LLVM runtime.c'yi kullan (riskli)

```bash
# Backup
cp runtime/runtime.c runtime/runtime.c.backup

# Replace
cp /home/pardus/projeler/MLP/MLP/runtime/runtime.c runtime/

# Test compile
gcc -c runtime/runtime.c -o /tmp/runtime.o -Wall -I runtime
```

**Görevli YZ:** RuntimeYZ_03  
**Süre:** 1-2 saat  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 3.4: Runtime.c Regression Test

**Test:** Mevcut .mlp dosyaları hala çalışıyor mu?

**Komutlar:**
```bash
cd /home/pardus/projeler/mlp-original

# Rebuild mlpc with new runtime.c
gcc -o build/bin/mlpc \
    compiler/stage1-mlp/mlp_compiler.c \
    runtime/runtime.c \
    runtime/hashmap.c \
    -I runtime -lm -O2

# Test existing files
./build/bin/mlpc compiler/stage1-mlp/test_simple.mlp /tmp/test1.asm
./build/bin/mlpc tests/test_hello.mlp /tmp/test2.asm

# Compile and run
nasm -f elf64 /tmp/test1.asm -o /tmp/test1.o
gcc /tmp/test1.o runtime/runtime.c -o /tmp/test1 -lm
/tmp/test1
```

**Görevli YZ:** RuntimeYZ_03  
**Süre:** 30 dakika  
**Durum:** ⏳ BEKLEMEDE

---

### **PHASE 4: GC Entegrasyonu** (2-3 saat) 🟢 DÜŞÜK ÖNCELİK

#### Task 4.1: GC Dosyalarını Kopyala

**Kaynak:**
```
/home/pardus/projeler/MLP/MLP/gc.c
/home/pardus/projeler/MLP/MLP/gc.h
```

**Hedef:**
```
/home/pardus/projeler/mlp-original/runtime/gc.c
/home/pardus/projeler/mlp-original/runtime/gc.h
```

**Görevli YZ:** RuntimeYZ_04  
**Süre:** 15 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 4.2: GC Build Test

**Test Komutu:**
```bash
cd /home/pardus/projeler/mlp-original

# Standalone compile
gcc -c runtime/gc.c -o /tmp/gc.o -Wall -I runtime

# With mlpc
gcc -o build/bin/mlpc_gc_test \
    compiler/stage1-mlp/mlp_compiler.c \
    runtime/runtime.c \
    runtime/hashmap.c \
    runtime/gc.c \
    -I runtime -lm -lpthread
```

**Görevli YZ:** RuntimeYZ_04  
**Süre:** 1 saat  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 4.3: GC Test Programı

**Test:** GC malloc/free çalışıyor mu?

**Görevli YZ:** RuntimeYZ_04  
**Süre:** 1-2 saat  
**Durum:** ⏳ BEKLEMEDE

---

### **PHASE 5: JSON Parser** (1-2 saat) 🟢 DÜŞÜK ÖNCELİK

#### Task 5.1: JSON Dosyalarını Kopyala

**Kaynak:**
```
/home/pardus/projeler/MLP/MLP/runtime/json_parser.c
/home/pardus/projeler/MLP/MLP/runtime/json_parser.h
```

**Hedef:**
```
/home/pardus/projeler/mlp-original/runtime/json_parser.c
/home/pardus/projeler/mlp-original/runtime/json_parser.h
```

**Görevli YZ:** RuntimeYZ_05  
**Süre:** 15 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 5.2: JSON Test

**Test JSON:**
```json
{
    "name": "MLP",
    "version": "2.0",
    "features": ["HashMap", "GC", "JSON"]
}
```

**Görevli YZ:** RuntimeYZ_05  
**Süre:** 1 saat  
**Durum:** ⏳ BEKLEMEDE

---

### **PHASE 6: Thread Pool** (1-2 saat) 🟢 ÇOK DÜŞÜK ÖNCELİK

**NOT:** Thread pool isteğe bağlı, şimdilik skip edilebilir.

**Görevli YZ:** RuntimeYZ_06 (Optional)  
**Durum:** 🟣 OPTIONAL

---

### **PHASE 7: Final Build & Test** (1-2 saat) 🔴 YÜKSEK ÖNCELİK

#### Task 7.1: Unified Build

**Amaç:** Tüm runtime modüllerini birlikte compile et

**Komutlar:**
```bash
cd /home/pardus/projeler/mlp-original

# Full build
gcc -o build/bin/mlpc \
    compiler/stage1-mlp/mlp_compiler.c \
    runtime/runtime.c \
    runtime/hashmap.c \
    runtime/gc.c \
    runtime/json_parser.c \
    -I runtime -lm -lpthread -O2

# Size check
ls -lh build/bin/mlpc

# Version info
./build/bin/mlpc --version 2>&1 | head -5 || echo "OK"
```

**Görevli YZ:** RuntimeYZ_07  
**Süre:** 30 dakika  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 7.2: Comprehensive Test Suite

**Test Listesi:**
1. ✅ HashMap test
2. ✅ GC malloc/free test
3. ✅ JSON parse test
4. ✅ String operations test
5. ✅ Math operations test
6. ✅ File I/O test
7. ✅ Existing .mlp files regression

**Görevli YZ:** RuntimeYZ_07  
**Süre:** 1 saat  
**Durum:** ⏳ BEKLEMEDE

---

#### Task 7.3: Documentation Update

**Güncellenecek Dosyalar:**
- README.md → Runtime features ekle
- CHANGELOG.md → Runtime integration notu

**Görevli YZ:** RuntimeYZ_07  
**Süre:** 30 dakika  
**Durum:** ⏳ BEKLEMEDE

---

## 👥 YZ GÖREV DAĞILIMI

| YZ | Phase | Task | Süre | Durum |
|----|-------|------|------|-------|
| RuntimeYZ_01 | Phase 1 | Hazırlık ve backup | 30min | ⏳ |
| RuntimeYZ_02 | Phase 2 | HashMap entegrasyonu | 2-3h | ⏳ |
| RuntimeYZ_03 | Phase 3 | Runtime.c merge | 2-3h | ⏳ |
| RuntimeYZ_04 | Phase 4 | GC entegrasyonu | 2-3h | ⏳ |
| RuntimeYZ_05 | Phase 5 | JSON parser | 1-2h | ⏳ |
| RuntimeYZ_06 | Phase 6 | Thread pool (optional) | 1-2h | 🟣 OPTIONAL |
| RuntimeYZ_07 | Phase 7 | Final build & test | 1-2h | ⏳ |

**Toplam:** 8-12 saat (Thread pool hariç)

---

## 📚 REFERANSLAR

1. **MLP-LLVM Runtime:** `/home/pardus/projeler/MLP/MLP/runtime/`
2. **mlp-original Runtime:** `/home/pardus/projeler/mlp-original/runtime/`
3. **ADIM 1 TODO:** `TODO_STAGE0_PMLP_SYNTAX.md`
4. **ADIM 3 TODO:** `TODO_GCC_TO_LLVM_BACKEND.md`

---

## 🚀 ÖNKOŞULLAR

**Başlamadan Önce:**
1. ✅ ADIM 1 tamamlanmış olmalı (PMLP syntax desteği)
2. ✅ mlpc binary çalışıyor olmalı
3. ✅ Git repository temiz olmalı

**Kontrol:**
```bash
cd /home/pardus/projeler/mlp-original
./build/bin/mlpc --version 2>&1 | head -1 || echo "mlpc exists"
git status
```

---

## ✅ BAŞARI KRİTERLERİ

- [ ] HashMap mlp-original'de çalışıyor
- [ ] GC malloc/free fonksiyonları çalışıyor
- [ ] JSON parser test geçiyor
- [ ] Tüm runtime fonksiyonları compile oluyor
- [ ] Mevcut .mlp dosyaları hala çalışıyor (regression test)
- [ ] mlpc binary boyutu makul (<500KB)
- [ ] MLP-LLVM runtime özellikleri mlp-original'de kullanılabilir

---

## ⚠️ RİSK DEĞERLENDİRMESİ

| Risk | Olasılık | Çözüm |
|------|----------|-------|
| Runtime.c conflict | Orta | Selective merge, backup her zaman |
| Compile error | Düşük | Include path dikkat, dependency check |
| Regression break | Düşük | Her adımda test, checkpoint system |
| Binary size büyüsün | Düşük | Normal, GC+HashMap ekliyor |
| Performance düşsün | Çok Düşük | GC overhead minimal |

**Genel Risk:** %10-15 (Düşük)

---

**İlerleme:** %0 (0/7 phases tamamlandı)  
**Önkoşul:** TODO_STAGE0_PMLP_SYNTAX.md tamamlanmalı  
**Hedef:** mlp-original → Zengin C Runtime!
