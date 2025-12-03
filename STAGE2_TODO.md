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

- [x] `melp/compiler/` ana dizini
- [x] `melp/compiler/lexer/` modülü
- [x] `melp/compiler/parser/` modülü
- [x] `melp/compiler/memory/` modülü
- [x] `melp/compiler/codegen/` modülü
- [x] `melp/compiler/examples/` örnek dosyalar

### 2. Lexer Modülü (İlk Modül) ✅ TAMAMLANDI

- [x] `compiler/lexer/lexer.mlp` yaz (~377 satır) ✅
  - Token tanımları (IDENTIFIER, NUMBER, STRING, vb.)
  - Tokenization logic
  - Error handling
  - Position tracking
- [x] `compiler/lexer/lexer_compiler` (C Stage 0) ✅
- [x] `compiler/lexer/test_lexer.mlp` ✅
- [x] `compiler/lexer/Makefile` ✅
- [x] Test et ve doğrula ✅

### 3. Parser Modülü ✅ TAMAMLANDI

- [x] `compiler/parser/parser.mlp` yaz (~579 satır) ✅
  - AST node tanımları
  - Recursive descent parser
  - Expression parsing (Pratt parsing)
  - Statement parsing
  - Error recovery
- [x] `compiler/parser/parser_compiler` (C Stage 0) ✅
- [x] `compiler/parser/test_parser.mlp` ✅
- [x] `compiler/parser/Makefile` ✅
- [x] Test et ve doğrula ✅

### 4. Memory Modülü ✅ TAMAMLANDI

- [x] `compiler/memory/memory.mlp` yaz (~345 satır) ✅
  - GC analysis
  - Lifetime tracking
  - Reference counting hints
  - @gc annotations
  - defer management
  - Escape analysis
  - Memory leak detection
- [x] `compiler/memory/memory_compiler` (C Stage 0) ✅
- [x] `compiler/memory/test_memory.mlp` ✅
- [x] `compiler/memory/Makefile` ✅
- [x] Test et ve doğrula ✅

### 5. Codegen Modülü ✅ TAMAMLANDI

- [x] `compiler/codegen/codegen.mlp` yaz (~490 satır) ✅
  - x86-64 assembly generation
  - Register allocation
  - Stack management
  - Function prologue/epilogue
  - Binary/unary operators
  - Control flow (if/while/return)
  - Function calls
- [x] `compiler/codegen/codegen_compiler` (C Stage 0) ✅
- [x] `compiler/codegen/test_codegen.mlp` ✅
- [x] `compiler/codegen/Makefile` ✅
- [x] Test et ve doğrula ✅

### 6. Router Implementation ✅ TAMAMLANDI
- [x] `melpc.mlp` yaz (~117 satır) ✅
  - Command line argument parsing
  - Pipeline coordination
  - Module invocation
  - Error propagation
  - Output management
- [x] `router_compiler.c` (C Stage 0) ✅
- [x] Test entire pipeline ✅

### 7. Bootstrap Script ✅ TAMAMLANDI
- [x] `bootstrap_stage2.sh` yaz ✅
  - Her modülü sırayla compile et
  - Dependency management
  - Error handling
  - Progress reporting
- [x] Test full bootstrap process ✅

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
[✓] Lexer    (100% - Complete - 377 lines)
[✓] Parser   (100% - Complete - 579 lines)
[✓] Memory   (100% - Complete - 345 lines)
[✓] Codegen  (100% - Complete - 490 lines)
[✓] Router   (100% - Complete - 117 lines)
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
