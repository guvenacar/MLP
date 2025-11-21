# Path Utilities - Design Specification

**Status**: Phase 5.3 - In Progress
**Version**: 1.0
**Last Updated**: November 21, 2025

---

## Overview

This document specifies path utility functions for MLP, enabling programs to manipulate file paths, extract components, and construct paths safely.

### Goals
- Join path components safely
- Extract filename and extension
- Extract directory from path
- Platform-aware path handling

### Non-Goals
- Path normalization (future work)
- Glob pattern matching (future work)
- Symlink resolution (future work)

---

## 1. Path Utility Functions

### 1.1 join_path()

**Purpose**: Join path components with correct separator

**Signature**:
```mlp
function join_path(parts[], count) then
    -- Returns joined path string
end
```

**Parameters**:
- `parts` (array of string): Path components to join
- `count` (int): Number of components

**Return Value**:
- Joined path string
- Empty string on error

**Behavior**:
- Uses "/" as separator (Linux/Unix)
- Handles leading/trailing separators correctly
- Does not normalize path (no ".." resolution)

**Example**:
```mlp
function build_output_path(dir, filename) then
    string parts[2]
    parts[0] = dir
    parts[1] = filename
    return join_path(parts, 2)
end

-- Usage
string path = build_output_path("build", "output.o")
-- Result: "build/output.o"
```

**Convenience Functions**:
```mlp
-- Two-part join (most common)
function join_path_2(dir, file) then
    string parts[2]
    parts[0] = dir
    parts[1] = file
    return join_path(parts, 2)
end

-- Three-part join
function join_path_3(a, b, c) then
    string parts[3]
    parts[0] = a
    parts[1] = b
    parts[2] = c
    return join_path(parts, 3)
end
```

**Edge Cases**:
```mlp
join_path_2("", "file.txt")        -- "file.txt"
join_path_2("/home", "user")       -- "/home/user"
join_path_2("/home/", "user")      -- "/home/user"
join_path_2("/home", "/user")      -- "/home/user" (removes duplicate /)
```

---

### 1.2 get_file_extension()

**Purpose**: Extract file extension from path

**Signature**:
```mlp
function get_file_extension(path) then
    -- Returns extension including dot
end
```

**Parameters**:
- `path` (string): File path or filename

**Return Value**:
- Extension including dot (e.g., ".txt")
- Empty string if no extension

**Behavior**:
- Returns text after last dot in filename
- Handles multiple dots correctly
- Ignores dots in directory names

**Example**:
```mlp
function is_source_file(path) then
    string ext = get_file_extension(path)
    if ext == ".mlp" then
        return 1
    end
    return 0
end

function compile_based_on_type(path) then
    string ext = get_file_extension(path)

    if ext == ".mlp" then
        compile_mlp(path)
    else if ext == ".asm" then
        assemble(path)
    else if ext == ".c" then
        compile_c(path)
    else
        print "Unknown file type: "
        print ext
    end
end
```

**Edge Cases**:
```mlp
get_file_extension("file.txt")           -- ".txt"
get_file_extension("archive.tar.gz")     -- ".gz"
get_file_extension("Makefile")           -- "" (no extension)
get_file_extension(".gitignore")         -- "" (hidden file, no ext)
get_file_extension("dir.name/file")      -- "" (dot in directory)
```

---

### 1.3 get_file_name()

**Purpose**: Extract filename from path

**Signature**:
```mlp
function get_file_name(path) then
    -- Returns filename without directory
end
```

**Parameters**:
- `path` (string): Full file path

**Return Value**:
- Filename component (with extension)
- Original path if no directory separator

**Behavior**:
- Returns text after last "/"
- Includes extension in result
- Handles trailing slashes

**Example**:
```mlp
function show_compiling(path) then
    string name = get_file_name(path)
    print "Compiling: "
    print name
end

function create_object_name(source_path) then
    string name = get_file_name(source_path)
    -- Replace extension
    string base = get_file_basename(name)
    return string_concat(base, ".o")
end
```

**Edge Cases**:
```mlp
get_file_name("/home/user/file.txt")    -- "file.txt"
get_file_name("file.txt")               -- "file.txt"
get_file_name("/home/user/")            -- "" (directory path)
get_file_name("")                       -- ""
```

---

### 1.4 get_directory()

**Purpose**: Extract directory from path

**Signature**:
```mlp
function get_directory(path) then
    -- Returns directory portion of path
end
```

**Parameters**:
- `path` (string): Full file path

**Return Value**:
- Directory portion (without trailing slash)
- "." if no directory component
- Empty string for root "/"

**Behavior**:
- Returns text before last "/"
- Handles relative and absolute paths
- Removes trailing slashes

**Example**:
```mlp
function ensure_output_directory(output_path) then
    string dir = get_directory(output_path)

    if string_length(dir) > 0 then
        if directory_exists(dir) == 0 then
            create_directory(dir)
        end
    end
end

function process_in_same_directory(source_path) then
    string dir = get_directory(source_path)
    string output_path = join_path_2(dir, "output.txt")
    write_file(output_path, "processed")
end
```

**Edge Cases**:
```mlp
get_directory("/home/user/file.txt")    -- "/home/user"
get_directory("dir/file.txt")           -- "dir"
get_directory("file.txt")               -- "."
get_directory("/file.txt")              -- ""
get_directory("/")                      -- ""
```

---

## 2. Additional Helper Functions

### 2.1 get_file_basename()

**Purpose**: Get filename without extension

**Signature**:
```mlp
function get_file_basename(path) then
    -- Returns filename without extension
end
```

**Example**:
```mlp
get_file_basename("file.txt")           -- "file"
get_file_basename("archive.tar.gz")     -- "archive.tar"
get_file_basename("/home/user/doc.pdf") -- "doc"
```

### 2.2 is_absolute_path()

**Purpose**: Check if path is absolute

**Signature**:
```mlp
function is_absolute_path(path) then
    -- Returns 1 if absolute, 0 if relative
end
```

**Example**:
```mlp
is_absolute_path("/home/user")          -- 1
is_absolute_path("relative/path")       -- 0
is_absolute_path("./file.txt")          -- 0
```

### 2.3 normalize_path()

**Purpose**: Normalize path (remove "..", ".")

**Signature**:
```mlp
function normalize_path(path) then
    -- Returns normalized path
end
```

**Example**:
```mlp
normalize_path("/home/user/../other")   -- "/home/other"
normalize_path("./dir/./file")          -- "dir/file"
normalize_path("a/b/../c")              -- "a/c"
```

---

## 3. Implementation Details

### 3.1 Runtime Implementation (C)

```c
#include <string.h>
#include <libgen.h>

// join_path - Join path components
const char* mlp_join_path(const char** parts, int64_t count) {
    static char buffer[4096];
    buffer[0] = '\0';

    int pos = 0;
    for (int i = 0; i < count && pos < sizeof(buffer) - 2; i++) {
        const char* part = parts[i];
        if (!part) continue;

        // Skip empty parts
        int len = strlen(part);
        if (len == 0) continue;

        // Add separator if needed
        if (pos > 0 && buffer[pos-1] != '/') {
            buffer[pos++] = '/';
        }

        // Skip leading slash if we already have content
        int start = 0;
        if (pos > 0 && part[0] == '/') {
            start = 1;
        }

        // Copy part (strip trailing slashes)
        while (start < len && pos < sizeof(buffer) - 1) {
            buffer[pos++] = part[start++];
        }

        // Remove trailing slashes
        while (pos > 1 && buffer[pos-1] == '/') {
            pos--;
        }
    }

    buffer[pos] = '\0';
    return buffer;
}

// get_file_extension - Extract extension
const char* mlp_get_file_extension(const char* path) {
    static char buffer[256];
    buffer[0] = '\0';

    if (!path) return buffer;

    // Find filename part (after last /)
    const char* filename = strrchr(path, '/');
    if (filename) {
        filename++;
    } else {
        filename = path;
    }

    // Find last dot in filename
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return buffer;  // No extension or hidden file
    }

    strncpy(buffer, dot, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    return buffer;
}

// get_file_name - Extract filename
const char* mlp_get_file_name(const char* path) {
    static char buffer[256];
    buffer[0] = '\0';

    if (!path) return buffer;

    const char* slash = strrchr(path, '/');
    if (slash) {
        strncpy(buffer, slash + 1, sizeof(buffer) - 1);
    } else {
        strncpy(buffer, path, sizeof(buffer) - 1);
    }
    buffer[sizeof(buffer) - 1] = '\0';
    return buffer;
}

// get_directory - Extract directory
const char* mlp_get_directory(const char* path) {
    static char buffer[4096];
    buffer[0] = '\0';

    if (!path || !*path) {
        strcpy(buffer, ".");
        return buffer;
    }

    // Copy path
    strncpy(buffer, path, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Remove trailing slashes
    int len = strlen(buffer);
    while (len > 1 && buffer[len-1] == '/') {
        buffer[--len] = '\0';
    }

    // Find last slash
    char* slash = strrchr(buffer, '/');
    if (!slash) {
        strcpy(buffer, ".");
        return buffer;
    }

    if (slash == buffer) {
        // Root directory
        buffer[1] = '\0';
        return buffer;
    }

    *slash = '\0';
    return buffer;
}

// is_absolute_path - Check if absolute
int64_t mlp_is_absolute_path(const char* path) {
    if (!path || !*path) return 0;
    return path[0] == '/' ? 1 : 0;
}
```

### 3.2 Compiler Integration

**Lexer Tokens**:
```c
TOKEN_JOIN_PATH,           // join_path
TOKEN_GET_FILE_EXTENSION,  // get_file_extension
TOKEN_GET_FILE_NAME,       // get_file_name
TOKEN_GET_DIRECTORY,       // get_directory
```

**Code Generation**:
```asm
; join_path_2(dir, file)
lea rdi, [parts_array]
mov rsi, 2
call mlp_join_path
; string pointer in rax

; get_file_extension(path)
mov rdi, [path_ptr]
call mlp_get_file_extension
; extension in rax
```

---

## 4. Use Cases

### 4.1 Build System Paths

```mlp
function get_object_path(source_path, build_dir) then
    -- source: "src/main.mlp" -> "build/obj/main.o"
    string name = get_file_name(source_path)
    string base = get_file_basename(name)
    string obj_name = string_concat(base, ".o")

    return join_path_3(build_dir, "obj", obj_name)
end

function setup_build_paths() then
    string src = "src/compiler/parser.mlp"

    string dir = get_directory(src)
    print "Source dir: "
    print dir

    string name = get_file_name(src)
    print "Source file: "
    print name

    string ext = get_file_extension(src)
    print "Extension: "
    print ext

    string obj = get_object_path(src, "build")
    print "Object path: "
    print obj
end
```

### 4.2 Source Tree Traversal

```mlp
function find_all_sources(dir, extension) then
    list[string] sources = list()
    list[string] entries = list_directory(dir)

    int i = 0
    while i < entries.size()
        string entry = entries.get(i)
        string full_path = join_path_2(dir, entry)

        if directory_exists(full_path) == 1 then
            -- Recurse into subdirectory
            list[string] sub = find_all_sources(full_path, extension)
            -- Merge lists (simplified)
        else
            string ext = get_file_extension(entry)
            if ext == extension then
                sources.add(full_path)
            end
        end

        i = i + 1
    end

    list_destroy(entries)
    return sources
end
```

### 4.3 Output File Generation

```mlp
function generate_output_paths(input_path) then
    string dir = get_directory(input_path)
    string base = get_file_basename(get_file_name(input_path))

    string asm_path = join_path_2(dir, string_concat(base, ".asm"))
    string obj_path = join_path_2(dir, string_concat(base, ".o"))
    string exe_path = join_path_2(dir, base)

    print "Assembly: "
    print asm_path
    print "Object: "
    print obj_path
    print "Executable: "
    print exe_path
end
```

---

## 5. Error Handling

### 5.1 Path Validation

```mlp
function validate_path(path) then
    if string_length(path) == 0 then
        print "Error: Empty path"
        return 0
    end

    if string_length(path) > 4095 then
        print "Error: Path too long"
        return 0
    end

    -- Check for null bytes (security)
    int i = 0
    while i < string_length(path)
        if char_code(string_char_at(path, i)) == 0 then
            print "Error: Null byte in path"
            return 0
        end
        i = i + 1
    end

    return 1
end
```

### 5.2 Safe Path Operations

```mlp
function safe_join_path(base, user_input) then
    -- Validate user input
    if string_find(user_input, "..") >= 0 then
        print "Error: Path traversal not allowed"
        return ""
    end

    if is_absolute_path(user_input) == 1 then
        print "Error: Absolute paths not allowed"
        return ""
    end

    return join_path_2(base, user_input)
end
```

---

## 6. Testing

### 6.1 Unit Tests

```mlp
function test_join_path() then
    string result = join_path_2("dir", "file.txt")
    assert(result == "dir/file.txt", "Basic join failed")

    result = join_path_2("/home", "user")
    assert(result == "/home/user", "Absolute join failed")

    result = join_path_2("dir/", "file")
    assert(result == "dir/file", "Trailing slash failed")

    print "test_join_path: PASSED"
end

function test_get_file_extension() then
    string ext = get_file_extension("file.txt")
    assert(ext == ".txt", "Basic extension failed")

    ext = get_file_extension("archive.tar.gz")
    assert(ext == ".gz", "Double extension failed")

    ext = get_file_extension("Makefile")
    assert(string_length(ext) == 0, "No extension failed")

    print "test_get_file_extension: PASSED"
end

function test_get_file_name() then
    string name = get_file_name("/home/user/file.txt")
    assert(name == "file.txt", "Absolute path failed")

    name = get_file_name("file.txt")
    assert(name == "file.txt", "Simple name failed")

    print "test_get_file_name: PASSED"
end

function test_get_directory() then
    string dir = get_directory("/home/user/file.txt")
    assert(dir == "/home/user", "Absolute path failed")

    dir = get_directory("file.txt")
    assert(dir == ".", "No directory failed")

    print "test_get_directory: PASSED"
end
```

---

## 7. Future Enhancements

- Path normalization (`normalize_path`)
- Glob pattern matching
- Symlink resolution
- Windows path support (future platform)
- URL/URI path handling
- Path comparison (equality, contains)

---

## 8. Examples

See:
- `examples/file_manager.mlp` - Path manipulation
- `examples/build_system.mlp` - Build path generation
- `test/test_path_utilities.mlp` - Test suite

---

## 9. API Summary

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `join_path` | parts[], count | string | Join path components |
| `get_file_extension` | path | string | Extract extension |
| `get_file_name` | path | string | Extract filename |
| `get_directory` | path | string | Extract directory |
| `is_absolute_path` | path | int | Check if absolute |
| `get_file_basename` | path | string | Filename without extension |

---

**Status**: Complete
**Next**: Implement in runtime.c
