# 🎯 TEMPLATE SİSTEMİ KULLANIM REHBERİ

**Proje:** Multi-TODO Template System  
**Tarih:** 25 Aralık 2025  
**Versiyon:** 1.0

---

## 📖 İÇİNDEKİLER

1. [Genel Bakış](#-genel-bakış)
2. [Hızlı Başlangıç](#-hızlı-başlangıç)
3. [Template Yapısı](#-template-yapısı)
4. [Yeni TODO Oluşturma](#-yeni-todo-oluşturma)
5. [Proje Konfigürasyonu](#-proje-konfigürasyonu)
6. [Özelleştirme](#-özelleştirme)
7. [Sorun Giderme](#-sorun-giderme)
8. [Best Practices](#-best-practices)

---

## 🎯 GENEL BAKIŞ

### Ne İçin Kullanılır?

Bu template sistemi, **çok TODO'lu projelerde** standart ve tutarlı yapı oluşturmak için tasarlanmıştır.

**Kullanım Alanları:**
- ✅ Yeni TODO oluşturma (STAGE0, RUNTIME, BACKEND, vb.)
- ✅ Mastermind/ÜstAkıl/YZ hiyerarşisi kurma
- ✅ Proje prensiplerini koruma
- ✅ Standart dokümantasyon yapısı

### Ne DEĞIL?

- ❌ Kod generator (sadece yapı ve döküman)
- ❌ Build sistemi (projenin build sistemi ayrı)
- ❌ Task runner (sadece TODO yapısı oluşturur)

---

## 🚀 HIZLI BAŞLANGIÇ

### 5 Dakikada İlk TODO

```bash
# 1. Proje kök dizinine git
cd /proje/dizini

# 2. Script'i çalıştır
./templates/setup_todo.sh MYTEST TEST_TODO 1-TODO_TEST.md

# 3. Sonuç:
# TODO_TEST_TODO/ klasörü oluşturuldu!
# ├── MYTEST_MM/
# ├── MYTEST_UA/
# ├── MYTEST_YZ/
# └── docs/
```

**Hepsi bu kadar!** 🎉

---

## 📁 TEMPLATE YAPISI

### Klasör Hiyerarşisi

```
templates/
├── setup_todo.sh                    # Ana kurulum scripti
├── TODO_TEMPLATE/                   # Template klasörü
│   ├── TODO_KISA_MM/               # Mastermind template
│   │   └── Mastermind_buradan_basla.md.template
│   │
│   ├── TODO_KISA_UA/               # Üst Akıl template
│   │   └── Ust_Akil_buradan_basla.md.template
│   │
│   ├── TODO_KISA_YZ/               # Görevli YZ template
│   │   ├── NEXT_AI_START_HERE.md.template
│   │   ├── YZ_RAPOR_TEMPLATE.md.template
│   │   └── completed/              # Tamamlanan raporlar
│   │
│   └── docs/                       # Proje spesifik belgeler
│       └── TEMPLATE_README.md
│
└── README.md                        # Template sistem dökümantasyonu
```

### Placeholder Sistemi

Template dosyalarında kullanılan değişkenler:

| Placeholder | Açıklama | Örnek |
|-------------|----------|-------|
| `{PROJECT_NAME}` | Proje adı | mlp-original |
| `{TODO_KISA}` | TODO kısa adı | STAGE0, RUNTIME |
| `{TODO_TAM}` | TODO tam adı | STAGE0_PMLP, RUNTIME_INT |
| `{TODO_BELGE}` | TODO belge adı | 1-TODO_STAGE0.md |

**Otomatik Değişim:** Script çalıştırıldığında bu placeholder'lar gerçek değerlerle değiştirilir.

---

## 🆕 YENİ TODO OLUŞTURMA

### Adım Adım

#### 1. Bilgileri Hazırla

Önce şunları belirle:

```
TODO Kısa Adı  : Ne ile çağrılacak? (tek kelime, BÜYÜK HARF)
                 Örnek: STAGE0, RUNTIME, BACKEND, PARSER, CODEGEN

TODO Tam Adı   : Klasör adı ne olacak? (ALT ÇİZGİLİ)
                 Örnek: STAGE0_PMLP, RUNTIME_INT, BACKEND_LLVM

TODO Belgesi   : Ana TODO dosyası adı
                 Örnek: 1-TODO_STAGE0_PMLP_SYNTAX.md
```

**İsimlendirme Kuralları:**
- ✅ TODO_KISA: Tek kelime, büyük harf, alt çizgi yok
- ✅ TODO_TAM: Alt çizgili, açıklayıcı
- ✅ TODO_BELGE: Sıra numarası + TODO_ öneki

#### 2. Script'i Çalıştır

```bash
cd /proje/kök/dizini

./templates/setup_todo.sh TODO_KISA TODO_TAM TODO_BELGE
```

**Örnek 1:** Runtime Integration
```bash
./templates/setup_todo.sh RUNTIME RUNTIME_INT 2-TODO_RUNTIME_INTEGRATION.md
```

**Örnek 2:** Parser Refactoring
```bash
./templates/setup_todo.sh PARSER PARSER_REFACTOR 4-TODO_PARSER_REFACTORING.md
```

**Örnek 3:** Test Framework
```bash
./templates/setup_todo.sh TESTFW TEST_FRAMEWORK 5-TODO_TEST_FRAMEWORK.md
```

#### 3. Çıktıyı Kontrol Et

Script başarılıysa göreceksin:

```
✅ TODO başarıyla oluşturuldu!

📂 Oluşturulan yapı:
  TODO_RUNTIME_INT/
  ├── RUNTIME_MM/
  │   └── Mastermind_buradan_basla.md
  ├── RUNTIME_UA/
  │   └── Ust_Akil_buradan_basla.md
  ├── RUNTIME_YZ/
  │   ├── NEXT_AI_START_HERE.md
  │   ├── YZ_RAPOR_TEMPLATE.md
  │   └── completed/
  └── docs/

⚠️ SONRAKİ ADIMLAR:
  1. TODO_RUNTIME_INT/docs/ klasörüne belgeler ekle
  2. 2-TODO_RUNTIME_INTEGRATION.md dosyasını oluştur
  3. Mastermind belgesini gözden geçir
  ...
```

#### 4. TODO Belgesini Oluştur

Ana TODO belgesi (görev listesi):

```bash
# Ana dizinde oluştur
touch 2-TODO_RUNTIME_INTEGRATION.md

# İçeriği doldur
cat > 2-TODO_RUNTIME_INTEGRATION.md << 'EOF'
# TODO: Runtime Integration

**Hedef:** MLP-LLVM runtime.c'yi mlp-original'e entegre et

## Phase 1: Analiz (1 hafta)
- Task 1.1: Mevcut runtime.c'yi incele
- Task 1.2: MLP-LLVM runtime.c'yi incele
- Task 1.3: Farkları tespit et

## Phase 2: Entegrasyon (2 hafta)
- Task 2.1: Ortak fonksiyonları belirle
- Task 2.2: Adapter katmanı yaz
- Task 2.3: Test et

## Phase 3: Test (1 hafta)
- Task 3.1: Birim testler
- Task 3.2: Entegrasyon testleri
- Task 3.3: Performans testleri
EOF
```

#### 5. Proje Spesifik Belgeler Ekle

TODO'ya özel dokümantasyon:

```bash
# Teknik detaylar
cat > TODO_RUNTIME_INT/docs/runtime_api.md << 'EOF'
# Runtime API Referansı

## Fonksiyonlar
...
EOF

# Entegrasyon kılavuzu
cat > TODO_RUNTIME_INT/docs/integration_guide.md << 'EOF'
# Entegrasyon Kılavuzu

## Adımlar
...
EOF
```

#### 6. Belgeleri Özelleştir

**Mastermind:**
```bash
vi TODO_RUNTIME_INT/RUNTIME_MM/Mastermind_buradan_basla.md

# Kontrol et:
# - Proje prensipleri doğru mu?
# - Örnekler RUNTIME için anlamlı mı?
# - Referanslar doğru mu?
```

**Üst Akıl:**
```bash
vi TODO_RUNTIME_INT/RUNTIME_UA/Ust_Akil_buradan_basla.md

# Güncelle:
# - Mevcut durum metrikleri
# - Phase/Task yapısı
# - İlk YZ için görev tanımı
```

**YZ:**
```bash
vi TODO_RUNTIME_INT/RUNTIME_YZ/NEXT_AI_START_HERE.md

# Hazırla:
# - İlk görev detayları
# - Test senaryoları
# - Örnek komutlar
```

---

## ⚙️ PROJE KONFİGÜRASYONU

### project_config.yaml

Bu dosya projenin merkezi konfigürasyonudur.

**Lokasyon:** Proje kök dizini

**İçerik:**

```yaml
project:
  name: "mlp-original"
  full_name: "MLP Compiler"
  description: "Bootstrap compiler"
  
principles:
  - name: "MODULAR"
    description: "Modüler yapı"
    critical: true
  
  - name: "STATELESS"
    description: "Pure functions"
    critical: true

tech_stack:
  languages: ["C", "MLP", "Assembly"]
  tools: ["GCC", "NASM", "Git"]

commands:
  build: "./mlpc program.mlp program.asm"
  test: "diff <(./program) expected.txt"

testing:
  coverage_target: 80
  coverage_minimum: 70

documentation:
  general: ["TODO_KURALLARI.md", "STRATEGY_OVERVIEW.md"]
  architecture: ["docs/ARCHITECTURE.md"]

todos:
  - id: "TODO_STAGE0_PMLP"
    short: "STAGE0"
    status: "active"
```

### Yeni Proje İçin Güncelleme

```bash
# 1. Proje bilgilerini değiştir
vi project_config.yaml

# 2. Prensipleri güncelle
# principles: bölümünü projeye göre yaz

# 3. Komutları güncelle
# commands: bölümünü projeye göre yaz

# 4. Kaydet ve kapat
```

---

## 🎨 ÖZELLEŞTİRME

### Yeni Proje İçin Template Adapte Etme

#### 1. project_config.yaml Oluştur

```bash
cat > project_config.yaml << 'EOF'
project:
  name: "yeni-proje"
  full_name: "Yeni Proje Adı"
  description: "Proje açıklaması"

principles:
  - name: "PRENSIP1"
    description: "İlk prensip"
  - name: "PRENSIP2"
    description: "İkinci prensip"

# ... devamı
EOF
```

#### 2. Template Dosyalarını Güncelle

```bash
# Örnek bölümleri değiştir
vi templates/TODO_TEMPLATE/TODO_KISA_MM/Mastermind_buradan_basla.md.template

# Proje spesifik örnekleri kaldır/değiştir
# Ama {PLACEHOLDER}'ları koru!
```

#### 3. setup_todo.sh İncele

Script projeye özel değişiklik gerektirmez ama inceleyebilirsin:

```bash
vi templates/setup_todo.sh

# Gerekirse özelleştir (genelde gerekmez)
```

---

## 🐛 SORUN GİDERME

### Yaygın Hatalar ve Çözümler

#### 1. "project_config.yaml bulunamadı"

**Neden:** Script proje kök dizininde çalıştırılmadı

**Çözüm:**
```bash
cd /proje/kök/dizini
./templates/setup_todo.sh ...
```

#### 2. "templates/TODO_TEMPLATE/ bulunamadı"

**Neden:** Template klasörü eksik veya yanlış yerde

**Çözüm:**
```bash
# Template'in varlığını kontrol et
ls -la templates/TODO_TEMPLATE/

# Yoksa yeniden kur
git pull  # veya template'i yeniden oluştur
```

#### 3. Placeholder'lar Değişmedi

**Neden:** Script sed komutunda sorun olabilir

**Çözüm:** Manuel değiştir
```bash
cd TODO_PROBLEMLI/

# Tüm .md dosyalarında değiştir
find . -type f -name "*.md" -exec sed -i \
  -e 's/{TODO_KISA}/GERCEK_AD/g' \
  -e 's/{TODO_TAM}/TAM_AD/g' \
  -e 's/{PROJECT_NAME}/proje-adi/g' \
  {} \;
```

#### 4. Klasör İsimleri Yanlış

**Neden:** Script mv komutunda sorun olabilir

**Çözüm:** Manuel düzelt
```bash
cd TODO_RUNTIME_INT/
mv TODO_KISA_MM RUNTIME_MM
mv TODO_KISA_UA RUNTIME_UA  
mv TODO_KISA_YZ RUNTIME_YZ
```

#### 5. "Permission Denied"

**Neden:** Script çalıştırma yetkisi yok

**Çözüm:**
```bash
chmod +x templates/setup_todo.sh
```

---

## 💡 BEST PRACTICES

### 1. İsimlendirme

**İyi:**
```
TODO_KISA: PARSER
TODO_TAM:  PARSER_REFACTOR
```

**Kötü:**
```
TODO_KISA: parser_module     ❌ (küçük harf, alt çizgi)
TODO_TAM:  Parser Refactor   ❌ (boşluk)
```

### 2. TODO Belgesi

**Yapı:**
```markdown
# TODO: [Başlık]

**Hedef:** Net hedef

## Phase 1: [Ad] (süre)
- Task 1.1: Detaylı görev
- Task 1.2: Test edilebilir task

## Phase 2: ...
```

**İpuçları:**
- ✅ Her task küçük ve test edilebilir
- ✅ Süre tahmini ekle
- ✅ Bağımlılıkları belirt
- ✅ Test kriterlerini yaz

### 3. Dokümantasyon

**TODO/docs/ Yapısı:**
```
TODO_RUNTIME_INT/docs/
├── README.md              # Genel bakış
├── api_reference.md       # Teknik detay
├── integration_guide.md   # Kılavuz
├── test_scenarios.md      # Test planı
└── troubleshooting.md     # Sorun giderme
```

### 4. Git Workflow

```bash
# Her TODO ayrı branch
git checkout -b TODO_RUNTIME_INT

# YZ'ler alt branch'lerde
git checkout -b TODO_RUNTIME_INT_YZ_01

# Düzenli commit
git commit -m "RUNTIME_YZ_01: runtime.c - analiz tamamlandı"
```

### 5. Bakım

**Düzenli Kontrol:**
- 📅 Her ay: Template'leri gözden geçir
- 📅 Her yeni proje: project_config.yaml güncelle
- 📅 Her TODO sonrası: Template'de eksik var mı bak

---

## 📚 İLERİ SEVİYE

### Çoklu Proje Yönetimi

Birden fazla proje için template:

```
~/templates/
├── mlp-original-template/
│   ├── project_config.yaml
│   └── TODO_TEMPLATE/
│
├── compiler-v2-template/
│   ├── project_config.yaml
│   └── TODO_TEMPLATE/
│
└── shared-scripts/
    └── setup_todo.sh      # Ortak script
```

### CI/CD Entegrasyonu

```yaml
# .github/workflows/todo-check.yml
name: TODO Structure Check

on: [push]

jobs:
  check:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Validate TODO Structure
        run: |
          # Her TODO klasörünün yapısını kontrol et
          ./scripts/validate_todo_structure.sh
```

---

## 🎯 ÖZET

### Template Sistemi ile Kazanımlar

✅ **Hız:** 1-2 dakikada yeni TODO  
✅ **Tutarlılık:** Her TODO aynı yapıda  
✅ **Kalite:** Standart dokümantasyon  
✅ **Bakım:** Merkezi güncelleme  
✅ **Ölçeklenebilirlik:** Sınırsız TODO

### Sonraki Adımlar

1. ✅ Template sistemi kuruldu
2. ⏭️ İlk TODO'yu oluştur
3. ⏭️ Kullanım deneyimini değerlendir
4. ⏭️ Template'i iyileştir
5. ⏭️ Diğer projelere adapte et

---

**Hazırlayan:** mlp-original Ekibi  
**Versiyon:** 1.0  
**Tarih:** 25 Aralık 2025  
**Lisans:** Proje ile aynı

🚀 **Mutlu TODO'lar!**
