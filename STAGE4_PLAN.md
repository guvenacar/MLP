# STAGE 4 PLAN: Full Self-Hosting

**Date:** 4 Aralık 2025  
**Prerequisites:** Stage 3 Complete ✅  
**Goal:** Achieve full self-hosting - MELP compiler written in MELP  
**Estimated Duration:** 2-3 weeks

---

## 🎯 Stage 4 Objective

**Rewrite all 5 C Stage 0 compilers in MELP language, achieving 100% self-hosting.**

Currently:
- ✅ Lexer/Parser/Memory/Codegen/Router written in MELP (Stage 2)
- ✅ C Stage 0 compilers compile MELP code (Stage 3 enhanced)
- ❌ C Stage 0 compilers still in C (not self-hosted)

Target:
- ✅ All compilers written in MELP
- ✅ MELP compiles MELP compilers
- ✅ Full bootstrap chain works

---

## 📋 Prerequisites (ALL MET ✓)

From Stage 3:
- ✅ Working GC runtime (270 lines, 7/7 tests passing)
- ✅ Enhanced lexer_compiler.c (538 lines) with full tokenization
- ✅ Enhanced parser_compiler.c (868 lines) with AST generation
- ✅ Enhanced memory_compiler.c (363 lines) with lifetime analysis
- ✅ Enhanced codegen_compiler.c (693 lines) with GC integration
- ✅ Full pipeline tested and working
- ✅ Modular architecture preserved

---

## 🏗️ Stage 4 Architecture

### Current State (Stage 3)
```
Lexer.mlp ──(lexer_compiler.c)──> lexer.s ──> lexer binary
Parser.mlp ──(parser_compiler.c)──> parser.s ──> parser binary
Memory.mlp ──(memory_compiler.c)──> memory.s ──> memory binary
Codegen.mlp ──(codegen_compiler.c)──> codegen.s ──> codegen binary
Router.mlp ──(router_compiler.c)──> router.s ──> router binary
```

### Target State (Stage 4)
```
lexer_compiler.mlp ──(lexer_compiler.c)──> lexer_compiler binary
parser_compiler.mlp ──(parser_compiler.c)──> parser_compiler binary
memory_compiler.mlp ──(memory_compiler.c)──> memory_compiler binary
codegen_compiler.mlp ──(codegen_compiler.c)──> codegen_compiler binary

# Then bootstrap:
lexer_compiler.mlp ──(lexer_compiler_binary)──> NEW lexer_compiler
# Full self-hosting achieved!
```

---

## 📝 Stage 4 Phases

### Phase 1: Lexer Compiler in MELP (Week 1)
**Goal:** Rewrite `lexer_compiler.c` → `lexer_compiler.mlp`

**Tasks:**
1. Analyze current `lexer_compiler.c` (538 lines)
2. Design MELP equivalent structure
3. Implement token types and lexer state in MELP
4. Implement tokenization logic
5. Implement file I/O in MELP
6. Test: Compile with C compiler, verify output matches
7. Bootstrap test: Use new MELP lexer_compiler to compile lexer.mlp

**Files:**
```
melp/compiler/bootstrap/
├── lexer_compiler.mlp       # New MELP version (~600 lines)
├── test_lexer_compiler.sh   # Comparison tests
└── Makefile                 # Build system
```

**Success Criteria:**
- ✅ lexer_compiler.mlp compiles with C Stage 0
- ✅ Output matches original C compiler
- ✅ Can compile lexer.mlp successfully
- ✅ All lexer tests still pass

---

### Phase 2: Parser Compiler in MELP (Week 1-2)
**Goal:** Rewrite `parser_compiler.c` → `parser_compiler.mlp`

**Tasks:**
1. Analyze current `parser_compiler.c` (868 lines)
2. Design AST structures in MELP
3. Implement token parsing
4. Implement AST node creation
5. Implement expression parsing (Pratt parsing)
6. Implement statement parsing
7. Test and bootstrap

**Files:**
```
melp/compiler/bootstrap/
├── parser_compiler.mlp      # New MELP version (~900 lines)
├── test_parser_compiler.sh  # Comparison tests
└── Makefile
```

**Success Criteria:**
- ✅ parser_compiler.mlp compiles successfully
- ✅ Output matches original C compiler
- ✅ Can compile parser.mlp successfully
- ✅ All parser tests still pass

---

### Phase 3: Memory Compiler in MELP (Week 2)
**Goal:** Rewrite `memory_compiler.c` → `memory_compiler.mlp`

**Tasks:**
1. Analyze current `memory_compiler.c` (363 lines)
2. Design memory tracking structures
3. Implement lifetime analysis
4. Implement escape analysis
5. Implement GC hint generation
6. Test and bootstrap

**Files:**
```
melp/compiler/bootstrap/
├── memory_compiler.mlp       # New MELP version (~400 lines)
├── test_memory_compiler.sh   # Comparison tests
└── Makefile
```

**Success Criteria:**
- ✅ memory_compiler.mlp compiles successfully
- ✅ Output matches original C compiler
- ✅ Can compile memory.mlp successfully
- ✅ All memory tests still pass

---

### Phase 4: Codegen Compiler in MELP (Week 2-3)
**Goal:** Rewrite `codegen_compiler.c` → `codegen_compiler.mlp`

**Tasks:**
1. Analyze current `codegen_compiler.c` (693 lines)
2. Design x86-64 generation structures
3. Implement register allocation
4. Implement expression codegen
5. Implement statement codegen
6. Implement GC integration (malloc → gc_alloc)
7. Test and bootstrap

**Files:**
```
melp/compiler/bootstrap/
├── codegen_compiler.mlp      # New MELP version (~750 lines)
├── test_codegen_compiler.sh  # Comparison tests
└── Makefile
```

**Success Criteria:**
- ✅ codegen_compiler.mlp compiles successfully
- ✅ Output matches original C compiler with GC
- ✅ Can compile codegen.mlp successfully
- ✅ All codegen tests still pass
- ✅ GC integration preserved

---

### Phase 5: Router Compiler in MELP (Week 3)
**Goal:** Rewrite `router_compiler.c` → `router_compiler.mlp`

**Tasks:**
1. Analyze current `router_compiler.c` (minimal)
2. Implement pipeline orchestration
3. Implement error handling
4. Test and bootstrap

**Files:**
```
melp/compiler/bootstrap/
├── router_compiler.mlp       # New MELP version (~150 lines)
└── Makefile
```

---

### Phase 6: Full Bootstrap Chain (Week 3)
**Goal:** Complete self-hosting bootstrap

**Tasks:**
1. Bootstrap Stage 1:
   ```bash
   # Use C compilers to build MELP compilers
   gcc lexer_compiler.c -o lexer_compiler_c
   gcc parser_compiler.c -o parser_compiler_c
   gcc memory_compiler.c -o memory_compiler_c
   gcc codegen_compiler.c -o codegen_compiler_c
   
   # Compile MELP compiler sources with C compilers
   ./lexer_compiler_c lexer_compiler.mlp lexer_compiler.s
   ./parser_compiler_c parser_compiler.mlp parser_compiler.s
   ./memory_compiler_c memory_compiler.mlp memory_compiler.s
   ./codegen_compiler_c codegen_compiler.mlp codegen_compiler.s
   
   # Assemble MELP compilers
   gcc lexer_compiler.s -o lexer_compiler_mlp
   gcc parser_compiler.s -o parser_compiler_mlp
   gcc memory_compiler.s -o memory_compiler_mlp
   gcc codegen_compiler.s -o codegen_compiler_mlp
   ```

2. Bootstrap Stage 2:
   ```bash
   # Use MELP compilers to rebuild themselves!
   ./lexer_compiler_mlp lexer_compiler.mlp lexer_compiler2.s
   ./parser_compiler_mlp parser_compiler.mlp parser_compiler2.s
   ./memory_compiler_mlp memory_compiler.mlp memory_compiler2.s
   ./codegen_compiler_mlp codegen_compiler.mlp codegen_compiler2.s
   
   # Assemble second generation
   gcc lexer_compiler2.s -o lexer_compiler_gen2
   # etc...
   ```

3. Verification:
   ```bash
   # Compare outputs - should be identical (fixed point)
   diff lexer_compiler.s lexer_compiler2.s
   # If identical: SELF-HOSTING ACHIEVED! ✓
   ```

4. Create `bootstrap_stage4.sh` script
5. Update documentation

**Success Criteria:**
- ✅ All MELP compilers compile themselves
- ✅ Output is deterministic (fixed point reached)
- ✅ All tests pass with MELP-compiled compilers
- ✅ Bootstrap script works reliably
- ✅ **FULL SELF-HOSTING ACHIEVED**

---

## 📊 Expected Code Metrics

### Stage 3 (Current)
```
MELP code: 2579 lines (5 modules)
C compilers: 2462 lines (5 C files)
Total: 5041 lines
Self-hosting: 0%
```

### Stage 4 (Target)
```
MELP code: 2579 lines (5 modules) - unchanged
MELP compilers: ~2800 lines (5 MELP compiler files)
C bootstrap: 2462 lines (kept for initial bootstrap)
Total: 7841 lines
Self-hosting: 100%
```

---

## 🎯 Critical Success Factors

### 1. Maintain Modular Architecture
- ✅ Keep all 5 compilers as separate MELP files
- ✅ Each under 1000 lines
- ✅ No monolithic rewrites

### 2. Incremental Development
- ✅ Rewrite one compiler at a time
- ✅ Test after each compiler
- ✅ Keep C versions for fallback

### 3. Output Compatibility
- ✅ MELP compilers must match C compiler output
- ✅ Use diff testing extensively
- ✅ Preserve GC integration

### 4. Bootstrap Verification
- ✅ Test bootstrap at each phase
- ✅ Verify fixed point convergence
- ✅ Ensure deterministic output

---

## 🛠️ Development Strategy

### Week 1: Lexer + Parser
- Days 1-3: Lexer compiler in MELP
- Days 4-7: Parser compiler in MELP
- Milestone: 2/5 compilers self-hosted

### Week 2: Memory + Codegen (Part 1)
- Days 8-10: Memory compiler in MELP
- Days 11-14: Codegen compiler in MELP (expressions)
- Milestone: 3/5 compilers self-hosted

### Week 3: Codegen (Part 2) + Router + Bootstrap
- Days 15-17: Codegen compiler completion
- Days 18-19: Router compiler in MELP
- Days 20-21: Full bootstrap chain testing
- Milestone: **FULL SELF-HOSTING ACHIEVED**

---

## 📝 Testing Strategy

### Unit Tests (Per Compiler)
```bash
# Test each MELP compiler against C compiler
./test_lexer_compiler.sh    # Compare outputs
./test_parser_compiler.sh   # Compare outputs
./test_memory_compiler.sh   # Compare outputs
./test_codegen_compiler.sh  # Compare outputs
```

### Integration Tests
```bash
# Test full pipeline with MELP compilers
./bootstrap_stage4.sh --test-mode
```

### Bootstrap Tests
```bash
# Multi-generation bootstrap
./bootstrap_stage4.sh --verify-fixed-point
```

---

## 🚀 Stage 4 Deliverables

### Code
1. ✅ `lexer_compiler.mlp` (~600 lines)
2. ✅ `parser_compiler.mlp` (~900 lines)
3. ✅ `memory_compiler.mlp` (~400 lines)
4. ✅ `codegen_compiler.mlp` (~750 lines)
5. ✅ `router_compiler.mlp` (~150 lines)

### Scripts
1. ✅ `bootstrap_stage4.sh` - Full bootstrap script
2. ✅ `test_bootstrap.sh` - Bootstrap verification
3. ✅ Individual test scripts per compiler

### Documentation
1. ✅ `STAGE4_COMPLETE.md` - Final report
2. ✅ Updated `AI_HANDOFF_DOCUMENT.md`
3. ✅ Bootstrap guide
4. ✅ Self-hosting proof documentation

---

## 🎓 What Self-Hosting Means

### Definition
A self-hosting compiler can compile its own source code.

### Why It Matters
1. **Proof of completeness** - Language is powerful enough to write compilers
2. **Bootstrap independence** - No longer needs C compiler after initial bootstrap
3. **Dogfooding** - Compiler developers use their own tool
4. **Milestone achievement** - Major accomplishment in language development

### Famous Self-Hosting Compilers
- GCC (C compiler written in C)
- Rust (rustc written in Rust)
- Go (Go compiler written in Go)
- **MELP** (to be added to this list!) 🎯

---

## 🎯 Stage 4 Success Criteria

### Technical
- ✅ All 5 compilers rewritten in MELP
- ✅ MELP compilers compile themselves (bootstrap works)
- ✅ Output deterministic (fixed point reached)
- ✅ All Stage 3 tests still pass
- ✅ GC integration preserved
- ✅ Modular architecture maintained

### Quality
- ✅ Each MELP compiler under 1000 lines
- ✅ Clean, idiomatic MELP code
- ✅ Well-documented
- ✅ Comprehensive test coverage

### Performance
- ✅ Bootstrap completes in reasonable time (<5 minutes)
- ✅ Compiled output performance matches C compilers

---

## 🔄 Phase 1 Starting Point

**Next Immediate Step:**
Start with `lexer_compiler.mlp` - rewrite the enhanced lexer compiler from C to MELP.

**Why Start Here:**
1. Lexer is the first stage of pipeline
2. Simplest compiler (no complex AST structures)
3. Clear input/output (text → tokens)
4. Good warm-up for more complex compilers

**Ready to begin when you are!** 🚀

---

*Document created: 4 Aralık 2025*  
*Stage 3 Complete: ✅*  
*Stage 4 Status: Ready to Start*
