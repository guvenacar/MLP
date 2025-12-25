# 📚 TODO_TEMPLATE Kullanım Kılavuzu

**Proje:** {PROJECT_NAME}  
**Tarih:** 25 Aralık 2025  
**Versiyon:** 1.0

---

## 🎯 AMAÇ

Bu template sistemi, yeni TODO'lar için hızlı ve tutarlı yapı oluşturmayı sağlar.

**Faydaları:**
- ✅ Hızlı TODO oluşturma (1-2 dakika)
- ✅ Tutarlı klasör yapısı
- ✅ Standart dökümanlar
- ✅ Hata riski minimum
- ✅ Proje prensipleri korunur

---

## 📋 GEREKSİNİMLER

### 1. Dosyalar
```
✅ project_config.yaml         - Proje konfigürasyonu
✅ templates/setup_todo.sh     - Kurulum scripti
✅ templates/TODO_TEMPLATE/    - Template klasörü
✅ TODO_KURALLARI.md           - Genel kurallar
```

### 2. Hazırlık
1. Proje kök dizinine git
2. `project_config.yaml` dosyasını kontrol et
3. Yeni TODO için bilgileri belirle:
   - TODO Kısa Adı (örn: RUNTIME)
   - TODO Tam Adı (örn: RUNTIME_INT)
   - TODO Belge Adı (örn: 2-TODO_RUNTIME_INTEGRATION.md)

---

## 🚀 KULLANIM

### Adım 1: Script'i Çalıştır

```bash
cd /proje/dizini

./templates/setup_todo.sh TODO_KISA TODO_TAM TODO_BELGE
```

**Örnek:**
```bash
./templates/setup_todo.sh RUNTIME RUNTIME_INT 2-TODO_RUNTIME_INTEGRATION.md
```

### Adım 2: Çıktıyı Kontrol Et

Script şunu oluşturur:
```
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
    └── (boş - proje spesifik belgeler ekle)
```

### Adım 3: Özelleştir

1. **TODO Belgesi Oluştur:**
   ```bash
   # Ana dizinde
   touch 2-TODO_RUNTIME_INTEGRATION.md
   ```
   
   İçeriğe görev listesi, Phase'ler, Task'lar ekle.

2. **Proje Spesifik Belgeler:**
   ```bash
   # TODO'ya özel dokümantasyon
   TODO_RUNTIME_INT/docs/
   ├── runtime_api.md
   ├── integration_guide.md
   └── test_scenarios.md
   ```

3. **Mastermind Belgesini Gözden Geçir:**
   - PROJECT_PRINCIPLES bölümünü kontrol et
   - Proje spesifik örnekleri güncelle
   - Referans belgeleri ekle

4. **Üst Akıl Belgesini Güncelle:**
   - Mevcut durum metriklerini yaz
   - Phase/Task yapısını tanımla
   - İlk YZ için görev belirle

5. **YZ Belgesini Hazırla:**
   - İlk görev detaylarını yaz
   - Test senaryolarını ekle
   - Örnek kod/komutlar ver

---

## 📝 ÖRNEK: Yeni TODO Oluşturma

### Senaryo: Backend Migration TODO'su

**1. Bilgileri Belirle:**
```
TODO Kısa Adı:  BACKEND
TODO Tam Adı:   BACKEND_LLVM
TODO Belgesi:   3-TODO_GCC_TO_LLVM_BACKEND.md
```

**2. Script Çalıştır:**
```bash
./templates/setup_todo.sh BACKEND BACKEND_LLVM 3-TODO_GCC_TO_LLVM_BACKEND.md
```

**3. Çıktı:**
```
✅ TODO başarıyla oluşturuldu!

📂 Oluşturulan yapı:
  TODO_BACKEND_LLVM/
  ├── BACKEND_MM/
  │   └── Mastermind_buradan_basla.md
  ├── BACKEND_UA/
  │   └── Ust_Akil_buradan_basla.md
  ├── BACKEND_YZ/
  │   ├── NEXT_AI_START_HERE.md
  │   ├── YZ_RAPOR_TEMPLATE.md
  │   └── completed/
  └── docs/
```

**4. TODO Belgesi Oluştur:**
```bash
cat > 3-TODO_GCC_TO_LLVM_BACKEND.md << 'EOF'
# TODO: GCC → LLVM Backend Migration

## Hedef
GCC x86-64 assembly backend'ini LLVM IR backend'ine dönüştür

## Phase'ler
- Phase 1: LLVM IR Öğren (research)
- Phase 2: Codegen Adapter (GCC wrapper)
- Phase 3: LLVM IR Generator (yeni backend)
- Phase 4: Test ve Optimizasyon
EOF
```

**5. Dokümantasyon Ekle:**
```bash
cat > TODO_BACKEND_LLVM/docs/llvm_ir_basics.md << 'EOF'
# LLVM IR Basics

## Ne öğrenilecek?
- LLVM IR syntax
- SSA (Static Single Assignment)
- Type system
- Function calls
- Memory operations
EOF
```

**6. Mastermind'a Tanıt:**
```bash
# Mastermind belgesini aç ve incele
vi TODO_BACKEND_LLVM/BACKEND_MM/Mastermind_buradan_basla.md

# Proje prensiplerini kontrol et
# Örnekleri BACKEND için güncelle
```

---

## 🔧 TEMPLATE ÖZELLEŞTİRME

### Yeni Proje İçin Template Güncelleme

**1. project_config.yaml Güncelle:**
```yaml
project:
  name: "yeni-proje-adi"
  full_name: "Yeni Proje Tam Adı"
  
principles:
  - name: "PRENSIP1"
    description: "Açıklama"
  - name: "PRENSIP2"
    description: "Açıklama"
```

**2. Template Dosyalarını Özelleştir:**
```bash
# Mastermind template'i düzenle
vi templates/TODO_TEMPLATE/TODO_KISA_MM/Mastermind_buradan_basla.md.template

# Proje spesifik örnekleri değiştir
# {PROJECT_NAME}, {TODO_KISA}, {TODO_TAM}, {TODO_BELGE} placeholder'larını koru
```

**3. Komutları Güncelle:**
```yaml
# project_config.yaml
commands:
  build: "yeni-proje-build-komutu"
  test: "yeni-proje-test-komutu"
```

---

## 🚨 SORUN GİDERME

### Script Çalışmıyor

**Hata:** `project_config.yaml bulunamadı`
```bash
# Çözüm: Proje kök dizininde çalıştır
cd /proje/kök/dizini
./templates/setup_todo.sh ...
```

**Hata:** `templates/TODO_TEMPLATE/ bulunamadı`
```bash
# Çözüm: Template klasörünü kontrol et
ls -la templates/TODO_TEMPLATE/
```

**Hata:** `Permission denied`
```bash
# Çözüm: Script'e çalıştırma yetkisi ver
chmod +x templates/setup_todo.sh
```

### Placeholder'lar Değiştirilmemiş

**Sorun:** Dosyalarda hala `{TODO_KISA}` görünüyor

**Çözüm 1:** Script'i tekrar çalıştır
```bash
rm -rf TODO_PROBLEMATIC/
./templates/setup_todo.sh TODO_KISA TODO_TAM TODO_BELGE
```

**Çözüm 2:** Manuel değiştir
```bash
cd TODO_PROBLEMATIC/
find . -type f -name "*.md" -exec sed -i 's/{TODO_KISA}/GERCEK_AD/g' {} \;
```

### Klasör İsimleri Yanlış

**Sorun:** `TODO_KISA_MM/` olarak kalmış, `RUNTIME_MM/` olması gerekiyor

**Çözüm:**
```bash
# Script bunu otomatik yapıyor ama manuel düzeltmek için:
cd TODO_RUNTIME_INT/
mv TODO_KISA_MM RUNTIME_MM
mv TODO_KISA_UA RUNTIME_UA
mv TODO_KISA_YZ RUNTIME_YZ
```

---

## 📋 CHECKLIST: Yeni TODO Oluşturma

Yeni TODO oluşturduğunda şunu kontrol et:

### Hazırlık
- [ ] Proje kök dizindesin
- [ ] `project_config.yaml` mevcut ve güncel
- [ ] TODO bilgileri belirlendi (KISA, TAM, BELGE)
- [ ] Mevcut TODO ile çakışma yok

### Oluşturma
- [ ] Script başarıyla çalıştı
- [ ] TODO klasörü oluşturuldu
- [ ] Alt klasörler doğru isimde (MM, UA, YZ)
- [ ] Template dosyaları kopyalandı
- [ ] Placeholder'lar değiştirildi

### Özelleştirme
- [ ] TODO belgesi oluşturuldu ve dolduruldu
- [ ] docs/ klasörüne proje spesifik belgeler eklendi
- [ ] Mastermind belgesi gözden geçirildi
- [ ] Üst Akıl belgesi güncellendi (metrikler, Phase'ler)
- [ ] YZ belgesi hazırlandı (ilk görev tanımlı)

### Test
- [ ] Mastermind belgesini oku - anlam bütünlüğü var mı?
- [ ] Üst Akıl belgesini oku - görevler net mi?
- [ ] YZ belgesini oku - başlayabilir mi?
- [ ] TODO belgesi tamamlandı mı?
- [ ] Referans linkler çalışıyor mu?

### Entegrasyon
- [ ] `project_config.yaml`'a TODO eklendi
- [ ] Ana `NEXT_AI_START_HERE.md` güncellendi
- [ ] `STRATEGY_OVERVIEW.md` güncellendi (varsa)
- [ ] Git'e commit edildi

---

## 🎯 SONUÇ

Template sistemi ile yeni TODO oluşturmak artık çok kolay!

**Süre:** 1-2 dakika (script) + 30-60 dakika (özelleştirme)

**Standart Kalite:** Her TODO aynı yapıya sahip, ekip kolayca adapte olur.

**Bakım:** Template'i bir kere güncelle, tüm yeni TODO'lar güncel template'i kullanır.

---

**Hazırlayan:** mlp-original Ekibi  
**Güncelleme:** Template her güncelleme sonrası bu belgede belirt  
**Versiyon:** 1.0 (2025-12-25)
