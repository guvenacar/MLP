<!-- 🔒 UYARI: BU DOSYA MLP DİLİNİN TEK GERÇEĞİDİR (SINGLE SOURCE OF TRUTH) -->
<!-- 🚫 HİÇBİR AI/GELIŞTIRICI BU DOSYAYI SPECS_LOCK.md OLMADAN DEĞİŞTİREMEZ -->

# MLP Dil Spesifikasyonu v2.0

**Durum:** 🔒 **LOCKED** - Değişiklik için SPECS_LOCK.md'ye kayıt gerekli
**Son Güncelleme:** 17 Kasım 2024
**Yazar:** Güven Acar
**Compiler Versiyonu:** Full Compiler v2.0 (Production Ready)

---

## 📋 İÇİNDEKİLER

1. [Hızlı Başlangıç](#hızlı-başlangıç)
2. [VSCode Extension Kurulumu](#vscode-extension-kurulumu)
3. [Değiştirilemez Kurallar](#değiştirilemez-kurallar)
4. [Söz Dizimi](#söz-dizimi)
5. [Anahtar Kelimeler](#anahtar-kelimeler)
6. [Operatörler](#operatörler)
7. [Veri Tipleri](#veri-tipleri)
8. [Kontrol Yapıları](#kontrol-yapıları)
9. [Fonksiyonlar](#fonksiyonlar)
10. [Gelişmiş Özellikler](#gelişmiş-özellikler)
11. [Compiler Özellikleri](#compiler-özellikleri)
12. [Eksik Yönler ve Roadmap](#eksik-yönler-ve-roadmap)
13. [Diğer Dillerle Karşılaştırma](#diğer-dillerle-karşılaştırma)

---

## 🚀 HIZLI BAŞLANGIÇ

### Minimum "Merhaba Dünya"

```tyd
YAZDIR "Merhaba Dünya!"
```

Derlemek için:
```bash
cd c_bootstrap
./calistir.sh merhaba.mlp
```

### Tam Örnek

```tyd
-- Fibonacci hesaplayan program
İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON
    DÖNÜŞ fibonacci(n - 1) + fibonacci(n - 2)
SON

SAYISAL sonuc = fibonacci(10);
YAZDIR "Fibonacci(10) ="
YAZDIR sonuc
```

---

## 🎨 VSCODE EXTENSION KURULUMU

### Kurulum Adımları

#### 1. VSIX Paketini İndirin/Derleyin

**Seçenek A: Hazır VSIX Kullanın**
```bash
cd vscode-tyd
# VSIX dosyası: tyd-language-0.1.0.vsix
```

**Seçenek B: Kendiniz Derleyin**
```bash
cd vscode-tyd
npm install
npm run compile
npm install -g @vscode/vsce
vsce package
```

#### 2. VSCode'a Kurun

**Grafiksel Arayüz ile:**
1. VSCode'u açın
2. Extensions paneline gidin (Ctrl+Shift+X)
3. "..." menüsünden "Install from VSIX..." seçin
4. `tyd-language-0.1.0.vsix` dosyasını seçin
5. VSCode'u yeniden başlatın

**Komut Satırı ile:**
```bash
code --install-extension vscode-tyd/tyd-language-0.1.0.vsix
```

### VSCode Extension Özellikleri

✅ **Syntax Highlighting**
- Anahtar kelimeler (EĞER, DÖNGÜ, İŞLEÇ, vb.)
- String literals ve escape characters
- Yorumlar (-- ve {- -})
- Operatörler ve sayılar

✅ **Code Snippets**
- `func` → İŞLEÇ template
- `if` → EĞER/DEĞİLSE template
- `while` → DÖNGÜ template
- `for` → Sayaç bazlı döngü template

✅ **IntelliSense**
- Anahtar kelime tamamlama
- Fonksiyon parametre ipuçları
- Hover bilgileri

✅ **Build Komutları**
- **Ctrl+Shift+B**: Aktif MLP dosyasını derle
- **F5**: Derle ve çalıştır
- Command Palette: "MLP: Build All" → Tüm .mlp dosyalarını derle

✅ **Diagnostics**
- Gerçek zamanlı sözdizimi kontrolü
- Hata ve uyarılar
- Satır ve kolon bilgileri

### Kullanım

1. Yeni bir `.mlp` dosyası oluşturun
2. Kodunuzu yazın (IntelliSense aktif olacak)
3. **Ctrl+Shift+B** ile derleyin
4. Terminal'de sonuçları görün
5. **F5** ile çalıştırın

---

## 🚫 DEĞİŞTİRİLEMEZ KURALLAR

### Kural 1: Noktalı Virgül (;) Kullanımı

**SADECE değişken tanımlamalarında kullanılır:**

✅ **İZİN VERİLEN:**
```tyd
SAYISAL x;
SAYISAL y = 10;
METIN isim = "Ahmet";
MANTIKSAL aktif = DOĞRU;
```

❌ **KESINLIKLE YASAK:**
```tyd
DÖNÜŞ x + y;        -- ❌ YANLIŞ
SON;                -- ❌ YANLIŞ
YAZDIR x;           -- ❌ YANLIŞ
x = 5;              -- ❌ YANLIŞ (atama)
```

**NEDEN?**
- Söz dizimi tutarlılığı
- Blok tabanlı yapılar nokta virgül gerektirmez
- Sadece tanımlamalar cümle benzeri yapılardır

---

### Kural 2: Blok Sonlandırma Formatı

**Sadece `SON` kullanılır:**

```tyd
İŞLEÇ fonksiyon() İSE
    -- kod
SON

EĞER koşul İSE
    -- kod
DEĞİLSE
    -- kod
SON

DÖNGÜ
    -- kod
SON
```

❌ **ASLA:**
```tyd
SON;              -- ❌ Noktalı virgül yok
SON İŞLEÇ         -- ❌ Anahtar kelime yok
İŞLEÇ SON         -- ❌ Sıra yanlış
```

---

### Kural 3: Yorumlar

**Tek satır:** `--`
```tyd
-- Bu bir yorum
SAYISAL x = 10  -- Satır sonu yorumu
```

**Çok satırlı:** `{- ... -}`
```tyd
{-
   Bu bir çok satırlı yorum
   Birden fazla satır olabilir
   Kodun ortasında kullanılabilir
-}
```

**Önemli:** `---` artık desteklenmiyor, yerine `{- -}` kullanın!

---

## 🔤 SÖZ DİZİMİ

### Değişken Tanımlama

```tyd
SAYISAL x;                    -- Tanımlama (varsayılan = 0)
SAYISAL y = 10;               -- Başlangıç değeriyle
METIN isim = "Ahmet";         -- String tanımlama
```

**Önemli:** Noktalı virgül **SADECE** bu satırlarda!

---

### Atama

```tyd
x = 20                        -- ❌ Noktalı virgül YOK
isim = "Mehmet"               -- ❌ Noktalı virgül YOK
```

---

### String Literals ve Escape Characters

MLP tam UTF-8 ve escape character desteği sağlar:

```tyd
-- Temel string
METIN mesaj = "Merhaba Dünya";

-- Escape characters
METIN tırnak = "\"Merhaba\" dedi";
METIN yeni_satir = "Satır 1\nSatır 2";
METIN tab = "Kolon1\tKolon2";
METIN backslash = "C:\\dosyalar\\test.txt";

-- Türkçe karakterler (tam UTF-8 desteği)
METIN türkçe = "Şeker, çiğköfte, İstanbul, Ğ harfi";

-- Karmaşık kombinasyonlar
METIN karışık = "Şöyle dedi:\n\t\"Merhaba\\nDünya!\"";
```

**Desteklenen Escape Sequences:**
- `\"` → Tırnak işareti
- `\n` → Yeni satır (newline)
- `\t` → Tab
- `\r` → Carriage return
- `\\` → Backslash
- `\'` → Tek tırnak
- `\0` → Null karakter

---

### Fonksiyon Tanımlama

```tyd
İŞLEÇ topla(a, b) İSE
    SAYISAL sonuc = a + b;  -- Tanımlama: noktalı virgül VAR
    DÖNÜŞ sonuc             -- Komut: noktalı virgül YOK
SON
```

**Çağırma:**
```tyd
SAYISAL x = topla(5, 3);     -- Tanımlama: noktalı virgül VAR
YAZDIR topla(10, 20)          -- Komut: noktalı virgül YOK
```

---

### Koşullu İfadeler

```tyd
EĞER x > 10 İSE
    YAZDIR "Büyük"
DEĞİLSE
    YAZDIR "Küçük"
SON

-- İç içe koşullar
EĞER x == 0 İSE
    YAZDIR "Sıfır"
DEĞİLSE
    EĞER x > 0 İSE
        YAZDIR "Pozitif"
    DEĞİLSE
        YAZDIR "Negatif"
    SON
SON
```

---

### Döngüler

```tyd
-- Sonsuz döngü
DÖNGÜ
    YAZDIR "Sonsuz"
    DÖNGÜ_BITIR  -- Break ile çık
SON

-- Koşullu döngü (while benzeri)
SAYISAL i = 0;
DÖNGÜ
    EĞER i >= 10 İSE
        DÖNGÜ_BITIR
    SON

    YAZDIR i
    i = i + 1
SON

-- For-like pattern
SAYISAL sayac = 0;
DÖNGÜ
    YAZDIR sayac
    sayac = sayac + 1

    EĞER sayac == 5 İSE
        DÖNGÜ_BITIR
    SON
SON
```

---

### Yazdırma

```tyd
YAZDIR "Merhaba"              -- String
YAZDIR 42                     -- Sayı
YAZDIR x + y                  -- İfade
YAZDIR STRING_BIRLESTIR("A", "B")  -- Fonksiyon çağrısı
```

---

## 🔑 ANAHTAR KELİMELER

| Türkçe | İngilizce | Kullanım |
|--------|-----------|----------|
| `İŞLEÇ` | function | Fonksiyon tanımlama |
| `DÖNÜŞ` | return | Değer döndürme |
| `EĞER` | if | Koşul başlangıcı |
| `İSE` | then | Blok başlangıcı |
| `DEĞİLSE` | else | Alternatif blok |
| `DÖNGÜ` | while/loop | Döngü başlangıcı |
| `DÖNGÜ_BITIR` | break | Döngüden çık |
| `SON` | end | Blok sonlandırma |
| `SAYISAL` | int/number | 64-bit tam sayı |
| `METIN` | string | String tipi |
| `MANTIKSAL` | boolean | Boolean (gelecekte) |
| `DOĞRU` | true | Boolean true (gelecekte) |
| `YANLIŞ` | false | Boolean false (gelecekte) |
| `YAZDIR` | print | Ekrana yazdır |

---

## ⚡ OPERATÖRLER

### Aritmetik
```tyd
x + y    -- Toplama
x - y    -- Çıkarma
x * y    -- Çarpma
x / y    -- Bölme
```

### Karşılaştırma
```tyd
x == y   -- Eşitlik
x != y   -- Eşitsizlik
x > y    -- Büyüktür
x < y    -- Küçüktür
x >= y   -- Büyük eşit
x <= y   -- Küçük eşit
```

### Mantıksal (Gelecekte)
```tyd
a VE b   -- Mantıksal VE (AND)
a VEYA b -- Mantıksal VEYA (OR)
DEĞİL a  -- Mantıksal DEĞİL (NOT)
```

---

## 📊 VERİ TİPLERİ

| Tip | Anahtar Kelime | Varsayılan | Örnek | Boyut |
|-----|---------------|-----------|-------|-------|
| 64-bit Tam Sayı | `SAYISAL` | 0 | `SAYISAL x = 42;` | 8 byte |
| String | `METIN` | "" | `METIN ad = "Ali";` | Pointer (8 byte) |
| Boolean (Gelecek) | `MANTIKSAL` | YANLIŞ | `MANTIKSAL ok = DOĞRU;` | 1 byte |

**Not:** Şu anda sadece SAYISAL ve METIN tam destekleniyor.

---

## 🎯 GELİŞMİŞ ÖZELLİKLER

### Built-in Fonksiyonlar

#### String İşlemleri
```tyd
-- String birleştirme
METIN ad = "Ahmet";
METIN soyad = "Yılmaz";
METIN tam_ad = STRING_BIRLESTIR(ad, " ");
tam_ad = STRING_BIRLESTIR(tam_ad, soyad);
YAZDIR tam_ad  -- "Ahmet Yılmaz"

-- String uzunluğu
SAYISAL uzunluk = STRING_UZUNLUK("Merhaba");
YAZDIR uzunluk  -- 7

-- Karakter al
METIN ilk_harf = STRING_KARAKTER_AL("Merhaba", 0);
YAZDIR ilk_harf  -- "M"

-- Substring
METIN alt = STRING_ALT("Merhaba", 3, 4);
YAZDIR alt  -- "haba"

-- Karakter kodu
SAYISAL kod = KARAKTER_KODU("A");
YAZDIR kod  -- 65

-- Kod'dan karakter
METIN harf = KODU_KARAKTERE(65);
YAZDIR harf  -- "A"
```

#### Dosya İşlemleri
```tyd
-- Dosya aç
METIN mod_oku = "r";
SAYISAL dosya = DOSYA_AC("test.txt", mod_oku);

-- Dosya oku
METIN icerik = DOSYA_OKU(dosya);
YAZDIR icerik

-- Dosya kapat
SAYISAL sonuc = DOSYA_KAPAT(dosya);

-- Dosya yaz
METIN mod_yaz = "w";
SAYISAL dosya2 = DOSYA_AC("output.txt", mod_yaz);
SAYISAL yazilan = DOSYA_YAZ(dosya2, "Merhaba Dünya!");
DOSYA_KAPAT(dosya2)
```

#### Sistem Fonksiyonları
```tyd
-- Çalışma dizinini al
METIN dizin = DIZIN_AL();
YAZDIR dizin
```

### Variable Scope

MLP 20 seviye derinliğe kadar nested scope destekler:

```tyd
SAYISAL x = 10;  -- Global scope (level 0)

İŞLEÇ test() İSE
    SAYISAL x = 20;  -- Function scope (level 1) - shadows global
    YAZDIR x         -- 20

    EĞER DOĞRU İSE
        SAYISAL x = 30;  -- If scope (level 2) - shadows function
        YAZDIR x         -- 30
    SON

    YAZDIR x  -- 20 (if scope'u çıkınca function scope)
SON

YAZDIR x  -- 10 (global scope)
```

---

## 🔧 COMPILER ÖZELLİKLERİ

### Compiler Mimarisi

```
MLP Kaynak Kodu (.mlp)
    ↓
┌────────────────┐
│   LEXER        │  → Token stream (UTF-8 aware)
│  (c_lexer.c)   │  → Position tracking (line, column)
└────────────────┘  → Comment skipping ({- -} ve --)
    ↓
┌────────────────┐
│   PARSER       │  → Abstract Syntax Tree (AST)
│  (c_parser.c)  │  → Syntax validation
└────────────────┘  → Detailed error messages
    ↓
┌────────────────┐
│  GENERATOR     │  → x86-64 Assembly (NASM)
│ (c_generator.c)│  → Stack frame management
└────────────────┘  → Scope tracking
    ↓
Assembly (.asm)
    ↓
NASM Assembler
    ↓
Object File (.o)
    ↓
GCC Linker
    ↓
Executable
```

### Compiler Özellikleri

✅ **Lexer:**
- UTF-8 multi-byte character support (Ğ, İ, Ş, Ü, Ö, Ç)
- Escape sequence handling (\n, \t, \", \\, vb.)
- Single-line comments (--)
- Multi-line comments ({- -})
- Token position tracking (line, column)
- Whitespace optimization

✅ **Parser:**
- Full language syntax support
- Nested structures (20 level depth)
- Detailed error messages with position
- Human-readable token names in errors
- Multi-line source file support

✅ **Code Generator:**
- x86-64 assembly generation (NASM syntax)
- Variable scope tracking (nested scopes)
- Stack frame optimization
- Function call conventions (Linux x86-64 ABI)
- String literal pooling
- Label generation

### Hata Mesajları

MLP compiler çok detaylı hata mesajları verir:

```
╔════════════════════════════════════════════════════════════╗
║ HATA [Parser]: Sözdizimi Hatası!                          ║
╚════════════════════════════════════════════════════════════╝

📍 Konum: Satır 5, Kolon 10

❌ Bulunan: "EĞER" (EĞER)
✓ Beklenen: ; (SAYISAL, METIN, BOOL değişken tanımlarında gerekli)

💡 Açıklama: Token tipi uyuşmuyor.
```

### Derleme Süreci

```bash
# Adım 1: MLP → Assembly
./c_bootstrap/compiler_test input.mlp output.asm

# Adım 2: Assembly → Object
nasm -f elf64 output.asm -o output.o

# Adım 3: Object + Runtime → Executable
gcc output.o runtime/runtime.o -o output -no-pie

# Veya hepsini birden:
./c_bootstrap/calistir.sh input.mlp
```

---

## ⚠️ EKSİK YÖNLER VE ROADMAP

### Şu Anda Eksik Olanlar

❌ **Veri Yapıları:**
- [ ] Array/Dizi desteği
- [ ] Struct/Yapı desteği
- [ ] Pointer desteği

❌ **Tip Sistemi:**
- [ ] MANTIKSAL (Boolean) tam implementasyonu
- [ ] Float/Ondalık sayı desteği
- [ ] Type checking ve casting

❌ **Kontrol Yapıları:**
- [ ] Switch/Case yapısı
- [ ] For döngüsü (şimdilik DÖNGÜ ile simüle ediliyor)
- [ ] Continue desteği

❌ **İleri Seviye:**
- [ ] Module/Import sistemi
- [ ] Exception handling
- [ ] Generics
- [ ] Lambda/Closure
- [ ] Pattern matching

❌ **Standart Kütüphane:**
- [ ] Math kütüphanesi
- [ ] Network kütüphanesi
- [ ] Threading desteği

### Roadmap

**v2.1 (Ocak 2025):**
- Array/Dizi desteği
- MANTIKSAL tip tam implementasyonu
- Switch/Case yapısı

**v2.2 (Şubat 2025):**
- Struct/Yapı desteği
- Module/Import sistemi
- Standard library başlangıcı

**v3.0 (Mart 2025) - SELF-HOSTING:**
- MLP compiler'ı MLP'de yazılacak
- Bootstrap döngüsü tamamlanacak
- Self-hosting test başarılı olacak

**v4.0 (2025 Sonu):**
- Tam standard library
- Optimization passes
- Debugging desteği

---

## 🆚 DİĞER DİLLERLE KARŞILAŞTIRMA

### MLP vs Python

| Özellik | MLP | Python |
|---------|-----|--------|
| **Dil** | Türkçe | İngilizce |
| **Tip** | Statik (compile-time) | Dinamik (runtime) |
| **Performans** | Native (x86-64) | Interpreted |
| **Bellek** | Manuel (stack/heap) | GC |
| **Derleme** | AOT compiler | Interpreter |
| **Syntax** | Blok bazlı (SON) | Indent bazlı |
| **Noktalı virgül** | Sadece tanımda | Hiç |

**Örnek:**
```python
# Python
def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n-1) + fibonacci(n-2)

print(fibonacci(10))
```

```tyd
-- MLP
İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON
    DÖNÜŞ fibonacci(n - 1) + fibonacci(n - 2)
SON

YAZDIR fibonacci(10)
```

### MLP vs C

| Özellik | MLP | C |
|---------|-----|---|
| **Dil** | Türkçe | İngilizce |
| **Syntax** | Yüksek seviye | Düşük seviye |
| **Pointer** | Yok (henüz) | Manuel |
| **String** | Built-in | char* |
| **Memory** | Stack (şimdilik) | malloc/free |
| **Header** | Yok | #include |

**Örnek:**
```c
// C
#include <stdio.h>

int fibonacci(int n) {
    if (n <= 1)
        return n;
    return fibonacci(n-1) + fibonacci(n-2);
}

int main() {
    printf("%d\n", fibonacci(10));
    return 0;
}
```

```tyd
-- MLP
İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON
    DÖNÜŞ fibonacci(n - 1) + fibonacci(n - 2)
SON

YAZDIR fibonacci(10)
```

### MLP vs Go

| Özellik | MLP | Go |
|---------|-----|---|
| **Dil** | Türkçe | İngilizce |
| **Concurrency** | Yok (henüz) | Goroutines |
| **Package** | Yok (henüz) | import |
| **Interface** | Yok (henüz) | interface |
| **GC** | Yok | Var |
| **Syntax** | SON blokları | Curly braces |

### MLP'nin Avantajları

✅ **Türkçe:** Türkçe konuşanlar için doğal
✅ **Basit:** Minimal syntax, kolay öğrenme
✅ **Native:** Doğrudan makine kodu, hızlı
✅ **Self-hosting:** Kendi kendini derleyebilecek
✅ **Şeffaf:** Compiler C'de, anlaşılır
✅ **Eğitim:** Compiler nasıl çalışır öğretir

### MLP'nin Dezavantajları

❌ **Genç:** Henüz stabil değil
❌ **Ekosistem:** Kütüphane yok
❌ **Topluluk:** Küçük kullanıcı tabanı
❌ **Araçlar:** IDE desteği sınırlı
❌ **Platform:** Sadece Linux x86-64 (şimdilik)

---

## 🎯 TAM ÖRNEK PROGRAMLAR

### Örnek 1: Fibonacci

```tyd
-- Fibonacci hesaplama
İŞLEÇ fibonacci(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ n
    SON
    DÖNÜŞ fibonacci(n - 1) + fibonacci(n - 2)
SON

SAYISAL i = 0;
DÖNGÜ
    EĞER i >= 10 İSE
        DÖNGÜ_BITIR
    SON

    SAYISAL sonuc = fibonacci(i);
    YAZDIR "fibonacci("
    YAZDIR i
    YAZDIR ") = "
    YAZDIR sonuc

    i = i + 1
SON
```

### Örnek 2: String İşlemleri

```tyd
-- String manipülasyon
METIN ad = "Ahmet";
METIN soyad = "Yılmaz";

METIN tam_ad = STRING_BIRLESTIR(ad, " ");
tam_ad = STRING_BIRLESTIR(tam_ad, soyad);

YAZDIR "Tam ad:"
YAZDIR tam_ad

SAYISAL uzunluk = STRING_UZUNLUK(tam_ad);
YAZDIR "Uzunluk:"
YAZDIR uzunluk

-- İlk harf
METIN ilk = STRING_KARAKTER_AL(tam_ad, 0);
YAZDIR "İlk harf:"
YAZDIR ilk
```

### Örnek 3: Dosya İşlemleri

```tyd
-- Dosya okuma/yazma
METIN mod_yaz = "w";
SAYISAL dosya = DOSYA_AC("test.txt", mod_yaz);

METIN mesaj = "Merhaba MLP!\nBu ikinci satır.";
DOSYA_YAZ(dosya, mesaj)
DOSYA_KAPAT(dosya)

-- Dosyayı oku
METIN mod_oku = "r";
dosya = DOSYA_AC("test.txt", mod_oku)
METIN icerik = DOSYA_OKU(dosya);
DOSYA_KAPAT(dosya)

YAZDIR "Dosya içeriği:"
YAZDIR icerik
```

### Örnek 4: Nested Scope

```tyd
-- Scope örneği
SAYISAL global_x = 100;

İŞLEÇ test() İSE
    SAYISAL local_x = 200;
    YAZDIR "Function scope:"
    YAZDIR local_x

    EĞER DOĞRU İSE
        SAYISAL nested_x = 300;
        YAZDIR "Nested scope:"
        YAZDIR nested_x
        YAZDIR local_x  -- Erişilebilir
    SON

    -- nested_x artık erişilemez
    YAZDIR local_x  -- Hala erişilebilir
SON

test()
YAZDIR global_x
```

---

## 🚫 PROJE KURALLARI

### Kural 4: Python Yasağı - Sadece C!

**MLP projesi tamamen Python bağımlılığından kurtulmuştur.**

❌ **KESINLIKLE YASAK:**
```bash
python anything.py
pip install ...
#!/usr/bin/env python3
```

✅ **ZORUNLU:**
```bash
gcc tool.c -o tool
./tool
```

**NEDEN?**
1. **Self-Hosting Hedefi:** MLP kendini derleyecek
2. **Bağımsızlık:** Hiçbir harici runtime yok
3. **Performans:** C native performans
4. **Tutarlılık:** Her şey C'de

---

### Kural 5: Hedef - Self-Hosting

**Aşamalar:**

```
✅ Aşama 0: Python Prototipi (Tamamlandı, Kaldırıldı)
✅ Aşama 1: C Bootstrap Compiler (TAMAMLANDI - %100)
🚧 Aşama 2: MLP ile MLP Compiler (DEVAM EDİYOR)
⏳ Aşama 3: Self-Hosting (HEDEF)
```

**Mevcut Durum (17 Kasım 2024):**
- ✅ Full compiler tamamlandı
- ✅ Tüm dil özellikleri çalışıyor
- ✅ Production-ready
- 🚀 Self-hosting için hazır!

---

## 📖 KAYNAKLAR

### Proje Yapısı
```
MLP/
├── c_bootstrap/          # C Bootstrap Compiler
│   ├── compiler_test    # Ana executable
│   ├── c_lexer.c/h      # Lexer
│   ├── c_parser.c/h     # Parser
│   ├── c_generator.c/h  # Code Generator
│   └── calistir.sh      # Build script
├── runtime/             # Runtime library
│   └── runtime.c        # Built-in fonksiyonlar
├── vscode-tyd/          # VSCode Extension
│   └── tyd-language-0.1.0.vsix
├── mlp_compiler/        # MLP ile yazılmış compiler (gelecek)
├── examples/            # Örnek programlar
└── SPECS.md            # Bu dosya
```

### Komutlar

```bash
# Compiler'ı derle
cd c_bootstrap
gcc main.c c_lexer.c c_parser.c c_generator.c -o compiler_test -no-pie

# MLP dosyası derle ve çalıştır
./calistir.sh ../examples/fibonacci.mlp

# Sadece compile et
./compiler_test input.mlp output.asm

# VSCode extension kur
code --install-extension ../vscode-tyd/tyd-language-0.1.0.vsix
```

### Test Örnekleri

- `mlp_compiler/escape_test.mlp` - Escape character testleri
- `mlp_compiler/multiline_test.mlp` - Multi-line ve nested yapılar
- `mlp_compiler/full_lang_test.mlp` - Tam dil özellikleri
- `mlp_compiler/comment_test.mlp` - Yorum testleri

---

## 📚 EK KAYNAKLAR

### Dokümantasyon

- `SPECS.md` - Bu dosya (tam spesifikasyon)
- `README.md` - Proje overview
- `vscode-tyd/BUILD_GUIDE.md` - VSCode extension kurulumu
- Inline code comments - Her fonksiyonda açıklamalar

### Topluluk

- GitHub: `github.com/guvenacar/MLP`
- Issues: Hata raporları ve özellik istekleri
- Discussions: Genel tartışmalar

---

**© 2024 MLP Projesi - Tüm hakları saklıdır**

**Versiyon:** 2.0
**Son Güncelleme:** 17 Kasım 2024
**Compiler Durumu:** Production Ready ✅
**Self-Hosting:** In Progress 🚧
