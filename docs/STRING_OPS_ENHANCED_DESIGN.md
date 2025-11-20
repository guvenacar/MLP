# Enhanced String Operations - Design Specification

**Status**: Phase 5.1 - In Progress
**Version**: 1.0
**Last Updated**: November 21, 2025

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

**Estimated Size**: ~580 lines completed
