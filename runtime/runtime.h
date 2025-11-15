/*
 * ===============================================
 * MLP Runtime Library Header
 * ===============================================
 */

#ifndef MLP_RUNTIME_H
#define MLP_RUNTIME_H

#include <stddef.h>
#include <stdbool.h>

// ===============================================
// Type Definitions
// ===============================================

typedef struct {
    char* data;
    size_t length;
    size_t capacity;
} mlp_string_t;

typedef struct {
    void** items;
    size_t length;
    size_t capacity;
} mlp_array_t;

typedef struct mlp_dict_entry {
    char* key;
    void* value;
    struct mlp_dict_entry* next;
} mlp_dict_entry_t;

typedef struct {
    mlp_dict_entry_t** buckets;
    size_t size;
    size_t capacity;
} mlp_dict_t;

// ===============================================
// Memory Management
// ===============================================

void* mlp_malloc(size_t size);
void* mlp_realloc(void* ptr, size_t size);
void mlp_free(void* ptr);

// ===============================================
// String Operations
// ===============================================

mlp_string_t* mlp_string_new(const char* str);
mlp_string_t* mlp_string_concat(mlp_string_t* s1, mlp_string_t* s2);
void mlp_string_free(mlp_string_t* s);
char* mlp_string_to_cstr(mlp_string_t* s);

// ===============================================
// Array Operations
// ===============================================

mlp_array_t* mlp_array_new(size_t initial_capacity);
void mlp_array_push(mlp_array_t* arr, void* item);
void* mlp_array_get(mlp_array_t* arr, size_t index);
void mlp_array_set(mlp_array_t* arr, size_t index, void* item);
size_t mlp_array_length(mlp_array_t* arr);
void mlp_array_free(mlp_array_t* arr);

// ===============================================
// Dictionary Operations
// ===============================================

mlp_dict_t* mlp_dict_new(void);
void mlp_dict_set(mlp_dict_t* dict, const char* key, void* value);
void* mlp_dict_get(mlp_dict_t* dict, const char* key);
bool mlp_dict_has(mlp_dict_t* dict, const char* key);
void mlp_dict_free(mlp_dict_t* dict);

// ===============================================
// I/O Operations
// ===============================================

void mlp_print(const char* format, ...);
void mlp_yazdir(const char* str);
char* mlp_file_read(const char* path);
bool mlp_file_write(const char* path, const char* content);

// ===============================================
// Type Conversions
// ===============================================

char* mlp_int_to_string(long value);
char* mlp_float_to_string(double value);
long mlp_string_to_int(const char* str);
double mlp_string_to_float(const char* str);

// ===============================================
// Utility Functions
// ===============================================

size_t mlp_string_length(const char* str);
char mlp_char_at(const char* str, size_t index);
char* mlp_string_replace(const char* str, const char* old, const char* new);

// ===============================================
// Error Handling
// ===============================================

void mlp_error(const char* message);
void mlp_assert(bool condition, const char* message);

// ===============================================
// Main Entry Point Helper
// ===============================================

int mlp_run_main(int (*user_main)(int, char**), int argc, char** argv);

#endif // MLP_RUNTIME_H
