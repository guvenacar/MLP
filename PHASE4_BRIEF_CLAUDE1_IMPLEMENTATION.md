# Phase 4 - Core Implementation Brief (Claude #1 - Coordinator)

**Session**: Phase 4 - Hash Maps & Advanced Features
**Your Role**: Core Implementation & Coordination
**Branch**: `claude/phase4-implementation-[SESSION_ID]`
**Timeline**: ~5-6 hours

---

## 🎯 Your Mission

Implement Phase 4 features:
1. Hash map compiler integration (`map[KeyType:ValueType]` syntax)
2. Type conversion functions (int_to_string, string_to_int)
3. Math operations (basic math functions)
4. Additional string functions
5. Coordinate with Claude #2 and #3

---

## 📋 Phase 1: Hash Map Compiler Integration

### 1.1 Lexer Updates (`c_compiler/c_lexer.h` + `c_lexer.c`)

Add new token for `map` keyword:

```c
// In c_lexer.h
typedef enum {
    // ... existing tokens ...
    TOKEN_YAPI_MAP,      // map - Phase 4

    // Built-in map methods
    TOKEN_BUILTIN_MAP_SET,
    TOKEN_BUILTIN_MAP_GET,
    TOKEN_BUILTIN_MAP_HAS,
    TOKEN_BUILTIN_MAP_REMOVE,
    TOKEN_BUILTIN_MAP_SIZE,
    TOKEN_BUILTIN_MAP_CLEAR,
    TOKEN_BUILTIN_MAP_KEYS,
    TOKEN_BUILTIN_MAP_VALUES,
} TokenType;
```

Add to keyword table in `c_lexer.c`:
```c
{"map", TOKEN_YAPI_MAP},
```

### 1.2 AST Updates (`c_compiler/c_ast.h`)

Add new AST node types:

```c
typedef enum {
    // ... existing types ...
    AST_MAP_TANIMLAMA,     // Map declaration: map[string:int] ages = map();
    AST_MAP_SET,           // Map set: ages.set("Alice", 25)
    AST_MAP_GET,           // Map get: ages.get("Alice")
    AST_MAP_HAS,           // Map has: ages.has("Alice")
    AST_MAP_REMOVE,        // Map remove: ages.remove("Alice")
    AST_MAP_SIZE,          // Map size: ages.size()
    AST_MAP_CLEAR,         // Map clear: ages.clear()
} ASTNodeType;
```

Add node data structures:

```c
// In ASTNode union
struct {
    Token* key_type;      // Key type (string, int)
    Token* value_type;    // Value type (string, int)
    Token* map_adi;       // Map name
} map_tanimlama_data;

struct {
    Token* map_adi;       // Map name
    ASTNode* key;         // Key expression
    ASTNode* value;       // Value expression (for set)
} map_operation_data;
```

Helper functions:
```c
ASTNode* createAST_MapTanimlama(Token* key_type, Token* value_type, Token* map_adi);
ASTNode* createAST_MapOperation(ASTNodeType type, Token* map_adi, ASTNode* key, ASTNode* value);
```

### 1.3 Parser Updates (`c_compiler/c_parser.c`)

Parse map declarations in `komut()`:

```c
// Map declaration: map[string:int] ages = map();
if (current_token->type == TOKEN_YAPI_MAP) {
    consume(TOKEN_YAPI_MAP);

    // Expect [
    consume(TOKEN_LEFT_BRACKET);

    // Key type
    Token key_type;
    if (current_token->type == TOKEN_TANIMLA_SAYI ||
        current_token->type == TOKEN_TANIMLA_METIN) {
        key_type.type = current_token->type;
        key_type.value = strdup(current_token->value);
        consume(current_token->type);
    } else {
        parseError("Map key type", "int/string");
    }

    // Expect :
    consume(TOKEN_COLON);  // Need to add TOKEN_COLON to lexer!

    // Value type
    Token value_type;
    if (current_token->type == TOKEN_TANIMLA_SAYI ||
        current_token->type == TOKEN_TANIMLA_METIN) {
        value_type.type = current_token->type;
        value_type.value = strdup(current_token->value);
        consume(current_token->type);
    } else {
        parseError("Map value type", "int/string");
    }

    // Expect ]
    consume(TOKEN_RIGHT_BRACKET);

    // Variable name
    Token map_adi;
    map_adi.type = current_token->type;
    map_adi.value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER);

    // Expect = map();
    consume(TOKEN_ASSIGN);
    consume(TOKEN_YAPI_MAP);
    consume(TOKEN_LEFT_PAREN);
    consume(TOKEN_RIGHT_PAREN);
    consume(TOKEN_SEMICOLON);

    return createAST_MapTanimlama(&key_type, &value_type, &map_adi);
}
```

Parse map method calls in `birincil()` (similar to list methods):

```c
// Inside IDENTIFIER case, after checking for lists
if (strcmp(method_or_field.value, "set") == 0 ||
    strcmp(method_or_field.value, "get") == 0 ||
    strcmp(method_or_field.value, "has") == 0 ||
    strcmp(method_or_field.value, "remove") == 0 ||
    strcmp(method_or_field.value, "size") == 0 ||
    strcmp(method_or_field.value, "clear") == 0) {

    // This is a map method call
    // Parse arguments...
    // Return appropriate AST node
}
```

### 1.4 Code Generator Updates (`c_compiler/c_generator.c`)

Implement visitor functions:

```c
void visit_MapTanimlama(ASTNode* node) {
    char* map_adi = node->map_tanimlama_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Declaration: %s ---", map_adi);
    asm_append(&text_section, buffer);

    // Call hashmap_create()
    asm_append(&text_section, "    call hashmap_create");

    // Store HashMap* on stack
    char* stack_offset = kapsam_yeni_degisken_ekle(map_adi, "HashMap*");
    sprintf(buffer, "    mov %s, rax  ; Store HashMap* for %s", stack_offset, map_adi);
    asm_append(&text_section, buffer);
}

void visit_MapSet(ASTNode* node) {
    char* map_adi = node->map_operation_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Set: %s.set() ---", map_adi);
    asm_append(&text_section, buffer);

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    // Evaluate key
    visit(node->map_operation_data.key);
    asm_append(&text_section, "    mov rsi, rax  ; Key to rsi");

    // Evaluate value
    visit(node->map_operation_data.value);
    asm_append(&text_section, "    mov rdx, rax  ; Value to rdx");

    asm_append(&text_section, "    call hashmap_set");
}

void visit_MapGet(ASTNode* node) {
    char* map_adi = node->map_operation_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Get: %s.get() ---", map_adi);
    asm_append(&text_section, buffer);

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    // Evaluate key
    visit(node->map_operation_data.key);
    asm_append(&text_section, "    mov rsi, rax  ; Key to rsi");

    asm_append(&text_section, "    call hashmap_get");
    // Result in RAX
}

// Similar for MapHas, MapRemove, MapSize, MapClear
```

---

## 📋 Phase 2: Type Conversion Functions

### 2.1 Runtime Implementation (`runtime/runtime.c`)

Add to end of file:

```c
// ============================================
// Phase 4: Type Conversions
// ============================================

/**
 * int_to_string - Convert integer to string
 * @param num: Integer to convert
 * @return: String representation
 *
 * MLP Usage: string s = int_to_string(42);
 */
char* int_to_string(long num) {
    char buffer[32];
    sprintf(buffer, "%ld", num);
    return strdup(buffer);
}

/**
 * string_to_int - Convert string to integer
 * @param str: String to convert
 * @return: Integer value, 0 if invalid
 *
 * MLP Usage: int n = string_to_int("42");
 */
long string_to_int(const char* str) {
    if (!str) return 0;
    return atol(str);
}

/**
 * char_to_string - Convert single character to string
 * @param ch: Character code
 * @return: Single character string
 *
 * MLP Usage: string s = char_to_string(65);  // "A"
 */
char* char_to_string(int ch) {
    char* result = malloc(2);
    result[0] = (char)ch;
    result[1] = '\0';
    return result;
}

/**
 * string_concat - Concatenate two strings
 * @param s1: First string
 * @param s2: Second string
 * @return: Concatenated string
 *
 * MLP Usage: string result = string_concat("Hello", " World");
 */
char* string_concat(const char* s1, const char* s2) {
    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char* result = malloc(len1 + len2 + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}
```

### 2.2 Compiler Integration

Add tokens to lexer:
```c
TOKEN_BUILTIN_INT_TO_STRING,
TOKEN_BUILTIN_STRING_TO_INT,
TOKEN_BUILTIN_CHAR_TO_STRING,
TOKEN_BUILTIN_STRING_CONCAT,
```

Add to built-in function handler in parser (reuse AST_BUILTIN_CALL).

---

## 📋 Phase 3: Basic Math Functions

### 3.1 Runtime Implementation

```c
// ============================================
// Phase 4: Math Operations
// ============================================

/**
 * math_abs - Absolute value
 */
long math_abs(long n) {
    return (n < 0) ? -n : n;
}

/**
 * math_min - Minimum of two numbers
 */
long math_min(long a, long b) {
    return (a < b) ? a : b;
}

/**
 * math_max - Maximum of two numbers
 */
long math_max(long a, long b) {
    return (a > b) ? a : b;
}

/**
 * math_pow - Power (a^b) - integer only
 */
long math_pow(long base, long exp) {
    if (exp == 0) return 1;
    long result = 1;
    for (long i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}
```

Add tokens and integrate into compiler.

---

## 📋 Phase 4: Integration Test

Create `test/test_phase4_integration.mlp`:

```mlp
-- Phase 4 Integration Test

function test_hashmap() then
    map[string:int] ages = map();

    -- Set values
    ages.set("Alice", 25);
    ages.set("Bob", 30);
    ages.set("Charlie", 35);

    -- Get values
    int alice_age = ages.get("Alice");
    print "Alice age: ";
    print alice_age;

    -- Check existence
    int has_bob = ages.has("Bob");
    print "Has Bob: ";
    print has_bob;

    -- Size
    int size = ages.size();
    print "Map size: ";
    print size;

    -- Remove
    ages.remove("Charlie");
    size = ages.size();
    print "After remove: ";
    print size;

    return 0
end

function test_type_conversions() then
    -- Int to string
    string s = int_to_string(42);
    print "Int to string: ";
    print s;

    -- String to int
    int n = string_to_int("123");
    print "String to int: ";
    print n;

    -- String concat
    string result = string_concat("Hello", " World");
    print "Concat: ";
    print result;

    return 0
end

function test_math() then
    int a = 10;
    int b = -5;

    int abs_val = math_abs(b);
    print "Abs: ";
    print abs_val;

    int min_val = math_min(a, b);
    print "Min: ";
    print min_val;

    int max_val = math_max(a, b);
    print "Max: ";
    print max_val;

    int pow_val = math_pow(2, 3);
    print "Pow: ";
    print pow_val;

    return 0
end

print "=== Phase 4 Integration Tests ===";
print "";
test_hashmap();
print "";
test_type_conversions();
print "";
test_math();
```

Compile and verify.

---

## 📂 File Structure

Your changes:
```
/home/user/MLP/
├── runtime/
│   └── runtime.c              (MODIFIED: +300 lines)
├── c_compiler/
│   ├── c_lexer.c              (+15 lines)
│   ├── c_lexer.h              (+15 lines)
│   ├── c_parser.c             (+150 lines)
│   ├── c_ast.h                (+40 lines)
│   └── c_generator.c          (+200 lines)
├── test/
│   └── test_phase4_integration.mlp (NEW: ~100 lines)
└── PHASE4_IMPLEMENTATION_SUMMARY.md (NEW)
```

**Total**: ~820 lines

---

## ✅ Success Criteria

1. ✅ Hash map syntax working: `map[string:int] ages = map();`
2. ✅ Map methods: set, get, has, remove, size, clear
3. ✅ Type conversions: int_to_string, string_to_int
4. ✅ Math functions: abs, min, max, pow
5. ✅ String concat function
6. ✅ Integration test compiles
7. ✅ Documentation complete

---

## 🎯 Timeline

**Hour 1-2**: Hash map lexer/parser/AST
**Hour 3**: Hash map code generation
**Hour 4**: Type conversion functions
**Hour 5**: Math functions
**Hour 6**: Integration testing + docs

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase4-implementation-[SESSION_ID]

git add runtime/runtime.c
git commit -m "feat: Add hash map runtime integration"

git add c_compiler/*
git commit -m "feat: Add hash map compiler support"

git push -u origin claude/phase4-implementation-[SESSION_ID]
```

---

## 🎉 When You're Done

Report:
1. Features implemented (X/Y)
2. Tests passing
3. Ready for coordination

**Branch**: `claude/phase4-implementation-[SESSION_ID]`
**Expected**: 8-12 commits

Let's complete Phase 4! 🚀
