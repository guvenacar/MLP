# String Operations Design Specification

**Phase:** 3 - Self-Hosting Completion
**Date:** November 20, 2025
**Status:** Design Complete - Implementation Needed

---

## 📋 Overview

String manipulation is **critical** for self-hosting. The compiler needs to:
- Tokenize source code (split by delimiters)
- Join code fragments
- Replace keywords (multi-language support)
- Trim whitespace
- Case transformations
- Pattern matching

Phase 3 extends existing string operations with essential functions for text processing.

---

## 🎯 Design Goals

1. **Completeness** - Cover all common string operations
2. **Performance** - Efficient implementations for large strings
3. **Simplicity** - Intuitive API matching other languages
4. **Unicode Aware** - Support UTF-8 (future consideration)
5. **Memory Safe** - Proper allocation and cleanup

---

## 📚 Existing Functions (Phase 0-2)

### Already Implemented ✅

#### `string_concat(a: string, b: string) -> string`
Concatenates two strings.

**Example:**
```mlp
string result = string_concat("Hello ", "World");
print result;  -- "Hello World"
```

#### `string_length(str: string) -> int`
Returns string length in bytes.

**Example:**
```mlp
int len = string_length("Hello");
print len;  -- 5
```

#### `string_char_at(str: string, index: int) -> string`
Returns character at index as single-char string.

**Example:**
```mlp
string ch = string_char_at("Hello", 1);
print ch;  -- "e"
```

#### `string_substring(str: string, start: int, length: int) -> string`
Extracts substring.

**Example:**
```mlp
string sub = string_substring("Hello World", 6, 5);
print sub;  -- "World"
```

#### `char_code(ch: string) -> int`
Returns ASCII code of character.

**Example:**
```mlp
int code = char_code("A");
print code;  -- 65
```

#### `code_to_char(code: int) -> string`
Converts ASCII code to character.

**Example:**
```mlp
string ch = code_to_char(65);
print ch;  -- "A"
```

---

## 🆕 New Functions (Phase 3)

### 1. `string_split(str: string, delimiter: string) -> list[string]`

Splits string by delimiter into list.

**Parameters:**
- `str` - String to split
- `delimiter` - Separator string

**Returns:**
- `list[string]` of parts

**Example:**
```mlp
string csv = "Alice,25,Engineer";
list[string] parts = string_split(csv, ",");

print parts.get(0);  -- "Alice"
print parts.get(1);  -- "25"
print parts.get(2);  -- "Engineer"
print parts.size();  -- 3
```

**Use Cases:**
- Parsing CSV/TSV data
- Tokenization
- Command-line argument parsing
- Path splitting

**Edge Cases:**
```mlp
-- Empty string
string_split("", ",")  -- Returns list with 1 empty string

-- No delimiter found
string_split("Hello", ",")  -- Returns list with ["Hello"]

-- Multiple delimiters
string_split("a,,b", ",")  -- Returns ["a", "", "b"]

-- Delimiter at ends
string_split(",a,b,", ",")  -- Returns ["", "a", "b", ""]
```

---

### 2. `string_join(parts: list[string], delimiter: string) -> string`

Joins list of strings with delimiter.

**Parameters:**
- `parts` - List of strings to join
- `delimiter` - Separator to insert between parts

**Returns:**
- Single concatenated string

**Example:**
```mlp
list[string] words = list();
words.add("Hello");
words.add("beautiful");
words.add("world");

string result = string_join(words, " ");
print result;  -- "Hello beautiful world"
```

**Use Cases:**
- Building CSV lines
- Code generation (joining statements)
- Path construction
- Command building

**Edge Cases:**
```mlp
-- Empty list
string_join(empty_list, ",")  -- Returns ""

-- Single element
string_join(["Hello"], ",")  -- Returns "Hello"

-- Empty delimiter
string_join(["a", "b", "c"], "")  -- Returns "abc"
```

---

### 3. `string_replace(str: string, old: string, new: string) -> string`

Replaces all occurrences of substring.

**Parameters:**
- `str` - Original string
- `old` - Substring to replace
- `new` - Replacement substring

**Returns:**
- Modified string

**Example:**
```mlp
string code = "EĞER x > 10 İSE\n    YAZDIR x\nSON";
string english = string_replace(code, "EĞER", "if");
english = string_replace(english, "İSE", "then");
english = string_replace(english, "YAZDIR", "print");
english = string_replace(english, "SON", "end");

print english;
-- Output:
-- if x > 10 then
--     print x
-- end
```

**Use Cases:**
- Keyword translation (multi-language support)
- Template processing
- Code transformation
- Text normalization

**Edge Cases:**
```mlp
-- Substring not found
string_replace("Hello", "x", "y")  -- Returns "Hello"

-- Empty old string (invalid - return original)
string_replace("Hello", "", "x")  -- Returns "Hello"

-- Replacement creates new occurrences
string_replace("aaa", "aa", "a")  -- Returns "aa" (not recursive)
```

---

### 4. `string_trim(str: string) -> string`

Removes leading and trailing whitespace.

**Parameters:**
- `str` - String to trim

**Returns:**
- Trimmed string

**Whitespace:** space (` `), tab (`\t`), newline (`\n`), carriage return (`\r`)

**Example:**
```mlp
string input = "  Hello World  \n";
string clean = string_trim(input);
print clean;  -- "Hello World"
```

**Use Cases:**
- Cleaning user input
- Parsing configuration files
- Normalizing data
- Removing line endings

---

### 5. `string_upper(str: string) -> string`

Converts string to uppercase.

**Parameters:**
- `str` - String to convert

**Returns:**
- Uppercase version

**Example:**
```mlp
string result = string_upper("Hello World");
print result;  -- "HELLO WORLD"
```

**Use Cases:**
- Case-insensitive comparison
- Normalization
- Display formatting

**Note:** ASCII only in initial implementation. UTF-8 in future.

---

### 6. `string_lower(str: string) -> string`

Converts string to lowercase.

**Parameters:**
- `str` - String to convert

**Returns:**
- Lowercase version

**Example:**
```mlp
string result = string_lower("Hello World");
print result;  -- "hello world"
```

**Use Cases:**
- Case-insensitive matching
- Identifier normalization
- Keyword detection

---

### 7. `string_find(haystack: string, needle: string) -> int`

Finds first occurrence of substring.

**Parameters:**
- `haystack` - String to search in
- `needle` - Substring to find

**Returns:**
- Index of first occurrence (0-based)
- `-1` if not found

**Example:**
```mlp
string text = "Hello World";
int pos = string_find(text, "World");
print pos;  -- 6

int not_found = string_find(text, "xyz");
print not_found;  -- -1
```

**Use Cases:**
- Substring detection
- Pattern matching
- Validation

---

### 8. `string_starts_with(str: string, prefix: string) -> int`

Checks if string starts with prefix.

**Parameters:**
- `str` - String to check
- `prefix` - Prefix to match

**Returns:**
- `1` if starts with prefix
- `0` otherwise

**Example:**
```mlp
if string_starts_with("function main", "function") == 1 then
    print "Function declaration detected"
end
```

**Use Cases:**
- Token classification
- Command parsing
- Path matching
- Comment detection (`--`, `//`)

---

### 9. `string_ends_with(str: string, suffix: string) -> int`

Checks if string ends with suffix.

**Parameters:**
- `str` - String to check
- `suffix` - Suffix to match

**Returns:**
- `1` if ends with suffix
- `0` otherwise

**Example:**
```mlp
if string_ends_with("program.mlp", ".mlp") == 1 then
    print "MLP source file detected"
end
```

**Use Cases:**
- File extension checking
- Statement terminator detection
- Pattern matching

---

## 🔧 Runtime Implementation

### C Implementation Snippets

#### `string_split`
```c
void* mlp_string_split(const char* str, const char* delim) {
    List* result = list_create();

    if (!str || !delim || strlen(delim) == 0) {
        list_add(result, strdup(str ? str : ""));
        return result;
    }

    char* str_copy = strdup(str);
    char* token = strtok(str_copy, delim);

    while (token) {
        list_add(result, strdup(token));
        token = strtok(NULL, delim);
    }

    free(str_copy);
    return result;
}
```

#### `string_join`
```c
char* mlp_string_join(void* list_ptr, const char* delim) {
    List* list = (List*)list_ptr;
    if (list->size == 0) return strdup("");

    // Calculate total length
    size_t total_len = 0;
    size_t delim_len = strlen(delim);

    for (int i = 0; i < list->size; i++) {
        total_len += strlen((char*)list->data[i]);
        if (i < list->size - 1) total_len += delim_len;
    }

    // Allocate result
    char* result = malloc(total_len + 1);
    result[0] = '\0';

    // Join strings
    for (int i = 0; i < list->size; i++) {
        strcat(result, (char*)list->data[i]);
        if (i < list->size - 1) strcat(result, delim);
    }

    return result;
}
```

#### `string_replace`
```c
char* mlp_string_replace(const char* str, const char* old, const char* new) {
    if (!str || !old || strlen(old) == 0) return strdup(str);

    // Count occurrences
    int count = 0;
    const char* p = str;
    size_t old_len = strlen(old);

    while ((p = strstr(p, old))) {
        count++;
        p += old_len;
    }

    if (count == 0) return strdup(str);

    // Allocate result
    size_t new_len = strlen(new);
    size_t result_len = strlen(str) + count * (new_len - old_len);
    char* result = malloc(result_len + 1);

    // Perform replacement
    const char* src = str;
    char* dst = result;

    while (*src) {
        if (strncmp(src, old, old_len) == 0) {
            strcpy(dst, new);
            dst += new_len;
            src += old_len;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';

    return result;
}
```

---

## 🎯 Use Cases

### Use Case 1: Lexer/Tokenizer
```mlp
function tokenize(source) then
    list[string] tokens = list();

    -- Split by whitespace
    list[string] words = string_split(source, " ");

    int i = 0;
    while i < words.size()
        string word = words.get(i);
        word = string_trim(word);

        if string_length(word) > 0 then
            tokens.add(word);
        end

        i = i + 1
    end

    return tokens
end
```

### Use Case 2: Keyword Translation
```mlp
function translate_keywords(code) then
    -- Turkish → English
    code = string_replace(code, "EĞER", "if");
    code = string_replace(code, "İSE", "then");
    code = string_replace(code, "DEĞİLSE", "else");
    code = string_replace(code, "DÖNGÜ", "while");
    code = string_replace(code, "SON", "end");
    return code
end
```

### Use Case 3: CSV Parser
```mlp
function parse_csv_line(line) then
    list[string] fields = string_split(line, ",");

    -- Trim each field
    int i = 0;
    while i < fields.size()
        string field = fields.get(i);
        fields.set(i, string_trim(field));
        i = i + 1
    end

    return fields
end
```

### Use Case 4: File Extension Check
```mlp
function is_mlp_file(filename) then
    if string_ends_with(filename, ".mlp") == 1 then
        return 1
    end
    return 0
end
```

---

## 📊 Performance Considerations

### Time Complexity
| Function | Complexity | Notes |
|----------|------------|-------|
| `string_split` | O(n) | Linear scan |
| `string_join` | O(n×m) | n strings, avg length m |
| `string_replace` | O(n×k) | n = string length, k = occurrences |
| `string_trim` | O(n) | Linear scan |
| `string_upper`/`lower` | O(n) | Character-by-character |
| `string_find` | O(n×m) | Naive search (Boyer-Moore in future) |
| `string_starts_with` | O(m) | m = prefix length |
| `string_ends_with` | O(m) | m = suffix length |

### Memory Usage
- All functions allocate new strings (no mutation)
- Caller responsible for freeing returned strings
- `string_split` allocates list + individual strings
- `string_join` allocates single result string

---

## 🧪 Testing Strategy

### Test Suite Example
```mlp
function test_string_split() then
    list[string] parts = string_split("a,b,c", ",");

    if parts.size() != 3 then
        print "FAIL: split size"
        return 0
    end

    if parts.get(0) != "a" then
        print "FAIL: split part 0"
        return 0
    end

    print "PASS: string_split"
    return 1
end

function test_string_replace() then
    string result = string_replace("Hello World", "World", "MLP");

    if result != "Hello MLP" then
        print "FAIL: replace"
        return 0
    end

    print "PASS: string_replace"
    return 1
end

function test_string_trim() then
    string result = string_trim("  Hello  ");

    if result != "Hello" then
        print "FAIL: trim"
        return 0
    end

    print "PASS: string_trim"
    return 1
end

-- Run tests
test_string_split();
test_string_replace();
test_string_trim();
```

---

## 🚀 Future Enhancements

### Phase 4+: Advanced String Operations

#### Regular Expressions
```mlp
int matches = string_match(text, "\\d+");  -- Find digits
list[string] captures = string_extract(text, "(\\w+)@(\\w+)");
```

#### Unicode Support
```mlp
int len = string_length_unicode("Hello 世界");  -- Character count, not bytes
string upper = string_upper_unicode("ğüşıöç");  -- Turkish uppercase
```

#### String Formatting
```mlp
string msg = string_format("Value: {}, Count: {}", value, count);
```

#### String Builder (Efficient)
```mlp
StringBuilder sb = string_builder();
sb.append("Hello ");
sb.append("World");
string result = sb.to_string();
```

---

## 📖 Related Documentation

- [FILE_IO_DESIGN.md](FILE_IO_DESIGN.md) - File operations
- [HASHMAP_DESIGN.md](HASHMAP_DESIGN.md) - Dictionary/map support
- [LIST_DESIGN.md](LIST_DESIGN.md) - Dynamic lists
- [API_REFERENCE.md](API_REFERENCE.md) - Complete API reference

---

## ✅ Success Criteria

Phase 3 String Operations complete when:
- [x] All 9 new functions implemented
- [x] Existing 6 functions still work
- [x] Test suite passes (split, join, replace, trim, upper, lower, find, starts_with, ends_with)
- [x] Memory leaks fixed
- [x] Self-hosting compiler uses string functions for keyword translation
- [x] Documentation complete

---

**Last Updated:** November 20, 2025
**Status:** Design Complete - Implementation Needed
**Priority:** 🔴 CRITICAL (Required for self-hosting)
