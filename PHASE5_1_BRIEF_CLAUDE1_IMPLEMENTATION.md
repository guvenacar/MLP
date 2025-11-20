# Phase 5.1 - Implementation Brief (Claude #1)

**Session**: Phase 5.1 - Command-Line Args & Enhanced Strings
**Your Role**: Core Implementation
**Branch**: `claude/phase5-1-implementation-[SESSION_ID]`
**Timeline**: ~2 weeks

---

## 🎯 Your Mission

Implement Phase 5.1 features for self-hosting preparation:
1. Command-line argument support (argc, argv)
2. Enhanced string operations (4 new functions)
3. Full compiler integration (lexer, parser, AST, codegen)

**Total Expected**: ~400-500 lines of code

---

## 📋 Part 1: Command-Line Arguments

### 1.1 Main Function Enhancement

**Goal**: Support `main(argc, argv)` signature for accessing command-line arguments.

**Current State**:
```mlp
-- No parameters supported
function main() then
    print "Hello"
end
```

**Target State**:
```mlp
function main(argc, argv) then
    int i = 0;
    print "Arguments:"
    while i < argc then
        string arg = argv.get(i);
        print arg
        i = i + 1
    end
end
```

### 1.2 Implementation Details

**Runtime (runtime/runtime.c)**:

Add global variables for argc/argv storage:
```c
// Global argc/argv storage
static int mlp_argc = 0;
static char** mlp_argv = NULL;

/**
 * mlp_set_args - Store command-line arguments
 * Called by main() before executing MLP code
 */
void mlp_set_args(int argc, char** argv) {
    mlp_argc = argc;
    mlp_argv = argv;
}

/**
 * mlp_get_argc - Get argument count
 * @return: Number of command-line arguments
 *
 * MLP Usage: int count = argc;
 */
long mlp_get_argc(void) {
    return mlp_argc;
}

/**
 * mlp_get_argv - Get argument at index
 * @param index: Argument index (0 = program name)
 * @return: Argument string, or empty string if out of bounds
 *
 * MLP Usage: string arg = argv.get(0);
 */
char* mlp_get_argv(long index) {
    if (index < 0 || index >= mlp_argc) {
        return strdup("");
    }
    return strdup(mlp_argv[index]);
}
```

**Lexer (c_compiler/c_lexer.h)**:

No new tokens needed - `argc` and `argv` will be identifiers.

**Parser (c_compiler/c_parser.c)**:

Modify function parameter parsing to recognize `argc` and `argv` as special identifiers:
```c
// In parse_function():
// If function name is "main" and has 2 parameters named "argc" and "argv":
//   - Mark function as main with args
//   - Store parameter info
```

**Code Generator (c_compiler/c_generator.c)**:

Modify `visit_Degisken()` to handle `argc` and `argv`:
```c
void visit_Degisken(ASTNode* node) {
    char* var_name = node->degisken_data.ad->value;

    // Special handling for argc/argv
    if (strcmp(var_name, "argc") == 0 && is_in_main_function()) {
        asm_append(&text_section, "    call mlp_get_argc");
        return;
    }

    // Normal variable handling
    ...
}
```

For `argv.get(index)`:
- Parse as special list-like access
- Generate call to `mlp_get_argv(index)`

### 1.3 Main Wrapper Generation

Modify code generator to create proper C main:
```asm
global main
extern mlp_set_args
extern mlp_main  ; User's main function

main:
    ; Save argc/argv
    mov rdi, [rsp + 8]      ; argc (first argument)
    lea rsi, [rsp + 16]     ; argv (second argument)
    call mlp_set_args

    ; Call user's main
    call mlp_main

    ; Return 0
    xor rax, rax
    ret
```

**Estimated**: ~100 lines

---

## 📋 Part 2: Enhanced String Operations

### 2.1 Runtime Functions (runtime/runtime.c)

Add 4 new string functions:

```c
/**
 * string_length - Get string length
 * @param str: Input string
 * @return: Length of string
 *
 * MLP Usage: int len = string_length("hello");
 */
long string_length(const char* str) {
    if (!str) return 0;
    return strlen(str);
}

/**
 * string_substring - Extract substring
 * @param str: Input string
 * @param start: Start index (0-based)
 * @param length: Number of characters
 * @return: Substring (caller must free)
 *
 * MLP Usage: string sub = string_substring("hello", 1, 3); // "ell"
 */
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
        fprintf(stderr, "Memory allocation failed in string_substring\n");
        exit(1);
    }

    strncpy(result, str + start, length);
    result[length] = '\0';
    return result;
}

/**
 * string_index_of - Find first occurrence of substring
 * @param str: String to search in
 * @param needle: Substring to find
 * @return: Index of first occurrence, or -1 if not found
 *
 * MLP Usage: int pos = string_index_of("hello world", "world"); // 6
 */
long string_index_of(const char* str, const char* needle) {
    if (!str || !needle) return -1;

    char* found = strstr(str, needle);
    if (!found) return -1;

    return found - str;
}

/**
 * string_last_index_of - Find last occurrence of substring
 * @param str: String to search in
 * @param needle: Substring to find
 * @return: Index of last occurrence, or -1 if not found
 *
 * MLP Usage: int pos = string_last_index_of("hello hello", "hello"); // 6
 */
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

**Estimated**: ~80 lines

---

## 📋 Part 3: Compiler Integration

### 3.1 Lexer Updates (c_compiler/c_lexer.h + c_lexer.c)

Add tokens for new built-in functions:
```c
// In c_lexer.h
typedef enum {
    ...
    // Phase 5.1: Enhanced String Operations
    TOKEN_BUILTIN_STRING_LENGTH,
    TOKEN_BUILTIN_STRING_SUBSTRING,
    TOKEN_BUILTIN_STRING_INDEX_OF,
    TOKEN_BUILTIN_STRING_LAST_INDEX_OF,
    ...
} TokenType;
```

Add keyword mappings in `c_lexer.c`:
```c
// Phase 5.1: Enhanced String Operations
{"string_length",        TOKEN_BUILTIN_STRING_LENGTH},
{"string_substring",     TOKEN_BUILTIN_STRING_SUBSTRING},
{"string_index_of",      TOKEN_BUILTIN_STRING_INDEX_OF},
{"string_last_index_of", TOKEN_BUILTIN_STRING_LAST_INDEX_OF},
```

**Estimated**: ~10 lines

---

### 3.2 Parser Updates (c_compiler/c_parser.c)

Add new built-in function types to `birincil()`:
```c
if (current_token->type == TOKEN_BUILTIN_STRING_LENGTH ||
    current_token->type == TOKEN_BUILTIN_STRING_SUBSTRING ||
    current_token->type == TOKEN_BUILTIN_STRING_INDEX_OF ||
    current_token->type == TOKEN_BUILTIN_STRING_LAST_INDEX_OF) {

    TokenType func_type = current_token->type;
    consume(current_token->type);
    consume(TOKEN_LEFT_PAREN);

    ASTNode* arg1 = ifade();
    ASTNode* arg2 = NULL;
    ASTNode* arg3 = NULL;

    if (current_token->type == TOKEN_COMMA) {
        consume(TOKEN_COMMA);
        arg2 = ifade();

        if (current_token->type == TOKEN_COMMA) {
            consume(TOKEN_COMMA);
            arg3 = ifade();
        }
    }

    consume(TOKEN_RIGHT_PAREN);
    return createAST_BuiltinCall(func_type, arg1, arg2, arg3);
}
```

**Estimated**: ~15 lines

---

### 3.3 Code Generator Updates (c_compiler/c_generator.c)

Add new function name mappings in `visit_BuiltinCall()`:
```c
switch (func) {
    ...
    // Phase 5.1: Enhanced String Operations
    case TOKEN_BUILTIN_STRING_LENGTH: func_name = "string_length"; break;
    case TOKEN_BUILTIN_STRING_SUBSTRING: func_name = "string_substring"; break;
    case TOKEN_BUILTIN_STRING_INDEX_OF: func_name = "string_index_of"; break;
    case TOKEN_BUILTIN_STRING_LAST_INDEX_OF: func_name = "string_last_index_of"; break;
    ...
}
```

**Estimated**: ~10 lines

---

## 📋 Part 4: Integration Test

### 4.1 Create test/test_phase5_1_integration.mlp

```mlp
-- Phase 5.1 Integration Test

function test_string_length() then
    string text = "Hello World";
    int len = 0;

    print "=== String Length Test ==="

    len = string_length(text)
    print "Length of 'Hello World': "
    print len

    len = string_length("")
    print "Length of empty string: "
    print len

    return 0
end

function test_string_substring() then
    string text = "Hello World";
    string sub = "";

    print "=== Substring Test ==="

    sub = string_substring(text, 0, 5)
    print "Substring(0, 5): "
    print sub

    sub = string_substring(text, 6, 5)
    print "Substring(6, 5): "
    print sub

    return 0
end

function test_string_index_of() then
    string text = "Hello World Hello";
    int pos = 0;

    print "=== Index Of Test ==="

    pos = string_index_of(text, "World")
    print "First 'World' at: "
    print pos

    pos = string_index_of(text, "xyz")
    print "Not found: "
    print pos

    return 0
end

function test_string_last_index_of() then
    string text = "Hello World Hello";
    int pos = 0;

    print "=== Last Index Of Test ==="

    pos = string_last_index_of(text, "Hello")
    print "Last 'Hello' at: "
    print pos

    return 0
end

function test_cmdline_args() then
    print "=== Command-Line Args Test ==="
    print "This test requires argc/argv support"
    print "Run with: ./program arg1 arg2 arg3"
    return 0
end

print "==================================="
print "Phase 5.1 Integration Tests"
print "==================================="
print ""

test_string_length()
print ""

test_string_substring()
print ""

test_string_index_of()
print ""

test_string_last_index_of()
print ""

test_cmdline_args()
print ""

print "==================================="
print "All Tests Complete!"
print "==================================="
```

**Estimated**: ~100 lines

---

## 📂 File Summary

Your deliverables:
```
/home/user/MLP/
├── runtime/runtime.c                     (MODIFIED: +180 lines)
├── c_compiler/c_lexer.h                  (MODIFIED: +4 lines)
├── c_compiler/c_lexer.c                  (MODIFIED: +4 lines)
├── c_compiler/c_parser.c                 (MODIFIED: +15 lines)
├── c_compiler/c_generator.c              (MODIFIED: +20 lines)
├── test/test_phase5_1_integration.mlp    (NEW: ~100 lines)
└── test/test_cmdline_args.mlp            (NEW: ~30 lines)
```

**Total**: ~350 lines of implementation

---

## ✅ Success Criteria

1. ✅ Runtime functions compile and link
2. ✅ All 4 string functions work correctly
3. ✅ Command-line args accessible in main()
4. ✅ Integration test compiles
5. ✅ No breaking changes to existing code

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase5-1-implementation-[SESSION_ID]

# Step 1: Runtime functions
git add runtime/runtime.c
git commit -m "feat(phase5.1): Add enhanced string operations and argc/argv support"

# Step 2: Compiler integration
git add c_compiler/c_lexer.h c_compiler/c_lexer.c
git commit -m "feat(phase5.1): Add lexer tokens for new built-ins"

git add c_compiler/c_parser.c
git commit -m "feat(phase5.1): Add parser support for new built-ins"

git add c_compiler/c_generator.c
git commit -m "feat(phase5.1): Add codegen for new built-ins"

# Step 3: Tests
git add test/test_phase5_1_integration.mlp
git commit -m "test(phase5.1): Add integration tests"

git push -u origin claude/phase5-1-implementation-[SESSION_ID]
```

---

## 🎯 Implementation Order

**Day 1-2**: Runtime functions
- string_length, string_substring
- string_index_of, string_last_index_of
- argc/argv functions

**Day 3-4**: Compiler integration
- Lexer tokens
- Parser updates
- Code generation

**Day 5-6**: Testing
- Integration tests
- Command-line args test
- Verification

**Day 7**: Polish
- Bug fixes
- Documentation comments
- Final testing

---

## 💡 Tips

1. **argc/argv handling**: Keep it simple - treat as special identifiers in main()
2. **String safety**: Always check for NULL pointers
3. **Memory management**: All string returns must be malloc'd
4. **Testing**: Test edge cases (empty strings, out of bounds, etc.)

---

## 🎉 When You're Done

Report:
1. Functions implemented (X/8)
2. Compiler integration complete
3. Tests passing
4. Branch pushed

**Branch**: `claude/phase5-1-implementation-[SESSION_ID]`
**Expected**: 5-7 commits

Let's implement Phase 5.1! 🚀
