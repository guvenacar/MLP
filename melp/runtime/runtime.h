#ifndef RUNTIME_H
#define RUNTIME_H

#include <stddef.h>
#include <setjmp.h>

// Exception handling structures
typedef struct {
    char* message;
    char* type;
    long code;
} MlpException;

typedef struct ExceptionHandler {
    jmp_buf jump_buffer;
    MlpException exception;
    struct ExceptionHandler* prev;
} ExceptionHandler;

// Global exception handler stack
extern ExceptionHandler* mlp_exception_stack;

// Phase 1: Memory + Print
void* mlp_malloc(size_t size);
void mlp_free(void* ptr);
void print_number(long num);

// Phase 5: String operations
void print_string(const char* str);
long string_length(const char* str);
char* string_concat(const char* str1, const char* str2);
long string_equal(const char* str1, const char* str2);
long string_not_equal(const char* str1, const char* str2);

// Phase 7: String interpolation
char* int_to_string(long num);

// Phase 8B: Dynamic arrays
void* mlp_array_alloc(long size);
void mlp_array_free(void* data);
long mlp_array_length(void* data);
void* mlp_array_resize(void* data, long new_size);

// Phase 14: Range function for iterators
void* mlp_range(long start, long end, long step);
void* mlp_range1(long end);
void* mlp_range2(long start, long end);

// Phase 9: File I/O
char* mlp_file_read(const char* path);
long mlp_file_write(const char* path, const char* content);
long mlp_file_exists(const char* path);
long mlp_file_append(const char* path, const char* content);

// Phase 11: String utilities
char* mlp_substring(const char* str, long start, long length);
long mlp_indexOf(const char* str, const char* substr);
char* mlp_charAt(const char* str, long index);
long mlp_string_length(const char* str);

// Phase 12: CLI arguments
char** mlp_get_argv(long argc, char** argv_c);
long mlp_get_argc(void);

// Exception handling
void mlp_exception_init(void);
ExceptionHandler* mlp_exception_push(void);
void mlp_exception_pop(void);
void mlp_throw(const char* type, const char* message, long code);
MlpException* mlp_get_current_exception(void);
char* mlp_exception_message(void);
char* mlp_exception_type(void);
long mlp_exception_code(void);

// Phase 15: Null safety
long mlp_is_null(void* ptr);
long mlp_coalesce(long value, long default_val);
long mlp_safe_deref(void* ptr);

// Phase 19: Garbage Collection
void gc_init(void);
void* gc_alloc(size_t size);
void gc_free(void* ptr);
void gc_retain(void* ptr);
void gc_release(void* ptr);
void gc_collect(void);
void gc_full_collect(void);
void gc_shutdown(void);
void gc_add_root(void** root);
void gc_remove_root(void** root);
long gc_get_object_count(void);
long gc_get_total_bytes(void);
long gc_get_collections(void);
void gc_enable(void);
void gc_disable(void);

#endif // RUNTIME_H
