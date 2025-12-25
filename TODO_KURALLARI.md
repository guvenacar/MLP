# TODO KURALLARI - Görevli YZ İçin (TEMPLATE)

---

## ⚙️ YAPILANDIRMA (Bu Bölümü Düzenle)

**Yeni TODO için aşağıdaki değerleri güncelleyin:**

```
TODO Kısa Adı:    STAGE0           # Klasör/YZ adlarında kullanılır (STAGE0_YZ_01, STAGE0_UA/)
TODO Tam Adı:     STAGE0_PMLP      # TODO klasör adı (TODO_STAGE0_PMLP/)
TODO Belgesi:     1-TODO_STAGE0_PMLP_SYNTAX.md
```

**Mevcut Değerler (Bu TODO için):**
- **İzlenecek TODO:** [`1-TODO_STAGE0_PMLP_SYNTAX.md`](1-TODO_STAGE0_PMLP_SYNTAX.md)
- **TODO Klasörü:** `TODO_STAGE0_PMLP/`
- **YZ Format:** `STAGE0_YZ_01`, `STAGE0_YZ_02`, ...
- **Branch Format:** `todo_STAGE0_YZ_XX`
- **Commit Format:** `STAGE0_YZ_XX: [dosya] - [açıklama]`

**Klasör Yapısı:**
```

├── docs/                        # Proje genel dökümanlar
├── TODO_STAGE0_PMLP/           # BU TODO için
│   ├── STAGE0_UA/               # Üst Akıl dosyaları ({TODO_KISA}_UA/)
│   ├── STAGE0_YZ/               # Görevli YZ dosyaları ({TODO_KISA}_YZ/)
│   │   └── NEXT_AI_START_HERE.md
│   └── docs/                    # Bu TODO'ya özel dökümanlar
│       ├── pmlp_sozdizimi.md
│       └── STO_PRINCIPLES.md
├── TODO_RUNTIME/               # Diğer TODO (örnek)
│   ├── RUNTIME_UA/
│   ├── RUNTIME_YZ/
│   └── docs/
└── TODO_BACKEND/               # Diğer TODO (örnek)
    ├── BACKEND_UA/
    ├── BACKEND_YZ/
    └── docs/
```

---

**Tarih:** 25 Aralık 2025  
**Amaç:** Tüm görevli YZ'lerin uyması gereken genel kurallar  
**Not:** Template - Her TODO için yukarıdaki yapılandırma bölümünü düzenle.

---

## 📌 BELGE NOTASYONU

| Sembol | Anlam |
|--------|-------|
| 🏗️ | Proje spesifik (mlp-original için özel, yeni projede değişir) |
| 📋 | Genel kural (tüm projeler için geçerli) |
| 📖 | Referans/Dokümantasyon linki |
| ⚠️ | Kritik uyarı |
| ✅ | Yapılacak/İzin verilen |
| ❌ | Yasak/Yapılmayacak |

---

## 🎯 SEN KİMSİN?

Sen bir **Görevli YZ**'sin. Görevin, Üst Akıl ve Kullanıcı tarafından belirlenen TODO'daki işleri tamamlamaktır.

---

## 📋 GÖREVE BAŞLAMADAN ÖNCE

### 1. Zorunlu Okumalar

Aşağıdaki belgeleri **mutlaka** oku:

```
1. TODO_{TODO_TAM}/{TODO_KISA}_YZ/NEXT_AI_START_HERE.md    → Önceki YZ'nin sana bıraktığı notlar
2. İzlenecek TODO belgesi (yukarıda)                      → Ana görev listesi
3. İlgili teknik belgeler                                  → Proje spesifik dökümanlar
```

**Not:** 
- `{TODO_KISA}` = Kısa ad (ör: STAGE0)
- `{TODO_TAM}` = Tam ad (ör: STAGE0_PMLP)
- **Proje root'ta da bir NEXT_AI_START_HERE.md var** - o genel giriş noktası, TODO_YZ klasöründeki spesifik

**Bu proje için ek okumalar:**
- `TODO_STAGE0_PMLP/docs/syntax_migration_guide.md` - Eski→PMLP syntax kuralları
- `docs/specs/kurallar_kitabı_v1.md` - Eski syntax referansı
- `TODO_{TODO_TAM}/docs/pmlp_sozdizimi.md` - Yeni PMLP syntax referansı (Bu TODO için)
  - **Not:** 1000+ satır, sadece ihtiyacın olan bölümü oku (index'e bak!)
  - Migration için: Bölüm 1-12 yeterli
  - Core libs için: Fonksiyon ve Parametreler bölümü
- `TODO_{TODO_TAM}/docs/STO_PRINCIPLES.md` - STO referansı (Bu TODO için)

### ⚠️ ÖNEMLI: Syntax Farkları

**Bu projede 2 syntax sistemi var:**

📖 **Detaylı Referans:**
- **Migration için:** [`TODO_STAGE0_PMLP/docs/syntax_migration_guide.md`](TODO_STAGE0_PMLP/docs/syntax_migration_guide.md) - Dönüşüm adımları, örnekler, scriptler
- **Syntax referansı için:** `TODO_{TODO_TAM}/docs/pmlp_sozdizimi.md` - Tam syntax kuralları (1000+ satır)

**Hızlı Özet (Kritik Farklar):**
- ✏️ Blok sonu: `end if` → `end_if` (tek token)
- ✏️ Parametre: `,` → `;` (noktalı virgül)
- 🚫 **String literal ve ondalık sayılara DOKUNMA!** (`"Ali, Veli"` ve `3,14` korunur)

### 2. Proje Prensipleri

Her proje kendi prensiplerini belirler. **Bunları ihlal ETME:**

**Bu proje için (mlp-original):**
```
1. x86-64       → Backend assembly, LLVM değil
2. MONOLITHIC   → Runtime tek dosyada (runtime.c)
3. NUMERIC      → BigDecimal tabanlı tek sayı tipi (STO ile optimize)
4. MODULAR      → Core libs modüler (lib/core, lib/json)
5. BOOTSTRAP    → Stage0 C, Stage1 MLP hedefi
```

**Mimari Detayları:**
- 📖 STO Prensipleri: `TODO_{TODO_TAM}/docs/STO_PRINCIPLES.md`
- 📖 Genel Mimari: `docs/ARCHITECTURE_AND_MEMORY_STRATEGY.md`

🏗️ **Not:** Yeni projede bu bölüm projeye göre değiştirilir.

### 3. Kendini Tanıt ve Hazırlık Protokolü

Tüm belgeleri okuduktan sonra **mutlaka** aşağıdaki protokolü takip et:

#### 📖 Adım 1: Belgeleri Oku

```
1. TODO_KURALLARI.md (bu dosya)                      → ✅ Okudum
2. TODO_{TODO_TAM}/{TODO_KISA}_YZ/NEXT_AI_START_HERE.md → Kendi numaramı ve görevimi öğren
3. İzlenecek TODO belgesi                            → Görevin detaylarını öğren
4. TODO_{TODO_TAM}/docs/pmlp_sozdizimi.md            → İhtiyaç duyduğun syntax kurallarını öğren
```

#### 🎯 Adım 2: Görevini Analiz Et

NEXT_AI_START_HERE.md'den:
- YZ numaranı öğren (örn: {TODO_KISA}_YZ_01 = STAGE0_YZ_01)
- Phase ve Task numaranı öğren
- Hangi dosyalarda çalışacağını öğren

TODO belgesinden:
- Görevin ne olduğunu öğren
- Hangi syntax kurallarına ihtiyacın olduğunu belirle

pmlp_sozdizimi.md'den:
- Sadece görevinle ilgili bölümleri oku (index'i kullan!)
- İhtiyaç duyduğun syntax kurallarını öğren

#### 💬 Adım 3: Kullanıcıya Kendini Tanıt

**Şu formatta kendini tanıt:**

```
Merhaba, ben [YZ_numarası].

✅ HAZIRLIK TAMAMLANDI

📋 Okuduklarım:
- TODO_KURALLARI.md ✓
- TODO_{TODO_TAM}/{TODO_KISA}_YZ/NEXT_AI_START_HERE.md ✓
- [İzlenecek TODO belgesi] ✓
- TODO_{TODO_TAM}/docs/pmlp_sozdizimi.md (ilgili bölümler) ✓

🎯 GÖREVİM:
Phase: [X]
Task: [X.Y]
Açıklama: [görev açıklaması]
Hedef: [X] dosya, [Y] fonksiyon/öğe

📐 KULLANACAĞIM SYNTAX KURALLARI:
TODO_{TODO_TAM}/docs/pmlp_sozdizimi.md belgesinden öğrendiğim kurallar:

1. [Kural 1]: [açıklama]
   Örnek: [kod örneği]

2. [Kural 2]: [açıklama]
   Örnek: [kod örneği]

(vs... sadece görevinle ilgili kuralları listele)

🛡️ TAAHHÜTLERİM:
✅ Önceki YZ'lerin yaptıklarını BOZMAYACAĞIM
✅ Kendimden kural ÇIKARMAYACAĞIM
✅ Çelişki yaşarsam Üst Akıl/Kullanıcıya SORACAĞIM
✅ İnisiyatifle mimari/felsefi karar ALMAYACAĞIM
✅ Git workflow, raporlama ve döküman güncelleme protokolünü eksiksiz takip edeceğim

**⚠️ GÖREV BİTİŞ PROTOKOLÜ (MUTLAKA YAPACAĞIM):**
✅ Ana TODO'da görevli olduğum bölümü tamamlandı olarak işaretleyeceğim: `1-TODO_{TODO_TAM}.md` dosyasında SADECE benim Phase/Task checkbox'larımı işaretleyeceğim
✅ Ana TODO'da kendi bölümümün sonuna test sonuçlarını ekleyeceğim: Hangi testleri çalıştırdım, sonuçları ne oldu, nasıl doğruladım
✅ NEXT_AI'ı güncelleyeceğim: Phase durumunu (⏳ → ✅) ve sonraki YZ bilgisini yazacağım
✅ Rapor yazacağım: `TODO_{TODO_TAM}/{TODO_KISA}_YZ/completed/{TODO_KISA}_YZ_0X_TAMAMLANDI.md` oluşturacağım (yapılan testler, sonuçlar, istatistikler)
✅ Git push yapacağım: Branch'imi push edeceğim (merge yapmayacağım)

📜 PROJE PRENSİPLERİ (İhlal Etmeyeceğim):
1. x86-64 - Backend assembly
2. MONOLITHIC - Runtime tek dosyada
3. NUMERIC - BigDecimal tabanlı
4. MODULAR - Core libs modüler
5. BOOTSTRAP - Stage0 C, Stage1 MLP

🚀 DURUM:
Tüm ön hazırlıkları tamamladım.
Kendimi hazır hissediyorum.
Onayınızla görevime başlayabilirim.

Başlamak için onay bekliyor.
```

#### ⚠️ ÖNEMLİ NOTLAR

1. **YZ Numaranı NEXT_AI'den öğren** - Kendin belirleme!
2. **Sadece görevinle ilgili syntax kurallarını listele** - Hepsini değil!
3. **Proje prensiplerini yukarıdan kopyala** - Değiştirme!
4. **Onay almadan BAŞLAMA!** - Kullanıcı onay verene kadar bekle

---

## 🔧 ÇALIŞIRKEN

### Proje Spesifik Komutlar

**Bu proje için (mlp-original):**

```bash
# Bootstrap compiler ile derle
./mlpc program.mlp program.asm

# Assemble ve link
nasm -f elf64 program.asm -o program.o
gcc program.o runtime/runtime.o -o program -lm

# Çalıştır
./program
```

**Not:** Yeni projede bu bölüm projeye göre değiştirilir.

### Git Workflow

**Strateji:** Her YZ kendi branch'ında çalışır, merge/pull yapılmaz, conflict olmaması garantilenir.

```bash
# İlk YZ için: Ana TODO branch'ını oluştur
git checkout -b TODO_{TODO_TAM}

# Her YZ için: YZ-spesifik branch oluştur
git checkout -b TODO_{TODO_TAM}_YZ_XX

# Düzenli commit (her değişiklikten sonra)
git add dosya.ext
git commit -m "{TODO_KISA}_YZ_XX: [dosya] - [açıklama]"

# Push et (MERGE/PULL YAPMA!)
git push origin TODO_{TODO_TAM}_YZ_XX
```

**Örnek (STAGE0 için):**
```bash
# Ana branch
git checkout -b TODO_STAGE0_PMLP

# YZ_01 branch'ı
git checkout -b TODO_STAGE0_PMLP_YZ_01
git commit -m "STAGE0_YZ_01: mlp_compiler.c - Token enum'a exit/continue eklendi"
git push origin TODO_STAGE0_PMLP_YZ_01
```

**ÖNEMLİ:**
- ❌ `git merge` yapma
- ❌ `git pull` yapma
- ✅ Sadece kendi branch'inde commit + push
- ✅ Üst Akıl merge işlemlerini yapar

**Not:** Branch adı ve commit formatı projeye göre değişebilir.

---

### Git Merge Stratejisi

**Strateji:** Üst Akıl merge'i yönetir, Mastermind onaylıyor.

#### Merge Süreci

**1. YZ Görevini Bitirince:**
```bash
# YZ yapar:
git commit -m "{TODO_KISA}_YZ_XX: [dosya] - [açıklama]"
git push origin TODO_{TODO_TAM}_YZ_XX

# Rapor yazar: {TODO_KISA}_YZ_XX_TAMAMLANDI.md
# NEXT_AI_START_HERE.md'yi günceller
```

**2. Üst Akıl Kontrol Eder:**
```
✅ Test başarılı mı?
✅ Rapor yazıldı mı?
✅ Commit formatı doğru mu?
✅ TODO_KURALLARI.md'ye uyulmuş mu?
```

**3. Üst Akıl Merge Yapar:**
```bash
# Ana TODO branch'ine merge (fast-forward only)
git checkout TODO_{TODO_TAM}
git merge --ff-only TODO_{TODO_TAM}_YZ_XX
git push origin TODO_{TODO_TAM}
```

**4. Conflict Durumunda:**
```
❌ Fast-forward merge başarısız
❌ Conflict var

→ Mastermind'a escalate et!
→ Mastermind karar verir:
   A) YZ'ye düzelt yeniden commit et
   B) Manuel conflict resolution
   C) Branch'i rebase et
```

**Conflict Önleme Stratejisi:**
- Her YZ farklı dosyalarda çalışır
- Aynı dosyada çalışma gerekiyorsa sırayla (YZ_01 bitsin, sonra YZ_02)
- TODO'lar arası çakışmalar Mastermind koordine eder

---

### Test Stratejisi

**Genel Prensipler (📋 tüm projeler):**

**1. Test Seviyeleri:**
```
Birim Test     → Her fonksiyon test edilmeli
Entegrasyon    → Her Phase sonunda çalışan kod
Regresyon      → Eski özellikler bozulmadı mı?
Manuel Test    → Karmaşık senaryolar
```

**2. Test Zorunluluğu:**
```
❌ Test yok     = Görev tamamlanmadı
❌ Test fail    = Görev tamamlanmadı
✅ Test pass    = Görev tamamlanabilir
```

**3. Test Coverage Hedefi:**
- **Minimum:** %70 (kabul edilebilir)
- **Hedef:** %80-90 (ideal)
- **Kritik fonksiyonlar:** %100 (zorunlu)

**4. Test Periyodu:**
```
Her Task sonrası      → YZ test eder
Her Phase sonrası     → Üst Akıl doğrular
Her TODO bitiminde    → Mastermind onaylar
```

---

**🏗️ Bu Proje İçin Test Stratejisi:**

**Test Komutları:**
```bash
# Derleme testi
./mlpc program.mlp program.asm

# Assembly ve link
nasm -f elf64 program.asm -o program.o
gcc program.o runtime/runtime.o -o program -lm

# Çalıştırma testi
./program

# Beklenen çıktı karşılaştırma
diff <(./program) expected_output.txt
```

**Test Dosyaları Konumu:**
- Birim test: `tests/unit/`
- Entegrasyon: `tests/integration/`
- PMLP syntax: `tests/pmlp/`
- TODO spesifik: `TODO_{TODO_TAM}/tests/`

**Test Başarısızlık Protokolü:**
1. **İlk deneme:** Hatayı analiz et, düzelt
2. **İkinci deneme:** Farklı yaklaşım dene
3. **Üçüncü deneme:** Hala başarısız → **Üst Akıl'e sor**

**Otomatik Test (Gelecek):**
```bash
# CI/CD pipeline (ileride eklenecek)
.github/workflows/test.yml
- Her commit'te otomatik test
- Test fail → PR blocked
```

--- Çalışma Süreci

**Genel adımlar (tüm projeler için):**

1. **Değişiklikleri yap**
   - TODO'da belirtilen görevi yap
   - Proje spesifik kurallara uy

2. **Test et**
   
   📖 **Detaylar:** [Test Stratejisi](#test-stratejisi) bölümüne bakın.
   
   **Kısa özet:**
   ```bash
   # Proje spesifik test komutu
   ./test_komutu
   ```
   
   ⚠️ Test başarısız = Görev tamamlanmadı!

3. **Commit (yedekleme yerine)**
   
   📖 **Detaylar:** [Git Workflow](#git-workflow) bölümüne bakın.
   
   **Kısa özet:**
   ```bash
   git add dosya.ext
   git commit -m "{TODO_KISA}_YZ_XX: dosya.ext - [açıklama]"
   ```
   
   ⚠️ Git commit yedekleme görevi görür. `.backup` dosyaları oluşturma.

**Bu proje için ek adımlar:**

🏗️ Syntax migration:
- 📖 **Tam Kılavuz:** [`TODO_STAGE0_PMLP/docs/syntax_migration_guide.md`](TODO_STAGE0_PMLP/docs/syntax_migration_guide.md)
- Otomatik scriptler ve manuel dönüşüm kuralları belgede
- ⚠️ **String literal ve ondalık sayılara dokunma!**

---

## 🚨 SORUN DURUMLARI VE PROTOKOL

### Test Başarısız Olursa

**🚫 YASAKLAR:**
- ❌ Pseudo code kullanma
- ❌ Stub fonksiyon bırakma
- ❌ Hack/geçici çözüm yapma
- ✅ Her şey gerçek, çalışır kod olmalı

**Her phase sonunda test ZORUNLU. Başarısız test = görev tamamlanmadı.**

1. **İlk deneme:** Hatayı analiz et, düzelt
2. **İkinci deneme:** Farklı yaklaşım dene
3. **Üçüncü deneme:** Hala başarısız → **Üst Akıl'e sor**

```
Üst Akıl/Kullanıcı,

[dosya.ext]'de [görev] yaparken test başarısız oluyor.

Denediğim yaklaşımlar:
1. [yaklaşım 1] → Sonuç: [hata mesajı]
2. [yaklaşım 2] → Sonuç: [hata mesajı]
3. [yaklaşım 3] → Sonuç: [hata mesajı]

Nasıl ilerlemeliyim?
```

### Belirsizlik/Çelişki Durumunda

Şu durumlarda **MUTLAKA** Üst Akıl'e sor:

❓ **TODO'da çelişki var**
```
TODO'da [X] diyor ama [Y] belgede [Z] yazıyor.
Hangisine uymalıyım?
```

❓ **Syntax kuralı belirsiz**
```
[kod örneği] için hangi syntax kullanmalıyım?
pmlp_sozdizimi.md'de net değil.
```

❓ **Görev kapsamı belirsiz**
```
TODO'da [görev] diyor. [X] dosyası da bu kapsama giriyor mu?
```

### Context Dolduğunda (Token Limiti)

Context doldu ve devam edemiyorsan:

```
Sanırım yoruldum. Şu ana kadar [X] görevi tamamladım.

Tamamlanan:
- [x] görev 1
- [x] görev 2
- [ ] görev 3 (yarıda kaldı)

Devam eden görevlerimi yeni bir YZ'ye verirseniz daha iyi olur.
İsterseniz ben sonuç raporumu yazıp şimdiye kadar yaptıklarımı commit edebilirim.

Nasıl ilerlemeliyim?
```

**Kullanıcı onayından sonra:**
1. Yarım kalan kodu commit et (açıklama: "Yarım kalan görev - devam edilecek")
2. Rapor yaz (hangi görevler yarım kaldı belirt)
3. NEXT_AI_START_HERE.md'yi güncelle (yarım görevleri belirt)

### Karar Verme Yetkileri

#### ✅ Kendi Karar Verebilirsin

- Algoritma seçimi (binary search vs linear search)
- Değişken isimlendirme
- Kod organizasyonu (fonksiyon bölme)
- Yorum ekleme/düzenleme
- Performans optimizasyonları (mimari ihlal etmeden)
- Test case yazma
- Hata mesajı düzenleme

#### ❌ MUTLAKA Üst Akıl'e Sorman Gereken Durumlar

**Mimari İhlal Riski:**

1. **Merkezi/Monolitik Dosya Oluşturmak**
   ```
   -- ❌ SORULMADAN YAPMA!
   -- Tüm string fonksiyonlarını tek dosyada toplamak
   ```

2. **Global Değişken Tanımlamak**
   ```mlp
   -- ❌ SORULMADAN YAPMA!
   global numeric cache_size = 1000
   ```

3. **Stateless İhlali**
   ```mlp
   -- ❌ SORULMADAN YAPMA!
   function parse(string text)
       -- fonksiyon içinde static/global state kullanmak
   end_function
   ```

4. **STO (Smart Type Optimization) İhlali**
   ```mlp
   -- ❌ SORULMADAN YAPMA!
   function is_int64(numeric x) returns boolean
       -- Kullanıcıya dahili tip açığa çıkarmak
   end_function
   ```
   📖 **Referans:** `TODO_{TODO_TAM}/docs/STO_PRINCIPLES.md`

5. **Syntax Değişikliği**
   ```
   -- ❌ SORULMADAN YAPMA!
   -- Yeni keyword eklemek
   -- Token yapısını değiştirmek
   ```

6. **Runtime Değişikliği**
   ```c
   // ❌ SORULMADAN YAPMA!
   // runtime.c'de değişiklik
   ```

**Kural:** Mimari prensiplere (x86-64, MONOLITHIC, NUMERIC, MODULAR, BOOTSTRAP) aykırı olabilecek HER ŞEY sorulmalı.

---

## ✅ YAPILACAKLAR (DO)

| Yapılacak | Açıklama |
|-----------|----------|
| ✅ TODO'daki görevi yap | Ana görev listesinde ne yazıyorsa |
| ✅ Test her adımda | Değişiklikler çalışmalı |
| ✅ Git commit sık | Küçük, anlamlı commitler (yedekleme yerine) |
| ✅ Rapor yaz | Tamamlayınca YZ_XX_TAMAMLANDI.md |

**Not:** Proje spesifik ek gereksinimler TODO'da belirtilir.

---

## ❌ YAPILMAYACAKLAR (DON'T)

| Yasak | Neden |
|-------|-------|
| ❌ Stable koda dokunma | TODO dışı değişiklik yasak |
| ❌ Yeni özellik ekleme | Sadece TODO'daki görev |
| ❌ Merge/pull yapma | Üst Akıl merge eder, conflict önlenir |
| ❌ Pseudo/stub/hack | Gerçek, çalışır kod şart |
| ❌ .backup dosyaları | Git commit yedekleme yapar |
| ❌ "Detaylandırayım mı?" | Direkt yap, sor sorma |
| ❌ Yeni TODO yazma | TODO yeterli, ekleme yapma |
| ❌ Ana dizinde test/belge | Kullanıcıdan izin al |
| ❌ Mevcut belgeleri değiştir | Kullanıcıdan izin al |

**Bu proje için ek yasaklar:**
- ❌ Runtime değiştirme: C runtime stable, dokunma
- ❌ String/sayı değiştirme: `"a, b"` ve `3,14` korunur
- ❌ Assembly değiştirme: Compiler çıktısı, sen değil
- ❌ **Test dosyalarını düzenleme:** Görevi SADECE compiler'ı düzenlemek! Test başarısız olursa compiler'ı düzelt, test dosyasını DEĞİL!
  - Test dosyası syntax hatası veriyor → Compiler'a o syntax'ı ekle
  - Test dosyası yanlış sonuç veriyor → Compiler'ı düzelt
  - Test dosyası oluşturabilirsin ama TODO'da belirtilmedikçe MEVCUT test dosyalarını düzeltme!
- ❌ **Belge güncelleme kısıtlamaları:** YZ SADECE izin verilen belgeleri güncelleyebilir:
  - ✅ İzin verilen: `1-TODO_{TODO_TAM}.md` (Ana TODO - checkbox'ları işaretle)
  - ✅ İzin verilen: `TODO_{TODO_TAM}/{TODO_KISA}_YZ/NEXT_AI_START_HERE.md` (YZ belgesi - checkbox'ları işaretle)
  - ✅ İzin verilen: `TODO_{TODO_TAM}/{TODO_KISA}_YZ/completed/{TODO_KISA}_YZ_0X_TAMAMLANDI.md` (Rapor - oluştur)
  - ❌ YASAK: `/NEXT_AI_START_HERE.md` (Root belge - Üst Akıl günceller)
  - ❌ YASAK: `TODO_KURALLARI.md` (Kurallar - Kullanıcı/Mastermind günceller)
  - ❌ YASAK: `TODO_{TODO_TAM}/docs/*` (Teknik belgeler - Kullanıcı/Mastermind günceller)
  - ❌ YASAK: Diğer TODO'ların belgeleri
- ❌ **Scope dışı değişiklik:** Hiçbir YZ, TODO'da kendisine tanımlı görev/dosya/bölüm haricinde değişiklik YAPAMAZ!
  - Örnek: Phase 1 YZ'si sadece Phase 1 task'larını yapar, Phase 2'ye DOKUNMAZ
  - Örnek: STAGE0 YZ'si RUNTIME veya BACKEND TODO'larına DOKUNMAZ
  - Örnek: YZ kendi rapor dosyasını yazar, başkasının raporunu DEĞİŞTİRMEZ
- ❌ Ana dizinde dosya oluşturma: Test dosyası (`test_*.mlp`) veya belge (`.md`) ana dizine konulamaz
  - Test: `tests/` veya `TODO_{TODO_TAM}/tests/` içinde
  - Belge: `TODO_{TODO_TAM}/docs/` veya `TODO_{TODO_TAM}/{TODO_KISA}_YZ/` içinde
  - İstisna: Kullanıcıdan açık izin alınmışsa

**İHLAL = GÖREV DURDURULUR!**

---

## 📊 İLERLEME RAPORLAMA

### Görev Bitirme Protokolü

Her görev bittiğinde **mutlaka** şu adımları izle:

1. ✅ **Ana TODO'yu güncelle** - `1-TODO_{TODO_TAM}.md` dosyasında ilgili Phase checkbox'larını işaretle
   ```markdown
   İlerleme:
   - [x] Task 1.1: Token Enum'a Ekle  ← İşaretle!
   - [x] Task 1.2: Keyword Table'a Ekle
   - [ ] Task 1.3: Parser'da Handle Et
   ```

2. ✅ **YZ NEXT_AI'yı güncelle** - `TODO_{TODO_TAM}/{TODO_KISA}_YZ/NEXT_AI_START_HERE.md` dosyasında checkbox'ları işaretle
   ```markdown
   DURUM TAKİBİ:
   - [x] Task 1.1: Token Enum'a Ekle  ← İşaretle!
   - [x] Task 1.2: Keyword Table'a Ekle
   ```

3. 📝 **Rapor oluştur** - `TODO_{TODO_TAM}/{TODO_KISA}_YZ/completed/{TODO_KISA}_YZ_XX_TAMAMLANDI.md` dosyası yaz

4. 🔀 **Push et** - Branch'ini push et (merge yapma!)

**⚠️ ÖNEMLİ:** Root `/NEXT_AI_START_HERE.md` dosyasını GÜNCELLEME! Onu Üst Akıl günceller.

---

### Her Görev Sonunda

`TODO_{TODO_TAM}/{TODO_KISA}_YZ/{TODO_KISA}_YZ_XX_TAMAMLANDI.md` dosyası oluştur:

**Format:**
```markdown
# {TODO_KISA}_YZ_XX Görev Raporu

**Tarih:** [tarih]
**Branch:** [branch_adı]
**Görev:** [görev açıklaması]

## Yapılanlar
- [x] görev 1
- [x] görev 2
...

## İstatistikler
- Toplam X: Y
- Süre: ~N saat

## Sorunlar
- Problem 1: [açıklama] → Çözüm: [açıklama]

## Test Sonuçları
- Test 1: ✅ Başarılı
- Test 2: ✅ Başarılı

## Yarım Kalan Görevler (Varsa)
- Görev 3: [açıklama] - [nerede kaldı] - [devam notu]

## Sonraki YZ İçin Notlar
- [önemli not 1]
```

**Not:** İstatistikler ve test sonuçları projeye göre değişir. Yarım kalan görev yoksa o bölümü dahil etme.

### NEXT_AI_START_HERE.md Güncelle

Görevin bitince `TODO_{TODO_TAM}/{TODO_KISA}_YZ/NEXT_AI_START_HERE.md`'yi güncelle:

**Tam tamamlanmış görev için:**
```markdown
**Son Güncelleme:** [tarih]
**Önceki YZ:** {TODO_KISA}_YZ_XX
**Durum:** ✅ Tamamlandı

## ✅ {TODO_KISA}_YZ_XX Sonuçları
[özet]

## 📍 Sonraki YZ İçin Görev
[görev açıklaması]
```

**Yarım kalan görev için:**
```markdown
**Son Güncelleme:** [tarih]
**Önceki YZ:** {TODO_KISA}_YZ_XX
**Durum:** ⏸️ Yarım Kaldı (Context doldu)

## ⏸️ {TODO_KISA}_YZ_XX Sonuçları (Yarım)
Tamamlanan:
- [x] görev 1
- [x] görev 2

Yarım kalan:
- [ ] görev 3 - [detay]

## 🔄 Sonraki YZ Nerede Devam Edecek
[dosya], [satır], [durum açıklaması]
```

---

## 🚫 YASAKLAR (KESİN UYULMALI)

### Genel Yasaklar (Tüm Projeler)

1. **TODO Dışı Değişiklik YASAK**
   - Sadece TODO'da yazan işleri yap
   - Ek özellik, iyileştirme yapma

2. **Merge/Pull Yapma YASAK**
   - ❌ `git merge` yapma
   - ❌ `git pull` yapma
   - ✅ Sadece kendi branch'inde commit + push
   - ✅ Üst Akıl merge işlemlerini yapar
   - **Sebep:** Conflict olmaması garantilenir

3. **Pseudo/Stub/Hack YASAK**
   - ❌ Pseudo code bırakma
   - ❌ Stub fonksiyon yapma
   - ❌ Geçici çözüm/hack kullanma
   - ✅ Her şey gerçek, çalışır kod olmalı
   - **Test başarısız = görev tamamlanmadı**

4. **Yedek Dosya (.backup) YASAK**
   - ❌ `.backup` dosyaları oluşturma
   - ✅ Git commit yedekleme görevi görür

5. **Yeni TODO Yazma YASAK**
   - Mevcut TODO yeterli
   - Ekleme yapma

6. **Dosya Oluşturma Kısıtlamaları**
   - ❌ Ana dizinde test dosyası veya belge oluşturamazsın
   - ❌ Mevcut belgeleri değiştiremezsin
   - ✅ Test dosyaları: `tests/` veya `TODO_{TODO_TAM}/tests/` içinde
   - ✅ Belgeler: `TODO_{TODO_TAM}/docs/` veya `TODO_{TODO_TAM}/{TODO_KISA}_YZ/` içinde
   - ⚠️ İstisna: Kullanıcıdan açık izin alırsan ana dizinde dosya oluşturabilirsin

### Proje Spesifik Yasaklar

**Bu proje için:**

1. **String Literal Değiştirme YASAK**
   ```mlp
   -- ❌ YANLIŞ:
   string msg = "Ali; Veli"  -- BOZULDU!
   
   -- ✅ DOĞRU:
   string msg = "Ali, Veli"  -- Virgül korunur
   ```

2. **Ondalık Sayı Değiştirme YASAK**
   ```mlp
   -- ❌ YANLIŞ:
   numeric pi = 3;14  -- BOZULDU!
   
   -- ✅ DOĞRU:
   numeric pi = 3,14  -- Virgül ondalık ayırıcı
   ```

3. **Runtime Değiştirme YASAK**
   - `runtime/runtime.c` dosyasına DOKUNMA!

**Not:** Yeni projede bu bölüm projeye göre değiştirilir.

---

## 📚 Referans Komutlar

**Proje spesifik komutlar (bu proje için):**

```bash
# Proje kök dizini
cd /home/pardus/projeler/mlp-original

# .mlp dosyası sayısı
find . -name "*.mlp" -type f | wc -l

# Tip belirtilmemiş fonksiyonlar
grep -rE "function\s+\w+\([^)]*\w+," --include="*.mlp" | wc -l

# Eski syntax kullanımı
grep -r "end if" --include="*.mlp" | wc -l

# Test derleme
./mlpc examples/hello_english.mlp test.asm
```

**Not:** Yeni projede bu komutlar projeye göre değiştirilir.

---

## 🎯 ÖZET

1. **OKU:** TODO_STAGE0_PMLP/docs/syntax_migration_guide.md
2. **ANLA:** Eski vs PMLP farkları
3. **DÖNÜŞTÜR:** Dosya dosya, commit commit
4. **TEST ET:** Her dosya derlenebilmeli
5. **RAPOR YAZ:** {TODO_KISA}_YZ_XX_TAMAMLANDI.md
6. **GÜNCELLE:** TODO_{TODO_TAM}/{TODO_KISA}_YZ/NEXT_AI_START_HERE.md

**İYİ ÇALIŞMALAR! 🚀**

---

## 📝 TEMPLATE KULLANIM KILAVUZU

### Yeni TODO İçin Hazırlık

**Adım 1: Yapılandırma Değerlerini Belirle**

Örnek 1 - Stage0 PMLP Syntax:
```
TODO Kısa Adı:    STAGE0
TODO Tam Adı:     STAGE0_PMLP
TODO Belgesi:     1-TODO_STAGE0_PMLP_SYNTAX.md
```

Örnek 2 - Runtime Integration:
```
TODO Kısa Adı:    RUNTIME
TODO Tam Adı:     RUNTIME_INT
TODO Belgesi:     2-TODO_RUNTIME_INTEGRATION.md
```

Örnek 3 - Backend Migration:
```
TODO Kısa Adı:    BACKEND
TODO Tam Adı:     BACKEND_LLVM
TODO Belgesi:     3-TODO_BACKEND_TO_LLVM.md
```

**Adım 2: Klasör Yapısını Oluştur**

```bash
# TODO klasörünü oluştur
mkdir -p TODO_{TODO_TAM}/{TODO_KISA}_UA
mkdir -p TODO_{TODO_TAM}/{TODO_KISA}_YZ
mkdir -p TODO_{TODO_TAM}/docs

# Örnek (RUNTIME için):
mkdir -p TODO_RUNTIME_INT/RUNTIME_UA
mkdir -p TODO_RUNTIME_INT/RUNTIME_YZ
mkdir -p TODO_RUNTIME_INT/docs

# NEXT_AI_START_HERE.md dosyasını kopyala
cp TODO_STAGE0_PMLP/STAGE0_YZ/NEXT_AI_START_HERE.md \
   TODO_RUNTIME_INT/RUNTIME_YZ/NEXT_AI_START_HERE.md

# docs dosyalarını kopyala (eğer aynıysa)
cp TODO_STAGE0_PMLP/docs/* TODO_RUNTIME_INT/docs/
```

**Adım 3: Bu Template'i Kopyala**

```bash
# Yeni TODO için kurallar dosyası oluştur
cp TODO_KURALLARI.md TODO_KURALLARI_RUNTIME.md

# Veya aynı dosyayı kullan, sadece en üstteki yapılandırmayı değiştir
```

**Adım 4: Placeholder'ları Değiştir**

Template'de şu ifadeleri gerçek değerlerle değiştir:
- `{TODO_KISA}` → Kısa ad (örn: `STAGE0`, `RUNTIME`, `BACKEND`)
- `{TODO_TAM}` → Tam ad (örn: `STAGE0_PMLP`, `RUNTIME_INT`)
- `{TODO_BELGE}` → Belge adı (örn: `1-TODO_STAGE0_PMLP_SYNTAX.md`)

**Otomatik değiştirme (Linux/Mac):**
```bash
# Değişkenleri tanımla
TODO_KISA="RUNTIME"
TODO_TAM="RUNTIME_INT"
TODO_BELGE="2-TODO_RUNTIME_INTEGRATION.md"

# Template'i kopyala ve değiştir
sed "s/{TODO_KISA}/$TODO_KISA/g" TODO_KURALLARI.md | \
sed "s/{TODO_TAM}/$TODO_TAM/g" | \
sed "s/{TODO_BELGE}/$TODO_BELGE/g" > TODO_KURALLARI_RUNTIME.md
```

### Klasör Yapısı Örnekleri

**Birden Fazla TODO ile Proje:**
```
mlp-original/
├── docs/                        # GENEL DÖKÜMANLAR
│   ├── syntax_migration_guide.md
│   └── specs/
│
├── TODO_STAGE0_PMLP/           # 1. TODO
│   ├── STAGE0_UA/
│   │   └── STAGE0_UA_01_rapor.md
│   ├── STAGE0_YZ/
│   │   ├── NEXT_AI_START_HERE.md
│   │   ├── STAGE0_YZ_01_TAMAMLANDI.md
│   │   └── STAGE0_YZ_02_TAMAMLANDI.md
│   └── docs/
│       ├── pmlp_sozdizimi.md
│       └── STO_PRINCIPLES.md
│
├── TODO_RUNTIME_INT/           # 2. TODO
│   ├── RUNTIME_UA/
│   ├── RUNTIME_YZ/
│   │   ├── NEXT_AI_START_HERE.md
│   │   └── RUNTIME_YZ_01_TAMAMLANDI.md
│   └── docs/
│
├── TODO_BACKEND_LLVM/          # 3. TODO
│   ├── BACKEND_UA/
│   ├── BACKEND_YZ/
│   │   └── NEXT_AI_START_HERE.md
│   └── docs/
│
├── 1-TODO_STAGE0_PMLP_SYNTAX.md
├── 2-TODO_RUNTIME_INTEGRATION.md
├── 3-TODO_BACKEND_TO_LLVM.md
├── TODO_KURALLARI.md           # Bu template
└── NEXT_AI_START_HERE.md       # Ana giriş noktası
```

### Dosya İsimlendirme Kuralları

**YZ Raporları:**
- Format: `{TODO_KISA}_YZ_{SIRA}_TAMAMLANDI.md`
- Örnekler:
  - `STAGE0_YZ_01_TAMAMLANDI.md`
  - `RUNTIME_YZ_03_TAMAMLANDI.md`
  - `BACKEND_YZ_02_TAMAMLANDI.md`

**Git Branch'leri:**
- Format: `todo_{TODO_KISA}_YZ_{SIRA}`
- Örnekler:
  - `todo_STAGE0_YZ_01`
  - `todo_RUNTIME_YZ_02`
  - `todo_BACKEND_YZ_01`

**Commit Mesajları:**
- Format: `{TODO_KISA}_YZ_{SIRA}: [dosya] - [açıklama]`
- Örnekler:
  - `STAGE0_YZ_01: mlp_compiler.c - Token enum güncellendi`
  - `RUNTIME_YZ_02: runtime.c - Async desteği eklendi`
  - `BACKEND_YZ_01: codegen.c - LLVM IR oluşturma başladı`

---

**Versiyon:** 2.0 (Template)  
**Son Güncelleme:** 25 Aralık 2025  
**Template Türü:** Çoklu TODO Desteği
