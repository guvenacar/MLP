# MODULAR SELF-HOSTING CHECKLIST
## Strategy: Each Module Self-Hosts Independently, Then Router Integrates

**Date Started:** 4 Aralık 2024 09:00  
**Git Branch:** melp20241204  
**Last Commit:** b360d23 (MVL Created)

---

## 🎯 Core Philosophy
- Each module < 500 lines (avoid timeout)
- Each module compiles itself FIRST
- Build incrementally: MVL → MVL+ → MVL++ → Full MELP
- Router integrates at the END
- Test each step before proceeding

---

## MODULE 1: MVL (Minimal Variable Language)
**Purpose:** Smallest possible self-hosting language  
**Features:** let, arithmetic (+, -, *, /), print_int, return  
**File:** `/melp/compiler/mvl/mvl_compiler.mlp`

### Checklist:
- [x] 1.1 Create mvl_compiler.mlp skeleton
  - Status: ✅ DONE (17 lines, 2 functions)
  - File: mvl_compiler.mlp
  - Output: 1 (skeleton works)
  
- [ ] 1.2 Add MVL lexer (tokenize MVL syntax)
  - Status: ⏳ TODO
  - Functions needed: tokenize_mvl()
  - Tokens: LET, IDENTIFIER, NUMBER, OPERATOR (+,-,*,/), SEMICOLON, EQUALS
  
- [ ] 1.3 Add MVL parser (build simple AST)
  - Status: ⏳ TODO
  - Functions needed: parse_statement(), parse_expression()
  - AST: VarDecl, BinaryOp, PrintCall, Return
  
- [ ] 1.4 Add MVL codegen (generate x86-64 assembly)
  - Status: ⏳ TODO
  - Functions needed: codegen_statement(), codegen_expression()
  - Output: .s file with assembly
  
- [ ] 1.5 Bootstrap Test: MVL compiles test.mvl
  - Status: ⏳ TODO
  - Command: `./mvl_compiler test.mvl test.s`
  - Expected: test.s created, gcc test.s works
  
- [ ] 1.6 Self-Host Test: MVL compiles mvl_compiler.mlp
  - Status: ⏳ TODO
  - Command: `./mvl_compiler mvl_compiler.mlp mvl_compiler_v2.s`
  - Expected: mvl_compiler_v2 == mvl_compiler (binary identical)

**Lines:** Target < 300 lines  
**Completion:** 1/6 tasks (17%)

---

## MODULE 2: MVL+L (MVL + Lexer Module)
**Purpose:** Add lexer-specific features to MVL  
**New Features:** struct Token, array, string operations, while loop  
**File:** `/melp/compiler/mvl_plus_lexer/mvl_lexer_compiler.mlp`

### Checklist:
- [ ] 2.1 Extend MVL with struct support
  - Status: ⏳ TODO
  - Add: struct Token { type, value, line, column }
  - Functions: create_token(), token_type()
  
- [ ] 2.2 Extend MVL with array support
  - Status: ⏳ TODO
  - Add: let tokens[100];
  - Functions: array_get(), array_set()
  
- [ ] 2.3 Extend MVL with string operations
  - Status: ⏳ TODO
  - Add: string_length(), string_char_at(), string_compare()
  
- [ ] 2.4 Extend MVL with while loop
  - Status: ⏳ TODO
  - Add: while (condition) { ... }
  
- [ ] 2.5 Implement lexer_init(), lexer_advance()
  - Status: ⏳ TODO
  - Full lexer functionality for MELP
  
- [ ] 2.6 Bootstrap Test: MVL+L compiles lexer.mlp
  - Status: ⏳ TODO
  - Command: `./mvl_lexer_compiler lexer.mlp lexer_v2.s`
  
- [ ] 2.7 Self-Host Test: MVL+L compiles itself
  - Status: ⏳ TODO
  - Command: `./mvl_lexer_compiler mvl_lexer_compiler.mlp mvl_lexer_v2.s`

**Lines:** Target < 400 lines  
**Completion:** 0/7 tasks (0%)

---

## MODULE 3: MVL+P (MVL + Parser Module)
**Purpose:** Add parser-specific features to MVL+L  
**New Features:** AST nodes, recursion, function pointers  
**File:** `/melp/compiler/mvl_plus_parser/mvl_parser_compiler.mlp`

### Checklist:
- [ ] 3.1 Extend MVL+L with AST node structures
  - Status: ⏳ TODO
  - Add: struct Node { type, left, right, value }
  
- [ ] 3.2 Extend MVL+L with recursion support
  - Status: ⏳ TODO
  - Test: Recursive descent parser functions
  
- [ ] 3.3 Implement parse_program(), parse_function()
  - Status: ⏳ TODO
  - Full parser functionality for MELP
  
- [ ] 3.4 Implement parse_statement(), parse_expression()
  - Status: ⏳ TODO
  - Handle all MELP statement types
  
- [ ] 3.5 Bootstrap Test: MVL+P compiles parser.mlp
  - Status: ⏳ TODO
  - Command: `./mvl_parser_compiler parser.mlp parser_v2.s`
  
- [ ] 3.6 Self-Host Test: MVL+P compiles itself
  - Status: ⏳ TODO
  - Command: `./mvl_parser_compiler mvl_parser_compiler.mlp mvl_parser_v2.s`

**Lines:** Target < 500 lines  
**Completion:** 0/6 tasks (0%)

---

## MODULE 4: MVL+M (MVL + Memory Module)
**Purpose:** Add memory management to MVL+P  
**New Features:** malloc/free wrappers, allocation tracking  
**File:** `/melp/compiler/mvl_plus_memory/mvl_memory_compiler.mlp`

### Checklist:
- [ ] 4.1 Extend MVL+P with malloc/free support
  - Status: ⏳ TODO
  - Add: allocate(), deallocate()
  
- [ ] 4.2 Implement memory tracking structures
  - Status: ⏳ TODO
  - Add: struct Allocation { ptr, size, line }
  
- [ ] 4.3 Implement memory_init(), memory_alloc()
  - Status: ⏳ TODO
  - Full memory management for MELP
  
- [ ] 4.4 Bootstrap Test: MVL+M compiles memory.mlp
  - Status: ⏳ TODO
  - Command: `./mvl_memory_compiler memory.mlp memory_v2.s`
  
- [ ] 4.5 Self-Host Test: MVL+M compiles itself
  - Status: ⏳ TODO
  - Command: `./mvl_memory_compiler mvl_memory_compiler.mlp mvl_memory_v2.s`

**Lines:** Target < 350 lines  
**Completion:** 0/5 tasks (0%)

---

## MODULE 5: MVL+C (MVL + Codegen Module)
**Purpose:** Add assembly generation to MVL+M  
**New Features:** x86-64 instruction emission, register allocation  
**File:** `/melp/compiler/mvl_plus_codegen/mvl_codegen_compiler.mlp`

### Checklist:
- [ ] 5.1 Extend MVL+M with assembly emission
  - Status: ⏳ TODO
  - Add: emit_asm(), emit_label()
  
- [ ] 5.2 Implement register allocation
  - Status: ⏳ TODO
  - Add: alloc_register(), free_register()
  
- [ ] 5.3 Implement codegen_function(), codegen_statement()
  - Status: ⏳ TODO
  - Full codegen functionality for MELP
  
- [ ] 5.4 Bootstrap Test: MVL+C compiles codegen.mlp
  - Status: ⏳ TODO
  - Command: `./mvl_codegen_compiler codegen.mlp codegen_v2.s`
  
- [ ] 5.5 Self-Host Test: MVL+C compiles itself
  - Status: ⏳ TODO
  - Command: `./mvl_codegen_compiler mvl_codegen_compiler.mlp mvl_codegen_v2.s`

**Lines:** Target < 500 lines  
**Completion:** 0/5 tasks (0%)

---

## MODULE 6: MELPC Router (Integration Layer)
**Purpose:** Orchestrate all modules  
**Strategy:** "lexer işi? lexer modülüne git"  
**File:** `/melp/compiler/router/melpc_router.mlp`

### Checklist:
- [ ] 6.1 Create router skeleton
  - Status: ⏳ TODO
  - Functions: route_request(), call_module()
  
- [ ] 6.2 Implement module dispatch logic
  - Status: ⏳ TODO
  - Routes: lex → MVL+L, parse → MVL+P, memory → MVL+M, codegen → MVL+C
  
- [ ] 6.3 Implement pipeline orchestration
  - Status: ⏳ TODO
  - Flow: source → lex → tokens → parse → AST → memory → codegen → assembly
  
- [ ] 6.4 Integration Test: melpc compiles simple.mlp
  - Status: ⏳ TODO
  - Command: `./melpc simple.mlp simple.s`
  
- [ ] 6.5 Integration Test: melpc compiles all 5 modules
  - Status: ⏳ TODO
  - Test: lexer.mlp, parser.mlp, memory.mlp, codegen.mlp, melpc.mlp
  
- [ ] 6.6 FINAL Self-Host: melpc compiles itself
  - Status: ⏳ TODO
  - Command: `./melpc melpc_router.mlp melpc_v2.s`
  - Expected: melpc_v2 == melpc (Stage 4 COMPLETE!)

**Lines:** Target < 200 lines  
**Completion:** 0/6 tasks (0%)

---

## 📊 Overall Progress

### Modules Completed:
- [x] Module 1: MVL (17% - skeleton only)
- [ ] Module 2: MVL+L (0%)
- [ ] Module 3: MVL+P (0%)
- [ ] Module 4: MVL+M (0%)
- [ ] Module 5: MVL+C (0%)
- [ ] Module 6: MELPC Router (0%)

### Total Tasks: 35
- ✅ Completed: 1
- ⏳ In Progress: 0
- 🔲 TODO: 34

**Overall Completion: 3% (1/35 tasks)**

---

## 🎯 Current Focus: MODULE 1 - MVL

**Next Action:** Task 1.2 - Add MVL lexer (tokenize_mvl)

**Strategy:**
1. Keep mvl_compiler.mlp under 300 lines
2. Test each function as we add it
3. Use simple_codegen to compile after each addition
4. Verify output at each step

**Blocking Issues:** None (greenfield)

**Dependencies:** simple_codegen.c must support:
- [x] Functions with parameters ✅
- [x] Function calls ✅
- [x] Return expressions ✅
- [ ] Arrays (will add when needed for Module 2)
- [ ] Structs (will add when needed for Module 2)
- [ ] While loops (will add when needed for Module 2)

---

## 📝 Notes

### Why This Strategy Works:
1. **Incremental Complexity:** Each module adds ONE new concept
2. **Testable:** Each step produces a working compiler
3. **No Timeouts:** Small files compile quickly
4. **Visible Progress:** 35 checkboxes = clear milestones
5. **Modular:** Failure in one module doesn't break others

### Lessons From Previous Attempts:
- ❌ Monolithic simple_codegen.c → would grow to 10,000+ lines
- ❌ lexer_compiler.mlp (498 lines) → timeout after 3 functions
- ✅ MVL (17 lines) → compiles in seconds
- ✅ Modular approach → each module self-hosts independently

### Git Strategy:
- Commit after each completed task
- Branch: melp20241204
- Tag each module completion: mvl-complete, mvl-l-complete, etc.

---

## 🚀 Success Criteria

**Stage 4 Complete When:**
1. ✅ All 6 modules self-host independently
2. ✅ melpc router orchestrates all modules
3. ✅ melpc compiles itself (full self-hosting chain)
4. ✅ Binary comparison: melpc_v1 == melpc_v2
5. ✅ All modules < 500 lines (no monoliths)

**Victory Condition:**
```bash
./melpc melpc_router.mlp melpc_v2.s
gcc melpc_v2.s -o melpc_v2
./melpc_v2 melpc_router.mlp melpc_v3.s
diff melpc_v2.s melpc_v3.s  # Should be identical!
```

---

**Last Updated:** 4 Aralık 2024 09:10  
**Status:** Module 1 (MVL) in progress - Task 1.2 next
