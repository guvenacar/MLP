# 🤖 AI Asistanları İçin MLP Projesi Kılavuzu

**Son Güncelleme:** 19 Kasım 2025  
**Durum:** 🚀 Production Ready v3.0  
**Hedef:** Multi-Language Programming Language - Kod yazmanın dil engeli yok!  

---

## ⚠️ BAŞLAMADAN ÖNCE ZORUNLU OKUMALAR

### 1. 🔒 SPECS.md - TEK GERÇEK KAYNAK

```bash
/home/pardus/projeler/tyd-lang/MLP/SPECS.md
```

**BU DOSYA KİLİTLİDİR!** Hiçbir AI/geliştirici bu dosyayı kullanıcının onayı olmadan değiştiremez.

**İçeriği:**
- Dil spesifikasyonu (syntax, semantics)
- Veri tipleri (SAYISAL, SÖZEL, ZITLIK, HİÇLİK)
- Operatörler ve kontrol yapıları
- Noktalı virgül kuralları
- Çok dilli destek mimarisi

### 2. 📚 README.md - Proje Genel Bakış

```bash
/home/pardus/projeler/tyd-lang/MLP/README.md
```

**İçeriği:**
- Proje tanıtımı ve felsefesi
- Hızlı başlangıç örnekleri
- Desteklenen diller (en-US, tr-TR, ru-RU, zh-CN, hi-IN)
- Mimari açıklaması (Preprocessor → Compiler → Assembly)
- GUI geliştirme rehberi

### 3. 📄 diller.json - Dil Tanımları

```bash
/home/pardus/projeler/tyd-lang/MLP/diller.json
```

**UYARI:** Yeni dil eklemek dışında bu dosyayı değiştirmeyin!

**İçeriği:**
- Şu an 6 dil örneği (English, Turkish, Russian, Chinese, Hindi, Custom-Mehmet)
- **Teorik olarak SINIRSIZ dil desteklenebilir** (JSON'a eklemek yeterli)
- Anahtar kelime çevirileri (function → işleç, if → eğer)
- Variant desteği (eğer, EĞER, eger, EGER)

---

## 🎯 PROJE HEDEFİ VE FELSEFESİ

### Ana Hedef

**"Programlama sadece İngilizce değil, HER DİLDE yapılabilir!"**

### MLP Nedir?

- **Multi-Language Programming Language** (Çok Dilli Programlama Dili)
- Herkes kendi dilinde kod yazabilir
- Türkçe, Rusça, Çince, Hintçe... hepsi eşit!
- **Sınırsız dil desteklenebilir** (diller.json'a eklenerek)
- Aynı programı farklı dillerde yazabilirsiniz

### Mimari Felsefesi

```
┌─────────────────────────────────────────┐
│  Kaynak Kod (Herhangi bir dil)          │
│  - Türkçe: EĞER, DÖNGÜ, İŞLEÇ         │
│  - Rusça: если, пока, функция          │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│  PREPROCESSOR (dil_cevirici.py)         │
│  - Dil tespiti (-- lang: tr-TR)        │
│  - Anahtar kelime çevirisi             │
│  - String ve comment korunması          │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│  İngilizce Ara Kod (.preprocessed.mlp)  │
│  - if, while, function                  │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│  COMPILER (C ile yazılmış)              │
│  - Lexer → Parser → Generator           │
│  - SADECE İngilizce anlar              │
└─────────────────────────────────────────┘
                ↓
┌─────────────────────────────────────────┐
│  x86-64 Assembly → Native Executable    │
└─────────────────────────────────────────┘
```

**Tasarım Kararı:** Compiler core basit kalsın (English-only), çok dil desteği preprocessor katmanında!

---

## 🔴 VERİ TİPLERİ - KRİTİK BİLGİ!

### ⚠️ DİKKAT: MLP'de `int` YOK!

**MLP'nin gerçek veri tipleri:**

| Tip | English | Turkish | Açıklama |
|-----|---------|---------|----------|
| **SAYISAL** | `int` (compiler'da) | `SAYISAL`, `sayisal`, `SAYISAL` | BigDecimal (sınırsız hassasiyet) - tam sayı + ondalık |
| **SÖZEL** | `string` | `METIN`, `metin`, `YAZI`, `yazi` | BigString (sınırsız uzunluk) |
| **ZITLIK** | `true`/`false` | `DOĞRU`/`YANLIŞ`, `dogru`/`yanlis` | Boolean |
| **HİÇLİK** | (future) | `HİÇ`, `NULL` | Null değer (henüz implement edilmedi) |

### 🚨 YANLIŞ VS DOĞRU

```mlp
❌ YANLIŞ: int x = 5;              -- "int" kelimesi kullanıcıya gösterilmemeli
✅ DOĞRU:  SAYISAL x = 5;          -- Türkçe programda
✅ DOĞRU:  int x = 5;              -- İngilizce programda
✅ DOĞRU:  целое x = 5;            -- Rusça programda
```

**NEDEN?**
- MLP'de "int" sadece compiler'ın internal keyword'ü
- Kullanıcı kendi dilindeki kelimeyi kullanır
- Preprocessor `SAYISAL` → `int` çevirisini yapar
- Compiler sadece `int` görür

---

## 🚫 YAPMAMANIZ GEREKENLER (KRİTİK!)

### 1. ❌ Veri Tiplerini Karıştırmayın!

```mlp
❌ YANLIŞ:
"MLP'de int, string, boolean tipleri vardır"

✅ DOĞRU:
"MLP'de SAYISAL (BigDecimal), SÖZEL (BigString), ZITLIK (Boolean) tipleri vardır.
Compiler internal olarak bunları int/string/bool olarak işler ama kullanıcı 
her zaman kendi dilindeki kelimeyi kullanır."
```

**Kural:** Kullanıcıya konuşurken **her zaman dil-agnostik** ol!

### 2. ❌ SPECS.md'yi İhlal Etmeyin!

**Noktalı Virgül Kuralı:**

```mlp
✅ DOĞRU:
SAYISAL x;              -- Tanımlama → noktalı virgül VAR
SAYISAL y = 10;         -- İnitialize → noktalı virgül VAR
x = 20                  -- Atama → noktalı virgül YOK
DÖNÜŞ x + y             -- Return → noktalı virgül YOK
YAZDIR x                -- Print → noktalı virgül YOK
SON                     -- Block end → noktalı virgül YOK

❌ YANLIŞ:
DÖNÜŞ x + y;            -- Noktalı virgül YASAK
SON;                    -- Noktalı virgül YASAK
x = 20;                 -- Noktalı virgül YASAK
```

**NEDEN?** Tutarlılık! Sadece tanımlamalar cümle gibi biter.

### 3. ❌ Dil Eşitliğini Bozma!

**Tüm diller eşittir!**

```mlp
❌ YANLIŞ: "İngilizce default dil, diğerleri opsiyonel"
✅ DOĞRU:  "Tüm diller eşit, compiler internal olarak English kullanır"

❌ YANLIŞ: "Türkçe anahtar kelimeler ekleyelim"
✅ DOĞRU:  "diller.json'a yeni dil tanımı ekleyelim"
```

### 4. ❌ String/Comment İçeriğini Çevirmeyin!

**Preprocessor state machine:**

```mlp
-- lang: tr-TR
METIN mesaj = "EĞER bu değişmez";  -- String içi korunur!
EĞER x > 10 İSE                     -- Bu çevrilir → if x > 10 then
    YAZDIR mesaj                    -- Bu çevrilir → print mesaj
SON                                 -- Bu çevrilir → end

-- Bu yorum da korunur: EĞER DÖNGÜ İŞLEÇ
```

**KURAL:** STATE_CODE → çevir, STATE_STRING/STATE_COMMENT → koru!

### 5. ❌ Tasarım Kararlarını Değiştirmeyin!

**Bu konularda kullanıcıya sormadan değişiklik yapmayın:**
- Anahtar kelimeler (İŞLEÇ, DÖNÜŞ, EĞER)
- Blok sonlandırma (SON, end)
- Noktalı virgül kullanımı
- Yorum syntax (-- ve {- -})
- Veri tipleri (SAYISAL, SÖZEL, ZITLIK)
- Preprocessor yaklaşımı

---

## ✅ YAPMANIZ GEREKENLER

### Her Oturumda (ZORUNLU!)

```bash
# 1. SPECS.md'yi okuyun
cat /home/pardus/projeler/tyd-lang/MLP/SPECS.md

# 2. README.md'yi okuyun (proje durumu)
cat /home/pardus/projeler/tyd-lang/MLP/README.md

# 3. diller.json'u inceleyin (dil tanımları)
cat /home/pardus/projeler/tyd-lang/MLP/diller.json

# 4. Mevcut durumu test edin
./mlpc merhaba_dunya.mlp -o test
./test
```

### Kod Yazmadan Önce

1. **Dil Tespiti**
   ```bash
   # Kaynak dosyanın dilini belirle
   head -5 input.mlp | grep "-- lang:"
   ```

2. **Syntax Kontrolü**
   ```bash
   # Noktalı virgül kontrolü (sadece tanımlamalarda olmalı)
   grep -n "DÖNÜŞ.*;" input.mlp  # Hata!
   grep -n "SON;" input.mlp       # Hata!
   ```

3. **Test**
   ```bash
   # Tüm pipeline'ı test et
   ./mlpc input.mlp -o output --verbose
   ./output
   ```

### Yeni Özellik Eklerken

```markdown
1. SPECS.md'de tanımlı mı kontrol et
2. Compiler'a ekle (c_compiler/)
   - c_lexer.c: Token ekle
   - c_parser.c: Parsing ekle
   - c_generator.c: Assembly generation ekle
3. diller.json'a anahtar kelime ekle (TÜM diller için!)
4. Test dosyası yaz (her dilde)
5. mlpc ile derle ve test et
6. README.md güncelle (gerekirse)
```

---

## 📖 DİL KURALLARI (ÖZET)

### Noktalı Virgül (;)

```mlp
SAYISAL x;              ✅ Declaration
SAYISAL y = 10;         ✅ Initialization
x = 20                  ✅ Assignment (NO semicolon)
DÖNÜŞ x + y             ✅ Return (NO semicolon)
YAZDIR x                ✅ Print (NO semicolon)
SON                     ✅ Block end (NO semicolon)
```

### Blok Yapıları

```mlp
İŞLEÇ topla(a, b) İSE
    DÖNÜŞ a + b
SON                     ✅ end (NOT: SON İŞLEÇ gibi context-aware değil!)

EĞER x > 5 İSE
    YAZDIR x
DEĞİLSE
    YAZDIR "küçük"
SON

DÖNGÜ
    EĞER i >= 10 İSE
        BİTİR
    SON
    i = i + 1
SON
```

### Yorumlar

```mlp
-- Tek satır yorum

{-
  Çok satırlı
  yorum bloğu
-}
```

### Çok Dilli Örnek

**Türkçe:**
```mlp
-- lang: tr-TR
SAYISAL x = 42;
YAZDIR "Merhaba Dünya"
```

**Rusça:**
```mlp
-- lang: ru-RU
целое x = 42;
печать "Привет Мир"
```

**İkisi de aynı assembly → aynı executable!**

---

## 🔧 GELİŞTİRME SÜRECİ

### 1. Yeni Dil Eklerken

```markdown
1. diller.json'a dil tanımı ekle
2. Tüm anahtar kelimeleri tanımla
3. Variant'ları ekle (büyük/küçük harf)
4. Test dosyası yaz (test_newlang.mlp)
5. mlpc ile derle
6. README.md'ye dili ekle
```

**Örnek:**

```json
{
  "id": "es-ES",
  "name": "Español (Spanish)",
  "keywords": {
    "int": ["entero"],
    "string": ["cadena"],
    "if": ["si"],
    "then": ["entonces"],
    "else": ["sino"],
    "end": ["fin"],
    "function": ["función"],
    "return": ["devolver"],
    "print": ["imprimir"]
  }
}
```

### 2. Compiler Hatası Düzeltirken

```markdown
1. Hangi aşamada hata? (lexer/parser/generator)
2. SPECS.md'ye uygun mu kontrol et
3. Test case yaz (minimal repro)
4. Düzelt ve test et
5. Tüm test dosyalarını çalıştır
```

### 3. Preprocessor Hatası Düzeltirken

```markdown
1. State machine state'ini kontrol et
2. String/comment içi korunuyor mu?
3. Çeviri doğru mu? (diller.json)
4. --verbose ile debug
5. .preprocessed.mlp çıktısını incele
```

---

## 📊 PROJE YAPISI

```
MLP/
├── SPECS.md                 # 🔒 Dil spesifikasyonu (KİLİTLİ)
├── README.md                # Proje tanıtımı
├── AI_RULES.md              # ← Bu dosya (AI kuralları)
├── diller.json              # 🔒 Dil tanımları (dikkatli değiştir)
│
├── c_compiler/              # English-native compiler (C)
│   ├── c_lexer.c/h         # Lexical analyzer
│   ├── c_parser.c/h        # Parser (syntax check)
│   ├── c_generator.c/h     # Assembly generator
│   ├── main.c              # Entry point
│   └── Makefile            # Build script
│
├── runtime/
│   ├── runtime.c/h         # Built-in functions
│   ├── gui_mock.c          # GUI mock backend
│   └── gui_sdl.c           # GUI SDL2 backend (future)
│
├── dil_cevirici.py         # 🌍 Preprocessor (multi-language)
├── mlpc                    # Compiler wrapper script
├── migrate.py              # Migration tool
│
├── test_*.mlp              # Multi-language test cases
└── vscode-mlp/             # VSCode extension
    └── syntaxes/
        └── mlp.tmLanguage.json
```

---

## 🎯 ŞU ANKİ ÖNCELİKLER

### Durum: Production Ready ✅

- ✅ English-native compiler çalışıyor
- ✅ Multi-language preprocessor çalışıyor
- ✅ Şu an 6 dil (en, tr, ru, zh, hi, custom-mehmet) - **sınırsız eklenebilir**
- ✅ Self-hosting compiler (MLP ile yazılmış)
- ✅ GUI support (mock backend)
- ✅ VSCode extension

### Gelecek Özellikler (v3.1)

1. ⏳ **float / ONDALIK** - Floating point desteği
2. ⏳ **AND/OR/NOT / VE/VEYA/DEĞİL** - Mantıksal operatörler
3. ⏳ **elif / EĞER_DEĞİLSE_EĞER** - Çoklu koşul
4. ⏳ **array / DİZİ** - Array tipi
5. ⏳ **input / GİRDİ_AL** - Kullanıcı girişi

### Gelecek (v4.0)

6. 📅 **class / SINIF** - OOP desteği
7. 📅 **try-catch / DENE-YAKALA** - Exception handling
8. 📅 **import / İÇE_AKTAR** - Module sistemi

---

## 🔍 SIKÇA SORULAN SORULAR

### S: MLP neden preprocessor kullanıyor?

**C:** Compiler core'u basit tutmak için. Compiler sadece İngilizce anlar, preprocessor çok dil desteği sağlar. Bu mimari:
- Compiler'ı basit tutar
- Yeni dil eklemek kolay (sadece JSON)
- Her dil eşit şekilde işlenir
- Bakımı kolay

### S: Neden Python preprocessor var? TYD'de Python yasaktı!

**C:** MLP farklı bir proje! TYD self-hosting hedefliyordu (hiç bağımlılık yok). MLP çok dilli destek hedefliyor, Python preprocessor bu iş için uygun. Gelecekte MLP'de yazılabilir ama öncelik değil.

### S: `int` mi yoksa `SAYISAL` mı kullanmalıyım?

**C:** **Bağlama göre!**
- Kullanıcıya açıklarken → `SAYISAL` (veya hangi dildeyse o)
- Compiler kodunda → `int` (internal keyword)
- SPECS.md'de → Her ikisi de (mapping göster)

### S: Yeni dil eklemek ne kadar sürer?

**C:** ~10 dakika!
1. diller.json'a dil ekle (5 dk)
2. Test dosyası yaz (3 dk)
3. Derle ve test et (2 dk)

### S: SPECS.md'yi değiştirmem gerekirse?

**C:**
1. Kullanıcıdan onay al
2. Değişikliği açıkla (neden gerekli?)
3. Tüm testleri güncelle
4. README.md'yi güncelle
5. Commit yap (açıklayıcı mesaj)

### S: GUI backend neden mock?

**C:** SDL2 kurulumu gerekiyor. Mock backend test ve demo için yeterli. Kullanıcı gerçek GUI isterse SDL2 kurup `gui_sdl.c` kullanabilir.

---

## 📞 DESTEK VE İLETİŞİM

### Commit Mesajları

```bash
# İyi örnekler:
git commit -m "lexer: GUI keywords eklendi (6 dilde)"
git commit -m "diller.json: Hindi dil desteği eklendi"
git commit -m "SPECS.md: Float tip tanımı eklendi (kullanıcı onayı)"

# Kötü örnekler:
git commit -m "fix"
git commit -m "güncelleme"
git commit -m "çalışıyor"
```

### Issue/PR Formatı

**Issue başlığı:**
```
[LANG] Rusça 'elif' anahtar kelimesi eksik
[COMPILER] Parser float literal desteklemiyor
[DOCS] SPECS.md'de array syntax belirsiz
```

**PR açıklaması:**
```markdown
## Değişiklik
Hindi dil desteği eklendi

## Dosyalar
- diller.json: Hindi keywords eklendi
- test_hindi.mlp: Test case eklendi
- README.md: Hindi örneği eklendi

## Test
✅ ./mlpc test_hindi.mlp -o test
✅ ./test → Çıktı doğru
```

---

## 🎓 ÖĞRENİLMİŞ DERSLER

### Geçmiş Hatalar (Tekrarlamayın!)

1. **Veri Tiplerini Karıştırma**
   - ❌ Hata: "MLP'de int, string vardır"
   - ✅ Çözüm: "MLP'de SAYISAL, SÖZEL vardır (compiler internal: int/string)"

2. **String İçinde Çeviri Yapma**
   - ❌ Hata: `"EĞER"` string'i `"if"` olarak çevrildi
   - ✅ Çözüm: Preprocessor state machine düzeltildi

3. **Dil Eşitsizliği**
   - ❌ Hata: "İngilizce default, diğerleri ekstra"
   - ✅ Çözüm: Tüm diller eşit, preprocessor tüm dilleri destekler

4. **SPECS.md Göz Ardı Etme**
   - ❌ Hata: Noktalı virgül kuralı ihlal edildi
   - ✅ Çözüm: Her değişiklik önce SPECS.md kontrol

---

## ✅ BAŞLAMADAN ÖNCE CHECKLİST

Her oturumda bu adımları izleyin:

```markdown
- [ ] SPECS.md okudum
- [ ] README.md okudum (proje durumu)
- [ ] diller.json'u inceledim
- [ ] Mevcut durumu test ettim (./mlpc test.mlp)
- [ ] Yapacağım değişiklik SPECS'e uygun
- [ ] Veri tiplerini doğru kullanıyorum (SAYISAL/SÖZEL)
- [ ] Noktalı virgül kuralına uyuyorum
- [ ] Çok dil desteğini göz önünde bulunduruyorum
- [ ] Test case'leri hazırladım
```

---

## 🚀 HEMEN BAŞLAYIN

```bash
# Proje dizinine git
cd /home/pardus/projeler/tyd-lang/MLP

# SPECS.md'yi oku
cat SPECS.md

# Test et
./mlpc merhaba_dunya.mlp -o test
./test

# Yeni özellik ekle veya hata düzelt
# ... (kodlama) ...

# Test et (tüm diller)
./mlpc test_turkish.mlp -o test_tr && ./test_tr
./mlpc test_russian.mlp -o test_ru && ./test_ru
./mlpc test_chinese.mlp -o test_zh && ./test_zh

# Commit yap
git add .
git commit -m "feature: Açıklayıcı mesaj"
```

---

## 📝 RAPOR FORMATI (ÖNERİLEN)

Her oturum sonunda kısa rapor yazılması önerilir:

```markdown
# AI Çalışma Raporu - YYYY-MM-DD

**AI:** [İsim]  
**Tarih:** YYYY-MM-DD  
**Süre:** XX dakika  
**Durum:** ✅ Başarılı / ⚠️ Kısmi / ❌ Başarısız  

---

## Özet (2-3 cümle)
Ne yapıldı?

## ✅ Tamamlananlar
- [x] Görev 1
- [x] Görev 2
- [ ] Görev 3 (kısmi)

## ⚠️ Sorunlar
| Sorun | Çözüm |
|-------|-------|
| Parser segfault | Null check eklendi |

## 🔄 Sonraki Adımlar
1. [ ] Yapılacak iş 1
2. [ ] Yapılacak iş 2

## 📁 Dosya Değişiklikleri
- c_lexer.c (+50 satır)
- diller.json (+1 dil)
- test_spanish.mlp (yeni)
```

---

## 🌟 MLP'NİN GÜÇLÜ YÖNLERİ

### Neden MLP Özel?

1. **Gerçek Çok Dil Desteği**
   - Sadece string değil, tüm syntax çok dilli!
   - Her dil eşit şekilde işlenir

2. **Basit Mimari**
   - Preprocessor + Compiler ayrımı
   - Her biri tek sorumluluğa sahip
   - Bakımı kolay

3. **Extensible (Sınırsız Dil Desteği)**
   - Yeni dil eklemek 10 dakika
   - JSON tabanlı konfigürasyon
   - Kod değişikliği gerektirmez
   - Teorik limit yok, pratik sınırsız

4. **Native Performance**
   - Direkt x86-64 assembly
   - VM yok, interpreter yok
   - Hızlı çalışma

5. **Self-Hosting**
   - MLP compiler'ı MLP'de yazılmış
   - Yani MLP kendini derleyebiliyor!

---

## 💡 SON NOTLAR

### MLP Felsefesi

> **"Programlama herkes için, her dilde olmalı!"**

### Hatırlanması Gerekenler

1. **Veri Tipleri:** SAYISAL (BigDecimal), SÖZEL (BigString), ZITLIK (Boolean)
2. **Noktalı Virgül:** Sadece tanımlamalarda
3. **Preprocessor:** Çok dil desteğinin kalbi
4. **Compiler:** English-only, basit
5. **Eşitlik:** Tüm diller eşit!

### AI'lar İçin Altın Kurallar

1. ✅ SPECS.md'ye uyun
2. ✅ Veri tiplerini doğru kullanın
3. ✅ Çok dil desteğini koruyun
4. ✅ Test edin, test edin, test edin
5. ✅ Kullanıcıya açık olun (değişiklik için onay)

---

**Notlar:**
- Bu dosya AI'lar için bir kılavuzdur
- İnsan geliştiriciler için de faydalıdır
- Düzenli olarak güncellenmelidir
- SPECS.md ile çelişmemelidir
- Proje felsefesini yansıtmalıdır

---

**Son Güncelleme:** 19 Kasım 2025  
**Yazar:** Güven Acar + GitHub Copilot  
**Durum:** 🔒 Aktif Kılavuz  
**Versiyon:** 1.0  

---

## 🎯 ÖZET: TEMEL KURALLAR

```
1. SPECS.md = Kutsal Kitap (değiştirme!)
2. SAYISAL/SÖZEL/ZITLIK = Gerçek tipler (int/string değil!)
3. Noktalı virgül = Sadece tanımlama
4. Preprocessor = Çok dil sihri
5. Tüm diller = Eşit!
6. Test = Her zaman
7. Kullanıcı = Patron (onay al)
```

**Başarılar! 🚀**
