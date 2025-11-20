# Hash Map Compiler Integration Design

**Phase:** 4 - Hash Maps & Advanced Features
**Date:** November 20, 2025
**Status:** Implementation Ready
**Priority:** 🔴 CRITICAL (Symbol tables for self-hosting)

---

## 📋 Overview

This document describes the **compiler integration** of hash maps in MLP. For the runtime design and general usage, see [HASHMAP_DESIGN.md](HASHMAP_DESIGN.md).

Hash maps provide O(1) average-case key-value storage, essential for:
- **Symbol tables** (variable name → type/location)
- **Keyword translation** (multi-language support)
- **Configuration storage** (compiler options)
- **String interning** (memory efficiency)

---

## 🎯 Syntax

### Declaration

```mlp
map[KeyType:ValueType] variable_name = map();
```

**Supported Type Combinations:**
```mlp
map[string:int] ages = map();      -- String keys, int values
map[int:string] names = map();     -- Int keys, string values
map[string:string] config = map(); -- String keys, string values
map[int:int] lookup = map();       -- Int keys, int values
```

### Usage Examples

```mlp
-- Create map
map[string:int] ages = map();

-- Set values
ages.set("Alice", 25);
ages.set("Bob", 30);
ages.set("Charlie", 35);

-- Get values
int alice_age = ages.get("Alice");  -- Returns 25
int david_age = ages.get("David");  -- Returns 0 (not found)

-- Check existence
int has_bob = ages.has("Bob");      -- Returns 1
int has_eve = ages.has("Eve");      -- Returns 0

-- Remove entry
ages.remove("Alice");

-- Get size
int count = ages.size();  -- Returns 2 (Bob + Charlie)

-- Clear all
ages.clear();
print ages.size();  -- Returns 0
```

---

## 🔧 Compiler Components

### 1. Lexer Changes

**New Tokens:**

```c
// In c_lexer.h
typedef enum {
    // ... existing tokens ...
    TOKEN_YAPI_MAP,        // "map" keyword
    TOKEN_COLON,           // ":" for key:value syntax
    TOKEN_DOT,             // "." for method calls (already exists)
    TOKEN_LBRACKET,        // "[" (already exists)
    TOKEN_RBRACKET,        // "]" (already exists)
} TokenType;
```

**Keyword Addition:**

```c
// In c_lexer.c
static struct {
    const char* keyword;
    TokenType type;
} keywords[] = {
    // ... existing keywords ...
    {"map", TOKEN_YAPI_MAP},
};
```

**Tokenization Examples:**

```
Input:  map[string:int]
Tokens: TOKEN_YAPI_MAP, TOKEN_LBRACKET, TOKEN_IDENTIFIER("string"),
        TOKEN_COLON, TOKEN_IDENTIFIER("int"), TOKEN_RBRACKET

Input:  ages.set("Alice", 25)
Tokens: TOKEN_IDENTIFIER("ages"), TOKEN_DOT, TOKEN_IDENTIFIER("set"),
        TOKEN_LPAREN, TOKEN_STRING("Alice"), TOKEN_COMMA,
        TOKEN_NUMBER(25), TOKEN_RPAREN
```

---

### 2. Parser Changes

#### 2.1 Map Declaration Parsing

**Grammar:**
```
map_declaration := 'map' '[' type_name ':' type_name ']' identifier '=' 'map' '(' ')'
```

**Parser Function:**

```c
// In c_parser.c
ASTNode* parse_map_declaration() {
    expect(TOKEN_YAPI_MAP);  // "map"
    expect(TOKEN_LBRACKET);  // "["

    // Parse key type
    Token* key_type_token = expect(TOKEN_IDENTIFIER);
    char* key_type = strdup(key_type_token->value);

    expect(TOKEN_COLON);  // ":"

    // Parse value type
    Token* value_type_token = expect(TOKEN_IDENTIFIER);
    char* value_type = strdup(value_type_token->value);

    expect(TOKEN_RBRACKET);  // "]"

    // Parse variable name
    Token* var_name_token = expect(TOKEN_IDENTIFIER);
    char* var_name = strdup(var_name_token->value);

    expect(TOKEN_ASSIGN);     // "="
    expect(TOKEN_YAPI_MAP);   // "map"
    expect(TOKEN_LPAREN);     // "("
    expect(TOKEN_RPAREN);     // ")"

    // Create AST node
    ASTNode* node = create_ast_node(AST_MAP_TANIMLAMA);
    node->map_decl.key_type = key_type;
    node->map_decl.value_type = value_type;
    node->map_decl.var_name = var_name;

    return node;
}
```

#### 2.2 Method Call Parsing

**Grammar:**
```
method_call := identifier '.' method_name '(' arguments ')'
```

**Parser Function:**

```c
// In c_parser.c
ASTNode* parse_method_call(char* object_name) {
    expect(TOKEN_DOT);  // "."

    Token* method_token = expect(TOKEN_IDENTIFIER);
    char* method_name = strdup(method_token->value);

    expect(TOKEN_LPAREN);  // "("

    // Parse arguments
    list[ASTNode*] args = parse_argument_list();

    expect(TOKEN_RPAREN);  // ")"

    // Determine method type
    ASTNodeType node_type;
    if (strcmp(method_name, "set") == 0) {
        node_type = AST_MAP_SET;
    } else if (strcmp(method_name, "get") == 0) {
        node_type = AST_MAP_GET;
    } else if (strcmp(method_name, "has") == 0) {
        node_type = AST_MAP_HAS;
    } else if (strcmp(method_name, "remove") == 0) {
        node_type = AST_MAP_REMOVE;
    } else if (strcmp(method_name, "size") == 0) {
        node_type = AST_MAP_SIZE;
    } else if (strcmp(method_name, "clear") == 0) {
        node_type = AST_MAP_CLEAR;
    }

    // Create AST node
    ASTNode* node = create_ast_node(node_type);
    node->method_call.object = object_name;
    node->method_call.method = method_name;
    node->method_call.args = args;

    return node;
}
```

---

### 3. AST Nodes

**New Node Types:**

```c
// In c_ast.h
typedef enum {
    // ... existing types ...
    AST_MAP_TANIMLAMA,   // Map declaration
    AST_MAP_SET,         // map.set(key, value)
    AST_MAP_GET,         // map.get(key)
    AST_MAP_HAS,         // map.has(key)
    AST_MAP_REMOVE,      // map.remove(key)
    AST_MAP_SIZE,        // map.size()
    AST_MAP_CLEAR,       // map.clear()
} ASTNodeType;
```

**Node Structures:**

```c
// Map declaration node
typedef struct {
    char* key_type;      // "string", "int"
    char* value_type;    // "string", "int"
    char* var_name;      // Variable identifier
} ASTMapDecl;

// Method call node
typedef struct {
    char* object;        // Map variable name
    char* method;        // "set", "get", etc.
    ASTNode** args;      // Arguments
    int arg_count;       // Number of arguments
} ASTMethodCall;

// Union in ASTNode
typedef struct ASTNode {
    ASTNodeType type;
    union {
        // ... existing fields ...
        ASTMapDecl map_decl;
        ASTMethodCall method_call;
    };
} ASTNode;
```

---

### 4. Code Generation

#### 4.1 Map Declaration

**AST:** `map[string:int] ages = map();`

**Generated Assembly:**

```asm
; Allocate space for map pointer
section .bss
    ages resq 1

section .text
    ; Call hashmap_create()
    call hashmap_create
    mov [ages], rax     ; Store map pointer
```

**Code Generator Function:**

```c
void visit_map_tanimlama(ASTNode* node, CodeGenContext* ctx) {
    char* var_name = node->map_decl.var_name;

    // Emit variable declaration in .bss
    emit_bss(ctx, "%s resq 1\n", var_name);

    // Emit initialization code
    emit_code(ctx, "    ; map[%s:%s] %s = map()\n",
              node->map_decl.key_type,
              node->map_decl.value_type,
              var_name);
    emit_code(ctx, "    call hashmap_create\n");
    emit_code(ctx, "    mov [%s], rax\n", var_name);
}
```

#### 4.2 Map Set

**AST:** `ages.set("Alice", 25);`

**Generated Assembly:**

```asm
    ; ages.set("Alice", 25)
    mov rdi, [ages]          ; First arg: map pointer
    lea rsi, [rel str_Alice] ; Second arg: key pointer
    mov rdx, 25              ; Third arg: value
    call hashmap_set
```

**Code Generator Function:**

```c
void visit_map_set(ASTNode* node, CodeGenContext* ctx) {
    char* object = node->method_call.object;
    ASTNode* key_arg = node->method_call.args[0];
    ASTNode* value_arg = node->method_call.args[1];

    // Load map pointer into RDI
    emit_code(ctx, "    ; %s.set(...)\n", object);
    emit_code(ctx, "    mov rdi, [%s]\n", object);

    // Load key into RSI
    visit(key_arg, ctx);  // Result in RAX
    emit_code(ctx, "    mov rsi, rax\n");

    // Load value into RDX
    visit(value_arg, ctx);  // Result in RAX
    emit_code(ctx, "    mov rdx, rax\n");

    // Call runtime function
    emit_code(ctx, "    call hashmap_set\n");
}
```

#### 4.3 Map Get

**AST:** `int age = ages.get("Alice");`

**Generated Assembly:**

```asm
    ; ages.get("Alice")
    mov rdi, [ages]          ; First arg: map pointer
    lea rsi, [rel str_Alice] ; Second arg: key pointer
    call hashmap_get         ; Result in RAX
    mov [age], rax           ; Store result
```

**Code Generator Function:**

```c
void visit_map_get(ASTNode* node, CodeGenContext* ctx) {
    char* object = node->method_call.object;
    ASTNode* key_arg = node->method_call.args[0];

    // Load map pointer into RDI
    emit_code(ctx, "    ; %s.get(...)\n", object);
    emit_code(ctx, "    mov rdi, [%s]\n", object);

    // Load key into RSI
    visit(key_arg, ctx);  // Result in RAX
    emit_code(ctx, "    mov rsi, rax\n");

    // Call runtime function
    emit_code(ctx, "    call hashmap_get\n");
    // Result in RAX
}
```

#### 4.4 Map Has

**AST:** `int exists = ages.has("Bob");`

**Generated Assembly:**

```asm
    ; ages.has("Bob")
    mov rdi, [ages]        ; First arg: map pointer
    lea rsi, [rel str_Bob] ; Second arg: key pointer
    call hashmap_has       ; Result in RAX (1 or 0)
    mov [exists], rax
```

#### 4.5 Map Remove

**AST:** `ages.remove("Charlie");`

**Generated Assembly:**

```asm
    ; ages.remove("Charlie")
    mov rdi, [ages]            ; First arg: map pointer
    lea rsi, [rel str_Charlie] ; Second arg: key pointer
    call hashmap_remove
```

#### 4.6 Map Size

**AST:** `int count = ages.size();`

**Generated Assembly:**

```asm
    ; ages.size()
    mov rdi, [ages]      ; First arg: map pointer
    call hashmap_size    ; Result in RAX
    mov [count], rax
```

#### 4.7 Map Clear

**AST:** `ages.clear();`

**Generated Assembly:**

```asm
    ; ages.clear()
    mov rdi, [ages]    ; First arg: map pointer
    call hashmap_clear
```

---

## 🔗 Runtime Integration

### Runtime Functions (C)

```c
// In runtime/hashmap.c

// Create new hash map
void* hashmap_create();

// Set key-value pair
void hashmap_set(void* map, void* key, void* value);

// Get value for key (returns NULL/0 if not found)
void* hashmap_get(void* map, void* key);

// Check if key exists (returns 1 or 0)
int hashmap_has(void* map, void* key);

// Remove key-value pair
void hashmap_remove(void* map, void* key);

// Get number of entries
int hashmap_size(void* map);

// Clear all entries
void hashmap_clear(void* map);

// Destroy map (free memory)
void hashmap_destroy(void* map);
```

### Linking

```bash
# Compilation
gcc -c runtime/hashmap.c -o runtime/hashmap.o

# Linking with program
gcc program.o runtime/hashmap.o runtime/runtime.o -o program
```

---

## 💾 Memory Management

### Allocation

```mlp
map[string:int] ages = map();  -- Heap allocation via hashmap_create()
```

**Memory Layout:**
- HashMap struct: ~32 bytes
- Buckets array: capacity × 8 bytes (initial: 16 × 8 = 128 bytes)
- Entries: per entry ~40 bytes (key + value + pointers)

### Cleanup

**Manual (Current):**
```mlp
hashmap_destroy(ages);  -- Free all memory
```

**Future (Phase 5+):**
- Reference counting
- Automatic cleanup on scope exit
- Garbage collection

---

## ⚙️ Type Safety

### Compile-Time Checking

Parser validates:
- Key type is `int` or `string`
- Value type is `int` or `string`
- Method arguments match declaration

**Example:**
```mlp
map[string:int] ages = map();
ages.set("Alice", 25);  -- ✅ OK
ages.set(123, 25);      -- ❌ Parse error: key must be string
ages.set("Bob", "30");  -- ❌ Parse error: value must be int
```

### Runtime Behavior

- Keys stored as `void*`
- Values stored as `void*`
- Type information not preserved at runtime
- Caller responsible for type correctness

---

## 🎯 Use Cases

### 1. Symbol Table (Compiler)

```mlp
map[string:string] symbol_types = map();

function define_variable(name, type) then
    if symbol_types.has(name) == 1 then
        print "Error: Variable already defined";
        return -1;
    end

    symbol_types.set(name, type);
    return 0;
end

function lookup_type(name) then
    if symbol_types.has(name) == 0 then
        print "Error: Undefined variable";
        return "";
    end

    return symbol_types.get(name);
end
```

### 2. Keyword Translation

```mlp
map[string:string] keywords = map();

function init_keywords() then
    keywords.set("EĞER", "if");
    keywords.set("İSE", "then");
    keywords.set("DEĞİLSE", "else");
    keywords.set("DÖNGÜ", "while");
    keywords.set("SON", "end");
    return 0;
end

function translate(turkish) then
    if keywords.has(turkish) == 1 then
        return keywords.get(turkish);
    end
    return turkish;  -- Not a keyword
end
```

### 3. Configuration Storage

```mlp
map[string:string] config = map();

function load_config() then
    config.set("output_dir", "/tmp/build");
    config.set("opt_level", "2");
    config.set("target", "x86_64");
    return 0;
end

function get_config(key, default_value) then
    if config.has(key) == 1 then
        return config.get(key);
    end
    return default_value;
end
```

---

## ⚠️ Limitations (Phase 4)

### Current Limitations

1. **No nested maps**
   ```mlp
   map[string:map[string:int]] nested = map();  -- ❌ Not supported yet
   ```

2. **No iteration**
   ```mlp
   -- No for-each over keys/values yet
   -- Use .keys() method (Phase 4.5)
   ```

3. **Limited key types**
   - Only `int` and `string`
   - No custom struct keys

4. **No custom hash functions**
   - Fixed DJB2 hash for strings
   - Fixed identity hash for ints

5. **Manual memory management**
   - Must call `hashmap_destroy()`
   - No automatic cleanup

### Future Phases

**Phase 4.5:**
- `.keys()` and `.values()` methods
- Iteration support

**Phase 5:**
- Nested maps
- Custom hash functions
- Automatic memory management
- More key/value types

---

## 🧪 Testing

### Test 1: Basic Operations

```mlp
map[string:int] test = map();

test.set("a", 1);
test.set("b", 2);
test.set("c", 3);

int val_a = test.get("a");  -- Should be 1
int val_missing = test.get("d");  -- Should be 0

int has_b = test.has("b");  -- Should be 1
int has_e = test.has("e");  -- Should be 0

int size = test.size();  -- Should be 3

test.remove("b");
int size2 = test.size();  -- Should be 2

test.clear();
int size3 = test.size();  -- Should be 0
```

### Test 2: String-String Map

```mlp
map[string:string] dict = map();

dict.set("hello", "merhaba");
dict.set("world", "dünya");
dict.set("goodbye", "hoşçakal");

string translation = dict.get("hello");  -- "merhaba"
```

### Test 3: Int-String Map

```mlp
map[int:string] names = map();

names.set(101, "Alice");
names.set(102, "Bob");
names.set(103, "Charlie");

string name = names.get(102);  -- "Bob"
```

---

## 📚 Related Documentation

- [HASHMAP_DESIGN.md](HASHMAP_DESIGN.md) - Runtime design and algorithms
- [TYPE_CONVERSIONS_DESIGN.md](TYPE_CONVERSIONS_DESIGN.md) - Type conversion functions
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API
- [examples/hashmap_examples.mlp](examples/hashmap_examples.mlp) - Usage examples

---

## ✅ Implementation Checklist

### Lexer
- [x] Add TOKEN_YAPI_MAP
- [x] Add TOKEN_COLON
- [x] Update keyword table

### Parser
- [x] Parse `map[type:type]` declarations
- [x] Parse `map()` initialization
- [x] Parse method calls (`.set()`, `.get()`, etc.)
- [x] Type validation

### AST
- [x] AST_MAP_TANIMLAMA node
- [x] AST_MAP_SET/GET/HAS/REMOVE/SIZE/CLEAR nodes
- [x] ASTMapDecl structure
- [x] ASTMethodCall structure

### Code Generator
- [x] Emit map variable declarations
- [x] Emit hashmap_create calls
- [x] Emit method call code
- [x] Proper register allocation (RDI, RSI, RDX)

### Runtime
- [x] hashmap_create/set/get/has/remove/size/clear functions
- [x] DJB2 hash function
- [x] Collision handling (chaining)
- [x] Dynamic resizing

### Testing
- [x] Unit tests (C)
- [x] Integration tests (MLP)
- [x] Example programs

---

**Last Updated:** November 20, 2025
**Status:** ✅ Design Complete - Implementation Ready
**Priority:** 🔴 CRITICAL
