# Phase 5.1 - Documentation Brief (Claude #2)

**Session**: Phase 5.1 - Command-Line Args & Enhanced Strings
**Your Role**: Documentation & Design Specifications
**Branch**: `claude/phase5-1-docs-[SESSION_ID]`
**Timeline**: ~2 weeks

---

## 🎯 Your Mission

Create comprehensive documentation for Phase 5.1 features:
1. Command-line argument support (argc, argv) specification
2. Enhanced string operations (4 new functions) specification
3. Example programs demonstrating new features
4. API reference updates

**Total Expected**: ~1,500 lines of documentation

---

## 📋 Part 1: Command-Line Arguments Design Document

### 1.1 Create docs/CMD_ARGS_DESIGN.md

**Goal**: Comprehensive specification for command-line argument support in MLP.

**Structure**:

```markdown
# Command-Line Arguments - Design Specification

**Status**: Phase 5.1 - In Progress
**Version**: 1.0
**Last Updated**: [DATE]

---

## Overview

This document specifies command-line argument support for MLP, enabling programs to access argc/argv parameters passed to the main function.

### Goals
- Enable command-line argument parsing in MLP programs
- Support standard argc/argv conventions
- Maintain compatibility with existing programs
- Provide safe, bounds-checked access to arguments

### Non-Goals
- Complex argument parsing libraries (future work)
- Flag parsing utilities (future work)
- Environment variable access (Phase 5.2)

---

## Language Syntax

### Main Function Enhancement

**Current Syntax** (still supported):
```mlp
function main() then
    print "Hello World"
end
```

**New Syntax** (Phase 5.1):
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

### Special Identifiers

- `argc`: Special identifier representing argument count (int)
- `argv`: Special identifier representing argument vector (string array-like)

**Key Characteristics**:
- `argc` and `argv` are NOT keywords - they're special identifiers only in main() parameters
- Only valid when main() has exactly 2 parameters named "argc" and "argv"
- `argc` is accessible as a read-only integer
- `argv` provides `.get(index)` method for accessing arguments

---

## Runtime API

### Function Signatures

```c
/**
 * mlp_set_args - Store command-line arguments
 * @param argc: Number of arguments
 * @param argv: Array of argument strings
 *
 * Called by the C main() wrapper before executing MLP main().
 * Stores argc/argv in global storage for later access.
 */
void mlp_set_args(int argc, char** argv);

/**
 * mlp_get_argc - Get argument count
 * @return: Number of command-line arguments (including program name)
 *
 * MLP Usage: int count = argc;
 *
 * Returns the total number of command-line arguments passed to the program.
 * Index 0 is always the program name.
 */
long mlp_get_argc(void);

/**
 * mlp_get_argv - Get argument at index
 * @param index: Argument index (0-based, 0 = program name)
 * @return: Argument string (caller must free), or empty string if out of bounds
 *
 * MLP Usage: string arg = argv.get(0);
 *
 * Returns a newly allocated copy of the argument at the given index.
 * Returns empty string if index is out of bounds (no error).
 * Caller is responsible for freeing the returned string.
 */
char* mlp_get_argv(long index);
```

---

## Implementation Details

### Argument Storage

Arguments are stored in global variables:
```c
static int mlp_argc = 0;
static char** mlp_argv = NULL;
```

**Lifetime**:
- Set once at program start by mlp_set_args()
- Available throughout program execution
- No need to free (managed by OS)

### Main Wrapper Pattern

The compiler generates a C main() wrapper:
```asm
global main
extern mlp_set_args
extern mlp_main

main:
    ; Save argc/argv for later access
    mov rdi, [rsp + 8]      ; argc
    lea rsi, [rsp + 16]     ; argv
    call mlp_set_args

    ; Call user's main function
    call mlp_main

    ; Return 0
    xor rax, rax
    ret
```

---

## Usage Examples

### Example 1: Print All Arguments

```mlp
function main(argc, argv) then
    int i = 0;

    print "Program name:"
    string prog = argv.get(0);
    print prog
    print ""

    print "Arguments:"
    i = 1
    while i < argc then
        string arg = argv.get(i);
        print arg
        i = i + 1
    end
end
```

**Run**:
```bash
$ ./program hello world 123
Program name:
./program

Arguments:
hello
world
123
```

### Example 2: Argument Count Check

```mlp
function main(argc, argv) then
    if argc < 2 then
        print "Usage: program <filename>"
        return 1
    end

    string filename = argv.get(1);
    print "Processing file:"
    print filename

    return 0
end
```

### Example 3: Simple Flag Parser

```mlp
function main(argc, argv) then
    int verbose = 0;
    int i = 1;

    while i < argc then
        string arg = argv.get(i);

        -- Check for -v flag
        if string_equals(arg, "-v") then
            verbose = 1
            print "Verbose mode enabled"
        end

        i = i + 1
    end

    return 0
end
```

---

## Compiler Integration

### Lexer Changes
- No new tokens required
- "argc" and "argv" remain identifiers

### Parser Changes
- Detect main() function with 2 parameters
- Validate parameter names are "argc" and "argv"
- Mark function metadata for special handling

### Code Generation
- Generate argc access as `call mlp_get_argc`
- Generate argv.get(i) as `call mlp_get_argv` with index
- Generate main wrapper that calls mlp_set_args

---

## Testing Strategy

### Unit Tests (C)
- Test mlp_set_args/get_argc/get_argv functions
- Test bounds checking (negative, >= argc)
- Test empty argument list

### Integration Tests (MLP)
- Test argc access in main()
- Test argv.get() for all indices
- Test out-of-bounds access (should return empty string)
- Test programs with 0, 1, 10+ arguments

### Edge Cases
- Empty program name
- Arguments with spaces
- Arguments with special characters
- Very long argument lists (100+ args)

---

## Migration Guide

### Existing Programs
All existing programs without main() parameters continue to work unchanged:
```mlp
function main() then
    print "Hello"
end
```

### New Programs
To access command-line arguments, add parameters:
```mlp
function main(argc, argv) then
    -- Access arguments here
end
```

---

## Future Enhancements (Not in Phase 5.1)

1. **Flag Parsing Library**:
   - Automatic flag parsing: `--verbose`, `-v`, etc.
   - Type conversion for flags
   - Help text generation

2. **Argument Validation**:
   - Required/optional arguments
   - Type checking
   - Range validation

3. **Environment Variables**:
   - `getenv()` function
   - Environment variable iteration

---

## API Reference Addition

Add to `API_REFERENCE.md`:

### Command-Line Arguments

| Function | Signature | Description | Returns |
|----------|-----------|-------------|---------|
| `argc` | `argc` (in main) | Get argument count | int |
| `argv.get()` | `argv.get(index)` | Get argument at index | string |

**MLP Usage**:
```mlp
function main(argc, argv) then
    int count = argc;                  -- Get count
    string program = argv.get(0);      -- Program name
    string first_arg = argv.get(1);    -- First argument
end
```

---

## Success Criteria

- ✅ Complete specification document (~800 lines)
- ✅ All usage examples provided
- ✅ Migration guide for existing code
- ✅ Testing strategy defined
- ✅ API reference updated

---

**Estimated Size**: ~800 lines
```

---

## 📋 Part 2: Enhanced String Operations Design Document

### 2.1 Create docs/STRING_OPS_ENHANCED_DESIGN.md

**Goal**: Comprehensive specification for 4 new string operations.

**Structure**:

```markdown
# Enhanced String Operations - Design Specification

**Status**: Phase 5.1 - In Progress
**Version**: 1.0
**Last Updated**: [DATE]

---

## Overview

This document specifies 4 new string operations for Phase 5.1, extending MLP's string manipulation capabilities beyond the existing Phase 3 functions.

### New Functions (Phase 5.1)
1. `string_length()` - Get string length
2. `string_substring()` - Extract substring
3. `string_index_of()` - Find first occurrence
4. `string_last_index_of()` - Find last occurrence

### Existing Functions (Phase 3)
For context, these already exist:
- `string_split()`, `string_join()`, `string_replace()`
- `string_trim()`, `string_upper()`, `string_lower()`
- `string_find()`, `string_starts_with()`, `string_ends_with()`

---

## Function 1: string_length()

### Purpose
Get the length of a string (number of characters).

### Signature
```mlp
int string_length(string str)
```

### Parameters
- `str`: Input string

### Returns
- Length of string (number of characters)
- Returns 0 for empty string
- Returns 0 for NULL (internal handling)

### Implementation
```c
long string_length(const char* str) {
    if (!str) return 0;
    return strlen(str);
}
```

### Usage Examples

**Example 1: Basic Length**
```mlp
string text = "Hello World";
int len = string_length(text);
print len;  -- Output: 11
```

**Example 2: Empty String**
```mlp
string empty = "";
int len = string_length(empty);
print len;  -- Output: 0
```

**Example 3: Validation**
```mlp
function validate_password(string password) then
    int len = string_length(password);

    if len < 8 then
        print "Password too short!"
        return 0
    end

    if len > 128 then
        print "Password too long!"
        return 0
    end

    print "Password valid"
    return 1
end
```

---

## Function 2: string_substring()

### Purpose
Extract a substring from a string.

### Signature
```mlp
string string_substring(string str, int start, int length)
```

### Parameters
- `str`: Input string
- `start`: Start index (0-based)
- `length`: Number of characters to extract

### Returns
- New string containing substring
- Empty string if start is out of bounds
- Truncated if length exceeds string bounds
- Returns empty string for NULL input

### Behavior
- If `start < 0`: returns empty string
- If `start >= string_length(str)`: returns empty string
- If `start + length > string_length(str)`: returns substring to end
- Result is always NULL-terminated
- Caller must free the returned string (handled by runtime)

### Implementation
```c
char* string_substring(const char* str, long start, long length) {
    if (!str) return strdup("");

    long str_len = strlen(str);
    if (start < 0 || start >= str_len) return strdup("");
    if (length < 0) length = 0;

    // Adjust length if it exceeds string
    if (start + length > str_len) {
        length = str_len - start;
    }

    char* result = (char*)malloc(length + 1);
    if (!result) {
        fprintf(stderr, "Memory allocation failed\\n");
        exit(1);
    }

    strncpy(result, str + start, length);
    result[length] = '\\0';
    return result;
}
```

### Usage Examples

**Example 1: Basic Substring**
```mlp
string text = "Hello World";
string sub = string_substring(text, 0, 5);
print sub;  -- Output: "Hello"
```

**Example 2: Mid-String Extraction**
```mlp
string text = "Hello World";
string sub = string_substring(text, 6, 5);
print sub;  -- Output: "World"
```

**Example 3: To End of String**
```mlp
string text = "Hello World";
string sub = string_substring(text, 6, 100);
print sub;  -- Output: "World" (truncated)
```

**Example 4: Parse CSV**
```mlp
function parse_csv_field(string line, int field_num) then
    int start = 0;
    int end = 0;
    int current_field = 0;
    int len = string_length(line);
    int i = 0;

    while i < len then
        if current_field == field_num then
            -- Found field start
            start = i

            -- Find field end (comma or end of line)
            while i < len then
                string ch = string_substring(line, i, 1);
                if string_equals(ch, ",") then
                    end = i
                    return string_substring(line, start, end - start)
                end
                i = i + 1
            end

            -- Field extends to end
            return string_substring(line, start, len - start)
        end

        -- Skip to next field
        string ch = string_substring(line, i, 1);
        if string_equals(ch, ",") then
            current_field = current_field + 1
        end
        i = i + 1
    end

    return ""
end
```

---

## Function 3: string_index_of()

### Purpose
Find the first occurrence of a substring within a string.

### Signature
```mlp
int string_index_of(string str, string needle)
```

### Parameters
- `str`: String to search in (haystack)
- `needle`: Substring to find

### Returns
- Index of first occurrence (0-based)
- Returns -1 if not found
- Returns -1 if either parameter is NULL
- Returns 0 if needle is empty string

### Implementation
```c
long string_index_of(const char* str, const char* needle) {
    if (!str || !needle) return -1;

    char* found = strstr(str, needle);
    if (!found) return -1;

    return found - str;
}
```

### Usage Examples

**Example 1: Basic Search**
```mlp
string text = "Hello World";
int pos = string_index_of(text, "World");
print pos;  -- Output: 6
```

**Example 2: Not Found**
```mlp
string text = "Hello World";
int pos = string_index_of(text, "xyz");
print pos;  -- Output: -1
```

**Example 3: Case Sensitivity**
```mlp
string text = "Hello World";
int pos = string_index_of(text, "world");
print pos;  -- Output: -1 (case-sensitive)
```

**Example 4: Extract Until Delimiter**
```mlp
function get_first_word(string text) then
    int space_pos = string_index_of(text, " ");

    if space_pos == -1 then
        -- No space, return whole string
        return text
    end

    return string_substring(text, 0, space_pos)
end
```

**Example 5: Check File Extension**
```mlp
function has_extension(string filename, string ext) then
    int dot_pos = string_index_of(filename, ".");

    if dot_pos == -1 then
        return 0  -- No extension
    end

    int ext_len = string_length(ext);
    string file_ext = string_substring(filename, dot_pos + 1, ext_len);

    if string_equals(file_ext, ext) then
        return 1
    end

    return 0
end
```

---

## Function 4: string_last_index_of()

### Purpose
Find the last occurrence of a substring within a string.

### Signature
```mlp
int string_last_index_of(string str, string needle)
```

### Parameters
- `str`: String to search in (haystack)
- `needle`: Substring to find

### Returns
- Index of last occurrence (0-based)
- Returns -1 if not found
- Returns -1 if either parameter is NULL
- Returns string_length(str) if needle is empty string

### Implementation
```c
long string_last_index_of(const char* str, const char* needle) {
    if (!str || !needle) return -1;

    char* last_found = NULL;
    char* current = (char*)str;

    while ((current = strstr(current, needle)) != NULL) {
        last_found = current;
        current++;
    }

    if (!last_found) return -1;
    return last_found - str;
}
```

### Usage Examples

**Example 1: Multiple Occurrences**
```mlp
string text = "Hello World Hello";
int pos = string_last_index_of(text, "Hello");
print pos;  -- Output: 12
```

**Example 2: File Extension**
```mlp
function get_file_extension(string filename) then
    int dot_pos = string_last_index_of(filename, ".");

    if dot_pos == -1 then
        return ""  -- No extension
    end

    int len = string_length(filename);
    return string_substring(filename, dot_pos + 1, len - dot_pos - 1)
end

-- Usage:
string ext = get_file_extension("document.backup.txt");
print ext;  -- Output: "txt"
```

**Example 3: Path Parsing**
```mlp
function get_filename(string path) then
    int slash_pos = string_last_index_of(path, "/");

    if slash_pos == -1 then
        -- No slash, return whole path
        return path
    end

    int len = string_length(path);
    return string_substring(path, slash_pos + 1, len - slash_pos - 1)
end

-- Usage:
string fname = get_filename("/home/user/document.txt");
print fname;  -- Output: "document.txt"
```

---

## Comparison with Phase 3 Functions

### Overlap with string_find()

Phase 3 has `string_find()` which is similar to `string_index_of()`:

**Differences**:
- `string_find()` might return boolean or index (check implementation)
- `string_index_of()` always returns index or -1 (standard convention)
- `string_index_of()` follows JavaScript/Java naming convention

**Recommendation**: Keep both for now, document the difference clearly.

---

## Integration with Existing String Functions

### Complete String API (Phase 3 + Phase 5.1)

| Function | Phase | Purpose |
|----------|-------|---------|
| `string_length()` | 5.1 | Get length |
| `string_substring()` | 5.1 | Extract substring |
| `string_index_of()` | 5.1 | Find first occurrence |
| `string_last_index_of()` | 5.1 | Find last occurrence |
| `string_split()` | 3 | Split by delimiter |
| `string_join()` | 3 | Join list of strings |
| `string_replace()` | 3 | Replace occurrences |
| `string_trim()` | 3 | Remove whitespace |
| `string_upper()` | 3 | Convert to uppercase |
| `string_lower()` | 3 | Convert to lowercase |
| `string_find()` | 3 | Find substring |
| `string_starts_with()` | 3 | Check prefix |
| `string_ends_with()` | 3 | Check suffix |

**Total**: 13 string functions after Phase 5.1

---

## Common Use Cases

### Use Case 1: Parse Command-Line Flag Value

```mlp
function parse_flag_value(string arg) then
    -- Parse "--output=filename.txt" format
    int equals_pos = string_index_of(arg, "=");

    if equals_pos == -1 then
        return ""
    end

    int len = string_length(arg);
    return string_substring(arg, equals_pos + 1, len - equals_pos - 1)
end
```

### Use Case 2: Word Count

```mlp
function word_count(string text) then
    int count = 0;
    int pos = 0;
    int len = string_length(text);

    if len == 0 then
        return 0
    end

    -- Count spaces + 1
    while pos < len then
        string ch = string_substring(text, pos, 1);
        if string_equals(ch, " ") then
            count = count + 1
        end
        pos = pos + 1
    end

    return count + 1
end
```

### Use Case 3: URL Path Parser

```mlp
function parse_url_path(string url) then
    -- Extract "/path/to/resource" from "https://example.com/path/to/resource"

    -- Find "://" to skip protocol
    int protocol_end = string_index_of(url, "://");
    if protocol_end == -1 then
        return ""
    end

    -- Find first "/" after protocol
    int path_start = string_index_of(
        string_substring(url, protocol_end + 3, 1000),
        "/"
    );

    if path_start == -1 then
        return "/"
    end

    path_start = path_start + protocol_end + 3
    int len = string_length(url);
    return string_substring(url, path_start, len - path_start)
end
```

---

## Compiler Integration

### Lexer Tokens
```c
TOKEN_BUILTIN_STRING_LENGTH
TOKEN_BUILTIN_STRING_SUBSTRING
TOKEN_BUILTIN_STRING_INDEX_OF
TOKEN_BUILTIN_STRING_LAST_INDEX_OF
```

### Parser
Recognize as built-in function calls, similar to existing string functions from Phase 3.

### Code Generation
Generate calls to runtime functions:
```asm
call string_length
call string_substring
call string_index_of
call string_last_index_of
```

---

## Testing Strategy

### Unit Tests (C)
- Test each function with various inputs
- Test NULL handling
- Test empty strings
- Test boundary conditions
- Test memory allocation/deallocation

### Integration Tests (MLP)
- Test in realistic scenarios
- Test combination of functions
- Test with existing Phase 3 string functions
- Test performance with long strings (1KB, 10KB, 100KB)

### Edge Cases
- Empty strings
- Very long strings (>1MB)
- Special characters (Unicode, null bytes)
- Out-of-bounds indices
- Negative indices

---

## Performance Considerations

### Time Complexity
- `string_length()`: O(n) where n = string length
- `string_substring()`: O(m) where m = substring length
- `string_index_of()`: O(n*m) worst case (strstr implementation)
- `string_last_index_of()`: O(n*m) worst case

### Memory Usage
- `string_substring()` allocates new memory (caller must free)
- Other functions return primitives (no allocation)

### Optimization Opportunities
- Consider caching string lengths
- Consider more efficient search algorithms (Boyer-Moore, KMP) for long strings
- Phase 7 work

---

## Success Criteria

- ✅ Complete specification document (~700 lines)
- ✅ All 4 functions documented with examples
- ✅ Integration with existing API shown
- ✅ Common use cases demonstrated
- ✅ Testing strategy defined

---

**Estimated Size**: ~700 lines
```

---

## 📋 Part 3: Example Programs

### 3.1 Create examples/example_cmdline_args.mlp

**Goal**: Demonstrate command-line argument usage.

```mlp
-- Example: Command-Line Arguments
-- Demonstrates argc/argv support in Phase 5.1

function print_usage(string program_name) then
    print "Usage:"
    print program_name
    print " [options] <input_file>"
    print ""
    print "Options:"
    print "  -v, --verbose    Enable verbose output"
    print "  -h, --help       Show this help message"
    return 0
end

function main(argc, argv) then
    int verbose = 0;
    string input_file = "";
    int i = 0;

    -- Get program name
    string program_name = argv.get(0);

    -- Check if we have arguments
    if argc < 2 then
        print "Error: No arguments provided"
        print ""
        print_usage(program_name)
        return 1
    end

    -- Parse arguments
    i = 1
    while i < argc then
        string arg = argv.get(i);

        if string_equals(arg, "-v") then
            verbose = 1
            if verbose == 1 then
                print "Verbose mode enabled"
            end
        end

        if string_equals(arg, "--verbose") then
            verbose = 1
            if verbose == 1 then
                print "Verbose mode enabled"
            end
        end

        if string_equals(arg, "-h") then
            print_usage(program_name)
            return 0
        end

        if string_equals(arg, "--help") then
            print_usage(program_name)
            return 0
        end

        -- Check if it's a file (doesn't start with -)
        string first_char = string_substring(arg, 0, 1);
        if string_equals(first_char, "-") == 0 then
            input_file = arg
        end

        i = i + 1
    end

    -- Validate we got an input file
    if string_length(input_file) == 0 then
        print "Error: No input file specified"
        print ""
        print_usage(program_name)
        return 1
    end

    -- Process the file
    print "Processing file:"
    print input_file

    if verbose == 1 then
        print "Verbose: Opening file..."
        print "Verbose: Reading contents..."
        print "Verbose: Processing data..."
    end

    -- Check if file exists
    int exists = file_exists(input_file);
    if exists == 0 then
        print "Error: File not found:"
        print input_file
        return 1
    end

    -- Read and process file
    string content = read_file(input_file);
    int len = string_length(content);

    print "File size:"
    print len
    print "bytes"

    if verbose == 1 then
        print "Verbose: Processing complete!"
    end

    print "Done!"
    return 0
end
```

**Estimated**: ~120 lines

---

### 3.2 Create examples/example_string_parser.mlp

**Goal**: Demonstrate all 4 new string functions in a realistic parsing scenario.

```mlp
-- Example: String Parser
-- Demonstrates enhanced string operations from Phase 5.1

-- Parse a CSV line into fields
function parse_csv_line(string line) then
    list[string] fields = list();
    int pos = 0;
    int field_start = 0;
    int len = string_length(line);

    print "Parsing CSV line..."
    print line
    print ""

    while pos < len then
        -- Find next comma
        string remaining = string_substring(line, pos, len - pos);
        int comma_pos = string_index_of(remaining, ",");

        if comma_pos == -1 then
            -- Last field
            string field = string_substring(line, field_start, len - field_start);
            fields.add(field)
            pos = len  -- Exit loop
        end

        if comma_pos >= 0 then
            -- Extract field
            int field_len = (pos + comma_pos) - field_start;
            string field = string_substring(line, field_start, field_len);
            fields.add(field)

            -- Move to next field
            pos = pos + comma_pos + 1
            field_start = pos
        end
    end

    -- Print fields
    int count = fields.size();
    print "Found fields:"
    print count
    print ""

    int i = 0;
    while i < count then
        string field = fields.get(i);
        print "Field "
        print i
        print ": "
        print field
        i = i + 1
    end

    return 0
end

-- Extract file extension from path
function get_file_extension(string path) then
    print "Getting extension from:"
    print path

    -- Find last dot
    int dot_pos = string_last_index_of(path, ".");

    if dot_pos == -1 then
        print "No extension found"
        return ""
    end

    -- Extract extension
    int len = string_length(path);
    int ext_len = len - dot_pos - 1;
    string ext = string_substring(path, dot_pos + 1, ext_len);

    print "Extension: "
    print ext
    print ""

    return ext
end

-- Parse URL into components
function parse_url(string url) then
    print "Parsing URL:"
    print url
    print ""

    -- Find protocol
    int protocol_end = string_index_of(url, "://");

    if protocol_end == -1 then
        print "Invalid URL: no protocol"
        return 0
    end

    string protocol = string_substring(url, 0, protocol_end);
    print "Protocol: "
    print protocol

    -- Find hostname end (first / after ://)
    int hostname_start = protocol_end + 3;
    int len = string_length(url);
    string after_protocol = string_substring(url, hostname_start, len - hostname_start);
    int path_start = string_index_of(after_protocol, "/");

    if path_start == -1 then
        -- No path, just hostname
        string hostname = after_protocol;
        print "Hostname: "
        print hostname
        print "Path: /"
    end

    if path_start >= 0 then
        string hostname = string_substring(after_protocol, 0, path_start);
        print "Hostname: "
        print hostname

        string path = string_substring(after_protocol, path_start, len);
        print "Path: "
        print path
    end

    print ""
    return 0
end

-- Count word occurrences in text
function count_word(string text, string word) then
    int count = 0;
    int pos = 0;
    int text_len = string_length(text);
    int word_len = string_length(word);

    print "Counting occurrences of '"
    print word
    print "' in text..."
    print ""

    while pos < text_len then
        -- Search for word starting at pos
        string remaining = string_substring(text, pos, text_len - pos);
        int found_pos = string_index_of(remaining, word);

        if found_pos == -1 then
            pos = text_len  -- Exit loop
        end

        if found_pos >= 0 then
            count = count + 1
            pos = pos + found_pos + word_len
        end
    end

    print "Found "
    print count
    print " occurrence(s)"
    print ""

    return count
end

-- Validate email format (basic)
function validate_email(string email) then
    print "Validating email:"
    print email

    int len = string_length(email);

    -- Check minimum length
    if len < 5 then
        print "Invalid: too short"
        return 0
    end

    -- Must contain @
    int at_pos = string_index_of(email, "@");
    if at_pos == -1 then
        print "Invalid: no @ symbol"
        return 0
    end

    -- @ must not be first or last character
    if at_pos == 0 then
        print "Invalid: @ at start"
        return 0
    end

    if at_pos == len - 1 then
        print "Invalid: @ at end"
        return 0
    end

    -- Must have only one @
    string after_at = string_substring(email, at_pos + 1, len - at_pos - 1);
    int second_at = string_index_of(after_at, "@");
    if second_at >= 0 then
        print "Invalid: multiple @ symbols"
        return 0
    end

    -- Must have dot after @
    int dot_pos = string_index_of(after_at, ".");
    if dot_pos == -1 then
        print "Invalid: no domain extension"
        return 0
    end

    print "Valid email format"
    print ""
    return 1
end

-- Main test program
print "===================================="
print "Phase 5.1 String Operations Demo"
print "===================================="
print ""

-- Test 1: CSV parsing
parse_csv_line("Alice,30,Engineer")
print ""

-- Test 2: File extensions
get_file_extension("document.txt")
get_file_extension("archive.tar.gz")
get_file_extension("README")
print ""

-- Test 3: URL parsing
parse_url("https://github.com/user/repo")
parse_url("http://example.com")
print ""

-- Test 4: Word counting
string text = "hello world hello universe hello everyone";
count_word(text, "hello")
print ""

-- Test 5: Email validation
validate_email("user@example.com")
validate_email("invalid@")
validate_email("@invalid.com")
validate_email("no-at.com")
validate_email("double@@invalid.com")
validate_email("valid.email@sub.domain.com")
print ""

print "===================================="
print "Demo Complete!"
print "===================================="
```

**Estimated**: ~280 lines

---

## 📋 Part 4: API Reference Updates

### 4.1 Update API_REFERENCE.md

Add new sections for Phase 5.1 features:

**Section to Add**:

```markdown
## Command-Line Arguments (Phase 5.1)

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

## Enhanced String Operations (Phase 5.1)

### string_length()

Get the length of a string.

**Syntax**: `int string_length(string str)`

**Parameters**:
- `str`: Input string

**Returns**: Length of string (number of characters)

**Example**:
```mlp
string text = "Hello";
int len = string_length(text);  -- 5
```

---

### string_substring()

Extract a substring from a string.

**Syntax**: `string string_substring(string str, int start, int length)`

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

### string_index_of()

Find the first occurrence of a substring.

**Syntax**: `int string_index_of(string str, string needle)`

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

### string_last_index_of()

Find the last occurrence of a substring.

**Syntax**: `int string_last_index_of(string str, string needle)`

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
```

---

## 📂 File Summary

Your deliverables:
```
/home/user/MLP/
├── docs/CMD_ARGS_DESIGN.md                    (NEW: ~800 lines)
├── docs/STRING_OPS_ENHANCED_DESIGN.md         (NEW: ~700 lines)
├── examples/example_cmdline_args.mlp          (NEW: ~120 lines)
├── examples/example_string_parser.mlp         (NEW: ~280 lines)
└── API_REFERENCE.md                           (MODIFIED: +~120 lines)
```

**Total**: ~2,020 lines of documentation

---

## ✅ Success Criteria

1. ✅ CMD_ARGS_DESIGN.md complete (~800 lines)
2. ✅ STRING_OPS_ENHANCED_DESIGN.md complete (~700 lines)
3. ✅ 2 example programs created (~400 lines)
4. ✅ API_REFERENCE.md updated (~120 lines)
5. ✅ All examples are syntactically correct MLP code
6. ✅ Documentation is comprehensive and clear

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase5-1-docs-[SESSION_ID]

# Step 1: Design documents
git add docs/CMD_ARGS_DESIGN.md
git commit -m "docs(phase5.1): Add command-line arguments design specification"

git add docs/STRING_OPS_ENHANCED_DESIGN.md
git commit -m "docs(phase5.1): Add enhanced string operations design specification"

# Step 2: Examples
git add examples/example_cmdline_args.mlp
git commit -m "docs(phase5.1): Add command-line arguments example"

git add examples/example_string_parser.mlp
git commit -m "docs(phase5.1): Add string parser example demonstrating new functions"

# Step 3: API reference
git add API_REFERENCE.md
git commit -m "docs(phase5.1): Update API reference with Phase 5.1 functions"

git push -u origin claude/phase5-1-docs-[SESSION_ID]
```

---

## 🎯 Documentation Standards

1. **Clarity**: Use simple, clear language
2. **Examples**: Provide realistic, runnable examples
3. **Completeness**: Cover all edge cases and behaviors
4. **Consistency**: Match existing documentation style
5. **Accuracy**: Ensure all code examples are valid MLP syntax

---

## 💡 Tips

1. **Cross-reference**: Link to Phase 3 string functions where relevant
2. **Use cases**: Show practical applications, not just toy examples
3. **Edge cases**: Document behavior for NULL, empty strings, out-of-bounds
4. **Performance**: Note time/space complexity where relevant

---

## 🎉 When You're Done

Report:
1. Design documents complete (2 files, ~1,500 lines)
2. Example programs complete (2 files, ~400 lines)
3. API reference updated (~120 lines)
4. Branch pushed

**Branch**: `claude/phase5-1-docs-[SESSION_ID]`
**Expected**: 5 commits

Let's document Phase 5.1! 📝
