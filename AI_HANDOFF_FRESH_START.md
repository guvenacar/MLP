# 🆕 Yeni AI Agent - Temiz Başlangıç Rehberi

**Tarih:** 29 Kasım 2025  
**Durum:** Phase 12 Complete - Self-Hosting İçin Hazır! 🎉

---

## ✅ Şu An Ne Durumda?

### Bootstrap Compiler (C) - %100 Tamamlandı
```bash
cd /home/pardus/projeler/MLP/MLP/melp
./melp-bootstrap source.mlp output.s   # MLP → x86-64 Assembly
nasm -f elf64 output.s -o output.o
ld output.o runtime/runtime.o -o program
./program
```

### Çalışan Özellikler (Phase 0-12)
- ✅ **Temel Tipler:** numeric, text, boolean, decimal
- ✅ **Veri Yapıları:** Arrays, Structs, Pointers
- ✅ **Kontrol Akışı:** if/else, for, while, do-while, switch/case, else-if
- ✅ **Fonksiyonlar:** Tanımlama, çağırma, recursion
- ✅ **Lambda & Closures:** Runtime magic number detection ile
- ✅ **Exception Handling:** try-catch-finally, throw
- ✅ **File I/O:** read, write, append operations
- ✅ **String Utilities:** length, concat, substring, compare
- ✅ **CLI Arguments:** argc/argv support
- ✅ **Modules:** import/export (çoğunlukla çalışıyor, %85)

### Proje Dosya Yapısı
```
/home/pardus/projeler/MLP/MLP/
├── kurallar_kitabı.md              # 📖 ANA REFERANS - ÖNCE BUNU OKU!
├── TODO.md                          # Gelecek özellikler
├── CONSISTENCY_ANALYSIS_REPORT.md   # Son analiz raporu
└── melp/                            # Bootstrap compiler (C)
    ├── bootstrap/
    │   ├── lexer.c                  # Tokenization
    │   ├── parser.c                 # AST oluşturma
    │   ├── codegen.c                # x86-64 assembly üretimi
    │   └── main.c
    ├── runtime/
    │   ├── runtime.c                # Minimal C runtime
    │   └── runtime.h
    ├── test*.mlp                    # Test dosyaları
    ├── Makefile                     # make clean && make
    └── melp-bootstrap               # Derlenmiş compiler binary
```

---

## 🎯 Sonraki Adım: Self-Hosting!

### Self-Hosting Nedir?
**MLP compiler'ını MLP dilinde yazmak!**

Şu an: `lexer.c + parser.c + codegen.c` (C'de)  
Hedef: `lexer.mlp + parser.mlp + codegen.mlp` (MLP'de)

### Neden Hazırız?
1. ✅ **Tüm dil özellikleri var:** Functions, arrays, structs, file I/O
2. ✅ **Bootstrap compiler çalışıyor:** Test edilmiş, kararlı
3. ✅ **Runtime sağlam:** Memory management, string ops hazır
4. ✅ **Proof of concept var:** `SELF_HOSTING_STRATEGY.md` belgesi mevcut

### Nasıl Başlanır?

#### Adım 1: Mini Lexer (MLP'de)
```mlp
-- lexer.mlp
func tokenize(text source) -> Array<Token>
    -- Token array oluştur
    -- source'u karakter karakter oku
    -- Token tipleri: NUMBER, IDENTIFIER, KEYWORD, etc.
    return tokens
end func
```

**Test:** Basit "numeric x = 5" kodunu tokenize et

#### Adım 2: Mini Parser (MLP'de)
```mlp
-- parser.mlp
func parse(Array<Token> tokens) -> AST
    -- Token listesini AST'ye çevir
    -- Sadece variable declaration ile başla
    return ast
end func
```

**Test:** Token array'i → AST struct

#### Adım 3: Mini Codegen (MLP'de)
```mlp
-- codegen.mlp
func generate(AST ast) -> text
    -- AST'den assembly string üret
    return assembly_code
end func
```

**Test:** AST → "section .text\n..." assembly string

#### Adım 4: Dosya I/O Entegrasyonu
```mlp
-- compiler.mlp
func main()
    text source = file_read("input.mlp")
    Array<Token> tokens = tokenize(source)
    AST ast = parse(tokens)
    text asm = generate(ast)
    file_write("output.s", asm)
end func
```

**Milestone:** İlk self-compiled program! 🎉

---

## 🚨 Önemli Notlar

### Yapılması Gerekenler
1. **ÖNCELİK 1:** kurallar_kitabı.md'yi baştan sona oku
2. **ÖNCELİK 2:** `melp/SELF_HOSTING_STRATEGY.md` belgesi var mı kontrol et
3. **Test sürekli:** Her adımda küçük programlar yaz ve çalıştır
4. **Temiz tut:** Git durumunu kontrol et, gereksiz dosyaları commit'leme

### Yapılmaması Gerekenler
- ❌ **Git push yapma** - Tüm çalışma lokal
- ❌ **Büyük değişiklikler yapma** - İnkremental ilerle
- ❌ **Yeni dökümantasyon oluşturma** - kurallar_kitabı.md'yi güncelle
- ❌ **Eksik özellikler ekleme** - Self-hosting odaklı kal

### Takıldığın Zaman
1. `kurallar_kitabı.md` → Tüm syntax ve örnekler orada
2. `melp/bootstrap/*.c` → Mevcut C implementasyonu referans
3. `melp/test*.mlp` → Çalışan örnekler
4. `TODO.md` → Eksik özellikler listesi

---

## 🎨 Strateji

### Minimal Başla
İlk self-hosted compiler **minimal** olacak:
- Sadece temel özellikler: variables, if/else, functions
- Gelişmiş özellikler sonra eklenecek: lambdas, exceptions, modules

### Kademeli Büyüt
```
Stage 0 (C Bootstrap): Tüm özellikler ✅
    ↓
Stage 1 (MLP Minimal): Variables, if/else, functions
    ↓
Stage 2 (MLP Enhanced): +Arrays, +Structs
    ↓
Stage 3 (MLP Full): +Lambdas, +Exceptions, +Modules
```

Her stage, kendinden sonraki stage'i derleyecek!

---

## 📊 Başlangıç Komutları

```bash
# Proje dizinine git
cd /home/pardus/projeler/MLP/MLP/melp

# Compiler'ı derle
make clean && make

# Test et
./melp-bootstrap test_simple.mlp test_simple.s
nasm -f elf64 test_simple.s -o test_simple.o
ld test_simple.o runtime/runtime.o -o test_simple
./test_simple

# Git durumu
git status

# Dökümantasyon oku
cat ../kurallar_kitabı.md | head -100
```

---

## 🏁 İlk Görev Önerisi

**"Minimal Lexer in MLP"**

1. `melp/self_host_lexer.mlp` dosyası oluştur
2. Basit tokenizer yaz (sadece numeric, identifier, operators)
3. Test programı: "numeric x = 5" → [NUMERIC, IDENTIFIER, EQUALS, NUMBER]
4. Bootstrap compiler ile derle
5. Çalıştır ve sonucu gör

**Tahmini süre:** 2-3 saat  
**Başarı kriteri:** Token array'i ekrana yazdır

---

## 📚 Ek Kaynaklar

- **Phase dokümantasyonu:** `melp/PHASE_*_COMPLETE.md` dosyaları
- **Exception handling:** `EXCEPTION_HANDLING_COMPLETE.md`
- **Syntax referansı:** `kurallar_kitabı.md` → "Söz Dizimi Referansı" bölümü
- **Module sistemi:** `TODO.md` → "Modül Sistemi" bölümü

---

**Hazır mısın? İyi şanslar! 🚀**

*Son not: Generics denemesi yapıldı ama geri alındı (29 Kasım). Self-hosting için gerekli değil.*
