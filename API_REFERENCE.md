# MLP API Reference

**Version:** Phase 7.9+
**Last Updated:** November 23, 2025

---

## Quick Reference

| Category | Functions | Status |
|----------|-----------|--------|
| **Async/Await** | 27 functions (Phase 8.9) | ✅ Phase 8 |
| **Command-Line Args** | argc, argv | Phase 5.1 🔄 |
| **Enhanced String Ops** | 4 new functions | Phase 5.1 🔄 |
| **File I/O** | 6 functions | Phase 3 ✅ |
| **Binary I/O** | 4 functions | Phase 5.3 🔄 |
| **Directory Ops** | 6 functions | Phase 5.3 🔄 |
| **Process Control** | 4 functions | Phase 5.3 🔄 |
| **Time Utilities** | 4 functions | Phase 5.3 🔄 |
| **Path Utilities** | 4 functions | Phase 5.3 🔄 |
| **String Ops** | 15 functions (6 existing + 9 new) | Phase 3 ✅ |
| **Type Conversions** | 8 functions (str, num + 6 existing) | Phase 7.9+ ✅ |
| **Math Operations** | 4 functions | Phase 4 ✅ |
| **Lists** | 7 functions | Phase 2 ✅ |
| **Hash Maps** | 8 functions | Phase 3 ✅ |
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

## 📦 Binary File I/O (Phase 5.3) 🔄

### `read_binary(path: string) -> pointer`
Read entire file as byte array.
- **Returns:** Pointer to allocated bytes, 0 on error
- **Example:** `int data = read_binary("program.o");`

### `write_binary(path: string, data: pointer, size: int) -> int`
Write byte array to file.
- **Returns:** 0 on success, -1 on error
- **Example:** `write_binary("output.bin", buffer, 1024);`

### `get_file_info(path: string) -> FileInfo*`
Get file metadata (size, modified time, permissions).
- **Returns:** Pointer to FileInfo struct, 0 if not found
- **Example:** `int info = get_file_info("source.mlp");`

### `copy_file(source: string, dest: string) -> int`
Copy file from source to destination.
- **Returns:** 0 on success, -1 on error
- **Example:** `copy_file("original.txt", "backup.txt");`

---

## 📂 Directory Operations (Phase 5.3) 🔄

### `list_directory(path: string) -> list[string]`
List files and subdirectories in directory.
- **Returns:** List of entry names
- **Example:** `list[string] files = list_directory("src");`

### `create_directory(path: string) -> int`
Create new directory.
- **Returns:** 0 on success, -1 on error
- **Example:** `create_directory("build/obj");`

### `remove_directory(path: string) -> int`
Remove empty directory.
- **Returns:** 0 on success, -1 on error
- **Example:** `remove_directory("temp");`

### `directory_exists(path: string) -> int`
Check if directory exists.
- **Returns:** 1 if exists, 0 otherwise
- **Example:** `if directory_exists("build") == 1 then`

### `get_current_dir() -> string`
Get current working directory.
- **Returns:** Absolute path to current directory
- **Example:** `string cwd = get_current_dir();`

### `change_directory(path: string) -> int`
Change current working directory.
- **Returns:** 0 on success, -1 on error
- **Example:** `change_directory("src");`

---

## 🔧 Process Control (Phase 5.3) 🔄

### `execute_command(cmd: string) -> int`
Execute shell command and return exit code.
- **Returns:** Exit code (0-255), -1 on execution failure
- **Example:** `int result = execute_command("nasm -f elf64 file.asm");`

### `get_command_output(cmd: string) -> string`
Execute command and capture stdout.
- **Returns:** Command output as string
- **Example:** `string branch = get_command_output("git branch --show-current");`

### `get_process_id() -> int`
Get current process ID.
- **Returns:** PID (positive integer)
- **Example:** `int pid = get_process_id();`

### `get_parent_process_id() -> int`
Get parent process ID.
- **Returns:** PPID (positive integer)
- **Example:** `int ppid = get_parent_process_id();`

---

## ⏰ Time Utilities (Phase 5.3) 🔄

### `format_timestamp(timestamp: int, format: string) -> string`
Format Unix timestamp to string.
- **Returns:** Formatted date/time string
- **Example:** `string date = format_timestamp(ts, "%Y-%m-%d %H:%M:%S");`

### `parse_timestamp(date_string: string) -> int`
Parse date string to Unix timestamp.
- **Returns:** Timestamp, 0 on parse error
- **Example:** `int ts = parse_timestamp("2025-11-21");`

### `get_milliseconds() -> int`
Get high-precision time for profiling.
- **Returns:** Milliseconds (monotonic)
- **Example:** `int start = get_milliseconds();`

### `get_time_string() -> string`
Get current time as formatted string.
- **Returns:** "YYYY-MM-DD HH:MM:SS"
- **Example:** `string now = get_time_string();`

---

## 📁 Path Utilities (Phase 5.3) 🔄

### `join_path(parts: string[], count: int) -> string`
Join path components with separator.
- **Returns:** Joined path string
- **Example:** `string path = join_path_2("build", "output.o");`

### `get_file_extension(path: string) -> string`
Extract file extension from path.
- **Returns:** Extension including dot (e.g., ".txt")
- **Example:** `string ext = get_file_extension("file.mlp");`

### `get_file_name(path: string) -> string`
Extract filename from path.
- **Returns:** Filename with extension
- **Example:** `string name = get_file_name("/home/user/doc.txt");`

### `get_directory(path: string) -> string`
Extract directory from path.
- **Returns:** Directory portion
- **Example:** `string dir = get_directory("/home/user/doc.txt");`

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

## 🔄 Type Conversions (Phase 4) ✅

### `str(value: numeric) -> string` **⭐ NEW (Phase 7.9+)**
Convert numeric value to string (wrapper for int_to_string).
- **Returns:** String representation (e.g., `42` → `"42"`)
- **Example:** `print "Value: " + str(42);  -- "Value: 42"`
- **Use Case:** Clean syntax for type conversion in expressions

### `num(text: string) -> numeric` **⭐ NEW (Phase 7.9+)**
Parse numeric value from string (wrapper for string_to_int).
- **Returns:** Parsed number or `0` on error
- **Example:** `numeric value = num("123");  -- 123`
- **Use Case:** User input parsing, config file reading

### `int_to_string(num: int) -> string`
Convert integer to string.
- **Returns:** String representation (e.g., `42` → `"42"`)
- **Example:** `string msg = "Count: " + int_to_string(count);`

### `string_to_int(str: string) -> int`
Parse integer from string.
- **Returns:** Parsed integer or `0` on error
- **Example:** `int value = string_to_int("42");`

### `char_code(ch: string) -> int`
Get ASCII/Unicode code point of first character.
- **Returns:** Character code or `0` if empty
- **Example:** `int code = char_code("A"); -- 65`

### `char_from_code(code: int) -> string`
Create single-character string from code point.
- **Returns:** Single-character string or `""` if invalid
- **Example:** `string letter = char_from_code(65); -- "A"`

### `string_concat(s1: string, s2: string) -> string`
Concatenate two strings (alternative to `+` operator).
- **Returns:** New concatenated string
- **Example:** `string result = string_concat("Hello", "World");`
- **Note:** The `+` operator now works for string concatenation: `"Hello" + " World"`

### `string_char_at(str: string, index: int) -> string`
Get character at index.
- **Returns:** Single-character string or `""` if out of bounds
- **Example:** `string ch = string_char_at("Hello", 0); -- "H"`

---

## 🔢 Math Operations (Phase 4) 🔄

### `abs(num: int) -> int`
Get absolute value.
- **Returns:** Non-negative integer
- **Example:** `int distance = abs(a - b);`

### `min(a: int, b: int) -> int`
Get smaller of two integers.
- **Returns:** Minimum value
- **Example:** `int smaller = min(10, 20); -- 10`

### `max(a: int, b: int) -> int`
Get larger of two integers.
- **Returns:** Maximum value
- **Example:** `int larger = max(10, 20); -- 20`

### `clamp(value: int, min_val: int, max_val: int) -> int`
Restrict value to range.
- **Returns:** Clamped value
- **Example:** `int safe = clamp(value, 0, 100);`

---

## 🎮 Command-Line Arguments (Phase 5.1) 🔄

### argc / argv

Access command-line arguments in the main function.

**Syntax**:
```mlp
function main(argc, argv) then
    -- argc is int: argument count
    -- argv provides .get(index) to access arguments
end
```

**Parameters**:
- `argc` (int): Number of arguments (including program name)
- `argv` (special): Argument vector with `.get(index)` method

**Returns**:
- `argc`: integer count
- `argv.get(i)`: string at index i, or empty string if out of bounds

**Example**:
```mlp
function main(argc, argv) then
    int i = 0;
    while i < argc then
        string arg = argv.get(i);
        print arg
        i = i + 1
    end
end
```

**Notes**:
- Index 0 is always the program name
- Out-of-bounds access returns empty string (safe)
- argc and argv only work as main() parameters

---

## 📝 Enhanced String Operations (Phase 5.1) 🔄

### `string_length(str: string) -> int`

Get the length of a string.

**Parameters**:
- `str`: Input string

**Returns**: Length of string (number of characters)

**Example**:
```mlp
string text = "Hello";
int len = string_length(text);  -- 5
```

---

### `string_substring(str: string, start: int, length: int) -> string`

Extract a substring from a string.

**Parameters**:
- `str`: Input string
- `start`: Start index (0-based)
- `length`: Number of characters to extract

**Returns**: New string containing substring

**Example**:
```mlp
string text = "Hello World";
string sub = string_substring(text, 0, 5);  -- "Hello"
```

**Notes**:
- Returns empty string if start is out of bounds
- Automatically truncates if length exceeds string
- Safe for all input values

---

### `string_index_of(str: string, needle: string) -> int`

Find the first occurrence of a substring.

**Parameters**:
- `str`: String to search in
- `needle`: Substring to find

**Returns**: Index of first occurrence, or -1 if not found

**Example**:
```mlp
string text = "Hello World";
int pos = string_index_of(text, "World");  -- 6
int not_found = string_index_of(text, "xyz");  -- -1
```

**Notes**:
- Case-sensitive search
- Returns -1 if not found
- 0-based indexing

---

### `string_last_index_of(str: string, needle: string) -> int`

Find the last occurrence of a substring.

**Parameters**:
- `str`: String to search in
- `needle`: Substring to find

**Returns**: Index of last occurrence, or -1 if not found

**Example**:
```mlp
string text = "Hello World Hello";
int pos = string_last_index_of(text, "Hello");  -- 12
```

**Notes**:
- Useful for finding file extensions, paths
- Returns -1 if not found
- 0-based indexing

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
func add(numeric a, numeric b)
    return a + b
end func

numeric result = add(5, 3)
```

---

## 🚀 Async/Await Operations (Phase 8)

### Core Promise Functions

#### `promise_create() -> numeric`
Create a new pending promise.
- **Returns:** Promise pointer (as numeric)
- **Example:** `numeric p = promise_create();`

#### `promise_resolve(promise: numeric, value: numeric) -> void`
Resolve a promise with a value.
- **Parameters:** Promise pointer, value
- **Example:** `promise_resolve(p, 42);`

#### `promise_reject(promise: numeric, error: string) -> void`
Reject a promise with an error message.
- **Parameters:** Promise pointer, error message
- **Example:** `promise_reject(p, "Operation failed");`

#### `promise_await_safe(promise: numeric) -> numeric`
Safely await promise completion (blocking).
- **Returns:** 0=resolved, -1=rejected, -2=invalid
- **Example:** `numeric status = promise_await_safe(p);`
- **Note:** Handles both resolved AND rejected promises (no infinite loop)

#### `promise_is_completed(promise: numeric) -> numeric`
Check if promise is completed (resolved OR rejected).
- **Returns:** 1=completed, 0=pending
- **Example:** `numeric done = promise_is_completed(p);`

#### `promise_free(promise: numeric) -> void`
Free promise memory.
- **Parameters:** Promise pointer
- **Example:** `promise_free(p);`

### Promise State Checks

#### `promise_is_pending(promise: numeric) -> numeric`
Check if promise is still pending.
- **Returns:** 1=pending, 0=not pending
- **Example:** `if promise_is_pending(p) then ... end`

#### `promise_is_resolved(promise: numeric) -> numeric`
Check if promise is resolved.
- **Returns:** 1=resolved, 0=not resolved
- **Example:** `if promise_is_resolved(p) then ... end`

#### `promise_is_rejected(promise: numeric) -> numeric`
Check if promise is rejected.
- **Returns:** 1=rejected, 0=not rejected
- **Example:** `if promise_is_rejected(p) then ... end`

#### `promise_get_state(promise: numeric) -> numeric`
Get raw promise state.
- **Returns:** 0=pending, 1=resolved, 2=rejected
- **Example:** `numeric state = promise_get_state(p);`

### Promise Value Access

#### `promise_get_value(promise: numeric) -> numeric`
Get value from resolved promise.
- **Returns:** Promise value or 0
- **Example:** `numeric val = promise_get_value(p);`

#### `promise_get_numeric_value(promise: numeric) -> numeric`
Get numeric value from resolved promise.
- **Returns:** Numeric value or 0
- **Example:** `numeric num = promise_get_numeric_value(p);`

#### `promise_get_string_value(promise: numeric) -> string`
Get string value from resolved promise.
- **Returns:** String value or ""
- **Example:** `string text = promise_get_string_value(p);`

#### `promise_await_value(promise: numeric) -> numeric`
Blocking await that returns value directly.
- **Returns:** Value if resolved, NULL if rejected
- **Example:** `numeric val = promise_await_value(p);`

### Promise Composition

#### `promise_race(promises: numeric[], count: numeric) -> numeric`
Return first completed promise (resolved or rejected).
- **Returns:** First completed promise
- **Example:** `numeric winner = promise_race(promises, 3);`
- **Use case:** Timeout patterns, fastest response

#### `promise_race_simple(p1, p2, p3: numeric) -> numeric`
Race helper for 3 promises (no array needed).
- **Returns:** First completed promise
- **Example:** `numeric winner = promise_race_simple(p1, p2, p3);`

#### `promise_any(promises: numeric[], count: numeric) -> numeric`
Return first **successful** promise (ignores rejections).
- **Returns:** First resolved promise, or rejected if all fail
- **Example:** `numeric winner = promise_any(promises, 3);`
- **Use case:** Fallback strategies

#### `promise_any_simple(p1, p2, p3: numeric) -> numeric`
Any helper for 3 promises.
- **Returns:** First resolved promise
- **Example:** `numeric winner = promise_any_simple(p1, p2, p3);`

#### `promise_allSettled(promises: numeric[], count: numeric) -> numeric`
Wait for all promises to complete (success or failure).
- **Returns:** Promise that resolves when all are done
- **Example:** `numeric result = promise_allSettled(promises, 3);`
- **Use case:** Batch processing

#### `promise_allSettled_simple(p1, p2, p3: numeric) -> numeric`
AllSettled helper for 3 promises.
- **Returns:** Promise when all 3 complete
- **Example:** `numeric result = promise_allSettled_simple(p1, p2, p3);`

#### `promise_all(promises: numeric[], count: numeric) -> numeric`
Wait for all promises to resolve (fails if any rejects).
- **Returns:** Promise that resolves when all succeed
- **Example:** `numeric result = promise_all(promises, 3);`

#### `promise_all_simple(p1, p2, p3: numeric) -> numeric`
All helper for 3 promises.
- **Returns:** Promise when all 3 resolve
- **Example:** `numeric result = promise_all_simple(p1, p2, p3);`

### Async I/O Primitives

#### `async_sleep(milliseconds: numeric) -> numeric`
Asynchronous delay (pthread-based).
- **Returns:** Promise that resolves after delay
- **Example:** `numeric p = async_sleep(1000);`  // 1 second

#### `async_sleep_promise(milliseconds: numeric) -> numeric`
Alias for async_sleep.
- **Returns:** Promise
- **Example:** `numeric p = async_sleep_promise(500);`

#### `async_delay(milliseconds: numeric) -> numeric`
Another alias for async_sleep.
- **Returns:** Promise
- **Example:** `numeric p = async_delay(2000);`

#### `async_read_file(path: string) -> numeric`
Asynchronous file reading.
- **Returns:** Promise that resolves with file content
- **Example:** `numeric p = async_read_file("data.txt");`

#### `async_write_file(path: string, content: string) -> numeric`
Asynchronous file writing.
- **Returns:** Promise that resolves on success
- **Example:** `numeric p = async_write_file("out.txt", data);`

#### `async_http_get(url: string) -> numeric`
Asynchronous HTTP GET request (libcurl).
- **Returns:** Promise that resolves with response
- **Example:** `numeric p = async_http_get("https://api.example.com");`

### Timeout & Error Handling

#### `async_timeout(promise: numeric, milliseconds: numeric) -> numeric`
Wrap promise with timeout.
- **Returns:** New promise that rejects on timeout
- **Example:** `numeric timed = async_timeout(task, 5000);`  // 5 sec timeout
- **Behavior:** Rejects if timeout expires before promise resolves
- **Note:** 0ms timeout immediately rejects (fixed in Phase 8.10)

#### `promise_with_timeout(promise: numeric, ms: numeric) -> numeric`
Alias for async_timeout.
- **Returns:** Timeout-wrapped promise
- **Example:** `numeric timed = promise_with_timeout(p, 3000);`

#### `promise_has_error(promise: numeric) -> numeric`
Check if promise has error message.
- **Returns:** 1=has error, 0=no error
- **Example:** `if promise_has_error(p) then ... end`

#### `promise_get_error(promise: numeric) -> string`
Get error message from rejected promise.
- **Returns:** Error string or ""
- **Example:** `string err = promise_get_error(p);`

### Advanced API (Manual Promise Control)

#### `promise_new() -> numeric`
Create empty promise (manual control).
- **Returns:** New promise pointer
- **Example:** `numeric p = promise_new();`

#### `promise_resolve_numeric(promise: numeric, value: numeric) -> void`
Manually resolve promise with numeric value.
- **Example:** `promise_resolve_numeric(p, 123);`

#### `promise_resolve_string(promise: numeric, value: string) -> void`
Manually resolve promise with string value.
- **Example:** `promise_resolve_string(p, "Success!");`

#### `promise_reject_with_error(promise: numeric, error: string) -> void`
Manually reject promise with error.
- **Example:** `promise_reject_with_error(p, "Failed");`

#### `promise_chain(p1, p2: numeric) -> numeric`
Chain two promises sequentially.
- **Returns:** Promise that completes after both
- **Example:** `numeric chain = promise_chain(first, second);`

#### `promise_finally(promise: numeric, callback: numeric) -> void`
Register cleanup handler (runs on any completion).
- **Example:** `promise_finally(p, cleanup_fn);`

### Usage Examples

#### Basic Async/Await
```mlp
async function fetch_data()
    numeric p = async_sleep(1000)
    numeric status = promise_await_safe(p)
    if status == 0 then
        print "Success!"
    end
    return 42
end
```

#### Parallel Execution
```mlp
numeric p1 = async_http_get("https://api1.com")
numeric p2 = async_http_get("https://api2.com")
numeric p3 = async_http_get("https://api3.com")

numeric all = promise_all_simple(p1, p2, p3)
numeric status = promise_await_safe(all)
print "All requests complete!"
```

#### Race Pattern (Fastest Wins)
```mlp
numeric server1 = async_http_get("https://server1.com")
numeric server2 = async_http_get("https://server2.com")

numeric winner = promise_race_simple(server1, server2)
numeric status = promise_await_safe(winner)
print "Fastest server responded!"
```

#### Timeout Handling
```mlp
numeric task = async_http_get("https://slow-api.com")
numeric timed = async_timeout(task, 5000)

numeric status = promise_await_safe(timed)
if status == 0 then
    print "Success!"
else
    print "Timeout or error"
end
```

#### Error Handling
```mlp
numeric p = async_http_get("https://api.com")
numeric status = promise_await_safe(p)

if promise_has_error(p) then
    string error = promise_get_error(p)
    print "Error: " + error
end
```

---

## 📖 See Also

### Phase 5.1 Documentation
- [docs/CMD_ARGS_DESIGN.md](docs/CMD_ARGS_DESIGN.md) - Command-line arguments specification
- [docs/STRING_OPS_ENHANCED_DESIGN.md](docs/STRING_OPS_ENHANCED_DESIGN.md) - Enhanced string operations

### Phase 3 Documentation
- [FILE_IO_DESIGN.md](FILE_IO_DESIGN.md) - File I/O specification
- [STRING_OPS_DESIGN.md](STRING_OPS_DESIGN.md) - String operations specification
- [HASHMAP_DESIGN.md](HASHMAP_DESIGN.md) - Hash map runtime specification
- [LIST_DESIGN.md](LIST_DESIGN.md) - List specification

### Phase 4 Documentation
- [HASHMAP_COMPILER_DESIGN.md](HASHMAP_COMPILER_DESIGN.md) - Hash map compiler integration
- [TYPE_CONVERSIONS_DESIGN.md](TYPE_CONVERSIONS_DESIGN.md) - Type conversion utilities
- [MATH_OPERATIONS_DESIGN.md](MATH_OPERATIONS_DESIGN.md) - Math operation utilities

### Phase 5.3 Documentation
- [BINARY_IO_DESIGN.md](docs/BINARY_IO_DESIGN.md) - Binary file I/O specification
- [DIRECTORY_OPS_DESIGN.md](docs/DIRECTORY_OPS_DESIGN.md) - Directory operations specification
- [PROCESS_CONTROL_DESIGN.md](docs/PROCESS_CONTROL_DESIGN.md) - Process control specification
- [TIME_UTILITIES_DESIGN.md](docs/TIME_UTILITIES_DESIGN.md) - Time utilities specification
- [PATH_UTILITIES_DESIGN.md](docs/PATH_UTILITIES_DESIGN.md) - Path utilities specification

### Planning & Roadmap
- [TURING_COMPLETE_PLAN.md](TURING_COMPLETE_PLAN.md) - Development roadmap
- [ROADMAP.md](ROADMAP.md) - Feature roadmap

---

**Quick Links:**
- Examples: [examples/](examples/)
- Full docs: [docs/](docs/)
- Spec: [SPECS.md](SPECS.md)
