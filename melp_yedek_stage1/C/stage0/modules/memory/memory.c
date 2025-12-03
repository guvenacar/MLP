#include "memory.h"
#include <stdlib.h>
#include <string.h>

// Bellek ayırma oluştur
MemoryAlloc* memory_create_alloc(size_t size, AllocStrategy strategy, const char* var_name) {
    MemoryAlloc* alloc = malloc(sizeof(MemoryAlloc));
    alloc->size = size;
    alloc->strategy = strategy;
    alloc->var_name = strdup(var_name);
    alloc->is_array = 0;
    alloc->element_count = 0;
    alloc->address = NULL; // Runtime'da doldurulacak
    return alloc;
}

// Bellek serbest bırakma oluştur
MemoryFree* memory_create_free(const char* var_name) {
    MemoryFree* free_op = malloc(sizeof(MemoryFree));
    free_op->var_name = strdup(var_name);
    free_op->address = NULL; // Runtime'da doldurulacak
    return free_op;
}

// Bellek kopyalama oluştur
MemoryCopy* memory_create_copy(const char* dest, const char* src, size_t size) {
    MemoryCopy* copy = malloc(sizeof(MemoryCopy));
    copy->dest_var = strdup(dest);
    copy->src_var = strdup(src);
    copy->size = size;
    return copy;
}

// Bellek sıfırlama oluştur
MemoryZero* memory_create_zero(const char* var_name, size_t size) {
    MemoryZero* zero = malloc(sizeof(MemoryZero));
    zero->var_name = strdup(var_name);
    zero->size = size;
    return zero;
}

// Bellek yeniden boyutlandırma oluştur
MemoryRealloc* memory_create_realloc(const char* var_name, size_t old_size, size_t new_size) {
    MemoryRealloc* realloc_op = malloc(sizeof(MemoryRealloc));
    realloc_op->var_name = strdup(var_name);
    realloc_op->old_size = old_size;
    realloc_op->new_size = new_size;
    return realloc_op;
}

// Bellek temizleme
void memory_alloc_free(MemoryAlloc* alloc) {
    if (!alloc) return;
    free(alloc->var_name);
    free(alloc);
}

void memory_free_free(MemoryFree* free_op) {
    if (!free_op) return;
    free(free_op->var_name);
    free(free_op);
}

void memory_copy_free(MemoryCopy* copy) {
    if (!copy) return;
    free(copy->dest_var);
    free(copy->src_var);
    free(copy);
}

void memory_zero_free(MemoryZero* zero) {
    if (!zero) return;
    free(zero->var_name);
    free(zero);
}

void memory_realloc_free(MemoryRealloc* realloc_op) {
    if (!realloc_op) return;
    free(realloc_op->var_name);
    free(realloc_op);
}
