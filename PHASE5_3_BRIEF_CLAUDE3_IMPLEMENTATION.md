# Phase 5.3 - Claude #3 Implementation Brief

**Role**: Implementation Lead & Testing Coordinator  
**Branch**: `claude3`  
**Timeline**: ~9 days  
**Deliverables**: ~860 lines of code + tests

---

## 🎯 Your Mission

Implement 22 new system utility functions in runtime.c and integrate them into the compiler. These functions are critical for building practical applications and preparing for self-hosting.

---

## 📦 Implementation Tasks

### Task 1: Binary File I/O (4 functions)

#### 1.1 read_binary()
```c
// runtime.c
void* mlp_read_binary(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        mlp_set_error_code(ERR_FILE_NOT_FOUND);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    void* buffer = mlp_malloc(size);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    // Read data
    size_t bytes_read = fread(buffer, 1, size, file);
    fclose(file);
    
    if (bytes_read != size) {
        mlp_free(buffer);
        mlp_set_error_code(ERR_IO_ERROR);
        return NULL;
    }
    
    return buffer;
}
```

**Requirements**:
- Handle files up to 100MB efficiently
- Set error codes: ERR_FILE_NOT_FOUND, ERR_PERMISSION_DENIED, ERR_OUT_OF_MEMORY
- Use mlp_malloc for tracking
- Close file on all error paths

#### 1.2 write_binary()
```c
int mlp_write_binary(const char* path, const void* data, size_t size) {
    FILE* file = fopen(path, "wb");
    if (!file) {
        mlp_set_error_code(ERR_PERMISSION_DENIED);
        return 0;
    }
    
    size_t bytes_written = fwrite(data, 1, size, file);
    fclose(file);
    
    if (bytes_written != size) {
        mlp_set_error_code(ERR_IO_ERROR);
        return 0;
    }
    
    return 1; // Success
}
```

#### 1.3 get_file_info()
```c
typedef struct {
    long size;
    time_t modified_time;
    int is_directory;
    int is_readable;
    int is_writable;
} FileInfo;

FileInfo* mlp_get_file_info(const char* path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        mlp_set_error_code(ERR_FILE_NOT_FOUND);
        return NULL;
    }
    
    FileInfo* info = mlp_malloc(sizeof(FileInfo));
    info->size = st.st_size;
    info->modified_time = st.st_mtime;
    info->is_directory = S_ISDIR(st.st_mode);
    info->is_readable = (st.st_mode & S_IRUSR) != 0;
    info->is_writable = (st.st_mode & S_IWUSR) != 0;
    
    return info;
}
```

**Note**: Define FileInfo struct in runtime.h

#### 1.4 copy_file()
```c
int mlp_copy_file(const char* source, const char* dest) {
    FILE* src = fopen(source, "rb");
    if (!src) {
        mlp_set_error_code(ERR_FILE_NOT_FOUND);
        return 0;
    }
    
    FILE* dst = fopen(dest, "wb");
    if (!dst) {
        fclose(src);
        mlp_set_error_code(ERR_PERMISSION_DENIED);
        return 0;
    }
    
    // Copy in 4KB chunks
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            mlp_set_error_code(ERR_IO_ERROR);
            return 0;
        }
    }
    
    fclose(src);
    fclose(dst);
    return 1;
}
```

---

### Task 2: Directory Operations (6 functions)

#### 2.1 list_directory()
```c
char** mlp_list_directory(const char* path, int* count) {
    DIR* dir = opendir(path);
    if (!dir) {
        mlp_set_error_code(ERR_FILE_NOT_FOUND);
        *count = 0;
        return NULL;
    }
    
    // Count entries first
    int entry_count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip "." and ".."
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            entry_count++;
        }
    }
    
    // Allocate array
    char** files = mlp_malloc(sizeof(char*) * entry_count);
    
    // Read entries again
    rewinddir(dir);
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            files[i] = mlp_malloc(strlen(entry->d_name) + 1);
            strcpy(files[i], entry->d_name);
            i++;
        }
    }
    
    closedir(dir);
    *count = entry_count;
    return files;
}
```

**Requirements**:
- Skip "." and ".." entries
- Sort alphabetically (use qsort)
- Handle empty directories
- Free array and strings on errors

#### 2.2 create_directory()
```c
int mlp_create_directory(const char* path) {
    if (mkdir(path, 0755) != 0) {
        if (errno == EEXIST) {
            mlp_set_error_code(ERR_FILE_EXISTS);
        } else {
            mlp_set_error_code(ERR_PERMISSION_DENIED);
        }
        return 0;
    }
    return 1;
}
```

#### 2.3 remove_directory()
```c
int mlp_remove_directory(const char* path) {
    if (rmdir(path) != 0) {
        if (errno == ENOTEMPTY) {
            mlp_set_error_code(ERR_DIRECTORY_NOT_EMPTY);
        } else {
            mlp_set_error_code(ERR_PERMISSION_DENIED);
        }
        return 0;
    }
    return 1;
}
```

#### 2.4-2.6: Simple wrappers
```c
int mlp_directory_exists(const char* path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

char* mlp_get_current_dir(void) {
    char* buffer = mlp_malloc(PATH_MAX);
    if (getcwd(buffer, PATH_MAX) == NULL) {
        mlp_free(buffer);
        return NULL;
    }
    return buffer;
}

int mlp_change_directory(const char* path) {
    return (chdir(path) == 0) ? 1 : 0;
}
```

**Headers needed**: `<dirent.h>`, `<sys/stat.h>`, `<unistd.h>`

---

### Task 3: Process Control (4 functions)

#### 3.1 execute_command()
```c
int mlp_execute_command(const char* cmd) {
    int exit_code = system(cmd);
    if (exit_code == -1) {
        mlp_set_error_code(ERR_COMMAND_FAILED);
        return -1;
    }
    return WEXITSTATUS(exit_code);
}
```

#### 3.2 get_command_output()
```c
char* mlp_get_command_output(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        mlp_set_error_code(ERR_COMMAND_FAILED);
        return NULL;
    }
    
    // Read output in chunks
    size_t buffer_size = 1024;
    size_t output_size = 0;
    char* output = mlp_malloc(buffer_size);
    
    while (fgets(output + output_size, buffer_size - output_size, pipe) != NULL) {
        output_size = strlen(output);
        if (output_size >= buffer_size - 1) {
            buffer_size *= 2;
            output = mlp_realloc(output, buffer_size);
        }
    }
    
    pclose(pipe);
    return output;
}
```

#### 3.3-3.4: Simple wrappers
```c
int mlp_get_process_id(void) {
    return getpid();
}

int mlp_get_parent_process_id(void) {
    return getppid();
}
```

**Security Note**: Document command injection risks in comments

---

### Task 4: Time & Date (4 functions)

#### 4.1 format_timestamp()
```c
char* mlp_format_timestamp(time_t timestamp, const char* format) {
    char* buffer = mlp_malloc(256);
    struct tm* tm_info = localtime(&timestamp);
    
    if (strftime(buffer, 256, format, tm_info) == 0) {
        mlp_free(buffer);
        mlp_set_error_code(ERR_INVALID_FORMAT);
        return NULL;
    }
    
    return buffer;
}
```

#### 4.2 parse_timestamp()
```c
time_t mlp_parse_timestamp(const char* date_string) {
    struct tm tm = {0};
    
    // Try ISO 8601 format: YYYY-MM-DD HH:MM:SS
    if (strptime(date_string, "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
        return mktime(&tm);
    }
    
    // Try date only: YYYY-MM-DD
    if (strptime(date_string, "%Y-%m-%d", &tm) != NULL) {
        return mktime(&tm);
    }
    
    mlp_set_error_code(ERR_INVALID_FORMAT);
    return -1;
}
```

#### 4.3 get_milliseconds()
```c
long long mlp_get_milliseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}
```

#### 4.4 get_time_string()
```c
char* mlp_get_time_string(void) {
    time_t now = time(NULL);
    return mlp_format_timestamp(now, "%Y-%m-%d %H:%M:%S");
}
```

**Headers needed**: `<time.h>`

---

### Task 5: Path Utilities (4 functions)

#### 5.1 join_path()
```c
char* mlp_join_path(char** parts, int count) {
    if (count == 0) return mlp_malloc(1); // Empty string
    
    // Calculate total length
    size_t total_len = 0;
    for (int i = 0; i < count; i++) {
        total_len += strlen(parts[i]);
        if (i < count - 1) total_len++; // For '/' separator
    }
    
    char* result = mlp_malloc(total_len + 1);
    result[0] = '\0';
    
    for (int i = 0; i < count; i++) {
        strcat(result, parts[i]);
        if (i < count - 1) strcat(result, "/");
    }
    
    return result;
}
```

#### 5.2 get_file_extension()
```c
char* mlp_get_file_extension(const char* path) {
    const char* dot = strrchr(path, '.');
    const char* slash = strrchr(path, '/');
    
    // No extension or dot is in directory name
    if (!dot || (slash && dot < slash)) {
        return mlp_malloc(1); // Empty string
    }
    
    // Copy extension (without dot)
    char* ext = mlp_malloc(strlen(dot));
    strcpy(ext, dot + 1);
    return ext;
}
```

#### 5.3 get_file_name()
```c
char* mlp_get_file_name(const char* path) {
    const char* slash = strrchr(path, '/');
    const char* filename = slash ? slash + 1 : path;
    
    char* result = mlp_malloc(strlen(filename) + 1);
    strcpy(result, filename);
    return result;
}
```

#### 5.4 get_directory()
```c
char* mlp_get_directory(const char* path) {
    const char* slash = strrchr(path, '/');
    
    if (!slash) {
        // No directory, return "."
        char* result = mlp_malloc(2);
        strcpy(result, ".");
        return result;
    }
    
    size_t dir_len = slash - path;
    char* result = mlp_malloc(dir_len + 1);
    strncpy(result, path, dir_len);
    result[dir_len] = '\0';
    return result;
}
```

---

## 🔧 Compiler Integration

### Step 1: Add Tokens (c_lexer.h)

```c
// Add to TokenType enum
TOKEN_BUILTIN_READ_BINARY,
TOKEN_BUILTIN_WRITE_BINARY,
TOKEN_BUILTIN_GET_FILE_INFO,
TOKEN_BUILTIN_COPY_FILE,

TOKEN_BUILTIN_LIST_DIRECTORY,
TOKEN_BUILTIN_CREATE_DIRECTORY,
TOKEN_BUILTIN_REMOVE_DIRECTORY,
TOKEN_BUILTIN_DIRECTORY_EXISTS,
TOKEN_BUILTIN_GET_CURRENT_DIR,
TOKEN_BUILTIN_CHANGE_DIRECTORY,

TOKEN_BUILTIN_EXECUTE_COMMAND,
TOKEN_BUILTIN_GET_COMMAND_OUTPUT,
TOKEN_BUILTIN_GET_PROCESS_ID,
TOKEN_BUILTIN_GET_PARENT_PROCESS_ID,

TOKEN_BUILTIN_FORMAT_TIMESTAMP,
TOKEN_BUILTIN_PARSE_TIMESTAMP,
TOKEN_BUILTIN_GET_MILLISECONDS,
TOKEN_BUILTIN_GET_TIME_STRING,

TOKEN_BUILTIN_JOIN_PATH,
TOKEN_BUILTIN_GET_FILE_EXTENSION,
TOKEN_BUILTIN_GET_FILE_NAME,
TOKEN_BUILTIN_GET_DIRECTORY,
```

### Step 2: Update Keyword Map (c_lexer.c)

```c
{"read_binary", TOKEN_BUILTIN_READ_BINARY},
{"write_binary", TOKEN_BUILTIN_WRITE_BINARY},
// ... add all 22 functions
```

### Step 3: Parser Support (c_parser.c)

In `parse_builtin_call()`, add cases for all new tokens:

```c
case TOKEN_BUILTIN_READ_BINARY:
case TOKEN_BUILTIN_WRITE_BINARY:
case TOKEN_BUILTIN_GET_FILE_INFO:
// ... etc
    node->builtin_call_data.function_type = current_token.type;
    break;
```

### Step 4: Code Generation (c_generator.c)

In `visit_BuiltinCall()`, add generation for each function:

```c
case TOKEN_BUILTIN_READ_BINARY:
    fprintf(output, "mlp_read_binary(");
    visit_node(node->builtin_call_data.arguman_listesi->ifade_listesi_data.ifadeler[0]);
    fprintf(output, ")");
    break;

case TOKEN_BUILTIN_LIST_DIRECTORY:
    fprintf(output, "mlp_list_directory(");
    visit_node(node->builtin_call_data.arguman_listesi->ifade_listesi_data.ifadeler[0]);
    fprintf(output, ", &mlp_temp_count)"); // For count output
    break;

// ... continue for all functions
```

**Special Handling**:
- `list_directory()` needs count output parameter
- `write_binary()` has 3 parameters
- `join_path()` needs array and count

---

## 🧪 Testing Strategy

### Unit Tests (Individual Function Tests)

Create `test/test_binary_io.mlp`:
```mlp
# Test read_binary
function test_read_binary() {
    # Create test file
    write_binary("test.bin", "Hello", 5)
    
    # Read it back
    data := read_binary("test.bin")
    assert(data != null, "read_binary should succeed")
    
    # Clean up
    execute_command("rm test.bin")
    print "✓ read_binary test passed"
}

test_read_binary()
```

Create similar files for:
- `test_directory_ops.mlp` (~120 lines)
- `test_process_control.mlp` (~80 lines)
- `test_time_utils.mlp` (~100 lines)

### Integration Test

Create `test/test_phase5_3_integration.mlp`:
```mlp
print "=== Phase 5.3 Integration Test ==="

# 1. Binary File I/O
print "\n1. Testing Binary File I/O..."
data := "Test binary data"
success := write_binary("test.bin", data, 16)
assert(success, "write_binary failed")

read_data := read_binary("test.bin")
assert(read_data != null, "read_binary failed")

info := get_file_info("test.bin")
assert(info.size == 16, "file size mismatch")

copy_file("test.bin", "test_copy.bin")
print "✓ Binary I/O: 4/4 functions working"

# 2. Directory Operations
print "\n2. Testing Directory Operations..."
create_directory("test_dir")
assert(directory_exists("test_dir"), "directory not created")

files := list_directory(".")
assert(files != null, "list_directory failed")

current := get_current_dir()
assert(current != null, "get_current_dir failed")

remove_directory("test_dir")
print "✓ Directory Ops: 6/6 functions working"

# 3. Process Control
print "\n3. Testing Process Control..."
exit_code := execute_command("echo test")
assert(exit_code == 0, "execute_command failed")

output := get_command_output("echo hello")
assert(output != null, "get_command_output failed")

pid := get_process_id()
assert(pid > 0, "get_process_id failed")

ppid := get_parent_process_id()
assert(ppid > 0, "get_parent_process_id failed")
print "✓ Process Control: 4/4 functions working"

# 4. Time Utilities
print "\n4. Testing Time Utilities..."
timestamp := 1700000000
formatted := format_timestamp(timestamp, "%Y-%m-%d")
assert(formatted != null, "format_timestamp failed")

parsed := parse_timestamp("2025-11-21 14:30:00")
assert(parsed > 0, "parse_timestamp failed")

ms := get_milliseconds()
assert(ms > 0, "get_milliseconds failed")

time_str := get_time_string()
assert(time_str != null, "get_time_string failed")
print "✓ Time Utilities: 4/4 functions working"

# 5. Path Utilities
print "\n5. Testing Path Utilities..."
parts := ["home", "user", "file.txt"]
path := join_path(parts, 3)
assert(path == "home/user/file.txt", "join_path failed")

ext := get_file_extension("test.mlp")
assert(ext == "mlp", "get_file_extension failed")

filename := get_file_name("/path/to/file.txt")
assert(filename == "file.txt", "get_file_name failed")

dir := get_directory("/path/to/file.txt")
assert(dir == "/path/to", "get_directory failed")
print "✓ Path Utilities: 4/4 functions working"

# Memory check
leaks := check_memory_leaks()
assert(leaks == 0, "Memory leaks detected")

print "\n=== Phase 5.3: 22/22 Functions PASSING ✅ ==="
```

---

## 📊 Error Codes

Define in `runtime.h`:
```c
#define ERR_FILE_NOT_FOUND      101
#define ERR_PERMISSION_DENIED   102
#define ERR_IO_ERROR           103
#define ERR_OUT_OF_MEMORY      104
#define ERR_INVALID_FORMAT     105
#define ERR_COMMAND_FAILED     106
#define ERR_DIRECTORY_NOT_EMPTY 107
#define ERR_FILE_EXISTS        108
```

---

## ✅ Completion Checklist

- [ ] All 22 functions implemented in runtime.c
- [ ] FileInfo struct defined in runtime.h
- [ ] All functions use mlp_malloc/mlp_free for tracking
- [ ] Error codes set for all failure cases
- [ ] All file handles closed on error paths
- [ ] 22 new tokens added to c_lexer.h
- [ ] Keyword map updated in c_lexer.c
- [ ] Parser support added in c_parser.c
- [ ] Code generation added in c_generator.c
- [ ] Unit tests created (4 test files)
- [ ] Integration test created and passing
- [ ] Zero memory leaks detected
- [ ] All tests pass on Linux
- [ ] Code committed to claude3 branch

---

## 🚀 Getting Started

1. Read PHASE5_3_COORDINATION.md
2. Review Phase 5.2 runtime.c for code style
3. Create branch: `git checkout -b claude3`
4. Start with Binary File I/O (most critical)
5. Test each function immediately after implementation
6. Commit progress daily with descriptive messages

---

**Created**: November 21, 2025  
**Status**: ✅ READY TO START  
**Owner**: Claude #3 (Implementation Lead)

Let's build! 💪
