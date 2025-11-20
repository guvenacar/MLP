# MLP Turing Complete Modernization Plan

**Date:** November 20, 2025
**Goal:** Make MLP a modern, Turing-complete language with full data structures

---

## 🔍 Current Status Analysis

### ✅ What Works (Confirmed by Tests)

**1. Arrays - FULLY FUNCTIONAL**
```mlp
int numbers[5];
numbers[0] = 10
numbers[1] = 20
print numbers[0]  -- Works!
```

- **Declaration:** ✅ `int arr[size];`
- **Access:** ✅ `arr[index]`
- **Assignment:** ✅ `arr[index] = value`
- **Code Generation:** ✅ Full assembly support
- **Test File:** `test_array.mlp` compiles successfully

**2. Structs - PARTIAL (Definition Only)**
```mlp
struct Person then
    string name;
    int age;
end
-- Works! But cannot create instances yet
```

- **Definition:** ✅ `struct Name then ... end`
- **Fields:** ✅ Multiple typed fields
- **Instance Creation:** ❌ **BLOCKED** - Not implemented
- **Field Access:** ❓ Untested (blocked on instance creation)
- **Test File:** `test_struct_simple.mlp` compiles

---

## ❌ What's Missing

### 1. Struct Instance Creation (HIGH PRIORITY)

**Problem:**
```mlp
struct Person then
    string name;
    int age;
end

Person p;  -- ❌ FAILS: Parser doesn't recognize this pattern
p.name = "Alice"  -- ❌ Cannot test (no instance)
```

**Why it fails:**
- Parser expects `IDENTIFIER IDENTIFIER SEMICOLON` pattern
- Needs `peek_token()` function (not implemented)
- TODO comment in `c_compiler/c_parser.c:504`

**Implementation Needed:**
1. Add `peek_token()` to parser
2. Modify `komut()` to detect struct instance pattern
3. Call `createAST_StructDegisken()` (already exists!)
4. Test field access and assignment

**Estimated Effort:** 2-3 hours

### 2. Dynamic Lists (MEDIUM PRIORITY)

**Desired Syntax:**
```mlp
list[int] numbers = list();
numbers.add(10);
numbers.add(20);
int first = numbers.get(0);
int size = numbers.size();
```

**Implementation Needed:**
- New AST node: `AST_LIST_DECLARATION`
- List operations: add, get, remove, size
- Dynamic memory management (malloc/realloc in runtime)
- Bounds checking

**Estimated Effort:** 1 week

### 3. Dictionaries/Maps (MEDIUM PRIORITY)

**Desired Syntax:**
```mlp
dict[string, string] keywords = dict();
keywords.set("int", "sayısal");
keywords.set("if", "eğer");
string value = keywords.get("int");  -- "sayısal"
```

**Implementation Needed:**
- New AST node: `AST_DICT_DECLARATION`
- Hash table implementation in runtime
- Operations: set, get, has, remove, keys, values
- Generic types support

**Estimated Effort:** 1-2 weeks

### 4. Classes/Objects (LOW PRIORITY)

**Desired Syntax:**
```mlp
class Animal then
    string name;

    function speak() then
        print "Animal sound"
    end
end

class Dog extends Animal then
    function speak() then
        print "Woof!"
    end
end

Dog d = Dog();
d.name = "Buddy"
d.speak()  -- "Woof!"
```

**Implementation Needed:**
- Inheritance support
- Method binding
- `this`/`self` keyword
- Virtual method tables
- Constructor/destructor

**Estimated Effort:** 2-3 weeks

---

## 🎯 Implementation Roadmap

### Phase 1: Complete Structs (1-2 days)
**Priority:** 🔴 CRITICAL

**Tasks:**
1. ✅ Struct definition (DONE)
2. ❌ Add `peek_token()` to parser
3. ❌ Implement struct instance creation
4. ❌ Test struct field access
5. ❌ Test struct field assignment
6. ❌ Document struct usage

**Deliverable:** Full struct support working

### Phase 2: Dynamic Lists (1 week)
**Priority:** 🟡 HIGH

**Tasks:**
1. Design list API (add, get, remove, size, etc.)
2. Implement AST nodes for list operations
3. Add list runtime functions (C code)
4. Implement parser support
5. Implement code generator
6. Write comprehensive tests
7. Document list usage

**Deliverable:** Dynamic, resizable lists working

### Phase 3: Dictionaries (1-2 weeks)
**Priority:** 🟡 HIGH

**Tasks:**
1. Design hash table implementation
2. Implement AST nodes for dict operations
3. Add hash table runtime (C code)
4. Support string keys (primary use case)
5. Support generic key/value types
6. Implement parser support
7. Implement code generator
8. Collision handling (chaining or open addressing)
9. Write comprehensive tests
10. Document dict usage

**Deliverable:** Hash maps working for keyword translation

### Phase 4: Classes (2-3 weeks)
**Priority:** 🟢 MEDIUM

**Tasks:**
1. Design OOP model (prototypal vs classical)
2. Implement inheritance
3. Add method support
4. Implement `this` keyword
5. Virtual method tables
6. Constructor support
7. Parser & code generator updates
8. Comprehensive tests
9. Documentation

**Deliverable:** Basic OOP support

---

## 📊 Impact on Self-Hosting

### Before (Current State):
```
❌ JSON parsing: Impossible (no arrays for tokens)
❌ Keyword mapping: Impossible (no dictionaries)
❌ Complex data: Impossible (no structs)
⚠️  Workarounds needed: String manipulation only
```

### After Phase 1 (Structs Complete):
```
✅ JSON tokens: Store in structs
✅ Complex data: Possible with structs
⚠️  Still need arrays for token lists
⚠️  Still need dicts for keyword mapping
```

### After Phase 2 (Lists Complete):
```
✅ Token storage: list[Token] works!
✅ Dynamic data: Resizable structures
⚠️  Keyword mapping still needs dicts
```

### After Phase 3 (Dicts Complete):
```
✅ JSON parsing: Fully possible!
✅ Keyword mapping: dict[string, string]
✅ Config loading: diller_comprehensive.json
✅ Preprocessor: Can be written in pure MLP!
🎉 SELF-HOSTING UNLOCKED!
```

---

## 🔧 Technical Implementation Details

### Struct Instance Creation (Detailed Plan)

**Current Parser Flow:**
```
komut() called
  → Checks token type
  → If TOKEN_TANIMLA_SAYI: variable declaration
  → If TOKEN_IDENTIFIER: expression/assignment
  → NO CHECK for "IDENTIFIER IDENTIFIER SEMICOLON"
```

**Needed Changes:**

**1. Add peek_token() function:**
```c
// In c_parser.c
static Token* peek_token() {
    // Save current position
    // Get next token
    // Restore position
    // Return peeked token
}
```

**Problem:** Current implementation uses `getNextToken()` which consumes tokens. Need token stream.

**Alternative Solution (Easier):**
```c
// In komut(), before checking IDENTIFIER:
if (current_token->type == TOKEN_IDENTIFIER) {
    // Save current identifier
    char* potential_struct_type = current_token->value;

    Token* saved = current_token;
    Token* next = getNextToken();

    // Check if next is also IDENTIFIER
    if (next && next->type == TOKEN_IDENTIFIER) {
        // This is: StructName varName
        char* var_name = next->value;
        Token* third = getNextToken();

        if (third && third->type == TOKEN_SEMICOLON) {
            // This is struct instance: StructName varName;
            return createAST_StructDegisken(...);
        }
    }

    // Not struct instance, restore and continue normal flow
    // ... backtracking needed
}
```

**Problem:** Backtracking not supported.

**BEST SOLUTION: Refactor token handling to support lookahead**

---

## 🚀 Immediate Next Steps

### Today/Tomorrow:
1. ✅ Test arrays (DONE - working!)
2. ✅ Test struct definition (DONE - working!)
3. ❌ Implement `peek_token()` or token lookahead
4. ❌ Implement struct instance creation
5. ❌ Test struct field operations

### This Week:
6. Design list API
7. Begin list implementation
8. Update SESSION_SUMMARY.md

### Next Week:
9. Complete lists
10. Begin dictionary implementation
11. Test preprocessor use case

---

## 📝 Notes & Considerations

### Language Design Philosophy

**Question:** Should MLP follow modern or classical approaches?

**Options:**
1. **C-style:** Explicit, manual (current)
2. **Python-style:** Dynamic, implicit
3. **Rust-style:** Safe, ownership-based
4. **Hybrid:** Best of all worlds

**Current Leanings:**
- ✅ Explicit syntax (VB.NET-like)
- ✅ Simple, readable
- ✅ No hidden magic
- ⚠️ Could use more safety features

### Memory Management

**Current:** Manual (C-style)
**Future Options:**
1. Reference counting (Python-style)
2. Garbage collection (Java-style)
3. Ownership (Rust-style)
4. Keep manual (C-style)

**Recommendation:** Start with manual, add GC later if needed

### Generic Types

**For lists and dicts, need generic types:**
```mlp
list[int] numbers
list[string] names
dict[string, int] ages
```

**Implementation:**
- Templates (C++-style)
- Type erasure (Java-style)
- Simple: Compile-time specialization

---

## 📚 References

- **AST Nodes:** `c_compiler/c_ast.h`
- **Parser:** `c_compiler/c_parser.c`
- **Generator:** `c_compiler/c_generator.c`
- **Tests:** `test_array.mlp`, `test_struct.mlp`, `test_struct_simple.mlp`
- **Session Summary:** `SESSION_SUMMARY.md`

---

## ✅ Success Criteria

### Phase 1 Complete When:
- [ ] Struct instances can be created
- [ ] Struct fields can be accessed
- [ ] Struct fields can be assigned
- [ ] All tests pass
- [ ] Documentation updated

### Project Complete When:
- [ ] Arrays ✅
- [ ] Structs ✅
- [ ] Lists ✅
- [ ] Dictionaries ✅
- [ ] JSON parser written in MLP
- [ ] Preprocessor written in MLP
- [ ] Self-hosting achieved!

---

*Last Updated: November 20, 2025*
*Status: Phase 1 in progress*
