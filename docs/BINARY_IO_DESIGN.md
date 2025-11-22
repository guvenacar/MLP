# Binary File I/O - Design Specification

**Status**: Phase 5.3 - In Progress
**Version**: 1.0
**Last Updated**: November 21, 2025

---

## Overview

This document specifies binary file I/O functions for MLP, enabling programs to read and write raw binary data, handle compiled files, and perform file operations with metadata access.

### Goals
- Read and write raw binary data (byte arrays)
- Access file metadata (size, modification time, permissions)
- Copy files with proper error handling
- Support large file operations efficiently

### Non-Goals
- Memory-mapped files - future work
- Async I/O - future work
- File locking - future work

---

## 1. Binary File I/O Functions

### 1.1 read_binary()

**Purpose**: Read entire file as byte array

**Signature**:
```mlp
func read_binary(path)
    -- Returns pointer to byte array, sets size via out parameter
end func
```

**Parameters**:
- `path` (string): Path to file to read

**Return Value**:
- Pointer to allocated byte array (int/pointer)
- 0 (NULL) on failure

**Behavior**:
- Opens file in binary mode ("rb")
- Determines file size using fseek/ftell
- Allocates buffer for entire file
- Reads all bytes into buffer
- Closes file
- Returns pointer to buffer (caller must free)

**Example**:
```mlp
func load_object_file(path)
    int data = read_binary(path)
    if data == 0 then
        panic("Failed to read object file")
    end

    -- Process binary data
    int size = get_file_size(path)
    print "Loaded "
    print size
    print " bytes"

    -- Don't forget to free
    mlp_free(data)
    return 1
end func
```

**Error Handling**:
- Returns NULL if file doesn't exist
- Returns NULL if read fails
- Sets error code via `set_error_code()`

**Error Codes**:
- `0` - Success
- `1` - File not found
- `2` - Permission denied
- `3` - Read error
- `4` - Memory allocation failed

---

### 1.2 write_binary()

**Purpose**: Write byte array to file

**Signature**:
```mlp
func write_binary(path, data, size)
    -- Writes size bytes from data to file
    -- Returns 0 on success, -1 on error
end func
```

**Parameters**:
- `path` (string): Path to file to write
- `data` (pointer): Pointer to byte array
- `size` (int): Number of bytes to write

**Return Value**:
- `0` on success
- `-1` on error

**Behavior**:
- Opens file in binary write mode ("wb")
- Writes exactly `size` bytes from `data`
- Closes file
- Creates file if it doesn't exist
- Overwrites existing file

**Example**:
```mlp
func save_compiled_output(path, code, code_size)
    int result = write_binary(path, code, code_size)
    if result != 0 then
        print "Error: Failed to write output file"
        return 0
    end
    print "Wrote "
    print code_size
    print " bytes to "
    print path
    return 1
end func
```

**Error Handling**:
- Returns -1 if file cannot be created
- Returns -1 if write fails
- Sets error code via `set_error_code()`

**Security Considerations**:
- Validates path (no directory traversal)
- Checks write permissions
- Does not follow symlinks to directories

---

### 1.3 get_file_info()

**Purpose**: Get file metadata

**Signature**:
```mlp
func get_file_info(path)
    -- Returns pointer to FileInfo struct
end func
```

**Parameters**:
- `path` (string): Path to file

**Return Value**:
- Pointer to FileInfo struct
- 0 (NULL) if file doesn't exist

**FileInfo Structure**:
```mlp
struct FileInfo then
    int size;           -- File size in bytes
    int modified_time;  -- Last modification (Unix timestamp)
    int created_time;   -- Creation time (Unix timestamp)
    int permissions;    -- Unix permissions (e.g., 0644)
    int is_directory;   -- 1 if directory, 0 if file
    int is_readable;    -- 1 if readable, 0 otherwise
    int is_writable;    -- 1 if writable, 0 otherwise
end func
```

**Example**:
```mlp
func check_source_file(path)
    int info = get_file_info(path)
    if info == 0 then
        print "File not found: "
        print path
        return 0
    end

    -- Access struct fields (implementation detail)
    int size = file_info_size(info)
    int mtime = file_info_modified(info)

    print "File: "
    print path
    print " Size: "
    print size
    print " bytes"

    mlp_free(info)
    return 1
end func
```

**Helper Functions**:
```mlp
-- Access FileInfo fields
func file_info_size(info) ... end
func file_info_modified(info) ... end
func file_info_created(info) ... end
func file_info_permissions(info) ... end
func file_info_is_directory(info) ... end
func file_info_is_readable(info) ... end
func file_info_is_writable(info) ... end
```

---

### 1.4 copy_file()

**Purpose**: Copy file from source to destination

**Signature**:
```mlp
func copy_file(source, dest)
    -- Copies file, returns 0 on success
end func
```

**Parameters**:
- `source` (string): Path to source file
- `dest` (string): Path to destination file

**Return Value**:
- `0` on success
- `-1` on error

**Behavior**:
- Opens source file in binary read mode
- Opens destination file in binary write mode
- Copies data in 4KB chunks for efficiency
- Preserves file permissions (optional)
- Closes both files

**Example**:
```mlp
func backup_file(path)
    string backup_path = string_concat(path, ".bak")

    int result = copy_file(path, backup_path)
    if result != 0 then
        print "Error: Failed to create backup"
        return 0
    end

    print "Backup created: "
    print backup_path
    return 1
end func
```

**Error Handling**:
- Returns -1 if source doesn't exist
- Returns -1 if destination cannot be created
- Returns -1 if copy fails mid-way
- Sets error code for specific failure reason

**Error Codes**:
- `0` - Success
- `1` - Source file not found
- `2` - Source permission denied
- `3` - Destination cannot be created
- `4` - Write error
- `5` - Disk full

---

## 2. Implementation Details

### 2.1 Runtime Implementation (C)

```c
// read_binary - Read entire file as byte array
void* mlp_read_binary(const char* path, int64_t* out_size) {
    FILE* f = fopen(path, "rb");
    if (!f) {
        mlp_set_error_code(1);  // File not found
        *out_size = 0;
        return NULL;
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Allocate buffer
    void* buffer = mlp_malloc(size);
    if (!buffer) {
        fclose(f);
        mlp_set_error_code(4);  // Memory allocation failed
        *out_size = 0;
        return NULL;
    }

    // Read file
    size_t read = fread(buffer, 1, size, f);
    fclose(f);

    if (read != size) {
        mlp_free(buffer);
        mlp_set_error_code(3);  // Read error
        *out_size = 0;
        return NULL;
    }

    mlp_set_error_code(0);
    *out_size = size;
    return buffer;
}

// write_binary - Write byte array to file
int64_t mlp_write_binary(const char* path, const void* data, int64_t size) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        mlp_set_error_code(3);  // Cannot create file
        return -1;
    }

    size_t written = fwrite(data, 1, size, f);
    fclose(f);

    if (written != size) {
        mlp_set_error_code(4);  // Write error
        return -1;
    }

    mlp_set_error_code(0);
    return 0;
}

// get_file_info - Get file metadata
FileInfo* mlp_get_file_info(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        mlp_set_error_code(1);  // File not found
        return NULL;
    }

    FileInfo* info = mlp_malloc(sizeof(FileInfo));
    if (!info) {
        mlp_set_error_code(4);
        return NULL;
    }

    info->size = st.st_size;
    info->modified_time = st.st_mtime;
    info->created_time = st.st_ctime;
    info->permissions = st.st_mode & 0777;
    info->is_directory = S_ISDIR(st.st_mode) ? 1 : 0;
    info->is_readable = access(path, R_OK) == 0 ? 1 : 0;
    info->is_writable = access(path, W_OK) == 0 ? 1 : 0;

    mlp_set_error_code(0);
    return info;
}

// copy_file - Copy file with chunked transfer
int64_t mlp_copy_file(const char* source, const char* dest) {
    FILE* src = fopen(source, "rb");
    if (!src) {
        mlp_set_error_code(1);  // Source not found
        return -1;
    }

    FILE* dst = fopen(dest, "wb");
    if (!dst) {
        fclose(src);
        mlp_set_error_code(3);  // Cannot create destination
        return -1;
    }

    char buffer[4096];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            mlp_set_error_code(4);  // Write error
            return -1;
        }
    }

    fclose(src);
    fclose(dst);
    mlp_set_error_code(0);
    return 0;
}
```

### 2.2 Compiler Integration

**Lexer Tokens**:
```c
TOKEN_READ_BINARY,      // read_binary
TOKEN_WRITE_BINARY,     // write_binary
TOKEN_GET_FILE_INFO,    // get_file_info
TOKEN_COPY_FILE,        // copy_file
```

**Parser Recognition**:
- All functions recognized as built-in calls
- Type checking for parameters
- Return type inference

**Code Generation**:
```asm
; read_binary(path)
mov rdi, [path_ptr]      ; path argument
lea rsi, [size_var]      ; out_size pointer
call mlp_read_binary
; result in rax

; write_binary(path, data, size)
mov rdi, [path_ptr]      ; path
mov rsi, [data_ptr]      ; data
mov rdx, [size_val]      ; size
call mlp_write_binary
; result in rax
```

---

## 3. Use Cases

### 3.1 Compiler Object File Handling

```mlp
func compile_to_object(source_path, object_path)
    -- Read source
    string source = read_file(source_path)
    if string_length(source) == 0 then
        panic("Cannot read source file")
    end

    -- Compile to binary (simplified)
    int code_size = 0
    int machine_code = compile(source)

    -- Write object file
    int result = write_binary(object_path, machine_code, code_size)
    if result != 0 then
        panic("Cannot write object file")
    end

    mlp_free(machine_code)
    return 1
end func
```

### 3.2 Build System File Comparison

```mlp
func needs_rebuild(source, object)
    int src_info = get_file_info(source)
    int obj_info = get_file_info(object)

    -- Object doesn't exist, needs build
    if obj_info == 0 then
        mlp_free(src_info)
        return 1
    end

    -- Compare modification times
    int src_mtime = file_info_modified(src_info)
    int obj_mtime = file_info_modified(obj_info)

    mlp_free(src_info)
    mlp_free(obj_info)

    -- Source newer than object
    if src_mtime > obj_mtime then
        return 1
    end

    return 0
end func
```

### 3.3 Binary File Processing

```mlp
func process_executable(path)
    int data = read_binary(path)
    if data == 0 then
        print "Cannot read executable"
        return 0
    end

    int size = get_file_size(path)

    -- Check ELF magic number (simplified)
    -- In real code, would use byte access functions
    print "Processing executable: "
    print path
    print " ("
    print size
    print " bytes)"

    mlp_free(data)
    return 1
end func
```

---

## 4. Error Handling Patterns

### 4.1 Robust File Reading

```mlp
func safe_read_binary(path)
    -- Check file exists first
    if file_exists(path) == 0 then
        set_error_code(1)
        return 0
    end

    -- Check readable
    int info = get_file_info(path)
    if info == 0 then
        set_error_code(1)
        return 0
    end

    if file_info_is_readable(info) == 0 then
        mlp_free(info)
        set_error_code(2)
        return 0
    end

    int size = file_info_size(info)
    mlp_free(info)

    -- Check reasonable size (< 100MB)
    if size > 104857600 then
        set_error_code(5)  -- File too large
        return 0
    end

    -- Read file
    int data = read_binary(path)
    return data
end func
```

### 4.2 Atomic Write Pattern

```mlp
func atomic_write(path, data, size)
    -- Write to temp file first
    string temp_path = string_concat(path, ".tmp")

    int result = write_binary(temp_path, data, size)
    if result != 0 then
        return -1
    end

    -- TODO: Rename temp to target (needs rename function)
    -- For now, copy and delete
    result = copy_file(temp_path, path)
    -- delete_file(temp_path)  -- Needs Phase 5.3 completion

    return result
end func
```

---

## 5. Performance Considerations

### 5.1 Buffer Sizes
- Default chunk size: 4KB (optimal for most filesystems)
- Large file threshold: 1MB (consider streaming)
- Memory limit: Check available memory before large reads

### 5.2 Optimization Tips
- Use `get_file_info()` to check size before reading
- Reuse buffers when processing multiple files
- Consider memory-mapped I/O for very large files (future)

### 5.3 Benchmarks (Expected)
- Read 1MB file: < 10ms
- Write 1MB file: < 15ms
- Copy 1MB file: < 25ms
- Get file info: < 1ms

---

## 6. Security Considerations

### 6.1 Path Validation
- No `..` components allowed (prevent directory traversal)
- No null bytes in paths
- Validate path length (< 4096 characters)

### 6.2 Permissions
- Respect file system permissions
- Don't follow symlinks to different directories
- Check ownership for sensitive operations

### 6.3 Resource Limits
- Maximum file size for read_binary: 100MB default
- Timeout for long operations: configurable
- Memory allocation limits: checked

---

## 7. Testing

### 7.1 Unit Tests

```mlp
func test_read_write_binary()
    -- Create test data
    int data = mlp_malloc(256)
    -- Fill with test pattern

    -- Write
    int result = write_binary("test.bin", data, 256)
    assert(result == 0, "write_binary failed")

    -- Read back
    int read_data = read_binary("test.bin")
    assert(read_data != 0, "read_binary failed")

    -- Verify (simplified)
    int size = get_file_size("test.bin")
    assert(size == 256, "Size mismatch")

    mlp_free(data)
    mlp_free(read_data)
    print "test_read_write_binary: PASSED"
end func

func test_get_file_info()
    -- Create test file
    write_file("test_info.txt", "Hello World")

    int info = get_file_info("test_info.txt")
    assert(info != 0, "get_file_info failed")

    int size = file_info_size(info)
    assert(size == 11, "Size should be 11")

    int readable = file_info_is_readable(info)
    assert(readable == 1, "Should be readable")

    mlp_free(info)
    print "test_get_file_info: PASSED"
end func

func test_copy_file()
    write_file("source.txt", "Copy test content")

    int result = copy_file("source.txt", "dest.txt")
    assert(result == 0, "copy_file failed")

    string content = read_file("dest.txt")
    assert(string_length(content) == 17, "Content length mismatch")

    print "test_copy_file: PASSED"
end func
```

### 7.2 Integration Tests

- Test with actual binary files (executables, images)
- Test with large files (10MB+)
- Test error conditions (missing files, permissions)
- Test concurrent access (future)

---

## 8. Future Enhancements

- Memory-mapped file I/O for large files
- Async I/O with callbacks
- File locking (shared/exclusive)
- Sparse file support
- Extended attributes access
- File change watching/notifications

---

## 9. Examples

See:
- `examples/file_manager.mlp` - File manager utility
- `examples/build_system.mlp` - Simple build system
- `test/test_binary_io.mlp` - Test suite

---

## 10. API Summary

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `read_binary` | path | pointer | Read file as bytes |
| `write_binary` | path, data, size | int | Write bytes to file |
| `get_file_info` | path | FileInfo* | Get file metadata |
| `copy_file` | source, dest | int | Copy file |

---

**Status**: Complete
**Next**: Implement in runtime.c
