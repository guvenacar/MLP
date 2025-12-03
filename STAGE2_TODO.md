# STAGE 2 TODO - Self-Hosting Modular Compiler

## 📋 Genel Bakış
Stage 2'nin amacı: MELP dilinde yazılmış, modüler, self-hosting bir compiler oluşturmak.
**Kritik Ders**: Geçmiş 10 denemede monolitik dosyalar (5000+ satır) AI token limitini aşıyordu. 
**Çözüm**: Her modül <500 satır, bağımsız compile edilebilir, router pattern kullanılacak.

---

## 🎯 Mimari Tasarım

### Router Pattern (melpc.mlp ~80 satır)
```
melpc.mlp (ana koordinatör)
  ├─> compiler/lexer/lexer_compiler (Stage 0 C)
  ├─> compiler/parser/parser_compiler (Stage 0 C)
  ├─> compiler/memory/memory_compiler (Stage 0 C)
  └─> compiler/codegen/codegen_compiler (Stage 0 C)
```

### Modül Yapısı (Her modül için)
```
compiler/MODULE_NAME/
  ├─ module.mlp           (~300-500 satır, MELP'te yazılmış modül)
  ├─ module_compiler      (C Stage 0 derleyicisi)
  ├─ test_module.mlp      (Test dosyası)
  └─ Makefile             (Derleme otomasyonu)
```

---

## ✅ TAMAMLANAN İŞLER

### Stage 1 (100% Tamamlandı)
- [x] 63 C Stage 0 modülü (frozen)
- [x] 63 MELP constant modülü
- [x] Tüm modüller test edildi (63/63 passing)
- [x] test_all_modules.sh script
- [x] STAGE1_COMPLETE.md dokümantasyonu
- [x] melp20241204 branch'ine commit edildi

---

## 🔄 STAGE 2 GÖREVLER

### 1. Dizin Yapısını Oluştur
- [ ] `melp/compiler/` ana dizini
- [ ] `melp/compiler/lexer/` modülü
- [ ] `melp/compiler/parser/` modülü
- [ ] `melp/compiler/memory/` modülü
- [ ] `melp/compiler/codegen/` modülü
- [ ] `melp/compiler/examples/` örnek dosyalar

### 2. Lexer Modülü (İlk Modül)
- [ ] `compiler/lexer/lexer.mlp` yaz (~300 satır)
  - Token tanımları (IDENTIFIER, NUMBER, STRING, vb.)
  - Tokenization logic
  - Error handling
  - Position tracking
- [ ] `compiler/lexer/lexer_compiler` (C Stage 0)
- [ ] `compiler/lexer/test_lexer.mlp`
- [ ] `compiler/lexer/Makefile`
- [ ] Test et ve doğrula

### 3. Parser Modülü
- [ ] `compiler/parser/parser.mlp` yaz (~400 satır)
  - AST node tanımları
  - Recursive descent parser
  - Expression parsing
  - Statement parsing
  - Error recovery
- [ ] `compiler/parser/parser_compiler` (C Stage 0)
- [ ] `compiler/parser/test_parser.mlp`
- [ ] `compiler/parser/Makefile`
- [ ] Test et ve doğrula

### 4. Memory Modülü
- [ ] `compiler/memory/memory.mlp` yaz (~300 satır)
  - GC analysis
  - Lifetime tracking
  - Reference counting hints
  - @gc annotations
  - defer management
- [ ] `compiler/memory/memory_compiler` (C Stage 0)
- [ ] `compiler/memory/test_memory.mlp`
- [ ] `compiler/memory/Makefile`
- [ ] Test et ve doğrula

### 5. Codegen Modülü
- [ ] `compiler/codegen/codegen.mlp` yaz (~500 satır)
  - x86-64 assembly generation
  - Register allocation
  - Stack management
  - Function prologue/epilogue
  - Optimization passes
- [ ] `compiler/codegen/codegen_compiler` (C Stage 0)
- [ ] `compiler/codegen/test_codegen.mlp`
- [ ] `compiler/codegen/Makefile`
- [ ] Test et ve doğrula

### 6. Router Implementation
- [ ] `melpc.mlp` yaz (~80 satır)
  - Command line argument parsing
  - Pipeline coordination
  - Module invocation
  - Error propagation
  - Output management
- [ ] `melpc_compiler` (C Stage 0)
- [ ] Test entire pipeline

### 7. Bootstrap Script
- [ ] `bootstrap_stage2.sh` yaz
  - Her modülü sırayla compile et
  - Dependency management
  - Error handling
  - Progress reporting
- [ ] Test full bootstrap process

### 8. Integration Testing
- [ ] Her modül kendi başına çalışıyor mu?
- [ ] Pipeline doğru çalışıyor mu?
- [ ] Test case'ler passing mi?
- [ ] Error handling düzgün mü?

### 9. Self-Hosting Test
- [ ] Her modül kendini compile edebiliyor mu?
  - lexer.mlp → melpc → lexer_v2
  - parser.mlp → melpc → parser_v2
  - memory.mlp → melpc → memory_v2
  - codegen.mlp → melpc → codegen_v2
- [ ] Binary equivalence check (v1 == v2?)
- [ ] Reproducibility test (v2 → v3 == v2?)

### 10. Documentation
- [ ] STAGE2_COMPLETE.md yaz
  - Architecture overview
  - Module descriptions
  - Build instructions
  - Test results
  - Lessons learned
- [ ] README.md güncelle
- [ ] Code comments ekle

---

## 🚨 KRİTİK KURALLAR

### Dosya Boyutu Limitleri
- ⚠️ **Her modül MAKSIMUM 500 satır** (AI token limit)
- ⚠️ **Router maksimum 100 satır**
- ⚠️ **Test dosyaları maksimum 200 satır**

### Modülerlik Prensipleri
- ✅ Her modül bağımsız compile edilebilmeli
- ✅ Her modülün kendi testi olmalı
- ✅ Modüller arası coupling minimum olmalı
- ✅ Interface'ler açık ve net tanımlanmalı

### Test-Driven Development
- ✅ Her modül yazılmadan önce test yazılmalı
- ✅ Her değişiklik sonrası testler çalıştırılmalı
- ✅ Regression testler korunmalı
- ✅ Edge case'ler test edilmeli

### Git Workflow
- ✅ Her major milestone için commit
- ✅ Her modül tamamlandığında commit
- ✅ Descriptive commit messages
- ✅ Branch per stage (melp20241204)

---

## 📊 İlerleme Takibi

### Modül Durumu
```
[ ] Lexer    (0% - Not started)
[ ] Parser   (0% - Not started)
[ ] Memory   (0% - Not started)
[ ] Codegen  (0% - Not started)
[ ] Router   (0% - Not started)
```

### Test Coverage
```
Total Tests: 0/5 modules
Passing: 0
Failing: 0
```

### Self-Hosting Status
```
Bootstrap: [ ] Not attempted
Self-compile: [ ] Not attempted
Reproducibility: [ ] Not attempted
```

---

## 🎓 Geçmişten Öğrenilenler

### ❌ Başarısız 10 Deneme (Monolitik Yaklaşım)
1. **Problem**: 5000+ satırlık tek dosya
2. **Sonuç**: AI token limiti aşıldı
3. **Bellek**: Context window taştı
4. **Debug**: İmkansız, dosya çok büyük
5. **Maintainability**: Sıfır

### ✅ Stage 1 Başarısı (Modüler Yaklaşım)
1. **Çözüm**: 63 modül, her biri 3-4 dosya
2. **Sonuç**: %100 test success
3. **Maintainability**: Mükemmel
4. **Scalability**: Kanıtlandı

### 🎯 Stage 2 Stratejisi
1. **Modüler**: Her modül <500 satır
2. **Bağımsız**: Kendi başına compile
3. **Test-driven**: Önce test, sonra kod
4. **Incremental**: Bir modül, sonra diğeri
5. **Router Pattern**: Merkezi koordinasyon, dağıtık uygulama

---

## 🔮 STAGE 3 - GC Implementation (Stage 2'den Sonra)

### Not: Stage 1'de GC Kısmen Yapıldı

Stage 1'de `memory` modülünde sadece GC **sabitleri ve placeholder'lar** vardı:

- malloc/free constant'ları
- @gc annotation sabitleri  
- defer keyword sabitleri
- GC function pointer'ları

Ama **gerçek GC implementasyonu yapılmadı**. Stage 3'te ekleyeceğiz.

### Stage 3 Görevleri

- [ ] **GC Modülü Oluştur** (`melp/C/stage0/modules/gc/`)
  - [ ] `gc.h` - GC interface tanımları
  - [ ] `gc.c` - Mark & Sweep implementasyonu
  - [ ] `gc_compiler` - C Stage 0 derleyicisi
  - [ ] `test_gc.mlp` - GC test dosyası
  
- [ ] **GC Algoritması**
  - [ ] Mark phase (reachability analysis)
  - [ ] Sweep phase (garbage collection)
  - [ ] Root set tracking
  - [ ] Reference counting (optional, hybrid)
  - [ ] Generation support (optional)

- [ ] **Integration**
  - [ ] Memory modülü ile entegrasyon
  - [ ] Compiler'a GC hints ekleme
  - [ ] Runtime'a GC çağrıları ekleme
  - [ ] Automatic vs manual GC modes

- [ ] **Testing & Benchmarking**
  - [ ] Memory leak testleri
  - [ ] Performance benchmarks
  - [ ] Stress tests (büyük allocation'lar)
  - [ ] Fragmentation testleri

**Stratejisi**: Stage 2'yi tamamla, self-hosting çalışıyor duruma getir. Sonra GC'yi ayrı bir modül olarak ekle. Bu sayıs GC implementasyonu Stage 2'yi geciktirmez.

---

## 🚀 Sonraki Adım

**ŞİMDİ:** Lexer modülünden başla

1. `mkdir -p melp/compiler/lexer/`
2. `compiler/lexer/test_lexer.mlp` yaz (TDD)
3. `compiler/lexer/lexer.mlp` implement et
4. `compiler/lexer/lexer_compiler` (C Stage 0) yaz
5. Test et, doğrula, commit et

**SONRA:** Stage 2 tamamlandıktan sonra Stage 3'te GC implementasyonunu ekle.

---

**Not**: Bu TODO dosyası aktif olarak güncellenmelidir. Her görev tamamlandığında `[ ]` → `[x]` işaretle.
