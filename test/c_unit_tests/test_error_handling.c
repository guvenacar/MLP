/**
 * Phase 5.2 C Unit Tests - Error Handling
 * Tests: exit_with_code, panic, mlp_assert, get_error_code, set_error_code
 * 
 * NOTE: This test file is written BEFORE implementation exists.
 *       It will be compiled and run once Claude #1 completes runtime implementation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Forward declarations from runtime.c (will be implemented by Claude #1)
extern void exit_with_code(long code);
extern void panic(const char* message);
extern void mlp_assert(long condition, const char* message);
extern long get_error_code(void);
extern void set_error_code(long code);

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    printf("Testing %s... ", name); \
    if (test_##name()) { \
        printf("PASS\n"); \
        tests_passed++; \
    } else { \
        printf("FAIL\n"); \
        tests_failed++; \
    }

// ==================== Error Code Tests ====================

int test_get_set_error_code() {
    // Test initial state
    set_error_code(0);
    long code = get_error_code();
    if (code != 0) return 0;

    // Test setting and getting
    set_error_code(42);
    code = get_error_code();
    if (code != 42) return 0;

    set_error_code(100);
    code = get_error_code();
    if (code != 100) return 0;

    // Test negative values
    set_error_code(-1);
    code = get_error_code();
    if (code != -1) return 0;

    // Reset
    set_error_code(0);
    return 1;
}

int test_error_code_persistence() {
    // Set multiple times
    set_error_code(10);
    set_error_code(20);
    set_error_code(30);
    
    long code = get_error_code();
    if (code != 30) return 0;  // Should have last value
    
    set_error_code(0);
    return 1;
}

int test_error_code_large_values() {
    // Test with large positive value
    set_error_code(2147483647);  // Max int32
    long code = get_error_code();
    if (code != 2147483647) return 0;
    
    // Test with large negative value
    set_error_code(-2147483648);
    code = get_error_code();
    if (code != -2147483648) return 0;
    
    set_error_code(0);
    return 1;
}

// ==================== Assert Tests ====================

int test_assert_pass() {
    // Should not crash
    mlp_assert(1, "This should pass");
    mlp_assert(42, "Non-zero should pass");
    mlp_assert(-1, "Negative should pass");
    mlp_assert(999, "Large positive should pass");
    return 1;
}

int test_assert_with_null_message() {
    // Should not crash with NULL message
    mlp_assert(1, NULL);
    mlp_assert(100, NULL);
    return 1;
}

int test_assert_with_long_message() {
    // Test with very long message
    char long_msg[1024];
    for (int i = 0; i < 1020; i++) {
        long_msg[i] = 'A' + (i % 26);
    }
    long_msg[1020] = '\0';
    
    mlp_assert(1, long_msg);
    return 1;
}

// ==================== Exit Code Tests (via fork) ====================

int test_exit_with_code_zero() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(0);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 0;
        }
    }
    return 0;
}

int test_exit_with_code_one() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

int test_exit_with_code_custom() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(42);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 42;
        }
    }
    return 0;
}

int test_exit_with_code_large() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(255);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            // Exit codes are typically 0-255
            return WEXITSTATUS(status) == 255;
        }
    }
    return 0;
}

int test_exit_with_code_negative() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        exit_with_code(-1);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            // Negative codes should be converted (typically to 255)
            int exit_code = WEXITSTATUS(status);
            return exit_code >= 0 && exit_code <= 255;
        }
    }
    return 0;
}

// ==================== Panic Tests (via fork) ====================

int test_panic_exits_with_one() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process - redirect stderr to avoid output
        freopen("/dev/null", "w", stderr);
        panic("Test panic");
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

int test_panic_with_null_message() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        freopen("/dev/null", "w", stderr);
        panic(NULL);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

int test_panic_with_long_message() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        freopen("/dev/null", "w", stderr);
        char long_msg[1024];
        memset(long_msg, 'X', 1023);
        long_msg[1023] = '\0';
        panic(long_msg);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

// ==================== Assert Failure Tests (via fork) ====================

int test_assert_failure() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        freopen("/dev/null", "w", stderr);
        mlp_assert(0, "This should fail");
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

int test_assert_failure_with_null() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        freopen("/dev/null", "w", stderr);
        mlp_assert(0, NULL);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

int test_assert_with_false_like_values() {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process - test that only 0 is false
        freopen("/dev/null", "w", stderr);
        
        // These should all pass (not exit)
        mlp_assert(-1, "Negative is truthy");
        mlp_assert(2, "2 is truthy");
        mlp_assert(100, "100 is truthy");
        
        // This should fail and exit
        mlp_assert(0, "Zero is falsy");
        
        // Should never reach here
        exit(99);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            // Should exit with 1 from the assert(0) call
            return WEXITSTATUS(status) == 1;
        }
    }
    return 0;
}

// ==================== Integration Tests ====================

int test_error_code_with_assert() {
    // Test that error code persists through assertions
    set_error_code(50);
    mlp_assert(1, "This passes");
    long code = get_error_code();
    if (code != 50) return 0;
    
    set_error_code(0);
    return 1;
}

int test_multiple_operations() {
    // Test a sequence of operations
    set_error_code(10);
    mlp_assert(1, "Pass 1");
    set_error_code(20);
    mlp_assert(1, "Pass 2");
    long code = get_error_code();
    if (code != 20) return 0;
    
    set_error_code(0);
    return 1;
}

// ==================== Main ====================

int main() {
    printf("==================================\n");
    printf("Phase 5.2 C Unit Tests - Error Handling\n");
    printf("==================================\n\n");

    printf("--- Error Code Tests ---\n");
    TEST(get_set_error_code);
    TEST(error_code_persistence);
    TEST(error_code_large_values);
    printf("\n");

    printf("--- Assert Pass Tests ---\n");
    TEST(assert_pass);
    TEST(assert_with_null_message);
    TEST(assert_with_long_message);
    printf("\n");

    printf("--- Exit Code Tests ---\n");
    TEST(exit_with_code_zero);
    TEST(exit_with_code_one);
    TEST(exit_with_code_custom);
    TEST(exit_with_code_large);
    TEST(exit_with_code_negative);
    printf("\n");

    printf("--- Panic Tests ---\n");
    TEST(panic_exits_with_one);
    TEST(panic_with_null_message);
    TEST(panic_with_long_message);
    printf("\n");

    printf("--- Assert Failure Tests ---\n");
    TEST(assert_failure);
    TEST(assert_failure_with_null);
    TEST(assert_with_false_like_values);
    printf("\n");

    printf("--- Integration Tests ---\n");
    TEST(error_code_with_assert);
    TEST(multiple_operations);
    printf("\n");

    printf("==================================\n");
    printf("Results: %d passed, %d failed\n", tests_passed, tests_failed);
    printf("==================================\n");

    return tests_failed > 0 ? 1 : 0;
}
