#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// Print string to stdout
void print_string(const char* str) {
    if (str) {
        write(1, str, strlen(str));
        write(1, "\n", 1);
    }
}

// String length
long string_length(const char* str) {
    if (!str) return 0;
    return strlen(str);
}

// String concatenation
char* string_concat(const char* str1, const char* str2) {
    if (!str1 && !str2) return NULL;
    if (!str1) {
        char* result = mlp_malloc(strlen(str2) + 1);
        strcpy(result, str2);
        return result;
    }
    if (!str2) {
        char* result = mlp_malloc(strlen(str1) + 1);
        strcpy(result, str1);
        return result;
    }
    
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char* result = mlp_malloc(len1 + len2 + 1);
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

// String comparison (returns 1 if equal, 0 if not equal)
long string_equal(const char* str1, const char* str2) {
    if (str1 == str2) return 1;  // Same pointer or both NULL
    if (!str1 || !str2) return 0;  // One is NULL, other is not
    return strcmp(str1, str2) == 0 ? 1 : 0;
}

// String not equal (returns 1 if not equal, 0 if equal)
long string_not_equal(const char* str1, const char* str2) {
    return string_equal(str1, str2) ? 0 : 1;
}

// Convert integer to string
char* int_to_string(long num) {
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
    
    buffer[len] = '\0';
    
    // Allocate and copy
    char* result = mlp_malloc(len + 1);
    strcpy(result, buffer);
    return result;
}

// ============================================================================
// PHASE 8B: Dynamic Arrays
// ============================================================================

// Array metadata structure (stored before array data)
typedef struct {
    long size;      // Number of elements
    long capacity;  // Allocated capacity
} ArrayHeader;

// Allocate dynamic array with metadata
// Returns pointer to data (not header)
void* mlp_array_alloc(long size) {
    if (size < 0) {
        fprintf(stderr, "Array size cannot be negative: %ld\n", size);
        exit(1);
    }
    
    // Allocate: header + (size * 8 bytes per element)
    size_t total_size = sizeof(ArrayHeader) + (size * 8);
    ArrayHeader* header = (ArrayHeader*)mlp_malloc(total_size);
    
    header->size = size;
    header->capacity = size;
    
    // Return pointer to data (skip header)
    void* data = (void*)(header + 1);
    
    // Zero-initialize array elements
    memset(data, 0, size * 8);
    
    return data;
}

// Free dynamic array (given data pointer)
void mlp_array_free(void* data) {
    if (!data) return;
    
    // Get header (stored before data)
    ArrayHeader* header = ((ArrayHeader*)data) - 1;
    free(header);
}

// Get array length
long mlp_array_length(void* data) {
    if (!data) return 0;
    
    // Get header (stored before data)
    ArrayHeader* header = ((ArrayHeader*)data) - 1;
    return header->size;
}

// Resize array (reallocate with new size)
void* mlp_array_resize(void* data, long new_size) {
    if (new_size < 0) {
        fprintf(stderr, "Array size cannot be negative: %ld\n", new_size);
        exit(1);
    }
    
    if (!data) {
        // If null, just allocate new array
        return mlp_array_alloc(new_size);
    }
    
    ArrayHeader* old_header = ((ArrayHeader*)data) - 1;
    long old_size = old_header->size;
    
    // Allocate new array
    size_t total_size = sizeof(ArrayHeader) + (new_size * 8);
    ArrayHeader* new_header = (ArrayHeader*)mlp_malloc(total_size);
    
    new_header->size = new_size;
    new_header->capacity = new_size;
    
    void* new_data = (void*)(new_header + 1);
    
    // Copy old data (up to min of old and new size)
    long copy_size = (old_size < new_size) ? old_size : new_size;
    memcpy(new_data, data, copy_size * 8);
    
    // Zero-initialize remaining elements if growing
    if (new_size > old_size) {
        long* elements = (long*)new_data;
        for (long i = old_size; i < new_size; i++) {
            elements[i] = 0;
        }
    }
    
    // Free old array
    free(old_header);
    
    return new_data;
}

// ============================================================================
// PHASE 9: File I/O
// ============================================================================

// Read entire file contents into a string
// Returns NULL on error
char* mlp_file_read(const char* path) {
    if (!path) {
        fprintf(stderr, "File path cannot be NULL\n");
        return NULL;
    }
    
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", path);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size < 0) {
        fprintf(stderr, "Failed to get file size: %s\n", path);
        fclose(file);
        return NULL;
    }
    
    // Allocate buffer (+1 for null terminator)
    char* buffer = (char*)mlp_malloc(size + 1);
    
    // Read file contents
    size_t read_size = fread(buffer, 1, size, file);
    fclose(file);
    
    if (read_size != (size_t)size) {
        fprintf(stderr, "Failed to read file: %s\n", path);
        free(buffer);
        return NULL;
    }
    
    // Null-terminate
    buffer[size] = '\0';
    
    return buffer;
}

// Write string contents to file
// Returns 1 on success, 0 on error
long mlp_file_write(const char* path, const char* content) {
    if (!path) {
        fprintf(stderr, "File path cannot be NULL\n");
        return 0;
    }
    
    if (!content) {
        content = "";  // Write empty file
    }
    
    FILE* file = fopen(path, "wb");
    if (!file) {
        fprintf(stderr, "Failed to open file for writing: %s\n", path);
        return 0;
    }
    
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);
    fclose(file);
    
    if (written != len) {
        fprintf(stderr, "Failed to write file: %s\n", path);
        return 0;
    }
    
    return 1;  // Success
}

// Check if file exists and is readable
// Returns 1 if exists, 0 otherwise
long mlp_file_exists(const char* path) {
    if (!path) {
        return 0;
    }
    
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    
    return 0;
}

// Append string to file
// Returns 1 on success, 0 on error
long mlp_file_append(const char* path, const char* content) {
    if (!path) {
        fprintf(stderr, "File path cannot be NULL\n");
        return 0;
    }
    
    if (!content) {
        return 1;  // Nothing to append
    }
    
    FILE* file = fopen(path, "ab");
    if (!file) {
        fprintf(stderr, "Failed to open file for appending: %s\n", path);
        return 0;
    }
    
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);
    fclose(file);
    
    if (written != len) {
        fprintf(stderr, "Failed to append to file: %s\n", path);
        return 0;
    }
    
    return 1;  // Success
}

// ============================================================================
// Phase 11: String Utilities
// ============================================================================

// Get substring from string
// Parameters: str (source string), start (0-based index), length (chars to extract)
// Returns: New allocated string containing substring, or NULL on error
char* mlp_substring(const char* str, long start, long length) {
    if (!str) {
        fprintf(stderr, "substring: NULL string\n");
        return NULL;
    }
    
    long str_len = strlen(str);
    
    // Validate start index
    if (start < 0 || start >= str_len) {
        fprintf(stderr, "substring: start index %ld out of bounds (string length: %ld)\n", start, str_len);
        return NULL;
    }
    
    // Validate length
    if (length < 0) {
        fprintf(stderr, "substring: negative length %ld\n", length);
        return NULL;
    }
    
    // Adjust length if it goes beyond string end
    if (start + length > str_len) {
        length = str_len - start;
    }
    
    // Allocate new string
    char* result = mlp_malloc(length + 1);
    if (!result) {
        fprintf(stderr, "substring: memory allocation failed\n");
        return NULL;
    }
    
    // Copy substring
    strncpy(result, str + start, length);
    result[length] = '\0';
    
    return result;
}

// Find index of substring in string
// Parameters: str (haystack), substr (needle)
// Returns: Index of first occurrence (0-based), or -1 if not found
long mlp_indexOf(const char* str, const char* substr) {
    if (!str || !substr) {
        fprintf(stderr, "indexOf: NULL string\n");
        return -1;
    }
    
    // Empty substring is found at position 0
    if (substr[0] == '\0') {
        return 0;
    }
    
    const char* found = strstr(str, substr);
    if (found) {
        return (long)(found - str);
    }
    
    return -1;  // Not found
}

// Get character at index as a single-character string
// Parameters: str (source string), index (0-based)
// Returns: New allocated single-char string, or NULL on error
char* mlp_charAt(const char* str, long index) {
    if (!str) {
        fprintf(stderr, "charAt: NULL string\n");
        return NULL;
    }
    
    long str_len = strlen(str);
    
    if (index < 0 || index >= str_len) {
        fprintf(stderr, "charAt: index %ld out of bounds (string length: %ld)\n", index, str_len);
        return NULL;
    }
    
    // Allocate 2-byte string (char + null terminator)
    char* result = mlp_malloc(2);
    if (!result) {
        fprintf(stderr, "charAt: memory allocation failed\n");
        return NULL;
    }
    
    result[0] = str[index];
    result[1] = '\0';
    
    return result;
}

// Get string length (expose existing function to MELP)
// Parameters: str (source string)
// Returns: Length of string in characters (excluding null terminator)
long mlp_string_length(const char* str) {
    if (!str) {
        return 0;
    }
    return (long)strlen(str);
}

// ===== Phase 12: CLI Arguments =====

static long global_argc = 0;
static char** global_argv = NULL;

/**
 * Convert C-style argv to MLP string array
 * argc: argument count
 * argv_c: C-style char** argv
 * Returns: char** array for MLP (string[])
 */
char** mlp_get_argv(long argc, char** argv_c) {
    global_argc = argc;
    
    // Allocate array of string pointers
    char** argv_mlp = (char**)mlp_malloc((argc + 1) * sizeof(char*));
    
    // Copy each argument string
    for (long i = 0; i < argc; i++) {
        size_t len = strlen(argv_c[i]);
        argv_mlp[i] = (char*)mlp_malloc(len + 1);
        strcpy(argv_mlp[i], argv_c[i]);
    }
    argv_mlp[argc] = NULL; // Null-terminate
    
    global_argv = argv_mlp;
    return argv_mlp;
}

/**
 * Get argument count
 */
long mlp_get_argc(void) {
    return global_argc;
}

// ============================================================================
// Exception Handling Runtime
// ============================================================================

ExceptionHandler* mlp_exception_stack = NULL;

/**
 * Initialize exception handling system
 */
void mlp_exception_init(void) {
    mlp_exception_stack = NULL;
}

/**
 * Push new exception handler onto stack
 * Returns handler for setjmp
 */
ExceptionHandler* mlp_exception_push(void) {
    ExceptionHandler* handler = (ExceptionHandler*)mlp_malloc(sizeof(ExceptionHandler));
    handler->exception.message = NULL;
    handler->exception.type = NULL;
    handler->exception.code = 0;
    handler->prev = mlp_exception_stack;
    mlp_exception_stack = handler;
    return handler;
}

/**
 * Pop exception handler from stack
 */
void mlp_exception_pop(void) {
    if (mlp_exception_stack) {
        ExceptionHandler* handler = mlp_exception_stack;
        mlp_exception_stack = handler->prev;
        
        // Free exception data
        if (handler->exception.message) {
            mlp_free(handler->exception.message);
        }
        if (handler->exception.type) {
            mlp_free(handler->exception.type);
        }
        mlp_free(handler);
    }
}

/**
 * Throw exception - unwinds stack to nearest handler
 */
void mlp_throw(const char* type, const char* message, long code) {
    if (!mlp_exception_stack) {
        // No exception handler - print and exit
        fprintf(stderr, "Uncaught exception: %s: %s (code: %ld)\n", 
                type ? type : "Error", message ? message : "Unknown error", code);
        exit(1);
    }
    
    // Store exception in current handler
    ExceptionHandler* handler = mlp_exception_stack;
    
    if (type) {
        handler->exception.type = (char*)mlp_malloc(strlen(type) + 1);
        strcpy(handler->exception.type, type);
    }
    
    if (message) {
        handler->exception.message = (char*)mlp_malloc(strlen(message) + 1);
        strcpy(handler->exception.message, message);
    }
    
    handler->exception.code = code;
    
    // Jump to handler
    longjmp(handler->jump_buffer, 1);
}

/**
 * Get current exception object
 */
MlpException* mlp_get_current_exception(void) {
    if (mlp_exception_stack) {
        return &mlp_exception_stack->exception;
    }
    return NULL;
}

/**
 * Get exception message
 */
char* mlp_exception_message(void) {
    MlpException* ex = mlp_get_current_exception();
    return ex ? ex->message : NULL;
}

/**
 * Get exception type
 */
char* mlp_exception_type(void) {
    MlpException* ex = mlp_get_current_exception();
    return ex ? ex->type : NULL;
}

/**
 * Get exception code
 */
long mlp_exception_code(void) {
    MlpException* ex = mlp_get_current_exception();
    return ex ? ex->code : 0;
}

/**
 * Check if there's a handler available
 */
int mlp_exception_has_handler(void) {
    return mlp_exception_stack != NULL ? 1 : 0;
}

// Check if current handler has a parent handler
int mlp_exception_has_parent_handler(void) {
    if (mlp_exception_stack == NULL) return 0;
    return mlp_exception_stack->prev != NULL ? 1 : 0;
}
