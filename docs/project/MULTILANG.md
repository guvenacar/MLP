# MLP Multi-Language Programming Demo

## 🌍 Supported Languages / Desteklenen Diller

MLP now supports programming in **5 languages** through automatic keyword translation:

- 🇹🇷 **Türkçe** (Turkish) - `tr-TR`
- 🇷🇺 **Русский** (Russian) - `ru-RU`
- 🇮🇳 **हिन्दी** (Hindi) - `hi-IN`
- 🇨🇳 **中文** (Chinese) - `zh-CN`
- 🇺🇸 **English** (English) - `en-US`

## 📝 Usage / Kullanım

### Quick Build (One Command)
```bash
./mlpbuild input.mlp output
./output
```

### Manual Steps
```bash
# 1. Compile to assembly
./mlp input.mlp output.asm

# 2. Assemble
nasm -f elf64 output.asm -o output.o

# 3. Link
gcc -no-pie output.o runtime/runtime.o runtime/hashmap.o -o output -lm

# 4. Run
./output
```

## 🎯 Example Programs

### Turkish / Türkçe
```mlp
-- lang: tr-TR
SAYISAL sayi = 42
yazdir "Merhaba Dünya!"

eger sayi > 40 ise
    yazdir "Büyük sayı"
son eger
```

### Russian / Русский
```mlp
-- lang: ru-RU
целое число = 42
печать "Привет Мир!"

если число > 40 то
    печать "Большое число"
конец если
```

### Hindi / हिन्दी
```mlp
-- lang: hi-IN
sankhya num = 42
likho "नमस्ते दुनिया!"

agar num > 40 to
    likho "बड़ा अंक"
samaapt agar
```

## 🏗️ Architecture / Mimari

```
Source Code (.mlp)
    ↓
[mlp_preprocessor]  ← Translates keywords using diller.json
    ↓
English IR (.mlp)
    ↓
[mlpc compiler]     ← Parses and generates assembly
    ↓
Assembly (.asm)
    ↓
[nasm]              ← Assembles to object file
    ↓
[gcc]               ← Links with runtime
    ↓
Binary (executable)
```

## ✨ Features / Özellikler

- ✅ Multi-language keyword support via JSON configuration
- ✅ UTF-8 string support (preserves native characters)
- ✅ Multi-line strings with `"""..."""`
- ✅ Automatic language detection from `-- lang: XX-XX` header
- ✅ Self-hosting C compiler (no Python dependency)
- ✅ Phase 5.8 features: Range loops, For-each, Multi-line strings

## 📦 Files / Dosyalar

- `mlp` - Compiler wrapper (auto-preprocessing)
- `mlpbuild` - Full build script (compile + assemble + link)
- `mlpc` - Core compiler (English IR → Assembly)
- `runtime/mlp_preprocessor` - Keyword translator
- `diller.json` - Language mappings configuration
- `runtime/runtime.o` - Runtime library
- `runtime/hashmap.o` - HashMap implementation

## 🔧 Adding New Languages

Edit `diller.json` to add new language mappings:

```json
{
  "id": "fr-FR",
  "name": "Français",
  "keywords": {
    "numeric": ["numérique", "entier"],
    "print": ["imprimer", "afficher"],
    "if": ["si"],
    "then": ["alors"],
    ...
  }
}
```

Then use `-- lang: fr-FR` in your source files!

## 🎓 Examples

See `/tmp/fibonacci_*.mlp` for Fibonacci implementation in 4 languages!

## 📜 License

MIT License - See LICENSE file

---

**Built with ❤️ for multilingual programming**
