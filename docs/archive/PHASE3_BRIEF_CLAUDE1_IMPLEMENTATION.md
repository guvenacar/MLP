# Phase 3 - Core Implementation Brief (Claude #1 - Coordinator)

**Session**: Phase 3 - Self-Hosting Completion
**Your Role**: Core Implementation & Coordination
**Branch**: `claude/phase3-implementation-[SESSION_ID]`
**Timeline**: ~4-5 hours

---

## 🎯 Your Mission

Implement core Phase 3 features:
1. File I/O runtime functions
2. String manipulation runtime
3. Compiler integration (lexer, parser, codegen)
4. Coordinate with Claude #2 and #3
5. Final integration and merge

---

## 📋 Phase 1: File I/O Implementation

### 1.1 Runtime Functions (`runtime/runtime.c`)

Add these functions to existing runtime.c:

```c
// ============================================
// Phase 3: File I/O Operations
// ============================================

#include <errno.h>

// Read entire file into string
char* read_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error opening file: %s\n", path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    // Read content
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';

    fclose(file);
    return buffer;
}

// Write string to file
int write_file(const char* path, const char* content) {
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Error writing file: %s\n", path);
        return -1;
    }

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);

    fclose(file);
    return (written == len) ? 0 : -1;
}

// Read file as lines
char** read_lines(const char* path, int* line_count) {
    char* content = read_file(path);
    if (!content) {
        *line_count = 0;
        return NULL;
    }

    // Count lines
    int count = 1;
    for (char* p = content; *p; p++) {
        if (*p == '\n') count++;
    }

    // Allocate line array
    char** lines = malloc(sizeof(char*) * count);
    int line_idx = 0;
    char* line_start = content;

    // Split by newlines
    for (char* p = content; *p; p++) {
        if (*p == '\n') {
            *p = '\0';
            lines[line_idx++] = strdup(line_start);
            line_start = p + 1;
        }
    }

    // Last line
    if (*line_start) {
        lines[line_idx++] = strdup(line_start);
    }

    free(content);
    *line_count = line_idx;
    return lines;
}

// Append to file
int append_file(const char* path, const char* content) {
    FILE* file = fopen(path, "a");
    if (!file) {
        return -1;
    }

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);

    fclose(file);
    return (written == len) ? 0 : -1;
}

// Check if file exists
int file_exists(const char* path) {
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Get file size
long file_size(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}
```

### 1.2 Lexer Updates (`c_compiler/c_lexer.c`)

Add keywords to keyword table:

```c
// File I/O keywords
{"read_file",    TOKEN_BUILTIN_READ_FILE},
{"write_file",   TOKEN_BUILTIN_WRITE_FILE},
{"read_lines",   TOKEN_BUILTIN_READ_LINES},
{"append_file",  TOKEN_BUILTIN_APPEND_FILE},
{"file_exists",  TOKEN_BUILTIN_FILE_EXISTS},
{"file_size",    TOKEN_BUILTIN_FILE_SIZE},
```

Add to enum in `c_lexer.h`:

```c
// Built-in functions - File I/O
TOKEN_BUILTIN_READ_FILE,
TOKEN_BUILTIN_WRITE_FILE,
TOKEN_BUILTIN_READ_LINES,
TOKEN_BUILTIN_APPEND_FILE,
TOKEN_BUILTIN_FILE_EXISTS,
TOKEN_BUILTIN_FILE_SIZE,
```

### 1.3 Parser Updates (`c_compiler/c_parser.c`)

Handle built-in file I/O calls in `birincil()`:

```c
// File I/O built-in functions
if (current_token->type == TOKEN_BUILTIN_READ_FILE ||
    current_token->type == TOKEN_BUILTIN_WRITE_FILE ||
    current_token->type == TOKEN_BUILTIN_APPEND_FILE ||
    current_token->type == TOKEN_BUILTIN_FILE_EXISTS ||
    current_token->type == TOKEN_BUILTIN_FILE_SIZE) {

    TokenType func_type = current_token->type;
    consume(current_token->type);

    consume(TOKEN_LEFT_PAREN);

    // Parse argument(s)
    ASTNode* arg1 = ifade();
    ASTNode* arg2 = NULL;

    if (current_token->type == TOKEN_COMMA) {
        consume(TOKEN_COMMA);
        arg2 = ifade();
    }

    consume(TOKEN_RIGHT_PAREN);

    ASTNode* call_node = createAST_BuiltinFileCall(func_type, arg1, arg2);
    return call_node;
}
```

### 1.4 AST Updates (`c_compiler/c_ast.h`)

Add new node type:

```c
typedef enum {
    // ... existing types ...
    AST_BUILTIN_FILE_CALL,  // Phase 3: File I/O
} ASTNodeType;

// Node data for file I/O calls
typedef struct {
    TokenType function_type;  // Which file function
    ASTNode* arg1;           // First argument (path)
    ASTNode* arg2;           // Second argument (content, if any)
} BuiltinFileCallData;

// Update union
typedef struct ASTNode {
    ASTNodeType type;
    union {
        // ... existing ...
        BuiltinFileCallData file_call_data;
    };
} ASTNode;
```

Add constructor:

```c
ASTNode* createAST_BuiltinFileCall(TokenType func_type, ASTNode* arg1, ASTNode* arg2) {
    ASTNode* node = malloc(sizeof(ASTNode));
    node->type = AST_BUILTIN_FILE_CALL;
    node->file_call_data.function_type = func_type;
    node->file_call_data.arg1 = arg1;
    node->file_call_data.arg2 = arg2;
    return node;
}
```

### 1.5 Code Generator (`c_compiler/c_generator.c`)

Add visitor function:

```c
void visit_BuiltinFileCall(ASTNode* node, FILE* output) {
    TokenType func = node->file_call_data.function_type;

    // Evaluate arguments
    if (node->file_call_data.arg1) {
        visit(node->file_call_data.arg1, output);
        fprintf(output, "    mov rdi, rax\n");  // First arg in rdi
    }

    if (node->file_call_data.arg2) {
        visit(node->file_call_data.arg2, output);
        fprintf(output, "    mov rsi, rax\n");  // Second arg in rsi
    }

    // Call appropriate function
    switch (func) {
        case TOKEN_BUILTIN_READ_FILE:
            fprintf(output, "    call read_file\n");
            break;
        case TOKEN_BUILTIN_WRITE_FILE:
            fprintf(output, "    call write_file\n");
            break;
        case TOKEN_BUILTIN_APPEND_FILE:
            fprintf(output, "    call append_file\n");
            break;
        case TOKEN_BUILTIN_FILE_EXISTS:
            fprintf(output, "    call file_exists\n");
            break;
        case TOKEN_BUILTIN_FILE_SIZE:
            fprintf(output, "    call file_size\n");
            break;
        default:
            fprintf(stderr, "Unknown file I/O function\n");
    }

    // Result in RAX
}
```

Update main visitor:

```c
void visit(ASTNode* node, FILE* output) {
    // ... existing cases ...

    case AST_BUILTIN_FILE_CALL:
        visit_BuiltinFileCall(node, output);
        break;
}
```

---

## 📋 Phase 2: String Operations

### 2.1 Runtime Implementation

Add to `runtime/runtime.c`:

```c
// ============================================
// Phase 3: String Operations
// ============================================

// Split string by delimiter
char** string_split(const char* str, const char* delim, int* count) {
    char* copy = strdup(str);
    int capacity = 10;
    char** parts = malloc(sizeof(char*) * capacity);
    *count = 0;

    char* token = strtok(copy, delim);
    while (token) {
        if (*count >= capacity) {
            capacity *= 2;
            parts = realloc(parts, sizeof(char*) * capacity);
        }
        parts[(*count)++] = strdup(token);
        token = strtok(NULL, delim);
    }

    free(copy);
    return parts;
}

// Join strings with delimiter
char* string_join(char** parts, int count, const char* delim) {
    if (count == 0) return strdup("");

    // Calculate total length
    size_t total_len = 0;
    size_t delim_len = strlen(delim);

    for (int i = 0; i < count; i++) {
        total_len += strlen(parts[i]);
        if (i < count - 1) total_len += delim_len;
    }

    // Allocate buffer
    char* result = malloc(total_len + 1);
    result[0] = '\0';

    // Join parts
    for (int i = 0; i < count; i++) {
        strcat(result, parts[i]);
        if (i < count - 1) strcat(result, delim);
    }

    return result;
}

// Replace substring
char* string_replace(const char* str, const char* old, const char* new) {
    char* result;
    int i, count = 0;
    size_t new_len = strlen(new);
    size_t old_len = strlen(old);

    // Count occurrences
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], old) == &str[i]) {
            count++;
            i += old_len - 1;
        }
    }

    // Allocate result
    result = malloc(i + count * (new_len - old_len) + 1);

    // Replace
    i = 0;
    while (*str) {
        if (strstr(str, old) == str) {
            strcpy(&result[i], new);
            i += new_len;
            str += old_len;
        } else {
            result[i++] = *str++;
        }
    }

    result[i] = '\0';
    return result;
}

// Trim whitespace
char* string_trim(const char* str) {
    while (isspace(*str)) str++;
    if (*str == '\0') return strdup("");

    const char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;

    size_t len = end - str + 1;
    char* result = malloc(len + 1);
    memcpy(result, str, len);
    result[len] = '\0';
    return result;
}

// Convert to uppercase
char* string_upper(const char* str) {
    char* result = strdup(str);
    for (char* p = result; *p; p++) {
        *p = toupper(*p);
    }
    return result;
}

// Convert to lowercase
char* string_lower(const char* str) {
    char* result = strdup(str);
    for (char* p = result; *p; p++) {
        *p = tolower(*p);
    }
    return result;
}

// Find substring
int string_find(const char* str, const char* needle) {
    const char* pos = strstr(str, needle);
    return pos ? (pos - str) : -1;
}

// Starts with
int string_starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0 ? 1 : 0;
}

// Ends with
int string_ends_with(const char* str, const char* suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) return 0;
    return strcmp(str + str_len - suffix_len, suffix) == 0 ? 1 : 0;
}
```

### 2.2 Compiler Integration

Similar to file I/O - add tokens, parse, generate code for:
- `string_split()`, `string_join()`
- `string_replace()`, `string_trim()`
- `string_upper()`, `string_lower()`
- `string_find()`, `string_starts_with()`, `string_ends_with()`

---

## 📋 Phase 3: Integration Testing

### 3.1 Compile Test Program

Create `test/test_phase3_integration.mlp`:

```mlp
-- Phase 3 integration test

function test_file_and_string() then
    -- Write file
    string content = "Hello, MLP World!";
    write_file("/tmp/test.txt", content);

    -- Read back
    string read = read_file("/tmp/test.txt");

    -- Transform
    string upper = string_upper(read);

    -- Write result
    write_file("/tmp/result.txt", upper);

    print "✅ Integration test passed";
    return 0
end

test_file_and_string()
```

Compile and run:
```bash
cd c_compiler
./compiler_test ../test/test_phase3_integration.mlp ../test/test_phase3_integration.asm

# Link with runtime
gcc -o test_phase3 ../test/test_phase3_integration.asm ../runtime/runtime.c
./test_phase3
```

---

## 📋 Phase 4: Coordination & Merge

### 4.1 Check Other Branches

```bash
# Check documentation (Claude #2)
git fetch origin
git checkout origin/claude/phase3-documentation-[ID]
# Review docs

# Check testing (Claude #3)
git checkout origin/claude/phase3-testing-[ID]
# Run tests
```

### 4.2 Integrate All Work

Merge order:
1. Your implementation (runtime + compiler)
2. Claude #3's tests
3. Claude #2's documentation

### 4.3 Final Verification

- All tests pass
- Documentation complete
- Example programs work
- Ready for main branch merge

---

## 📂 File Structure

Your changes:
```
/home/user/MLP/
├── runtime/
│   └── runtime.c              (MODIFIED: +500 lines)
├── c_compiler/
│   ├── c_lexer.c              (MODIFIED: +20 lines)
│   ├── c_lexer.h              (MODIFIED: +15 lines)
│   ├── c_parser.c             (MODIFIED: +100 lines)
│   ├── c_ast.h                (MODIFIED: +30 lines)
│   └── c_generator.c          (MODIFIED: +150 lines)
├── test/
│   └── test_phase3_integration.mlp (NEW)
└── PHASE3_IMPLEMENTATION_SUMMARY.md (NEW)
```

---

## ✅ Success Criteria

1. **File I/O working** (6 functions implemented)
2. **String ops working** (9 functions implemented)
3. **Compiler integration** complete
4. **Integration tests** passing
5. **Coordination** with other Claudes successful
6. **Documentation** of implementation

---

## 🎯 Timeline

**Hour 1-2**: File I/O runtime + compiler integration
**Hour 3**: String operations
**Hour 4**: Integration testing
**Hour 5**: Coordination & merge preparation

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase3-implementation-[SESSION_ID]

# Commit frequently
git add runtime/runtime.c
git commit -m "feat: Add file I/O runtime functions"

git add c_compiler/c_lexer.* c_compiler/c_parser.c
git commit -m "feat: Add file I/O compiler support"

git push -u origin claude/phase3-implementation-[SESSION_ID]
```

---

## 🎉 When You're Done

Report:
1. Features implemented (X/15 functions)
2. Tests passing
3. Integration status
4. Ready for merge

**Branch**: `claude/phase3-implementation-[SESSION_ID]`
**Expected**: 10-15 commits
**Time**: 4-5 hours

Let's make MLP self-hosting! 🚀
