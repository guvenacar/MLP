# 🌍 MLP - Kendi Dilinizde Programlama!

> **"Code in Your Language, Compile to Machine Code!"**

---

## 🎯 Proje Hakkında

**MLP (Multi-Language Programming)**, herkesin kendi ana dilinde programlama yapabileceği, derlenmiş bir programlama dilidir.

- 🇹🇷 **Türkçe**: `EĞER`, `DÖNGÜ`, `İŞLEÇ`, `YAZDIR`
- 🇷🇺 **Русский**: `если`, `пока`, `функция`, `печать`
- 🇨🇳 **中文**: `如果`, `当`, `函数`, `打印`
- 🇮🇳 **हिन्दी**: `अगर`, `जब_तक`, `फलन`, `लिखो`
- 🌐 **Ve daha fazlası...** (Sınırsız dil eklenebilir!)

**Özellikler:**
- ✅ Native x86-64 makine kodu üretiyor
- ✅ Self-hosting (Derleyici kendi dilinde yazılmış)
- ✅ 6 dil desteği (sınırsız eklenebilir)
- ✅ VSCode extension
- ✅ GUI desteği
- ✅ MIT Lisanslı (Açık kaynak)

---

## ✅ ŞU AN ÇALIŞAN ÖZELLİKLER

### Temel Özellikler (Hazır)

**Veri Tipleri:**
- ✅ **SAYISAL** (int - 64-bit integer)
- ✅ **SÖZEL** (string - dinamik string)
- ✅ **ZITLIK** (boolean - true/false)

**Kontrol Yapıları:**
- ✅ **EĞER-İSE-DEĞİLSE** (if-then-else)
- ✅ **DÖNGÜ** (while loop)
- ✅ **DÖNGÜ_BITIR** (break)
- ✅ **İŞLEÇ** (function) - Recursive fonksiyonlar dahil

**Built-in Fonksiyonlar:**
- ✅ **String İşlemleri**: `string_concat`, `string_length`, `string_char_at`, `string_substring`
- ✅ **Dosya I/O**: `file_open`, `file_read`, `file_write`, `file_close`
- ✅ **Karakter Dönüşüm**: `char_code`, `code_to_char`
- ✅ **Sistem**: `get_cwd`

**GUI (Grafiksel Arayüz):**
- ✅ **Pencere**: `gui_window_create`, `gui_window_show`, `gui_window_destroy`
- ✅ **Buton**: `gui_button_create`
- ✅ **Label**: `gui_label_create`
- ✅ **Canvas**: `gui_canvas_create`, `gui_canvas_draw_rect`, `gui_canvas_draw_circle`
- ✅ **Event Handling**: `gui_poll_event`, `gui_get_clicked_button`

**Derleyici Özellikleri:**
- ✅ **English-native compiler** (C ile yazılmış)
- ✅ **Multi-language preprocessor** (Python)
- ✅ **Self-hosting compiler** (MLP ile yazılmış, 984 satır)
- ✅ **x86-64 assembly** çıktısı (NASM)
- ✅ **One-command compilation**: `mlpc program.mlp -o output`

**Geliştirici Araçları:**
- ✅ **VSCode Extension**: Syntax highlighting, snippets
- ✅ **Language Server** (temel)
- ✅ **Compiler wrapper** (`mlpc`)

---

## 🚧 YAPILMAKTA OLAN / EKSİK ÖZELLİKLER

### Yüksek Öncelik (v3.1 - Yakın Gelecek)

**Veri Tipleri:**
- ⏳ **ONDALIK** (float/double) - Ondalıklı sayılar
- ⏳ **DİZİ** (array) - Sabit boyutlu diziler
- ⏳ **HİÇLİK** (null) - Null değer

**Operatörler:**
- ⏳ **VE/VEYA/DEĞİL** (AND/OR/NOT) - Mantıksal operatörler
- ⏳ **MOD** (%) - Modulo operatörü
- ⏳ **Atama Operatörleri**: `+=`, `-=`, `*=`, `/=`, `++`, `--`

**Kontrol Yapıları:**
- ⏳ **EĞER_DEĞİLSE_EĞER** (elif) - Çoklu koşul
- ⏳ **DURUM-HAL** (switch-case) - Çok yönlü dallanma
- ⏳ **DEVAM_ET** (continue) - Döngü devamı
- ⏳ **İÇİN** (for loop) - For döngüsü

**String İşlemleri:**
- ⏳ **BÜYÜK_HARFE** / **KÜÇÜK_HARFE** (to_upper/to_lower)
- ⏳ **AYIR** (split) - String bölme
- ⏳ **DEĞİŞTİR** (replace) - String değiştirme
- ⏳ **KIRP** (trim) - Boşluk temizleme

**I/O İşlemleri:**
- ⏳ **GİRDİ_AL** (input) - Kullanıcıdan girdi alma
- ⏳ **KOMUT_ÇALIŞTIR** (exec) - Sistem komutları

### Orta Öncelik (v3.2 - Orta Vade)

**Veri Yapıları:**
- 📋 **LİSTE** (list) - Dinamik boyutlu liste
- 📋 **SÖZLÜK** (dictionary) - Key-value mapping
- 📋 **KÜME** (set) - Unique değerler
- 📋 **DEMET** (tuple) - Sabit veri grubu

**Hata Yönetimi:**
- 📋 **DENE-YAKALA** (try-catch) - Exception handling
- 📋 **HATA_FIRLAT** (throw) - Exception fırlatma
- 📋 **SONUNDA** (finally) - Finally bloğu

**OOP (Object-Oriented):**
- 📋 **SINIF** (class) - Sınıf tanımı
- 📋 **KURUCU** (constructor) - Constructor metodu
- 📋 **BU** (this/self) - Self referansı
- 📋 **KALITIM** (inheritance) - Kalıtım desteği
- 📋 **ARAYÜZ** (interface) - Interface tanımı

**Module Sistemi:**
- 📋 **İÇE_AKTAR** (import) - Module import
- 📋 **DIŞA_AKTAR** (export) - Module export
- 📋 **PAKET** (package) - Paket yönetimi

### Uzun Vade (v4.0+)

**Compiler İyileştirmeleri:**
- 📅 **LLVM Backend** - LLVM altyapısı
- 📅 **Optimization Passes** - Kod optimizasyonu
- 📅 **Cross-compilation** - Farklı platformlar
- 📅 **WebAssembly Target** - WASM desteği

**Debugging:**
- 📅 **DWARF Debug Info** - Debug bilgileri
- 📅 **GDB/LLDB Support** - Debugger desteği
- 📅 **Breakpoint Support** - Breakpoint'ler

**Gelişmiş Özellikler:**
- 📅 **Generics** - Generic tipler
- 📅 **Lambda/Closures** - Anonim fonksiyonlar
- 📅 **Pointer/Reference** - Bellek kontrolü
- 📅 **Concurrent Programming** - Eşzamanlılık
- 📅 **Pattern Matching** - Pattern matching

**Ekosistem:**
- 📅 **Standard Library** - Kapsamlı stdlib
- 📅 **Package Manager** - Paket yöneticisi
- 📅 **Documentation Generator** - Dökümantasyon
- 📅 **IDE Support** - Gelişmiş IDE entegrasyonu

---

## 🎯 KATKI SUNABİLECEĞİNİZ ALANLAR

### 🟢 Kolay (Yeni Başlayanlar İçin)

**1. Yeni Dil Desteği Eklemek** (10-30 dakika)
- Arapça, İspanyolca, Fransızca, Almanca, Japonca, Korece...
- Sadece `diller.json` düzenleme
- Kod bilgisi gerektirmez
- Test case yazmak yeterli

**2. Dokümantasyon** (1-3 saat)
- README çevirisi (Türkçe, Rusça, Çince...)
- Örnek program yazma
- Tutorial hazırlama
- Blog yazısı

**3. Test Case Ekleme** (30 dakika - 2 saat)
- Farklı dillerde test programları
- Edge case'ler
- Bug report

**4. VSCode Extension İyileştirme** (2-5 saat)
- Syntax highlighting geliştirme
- Code snippet ekleme
- Tema iyileştirmeleri

### 🟡 Orta (Deneyimli Geliştiriciler)

**5. Built-in Fonksiyonlar** (3-8 saat)
- String işlemleri (split, replace, trim)
- Math fonksiyonları (sin, cos, sqrt)
- Dosya/dizin işlemleri
- Tarih/saat fonksiyonları
- **Gerekli Bilgi**: C programlama, syscall

**6. Yeni Operatörler** (5-10 saat)
- Mantıksal operatörler (AND/OR/NOT)
- Modulo (%)
- Bitwise operatörler
- **Gerekli Bilgi**: C, lexer/parser, assembly

**7. Kontrol Yapıları** (8-15 saat)
- elif (else-if)
- for loop
- switch-case
- continue
- **Gerekli Bilgi**: C, parser, code generation

**8. GUI Backend** (10-20 saat)
- SDL2 backend implementasyonu
- GTK backend
- Event handling iyileştirme
- **Gerekli Bilgi**: C, SDL2/GTK, GUI programlama

### 🔴 Zor (İleri Seviye)

**9. Float/Double Desteği** (15-30 saat)
- Lexer güncellemesi
- Parser güncellemesi
- Assembly generation (x87/SSE)
- Runtime fonksiyonlar
- **Gerekli Bilgi**: C, x86-64 assembly, floating-point

**10. Array Tipi** (20-40 saat)
- Tip sistemi güncellemesi
- Stack/heap allocation
- Bounds checking
- Multi-dimensional arrays
- **Gerekli Bilgi**: C, compiler design, memory management

**11. OOP (Class/Object)** (40-80 saat)
- Tip sistemi genişletme
- Vtable implementasyonu
- Method dispatch
- Inheritance/Polymorphism
- **Gerekli Bilgi**: Compiler design, OOP theory, assembly

**12. Hata Yönetimi (Try-Catch)** (30-60 saat)
- Exception handling mechanism
- Stack unwinding
- Try-catch-finally blokları
- **Gerekli Bilgi**: Compiler design, exception handling theory

**13. LLVM Backend** (60-120 saat)
- LLVM IR generation
- Optimization passes
- Cross-compilation
- **Gerekli Bilgi**: LLVM, compiler backend

**14. Package Manager** (80-150 saat)
- Package format tasarımı
- Dependency resolution
- Registry sistemi
- **Gerekli Bilgi**: Networking, database, security

---

## 🎓 ÖĞRENME KAYNAKLARI

Katkı sunmak için kendinizi geliştirmek isterseniz:

**Compiler Basics:**
- [Crafting Interpreters](https://craftinginterpreters.com/) (Ücretsiz online kitap)
- [Engineering a Compiler](https://www.elsevier.com/books/engineering-a-compiler/cooper/978-0-12-088478-0)

**Assembly/Low-Level:**
- [x86-64 Assembly Guide](https://cs.brown.edu/courses/cs033/docs/guides/x64_cheatsheet.pdf)
- [NASM Tutorial](https://cs.lmu.edu/~ray/notes/nasmtutorial/)

**MLP Spesifik:**
- `SPECS.md` - Dil spesifikasyonu
- `AI_RULES.md` - Geliştirme kuralları
- `CONTRIBUTING.md` - Katkı rehberi

---

## 💡 PROJE HAZIR MI? DEĞERLENDİRME

### ✅ Güçlü Yönler (İlan İçin Hazır)

1. **Temel Özellikler Çalışıyor**
   - Compiler stabil
   - Çok dil desteği fonksiyonel
   - Self-hosting başarılı

2. **Dokümantasyon İyi**
   - README kapsamlı
   - SPECS.md detaylı
   - AI_RULES.md ve CONTRIBUTING.md mevcut

3. **Proof of Concept Güçlü**
   - 6 dil desteği çalışıyor
   - GUI demo var
   - Fibonacci, faktöriyel gibi örnekler çalışıyor

4. **Unique Value Proposition**
   - Gerçek çok dil desteği (sadece string değil, tüm syntax)
   - Native performance
   - Extensible mimari

### ⚠️ Zayıf Yönler (İyileştirilebilir)

1. **Eksik Özellikler**
   - Float/double yok (matematiksel programlar için kritik)
   - Array yok (veri işleme için gerekli)
   - Mantıksal operatörler yok (AND/OR/NOT)
   - elif yok (çoklu koşul için gerekli)

2. **Test Coverage**
   - Unit testler eksik
   - Automated testing yok
   - CI/CD pipeline yok

3. **Hata Mesajları**
   - Bazı hata mesajları belirsiz
   - Line/column bilgisi bazen eksik

4. **Platform Desteği**
   - Sadece Linux/macOS
   - Windows desteği yok (WSL gerekiyor)

5. **Community Infrastructure**
   - GitHub Issues/Discussions yeni
   - Topluluk henüz yok
   - Örnek projeler az

---

## 🤔 TAVSİYE: İLAN ETMELİ Mİ?

### ✅ EVET, İLAN EDEBİLİRSİNİZ! İşte Neden:

**1. MVP (Minimum Viable Product) Hazır**
- Temel özellikler çalışıyor
- Gerçek programlar yazılabiliyor
- Unique value proposition var

**2. Açık Kaynak İçin İdeal Aşama**
- Erken katkı sunanlara sahiplenme fırsatı
- Topluluk ile birlikte büyüme
- Erken feedback almak değerli

**3. Dokümantasyon Yeterli**
- Yeni gelenleri yönlendirecek belgeler mevcut
- Katkı rehberi hazır

**4. Risk Düşük**
- MIT lisanslı, herkes kullanabilir
- "Production Ready" demişsiniz ama "v3.0" - gelişmeye açık
- Hala eksikler var ama açıkça belirtiliyor

### 📋 İlan Öncesi Yapılacaklar (1-2 Gün)

**Kritik:**
1. ✅ **GitHub Issues Template** ekleyin
2. ✅ **GitHub Discussions** aktif edin
3. ✅ **ROADMAP.md** oluşturun (bu listeden)
4. ⚠️ **Test bir kez daha çalıştırın** (tüm örnekler)
5. ⚠️ **README.md badges** ekleyin (build status olursa harika)

**İyi Olur:**
6. 📹 **Demo video** çekin (5 dakika, YouTube)
7. 📝 **Blog yazısı** yazın (Medium/Dev.to)
8. 🎨 **Logo** tasarlayın (basit olabilir)

### 🎯 İlan Stratejisi

**1. Sosyal Medya (Aynı Anda)**
- **Twitter/X**: Thread at (10-15 tweet)
- **Reddit**: r/ProgrammingLanguages, r/programming, r/Turkey
- **LinkedIn**: Profesyonel ağ
- **Hacker News**: "Show HN: MLP - Programming in Your Native Language"

**2. İlk Mesaj Örneği:**

```
🌍 MLP'yi tanıtıyorum: Kendi dilinizde programlama!

Türkçe: EĞER x > 5 İSE
Русский: если x > 5 то
中文: 如果 x > 5 那么

✅ Native x86-64 kod
✅ Self-hosting
✅ Sınırsız dil
✅ MIT lisanslı

Katkı sunmak ister misiniz?
- Yeni dil ekleyin (10 dakika)
- Dokümantasyon yazın
- Özellik geliştirin

GitHub: github.com/guvenacar/MLP
```

**3. Topluluk Beklentisi:**
- İlk hafta: 50-200 star (iyi)
- İlk ay: 2-5 katkı sunucu
- Uzun vade: Küçük ama aktif topluluk

---

## 🎬 SONUÇ

**Evet, şimdi ilan edebilirsiniz!**

**Neden?**
- ✅ MVP çalışıyor
- ✅ Dokümantasyon yeterli
- ✅ Unique ve ilgi çekici
- ✅ Açık kaynak için ideal aşama

**Ama önce:**
1. ROADMAP.md oluşturun (bu listeyi kullanın)
2. GitHub Issues/Discussions aktif edin
3. Bir son test turu yapın
4. Demo video çekin (opsiyonel ama etkili)

**Motto:**
> "Mükemmel olmayı beklemeyin, paylaşın ve birlikte geliştirin!" 🚀

---

**Sorularınız varsa, istediğiniz zaman sorun!**
