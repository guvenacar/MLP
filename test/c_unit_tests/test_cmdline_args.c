/**
 * Phase 5.1 C Unit Tests - Command-Line Arguments
 * Tests: mlp_set_args, mlp_get_argc, mlp_get_argv
 * 
 * Expected: 10 tests
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Forward declarations from runtime.c
extern void mlp_set_args(int argc, char** argv);
extern long mlp_get_argc(void);
extern char* mlp_get_argv(long index);

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    printf("Running: %s... ", #name); \
    test_##name(); \
    printf("PASSED\n"); \
    tests_passed++; \
} while(0)

#define ASSERT_EQ(actual, expected) do { \
    if ((actual) != (expected)) { \
        fprintf(stderr, "\nAssertion failed: %s == %s\n", #actual, #expected); \
        fprintf(stderr, "  Expected: %ld\n", (long)(expected)); \
        fprintf(stderr, "  Actual: %ld\n", (long)(actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

#define ASSERT_STR_EQ(actual, expected) do { \
    if (strcmp((actual), (expected)) != 0) { \
        fprintf(stderr, "\nAssertion failed: %s == %s\n", #actual, #expected); \
        fprintf(stderr, "  Expected: \"%s\"\n", (expected)); \
        fprintf(stderr, "  Actual: \"%s\"\n", (actual)); \
        tests_failed++; \
        return; \
    } \
} while(0)

// ===========================
// mlp_set_args() Tests (2 tests)
// ===========================

TEST(set_args_basic) {
    char* argv[] = {"program", "arg1", "arg2"};
    mlp_set_args(3, argv);

    long argc = mlp_get_argc();
    ASSERT_EQ(argc, 3);

    char* arg0 = mlp_get_argv(0);
    ASSERT_STR_EQ(arg0, "program");
    free(arg0);
}

TEST(set_args_empty) {
    char* argv[] = {"program"};
    mlp_set_args(1, argv);

    long argc = mlp_get_argc();
    ASSERT_EQ(argc, 1);

    char* arg0 = mlp_get_argv(0);
    ASSERT_STR_EQ(arg0, "program");
    free(arg0);
}

// ===========================
// mlp_get_argc() Tests (3 tests)
// ===========================

TEST(get_argc_zero) {
    char* argv[] = {};
    mlp_set_args(0, argv);

    long argc = mlp_get_argc();
    ASSERT_EQ(argc, 0);
}

TEST(get_argc_one) {
    char* argv[] = {"program"};
    mlp_set_args(1, argv);

    long argc = mlp_get_argc();
    ASSERT_EQ(argc, 1);
}

TEST(get_argc_many) {
    char* argv[] = {"program", "arg1", "arg2", "arg3", "arg4", "arg5"};
    mlp_set_args(6, argv);

    long argc = mlp_get_argc();
    ASSERT_EQ(argc, 6);
}

// ===========================
// mlp_get_argv() Tests (5 tests)
// ===========================

TEST(get_argv_valid_index) {
    char* argv[] = {"program", "first", "second", "third"};
    mlp_set_args(4, argv);

    char* arg0 = mlp_get_argv(0);
    ASSERT_STR_EQ(arg0, "program");
    free(arg0);

    char* arg1 = mlp_get_argv(1);
    ASSERT_STR_EQ(arg1, "first");
    free(arg1);

    char* arg2 = mlp_get_argv(2);
    ASSERT_STR_EQ(arg2, "second");
    free(arg2);

    char* arg3 = mlp_get_argv(3);
    ASSERT_STR_EQ(arg3, "third");
    free(arg3);
}

TEST(get_argv_negative_index) {
    char* argv[] = {"program", "arg1"};
    mlp_set_args(2, argv);

    char* arg = mlp_get_argv(-1);
    ASSERT_STR_EQ(arg, "");  // Should return empty string
    free(arg);

    arg = mlp_get_argv(-100);
    ASSERT_STR_EQ(arg, "");
    free(arg);
}

TEST(get_argv_out_of_bounds) {
    char* argv[] = {"program", "arg1"};
    mlp_set_args(2, argv);

    char* arg = mlp_get_argv(2);  // Index 2 is out of bounds (argc=2)
    ASSERT_STR_EQ(arg, "");
    free(arg);

    arg = mlp_get_argv(100);
    ASSERT_STR_EQ(arg, "");
    free(arg);
}

TEST(get_argv_with_spaces) {
    char* argv[] = {"program", "arg with spaces", "another arg"};
    mlp_set_args(3, argv);

    char* arg1 = mlp_get_argv(1);
    ASSERT_STR_EQ(arg1, "arg with spaces");
    free(arg1);

    char* arg2 = mlp_get_argv(2);
    ASSERT_STR_EQ(arg2, "another arg");
    free(arg2);
}

TEST(get_argv_with_special_chars) {
    char* argv[] = {"program", "--flag", "-v", "file.txt", "key=value"};
    mlp_set_args(5, argv);

    char* arg1 = mlp_get_argv(1);
    ASSERT_STR_EQ(arg1, "--flag");
    free(arg1);

    char* arg2 = mlp_get_argv(2);
    ASSERT_STR_EQ(arg2, "-v");
    free(arg2);

    char* arg3 = mlp_get_argv(3);
    ASSERT_STR_EQ(arg3, "file.txt");
    free(arg3);

    char* arg4 = mlp_get_argv(4);
    ASSERT_STR_EQ(arg4, "key=value");
    free(arg4);
}

// ===========================
// Main
// ===========================

int main() {
    printf("===============================================\n");
    printf("Phase 5.1 C Unit Tests - Command-Line Arguments\n");
    printf("===============================================\n\n");

    // mlp_set_args tests (2)
    RUN_TEST(set_args_basic);
    RUN_TEST(set_args_empty);

    // mlp_get_argc tests (3)
    RUN_TEST(get_argc_zero);
    RUN_TEST(get_argc_one);
    RUN_TEST(get_argc_many);

    // mlp_get_argv tests (5)
    RUN_TEST(get_argv_valid_index);
    RUN_TEST(get_argv_negative_index);
    RUN_TEST(get_argv_out_of_bounds);
    RUN_TEST(get_argv_with_spaces);
    RUN_TEST(get_argv_with_special_chars);

    printf("\n===============================================\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("===============================================\n");

    return tests_failed > 0 ? 1 : 0;
}
