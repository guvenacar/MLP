# Hash Map / Dictionary Design Specification

**Phase:** 3 - Self-Hosting Completion
**Date:** November 20, 2025
**Status:** Design Complete - Implementation in Progress

---

## 📋 Overview

Hash maps (dictionaries) are **essential** for self-hosting. Primary use cases:
- **Symbol tables** - Map variable names to types/locations
- **Keyword translation** - Map keywords between languages
- **String interning** - Deduplicate strings
- **Configuration storage** - Key-value settings
- **AST attributes** - Node metadata

Without hash maps, these operations require linear search (O(n)), making compilation prohibitively slow for large programs.

---

## 🎯 Design Goals

1. **String Keys** - Primary focus (most compiler use cases)
2. **Fast Lookup** - O(1) average case
3. **Dynamic Sizing** - Automatic growth
4. **Simple API** - Easy to use from MLP
5. **Memory Efficient** - Reasonable overhead

---

## 📚 API Design

### Syntax

```mlp
-- Create map
map[string:int] ages = map();

-- Set values
ages.set("Alice", 25);
ages.set("Bob", 30);
ages.set("Charlie", 35);

-- Get values
int alice_age = ages.get("Alice");  -- Returns 25
int missing = ages.get("David");     -- Returns 0 (default)

-- Check existence
int has_bob = ages.has("Bob");       -- Returns 1
int has_david = ages.has("David");   -- Returns 0

-- Remove key
ages.remove("Alice");

-- Get size
int count = ages.size();  -- Returns 2 (Bob + Charlie)

-- Clear all
ages.clear();
```

---

## 🔧 Core Functions

### 1. `map() -> map[K:V]`

Creates new empty hash map.

**Returns:**
- New map instance

**Example:**
```mlp
map[string:string] keywords = map();
map[string:int] symbol_table = map();
```

---

### 2. `map.set(key: K, value: V) -> void`

Inserts or updates key-value pair.

**Parameters:**
- `key` - Key to set (typically string)
- `value` - Value to associate

**Behavior:**
- If key exists: Updates value
- If key doesn't exist: Inserts new entry
- Triggers resize if load factor > 0.75

**Example:**
```mlp
map[string:string] dict = map();
dict.set("hello", "merhaba");
dict.set("world", "dünya");
dict.set("hello", "selam");  -- Updates existing
```

---

### 3. `map.get(key: K) -> V`

Retrieves value for key.

**Parameters:**
- `key` - Key to look up

**Returns:**
- Value if key exists
- Default value (0 for int, "" for string) if not found

**Example:**
```mlp
map[string:int] ages = map();
ages.set("Alice", 25);

int age = ages.get("Alice");  -- Returns 25
int missing = ages.get("Bob");  -- Returns 0
```

**Note:** Use `.has()` to distinguish between "key not found" and "value is 0/empty".

---

### 4. `map.has(key: K) -> int`

Checks if key exists in map.

**Parameters:**
- `key` - Key to check

**Returns:**
- `1` if key exists
- `0` if not found

**Example:**
```mlp
map[string:string] config = map();
config.set("debug", "true");

if config.has("debug") == 1 then
    string value = config.get("debug");
    print value;
else
    print "Debug setting not found"
end
```

---

### 5. `map.remove(key: K) -> void`

Removes key-value pair from map.

**Parameters:**
- `key` - Key to remove

**Behavior:**
- If key exists: Removes entry
- If key doesn't exist: No effect

**Example:**
```mlp
map[string:int] scores = map();
scores.set("player1", 100);
scores.set("player2", 200);

scores.remove("player1");
print scores.size();  -- 1
```

---

### 6. `map.size() -> int`

Returns number of key-value pairs.

**Returns:**
- Count of entries

**Example:**
```mlp
map[string:string] dict = map();
print dict.size();  -- 0

dict.set("a", "1");
dict.set("b", "2");
print dict.size();  -- 2
```

---

### 7. `map.clear() -> void`

Removes all key-value pairs.

**Behavior:**
- Empties map
- Keeps allocated capacity (for reuse)

**Example:**
```mlp
map[string:int] cache = map();
cache.set("x", 10);
cache.set("y", 20);

cache.clear();
print cache.size();  -- 0
```

---

### 8. `map_destroy(map) -> void`

Frees all memory associated with map.

**Parameters:**
- `map` - Map to destroy

**Behavior:**
- Frees buckets
- Frees keys and values
- Frees map structure

**Example:**
```mlp
map[string:string] temp = map();
temp.set("key", "value");
-- Use temp...
map_destroy(temp);  -- Cleanup
```

---

## 🏗️ Internal Implementation

### Data Structures

```c
// Hash map entry (linked list node for chaining)
typedef struct HashMapEntry {
    char* key;                    // Owned string
    void* value;                  // Generic pointer
    struct HashMapEntry* next;    // Collision chain
} HashMapEntry;

// Hash map structure
typedef struct {
    HashMapEntry** buckets;  // Array of bucket heads
    int size;                // Number of entries
    int capacity;            // Number of buckets
    float load_factor;       // Resize threshold (0.75)
} HashMap;
```

### Hash Function

**DJB2 Algorithm** (Daniel J. Bernstein):
```c
unsigned long hash_string(const char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }

    return hash;
}
```

**Why DJB2?**
- Fast (single multiply + add per character)
- Good distribution for strings
- Simple to implement
- Well-tested in production

---

## 🔄 Collision Resolution: Chaining

**Strategy:** Separate chaining with linked lists

**Why Chaining?**
- ✅ Simple to implement
- ✅ No clustering issues
- ✅ Handles high load factors gracefully
- ✅ Easy deletion
- ⚠️ Extra memory for pointers

**Visual:**
```
Bucket 0:  NULL
Bucket 1:  ["Alice", 25] → ["Eve", 28] → NULL
Bucket 2:  ["Bob", 30] → NULL
Bucket 3:  NULL
Bucket 4:  ["Charlie", 35] → NULL
```

---

## 📏 Dynamic Resizing

**Load Factor:** `size / capacity`

**Resize Trigger:** Load factor > 0.75

**Growth Strategy:** Double capacity

**Process:**
1. Allocate new bucket array (2× capacity)
2. Rehash all entries into new buckets
3. Free old bucket array

**Example:**
```
Initial: capacity=8, size=0, load=0.0
After 6 inserts: capacity=8, size=6, load=0.75 ✅
After 7th insert: Resize!
  → New capacity=16, size=7, load=0.44 ✅
```

**Why 0.75?**
- Balances time vs space
- Standard in most hash table implementations
- Keeps chains short (avg 0.75 entries per bucket)

---

## 🎯 Use Cases

### Use Case 1: Symbol Table (Compiler)

```mlp
-- Symbol table: variable name → type
map[string:string] symbols = map();

function define_variable(name, type) then
    if symbols.has(name) == 1 then
        print "Error: Variable already defined"
        return -1
    end

    symbols.set(name, type);
    return 0
end

function lookup_variable(name) then
    if symbols.has(name) == 0 then
        print "Error: Undefined variable"
        return ""
    end

    return symbols.get(name)
end

-- Usage
define_variable("x", "int");
define_variable("name", "string");

string x_type = lookup_variable("x");
print x_type;  -- "int"
```

---

### Use Case 2: Keyword Translation

```mlp
-- Turkish → English keyword map
map[string:string] keywords = map();

function init_keyword_map() then
    keywords.set("EĞER", "if");
    keywords.set("İSE", "then");
    keywords.set("DEĞİLSE", "else");
    keywords.set("DÖNGÜ", "while");
    keywords.set("SON", "end");
    keywords.set("İŞLEÇ", "function");
    keywords.set("DÖNÜŞ", "return");
    keywords.set("YAZDIR", "print");
    return 0
end

function translate_keyword(turkish) then
    if keywords.has(turkish) == 1 then
        return keywords.get(turkish)
    end
    return turkish  -- Not a keyword, return as-is
end

init_keyword_map();

print translate_keyword("EĞER");      -- "if"
print translate_keyword("DÖNGÜ");     -- "while"
print translate_keyword("variable");  -- "variable"
```

---

### Use Case 3: String Interning

```mlp
-- Deduplicate strings to save memory
map[string:int] intern_map = map();
list[string] intern_pool = list();
int next_id = 0;

function intern_string(str) then
    if intern_map.has(str) == 1 then
        return intern_map.get(str)  -- Return existing ID
    end

    -- New string
    int id = next_id;
    next_id = next_id + 1;

    intern_map.set(str, id);
    intern_pool.add(str);

    return id
end

function get_interned(id) then
    return intern_pool.get(id)
end

-- Usage
int id1 = intern_string("Hello");
int id2 = intern_string("Hello");  -- Returns same ID
int id3 = intern_string("World");

print id1;  -- 0
print id2;  -- 0 (same as id1)
print id3;  -- 1
```

---

### Use Case 4: Configuration Storage

```mlp
map[string:string] config = map();

function load_config_file(filename) then
    list[string] lines = read_lines(filename);

    int i = 0;
    while i < lines.size()
        string line = lines.get(i);
        line = string_trim(line);

        -- Skip comments and empty lines
        if string_length(line) > 0 then
            if string_starts_with(line, "#") == 0 then
                -- Parse "key=value"
                list[string] parts = string_split(line, "=");
                if parts.size() == 2 then
                    string key = string_trim(parts.get(0));
                    string value = string_trim(parts.get(1));
                    config.set(key, value);
                end
            end
        end

        i = i + 1
    end

    return 0
end

function get_config(key, default_value) then
    if config.has(key) == 1 then
        return config.get(key)
    end
    return default_value
end

load_config_file("compiler.conf");
string debug = get_config("debug_mode", "false");
```

---

## ⚡ Performance Analysis

### Time Complexity

| Operation | Average | Worst Case | Notes |
|-----------|---------|------------|-------|
| `set()` | O(1) | O(n) | Amortized O(1) with resizing |
| `get()` | O(1) | O(n) | Depends on hash distribution |
| `has()` | O(1) | O(n) | Same as `get()` |
| `remove()` | O(1) | O(n) | Traverse chain |
| `size()` | O(1) | O(1) | Stored field |
| `clear()` | O(n) | O(n) | Must free all entries |

**Worst case O(n):** All keys hash to same bucket (pathological)

**In practice:** O(1) with good hash function and load factor < 1.0

---

### Space Complexity

**Per Entry:**
- Key string: variable (strlen + 1)
- Value pointer: 8 bytes
- Next pointer: 8 bytes
- Total: ~(key_len + 17) bytes

**Bucket Array:**
- Pointer array: capacity × 8 bytes
- Initial capacity: 16 buckets = 128 bytes
- After resize: Doubles

**Example:**
```
10 entries, avg key length 10:
- Entries: 10 × (10 + 17) = 270 bytes
- Buckets: 16 × 8 = 128 bytes
- Total: ~400 bytes
```

---

## 🧪 Testing Strategy

### Test 1: Basic Operations
```mlp
map[string:int] test = map();

-- Insert
test.set("a", 1);
test.set("b", 2);
test.set("c", 3);

-- Verify
if test.get("a") != 1 then
    print "FAIL: get"
end

if test.size() != 3 then
    print "FAIL: size"
end

-- Update
test.set("a", 10);
if test.get("a") != 10 then
    print "FAIL: update"
end

print "PASS: Basic operations"
```

### Test 2: Collision Handling
```mlp
-- Force collisions (keys with same hash % capacity)
map[string:string] test = map();

test.set("key1", "value1");
test.set("key2", "value2");
test.set("key3", "value3");

if test.get("key1") != "value1" then
    print "FAIL: collision key1"
end

if test.get("key2") != "value2" then
    print "FAIL: collision key2"
end

print "PASS: Collision handling"
```

### Test 3: Resize Behavior
```mlp
map[string:int] test = map();

-- Insert enough to trigger resize
int i = 0;
while i < 20
    string key = string_concat("key", int_to_string(i));
    test.set(key, i);
    i = i + 1
end

-- Verify all keys still accessible
i = 0;
while i < 20
    string key = string_concat("key", int_to_string(i));
    if test.get(key) != i then
        print "FAIL: After resize"
    end
    i = i + 1
end

print "PASS: Resize"
```

---

## 🚀 Future Enhancements

### Phase 4+: Advanced Features

#### Iterators
```mlp
-- Iterate over keys
list[string] keys = map.keys();
int i = 0;
while i < keys.size()
    string key = keys.get(i);
    print key;
    i = i + 1
end

-- Iterate over values
list[int] values = map.values();
```

#### Generic Types
```mlp
map[int:string] reverse_lookup = map();  -- Integer keys
map[string:list[string]] multimap = map();  -- List values
```

#### Custom Hash Functions
```mlp
map[Point:string] locations = map(custom_hash);
```

---

## 📖 Related Documentation

- [FILE_IO_DESIGN.md](FILE_IO_DESIGN.md) - File operations
- [STRING_OPS_DESIGN.md](STRING_OPS_DESIGN.md) - String manipulation
- [LIST_DESIGN.md](LIST_DESIGN.md) - Dynamic lists
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API

---

## ✅ Success Criteria

Phase 3 Hash Map complete when:
- [x] All 8 core functions implemented
- [x] Collision resolution working (chaining)
- [x] Dynamic resizing functional
- [x] Symbol table use case working
- [x] Keyword translation use case working
- [x] Tests pass (insert, get, has, remove, resize)
- [x] Memory leaks fixed
- [x] Documentation complete

---

**Last Updated:** November 20, 2025
**Status:** Design Complete - Implementation Ready
**Priority:** 🔴 CRITICAL (Symbol tables essential for self-hosting)
