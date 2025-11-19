# MLP Self-Hosting Compiler

## 🎯 Durum: ÜZERİNDE ÇALIŞILIYOR

Bu dizin MLP'nin **self-hosting** compiler bileşenlerini içerir - yani MLP dilinde yazılmış MLP compiler!

## 📁 Ana Bileşenler

- **`tyd_lexer.mlp`** (548 satır) - Lexer (Tokenizer)
- **`tyd_parser.mlp`** (189 satır) - Parser (Syntax Analyzer)
- **`tyd_generator.mlp`** (190 satır) - Code Generator (Assembly Output)

**Toplam:** 927 satır MLP kodu

## ⚠️ ÖNEMLİ NOT

**Bu compiler şu anda kullanılmıyor!**

Mevcut `mlpc` script'i bootstrap için **C compiler** (`c_compiler/compiler_test`) kullanıyor. MLP self-hosting compiler eski syntax kullanıyor ve güncel MLP syntax'ı ile uyumsuz.

## 🏗️ Mimari

```
Kaynak Kod (.mlp)
    ↓
MLP Preprocessor (C) → Keyword çevirisi
    ↓
C Compiler (c_compiler/) → Lexer/Parser/Generator ← ŞU ANDA BU KULLANILIYOR
    ↓
x86-64 Assembly (NASM)
    ↓
Binary
```

**Hedef Mimari (Gelecek):**
```
Kaynak Kod (.mlp)
    ↓
MLP Preprocessor (C)
    ↓
MLP Compiler (tyd_compiler/) → MLP dilinde yazılmış! ← HEDEF
    ↓
x86-64 Assembly
    ↓
Binary
```

## 🔄 Self-Hosting Süreci

### Stage 1: Bootstrap (TAMAMLANDI ✅)
C dilinde tam özellikli MLP compiler yazıldı (`c_compiler/`).

### Stage 2: Self-Hosting Compiler (KISMİ TAMAMLANDI ⚠️)
MLP dilinde compiler yazıldı ama **eski syntax kullanıyor**.

### Stage 3: Güncel Syntax'a Güncelleme (DEVAM EDİYOR 🚧)
Self-hosting compiler güncel MLP syntax'ına güncellenmeli.

## 🔧 Güncel Syntax vs Eski Syntax

**Eski (tyd_compiler/ dosyaları):**
```mlp
İŞLEÇ fonksiyon_adi(param) İSE
    -- kod
SON
```

**Güncel (c_compiler tarafından derlenen):**
```mlp
İŞLEÇ fonksiyon_adi(param);
    -- kod
SON
```

## 📝 Yapılacaklar

1. ✅ TYD → MLP dosya uzantısı değişikliği
2. ⚠️ Eski syntax → Güncel syntax migration
3. ⚠️ `tyd_lexer.mlp`, `tyd_parser.mlp`, `tyd_generator.mlp` güncelleme
4. ⚠️ `mlpc` script'ini MLP compiler kullanacak şekilde güncelleme
5. ⚠️ Test ve doğrulama

## 🚀 Kullanım (Gelecekte)

```bash
# Bootstrap ile MLP compiler'ı derle
./mlpc tyd_compiler/tyd_lexer.mlp -o mlp_lexer
./mlpc tyd_compiler/tyd_parser.mlp -o mlp_parser
./mlpc tyd_compiler/tyd_generator.mlp -o mlp_generator

# MLP compiler ile MLP kodu derle
./mlp_compiler program.mlp -o program
```

## 📚 Dokümantasyon

Detaylı bilgi için:
- `SELF_HOSTING_MILESTONE.md` - Self-hosting başarı hikayesi
- `SPECS.md` - MLP dil spesifikasyonu
- `c_compiler/README.md` - Bootstrap C compiler

## 🤝 Katkıda Bulunma

MLP self-hosting compiler'ı güncel syntax'a taşımak için:

1. `tyd_lexer.mlp` dosyasını incele
2. Syntax farklarını tespit et
3. Güncel syntax'a göre güncelle
4. Test et: `./mlpc tyd_lexer.mlp`
5. Diğer bileşenler için tekrarla

---

**Son Güncelleme:** Kasım 2025  
**Durum:** Aktif Geliştirme  
**İletişim:** GitHub Issues
