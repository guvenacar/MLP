#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "runtime.h"

// ============================================================================
// PHASE 1: Runtime - Memory + Print
// ============================================================================

void* mlp_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    return ptr;
}

void mlp_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

// Print number to stdout using sys_write (pure assembly compatible)
void print_number(long num) {
    char buffer[32];
    int len = 0;
    long n = num;
    
    // Handle zero
    if (n == 0) {
        buffer[len++] = '0';
    } else {
        // Handle negative
        int is_negative = 0;
        if (n < 0) {
            is_negative = 1;
            n = -n;
        }
        
        // Convert to string (reverse order)
        char temp[32];
        int temp_len = 0;
        while (n > 0) {
            temp[temp_len++] = '0' + (n % 10);
            n /= 10;
        }
        
        // Add negative sign
        if (is_negative) {
            buffer[len++] = '-';
        }
        
        // Reverse digits
        for (int i = temp_len - 1; i >= 0; i--) {
            buffer[len++] = temp[i];
        }
    }
    
    // Add newline
    buffer[len++] = '\n';
    
    // Write to stdout using syscall
    write(1, buffer, len);
}
