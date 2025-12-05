# Stage 1: MLP Self-Hosting Bootstrap

**Son Güncelleme:** 3 Aralık 2025  
**Durum:** Temizlik yapıldı, yol haritası hazır

---

## 📍 Şu Anki Durum

### ✅ Tamamlanan
- **Stage 0:** 64 C modül yazıldı ve test edildi
- **Runtime:** C runtime.c mevcut ve çalışıyor
- **Dokümantasyon:** Tam ve güncel

### 🧹 Temizlenen
- Eski yedek dizinler (melp_yedek_*)
- Bozuk monolitik dizin (melp_monolitik)
- Yarım kalmış denemeler

### 📂 Temiz Yapı
```
melp/
├── C/
│   ├── stage0/
│   │   └── modules/        # 64 modül (✅ çalışıyor)
│   └── runtime/
│       └── runtime.c       # C runtime (✅ kalıcı)
└── MLP/
    └── stage1/
        ├── TEMPLATE_main.c # Başlangıç şablonu
        └── README.md       # Bu dosya
```

---

## 🎯 Sonraki Adımlar (Sırayla)

### 1. Basit C Compiler Yaz (melpc2)

**Hedef:** Minimal çalışan compiler

**Dosyalar:**
- `lexer.c` - Tokenizer (200 satır)
- `parser.c` - Parser (300 satır)
- `codegen.c` - x86-64 assembly generator (200 satır)
- `main.c` - Entry point (TEMPLATE_main.c'den başla)

**Build:**
```bash
gcc -o melpc2 main.c lexer.c parser.c codegen.c -lm
```

**Test:**
```bash
echo 'function main() numeric
    numeric x = 42
    return 0
end function' > test.mlp

./melpc2 test.mlp test.s
nasm -f elf64 test.s
ld test.o ../../C/runtime/runtime.o -o test -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2
./test && echo "✅ Works!"
```

### 2. MLP Lexer Yaz

**Hedef:** lexer.mlp (MLP dilinde)

**Syntax Kısıtlamaları (melpc2 için):**
- `function name(type arg) type` - Fonksiyon tanımı
- `numeric x = 42` - Değişken tanımı (literal değerle)
- `if cond then ... end if` - Kontrol yapısı
- `while cond do ... end while` - Döngü
- `return value` - Dönüş

**Örnek:**
```mlp
function is_digit(text c) numeric
    if c == "0" then return 1 end if
    if c == "1" then return 1 end if
    return 0
end function

function main() numeric
    text source = "x = 42"
    -- tokenize...
    return 0
end function
```

**Derleme:**
```bash
./melpc2 lexer.mlp lexer.s
nasm -f elf64 lexer.s
ld lexer.o ../../C/runtime/runtime.o -o lexer -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2
./lexer
```

### 3. MLP Parser Yaz

**Hedef:** parser.mlp

**Strateji:** Recursive descent, basit AST

### 4. MLP Codegen Yaz

**Hedef:** codegen.mlp

**Çıktı:** x86-64 NASM assembly

### 5. Self-Hosting Test

```bash
# MLP compiler MLP'de yazılmış dosyaları derler
./melpc_mlp lexer.mlp lexer.s
./melpc_mlp parser.mlp parser.s
./melpc_mlp codegen.mlp codegen.s

# Kendini derler
./melpc_mlp lexer.mlp lexer2.s
diff lexer.s lexer2.s  # Aynı mı?
# BAŞARILI ISE: SELF-HOSTING COMPLETE! 🎉
```

---

## 📚 Kaynaklar

### Önemli Dosyalar
- `STAGE1_CLEANUP_AND_ROADMAP.md` - Detaylı yol haritası
- `MLP_LANGUAGE_SPEC.md` - Dil spesifikasyonu
- `MODULAR_CHAIN_PROGRESS.md` - İlerleme takibi
- `cleanup_stage1.sh` - Temizlik scripti

### Stage 0 Modülleri
Her modül şu yapıda:
```
module_name/
├── module_name.h           # Header
├── module_name.c           # Core implementation
├── module_name_parser.c    # Parser integration
├── module_name_codegen.c   # Codegen integration
├── module_name_standalone.c # Test program
├── Makefile
└── test.mlp               # Test case
```

**Örnek Modüller:**
- `arithmetic` - +, -, *, /
- `variable` - Değişken tanımı ve kullanımı
- `functions` - Fonksiyon tanımı, çağırma
- `control_flow` - if/while/for
- `print` - Built-in print fonksiyonu

---

## ⚠️ Önemli Notlar

### melpc Binary Çalışmıyor!
Eski `melp_monolitik/selfhost_from_c/melpc` binary'si bozuk.  
**Kullanma!** YENİ compiler yaz: melpc2

### Runtime.c KALSIN
- C runtime performans ve güvenilirlik için gerekli
- Python, Ruby, Node.js de C runtime kullanır
- Self-hosting ≠ Runtime'ı MLP ile yazmak

### Tümevarım Yaklaşımı
1. En basit örnek yaz
2. Test et
3. Çalışırsa genişlet
4. Bug'a düşme, ilerlemeye odaklan

### Modüler Yapı Koru
- Her parça bağımsız test edilebilir
- Lexer → Parser → Codegen ayrı ayrı çalışır
- Stage 0 modüller örnek alınabilir

---

## 🚀 Hızlı Başlangıç

```bash
# 1. Bu dizine gel
cd /home/pardus/projeler/MLP/MLP/melp/MLP/stage1

# 2. TEMPLATE_main.c'yi main.c olarak kopyala
cp TEMPLATE_main.c main.c

# 3. Lexer.c yaz (basit başla)
nano lexer.c

# 4. Parser.c yaz
nano parser.c

# 5. Codegen.c yaz
nano codegen.c

# 6. Derle
gcc -o melpc2 main.c lexer.c parser.c codegen.c -lm

# 7. Test
echo 'function main() numeric
    return 0
end function' > test.mlp
./melpc2 test.mlp test.s

# 8. Çalıştır
nasm -f elf64 test.s
ld test.o ../../C/runtime/runtime.o -o test -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2
./test && echo "SUCCESS!"
```

---

**Başarılar!** 🚀  
Sorular için: STAGE1_CLEANUP_AND_ROADMAP.md'ye bak
