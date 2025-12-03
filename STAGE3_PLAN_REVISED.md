# Stage 3: Pragmatic Approach - GC First

## 🎯 Yeni Hedef: Working Compiler with GC

### Strateji Değişikliği
**Önceki Plan:** Full self-hosting (4000+ lines MELP) → 5 hafta  
**Yeni Plan:** GC + Enhanced C compilers → 2 hafta

**Rationale:**
- GC en kritik eksik feature (memory management)
- C compiler'ları geliştirmek daha hızlı ve pratik
- Full self-hosting Stage 4'e ertelenebilir

---

## 📋 Stage 3 Roadmap

### Phase 1: Full GC Implementation (Öncelik #1)
**Hedef:** Otomatik garbage collector

**Tasks:**
1. [ ] GC Runtime Module (~800 lines C)
   - Heap manager
   - Object headers (mark bit, type, size)
   - Free list management
2. [ ] Mark Phase
   - Root set scanning (stack, globals)
   - Graph traversal (DFS/BFS)
   - Mark reachable objects
3. [ ] Sweep Phase
   - Iterate heap
   - Free unmarked objects
   - Update free list
4. [ ] Integration
   - Inject GC calls into codegen
   - malloc() → gc_alloc()
   - free() → automatic (GC)
5. [ ] Testing
   - Memory leak tests
   - Cycle detection tests
   - Performance benchmarks

**Files:**
```
melp/runtime/
├── gc.c              # GC implementation
├── gc.h              # GC header
├── gc_test.c         # Unit tests
└── Makefile
```

---

### Phase 2: Enhanced Lexer Compiler
**Hedef:** Placeholder'dan gerçek lexer'a geçiş

**Improvements:**
- [ ] Real tokenization (not placeholder)
- [ ] All token types (40+ tokens)
- [ ] String/number literal parsing
- [ ] Comment handling
- [ ] Position tracking
- [ ] Error messages

**File:** `melp/compiler/lexer/lexer_compiler.c` (improve from 52 lines → ~400 lines)

---

### Phase 3: Enhanced Parser Compiler
**Hedef:** Gerçek AST construction

**Improvements:**
- [ ] Recursive descent parser
- [ ] All AST node types (29 types)
- [ ] Expression parsing (Pratt)
- [ ] Type checking
- [ ] Error recovery

**File:** `melp/compiler/parser/parser_compiler.c` (improve → ~600 lines)

---

### Phase 4: Enhanced Memory Compiler
**Hedef:** Real lifetime analysis

**Improvements:**
- [ ] Escape analysis
- [ ] Lifetime tracking
- [ ] GC hints generation
- [ ] Move semantics

**File:** `melp/compiler/memory/memory_compiler.c` (improve → ~500 lines)

---

### Phase 5: Enhanced Codegen Compiler
**Hedef:** Full x86-64 codegen with GC integration

**Improvements:**
- [ ] All expression types
- [ ] All statement types
- [ ] Function calls
- [ ] GC integration (gc_alloc calls)
- [ ] Stack maps (for GC root scanning)

**File:** `melp/compiler/codegen/codegen_compiler.c` (improve → ~700 lines)

---

### Phase 6: Integration Testing
**Tasks:**
- [ ] Compile real MELP programs
- [ ] Factorial test
- [ ] Fibonacci test
- [ ] Data structures test (linked list, tree)
- [ ] GC stress test (many allocations)
- [ ] Memory leak check (valgrind)

---

## 🏗️ GC Architecture

### Heap Layout
```
┌─────────────────────────────────┐
│ Object Header (16 bytes)        │
│  - gc_mark: u8 (GC flag)        │
│  - type_id: u8                  │
│  - size: u32                    │
│  - next: *Object (free list)    │
├─────────────────────────────────┤
│ Object Data (variable size)     │
└─────────────────────────────────┘
```

### Mark & Sweep Algorithm
```c
// 1. Mark Phase
void gc_mark() {
    // Mark from roots (stack, globals)
    for (Object* root : roots) {
        mark_object(root);
    }
}

void mark_object(Object* obj) {
    if (obj->gc_mark) return;  // Already marked
    obj->gc_mark = 1;
    
    // Recursively mark children
    for (Object* child : obj->children) {
        mark_object(child);
    }
}

// 2. Sweep Phase
void gc_sweep() {
    Object* obj = heap_start;
    while (obj < heap_end) {
        if (obj->gc_mark) {
            obj->gc_mark = 0;  // Unmark for next GC
        } else {
            free_object(obj);  // Unreachable, free it
        }
        obj = next_object(obj);
    }
}

// 3. GC Trigger
void* gc_alloc(size_t size) {
    if (heap_usage > threshold) {
        gc_collect();  // Trigger GC
    }
    return malloc_from_heap(size);
}
```

### GC Integration in Codegen
```asm
# Before (manual malloc):
movq $24, %rdi
call malloc

# After (GC alloc):
movq $24, %rdi
call gc_alloc
```

---

## 📊 Estimated Effort

### Code Lines (Revised)
```
GC Runtime (C):              ~800 lines
Enhanced Lexer Compiler:     ~400 lines
Enhanced Parser Compiler:    ~600 lines
Enhanced Memory Compiler:    ~500 lines
Enhanced Codegen Compiler:   ~700 lines
Integration Tests:           ~200 lines
-------------------------------------------
TOTAL:                       ~3200 lines (mostly C)

Stage 2:                     1908 lines (MELP)
Stage 3:                     ~3200 lines (C)
-------------------------------------------
Grand Total:                 ~5100 lines
```

### Timeline (Revised)
```
Week 1: GC Implementation (5 days)
  - Day 1-2: Heap manager + Object headers
  - Day 3: Mark phase
  - Day 4: Sweep phase
  - Day 5: Testing

Week 2: Enhanced Compilers (5 days)
  - Day 1: Lexer compiler
  - Day 2: Parser compiler
  - Day 3: Memory compiler
  - Day 4-5: Codegen compiler + GC integration

Week 3: Testing + Documentation (2 days)
  - Day 1: Integration tests
  - Day 2: STAGE3_COMPLETE.md
```

**Total:** ~12 days (2.5 weeks) vs 5 weeks (old plan)

---

## ✅ Success Criteria

### Stage 3 Complete When:
1. ✅ **GC Working:** Automatic memory management
2. ✅ **No Memory Leaks:** Valgrind clean
3. ✅ **Real Compiler:** Not placeholder (C-based but functional)
4. ✅ **Integration Tests:** Complex programs compile and run
5. ✅ **Performance:** GC overhead < 20%

---

## 🚦 Implementation Order

### Step 1: GC Core (Day 1-2)
```c
// gc.c
typedef struct Object {
    uint8_t gc_mark;
    uint8_t type_id;
    uint32_t size;
    struct Object* next;  // For free list
} Object;

void* gc_alloc(size_t size);
void gc_collect();
void gc_mark();
void gc_sweep();
```

### Step 2: GC Mark Phase (Day 3)
- Root scanning (stack walk)
- Object graph traversal
- Mark bit setting

### Step 3: GC Sweep Phase (Day 4)
- Heap iteration
- Free unmarked objects
- Free list update

### Step 4: GC Testing (Day 5)
- Unit tests
- Memory leak tests
- Cycle detection

### Step 5: Enhanced Compilers (Day 6-10)
- Lexer: Real tokenization
- Parser: Real AST
- Memory: Real analysis
- Codegen: GC integration

### Step 6: Integration (Day 11-12)
- Compile real programs
- Test GC under load
- Documentation

---

## 🎓 Key Design Decisions

### 1. C-based GC (not MELP)
**Why:** Faster implementation, easier debugging, proven tools (valgrind)

### 2. Simple Mark & Sweep (no generational)
**Why:** Simpler, sufficient for Stage 3, generational in Stage 4

### 3. Conservative GC (optional)
**Why:** No type information needed, scan everything as pointers

### 4. Defer full self-hosting to Stage 4
**Why:** Working GC more valuable than compiler written in MELP

---

## 📁 Directory Structure

```
MLP/
├── melp/
│   ├── runtime/
│   │   ├── gc.c           (NEW - GC implementation)
│   │   ├── gc.h           (NEW - GC header)
│   │   ├── gc_test.c      (NEW - GC tests)
│   │   └── Makefile
│   └── compiler/
│       ├── lexer/
│       │   └── lexer_compiler.c (ENHANCED - real lexer)
│       ├── parser/
│       │   └── parser_compiler.c (ENHANCED - real parser)
│       ├── memory/
│       │   └── memory_compiler.c (ENHANCED - real analysis)
│       └── codegen/
│           └── codegen_compiler.c (ENHANCED - GC integration)
├── bootstrap_stage3.sh (NEW - build with GC)
└── STAGE3_COMPLETE.md (future)
```

---

## 🚀 Next Actions

### Immediate:
1. [ ] Commit revised STAGE3_PLAN.md
2. [ ] Create `melp/runtime/` directory
3. [ ] Start GC implementation (`gc.c`)

### This Week:
- [ ] GC core implementation
- [ ] GC testing
- [ ] Enhanced lexer compiler

---

**Stage 3 Start:** 4 Aralık 2025  
**Estimated Completion:** ~17 Aralık 2025 (2.5 weeks)  
**Focus:** Working GC + Enhanced C compilers  
**Defer to Stage 4:** Full MELP self-hosting
