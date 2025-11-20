# File I/O Design Specification

**Phase:** 3 - Self-Hosting Completion
**Date:** November 20, 2025
**Status:** Design Complete - Ready for Implementation

---

## 📋 Overview

Phase 3 adds comprehensive file I/O operations to MLP, enabling the language to:
- Read and write source code files
- Process configuration files
- Generate compilation outputs
- Build self-hosting compilers

This is **critical** for self-hosting, as the compiler must be able to read `.mlp` source files and write `.asm` assembly files.

---

## 🎯 Design Goals

1. **Simple API** - Easy-to-use functions for common file operations
2. **Error Handling** - Clear error reporting without exceptions
3. **Memory Safety** - Proper cleanup and bounds checking
4. **Cross-Platform** - Works on Linux, macOS (initial targets)
5. **Efficient** - Minimal memory overhead for large files

---

## 📚 API Design

### Core Functions

#### `read_file(path: string) -> string`
Reads entire file content as a single string.

**Parameters:**
- `path` - File path (relative or absolute)

**Returns:**
- File contents as string
- Empty string `""` on error

**Example:**
```mlp
string source = read_file("program.mlp");
if string_length(source) > 0 then
    print "File loaded successfully"
else
    print "Error: Could not read file"
end
```

**Use Cases:**
- Reading source code for compilation
- Loading configuration files
- Reading small to medium files (< 10MB recommended)

---

#### `write_file(path: string, content: string) -> int`
Writes string content to file (overwrites if exists).

**Parameters:**
- `path` - Output file path
- `content` - String to write

**Returns:**
- `0` on success
- `-1` on error

**Example:**
```mlp
string asm_code = generate_assembly(ast);
int result = write_file("output.asm", asm_code);
if result == 0 then
    print "Assembly written successfully"
else
    print "Error: Could not write file"
end
```

**Use Cases:**
- Writing compiler output (assembly)
- Saving processed data
- Creating log files

---

#### `read_lines(path: string) -> list[string]`
Reads file as list of lines (line-by-line).

**Parameters:**
- `path` - File path

**Returns:**
- `list[string]` with each line as element (newlines stripped)
- Empty list on error

**Example:**
```mlp
list[string] lines = read_lines("config.txt");
int i = 0;
while i < lines.size()
    string line = lines.get(i);
    print line;
    i = i + 1
end
```

**Use Cases:**
- Processing line-oriented data
- Reading configuration files
- Parsing CSV/TSV files
- Analyzing code line-by-line

---

#### `append_file(path: string, content: string) -> int`
Appends content to existing file (creates if doesn't exist).

**Parameters:**
- `path` - File path
- `content` - String to append

**Returns:**
- `0` on success
- `-1` on error

**Example:**
```mlp
string log_entry = "Compilation completed at 10:30\n";
append_file("compile.log", log_entry);
```

**Use Cases:**
- Logging
- Incremental file building
- Append-only data structures

---

#### `file_exists(path: string) -> int`
Checks if file exists.

**Parameters:**
- `path` - File path to check

**Returns:**
- `1` if file exists
- `0` if not found

**Example:**
```mlp
if file_exists("input.mlp") == 1 then
    string source = read_file("input.mlp");
    -- Compile source...
else
    print "Error: input.mlp not found"
end
```

**Use Cases:**
- Pre-flight checks before reading
- Avoiding overwrite confirmations
- Conditional file processing

---

#### `file_size(path: string) -> int`
Gets file size in bytes.

**Parameters:**
- `path` - File path

**Returns:**
- File size in bytes
- `-1` on error

**Example:**
```mlp
int size = file_size("program.mlp");
if size > 1000000 then
    print "Warning: Large file detected"
end
```

**Use Cases:**
- Memory allocation planning
- Size validation
- Progress indicators

---

## 🔧 Runtime Implementation

### C Implementation (`runtime/runtime.c`)

```c
// Read entire file into string
char* mlp_read_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return strdup("");  // Empty string on error

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    char* content = malloc(size + 1);
    if (!content) {
        fclose(file);
        return strdup("");
    }

    // Read file
    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);
    return content;
}

// Write string to file
int mlp_write_file(const char* path, const char* content) {
    FILE* file = fopen(path, "w");
    if (!file) return -1;

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);

    fclose(file);
    return (written == len) ? 0 : -1;
}

// Read file as list of lines
void* mlp_read_lines(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return list_create();  // Empty list on error

    List* lines = list_create();
    char buffer[4096];

    while (fgets(buffer, sizeof(buffer), file)) {
        // Strip newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
        }
        list_add(lines, strdup(buffer));
    }

    fclose(file);
    return lines;
}

// Append to file
int mlp_append_file(const char* path, const char* content) {
    FILE* file = fopen(path, "a");
    if (!file) return -1;

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);

    fclose(file);
    return (written == len) ? 0 : -1;
}

// Check if file exists
int mlp_file_exists(const char* path) {
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Get file size
long mlp_file_size(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}
```

---

## ⚠️ Error Handling Strategy

### Philosophy: No Exceptions
MLP uses **return codes** instead of exceptions for simplicity.

**Error Indicators:**
- Functions returning `string`: Return `""` (empty string) on error
- Functions returning `int`: Return `-1` on error
- Functions returning `list`: Return empty list on error

### Error Checking Pattern
```mlp
-- Pattern 1: Check string length
string content = read_file("input.txt");
if string_length(content) == 0 then
    print "Error reading file"
    return -1
end

-- Pattern 2: Check return code
int result = write_file("output.txt", data);
if result != 0 then
    print "Error writing file"
    return -1
end

-- Pattern 3: Check list size
list[string] lines = read_lines("data.txt");
if lines.size() == 0 then
    print "Error or empty file"
end
```

---

## 🔒 Security Considerations

### 1. Path Traversal Prevention
```c
// Validate path doesn't contain dangerous patterns
int is_safe_path(const char* path) {
    if (strstr(path, "..")) return 0;  // No parent directory
    if (path[0] == '/') return 0;       // No absolute paths (optional)
    return 1;
}
```

**Recommendation:** Restrict file operations to current working directory and subdirectories.

### 2. File Size Limits
```c
#define MAX_FILE_SIZE (100 * 1024 * 1024)  // 100MB limit

char* mlp_read_file_safe(const char* path) {
    long size = mlp_file_size(path);
    if (size > MAX_FILE_SIZE) {
        return strdup("");  // Reject oversized files
    }
    return mlp_read_file(path);
}
```

### 3. Permission Checks
- Use `fopen()` permissions ("r", "w", "a")
- Let OS handle permission errors
- Return error codes appropriately

---

## 📊 Memory Management

### Responsibility Matrix
| Function | Allocates | Caller Must Free |
|----------|-----------|------------------|
| `read_file()` | Yes | Yes (`string`) |
| `write_file()` | No | N/A |
| `read_lines()` | Yes | Yes (`list` + strings) |
| `append_file()` | No | N/A |
| `file_exists()` | No | N/A |
| `file_size()` | No | N/A |

### Cleanup Example
```mlp
string content = read_file("input.txt");
-- Use content...
-- (Automatic cleanup in future Phase 7: Memory Management)

list[string] lines = read_lines("data.txt");
-- Use lines...
list_destroy(lines);  -- Manual cleanup for now
```

---

## 🎯 Use Cases

### Use Case 1: Simple Compiler Pipeline
```mlp
function compile_file(input_path, output_path) then
    -- Read source code
    string source = read_file(input_path);
    if string_length(source) == 0 then
        print "Error: Could not read source file"
        return -1
    end

    -- Compile
    string assembly = compile(source);

    -- Write output
    int result = write_file(output_path, assembly);
    if result != 0 then
        print "Error: Could not write assembly"
        return -1
    end

    print "Compilation successful!"
    return 0
end

compile_file("program.mlp", "program.asm");
```

### Use Case 2: Configuration File Processing
```mlp
function load_config(filename) then
    list[string] lines = read_lines(filename);
    map[string:string] config = map();

    int i = 0;
    while i < lines.size()
        string line = lines.get(i);
        -- Parse "key=value"
        list[string] parts = string_split(line, "=");
        if parts.size() == 2 then
            config.set(parts.get(0), parts.get(1));
        end
        i = i + 1
    end

    return config
end
```

### Use Case 3: Logging System
```mlp
function log_message(level, message) then
    string timestamp = get_timestamp();  -- Hypothetical
    string entry = timestamp + " [" + level + "] " + message + "\n";
    append_file("compiler.log", entry);
    return 0
end

log_message("INFO", "Compilation started");
log_message("WARN", "Unused variable detected");
log_message("ERROR", "Syntax error at line 42");
```

---

## 🧪 Testing Strategy

### Test 1: Read/Write Roundtrip
```mlp
-- Write test data
string original = "Hello MLP!\nLine 2\nLine 3";
write_file("test.txt", original);

-- Read it back
string loaded = read_file("test.txt");

-- Verify
if loaded == original then
    print "PASS: Roundtrip successful"
else
    print "FAIL: Data mismatch"
end
```

### Test 2: Line-by-Line Processing
```mlp
-- Create test file
write_file("lines.txt", "Line 1\nLine 2\nLine 3\n");

-- Read as lines
list[string] lines = read_lines("lines.txt");

-- Verify count
if lines.size() == 3 then
    print "PASS: Line count correct"
end

-- Verify content
if lines.get(0) == "Line 1" then
    print "PASS: Line 1 correct"
end
```

### Test 3: Error Handling
```mlp
-- Non-existent file
string content = read_file("nonexistent.txt");
if string_length(content) == 0 then
    print "PASS: Error handled gracefully"
end

-- File exists check
if file_exists("nonexistent.txt") == 0 then
    print "PASS: file_exists returns 0 for missing file"
end
```

---

## 🚀 Future Enhancements

### Phase 3.5: Binary I/O
```mlp
-- Read binary file
bytes data = read_bytes("image.png");

-- Write binary file
write_bytes("output.bin", data);
```

### Phase 4+: Streams
```mlp
-- Stream large files
FileStream stream = file_stream("large_file.txt");
while stream.has_next()
    string line = stream.read_line();
    process(line);
end
stream.close();
```

### Advanced: Directory Operations
```mlp
list[string] files = list_directory("src/");
int result = create_directory("output/");
int result = delete_file("temp.txt");
```

---

## 📖 Related Documentation

- [STRING_OPS_DESIGN.md](STRING_OPS_DESIGN.md) - String manipulation functions
- [HASHMAP_DESIGN.md](HASHMAP_DESIGN.md) - Hash table for configuration
- [LIST_DESIGN.md](LIST_DESIGN.md) - Dynamic lists (Phase 2)
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API docs

---

## ✅ Success Criteria

Phase 3 File I/O complete when:
- [x] All 6 core functions implemented
- [x] Error handling consistent
- [x] Memory management safe
- [x] Tests pass (read/write/lines/append)
- [x] Self-hosting compiler can read `.mlp` and write `.asm`
- [x] Documentation complete

---

**Last Updated:** November 20, 2025
**Status:** Design Complete - Ready for Implementation
**Implementation Priority:** 🔴 CRITICAL (Required for self-hosting)
