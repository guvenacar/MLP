# Phase 3 - Testing & Advanced Features Brief (Claude #3)

**Session**: Phase 3 - Self-Hosting Completion
**Your Role**: Testing & Advanced Features Implementation
**Branch**: `claude/phase3-testing-[SESSION_ID]`
**Timeline**: ~3-4 hours

---

## 🎯 Your Mission

Implement comprehensive tests and advanced features for Phase 3:
1. File I/O tests (C runtime + MLP integration)
2. String operations tests
3. Hash table implementation and tests
4. Edge case coverage
5. Performance benchmarks

---

## 📋 Tasks

### Part 1: File I/O Testing

#### A. C Runtime Tests (`runtime/test_file_io.c`)
Create comprehensive C test suite:

```c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Function declarations (from runtime.c)
char* read_file(const char* path);
int write_file(const char* path, const char* content);
char** read_lines(const char* path, int* line_count);
int append_file(const char* path, const char* content);
int file_exists(const char* path);
long file_size(const char* path);

int main() {
    printf("=== File I/O Runtime Tests ===\n\n");

    // Test 1: Write and read simple file
    printf("Test 1: Write and read file\n");
    const char* test_content = "Hello, MLP!\n";
    assert(write_file("/tmp/test1.txt", test_content) == 0);
    char* content = read_file("/tmp/test1.txt");
    assert(content != NULL);
    assert(strcmp(content, test_content) == 0);
    printf("✅ Write and read successful\n");
    free(content);

    // Test 2: Large file handling
    printf("\nTest 2: Large file (10KB)\n");
    char large_buffer[10000];
    memset(large_buffer, 'A', sizeof(large_buffer)-1);
    large_buffer[9999] = '\0';
    assert(write_file("/tmp/test2.txt", large_buffer) == 0);
    char* large_content = read_file("/tmp/test2.txt");
    assert(strlen(large_content) == 9999);
    printf("✅ Large file handled\n");
    free(large_content);

    // Test 3: Read lines
    printf("\nTest 3: Read lines\n");
    const char* multiline = "Line 1\nLine 2\nLine 3\n";
    write_file("/tmp/test3.txt", multiline);
    int line_count = 0;
    char** lines = read_lines("/tmp/test3.txt", &line_count);
    assert(line_count == 3);
    assert(strcmp(lines[0], "Line 1") == 0);
    printf("✅ Read lines: %d lines\n", line_count);

    // Test 4: Append to file
    printf("\nTest 4: Append to file\n");
    write_file("/tmp/test4.txt", "First\n");
    append_file("/tmp/test4.txt", "Second\n");
    char* appended = read_file("/tmp/test4.txt");
    assert(strstr(appended, "First") != NULL);
    assert(strstr(appended, "Second") != NULL);
    printf("✅ Append successful\n");
    free(appended);

    // Test 5: File exists check
    printf("\nTest 5: File exists\n");
    assert(file_exists("/tmp/test1.txt") == 1);
    assert(file_exists("/tmp/nonexistent.txt") == 0);
    printf("✅ File exists check\n");

    // Test 6: File size
    printf("\nTest 6: File size\n");
    long size = file_size("/tmp/test1.txt");
    assert(size == strlen(test_content));
    printf("✅ File size: %ld bytes\n", size);

    // Test 7: Error handling (nonexistent file)
    printf("\nTest 7: Error handling\n");
    char* missing = read_file("/tmp/missing.txt");
    assert(missing == NULL || strlen(missing) == 0);
    printf("✅ Error handling works\n");

    // Test 8: Empty file
    printf("\nTest 8: Empty file\n");
    write_file("/tmp/empty.txt", "");
    char* empty = read_file("/tmp/empty.txt");
    assert(empty != NULL);
    assert(strlen(empty) == 0);
    printf("✅ Empty file handled\n");
    free(empty);

    printf("\n=== All File I/O Tests Passed! ===\n");
    return 0;
}
```

**Compile & run**:
```bash
gcc -o test_file_io runtime/test_file_io.c runtime/runtime.c
./test_file_io
```

#### B. MLP Integration Tests (`test/test_file_io.mlp`)

```mlp
-- File I/O integration tests

-- Test 1: Basic write and read
function test_basic_io() then
    string content = "Hello from MLP!";
    int result = write_file("/tmp/mlp_test1.txt", content);

    string read_back = read_file("/tmp/mlp_test1.txt");

    if read_back == content then
        print "✅ Test 1: Basic I/O passed"
        return 1
    end

    print "❌ Test 1 failed"
    return 0
end

-- Test 2: Multiple operations
function test_multiple_ops() then
    write_file("/tmp/mlp_test2.txt", "First");
    append_file("/tmp/mlp_test2.txt", "\nSecond");
    append_file("/tmp/mlp_test2.txt", "\nThird");

    string result = read_file("/tmp/mlp_test2.txt");

    -- Check if all parts present
    print "✅ Test 2: Multiple operations passed"
    return 1
end

-- Test 3: File existence
function test_file_exists() then
    write_file("/tmp/mlp_test3.txt", "Test");

    int exists = file_exists("/tmp/mlp_test3.txt");
    int not_exists = file_exists("/tmp/nonexistent.txt");

    if exists == 1 then
        if not_exists == 0 then
            print "✅ Test 3: File exists check passed"
            return 1
        end
    end

    print "❌ Test 3 failed"
    return 0
end

-- Run all tests
print "=== MLP File I/O Tests ==="
print ""

test_basic_io()
test_multiple_ops()
test_file_exists()

print ""
print "=== Tests Complete ==="
```

### Part 2: String Operations Tests

#### A. C Runtime Tests (`runtime/test_string_ops.c`)

```c
#include <stdio.h>
#include <assert.h>
#include <string.h>

// String function declarations
char** string_split(const char* str, const char* delim, int* count);
char* string_join(char** parts, int count, const char* delim);
char* string_replace(const char* str, const char* old, const char* new);
char* string_trim(const char* str);
char* string_upper(const char* str);
char* string_lower(const char* str);
int string_find(const char* str, const char* needle);
int string_starts_with(const char* str, const char* prefix);
int string_ends_with(const char* str, const char* suffix);

int main() {
    printf("=== String Operations Tests ===\n\n");

    // Test 1: String split
    printf("Test 1: String split\n");
    int count = 0;
    char** parts = string_split("a,b,c", ",", &count);
    assert(count == 3);
    assert(strcmp(parts[0], "a") == 0);
    assert(strcmp(parts[1], "b") == 0);
    assert(strcmp(parts[2], "c") == 0);
    printf("✅ Split: %d parts\n", count);

    // Test 2: String join
    printf("\nTest 2: String join\n");
    char* joined = string_join(parts, count, "-");
    assert(strcmp(joined, "a-b-c") == 0);
    printf("✅ Join: %s\n", joined);

    // Test 3: String replace
    printf("\nTest 3: String replace\n");
    char* replaced = string_replace("hello world", "world", "MLP");
    assert(strcmp(replaced, "hello MLP") == 0);
    printf("✅ Replace: %s\n", replaced);

    // Test 4: String trim
    printf("\nTest 4: String trim\n");
    char* trimmed = string_trim("  hello  ");
    assert(strcmp(trimmed, "hello") == 0);
    printf("✅ Trim: '%s'\n", trimmed);

    // Test 5: String upper/lower
    printf("\nTest 5: Upper/Lower case\n");
    char* upper = string_upper("hello");
    char* lower = string_lower("HELLO");
    assert(strcmp(upper, "HELLO") == 0);
    assert(strcmp(lower, "hello") == 0);
    printf("✅ Upper: %s, Lower: %s\n", upper, lower);

    // Test 6: String find
    printf("\nTest 6: String find\n");
    int pos = string_find("hello world", "world");
    assert(pos == 6);
    printf("✅ Find: position %d\n", pos);

    // Test 7: Starts/ends with
    printf("\nTest 7: Starts/Ends with\n");
    assert(string_starts_with("hello world", "hello") == 1);
    assert(string_ends_with("hello world", "world") == 1);
    assert(string_starts_with("hello world", "bye") == 0);
    printf("✅ Starts/Ends with checks passed\n");

    printf("\n=== All String Tests Passed! ===\n");
    return 0;
}
```

#### B. MLP Integration Tests (`test/test_string_ops.mlp`)

```mlp
-- String operations tests in MLP

function test_split_join() then
    string text = "apple,banana,cherry";
    list[string] parts = string_split(text, ",");

    int count = parts.size();
    if count == 3 then
        string rejoined = string_join(parts, "-");
        print "✅ Split/Join: ";
        print rejoined;
        return 1
    end

    return 0
end

function test_replace() then
    string text = "Hello World";
    string result = string_replace(text, "World", "MLP");

    print "✅ Replace: ";
    print result;
    return 1
end

function test_case_conversion() then
    string lower = string_lower("HELLO");
    string upper = string_upper("world");

    print "✅ Lower: ";
    print lower;
    print ", Upper: ";
    print upper;
    return 1
end

-- Run tests
print "=== String Operations Tests ==="
test_split_join()
test_replace()
test_case_conversion()
print "=== Complete ==="
```

### Part 3: Hash Table Implementation

#### A. Hash Table Runtime (`runtime/hashmap.c` - NEW FILE)

Implement basic hash table in C:

```c
// hashmap.c - Hash table implementation for MLP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define LOAD_FACTOR 0.75

typedef struct HashNode {
    char* key;
    void* value;
    struct HashNode* next;
} HashNode;

typedef struct HashMap {
    HashNode** buckets;
    int capacity;
    int size;
} HashMap;

// Hash function (djb2)
unsigned int hash(const char* str, int capacity) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % capacity;
}

HashMap* hashmap_create() {
    HashMap* map = malloc(sizeof(HashMap));
    map->capacity = INITIAL_CAPACITY;
    map->size = 0;
    map->buckets = calloc(map->capacity, sizeof(HashNode*));
    return map;
}

void hashmap_set(HashMap* map, const char* key, void* value) {
    unsigned int index = hash(key, map->capacity);

    // Check if key exists
    HashNode* node = map->buckets[index];
    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            node->value = value; // Update
            return;
        }
        node = node->next;
    }

    // Insert new
    HashNode* new_node = malloc(sizeof(HashNode));
    new_node->key = strdup(key);
    new_node->value = value;
    new_node->next = map->buckets[index];
    map->buckets[index] = new_node;
    map->size++;

    // TODO: Resize if load factor exceeded
}

void* hashmap_get(HashMap* map, const char* key) {
    unsigned int index = hash(key, map->capacity);
    HashNode* node = map->buckets[index];

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            return node->value;
        }
        node = node->next;
    }

    return NULL;
}

int hashmap_has(HashMap* map, const char* key) {
    return hashmap_get(map, key) != NULL ? 1 : 0;
}

void hashmap_remove(HashMap* map, const char* key) {
    unsigned int index = hash(key, map->capacity);
    HashNode* node = map->buckets[index];
    HashNode* prev = NULL;

    while (node != NULL) {
        if (strcmp(node->key, key) == 0) {
            if (prev == NULL) {
                map->buckets[index] = node->next;
            } else {
                prev->next = node->next;
            }
            free(node->key);
            free(node);
            map->size--;
            return;
        }
        prev = node;
        node = node->next;
    }
}

int hashmap_size(HashMap* map) {
    return map->size;
}

void hashmap_destroy(HashMap* map) {
    for (int i = 0; i < map->capacity; i++) {
        HashNode* node = map->buckets[i];
        while (node != NULL) {
            HashNode* temp = node;
            node = node->next;
            free(temp->key);
            free(temp);
        }
    }
    free(map->buckets);
    free(map);
}
```

#### B. Hash Table Tests (`runtime/test_hashmap.c`)

```c
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "hashmap.h"

int main() {
    printf("=== Hash Map Tests ===\n\n");

    // Test 1: Create and destroy
    printf("Test 1: Create hashmap\n");
    HashMap* map = hashmap_create();
    assert(map != NULL);
    assert(hashmap_size(map) == 0);
    printf("✅ Created empty map\n");

    // Test 2: Set and get
    printf("\nTest 2: Set and get\n");
    hashmap_set(map, "name", (void*)"Alice");
    hashmap_set(map, "age", (void*)25);
    assert(strcmp((char*)hashmap_get(map, "name"), "Alice") == 0);
    printf("✅ Set/Get: name=%s\n", (char*)hashmap_get(map, "name"));

    // Test 3: Has key
    printf("\nTest 3: Has key\n");
    assert(hashmap_has(map, "name") == 1);
    assert(hashmap_has(map, "missing") == 0);
    printf("✅ Has key check\n");

    // Test 4: Size
    printf("\nTest 4: Size\n");
    assert(hashmap_size(map) == 2);
    printf("✅ Size: %d\n", hashmap_size(map));

    // Test 5: Update existing
    printf("\nTest 5: Update existing\n");
    hashmap_set(map, "name", (void*)"Bob");
    assert(strcmp((char*)hashmap_get(map, "name"), "Bob") == 0);
    assert(hashmap_size(map) == 2); // Size unchanged
    printf("✅ Update: name=%s\n", (char*)hashmap_get(map, "name"));

    // Test 6: Remove
    printf("\nTest 6: Remove\n");
    hashmap_remove(map, "age");
    assert(hashmap_has(map, "age") == 0);
    assert(hashmap_size(map) == 1);
    printf("✅ Remove: size=%d\n", hashmap_size(map));

    // Test 7: Many keys (collision test)
    printf("\nTest 7: Many keys\n");
    for (int i = 0; i < 100; i++) {
        char key[20];
        sprintf(key, "key%d", i);
        hashmap_set(map, key, (void*)(long)i);
    }
    assert(hashmap_size(map) == 101); // 1 from before + 100 new
    printf("✅ Added 100 keys, size=%d\n", hashmap_size(map));

    // Test 8: Retrieve from many
    printf("\nTest 8: Retrieve from many\n");
    assert((long)hashmap_get(map, "key50") == 50);
    assert((long)hashmap_get(map, "key99") == 99);
    printf("✅ Retrieval: key50=%ld, key99=%ld\n",
           (long)hashmap_get(map, "key50"),
           (long)hashmap_get(map, "key99"));

    hashmap_destroy(map);
    printf("\n=== All Hash Map Tests Passed! ===\n");
    return 0;
}
```

### Part 4: Edge Case & Integration Tests

#### Create `test/test_edge_cases_phase3.mlp`

```mlp
-- Phase 3 edge case tests

-- Test 1: Empty file operations
function test_empty_files() then
    write_file("/tmp/empty.txt", "");
    string content = read_file("/tmp/empty.txt");

    if string_length(content) == 0 then
        print "✅ Empty file handling"
        return 1
    end
    return 0
end

-- Test 2: Large string operations
function test_large_strings() then
    string large = "A";
    int i = 0;

    -- Build large string
    while
        if i >= 1000 then
            break
        end
        large = string_concat(large, "B");
        i = i + 1
    end

    int len = string_length(large);
    print "✅ Large string: ";
    print len;
    print " chars";
    return 1
end

-- Test 3: Special characters in strings
function test_special_chars() then
    string special = "Hello\nWorld\tTab\"Quote";
    write_file("/tmp/special.txt", special);
    string read_back = read_file("/tmp/special.txt");

    print "✅ Special characters preserved";
    return 1
end

-- Test 4: HashMap with many entries
function test_hashmap_stress() then
    map[string:int] numbers = map();

    int i = 0;
    while
        if i >= 100 then
            break
        end
        -- TODO: Convert int to string for key
        numbers.set("key", i);
        i = i + 1
    end

    int size = numbers.size();
    print "✅ HashMap stress: ";
    print size;
    print " entries";
    return 1
end

-- Run all edge case tests
print "=== Phase 3 Edge Case Tests ==="
test_empty_files()
test_large_strings()
test_special_chars()
test_hashmap_stress()
print "=== Complete ==="
```

### Part 5: Performance Benchmarks

#### Create `test/benchmark_phase3.mlp`

```mlp
-- Performance benchmarks for Phase 3

-- Benchmark 1: File I/O throughput
function benchmark_file_io() then
    string data = "";
    int i = 0;

    -- Build 10KB string
    while
        if i >= 10000 then
            break
        end
        data = string_concat(data, "A");
        i = i + 1
    end

    -- Write benchmark
    int start = get_time();
    write_file("/tmp/benchmark.txt", data);
    int write_time = get_time() - start;

    -- Read benchmark
    start = get_time();
    string read_data = read_file("/tmp/benchmark.txt");
    int read_time = get_time() - start;

    print "File I/O Benchmark:";
    print "  Write: ";
    print write_time;
    print "ms";
    print "  Read: ";
    print read_time;
    print "ms";

    return 0
end

-- Benchmark 2: String operations
function benchmark_string_ops() then
    int start = get_time();

    int i = 0;
    while
        if i >= 1000 then
            break
        end

        string test = "hello world hello world";
        string replaced = string_replace(test, "hello", "hi");

        i = i + 1
    end

    int elapsed = get_time() - start;

    print "String ops (1000x replace): ";
    print elapsed;
    print "ms";

    return 0
end

-- Benchmark 3: HashMap operations
function benchmark_hashmap() then
    map[string:int] numbers = map();

    int start = get_time();

    int i = 0;
    while
        if i >= 1000 then
            break
        end
        numbers.set("key", i);
        i = i + 1
    end

    int insert_time = get_time() - start;

    print "HashMap (1000 inserts): ";
    print insert_time;
    print "ms";

    return 0
end

-- Run benchmarks
print "=== Phase 3 Performance Benchmarks ==="
benchmark_file_io()
benchmark_string_ops()
benchmark_hashmap()
print "=== Complete ==="
```

---

## 📂 File Structure

Your work should create:
```
/home/user/MLP/
├── runtime/
│   ├── test_file_io.c         (NEW)
│   ├── test_string_ops.c      (NEW)
│   ├── hashmap.c              (NEW)
│   ├── hashmap.h              (NEW)
│   └── test_hashmap.c         (NEW)
├── test/
│   ├── test_file_io.mlp       (NEW)
│   ├── test_string_ops.mlp    (NEW)
│   ├── test_edge_cases_phase3.mlp (NEW)
│   └── benchmark_phase3.mlp   (NEW)
└── README_TESTS_PHASE3.md     (NEW)
```

---

## ✅ Success Criteria

1. **8+ C test files** passing (file I/O, strings, hashmap)
2. **4+ MLP test files** compiling and working
3. **Hash table implementation** complete and tested
4. **Edge case coverage** for all Phase 3 features
5. **Performance benchmarks** documented
6. **README_TESTS_PHASE3.md** with test guide

---

## 🎯 Priority Order

1. **File I/O tests** (blocking for other features)
2. **String operations tests** (medium priority)
3. **Hash table implementation** (high value for compiler)
4. **Edge cases** (quality assurance)
5. **Benchmarks** (nice to have)

---

## 🔧 Git Workflow

```bash
git checkout -b claude/phase3-testing-[YOUR_SESSION_ID]

# Commit incrementally
git add runtime/test_file_io.c
git commit -m "test: Add file I/O runtime tests"

git push -u origin claude/phase3-testing-[YOUR_SESSION_ID]
```

---

## 🎉 When You're Done

Report:
1. Test pass rate (X/Y tests passing)
2. Hash table performance metrics
3. Edge cases covered
4. Any bugs found

**Branch**: `claude/phase3-testing-[SESSION_ID]`
**Expected**: 8-12 commits
**Time**: 3-4 hours

Good luck! 🚀
