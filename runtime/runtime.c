/*
 * ===============================================
 * MLP Runtime Library (C)
 * ===============================================
 * Provides core runtime support for MLP programs
 * - Memory management
 * - String operations
 * - I/O operations
 * - Array/Dictionary operations
 * - Type conversions
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
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

void* mlp_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "MLP Runtime Error: Memory allocation failed\n");
        exit(1);
    }
    return ptr;
}

void* mlp_realloc(void* ptr, size_t size) {
    void* new_ptr = realloc(ptr, size);
    if (!new_ptr && size > 0) {
        fprintf(stderr, "MLP Runtime Error: Memory reallocation failed\n");
        exit(1);
    }
    return new_ptr;
}

void mlp_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

// ===============================================
// String Operations
// ===============================================

mlp_string_t* mlp_string_new(const char* str) {
    mlp_string_t* s = mlp_malloc(sizeof(mlp_string_t));
    s->length = strlen(str);
    s->capacity = s->length + 1;
    s->data = mlp_malloc(s->capacity);
    strcpy(s->data, str);
    return s;
}

mlp_string_t* mlp_string_concat(mlp_string_t* s1, mlp_string_t* s2) {
    mlp_string_t* result = mlp_malloc(sizeof(mlp_string_t));
    result->length = s1->length + s2->length;
    result->capacity = result->length + 1;
    result->data = mlp_malloc(result->capacity);
    strcpy(result->data, s1->data);
    strcat(result->data, s2->data);
    return result;
}

void mlp_string_free(mlp_string_t* s) {
    if (s) {
        mlp_free(s->data);
        mlp_free(s);
    }
}

char* mlp_string_to_cstr(mlp_string_t* s) {
    return s->data;
}

// ===============================================
// Array Operations
// ===============================================

mlp_array_t* mlp_array_new(size_t initial_capacity) {
    mlp_array_t* arr = mlp_malloc(sizeof(mlp_array_t));
    arr->length = 0;
    arr->capacity = initial_capacity > 0 ? initial_capacity : 8;
    arr->items = mlp_malloc(sizeof(void*) * arr->capacity);
    return arr;
}

void mlp_array_push(mlp_array_t* arr, void* item) {
    if (arr->length >= arr->capacity) {
        arr->capacity *= 2;
        arr->items = mlp_realloc(arr->items, sizeof(void*) * arr->capacity);
    }
    arr->items[arr->length++] = item;
}

void* mlp_array_get(mlp_array_t* arr, size_t index) {
    if (index >= arr->length) {
        fprintf(stderr, "MLP Runtime Error: Array index out of bounds\n");
        exit(1);
    }
    return arr->items[index];
}

void mlp_array_set(mlp_array_t* arr, size_t index, void* item) {
    if (index >= arr->length) {
        fprintf(stderr, "MLP Runtime Error: Array index out of bounds\n");
        exit(1);
    }
    arr->items[index] = item;
}

size_t mlp_array_length(mlp_array_t* arr) {
    return arr->length;
}

void mlp_array_free(mlp_array_t* arr) {
    if (arr) {
        mlp_free(arr->items);
        mlp_free(arr);
    }
}

// ===============================================
// Dictionary Operations
// ===============================================

#define MLP_DICT_INITIAL_CAPACITY 16

static unsigned long mlp_hash(const char* str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

mlp_dict_t* mlp_dict_new(void) {
    mlp_dict_t* dict = mlp_malloc(sizeof(mlp_dict_t));
    dict->capacity = MLP_DICT_INITIAL_CAPACITY;
    dict->size = 0;
    dict->buckets = mlp_malloc(sizeof(mlp_dict_entry_t*) * dict->capacity);
    for (size_t i = 0; i < dict->capacity; i++) {
        dict->buckets[i] = NULL;
    }
    return dict;
}

void mlp_dict_set(mlp_dict_t* dict, const char* key, void* value) {
    unsigned long hash_value = mlp_hash(key);
    size_t index = hash_value % dict->capacity;

    mlp_dict_entry_t* entry = dict->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        entry = entry->next;
    }

    // Create new entry
    mlp_dict_entry_t* new_entry = mlp_malloc(sizeof(mlp_dict_entry_t));
    new_entry->key = strdup(key);
    new_entry->value = value;
    new_entry->next = dict->buckets[index];
    dict->buckets[index] = new_entry;
    dict->size++;
}

void* mlp_dict_get(mlp_dict_t* dict, const char* key) {
    unsigned long hash_value = mlp_hash(key);
    size_t index = hash_value % dict->capacity;

    mlp_dict_entry_t* entry = dict->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

bool mlp_dict_has(mlp_dict_t* dict, const char* key) {
    return mlp_dict_get(dict, key) != NULL;
}

void mlp_dict_free(mlp_dict_t* dict) {
    if (dict) {
        for (size_t i = 0; i < dict->capacity; i++) {
            mlp_dict_entry_t* entry = dict->buckets[i];
            while (entry) {
                mlp_dict_entry_t* next = entry->next;
                free(entry->key);
                mlp_free(entry);
                entry = next;
            }
        }
        mlp_free(dict->buckets);
        mlp_free(dict);
    }
}

// ===============================================
// I/O Operations
// ===============================================

void mlp_print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
}

void mlp_yazdir(const char* str) {
    printf("%s\n", str);
}

char* mlp_file_read(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "MLP Runtime Error: Cannot open file '%s'\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = mlp_malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);
    return content;
}

bool mlp_file_write(const char* path, const char* content) {
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "MLP Runtime Error: Cannot write to file '%s'\n", path);
        return false;
    }

    fputs(content, file);
    fclose(file);
    return true;
}

// ===============================================
// Type Conversions
// ===============================================

char* mlp_int_to_string(long value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld", value);
    return strdup(buffer);
}

char* mlp_float_to_string(double value) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%g", value);
    return strdup(buffer);
}

long mlp_string_to_int(const char* str) {
    return atol(str);
}

double mlp_string_to_float(const char* str) {
    return atof(str);
}

// ===============================================
// Utility Functions
// ===============================================

size_t mlp_string_length(const char* str) {
    return strlen(str);
}

char mlp_char_at(const char* str, size_t index) {
    if (index >= strlen(str)) {
        fprintf(stderr, "MLP Runtime Error: String index out of bounds\n");
        exit(1);
    }
    return str[index];
}

char* mlp_string_replace(const char* str, const char* old, const char* new) {
    char* result;
    int i, cnt = 0;
    int newlen = strlen(new);
    int oldlen = strlen(old);

    // Count occurrences
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], old) == &str[i]) {
            cnt++;
            i += oldlen - 1;
        }
    }

    result = mlp_malloc(i + cnt * (newlen - oldlen) + 1);

    i = 0;
    while (*str) {
        if (strstr(str, old) == str) {
            strcpy(&result[i], new);
            i += newlen;
            str += oldlen;
        } else {
            result[i++] = *str++;
        }
    }

    result[i] = '\0';
    return result;
}

bool mlp_string_starts_with(const char* str, const char* prefix) {
    if (!str || !prefix) {
        return false;
    }
    size_t prefix_len = strlen(prefix);
    size_t str_len = strlen(str);
    if (prefix_len > str_len) {
        return false;
    }
    return strncmp(str, prefix, prefix_len) == 0;
}

char* mlp_string_substring(const char* str, int start, int end) {
    if (!str) {
        return NULL;
    }
    int len = strlen(str);

    // Handle negative indices (Python-style)
    if (start < 0) start = len + start;
    if (end < 0) end = len + end;

    // Clamp to valid range
    if (start < 0) start = 0;
    if (end > len) end = len;
    if (start >= end) {
        char* empty = mlp_malloc(1);
        empty[0] = '\0';
        return empty;
    }

    int substr_len = end - start;
    char* result = mlp_malloc(substr_len + 1);
    strncpy(result, str + start, substr_len);
    result[substr_len] = '\0';
    return result;
}

// ===============================================
// Error Handling
// ===============================================

void mlp_error(const char* message) {
    fprintf(stderr, "MLP Runtime Error: %s\n", message);
    exit(1);
}

void mlp_assert(bool condition, const char* message) {
    if (!condition) {
        mlp_error(message);
    }
}

// ===============================================
// Main Entry Point Helper
// ===============================================

int mlp_run_main(int (*user_main)(int, char**), int argc, char** argv) {
    return user_main(argc, argv);
}
