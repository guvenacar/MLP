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

**2. Structs - ✅ FULLY WORKING!**
```mlp
struct Person then
    string name;
    int age;
end

Person p;  -- ✅ Works!
p.age = 25  -- ✅ Works perfectly!
p.name = "Alice"  -- ✅ Works perfectly!
print p.age  -- ✅ Works perfectly!
```

- **Definition:** ✅ `struct Name then ... end`
- **Fields:** ✅ Multiple typed fields with offset calculation
- **Instance Creation:** ✅ `StructName varName;` with stack allocation
- **Field Access:** ✅ Proper offset calculation and memory load
- **Field Assignment:** ✅ Proper offset calculation and memory store
- **Metadata System:** ✅ Global struct metadata tracking
- **Test Files:** `test_struct_instance.mlp`, `test_struct_complete.mlp`, `test_struct_comprehensive.mlp`

---

## ❌ What's Missing

### 1. Dynamic Lists (MEDIUM PRIORITY)

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

### Phase 2: Dynamic Lists ✅ COMPLETE
**Priority:** 🟡 HIGH
**Status:** ✅ COMPLETE (November 20, 2025)

**Completed Tasks:**
1. ✅ Designed list API (add, get, remove, size, clear)
2. ✅ Implemented AST nodes for list operations
3. ✅ Added list runtime functions (196 lines C code)
4. ✅ Implemented parser support (121 lines)
5. ✅ Implemented code generator (120 lines)
6. ✅ Wrote comprehensive tests (2 test files)
7. ✅ Documented list usage (LIST_DESIGN.md)

**Metrics:**
- Runtime: 196 lines
- Parser: 121 lines
- Code Generator: 120 lines
- Tests: 2 files passing
- Total: 555+ lines

**Deliverable:** ✅ Dynamic, resizable lists working and tested

### Phase 3: File I/O & Self-Hosting 🔄 IN PROGRESS
**Priority:** 🔴 CRITICAL
**Status:** 🔄 Implementation ongoing (November 20, 2025)

**Goals:**
- File operations (read/write) for source code processing
- String manipulation (split/join/replace) for text processing
- Hash maps for symbol tables and keyword translation
- Full MLP compiler capability

**Components:**

**3.1 File I/O** 📋 Designed
- `read_file(path)` - Read source files
- `write_file(path, content)` - Write assembly output
- `read_lines(path)` - Line-by-line processing
- `append_file(path, content)` - Logging
- `file_exists(path)` - Pre-flight checks
- `file_size(path)` - Size validation
- Documentation: FILE_IO_DESIGN.md

**3.2 String Operations** 📋 Designed
- `string_split(str, delim)` - Tokenization
- `string_join(parts, delim)` - Code assembly
- `string_replace(str, old, new)` - Keyword translation
- `string_trim(str)` - Whitespace cleanup
- `string_upper(str)` / `string_lower(str)` - Case normalization
- `string_find(haystack, needle)` - Pattern matching
- `string_starts_with(str, prefix)` - Token classification
- `string_ends_with(str, suffix)` - Extension checking
- Documentation: STRING_OPS_DESIGN.md

**3.3 Hash Maps/Dictionaries** 📋 Designed
- `map()` - Create hash map
- `map.set(key, value)` - Insert/update
- `map.get(key)` - Retrieve
- `map.has(key)` - Check existence
- `map.remove(key)` - Delete entry
- `map.size()` - Get count
- `map.clear()` - Empty map
- Implementation: DJB2 hash + chaining
- Documentation: HASHMAP_DESIGN.md

**Timeline:** 2-3 weeks
**Deliverable:** Self-hosting compiler can read `.mlp` and write `.asm`

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

### ✅ Phase 1 COMPLETED (commits c222693, cd9dd70):
1. ✅ Test arrays (DONE - working!)
2. ✅ Test struct definition (DONE - working!)
3. ✅ Implement `peekNextToken()` for token lookahead
4. ✅ Implement struct instance creation (DONE!)
5. ✅ Implement field offset calculations in code generator
6. ✅ Add proper struct memory allocation
7. ✅ Complete field access code generation
8. ✅ Complete field assignment code generation
9. ✅ Test full struct operations end-to-end
10. ✅ Create comprehensive tests

### 🎯 Phase 2 Next (Lists):
11. Design dynamic list API and syntax
12. Implement list AST nodes
13. Add list runtime functions (malloc/realloc/free)
14. Implement list operations (add, get, size, clear)
15. Test list functionality

### This Week:
16. Begin Phase 2 implementation
17. Update SESSION_SUMMARY.md
18. Document struct implementation details

### Later:
19. Phase 3: Dictionary implementation
20. Phase 4: Classes and OOP
21. Self-hosting with JSON parser rewrite

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
- **Lexer:** `c_compiler/c_lexer.c`, `c_compiler/c_lexer.h`
- **Parser:** `c_compiler/c_parser.c` (struct instance: line 502)
- **Generator:** `c_compiler/c_generator.c`
- **Tests:** `test_array.mlp`, `test_struct_instance.mlp`, `test_struct_complete.mlp`
- **Session Summary:** `SESSION_SUMMARY.md`
- **Recent Commits:** `c222693` (struct instance creation)

---

## ✅ Success Criteria

### Phase 1: COMPLETE! ✅
- [x] Struct definition works ✅
- [x] Struct instances can be created ✅ (commit c222693)
- [x] Struct fields can be accessed ✅ (commit cd9dd70)
- [x] Struct fields can be assigned ✅ (commit cd9dd70)
- [x] Metadata system implemented ✅
- [x] Proper offset calculations ✅
- [x] Stack memory allocation ✅
- [x] All tests pass ✅
- [x] Documentation updated ✅

### Project Complete When:
- [ ] Arrays ✅
- [ ] Structs ✅
- [ ] Lists ✅
- [ ] Dictionaries ✅
- [ ] JSON parser written in MLP
- [ ] Preprocessor written in MLP
- [ ] Self-hosting achieved!

---

*Last Updated: November 20, 2025 (Session 2)*
*Status: Phase 1 - ✅ COMPLETE! Structs fully working!*
*Latest Commits: c222693 (instance creation), cd9dd70 (field operations)*
*Next: Phase 2 - Dynamic Lists*
