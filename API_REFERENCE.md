# MLP API Reference

**Version:** Phase 3
**Last Updated:** November 20, 2025

---

## Quick Reference

| Category | Functions | Status |
|----------|-----------|--------|
| **File I/O** | 6 functions | Phase 3 🔄 |
| **String Ops** | 15 functions (6 existing + 9 new) | Phase 3 🔄 |
| **Lists** | 7 functions | Phase 2 ✅ |
| **Hash Maps** | 8 functions | Phase 3 🔄 |
| **Structs** | Language feature | Phase 1 ✅ |
| **Arrays** | Language feature | Phase 0 ✅ |

---

## 📁 File Operations (Phase 3)

### `read_file(path: string) -> string`
Read entire file as string.
- **Returns:** File contents or "" on error
- **Example:** `string code = read_file("program.mlp");`

### `write_file(path: string, content: string) -> int`
Write string to file (overwrites).
- **Returns:** 0 on success, -1 on error
- **Example:** `write_file("output.asm", assembly);`

### `read_lines(path: string) -> list[string]`
Read file as list of lines.
- **Returns:** List of lines (newlines stripped)
- **Example:** `list[string] lines = read_lines("config.txt");`

### `append_file(path: string, content: string) -> int`
Append to file.
- **Returns:** 0 on success, -1 on error
- **Example:** `append_file("log.txt", "Entry\n");`

### `file_exists(path: string) -> int`
Check if file exists.
- **Returns:** 1 if exists, 0 otherwise
- **Example:** `if file_exists("input.txt") == 1 then`

### `file_size(path: string) -> int`
Get file size in bytes.
- **Returns:** Size or -1 on error
- **Example:** `int size = file_size("data.bin");`

---

## 🔤 String Operations

### Existing (Phase 0-2) ✅

#### `string_concat(a: string, b: string) -> string`
Concatenate two strings.

#### `string_length(str: string) -> int`
Get string length in bytes.

#### `string_char_at(str: string, index: int) -> string`
Get character at index.

#### `string_substring(str: string, start: int, length: int) -> string`
Extract substring.

#### `char_code(ch: string) -> int`
Get ASCII code of character.

#### `code_to_char(code: int) -> string`
Convert ASCII code to character.

### New (Phase 3) 🔄

#### `string_split(str: string, delim: string) -> list[string]`
Split string by delimiter.
- **Example:** `list[string] parts = string_split("a,b,c", ",");`

#### `string_join(parts: list[string], delim: string) -> string`
Join list of strings.
- **Example:** `string result = string_join(words, " ");`

#### `string_replace(str: string, old: string, new: string) -> string`
Replace all occurrences.
- **Example:** `string english = string_replace(turkish, "EĞER", "if");`

#### `string_trim(str: string) -> string`
Remove leading/trailing whitespace.
- **Example:** `string clean = string_trim("  Hello  ");`

#### `string_upper(str: string) -> string`
Convert to uppercase.
- **Example:** `string upper = string_upper("hello");`

#### `string_lower(str: string) -> string`
Convert to lowercase.
- **Example:** `string lower = string_lower("HELLO");`

#### `string_find(haystack: string, needle: string) -> int`
Find first occurrence.
- **Returns:** Index or -1 if not found
- **Example:** `int pos = string_find("Hello World", "World");`

#### `string_starts_with(str: string, prefix: string) -> int`
Check if starts with prefix.
- **Returns:** 1 or 0
- **Example:** `if string_starts_with(line, "function") == 1 then`

#### `string_ends_with(str: string, suffix: string) -> int`
Check if ends with suffix.
- **Returns:** 1 or 0
- **Example:** `if string_ends_with(file, ".mlp") == 1 then`

---

## 📋 List Operations (Phase 2) ✅

### `list() -> list[T]`
Create new empty list.

### `list.add(value: T) -> void`
Append element to end.

### `list.get(index: int) -> T`
Get element at index.

### `list.size() -> int`
Get number of elements.

### `list.clear() -> void`
Remove all elements.

### `list_destroy(list) -> void`
Free memory.

---

## 🗺️ Hash Map Operations (Phase 3) 🔄

### `map() -> map[K:V]`
Create new empty hash map.

### `map.set(key: K, value: V) -> void`
Insert or update key-value pair.

### `map.get(key: K) -> V`
Retrieve value for key (default if not found).

### `map.has(key: K) -> int`
Check if key exists.
- **Returns:** 1 if exists, 0 otherwise

### `map.remove(key: K) -> void`
Remove key-value pair.

### `map.size() -> int`
Get number of entries.

### `map.clear() -> void`
Remove all entries.

### `map_destroy(map) -> void`
Free memory.

---

## 🏗️ Language Features

### Structs (Phase 1) ✅

```mlp
struct Person then
    string name;
    int age;
end

Person p;
p.name = "Alice";
p.age = 25;
```

### Arrays (Phase 0) ✅

```mlp
int numbers[10];
numbers[0] = 42;
int value = numbers[0];
```

### Control Flow

```mlp
-- Conditional
if condition then
    -- body
else
    -- alternative
end

-- While loop
while condition
    -- body
    if exit_cond then break end
end

-- For loop
for i = 0 to 10
    -- body
end
```

### Functions

```mlp
function add(a, b) then
    return a + b
end

int result = add(5, 3);
```

---

## 📖 See Also

- [FILE_IO_DESIGN.md](FILE_IO_DESIGN.md) - File I/O specification
- [STRING_OPS_DESIGN.md](STRING_OPS_DESIGN.md) - String operations specification
- [HASHMAP_DESIGN.md](HASHMAP_DESIGN.md) - Hash map specification
- [LIST_DESIGN.md](LIST_DESIGN.md) - List specification
- [TURING_COMPLETE_PLAN.md](TURING_COMPLETE_PLAN.md) - Roadmap

---

**Quick Links:**
- Examples: [examples/](examples/)
- Full docs: [docs/](docs/)
- Spec: [SPECS.md](SPECS.md)
