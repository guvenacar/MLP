#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

// Test macros
#define TEST_START(name) printf("\n=== Test %d: %s ===\n", ++test_count, name)
#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        fprintf(stderr, "❌ FAIL: Expected %ld, got %ld\n", (long)(b), (long)(a)); \
        failed_count++; \
        return; \
    } \
} while(0)

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "❌ FAIL: Expression false: %s\n", #expr); \
        failed_count++; \
        return; \
    } \
} while(0)

#define ASSERT_NOT_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "❌ FAIL: NULL pointer\n"); \
        failed_count++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(a, b) do { \
    if (strcmp((a), (b)) != 0) { \
        fprintf(stderr, "❌ FAIL: Expected '%s', got '%s'\n", (b), (a)); \
        failed_count++; \
        return; \
    } \
} while(0)

#define TEST_END() do { \
    printf("✅ PASSED\n"); \
    passed_count++; \
} while(0)

// Function declarations from runtime.c
char* read_file(const char* path);
int write_file(const char* path, const char* content);
char** read_lines(const char* path, int* line_count);
int append_file(const char* path, const char* content);
int file_exists(const char* path);
long file_size(const char* path);

// Test counters
static int test_count = 0;
static int passed_count = 0;
static int failed_count = 0;

// Test 1: Write and read simple file
void test_write_and_read() {
    TEST_START("Write and read file");
    
    const char* test_content = "Hello, MLP!\n";
    int result = write_file("/tmp/test1.txt", test_content);
    ASSERT_EQ(result, 0);
    
    char* content = read_file("/tmp/test1.txt");
    ASSERT_NOT_NULL(content);
    ASSERT_STR_EQ(content, test_content);
    
    free(content);
    TEST_END();
}

// Test 2: Large file handling (10KB)
void test_large_file() {
    TEST_START("Large file (10KB)");
    
    char large_buffer[10000];
    memset(large_buffer, 'A', sizeof(large_buffer) - 1);
    large_buffer[9999] = '\0';
    
    int result = write_file("/tmp/test2.txt", large_buffer);
    ASSERT_EQ(result, 0);
    
    char* large_content = read_file("/tmp/test2.txt");
    ASSERT_NOT_NULL(large_content);
    ASSERT_EQ(strlen(large_content), 9999);
    
    free(large_content);
    TEST_END();
}

// Test 3: Read lines
void test_read_lines() {
    TEST_START("Read lines");
    
    const char* multiline = "Line 1\nLine 2\nLine 3\n";
    write_file("/tmp/test3.txt", multiline);
    
    int line_count = 0;
    char** lines = read_lines("/tmp/test3.txt", &line_count);
    
    ASSERT_EQ(line_count, 3);
    ASSERT_STR_EQ(lines[0], "Line 1");
    ASSERT_STR_EQ(lines[1], "Line 2");
    ASSERT_STR_EQ(lines[2], "Line 3");
    
    // Free lines
    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }
    free(lines);
    
    TEST_END();
}

// Test 4: Append to file
void test_append_file() {
    TEST_START("Append to file");
    
    write_file("/tmp/test4.txt", "First\n");
    append_file("/tmp/test4.txt", "Second\n");
    
    char* appended = read_file("/tmp/test4.txt");
    ASSERT_NOT_NULL(appended);
    ASSERT_TRUE(strstr(appended, "First") != NULL);
    ASSERT_TRUE(strstr(appended, "Second") != NULL);
    
    free(appended);
    TEST_END();
}

// Test 5: File exists check
void test_file_exists() {
    TEST_START("File exists");
    
    write_file("/tmp/test5.txt", "Test");
    
    ASSERT_EQ(file_exists("/tmp/test5.txt"), 1);
    ASSERT_EQ(file_exists("/tmp/nonexistent_file_12345.txt"), 0);
    
    TEST_END();
}

// Test 6: File size
void test_file_size_func() {
    TEST_START("File size");
    
    const char* test_content = "Hello, MLP!";
    write_file("/tmp/test6.txt", test_content);
    
    long size = file_size("/tmp/test6.txt");
    ASSERT_EQ(size, strlen(test_content));
    
    TEST_END();
}

// Test 7: Error handling (nonexistent file)
void test_error_handling() {
    TEST_START("Error handling");
    
    char* missing = read_file("/tmp/missing_file_xyz.txt");
    ASSERT_TRUE(missing == NULL);
    
    TEST_END();
}

// Test 8: Empty file
void test_empty_file() {
    TEST_START("Empty file");
    
    write_file("/tmp/empty.txt", "");
    
    char* empty = read_file("/tmp/empty.txt");
    ASSERT_NOT_NULL(empty);
    ASSERT_EQ(strlen(empty), 0);
    
    free(empty);
    TEST_END();
}

// Main test runner
int main() {
    printf("╔══════════════════════════════════════╗\n");
    printf("║   File I/O Runtime Tests - Phase 3  ║\n");
    printf("╚══════════════════════════════════════╝\n");
    
    test_write_and_read();
    test_large_file();
    test_read_lines();
    test_append_file();
    test_file_exists();
    test_file_size_func();
    test_error_handling();
    test_empty_file();
    
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║          Test Results                ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║ Passed:  %2d                         ║\n", passed_count);
    printf("║ Failed:  %2d                         ║\n", failed_count);
    printf("║ Total:   %2d                         ║\n", passed_count + failed_count);
    printf("╚══════════════════════════════════════╝\n");
    
    return failed_count == 0 ? 0 : 1;
}
