# YZ_00: BUG-001 Kök Sebep Analizi

**Tarih:** 20 Aralık 2025  
**Atanan:** YZ_00 (Görevli)  
**Üst Akıl:** YZ_ÜA_04  
**Öncelik:** 🔴 YÜKSEK

---

## 🎯 Görev

**BUG-001: Fonksiyon body boş üretiliyor** sorununu analiz et ve kök sebebini bul.

---

## 🐛 Bug Detayları

**Kod:**
```mlp
function main() as int {
    const x = 88
    return(x)
}
```

**Beklenen Assembly:**
```asm
main:
    pushq %rbp
    movq %rsp, %rbp
    movq $88, %rax      # const x = 88
.Lmain_return:
    movq %rbp, %rsp
    popq %rbp
    ret
```

**Gerçek Assembly:**
```asm
main:
    pushq %rbp
    movq %rsp, %rbp
    # <-- BOŞ! const ve return için kod yok
.Lmain_return:
    movq %rbp, %rsp
    popq %rbp
    ret
```

---

## 📋 Yapılacaklar

### 1. Bug'ı Doğrula
- [ ] Test dosyası oluştur: `tests/manual/bug_001_const_return.mlp`
- [ ] Derle ve assembly çıktısını kaydet
- [ ] Body'nin gerçekten boş olduğunu doğrula

### 2. Compiler Kodunu İncele

**İncelenecek Dosyalar:**
- `compiler/stage0/modules/variable/variable_parser.c`
  - `const` keyword nasıl parse ediliyor?
  - Variable declaration sonrası parsing devam ediyor mu?
  
- `compiler/stage0/modules/statement/statement_parser.c`
  - Statement parsing fonksiyonu nedir?
  - `const` keyword tanınıyor mu?
  - Return statement parsing çağrılıyor mu?
  
- `compiler/stage0/modules/functions/functions_parser.c`
  - Function body parsing döngüsü nasıl çalışıyor?
  - Hangi durumlarda döngü sonlanıyor?
  - `}` görülene kadar devam ediyor mu?

### 3. Kök Sebep Analizi

**Muhtemel Sebepler:**
- [ ] `const` keyword statement parser tarafından tanınmıyor
- [ ] Variable parser `const` sonrası statement parsing'i durduruyor
- [ ] Function body parser `const` gördüğünde erkenden çıkıyor
- [ ] Return statement parser çağrılmıyor

**Bulunacak:**
- Hangi satırda/fonksiyonda parsing sonlanıyor?
- Neden `return` statement'a gelinmiyor?
- Bug fix için hangi kod değişmeli?

### 4. Rapor

**Oluşturulacak:** `stage1_bug_YZ/YZ_00.md`

**İçerik:**
- Bug doğrulama sonuçları
- İncelenen dosyalar ve bulgular
- Kök sebep (hangi satır/fonksiyon)
- Önerilen çözüm (pseudo-code seviyesinde)
- Test planı

---

## 🧪 Test Komutu

```bash
# Compiler'ı bul ve derle
cd compiler/stage0/modules/functions
make

# Test dosyasını derle
./functions_compiler ../../../../tests/manual/bug_001_const_return.mlp \
                     ../../../../temp/bug_001.s

# Assembly'yi incele
cat ../../../../temp/bug_001.s

# Çalıştır
gcc ../../../../temp/bug_001.s -o ../../../../temp/bug_001
../../../../temp/bug_001
echo "Exit code: $?"
```

**Beklenen:** Exit code: 88

---

## 📁 Referanslar

- `stage1_bug_YZ/BUG_LIST.md` - Bug detayları
- `ust_akil_YZ/YZ_UA_03_DEVIR.md` - Önceki analiz
- `ARCHITECTURE.md` - MELP mimarisi (5 ayak)

---

## ✅ Tamamlanma Kriterleri

- [ ] Bug doğrulandı (test ile)
- [ ] Kök sebep bulundu (hangi satır/fonksiyon)
- [ ] Çözüm önerildi (pseudo-code)
- [ ] Rapor yazıldı (YZ_00.md)
- [ ] Üst Akıl'e bildirim yapıldı

---

**Üst Akıl:** YZ_ÜA_04  
**Durum:** ⏳ Atandı
