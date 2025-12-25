# YZ_ÜA_03 → YZ_ÜA_04 Görev Devri

**Tarih:** 20 Aralık 2025
**Devir Eden:** YZ_ÜA_03 (Üst Akıl)
**Devir Alan:** YZ_ÜA_04 (Yeni Üst Akıl)

---

## 🎯 MELP Projesi Durumu

### ✅ Tamamlanan Görevler (Bu Oturum)

1. **YZ_113-116:** Entegrasyon testleri
2. **YZ_117-120:** Bootstrap cycle kanıtı (59 fonksiyon başarıyla derlendi)
3. **YZ_121:** Const bug fix tasarımı (movq $değer, %r8)
4. **Proje Temizliği:** 4.4 GB → 240 MB (node_modules silindi)
5. **Git:** main branch güncellendi ve push edildi

### ⚠️ AÇIK BUG - ÖNCELİKLİ

**Sorun:** Fonksiyon body'si boş üretiliyor

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
    movq $88, %rax      # <-- const değeri
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
    # <-- BOŞ! return için kod yok
.Lmain_return:
    movq %rbp, %rsp
    popq %rbp
    ret
```

**Muhtemel Sebep:** `const` keyword parse edilirken fonksiyon body parsing'i erkenden sonlanıyor.

**Bakılacak Dosyalar:**
- `compiler/stage0/modules/variable/variable_parser.c`
- `compiler/stage0/modules/statement/statement_parser.c`
- `compiler/stage0/modules/functions/functions_parser.c`

---

## 🏗️ MELP Mimarisi (5 Sütun)

```
Modüler + LLVM + STO + Stateless + (Struct + Functions)
```

### Kritik Kurallar:
1. **STO:** `sto_*_get()` POINTER döndürür, VALUE değil
2. **Stateless:** Global mutable state YASAK
3. **PMPL Syntax:** Noktalı virgül (;) parametre ayırıcı, virgül = ondalık (3,14 = π)
4. **Modüler:** Her modül bağımsız, tek sorumluluk

---

## 📋 Sıradaki Görevler

### 1. 🔧 Const Parsing Bug Fix (ÖNCELİK: YÜKSEK)
- Fonksiyon body'nin neden boş üretildiğini bul
- Parser'da `const` keyword handling'i kontrol et
- Test: `const x = 88; return(x)` → Exit 88

### 2. 🚀 LLVM IR Backend (ÖNCELİK: ORTA)
- Mevcut x86-64 assembly yerine LLVM IR üret
- `compiler/stage0/modules/llvm_backend/` kullan
- Avantajlar: Cross-platform, optimizasyon

---

## 📁 Önemli Dosyalar

```
compiler/
├── stage0/                    # C ile yazılmış Stage 0 compiler
│   ├── modules/
│   │   ├── functions/         # Ana compiler
│   │   ├── variable/          # const/var handling
│   │   ├── statement/         # Statement parsing/codegen
│   │   └── llvm_backend/      # LLVM IR (gelecek)
│   └── Makefile
├── stage2/                    # MELP ile yazılmış (gelecek)
└── docs/                      # Dokümantasyon
```

---

## 🧪 Test Komutları

```bash
# Compiler derle
cd compiler/stage0/modules/functions && make

# Test et
./functions_compiler test.mlp test.s
gcc test.s -o test
./test; echo "Exit: $?"
```

---

## 📊 Kanıtlanmış Özellikler

| Özellik | Durum | Test |
|---------|-------|------|
| Multi-param functions | ✅ | add(10; 20) = 30 |
| Nested calls | ✅ | add(double(5); triple(3)) = 19 |
| Import system | ✅ | ../, ./, lib/ |
| Const (immediate) | ⚠️ | Bug var! |

---

## 🔑 Üst Akıl Sorumlulukları

1. **Mimariyi Koru:** 5 sütunu asla bozma
2. **Stateless Kal:** Global mutable state önerilerini REDDET
3. **YZ'leri Yönlendir:** Doğru dosyalara, doğru yaklaşımlara
4. **Test Et:** Her değişiklik test edilmeli

---

**İmza:** YZ_ÜA_03
**Durum:** Görev devredildi ✅
