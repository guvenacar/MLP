# 🔗 MELP Stage 0 - Modüler Zincir Sistemi

## ⚠️ ÖNEMLİ: YENİ YZ BURADAN BAŞLA!

**Bu bir MODÜLER ZİNCİR projesidir!** Her modül kendi tam compiler'ını barındırır.

### 📋 İLK YAPILACAKLAR:

1. **İlerleme listesini oku:**
   ```bash
   cat MODULAR_CHAIN_PROGRESS.md
   ```

2. **Son durumu öğren:**
   - Hangi modüller tamamlandı? ✅
   - Sırada hangi modül var? 🟡
   - TODO'su olan modüller? 🔴

3. **Sıradaki modülü yap:**
   - Liste sırasına uy!
   - Şablonu kullan
   - Test et
   - Listeyi güncelle

---

## 🎯 PROJE VİZYONU

### ❌ ESKİ YÖNTEM (Monolitik - 10 kez başarısız oldu):
```
main.c (merkezi)
  ├─> parser.c (10,000+ satır)
  ├─> codegen.c (8,000+ satır)
  └─> Kaos! YZ kafası karışır!
```

### ✅ YENİ YÖNTEM (Modüler Zincir):
```
Her modül = TAM COMPILER
  
variable/
  ├─ variable_standalone.c → variable_compiler
  └─ .mlp → .s (ÇALIŞIR!)

functions/
  ├─ functions_standalone.c → functions_compiler
  └─ .mlp → .s (ÇALIŞIR!)

...

Son aşama: LEGO gibi birleştir!
```

---

## 📂 DİZİN YAPISI

```
stage0/
├── MODULAR_CHAIN_PROGRESS.md  ← İLERLEME LİSTESİ (ÖNEMLİ!)
├── README_MODULAR.md           ← Bu dosya
├── lexer.c, lexer.h            ← Ortak lexer
├── modules/
│   ├── variable/               ✅ TAMAMLANDI
│   │   ├── variable_standalone.c
│   │   ├── variable_compiler   (binary)
│   │   ├── Makefile
│   │   └── test_var.mlp
│   │
│   ├── comments/               🟡 SONRAKİ
│   │   └── (yapılacak)
│   │
│   ├── arithmetic/             🔴 BEKLEMEDE
│   ├── comparison/             🔴 BEKLEMEDE
│   ├── control_flow/           🔴 BEKLEMEDE
│   └── ...                     🔴 (34 modül daha)
```

---

## 🔧 YENİ MODÜL EKLEME (ŞABLONİ)

### Adım 1: Standalone Compiler Yaz

```c
// modules/[NAME]/[NAME]_standalone.c
#include <stdio.h>
#include "[name].h"
#include "[name]_parser.h"
#include "[name]_codegen.h"
#include "../../lexer.h"

int main(int argc, char** argv) {
    printf("🔗 [NAME] Module - Standalone Compiler\n");
    
    // 1. Read file
    // 2. Lexer
    // 3. Parser
    // 4. Codegen
    // 5. Output .s
    
    printf("✅ SUCCESS!\n");
    return 0;
}
```

### Adım 2: Makefile Oluştur

```makefile
CC = gcc
CFLAGS = -Wall -g -I../..
SOURCES = [name]_standalone.c [name].c [name]_parser.c [name]_codegen.c ../../lexer.c ../comments/comments.c
TARGET = [name]_compiler

all: $(TARGET)
clean:
    rm -f *.o $(TARGET) test*
test: $(TARGET)
    ./$(TARGET) test.mlp test.s
```

### Adım 3: Test Et

```bash
cd modules/[NAME]
make clean && make
echo "test code" > test.mlp
./[name]_compiler test.mlp test.s
nasm -f elf64 test.s -o test.o
ld test.o -o test_prog
./test_prog
```

### Adım 4: İlerleme Listesini Güncelle

```bash
# MODULAR_CHAIN_PROGRESS.md dosyasını aç
# Tamamlanan modülü ✅ yap
# Tüm detayları doldur
# Commit at
```

---

## ✅ TAMAMLANAN MODÜLLER

### 1. variable ✅
- **Binary:** `modules/variable/variable_compiler` (40 KB)
- **Test:** ✅ BAŞARILI
- **Çıktı:** Geçerli x86-64 assembly
- **Tarih:** 2 Aralık 2025

---

## 🎯 SONRAKİ MODÜL

### 2. comments 🟡
- **Dosya:** `modules/comments/comments_standalone.c`
- **Durum:** Henüz yazılmadı
- **Sıra:** SENİN!

**Başlamak için:**
```bash
cd modules/comments
# comments_standalone.c yaz (variable_standalone.c'yi örnek al)
# Makefile yaz
# Test et
# MODULAR_CHAIN_PROGRESS.md güncelle
```

---

## 🚨 YAPMAMAN GEREKENLER

1. ❌ **main.c düzenleme** - Her modül bağımsız!
2. ❌ **Merkezi codegen.c** - Her modül kendi codegen'ini kullanır!
3. ❌ **Modül sırasını atlama** - Listeye uy!
4. ❌ **Önceki modülleri değiştirme** - Sadece yeni modül ekle!
5. ❌ **Birleştirme** - Henüz değil! Önce tüm modülleri standalone yap!

---

## 📊 PROJE DURUMU

```
Tamamlanan:    1/37 modül  (%2.7)
Sıradaki:      comments
Kalan TODO=0:  16 modül
Kalan TODO>0:  20 modül
```

---

## 🎓 FELSEFE

> **"Her modül bir LEGO parçası - önce hepsini yap, sonra birleştir!"**

Monolitik yapı 10 kez başarısız oldu çünkü:
- Kod karmaşık
- Değişiklik riski yüksek
- YZ kafası karıştı
- Debug imkansız

Modüler zincir başarılı olacak çünkü:
- Her modül izole
- Test edilebilir
- Anlaşılır
- Birbirinden bağımsız

---

## 📞 YARDIM

Eğer takıldıysan:

1. **İlerleme listesini oku:** `MODULAR_CHAIN_PROGRESS.md`
2. **Örneklere bak:** `modules/variable/` (tamamlanmış)
3. **Şablonu kullan:** Bu README'deki şablon
4. **Test et:** Her modül çalışmalı!

---

**SON GÜNCELLEME:** 2 Aralık 2025, 23:30  
**PROJENİN SAHİBİ:** Güven ACAR  
**10. SELF-HOSTING DENEMESİ - YENİ YAKLAŞIM**

**Başarı şansımız:** %90+ (önceki denemelerden ders aldık!)
