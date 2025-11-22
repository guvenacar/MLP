# Phase 5.3 - Claude #2 Documentation Brief

**Role**: Documentation & Examples Specialist  
**Branch**: `claude2`  
**Timeline**: ~7 days  
**Deliverables**: ~2,950 lines of documentation

---

## 🎯 Your Mission

Create comprehensive documentation for 22 new I/O and system utility functions. Your work will guide Claude #3's implementation and help users understand these critical features.

---

## 📦 Deliverables Overview

### 1. BINARY_IO_DESIGN.md (~400 lines)
Document binary file operations for reading/writing compiled files, object files, and binary data.

**Functions to Document**:
```mlp
read_binary(path: string) -> bytes
write_binary(path: string, data: bytes, size: int) -> bool
get_file_info(path: string) -> FileInfo
copy_file(source: string, dest: string) -> bool
```

**Required Sections**:
- **Overview**: Why binary I/O matters for compilers
- **API Reference**: Each function with parameters, return values, errors
- **FileInfo Structure**: Define fields (size, modified_time, permissions, is_directory)
- **Usage Examples**: 
  - Read a compiled binary
  - Write bytecode to file
  - Copy executable with permissions
  - Check file size before loading
- **Error Handling**: File not found, permission denied, disk full
- **Performance Notes**: Buffer sizes, memory usage for large files
- **Platform Notes**: Linux focus, endianness considerations

**Example Structure**:
```markdown
## read_binary()

### Signature
```mlp
read_binary(path: string) -> bytes
```

### Description
Reads entire file as binary data into byte array. Returns null on error.

### Parameters
- `path` (string): Absolute or relative file path

### Returns
- `bytes`: Binary data array, or null on error

### Error Codes
- `ERR_FILE_NOT_FOUND`: Path doesn't exist
- `ERR_PERMISSION_DENIED`: No read permission
- `ERR_OUT_OF_MEMORY`: File too large

### Example
```mlp
# Read compiled binary
data := read_binary("output.bin")
if data == null {
    print "Failed to read file"
    exit(1)
}
print "Read " + string(len(data)) + " bytes"
```

### Notes
- Reads entire file into memory - check size first for large files
- Binary data is platform-dependent (endianness)
- Use get_file_info() to check size before reading
```

---

### 2. DIRECTORY_OPS_DESIGN.md (~500 lines)
Document directory traversal and management operations.

**Functions to Document**:
```mlp
list_directory(path: string) -> string[]
create_directory(path: string) -> bool
remove_directory(path: string) -> bool
directory_exists(path: string) -> bool
get_current_dir() -> string
change_directory(path: string) -> bool
```

**Required Sections**:
- **Overview**: Directory operations for build systems
- **API Reference**: All 6 functions with full specs
- **Usage Examples**:
  - Traverse source tree
  - Create build output directories
  - Check if path is directory
  - Recursive directory listing
  - Safe directory removal
- **Error Handling**: Directory not empty, permission errors, path too long
- **Security**: Prevent directory traversal attacks
- **Platform Notes**: Path separators, symlink handling

**Special Focus**:
- `list_directory()` should document sorting behavior (alphabetical)
- Explain difference between relative and absolute paths
- Document "." and ".." entries handling

---

### 3. PROCESS_CONTROL_DESIGN.md (~450 lines)
Document process execution for running external tools (assembler, linker, etc).

**Functions to Document**:
```mlp
execute_command(cmd: string) -> int
get_command_output(cmd: string) -> string
get_process_id() -> int
get_parent_process_id() -> int
```

**Required Sections**:
- **Overview**: Why compilers need process control
- **API Reference**: All 4 functions
- **Usage Examples**:
  - Run assembler and check exit code
  - Capture compiler output
  - Execute linker with arguments
  - Build system orchestration
  - Parallel compilation (future)
- **Error Handling**: Command not found, timeout, signal handling
- **Security**: Command injection prevention, argument escaping
- **Performance**: Async execution (future consideration)

**Critical**: Document security best practices for user input in commands.

---

### 4. TIME_UTILITIES_DESIGN.md (~350 lines)
Document time/date operations for timestamps, profiling, and logging.

**Functions to Document**:
```mlp
format_timestamp(timestamp: int, format: string) -> string
parse_timestamp(date_string: string) -> int
get_milliseconds() -> int
get_time_string() -> string
```

**Required Sections**:
- **Overview**: Time utilities for build systems and profiling
- **API Reference**: All 4 functions
- **Format Specifiers**: Document strftime format codes
  - `%Y-%m-%d %H:%M:%S` (ISO 8601)
  - `%Y/%m/%d` (Date only)
  - `%H:%M:%S` (Time only)
- **Usage Examples**:
  - Build timestamp in output binary
  - Profiling compilation time
  - Log file with timestamps
  - Parse date from user input
- **Precision**: Millisecond vs second precision
- **Timezone**: UTC vs local time handling

---

### 5. PATH_UTILITIES_DESIGN.md (~300 lines)
Document path manipulation helper functions.

**Functions to Document**:
```mlp
join_path(parts: string[], count: int) -> string
get_file_extension(path: string) -> string
get_file_name(path: string) -> string
get_directory(path: string) -> string
```

**Required Sections**:
- **Overview**: Path utilities for cross-platform compatibility
- **API Reference**: All 4 functions
- **Usage Examples**:
  - Build output paths
  - Filter files by extension
  - Extract filename from path
  - Navigate directory tree
- **Edge Cases**:
  - Empty paths
  - Paths with multiple extensions (.tar.gz)
  - Root directory handling
  - Relative vs absolute paths
- **Platform Notes**: Linux focus, forward slash separators

---

### 6. Example Programs

#### file_manager.mlp (~250 lines)
A practical file manager demonstrating file/directory operations.

**Features**:
- List directory contents with file info
- Copy/move files
- Create/remove directories
- Display file sizes and timestamps
- Search files by extension

**Code Structure**:
```mlp
# Main menu
function show_menu() {
    print "1. List directory"
    print "2. Copy file"
    print "3. Create directory"
    print "4. File info"
    print "5. Exit"
}

# List with details
function list_with_info(path: string) {
    files := list_directory(path)
    for file in files {
        info := get_file_info(join_path([path, file], 2))
        print file + " - " + string(info.size) + " bytes"
    }
}
```

---

#### build_system.mlp (~350 lines)
A simple build system demonstrating process control and file operations.

**Features**:
- Detect source files (.mlp)
- Compile each file (execute_command)
- Link object files
- Capture compiler output
- Parallel compilation (sequential for now)
- Build timestamp tracking

**Code Structure**:
```mlp
# Build single file
function compile_file(source: string) -> bool {
    output := get_file_name(source) + ".o"
    cmd := "mlp -c " + source + " -o " + output
    exit_code := execute_command(cmd)
    return exit_code == 0
}

# Build all sources
function build_all() {
    files := list_directory("src")
    for file in files {
        if get_file_extension(file) == "mlp" {
            print "Compiling " + file
            if !compile_file(join_path(["src", file], 2)) {
                print "Build failed!"
                exit(1)
            }
        }
    }
}
```

---

#### log_analyzer.mlp (~200 lines)
Log file analyzer demonstrating time/date operations and file I/O.

**Features**:
- Read log files
- Parse timestamps
- Filter by date range
- Count errors/warnings
- Generate statistics

**Code Structure**:
```mlp
# Parse log entry
function parse_log_line(line: string) -> LogEntry {
    # Extract timestamp and message
    timestamp_str := line[0:19]  # "2025-11-21 14:30:00"
    timestamp := parse_timestamp(timestamp_str)
    message := line[20:]
    return LogEntry{timestamp, message}
}

# Filter by date
function filter_logs(entries: LogEntry[], start: int, end: int) -> LogEntry[] {
    result := []
    for entry in entries {
        if entry.timestamp >= start and entry.timestamp <= end {
            result.append(entry)
        }
    }
    return result
}
```

---

### 7. API_REFERENCE.md Update (~150 lines)

Add new section "System Utilities" with all 22 functions:

```markdown
## System Utilities

### Binary File I/O
- `read_binary(path)` - Read file as bytes
- `write_binary(path, data, size)` - Write bytes to file
- `get_file_info(path)` - Get file metadata
- `copy_file(source, dest)` - Copy file

### Directory Operations
- `list_directory(path)` - List files in directory
- `create_directory(path)` - Create directory
- `remove_directory(path)` - Remove directory
- `directory_exists(path)` - Check if directory exists
- `get_current_dir()` - Get current directory
- `change_directory(path)` - Change directory

[... continue for all categories ...]
```

---

## 📝 Documentation Standards

### Format Requirements
- Use markdown with proper headings
- Include code examples for every function
- Show both success and error cases
- Document return values clearly
- List all possible error codes

### Code Example Format
```mlp
# Single-line comment explaining what this does
variable := function_call(args)
if variable == expected {
    print "Success"
} else {
    print "Failed"
}
```

### Writing Style
- Clear and concise
- Focus on practical usage
- Assume reader knows basic programming
- Link to related functions
- Warn about common mistakes

---

## 🔗 Cross-References

When documenting, reference:
- Phase 5.1 (string operations)
- Phase 5.2 (error handling, memory management)
- Existing file I/O (if any)
- C standard library equivalents

---

## ✅ Completion Checklist

Before marking documentation complete:

- [ ] All 22 functions documented with API specs
- [ ] Each function has 2+ code examples
- [ ] Error codes listed for each function
- [ ] 3 example programs complete and tested
- [ ] API_REFERENCE.md updated
- [ ] All markdown renders correctly
- [ ] Code examples use correct MLP syntax
- [ ] Security warnings included where needed
- [ ] Performance notes for expensive operations
- [ ] Platform-specific notes (Linux focus)

---

## 📊 Estimated Effort

| Deliverable | Lines | Time |
|-------------|-------|------|
| BINARY_IO_DESIGN.md | ~400 | 1 day |
| DIRECTORY_OPS_DESIGN.md | ~500 | 1.5 days |
| PROCESS_CONTROL_DESIGN.md | ~450 | 1.5 days |
| TIME_UTILITIES_DESIGN.md | ~350 | 1 day |
| PATH_UTILITIES_DESIGN.md | ~300 | 1 day |
| file_manager.mlp | ~250 | 0.5 day |
| build_system.mlp | ~350 | 1 day |
| log_analyzer.mlp | ~200 | 0.5 day |
| API updates | ~150 | 0.5 day |
| **Total** | **~2,950** | **~9 days** |

Note: Timeline includes writing, review, and revisions based on implementation feedback.

---

## 🚀 Getting Started

1. Read PHASE5_3_COORDINATION.md
2. Review Phase 5.2 design docs for format reference
3. Create branch from main: `git checkout -b claude2`
4. Start with BINARY_IO_DESIGN.md (most critical)
5. Commit progress daily
6. Push to `claude2` branch regularly

---

## 💬 Communication

- Daily push to `claude2` branch with progress
- Tag commit messages with doc name
- Ask questions via GitHub Issues tagged `phase5.3-docs`
- Review Claude #3's implementation for accuracy

---

## 🎯 Success Criteria

Your documentation is complete when:
- Claude #3 can implement functions from your specs alone
- Users can write code using only your examples
- All edge cases and errors documented
- Examples compile and run correctly
- No ambiguity in API descriptions

---

**Created**: November 21, 2025  
**Status**: ✅ READY TO START  
**Owner**: Claude #2 (Documentation Specialist)

Good luck! 🚀
