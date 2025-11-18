# MLP - Multi-Language Programming

> **"Write Once, Any Syntax - Bir Kez Yaz, İstediğin Dilde Oku!"** 🌍

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-blue)]()
[![Status](https://img.shields.io/badge/status-Self--Hosting%20✅-success)]()
[![Compiler](https://img.shields.io/badge/compiler-C%20Bootstrap%20%2B%20MLP-orange)]()

---

## 🎉 Proje Durumu: SELF-HOSTING BAŞARILI!

**MLP artık kendi kendini derleyebilen bir programlama dili!** 🚀

- ✅ **Bootstrap Compiler** (C dilinde): Tam çalışır durumda
- ✅ **Self-Hosting Compiler** (MLP dilinde): 927 satır kod
- ✅ **x86-64 Assembly Üretimi**: NASM syntax
- ✅ **Struct Desteği**: YAPI keyword ile yapılandırılmış veri
- ✅ **Arrays, Functions, Loops**: Tam functional
- 🚧 **Multi-Language Support**: Türkçe, English, हिन्दी, 中文... (Geliştiriliyor)

---

## 🎯 MLP Nedir?

**MLP (Multi-Language Programming)**, herhangi bir doğal dilde (Türkçe, İngilizce, Hintçe, Çince vb.) kod yazmanızı sağlayan, **dil-bağımsız** bir programlama dilidir.

### 🌟 Temel Felsefe

> "Bir Türk developer SINIF yazsın, bir Hintli developer class yazsın, bir Çinli developer 类 yazsın - **hepsi aynı programı yazıyor!**"

### Aynı Program, Farklı Diller

**Türkçe:**
```mlp
İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON
    DÖNÜŞ fibonacci(n-1) + fibonacci(n-2)
SON
```

**English:**
```mlp
function fibonacci(n) {
    if (n <= 1) {
        return n
    }
    return fibonacci(n-1) + fibonacci(n-2)
}
```

**हिन्दी (Hindi):**
```mlp
कार्य fibonacci(n) है
    यदि n <= 1 है
        वापसी n
    समाप्त
    वापसी fibonacci(n-1) + fibonacci(n-2)
समाप्त
```

**Her üçü de aynı assembly koduna derlenir!** ✨

---

## 🏗️ Mimari: Language-Agnostic Compiler

```
┌─────────────────────────────────────────┐
│  Kullanıcı Kodu (Herhangi Bir Dil)      │
│  Türkçe:  SINIF Kişi İSE ... SON        │
│  English: class Person { ... }          │
│  हिन्दी:   वर्ग व्यक्ति { ... }         │
└─────────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│  MLP Preprocessor (Language Normalizer) │
│  - UTF-8/Unicode aware                  │
│  - Auto language detection              │
│  - Keyword mapping (config/lang/*.yaml) │
└─────────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│  Normalized Internal Syntax (Turkish)   │
│  İŞLEÇ ... İSE ... SON                  │
└─────────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│  MLP Compiler (C Bootstrap)             │
│  Lexer → Parser → Generator → ASM       │
└─────────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│  x86-64 Assembly (NASM)                 │
└─────────────────────────────────────────┘
                 ↓
┌─────────────────────────────────────────┐
│  Native Binary Executable               │
└─────────────────────────────────────────┘
```

---

## ⚡ Hızlı Başlangıç

### Gereksinimler
- GCC (C compiler)
- NASM (Assembly)
- Linux veya macOS

### Derleme ve Çalıştırma

```bash
# Repo'yu klonla
git clone https://github.com/guvenacar/MLP.git
cd MLP

# Bootstrap compiler'ı derle
gcc -c c_compiler/c_lexer.c -o c_lexer.o
gcc -c c_compiler/c_parser.c -o c_parser.o
gcc -c c_compiler/c_generator.c -o c_generator.o
gcc -c c_compiler/main.c -o main.o
gcc -c runtime/runtime.c -o runtime.o
gcc c_lexer.o c_parser.o c_generator.o main.o runtime.o -o mlp_compiler -no-pie

# Örnek program derle (Türkçe)
./mlp_compiler ornekler/merhaba.mlp merhaba.asm

# Assembly'i binary'ye çevir
nasm -f elf64 merhaba.asm -o merhaba.o
gcc merhaba.o runtime.o -o merhaba -no-pie

# Çalıştır
./merhaba
```

---

## 📚 Desteklenen Diller (Language Support)

### ✅ Aktif Desteklenen
- 🇹🇷 **Türkçe** (Turkish) - Tam destek
- 🇬🇧 **English** - Tam destek

### 🚧 Geliştiriliyor
- 🇮🇳 **हिन्दी (Hindi)** - Preprocessor geliştiriliyor
- 🇨🇳 **中文 (Chinese)** - Planlı
- 🇯🇵 **日本語 (Japanese)** - Planlı
- 🇪🇸 **Español (Spanish)** - Planlı
- 🇫🇷 **Français (French)** - Planlı
- 🇩🇪 **Deutsch (German)** - Planlı
- 🇷🇺 **Русский (Russian)** - Planlı
- 🇦🇪 **العربية (Arabic)** - Planlı

**Kendi dilinizi eklemek ister misiniz?** `config/lang/` altına YAML dosyası ekleyerek kendi dilinizi destekleyebilirsiniz!

---

## 🎨 Söz Dizimi Özellikleri

### Veri Tipleri (Data Types)

| Türkçe | English | हिन्दी | Tip |
|--------|---------|--------|-----|
| SAYISAL | number | संख्या | 64-bit integer |
| METIN | string | पाठ | String (char*) |
| BOOL | bool | बूल | Boolean |

### Kontrol Yapıları (Control Flow)

| Türkçe | English | हिन्दी | Açıklama |
|--------|---------|--------|----------|
| EĞER ... İSE | if ... then | यदि ... है | Conditional |
| DEĞİLSE | else | अन्यथा | Else |
| SON | end | समाप्त | End block |
| DÖNGÜ | loop | लूप | Loop |
| DÖNGÜ_BITIR | break | तोड़ो | Break |

### Fonksiyonlar (Functions)

| Türkçe | English | हिन्दी | Açıklama |
|--------|---------|--------|----------|
| İŞLEÇ ... İSE | function ... { | कार्य ... है | Function def |
| DÖNÜŞ | return | वापसी | Return |

---

## 📖 Örnekler (Examples)

### 1. Merhaba Dünya (Hello World)

**Türkçe:**
```mlp
YAZDIR "Merhaba Dünya!"
```

**English:**
```mlp
print("Hello World!")
```

**हिन्दी:**
```mlp
लिखो "नमस्ते दुनिया!"
```

### 2. Fibonacci Hesaplama

**Türkçe:**
```mlp
İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON
    SAYISAL a = fibonacci(n - 1);
    SAYISAL b = fibonacci(n - 2);
    DÖNÜŞ a + b
SON

SAYISAL sonuc = fibonacci(10);
YAZDIR "Fibonacci(10) = "
YAZDIR sonuc
```

**English:**
```mlp
function fibonacci(n) {
    if (n <= 1) {
        return n
    }
    number a = fibonacci(n - 1);
    number b = fibonacci(n - 2);
    return a + b
}

number result = fibonacci(10);
print("Fibonacci(10) = ")
print(result)
```

### 3. Struct/Yapı Kullanımı

**Türkçe:**
```mlp
YAPI Nokta İSE
    SAYISAL x;
    SAYISAL y;
SON

YAZDIR "Nokta yapısı tanımlandı"
```

**English:**
```mlp
struct Point {
    number x;
    number y;
}

print("Point struct defined")
```

---

## 🌍 Kendi Dilinizi Ekleyin!

MLP'ye yeni bir dil eklemek çok kolay! Sadece bir YAML dosyası oluşturun:

### Örnek: `config/lang/hindi.yaml`

```yaml
language:
  name: "Hindi"
  code: "hi"
  native_name: "हिन्दी"
  encoding: "UTF-8"

keywords:
  # Data types
  SAYISAL: "संख्या"
  METIN: "पाठ"
  BOOL: "बूल"

  # Control flow
  EĞER: "यदि"
  İSE: "है"
  DEĞİLSE: "अन्यथा"
  SON: "समाप्त"

  # Functions
  İŞLEÇ: "कार्य"
  DÖNÜŞ: "वापसी"

  # I/O
  YAZDIR: "लिखो"

  # Loops
  DÖNGÜ: "लूप"
  DÖNGÜ_BITIR: "तोड़ो"
```

Dosyayı kaydedin, MLP otomatik olarak Hindi desteğini etkinleştirir!

---

## 🚀 Self-Hosting Mimarisi

MLP, **bootstrap** sürecini tamamlamış bir dildir:

### Stage 1: Bootstrap Compiler (C)
```
C Source Code (3,700 satır)
    ↓ [GCC]
MLP Compiler Executable
```

### Stage 2: Self-Hosting Compiler (MLP)
```
MLP Source Code (927 satır)
    ↓ [Stage 1 Compiler]
Assembly Code (3,342 satır)
    ↓ [NASM + GCC]
MLP Compiler (MLP ile yazılmış!)
```

### Stage 3: Multi-Language Preprocessor 🚧
```
Any Language Source Code
    ↓ [Preprocessor - UTF-8 aware]
Normalized MLP Code
    ↓ [Stage 2 Compiler]
Binary
```

---

## 🏗️ Proje Yapısı

```
MLP/
├── c_compiler/              # Bootstrap Compiler (C)
│   ├── c_lexer.c/h         # UTF-8 aware lexer
│   ├── c_parser.c/h        # Parser
│   ├── c_generator.c/h     # Assembly generator
│   └── main.c              # Entry point
│
├── mlp_compiler/            # Self-Hosting Compiler (MLP)
│   ├── lexer.mlp           # Lexer
│   ├── parser.mlp          # Parser
│   └── generator.mlp       # Generator
│
├── preprocessor/            # Multi-Language Preprocessor 🆕
│   ├── language_detector.c # Auto-detect language
│   ├── keyword_mapper.c    # Map keywords to internal
│   └── utf8_handler.c      # UTF-8/Unicode support
│
├── config/lang/             # Language Definitions 🆕
│   ├── turkish.yaml        # Türkçe
│   ├── english.yaml        # English
│   ├── hindi.yaml          # हिन्दी
│   └── ...
│
├── runtime/
│   └── runtime.c           # Runtime Library
│
├── ornekler/               # Example Programs
│   ├── tr/                 # Türkçe örnekler
│   ├── en/                 # English examples
│   └── hi/                 # हिन्दी उदाहरण
│
└── docs/
    ├── SPECS.md            # Language Spec
    ├── MULTILANG_GUIDE.md  # Multi-language guide 🆕
    └── LANGUAGE_SUPPORT.md # How to add languages 🆕
```

---

## 🎯 Yol Haritası (Roadmap)

### ✅ Tamamlanan
- [x] Self-hosting compiler (C → MLP)
- [x] x86-64 assembly generation
- [x] Struct/Array support
- [x] Turkish keyword support
- [x] English keyword support

### 🚧 Şu An Üzerinde Çalışılan
- [ ] **Multi-language preprocessor** (öncelik!)
- [ ] UTF-8/Unicode lexer (sıfırdan C'de)
- [ ] Language auto-detection
- [ ] Hindi keyword support
- [ ] YAML-based language config

### 🔮 Gelecek
- [ ] Chinese, Japanese, Arabic support
- [ ] VSCode extension (multi-lang syntax highlighting)
- [ ] Online playground (write in any language)
- [ ] Language Server Protocol (LSP)
- [ ] Community language packs

---

## 🤝 Katkıda Bulunma

MLP açık kaynak bir projedir ve katkılarınızı bekliyoruz!

### Nasıl Katkıda Bulunabilirsiniz?

1. **Yeni Dil Ekleyin** - `config/lang/your_language.yaml`
2. **Preprocessor Geliştirin** - UTF-8 handling, keyword mapping
3. **Örnek Programlar Yazın** - Kendi dilinizde örnekler
4. **Dokümantasyon** - Dil rehberleri, tutoriallar
5. **Test Edin** - Farklı dillerde test senaryoları

---

## 📜 Lisans

MIT License - Detaylar için [LICENSE](./LICENSE) dosyasına bakın.

---

## 🌟 Vizyon

**MLP, programlamanın evrensel bir dil olmasını hedefliyor.**

Bir Türk öğrenci, Türkçe ile öğrensin.
Bir Hintli developer, Hindi ile çalışsın.
Bir Çinli mühendis, Çince ile kodlasın.

**Hepsi aynı programı yazsın, aynı sonucu alsın.** 🌍

---

**⭐ Projeyi beğendiyseniz, GitHub'da yıldız vermeyi unutmayın!**

**🚀 Kendi dilinizi eklemek için Pull Request açın!**

**🌍 MLP ile programlama evrensel oluyor!**
