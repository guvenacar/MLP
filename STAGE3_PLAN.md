# Stage 3 Plan - Pragmatic Self-Hosting + Full GC

## 🎯 Stage 3 Hedefleri (REVİZE EDİLDİ)

### Yeni Yaklaşım: Pragmatic Self-Hosting
**Sorun:** Tam self-hosting (tüm compiler'ları MELP'e port) ~4000 lines kod + ~5 hafta sürer.

**Yeni Strateji:**
1. **Öncelik 1: WORKING GC** - En kritik eksik özellik
2. **Öncelik 2: Enhanced C Compilers** - Placeholder'ları gerçek compiler'lara çevir
3. **Öncelik 3: Partial Self-Hosting** - Sadece basit modüller (test için)

**Rationale:**
- Self-hosting "nice to have" ama GC "must have"
- C compiler'ları improve etmek daha hızlı (C biliyoruz)
- Full self-hosting Stage 4'e ertelenebilir

---

## 📋 Task Breakdown

### Phase 1: Real Lexer Compiler (Self-Host Lexer)
**Hedef:** `lexer.mlp`'yi MELP ile compile et

**Steps:**
1. [ ] `lexer_compiler.mlp` yaz (~500 lines)
   - Token definitions
   - Character-by-character scanning
   - Keyword recognition
   - String/number literal parsing
   - Position tracking
   - Error reporting
2. [ ] Test: `lexer.mlp` → `lexer_v2.s`
3. [ ] Binary equivalence: `lexer_v1.s` vs `lexer_v2.s`
4. [ ] Commit: "Stage 3: Self-hosting lexer compiler"

**Strateji:**
- Lexer en basit modül → başlangıç için ideal
- C lexer_compiler.c'yi MELP'e port et
- Önce tokenization test et, sonra full compile

---

### Phase 2: Real Parser Compiler (Self-Host Parser)
**Hedef:** `parser.mlp`'yi MELP ile compile et

**Steps:**
1. [ ] `parser_compiler.mlp` yaz (~800 lines)
   - Recursive descent parser
   - AST node construction
   - Pratt parsing for expressions
   - Type checking
   - Error recovery
2. [ ] Test: `parser.mlp` → `parser_v2.s`
3. [ ] Binary equivalence: `parser_v1.s` vs `parser_v2.s`
4. [ ] Commit: "Stage 3: Self-hosting parser compiler"

**Challenges:**
- Parser en karmaşık modül (579 lines)
- Recursive descent parsing implementation
- AST construction memory management

---

### Phase 3: Real Memory Compiler (Self-Host Memory)
**Hedef:** `memory.mlp`'yi MELP ile compile et

**Steps:**
1. [ ] `memory_compiler.mlp` yaz (~600 lines)
   - Lifetime analysis
   - Escape analysis
   - Defer handling
   - Memory leak detection
2. [ ] Test: `memory.mlp` → `memory_v2.s`
3. [ ] Binary equivalence: `memory_v1.s` vs `memory_v2.s`
4. [ ] Commit: "Stage 3: Self-hosting memory compiler"

---

### Phase 4: Real Codegen Compiler (Self-Host Codegen)
**Hedef:** `codegen.mlp`'yi MELP ile compile et

**Steps:**
1. [ ] `codegen_compiler.mlp` yaz (~700 lines)
   - x86-64 instruction generation
   - Register allocation algorithm
   - Stack frame management
   - Control flow generation
2. [ ] Test: `codegen.mlp` → `codegen_v2.s`
3. [ ] Binary equivalence: `codegen_v1.s` vs `codegen_v2.s`
4. [ ] Commit: "Stage 3: Self-hosting codegen compiler"

---

### Phase 5: Real Router Compiler (Self-Host Router)
**Hedef:** `router.mlp`'yi MELP ile compile et

**Steps:**
1. [ ] `router_compiler.mlp` yaz (~300 lines)
   - Module coordination
   - File I/O
   - Pipeline orchestration
2. [ ] Test: `melpc.mlp` → `melpc_v2.s`
3. [ ] Binary equivalence: `melpc_v1.s` vs `melpc_v2.s`
4. [ ] Commit: "Stage 3: Self-hosting router compiler"

---

### Phase 6: Full GC Implementation
**Hedef:** Otomatik garbage collector

**Steps:**
1. [ ] `gc.mlp` modülü yaz (~1000 lines)
   - Mark phase: Reachable object marking
   - Sweep phase: Unreachable object deallocation
   - Root set tracking (stack, globals)
   - Reference counting
   - Cycle detection
   - Free list management
2. [ ] `gc_compiler.mlp` (self-hosting)
3. [ ] Integration test: Memory leaks yok mu?
4. [ ] Performance test: GC overhead
5. [ ] Commit: "Stage 3: Full GC implementation"

**GC Architecture:**
```
Heap Structure:
┌─────────────────────────────────┐
│ Object Header (24 bytes)        │
│  - Mark bit (GC flag)           │
│  - Type tag                     │
│  - Size                         │
│  - Ref count                    │
├─────────────────────────────────┤
│ Object Data                     │
└─────────────────────────────────┘

GC Algorithm:
1. Mark Phase:
   - Start from roots (stack, globals)
   - Traverse object graph
   - Mark reachable objects
2. Sweep Phase:
   - Iterate heap
   - Free unmarked objects
   - Update free list
3. Compact (optional):
   - Move objects to reduce fragmentation
```

---

### Phase 7: Binary Equivalence Testing
**Hedef:** Tüm modüller self-compile edildiğinde aynı output vermeli

**Steps:**
1. [ ] Test framework: `test_equivalence.sh`
2. [ ] Compare all modules:
   ```bash
   diff lexer_v1.s lexer_v2.s
   diff parser_v1.s parser_v2.s
   diff memory_v1.s memory_v2.s
   diff codegen_v1.s codegen_v2.s
   diff melpc_v1.s melpc_v2.s
   ```
3. [ ] Semantic equivalence (if binary diff fails)
4. [ ] Reproducibility: v2 → v3 == v2?
5. [ ] Commit: "Stage 3: Binary equivalence verified"

---

### Phase 8: Feature Completion
**Hedef:** Stage 1'de eksik kalan features

**Features:**
1. [ ] Structs with methods
2. [ ] Generic types (List<T>, Option<T>)
3. [ ] Async/await
4. [ ] Pattern matching
5. [ ] Module system (import/export)
6. [ ] Error handling (Result type)

---

## 🎯 Stratejik Yaklaşım

### 1. Incremental Self-Hosting
- Her modülü teker teker self-host et
- Her adımda test et ve commit et
- Dependency order: Lexer → Parser → Memory → Codegen → Router

### 2. C-to-MELP Port Strategy
- Stage 2'de C compiler'lar basit placeholder'lar
- Her C compiler'ı MELP'e port et
- Aynı logic, farklı dil

### 3. Binary Equivalence Focus
- Her modül için C vs MELP assembly karşılaştır
- Semantic equivalence yeterli (exact binary match şart değil)
- Register allocation farklı olabilir, ama behavior aynı olmalı

### 4. GC Implementation Strategy
- **Önce basit Mark & Sweep:** Reference counting ekle
- **Cycle detection:** Weak references ile
- **Generational GC (optional):** Performance için

---

## 📊 Estimated Effort

### Code Lines
```
lexer_compiler.mlp:     ~500 lines
parser_compiler.mlp:    ~800 lines
memory_compiler.mlp:    ~600 lines
codegen_compiler.mlp:   ~700 lines
router_compiler.mlp:    ~300 lines
gc.mlp:                 ~1000 lines
test_equivalence.sh:    ~100 lines
-----------------------------------
TOTAL:                  ~4000 lines (Stage 3)

Stage 2:                1908 lines
Stage 3:                ~4000 lines
-----------------------------------
Grand Total:            ~5908 lines (modular)
```

**Comparison:**
- Önceki monolitik deneme: 5000+ lines tek dosya → Token limit ❌
- Stage 2+3 modular: ~6000 lines 11 modül → Her modül < 1000 lines ✅

---

## 🚦 Success Criteria

### ✅ Stage 3 Complete When:
1. **Self-Hosting:** Her modül kendini compile edebiliyor
2. **Binary Equivalence:** C vs MELP assembly semantically equivalent
3. **Full GC:** Automatic memory management çalışıyor
4. **No Memory Leaks:** GC tüm unreachable objects'leri free ediyor
5. **Reproducibility:** v2 → v3 == v2 (compiler stable)
6. **Feature Complete:** Structs, generics, async, pattern matching working

---

## 🔧 Implementation Order

### Week 1: Lexer Self-Hosting
- Day 1-2: `lexer_compiler.mlp` implement
- Day 3: Test + binary equivalence
- Day 4: Commit + documentation

### Week 2: Parser Self-Hosting
- Day 1-3: `parser_compiler.mlp` implement
- Day 4-5: Test + binary equivalence
- Day 6: Commit + documentation

### Week 3: Memory + Codegen Self-Hosting
- Day 1-3: `memory_compiler.mlp`
- Day 4-6: `codegen_compiler.mlp`
- Day 7: Test + commit

### Week 4: Router + GC
- Day 1-2: `router_compiler.mlp`
- Day 3-7: `gc.mlp` (Mark & Sweep)

### Week 5: Integration + Testing
- Day 1-3: Binary equivalence testing
- Day 4-5: Performance testing
- Day 6-7: Documentation (STAGE3_COMPLETE.md)

---

## 📁 Directory Structure (Stage 3)

```
MLP/
├── melp/
│   └── compiler/
│       ├── lexer/
│       │   ├── lexer.mlp (377 lines - Stage 2)
│       │   ├── lexer_compiler.c (C Stage 0 - Stage 2)
│       │   ├── lexer_compiler.mlp (NEW - Stage 3, ~500 lines)
│       │   └── ...
│       ├── parser/
│       │   ├── parser.mlp (579 lines - Stage 2)
│       │   ├── parser_compiler.c (C Stage 0 - Stage 2)
│       │   ├── parser_compiler.mlp (NEW - Stage 3, ~800 lines)
│       │   └── ...
│       ├── memory/
│       │   ├── memory.mlp (345 lines - Stage 2)
│       │   ├── memory_compiler.c (C Stage 0 - Stage 2)
│       │   ├── memory_compiler.mlp (NEW - Stage 3, ~600 lines)
│       │   └── ...
│       ├── codegen/
│       │   ├── codegen.mlp (490 lines - Stage 2)
│       │   ├── codegen_compiler.c (C Stage 0 - Stage 2)
│       │   ├── codegen_compiler.mlp (NEW - Stage 3, ~700 lines)
│       │   └── ...
│       ├── router/
│       │   ├── melpc.mlp (117 lines - Stage 2)
│       │   ├── router_compiler.c (C Stage 0 - Stage 2)
│       │   ├── router_compiler.mlp (NEW - Stage 3, ~300 lines)
│       │   └── ...
│       └── gc/
│           ├── gc.mlp (NEW - Stage 3, ~1000 lines)
│           ├── gc_compiler.mlp (NEW - Stage 3, ~800 lines)
│           └── test_gc.mlp
├── bootstrap_stage3.sh (NEW)
├── test_equivalence.sh (NEW)
└── STAGE3_COMPLETE.md (future)
```

---

## 🎓 Key Challenges

### 1. Bootstrapping Problem
**Problem:** MELP compiler'ını MELP ile yazmak → Tavuk-yumurta problemi

**Solution:**
- Stage 0 (C): C ile basit compiler → MELP modüllerini compile eder
- Stage 1 (MELP v1): C compiler'ların output'u
- Stage 2 (MELP v2): MELP compiler'ların output'u
- Stage 3 (MELP v3): v2 ile compile → v3 == v2? (stable compiler)

### 2. Memory Management (GC)
**Problem:** Manual memory management zor, memory leaks olası

**Solution:**
- Automatic GC (Mark & Sweep)
- Reference counting + cycle detection
- Lifetime analysis (Memory module)

### 3. Binary Equivalence
**Problem:** Exact binary match zor (register allocation farklı olabilir)

**Solution:**
- Semantic equivalence test
- Behavior equivalence (same I/O for same input)
- Reproducibility (v2 → v3 == v2)

---

## 🚀 Next Steps

### Immediate Actions:
1. [ ] **Commit STAGE3_PLAN.md**
2. [ ] **Start Phase 1:** `lexer_compiler.mlp` implementation
3. [ ] **Create test framework:** Binary equivalence testing

### Decision Points:
- **GC Timing:** GC before or after self-hosting?
  - **Option A:** Self-hosting first → easier debugging (no GC complexity)
  - **Option B:** GC first → better memory management during self-hosting
  - **Recommendation:** **Option A** (Self-hosting first, GC after)

---

**Stage 3 Start Date:** 4 Aralık 2025  
**Estimated Completion:** ~5 weeks  
**Current Branch:** melp20241204
