# Dynamic List Design - MLP Phase 2

**Date:** November 20, 2025
**Goal:** Add dynamic, growable lists to MLP for self-hosting

---

## 🎯 Design Goals

1. **Dynamic Growth:** Lists automatically expand using realloc
2. **Type Safety:** Generic list[Type] syntax
3. **Simple API:** Minimal but complete operations
4. **Memory Safety:** Proper cleanup and bounds checking
5. **Self-Hosting Ready:** Sufficient for JSON parser and AST building

---

## 📝 Syntax Options

### Option A: Python-style (Recommended)
```mlp
list[int] numbers = list();
numbers.add(10)
numbers.add(20)
int x = numbers.get(0)
int size = numbers.size()
numbers.clear()
```

**Pros:**
- Clean and intuitive
- Familiar to most developers
- Method chaining possible

**Cons:**
- Requires dot notation for methods

---

### Option B: Function-style
```mlp
list[int] numbers = list();
list_add(numbers, 10)
list_add(numbers, 20)
int x = list_get(numbers, 0)
int size = list_size(numbers)
list_clear(numbers)
```

**Pros:**
- No new syntax needed (just functions)
- Easier to implement initially

**Cons:**
- More verbose
- Less OOP-like

---

### Option C: Hybrid (Best Balance)
```mlp
list[int] numbers = list();
numbers.add(10)          -- Method for mutations
numbers.add(20)
int x = numbers[0]       -- Array syntax for access
int size = numbers.size()
```

**Pros:**
- Array-like access with `[]`
- Method-style mutations
- Most flexible

**Cons:**
- Requires both syntax forms

---

## 🏗️ Implementation Plan

### 1. Core Data Structure

In runtime (C):
```c
typedef struct {
    void** data;        // Generic pointer array
    int size;           // Current number of elements
    int capacity;       // Allocated capacity
    int element_size;   // Size of each element (8 bytes for pointers)
} List;
```

### 2. Required Runtime Functions

```c
// runtime.c additions
List* list_create();
void list_add(List* list, void* element);
void* list_get(List* list, int index);
int list_size(List* list);
void list_clear(List* list);
void list_destroy(List* list);
void list_remove(List* list, int index);
```

### 3. AST Nodes Needed

```c
// c_ast.h additions
typedef enum {
    // ... existing nodes ...
    AST_LIST_TANIMLAMA,      // list[int] x = list();
    AST_LIST_ADD,            // x.add(10)
    AST_LIST_GET,            // x.get(0) or x[0]
    AST_LIST_SIZE,           // x.size()
    AST_LIST_CLEAR,          // x.clear()
} ASTNodeType;
```

### 4. Token Additions

```c
// c_lexer.h additions
TOKEN_YAPI_LIST,         // "list" keyword
TOKEN_LEFT_ANGLE,        // '<' for list[int]
TOKEN_RIGHT_ANGLE,       // '>' (or use existing)
```

---

## 🎬 Usage Examples

### Example 1: Simple Integer List
```mlp
list[int] numbers = list();

numbers.add(10)
numbers.add(20)
numbers.add(30)

print numbers.size()  -- Output: 3
print numbers[0]      -- Output: 10
print numbers[1]      -- Output: 20

numbers.clear()
print numbers.size()  -- Output: 0
```

### Example 2: String List
```mlp
list[string] names = list();

names.add("Alice")
names.add("Bob")
names.add("Charlie")

int i = 0
while i < names.size()
    print names[i]
    i = i + 1
end
```

### Example 3: Struct List (JSON Parser Use Case)
```mlp
struct Token then
    string type;
    string value;
end

list[Token] tokens = list();

Token t1;
t1.type = "NUMBER"
t1.value = "42"
tokens.add(t1)

Token t2;
t2.type = "STRING"
t2.value = "hello"
tokens.add(t2)

print tokens.size()  -- Output: 2
print tokens[0].type -- Output: NUMBER
```

---

## 🔧 Implementation Order

### Phase 2.1: Runtime Foundation (Day 1)
1. Add `List` struct to runtime.c
2. Implement `list_create()`, `list_destroy()`
3. Implement `list_add()` with realloc
4. Implement `list_get()` with bounds checking
5. Test with manual C code

### Phase 2.2: Lexer & Parser (Day 2)
1. Add `TOKEN_YAPI_LIST` to lexer
2. Update parser to recognize `list[Type] name = list()`
3. Parse `.add()` method calls
4. Parse `.get()` and `[]` access
5. Parse `.size()`, `.clear()`

### Phase 2.3: AST & Code Generation (Days 3-4)
1. Add list AST nodes
2. Implement `visit_ListTanimlama()`
3. Implement `visit_ListAdd()`
4. Implement `visit_ListGet()`
5. Implement `visit_ListSize()`
6. Generate proper assembly for list ops

### Phase 2.4: Testing & JSON Parser (Days 5-7)
1. Create comprehensive list tests
2. Rewrite JSON lexer to use lists
3. Write JSON parser with AST lists
4. Verify self-hosting readiness

---

## 📊 Memory Management

**Growth Strategy:**
- Initial capacity: 8 elements
- Growth factor: 2x (double when full)
- Realloc pattern: `capacity = capacity * 2`

**Memory Layout:**
```
List struct (24 bytes):
  - data pointer (8 bytes)
  - size (4 bytes)
  - capacity (4 bytes)
  - element_size (8 bytes)

Data array (dynamic):
  - capacity * 8 bytes (pointers)
```

---

## ⚠️ Limitations (v1)

**What we WON'T implement initially:**
- ❌ `list.remove(index)` - Can be added later
- ❌ `list.insert(index, value)` - Can be added later
- ❌ List slicing - Can be added later
- ❌ List comprehensions - Future feature
- ❌ Multi-dimensional lists - Can use `list[list[int]]` later

**What we WILL implement:**
- ✅ `list.add(value)` - Append
- ✅ `list.get(index)` or `list[index]` - Access
- ✅ `list.size()` - Get count
- ✅ `list.clear()` - Empty list (keep capacity)

---

## 🎯 Success Criteria

Phase 2 complete when:
- [ ] Lists can be created and destroyed
- [ ] Elements can be added dynamically
- [ ] Elements can be accessed by index
- [ ] List size can be queried
- [ ] Lists automatically grow with realloc
- [ ] Works with int, string, and struct types
- [ ] JSON parser can use lists for tokens
- [ ] All tests pass

---

## 🚀 Next Steps After Lists

Once lists are done:
1. **Rewrite JSON Parser** using lists
2. **Write Keyword Translator** (use JSON + string replacement)
3. **Write Preprocessor** in MLP
4. **Consider dictionaries** for Phase 3 (optional for self-hosting)

---

*Design v1.0 - Ready for implementation*
