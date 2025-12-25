# Yeni MLP - Temiz Mimari ile Yeniden Başlangıç

**Tarih:** 25 Kasım 2025  
**Durum:** Planlama Aşaması  
**Temel:** Mevcut MLP'den öğrenilen dersler

---

## 🎯 Proje Amacı

Mevcut MLP'nin **başarılı** olduğu yönleri koruyarak, **teknik borçları** olmadan **temiz mimari** ile yeni bir MLP implementasyonu.

---

## 📁 Proje Yapısı

```
yeni_mlp/
├── docs/                      # Dokümantasyon
│   ├── ARCHITECTURE.md        # Mimari tasarım (yol haritası)
│   ├── SPECS.md              # Dil spesifikasyonu
│   ├── API_REFERENCE.md      # Runtime API dokümantasyonu
│   └── CONTRIBUTING.md       # Katkıda bulunma kılavuzu
│
├── config/                    # Dil tanımları
│   ├── diller.json           # TR→EN keyword mapping
│   └── syntax.json           # MLP base syntax kuralları
│
├── src/                       # Kaynak kodlar
│   ├── normalizer/           # Dil normalizer
│   │   ├── mlp_normalize.py  # Python normalizer
│   │   └── tests/            # Normalizer testleri
│   │
│   ├── lexer/                # Lexer (tokenizer)
│   │   ├── lexer.c           # C implementation
│   │   ├── lexer.mlp         # MLP implementation (self-hosted)
│   │   └── tests/            # Lexer testleri
│   │
│   ├── parser/               # Parser (AST builder)
│   │   ├── parser.c          # C implementation (bootstrap)
│   │   ├── parser.mlp        # MLP implementation (self-hosted)
│   │   ├── ast.h             # AST node definitions
│   │   └── tests/            # Parser testleri
│   │
│   ├── codegen/              # Code generator
│   │   ├── codegen_x64.c     # x86-64 assembly generator
│   │   ├── codegen.mlp       # MLP implementation
│   │   └── tests/            # Codegen testleri
│   │
│   └── main/                 # Main compiler
│       ├── mlpc.c            # C bootstrap compiler
│       └── mlpc.mlp          # MLP self-hosted compiler
│
├── runtime/                   # Runtime library
│   ├── hashmap.c/.h          # HashMap (reuse from old MLP)
│   ├── list.c/.h             # List implementation
│   ├── string.c/.h           # String utilities
│   ├── file_io.c/.h          # File operations
│   └── memory.c/.h           # Memory management
│
├── tests/                     # Integration tests
│   ├── unit/                 # Unit tests
│   ├── integration/          # Integration tests
│   └── benchmarks/           # Performance benchmarks
│
├── examples/                  # Example MLP programs
│   ├── hello_world.mlp
│   ├── fibonacci.mlp
│   └── web_server.mlp
│
├── tools/                     # Development tools
│   ├── bootstrap.sh          # Bootstrap script
│   ├── test_runner.sh        # Test runner
│   └── benchmark.sh          # Benchmark runner
│
└── build/                     # Build artifacts (gitignored)
    ├── stage0/               # C compiler binaries
    ├── stage1/               # First MLP-compiled binaries
    └── stage2/               # Self-hosted verification
```

---

## 🚀 Geliştirme Aşamaları

### **Faz 1: Normalizer + Dil Tanımları (1-2 hafta)**

**Hedef:** Multi-language support temeli

**Deliverables:**
- ✅ `config/diller.json` - Tüm keyword mappings
- ✅ `config/syntax.json` - Base syntax rules
- ✅ `src/normalizer/mlp_normalize.py` - Working normalizer
- ✅ 50+ test case başarılı

**Test:**
```bash
python src/normalizer/mlp_normalize.py examples/hello_tr.mlp hello_en.mlp
./mlpc hello_en.mlp hello.asm
```

---

### **Faz 2: Lexer (1 hafta)**

**Hedef:** Normalize edilmiş kodu tokenize et

**Deliverables:**
- ✅ `src/lexer/lexer.c` - C implementation
- ✅ Token type definitions (80+ types)
- ✅ Unit tests (%90+ coverage)

**Test:**
```bash
./lexer_test hello_en.mlp > tokens.txt
# Verify token stream
```

---

### **Faz 3: Parser (2-3 hafta)**

**Hedef:** Token stream → AST

**Deliverables:**
- ✅ `src/parser/parser.c` - Context-aware parser
- ✅ `src/parser/ast.h` - AST node definitions (English naming)
- ✅ Context stack implementation
- ✅ Import system (circular detection)
- ✅ Nested structures working

**Test:**
```bash
./parser_test hello_en.mlp > ast.json
# Verify AST structure
```

---

### **Faz 4: Code Generator (3-4 hafta)**

**Hedef:** AST → x86-64 Assembly

**Deliverables:**
- ✅ `src/codegen/codegen_x64.c` - Assembly generator
- ✅ Register allocation
- ✅ Function calling convention (System V AMD64 ABI)
- ✅ Runtime library integration

**Test:**
```bash
./codegen_test ast.json > output.asm
nasm -f elf64 output.asm -o output.o
gcc output.o runtime/*.o -o program
./program  # Should work!
```

---

### **Faz 5: Self-Hosting (2-3 hafta)**

**Hedef:** MLP ile MLP derleyicisi

**Deliverables:**
- ✅ `src/main/mlpc.mlp` - Full compiler in MLP
- ✅ Bootstrap process documented
- ✅ Stage1 == Stage2 verification

**Test:**
```bash
./tools/bootstrap.sh
# Verify self-hosting successful
```

---

## 📚 Referans: Eski MLP'den Alınacaklar

### ✅ **%100 Yeniden Kullanılabilir:**
- `runtime/hashmap.c` - Production-ready
- Context stack pattern
- Token type system
- Import pre-scan strategy

### ⚠️ **Düzeltilerek Kullanılabilir:**
- Lexer (Turkish keywords çıkar)
- Parser (global state → local state)
- AST structure (English naming)

### ❌ **Kullanılmayacak:**
- Hardcoded Turkish keywords in C
- Global mutable state pattern
- Mixed language naming

---

## 🎓 Öğrenilen Dersler

### **1. Mimari Kararları Başta Al**
- Normalizer zorunlu (multi-language için)
- Context stack baştan tasarla
- Syntax tutarlılığı kritik

### **2. Test-Driven Development**
- Her adımda working code
- Incremental testing
- Regression prevention

### **3. Dokümantasyon**
- Her karar dokümante edilmeli
- API referansları zorunlu
- Syntax kuralları yazılı olmalı

### **4. Clean Code**
- English naming only
- No global mutable state
- Thread-safe from day 1

---

## 🔧 Kurulum

```bash
# 1. Dependencies:
sudo apt-get install gcc nasm python3 make

# 2. Build:
cd yeni_mlp
./tools/bootstrap.sh

# 3. Test:
./tools/test_runner.sh

# 4. Run:
./mlpc examples/hello_world.mlp hello.asm
nasm -f elf64 hello.asm -o hello.o
gcc hello.o -o hello
./hello
```

---

## 📊 İlerleme Takibi

**Durum:** Planlama
- [ ] Faz 1: Normalizer (0%)
- [ ] Faz 2: Lexer (0%)
- [ ] Faz 3: Parser (0%)
- [ ] Faz 4: Codegen (0%)
- [ ] Faz 5: Self-Hosting (0%)

**Tahmini Süre:** 10-12 hafta (3 ay)

---

## 🤝 Katkıda Bulunma

Detaylar için `docs/CONTRIBUTING.md` dosyasına bakın.

---

## 📝 Lisans

MIT License - Detaylar için LICENSE dosyasına bakın.

---

**Not:** Bu proje mevcut MLP'nin yeniden yazımıdır. Eski proje başarılı ve çalışıyor, ama teknik borçları var. Bu proje temiz mimari ile baştan başlama şansı.
