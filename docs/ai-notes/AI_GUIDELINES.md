# 🤖 AI Guidelines for MLP Compiler Project

**Last Updated:** November 20, 2025  
**Project:** MLP (Multi-Language Programming) v3.0  
**Goal:** Multi-language compiler with Base IR, self-hosting capable  

---

## ⚠️ CRITICAL: READ BEFORE ANY WORK

This document contains **mandatory rules** that ALL AI assistants MUST follow when working on the MLP project. Failure to follow these rules will result in:
- ❌ Breaking existing functionality
- ❌ Inconsistent type system
- ❌ Merge conflicts with other AI work
- ❌ Wasted development time

---

## 🔴 TYPE SYSTEM - MOST CRITICAL SECTION!

### ⚠️ ATTENTION: MLP Does NOT Have `int` or `float`!

**MLP's REAL data types:**

| User-Facing | Internal (Compiler) | Implementation | Description |
|-------------|---------------------|----------------|-------------|
| **SAYISAL** (Turkish) | `numeric` | **BigDecimal** | Unlimited precision - handles both integers and decimals |
| **SÖZEL** (Turkish) | `string` | **BigString** | Unlimited length strings |
| **ZITLIK** (Turkish) | `bool` | Boolean | True/False values |
| **HİÇLİK** (Turkish) | `null` | (future) | Null value (not yet implemented) |

### 🚨 WRONG vs RIGHT

```mlp
❌ WRONG: int x = 5;              -- "int" doesn't exist in MLP!
❌ WRONG: float pi = 3.14;        -- "float" doesn't exist in MLP!
✅ RIGHT:  numeric x = 5           -- Both integer and decimal
✅ RIGHT:  numeric pi = 3.14       -- Same type!
✅ RIGHT:  numeric result = x + pi -- No type conversion needed
```

**WHY?**
- MLP has **ONE numeric type**: `numeric` (internally BigDecimal)
- It handles **both integers and decimals** with unlimited precision
- No `int64_t`, no `double`, no `float` - just `numeric`
- User writes `SAYISAL` in Turkish, `целое` in Russian, `整数` in Chinese
- Preprocessor translates user keywords → `numeric` (internal compiler keyword)

### 📐 How BigDecimal Works

```mlp
numeric a = 5           # Stored as BigDecimal("5")
numeric b = 10.44       # Stored as BigDecimal("10.44")
numeric c = a + b       # Result: BigDecimal("15.44") - no precision loss
numeric d = 1 / 3       # Result: BigDecimal("0.333333...") - infinite precision
```

**Key Points:**
- ✅ No integer overflow (unlimited precision)
- ✅ No floating-point errors (0.1 + 0.2 = 0.3, not 0.30000000000000004)
- ✅ Exact decimal arithmetic
- ✅ Same type for all numbers (no conversion needed)

### 🛠️ Phase 4 Temporary Implementation

**Current Status (Phase 4):**
```c
// ⚠️ TEMPORARY IMPLEMENTATION - Will be replaced with BigDecimal
int64_t   // Used temporarily for integers
double    // Used temporarily for decimals
```

**Why temporary?**
- Phase 4 needed quick math operations
- BigDecimal implementation is complex (Phase 5+)
- Current `int64_t`/`double` will be **replaced** with BigDecimal library

**Documentation must state:**
> **Note:** Phase 4 uses `int64_t` and `double` as temporary implementations. 
> Final goal is single `numeric` type backed by BigDecimal for unlimited precision.

---

## 📚 MANDATORY READING BEFORE ANY WORK

### 1. 🔒 Core Documentation (ALWAYS READ FIRST)

```bash
# Critical files - READ IN THIS ORDER:
1. AI_RULES.md          # Turkish-language specific rules
2. AI_GUIDELINES.md     # This file (English guidelines)
3. CONTRIBUTING.md      # Contribution guidelines
4. SPECS.md             # Language specification (v3.0)
5. BASE_SYNTAX.md       # Base IR syntax
```

**Why this order?**
- `AI_RULES.md` → Language-specific rules (Turkish SAYISAL, etc.)
- `AI_GUIDELINES.md` → Technical implementation rules
- `CONTRIBUTING.md` → Type system explanation
- `SPECS.md` → Complete language specification
- `BASE_SYNTAX.md` → IR syntax details

### 2. 📖 Phase Documentation

Check which phase you're working on:

```bash
# Phase 3: Hash Tables, File I/O, String Operations
- README_TESTS_PHASE3.md
- Phase 3 C tests in runtime/

# Phase 4: Hash Maps, Type Conversions, Math Operations
- README_TESTS_PHASE4.md
- PHASE4_BRIEF_CLAUDE1_IMPLEMENTATION.md
- PHASE4_BRIEF_CLAUDE2_DOCS.md
- PHASE4_BRIEF_CLAUDE3_TESTING.md
- TYPE_CONVERSIONS_DESIGN.md
- MATH_OPERATIONS_DESIGN.md
- HASHMAP_COMPILER_DESIGN.md
```

### 3. 🔍 Check Previous AI Work

```bash
# Find latest work
git log --oneline -10

# Check all branches
git branch -a | grep phase

# Read recent commits
git show HEAD
git show HEAD~1
git show HEAD~2
```

---

## 🚫 THINGS YOU MUST NOT DO (CRITICAL!)

### 1. ❌ DO NOT Use `int` or `float` Types!

**WRONG:**
```c
int x = 42;
float pi = 3.14;
int result = x * 2;
```

**RIGHT:**
```c
// Use BigDecimal (future implementation)
// For now, document as temporary:
int64_t x = 42;  // TODO: Replace with BigDecimal
double pi = 3.14;  // TODO: Replace with BigDecimal

// Always add comment explaining this is temporary!
```

### 2. ❌ DO NOT Create Separate Type Conversion Functions!

**WRONG:**
```c
int string_to_int(const char* str);      // NO! Wrong type
float string_to_float(const char* str);  // NO! Wrong type
```

**RIGHT:**
```c
BigDecimal string_to_numeric(const char* str);  // Future implementation
// For Phase 4 (temporary):
int64_t string_to_int(const char* str);     // TODO: Merge into string_to_numeric
double string_to_float(const char* str);    // TODO: Merge into string_to_numeric
```

### 3. ❌ DO NOT Ignore Existing Documentation!

Before adding any feature:
```bash
# Search if it already exists
grep -r "feature_name" *.md
grep -r "feature_name" src/

# Check AI_RULES.md
cat AI_RULES.md | grep -i "feature_name"

# Check CONTRIBUTING.md
cat CONTRIBUTING.md | grep -i "feature_name"
```

### 4. ❌ DO NOT Mix Language Keywords!

**WRONG (in Turkish code):**
```mlp
int x = 5              -- Wrong! Use SAYISAL
SAYISAL y = 10
if x > y then          -- Wrong! Mixing English/Turkish
```

**RIGHT (in Turkish code):**
```mlp
SAYISAL x = 5
SAYISAL y = 10
EĞER x > y İSE        -- All Turkish
```

---

## ✅ THINGS YOU MUST DO (CRITICAL!)

### 1. ✅ Always Check Type System Rules

Before writing ANY code:
```bash
# Check type definitions
cat AI_RULES.md | grep -A 10 "VERİ TİPLERİ"
cat CONTRIBUTING.md | grep -A 10 "Data Types"

# Verify BigDecimal concept
grep -r "BigDecimal" *.md
```

### 2. ✅ Document Temporary Implementations

When using `int64_t` or `double`:
```c
// ⚠️ TEMPORARY: Phase 4 uses int64_t for quick implementation
// TODO (Phase 5): Replace with BigDecimal for unlimited precision
int64_t math_abs(int64_t num) {
    return num < 0 ? -num : num;
}
```

### 3. ✅ Add Migration Notes

In documentation:
```markdown
## Type System Migration

**Current (Phase 4):** Uses `int64_t` and `double`
**Future (Phase 5+):** Will use single `numeric` type (BigDecimal)

**Migration Path:**
1. Replace all `int64_t` → `BigDecimal`
2. Replace all `double` → `BigDecimal`
3. Merge type conversion functions
4. Update all tests
```

### 4. ✅ Test Against Existing Tests

```bash
# Run Phase 3 tests (must all pass)
cd runtime
./test_hashmap
./test_file_io
./test_string_ops

# Run Phase 4 tests (must all pass)
./test_type_conversion
./test_math_ops

# Verify all C tests pass
for test in test_*; do
    [ -x "$test" ] && ./"$test"
done
```

---

## 🔧 DEVELOPMENT WORKFLOW

### 1. Before Starting Any Work

```bash
# 1. Read ALL mandatory docs
cat AI_RULES.md
cat AI_GUIDELINES.md
cat CONTRIBUTING.md

# 2. Check current branch
git branch --show-current

# 3. Check for existing implementation
grep -r "feature_name" src/
grep -r "feature_name" *.md

# 4. Run existing tests
cd runtime && for t in test_*; do [ -x "$t" ] && ./"$t"; done

# 5. Check type system rules
grep -i "numeric\|bigdecimal\|sayisal" AI_RULES.md
```

### 2. During Development

```bash
# Verify type system consistency
grep -n "int64_t\|double\|float" src/*.c | head -20

# Check for type violations
grep -n "int x =\|float y =" src/*.c  # Should find NONE

# Ensure BigDecimal comments exist
grep -n "TODO.*BigDecimal\|TEMPORARY.*BigDecimal" src/*.c
```

### 3. Before Committing

```bash
# Run ALL tests
cd runtime
for test in test_*; do
    if [ -x "$test" ]; then
        echo "Running $test..."
        ./"$test" || echo "❌ FAILED: $test"
    fi
done

# Check documentation consistency
grep -i "int\|float" *.md | grep -v "BigDecimal\|TEMPORARY\|TODO"

# Verify no type system violations
git diff | grep -E "^\+.*int [a-z]+ =|^\+.*float [a-z]+ ="
```

### 4. Commit Message Format

```bash
# Good examples:
git commit -m "docs: Add BigDecimal migration notes to Phase 4 docs"
git commit -m "fix: Update type system comments (int64_t → BigDecimal TODO)"
git commit -m "test: Verify Phase 4 temporary types with migration plan"

# Bad examples (AVOID):
git commit -m "update"
git commit -m "fix tests"
git commit -m "working now"
```

---

## 📊 PROJECT STRUCTURE

```
MLP/
├── AI_RULES.md                 # 🔴 Turkish-language rules
├── AI_GUIDELINES.md            # 🔴 This file (English rules)
├── CONTRIBUTING.md             # Type system explanation
├── SPECS.md                    # Language specification v3.0
├── BASE_SYNTAX.md              # Base IR syntax
├── README_TESTS_PHASE3.md      # Phase 3 test documentation
├── README_TESTS_PHASE4.md      # Phase 4 test documentation
├── TYPE_CONVERSIONS_DESIGN.md  # Type conversion specs (⚠️ temporary types)
├── MATH_OPERATIONS_DESIGN.md   # Math operations specs (⚠️ temporary types)
├── HASHMAP_COMPILER_DESIGN.md  # Hash map design
├── ROADMAP.md                  # Future development plans
├── runtime/
│   ├── runtime.c               # Runtime library
│   ├── runtime.h               # Runtime header
│   ├── hashmap.c/h             # Hash map implementation
│   ├── test_*.c                # C unit tests
│   └── test_* (executables)    # Compiled tests
├── test/
│   ├── test_*.mlp              # MLP integration tests
│   └── benchmark_*.mlp         # Performance benchmarks
├── examples/
│   ├── hashmap_examples.mlp    # Hash map usage examples
│   ├── type_conversion_examples.mlp
│   └── math_examples.mlp
├── src/
│   ├── lexer/                  # Lexical analysis
│   ├── parser/                 # Syntax analysis
│   ├── codegen/                # Code generation
│   └── compiler/               # Compiler main
├── docs/
│   └── BOOTSTRAP.md            # Bootstrap process
└── gui_editor/                 # Visual designer (HTML5 + Flask)
```

---

## 🎯 CURRENT PRIORITIES

### Phase 4 (Current - November 2025)
- ✅ Hash maps implementation
- ✅ Type conversions (⚠️ using int64_t/double temporarily)
- ✅ Math operations (⚠️ using int64_t/double temporarily)
- ✅ Testing suite (88 tests, 47/47 C passing)
- ⚠️ **Documentation update needed:** Add BigDecimal migration notes

### Phase 5 (Next - Q1 2026)
- 🎯 BigDecimal implementation
- 🎯 Replace all int64_t → BigDecimal
- 🎯 Replace all double → BigDecimal
- 🎯 Merge type conversion functions
- 🎯 Update all tests for BigDecimal

### Phase 6+ (Future)
- 📅 Advanced data structures (arrays, structs)
- 📅 Self-hosting compiler (write MLP in MLP)
- 📅 Optimization passes
- 📅 Standard library expansion

---

## 🔍 FREQUENTLY ASKED QUESTIONS

### Q: Why BigDecimal instead of int/float?

**A:** 
1. **No overflow:** `999999999 * 999999999` works correctly
2. **No precision loss:** `0.1 + 0.2 = 0.3` exactly
3. **Single type:** No int↔float conversions needed
4. **Predictable:** Exact arithmetic, no floating-point surprises

### Q: Why does Phase 4 use int64_t/double if BigDecimal is the goal?

**A:** 
- BigDecimal implementation is complex (~2000 lines C code)
- Phase 4 needed quick prototyping for math operations
- Temporary types allow testing algorithm correctness
- Phase 5 will replace with proper BigDecimal

### Q: How do I know if I'm using temporary types correctly?

**A:**
```c
// ✅ CORRECT: Documented as temporary
int64_t math_abs(int64_t num) {  // TODO (Phase 5): Replace with BigDecimal
    return num < 0 ? -num : num;
}

// ❌ WRONG: No documentation
int math_abs(int num) {
    return num < 0 ? -num : num;
}
```

### Q: What if I see `int` or `float` in existing code?

**A:** 
1. Check if it has `TODO` comment
2. If yes → it's documented temporary implementation
3. If no → add comment explaining it's temporary
4. Never write new code with `int`/`float` without TODO comment

### Q: How do type conversions work with BigDecimal?

**A:**
```mlp
# Current (Phase 4 - temporary):
numeric x = string_to_int("42")      # Returns int64_t
numeric y = string_to_float("3.14")  # Returns double

# Future (Phase 5+ - final):
numeric x = string_to_numeric("42")     # Returns BigDecimal
numeric y = string_to_numeric("3.14")   # Same function!
```

---

## 📖 TYPE SYSTEM RULES SUMMARY

| Rule | ✅ Do This | ❌ Don't Do This |
|------|-----------|-----------------|
| **Type Declaration** | `numeric x = 5` | `int x = 5` |
| **Decimal Numbers** | `numeric pi = 3.14` | `float pi = 3.14` |
| **Type Conversion** | `string_to_numeric()` (future) | `string_to_int()`, `string_to_float()` (separate) |
| **Documentation** | Add `TODO: BigDecimal` comments | Silent use of int64_t/double |
| **Testing** | Test with both integers and decimals | Test only integers OR only decimals |

---

## 🚀 QUICK START CHECKLIST

Before working on MLP, complete this checklist:

- [ ] Read `AI_RULES.md` (Turkish-language rules)
- [ ] Read `AI_GUIDELINES.md` (this file)
- [ ] Read `CONTRIBUTING.md` (type system section)
- [ ] Understand: MLP has **one numeric type** (BigDecimal)
- [ ] Understand: Phase 4 uses int64_t/double as **temporary** implementation
- [ ] Know: All temporary types must have `TODO: BigDecimal` comments
- [ ] Checked: Existing tests still pass
- [ ] Verified: No documentation conflicts with type system

---

## 📝 COMMON MISTAKES (AVOID THESE!)

### Mistake 1: Adding int/float without documentation
```c
// ❌ WRONG
int calculate(int a, int b) {
    return a + b;
}

// ✅ RIGHT
int64_t calculate(int64_t a, int64_t b) {  // TODO (Phase 5): Replace with BigDecimal
    return a + b;
}
```

### Mistake 2: Separate type conversion functions
```c
// ❌ WRONG - Implies two different numeric types
int string_to_int(const char* str);
float string_to_float(const char* str);

// ✅ RIGHT - Single numeric type
BigDecimal string_to_numeric(const char* str);  // Future (Phase 5)
// Temporary (Phase 4):
int64_t string_to_int(const char* str);     // TODO: Merge into string_to_numeric
double string_to_float(const char* str);    // TODO: Merge into string_to_numeric
```

### Mistake 3: Not reading AI_RULES.md
```mlp
# ❌ WRONG - Using "int" keyword in Turkish code
int x = 5

# ✅ RIGHT - Using SAYISAL (Turkish) or numeric (internal)
SAYISAL x = 5  # User-facing Turkish
numeric x = 5  # Internal compiler representation
```

---

## 💡 TIPS FOR NEW AI ASSISTANTS

1. **Always search first:** Use `grep -r "keyword" *.md` before implementing
2. **Check git history:** `git log --grep="feature"` shows past decisions
3. **Test frequently:** Run tests after every change
4. **Document temporary code:** Add `TODO (Phase X)` comments
5. **Ask when unsure:** Better to clarify than break existing work

---

## 🎓 LEARNING PATH

### Day 1: Understanding the Project
1. Read `AI_RULES.md` (30 min)
2. Read `AI_GUIDELINES.md` (this file, 30 min)
3. Read `CONTRIBUTING.md` data types section (15 min)
4. Run existing tests to verify setup (10 min)

### Day 2: Type System Deep Dive
1. Read `TYPE_CONVERSIONS_DESIGN.md` (20 min)
2. Read `MATH_OPERATIONS_DESIGN.md` (20 min)
3. Check runtime.c for temporary type usage (15 min)
4. Read Phase 4 briefs (30 min)

### Day 3: Hands-On
1. Make a small test change
2. Run all tests
3. Check documentation consistency
4. Commit with proper message

---

## 📞 WHEN TO ASK THE USER

**Always ask the user when:**
- You want to change the type system design
- You're unsure if something is temporary or permanent
- You find conflicting information in documentation
- You want to add a new data type
- You want to change existing API signatures
- Tests fail and you don't know why

**Never assume:**
- That `int` or `float` are correct types
- That separate functions for int/float conversions are needed
- That you can skip reading documentation

---

## ✅ SUCCESS CRITERIA

Your work is correct if:
- ✅ All existing tests pass (47/47 C tests)
- ✅ No new `int`/`float` types without `TODO: BigDecimal` comments
- ✅ Documentation updated with migration notes
- ✅ Code follows type system rules (single numeric type philosophy)
- ✅ Commit messages are descriptive
- ✅ No conflicts with AI_RULES.md or CONTRIBUTING.md

---

**Last Updated:** November 20, 2025  
**Version:** 1.0  
**Status:** 🔒 Active Guidelines  

**Notes:**
- This document is for AI assistants working on MLP
- Human developers should also read this
- Must be kept in sync with AI_RULES.md and CONTRIBUTING.md
- Type system rules are CRITICAL and must not be violated

---

## 🔗 Related Documentation

- `AI_RULES.md` - Turkish-language specific rules
- `CONTRIBUTING.md` - Contribution guidelines with type system details
- `SPECS.md` - Complete language specification
- `BASE_SYNTAX.md` - Base IR syntax reference
- `TYPE_CONVERSIONS_DESIGN.md` - Type conversion specifications
- `MATH_OPERATIONS_DESIGN.md` - Math operations specifications
- `README_TESTS_PHASE4.md` - Phase 4 testing documentation
- `ROADMAP.md` - Future development plans
