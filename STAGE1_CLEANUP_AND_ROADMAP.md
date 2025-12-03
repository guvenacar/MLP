# MLP Stage 1 Temizlik ve Başlangıç Planı
**Tarih:** 3 Aralık 2025  
**Durum:** Temiz başlangıç için hazırlık

---

## 🗑️ Silinecek Dizinler (Gereksiz/Bozuk)

### 1. Yedek Dizinler (Git'te mevcut, gereksiz)
```bash
rm -rf melp_yedek_stage0/
rm -rf melp_yedek_stage1/
rm -rf melp_yedek_stage2/
rm -rf melp_yedek_stage2_final/
rm -rf melp_yedek_stage3/
```
**Sebep:** Git history'de zaten var, disk alanı kaplıyor

### 2. Monolitik Dizin (Bozuk/Karışık)
```bash
rm -rf melp_monolitik/
```
**Sebep:** 
- Diğer YZ'ler tarafından değiştirilmiş
- melpc binary çalışmıyor (test_minimal.mlp bile derlenmiyor)
- Karışık durum, temiz başlangıç için engel

### 3. Geçici Stage 1 Dosyaları (Eksik denemeler)
```bash
rm -rf melp/MLP/stage1/lexer.mlp
rm -rf melp/MLP/stage1/lexer.s
rm -rf melp/MLP/stage1/lexer.o
rm -rf melp/MLP/stage1/lexer
rm -rf melp/MLP/stage1/test_*
rm -rf melp/MLP/stage1/build_compiler.sh
```
**Sebep:** Yarım kalmış denemeler, yeniden yazılacak

---

## ✅ Kalacak Yapı (Temiz/Çalışan)

### 1. Stage 0 Modüller (64 Modül - %100 Tamamlanmış)
```
melp/C/stage0/modules/
├── arithmetic/
├── variable/
├── functions/
├── parser_core/
├── expression/
├── statement/
├── codegen_context/
└── ... (61 modül daha)
```
**Durum:** ✅ Temiz, test edilmiş, çalışıyor

### 2. Runtime
```
melp/C/runtime/runtime.c
melp/C/runtime/runtime.o
```
**Durum:** ✅ C runtime kalacak (performans için)

### 3. Dokümantasyon
```
MLP_LANGUAGE_SPEC.md
MODULAR_CHAIN_PROGRESS.md
STAGE1_PLAN.md (bu dosya)
```
**Durum:** ✅ Güncel ve doğru

---

## 🎯 Sonraki YZ için Yol Haritası

### ADIM 1: Stage 0 Modüllerden Compiler Build Et

**Hedef:** Çalışan bir C compiler (melpc2) oluştur

**Gerekli Modüller:**
1. `lexer` (yeni yazılacak - basit tokenizer)
2. `parser_core` (mevcut)
3. `expression` (mevcut)
4. `statement` (mevcut)
5. `variable` (mevcut)
6. `functions` (mevcut)
7. `codegen_context` (mevcut)
8. `arithmetic` (mevcut)
9. `comparison` (mevcut)
10. `logical` (mevcut)
11. `control_flow` (mevcut)
12. `print` (mevcut)

**Build Stratejisi:**
```bash
# 1. C ile minimal lexer.c yaz (200 satır)
# 2. main.c yaz - modülleri entegre et
# 3. Makefile oluştur
# 4. Derle: gcc -o melpc2 main.c lexer.c parser_core.c ... -lm
# 5. Test: ./melpc2 test.mlp test.s
```

### ADIM 2: MLP ile Lexer Yaz

**Hedef:** lexer.mlp (Stage 1 başlangıcı)

**Gereksinimler:**
- melpc2 ile derlenebilir olmalı
- Basit syntax (func, numeric, text, if/while)
- Global değişkenler literal değerlerle init

**Örnek:**
```mlp
-- lexer.mlp
function is_digit(text c) numeric
    if c == "0" then return 1 end if
    -- ...
    return 0
end function

function main() numeric
    text source = "numeric x = 42"
    -- tokenize...
    return 0
end function
```

**Test:**
```bash
./melpc2 lexer.mlp lexer.s
nasm -f elf64 lexer.s -o lexer.o
ld lexer.o runtime.o -o lexer -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2
./lexer
```

### ADIM 3: MLP ile Parser Yaz

**Hedef:** parser.mlp

**Strateji:**
- Recursive descent parser
- AST üretmeden direkt codegen (basitlik için)
- Veya basit AST (numeric array + text array)

### ADIM 4: MLP ile Codegen Yaz

**Hedef:** codegen.mlp

**Çıktı:** x86-64 NASM assembly

### ADIM 5: Self-Hosting Test

```bash
# MLP compiler ile kendini derle
./melpc_mlp lexer.mlp lexer.s
./melpc_mlp parser.mlp parser.s
./melpc_mlp codegen.mlp codegen.s
./melpc_mlp main.mlp main.s

# Link et
nasm -f elf64 lexer.s parser.s codegen.s main.s
ld ... -o melpc_v2

# Test: Kendini tekrar derle
./melpc_v2 lexer.mlp lexer2.s
# Başarılı ise: SELF-HOSTING COMPLETE! 🎉
```

---

## 📋 Kritik Notlar

### melpc Bozuk - Kullanma!
- `melp_monolitik/selfhost_from_c/melpc` binary'si çalışmıyor
- Basit dosyaları bile derleyemiyor
- YENİ compiler gerek: melpc2

### Runtime.c KALSIN
- C ile yazılmış runtime.c kullanmaya devam
- Performans kaybı istemiyoruz (%15-25)
- Python, Ruby, Node.js de C runtime kullanır
- Self-hosting ≠ Runtime'ı MLP ile yazmak

### Modüler Yapıyı Koru
- Her modül bağımsız (header, core, parser, codegen)
- Stage 0: 64 C modül ✅
- Stage 1: MLP compiler (lexer.mlp, parser.mlp, codegen.mlp)
- Stage 2: Self-hosting (MLP compiler kendini derler)

### Tümevarım Yaklaşımı
- Küçük başla, test et, genişlet
- Önce çalışan minimal örnek
- Sonra adım adım ekle
- Bug'a düşme, ilerlemeye odaklan

---

## 🚀 Hemen Şimdi Yapılacaklar

1. **Temizlik** (bu dosyayı çalıştır):
```bash
cd /home/pardus/projeler/MLP/MLP
rm -rf melp_yedek_* melp_monolitik
rm -rf melp/MLP/stage1/*
git add -A
git commit -m "chore: Clean obsolete directories, prepare for Stage 1 rebuild"
```

2. **C Lexer Yaz** (200 satır):
```bash
cd melp/MLP/stage1
# lexer.c oluştur (basit, C ile)
```

3. **Main.c Yaz** (100 satır):
```bash
# Stage 0 modülleri entegre et
```

4. **Build Script**:
```bash
# Makefile veya build.sh
```

5. **Test**:
```bash
./melpc2 test_simple.mlp test.s
```

---

## 📊 İlerleme Takibi

- [ ] Temizlik tamamlandı
- [ ] C lexer.c yazıldı
- [ ] main.c entegrasyonu tamamlandı
- [ ] melpc2 derlendi
- [ ] melpc2 basit test geçti
- [ ] MLP lexer.mlp yazıldı
- [ ] lexer.mlp melpc2 ile derlendi
- [ ] MLP parser.mlp yazıldı
- [ ] MLP codegen.mlp yazıldı
- [ ] Self-hosting test başarılı

---

**Sonraki YZ'ye Not:**  
Bu plan sana temiz bir yol haritası. Adım adım ilerle, acele etme.  
Her adımı test et. Çalışan kod > Karmaşık kod.  
Başarılar! 🚀
