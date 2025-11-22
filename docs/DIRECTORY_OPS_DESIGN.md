# Directory Operations - Design Specification

**Status**: Phase 5.3 - In Progress
**Version**: 1.0
**Last Updated**: November 21, 2025

---

## Overview

This document specifies directory operation functions for MLP, enabling programs to navigate, create, and manage directories in the filesystem.

### Goals
- List directory contents
- Create and remove directories
- Check directory existence
- Navigate filesystem (get/change current directory)

### Non-Goals
- Recursive directory operations - future work
- Directory watching/notifications - future work
- Extended attributes - future work

---

## 1. Directory Operation Functions

### 1.1 list_directory()

**Purpose**: List files and subdirectories in a directory

**Signature**:
```mlp
func list_directory(path)
    -- Returns list of directory entries
end func
```

**Parameters**:
- `path` (string): Path to directory

**Return Value**:
- `list[string]` containing filenames
- Empty list if directory is empty or doesn't exist

**Behavior**:
- Opens directory with opendir()
- Reads all entries with readdir()
- Skips "." and ".." entries
- Returns sorted list of names
- Closes directory

**Example**:
```mlp
func show_directory_contents(path)
    list[string] entries = list_directory(path)

    if entries.size() == 0 then
        print "Directory is empty or doesn't exist"
        return 0
    end

    print "Contents of "
    print path
    print ":"

    int i = 0
    while i < entries.size()
        string name = entries.get(i)
        print "  "
        print name
        i = i + 1
    end

    list_destroy(entries)
    return 1
end func
```

**Error Handling**:
- Returns empty list if directory doesn't exist
- Returns empty list if permission denied
- Sets error code for specific failure

**Error Codes**:
- `0` - Success
- `1` - Directory not found
- `2` - Permission denied
- `3` - Not a directory

---

### 1.2 create_directory()

**Purpose**: Create a new directory

**Signature**:
```mlp
func create_directory(path)
    -- Creates directory, returns 0 on success
end func
```

**Parameters**:
- `path` (string): Path to directory to create

**Return Value**:
- `0` on success
- `-1` on error

**Behavior**:
- Creates directory with mode 0755 (rwxr-xr-x)
- Parent directory must exist
- Does not create parent directories (use create_directory_recursive for that)

**Example**:
```mlp
func setup_build_directory()
    int result = create_directory("build")
    if result != 0 then
        int error = get_error_code()
        if error == 4 then
            print "Build directory already exists"
            return 1  -- Not an error
        end
        print "Error: Cannot create build directory"
        return 0
    end

    print "Created build directory"
    return 1
end func
```

**Error Codes**:
- `0` - Success
- `1` - Parent directory not found
- `2` - Permission denied
- `3` - Invalid path
- `4` - Directory already exists

---

### 1.3 remove_directory()

**Purpose**: Remove an empty directory

**Signature**:
```mlp
func remove_directory(path)
    -- Removes empty directory, returns 0 on success
end func
```

**Parameters**:
- `path` (string): Path to directory to remove

**Return Value**:
- `0` on success
- `-1` on error

**Behavior**:
- Only removes empty directories
- Fails if directory contains files or subdirectories
- Does not follow symlinks

**Example**:
```mlp
func cleanup_temp_directory()
    -- First, list and delete files (if needed)
    list[string] files = list_directory("temp")

    if files.size() > 0 then
        print "Error: Directory not empty"
        list_destroy(files)
        return 0
    end
    list_destroy(files)

    int result = remove_directory("temp")
    if result != 0 then
        print "Error: Cannot remove directory"
        return 0
    end

    print "Temp directory removed"
    return 1
end func
```

**Error Codes**:
- `0` - Success
- `1` - Directory not found
- `2` - Permission denied
- `3` - Directory not empty
- `4` - Not a directory

---

### 1.4 directory_exists()

**Purpose**: Check if directory exists

**Signature**:
```mlp
func directory_exists(path)
    -- Returns 1 if exists and is directory, 0 otherwise
end func
```

**Parameters**:
- `path` (string): Path to check

**Return Value**:
- `1` if path exists and is a directory
- `0` otherwise

**Behavior**:
- Uses stat() to check path
- Verifies it's a directory (not a file)
- Does not follow symlinks by default

**Example**:
```mlp
func ensure_output_directory(path)
    if directory_exists(path) == 1 then
        return 1  -- Already exists
    end

    -- Try to create it
    int result = create_directory(path)
    if result != 0 then
        print "Error: Cannot create output directory"
        return 0
    end

    return 1
end func
```

---

### 1.5 get_current_dir()

**Purpose**: Get current working directory

**Signature**:
```mlp
func get_current_dir()
    -- Returns current working directory path
end func
```

**Parameters**: None

**Return Value**:
- String containing absolute path to current directory
- Empty string on error

**Behavior**:
- Uses getcwd() to get current directory
- Returns absolute path
- Path does not end with separator

**Example**:
```mlp
func show_current_directory()
    string cwd = get_current_dir()

    if string_length(cwd) == 0 then
        print "Error: Cannot get current directory"
        return 0
    end

    print "Current directory: "
    print cwd
    return 1
end func
```

---

### 1.6 change_directory()

**Purpose**: Change current working directory

**Signature**:
```mlp
func change_directory(path)
    -- Changes to directory, returns 0 on success
end func
```

**Parameters**:
- `path` (string): Path to change to

**Return Value**:
- `0` on success
- `-1` on error

**Behavior**:
- Uses chdir() to change directory
- Accepts relative or absolute paths
- Updates process working directory

**Example**:
```mlp
func build_in_directory(build_dir)
    -- Save current directory
    string original_dir = get_current_dir()

    -- Change to build directory
    int result = change_directory(build_dir)
    if result != 0 then
        print "Error: Cannot change to build directory"
        return 0
    end

    -- Do build work here
    print "Building in: "
    print get_current_dir()

    -- Return to original directory
    change_directory(original_dir)
    return 1
end func
```

**Error Codes**:
- `0` - Success
- `1` - Directory not found
- `2` - Permission denied
- `3` - Not a directory

---

## 2. Implementation Details

### 2.1 Runtime Implementation (C)

```c
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

// list_directory - List directory contents
List* mlp_list_directory(const char* path) {
    List* list = mlp_list_create();

    DIR* dir = opendir(path);
    if (!dir) {
        mlp_set_error_code(1);  // Directory not found
        return list;  // Return empty list
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Add to list (duplicate string)
        char* name = strdup(entry->d_name);
        mlp_list_add(list, (int64_t)name);
    }

    closedir(dir);
    mlp_set_error_code(0);
    return list;
}

// create_directory - Create new directory
int64_t mlp_create_directory(const char* path) {
    // Check if already exists
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            mlp_set_error_code(4);  // Already exists
            return -1;
        }
    }

    // Create with mode 0755
    if (mkdir(path, 0755) != 0) {
        if (errno == ENOENT) {
            mlp_set_error_code(1);  // Parent not found
        } else if (errno == EACCES) {
            mlp_set_error_code(2);  // Permission denied
        } else {
            mlp_set_error_code(3);  // Invalid path
        }
        return -1;
    }

    mlp_set_error_code(0);
    return 0;
}

// remove_directory - Remove empty directory
int64_t mlp_remove_directory(const char* path) {
    if (rmdir(path) != 0) {
        if (errno == ENOENT) {
            mlp_set_error_code(1);  // Not found
        } else if (errno == EACCES) {
            mlp_set_error_code(2);  // Permission denied
        } else if (errno == ENOTEMPTY) {
            mlp_set_error_code(3);  // Not empty
        } else if (errno == ENOTDIR) {
            mlp_set_error_code(4);  // Not a directory
        }
        return -1;
    }

    mlp_set_error_code(0);
    return 0;
}

// directory_exists - Check if directory exists
int64_t mlp_directory_exists(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }
    return S_ISDIR(st.st_mode) ? 1 : 0;
}

// get_current_dir - Get current working directory
const char* mlp_get_current_dir() {
    static char buffer[4096];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        mlp_set_error_code(1);
        return "";
    }
    mlp_set_error_code(0);
    return buffer;
}

// change_directory - Change working directory
int64_t mlp_change_directory(const char* path) {
    if (chdir(path) != 0) {
        if (errno == ENOENT) {
            mlp_set_error_code(1);  // Not found
        } else if (errno == EACCES) {
            mlp_set_error_code(2);  // Permission denied
        } else if (errno == ENOTDIR) {
            mlp_set_error_code(3);  // Not a directory
        }
        return -1;
    }

    mlp_set_error_code(0);
    return 0;
}
```

### 2.2 Compiler Integration

**Lexer Tokens**:
```c
TOKEN_LIST_DIRECTORY,     // list_directory
TOKEN_CREATE_DIRECTORY,   // create_directory
TOKEN_REMOVE_DIRECTORY,   // remove_directory
TOKEN_DIRECTORY_EXISTS,   // directory_exists
TOKEN_GET_CURRENT_DIR,    // get_current_dir
TOKEN_CHANGE_DIRECTORY,   // change_directory
```

**Code Generation**:
```asm
; list_directory(path)
mov rdi, [path_ptr]
call mlp_list_directory
; result (list pointer) in rax

; create_directory(path)
mov rdi, [path_ptr]
call mlp_create_directory
; result in rax

; get_current_dir()
call mlp_get_current_dir
; result (string pointer) in rax
```

---

## 3. Use Cases

### 3.1 Source Tree Traversal

```mlp
func find_source_files(dir)
    list[string] sources = list()
    list[string] entries = list_directory(dir)

    int i = 0
    while i < entries.size()
        string name = entries.get(i)
        string full_path = join_path_2(dir, name)

        -- Check if it's a directory
        if directory_exists(full_path) == 1 then
            -- Recursively search subdirectory
            list[string] sub_sources = find_source_files(full_path)
            -- Add to sources (would need list_concat)
        else
            -- Check if .mlp file
            if string_ends_with(name, ".mlp") == 1 then
                sources.add(full_path)
            end
        end

        i = i + 1
    end

    list_destroy(entries)
    return sources
end func
```

### 3.2 Build System Output Directory

```mlp
func setup_build_output(config)
    -- Create main build directory
    if directory_exists("build") == 0 then
        create_directory("build")
    end

    -- Create config-specific subdirectory (debug/release)
    string config_dir = join_path_2("build", config)
    if directory_exists(config_dir) == 0 then
        create_directory(config_dir)
    end

    -- Create obj subdirectory
    string obj_dir = join_path_2(config_dir, "obj")
    if directory_exists(obj_dir) == 0 then
        create_directory(obj_dir)
    end

    print "Build output: "
    print config_dir
    return config_dir
end func
```

### 3.3 Temporary Directory Management

```mlp
func with_temp_directory()
    -- Create temp directory
    string temp_dir = "/tmp/mlp_build_12345"
    create_directory(temp_dir)

    -- Save current directory
    string original = get_current_dir()

    -- Work in temp directory
    change_directory(temp_dir)

    -- ... do work ...

    -- Return to original
    change_directory(original)

    -- Cleanup (would need recursive delete)
    -- For now, just remove if empty
    remove_directory(temp_dir)

    return 1
end func
```

---

## 4. Error Handling Patterns

### 4.1 Safe Directory Creation

```mlp
func ensure_directory_exists(path)
    -- Already exists?
    if directory_exists(path) == 1 then
        return 1
    end

    -- Try to create
    int result = create_directory(path)
    if result == 0 then
        return 1
    end

    -- Check error
    int error = get_error_code()
    if error == 4 then
        -- Already exists (race condition)
        return 1
    end

    -- Real error
    print "Error: Cannot create directory: "
    print path
    return 0
end func
```

### 4.2 Directory Stack Pattern

```mlp
-- Global directory stack (simplified)
list[string] dir_stack = list()

func push_directory(path)
    string current = get_current_dir()
    dir_stack.add(current)

    int result = change_directory(path)
    if result != 0 then
        -- Remove from stack on failure
        -- (would need list.pop())
        return 0
    end
    return 1
end func

func pop_directory()
    if dir_stack.size() == 0 then
        return 0
    end

    string prev = dir_stack.get(dir_stack.size() - 1)
    -- (would need list.remove_last())
    change_directory(prev)
    return 1
end func
```

---

## 5. Security Considerations

### 5.1 Path Validation
- No `..` components for security-sensitive operations
- Validate path length (< 4096 characters)
- No null bytes in paths

### 5.2 Symlink Handling
- `list_directory` includes symlinks in listing
- `directory_exists` follows symlinks by default
- Option for `lstat` behavior (future)

### 5.3 Permissions
- `create_directory` uses 0755 by default
- Respects umask
- Check parent directory permissions before operations

### 5.4 Race Conditions
- Between `directory_exists` and `create_directory`
- Use error codes to handle "already exists" gracefully
- Consider atomic operations (future)

---

## 6. Performance Considerations

### 6.1 Directory Listing
- Returns unsorted list by default
- Consider caching for frequently accessed directories
- Large directories (10000+ files) may be slow

### 6.2 Path Operations
- `change_directory` affects entire process
- `get_current_dir` allocates string each call
- Cache current directory if called frequently

### 6.3 Benchmarks (Expected)
- `list_directory` (100 files): < 5ms
- `create_directory`: < 2ms
- `directory_exists`: < 1ms
- `get_current_dir`: < 1ms

---

## 7. Testing

### 7.1 Unit Tests

```mlp
func test_create_remove_directory()
    -- Create
    int result = create_directory("test_dir")
    assert(result == 0, "create_directory failed")
    assert(directory_exists("test_dir") == 1, "Directory should exist")

    -- Remove
    result = remove_directory("test_dir")
    assert(result == 0, "remove_directory failed")
    assert(directory_exists("test_dir") == 0, "Directory should not exist")

    print "test_create_remove_directory: PASSED"
end func

func test_list_directory()
    -- Create test structure
    create_directory("test_list")
    write_file("test_list/file1.txt", "content1")
    write_file("test_list/file2.txt", "content2")

    -- List
    list[string] entries = list_directory("test_list")
    assert(entries.size() == 2, "Should have 2 entries")

    -- Cleanup
    -- (would need delete_file and remove_directory)

    list_destroy(entries)
    print "test_list_directory: PASSED"
end func

func test_current_directory()
    string original = get_current_dir()
    assert(string_length(original) > 0, "Should have current dir")

    -- Create and change to test dir
    create_directory("test_cd")
    int result = change_directory("test_cd")
    assert(result == 0, "change_directory failed")

    -- Verify
    string new_dir = get_current_dir()
    assert(string_ends_with(new_dir, "test_cd") == 1, "Should be in test_cd")

    -- Return
    change_directory(original)
    remove_directory("test_cd")

    print "test_current_directory: PASSED"
end func
```

### 7.2 Integration Tests

- Test with nested directories
- Test with many files (1000+)
- Test error conditions (permissions, non-existent)
- Test with special characters in names
- Test with unicode names

---

## 8. Future Enhancements

- Recursive directory creation (`mkdir -p`)
- Recursive directory deletion
- Directory watching (inotify)
- Extended attributes (xattr)
- Glob pattern matching
- Directory iteration (stream-based)
- Directory sorting options

---

## 9. Examples

See:
- `examples/file_manager.mlp` - Directory listing and navigation
- `examples/build_system.mlp` - Build directory management
- `test/test_directory_ops.mlp` - Test suite

---

## 10. API Summary

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `list_directory` | path | list[string] | List directory contents |
| `create_directory` | path | int | Create directory |
| `remove_directory` | path | int | Remove empty directory |
| `directory_exists` | path | int | Check if directory exists |
| `get_current_dir` | - | string | Get current working directory |
| `change_directory` | path | int | Change working directory |

---

**Status**: Complete
**Next**: Implement in runtime.c
