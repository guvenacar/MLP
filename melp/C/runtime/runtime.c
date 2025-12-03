#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Memory management
void* mlp_malloc(long size) {
    return malloc((size_t)size);
}

void mlp_free(void* ptr) {
    free(ptr);
}

// String operations
long mlp_strlen(const char* str) {
    return (long)strlen(str);
}

char* mlp_strdup(const char* str) {
    return strdup(str);
}

int mlp_strcmp(const char* s1, const char* s2) {
    return strcmp(s1, s2);
}

void mlp_strcpy(char* dest, const char* src) {
    strcpy(dest, src);
}

char* mlp_strcat(char* dest, const char* src) {
    return strcat(dest, src);
}

// File I/O
char* mlp_read_file(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char* content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    return content;
}

void mlp_write_file(const char* filename, const char* content) {
    FILE* f = fopen(filename, "w");
    if (!f) return;
    fprintf(f, "%s", content);
    fclose(f);
}

// Print functions
void mlp_print_num(long num) {
    printf("%ld\n", num);
}

void mlp_print_str(const char* str) {
    printf("%s\n", str);
}

void mlp_print(const char* str) {
    printf("%s", str);
}
