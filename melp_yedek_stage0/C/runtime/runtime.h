#ifndef RUNTIME_H
#define RUNTIME_H

// Memory management
void* mlp_malloc(long size);
void mlp_free(void* ptr);

// String operations
long mlp_strlen(const char* str);
char* mlp_strdup(const char* str);
int mlp_strcmp(const char* s1, const char* s2);
void mlp_strcpy(char* dest, const char* src);
char* mlp_strcat(char* dest, const char* src);

// File I/O
char* mlp_read_file(const char* filename);
void mlp_write_file(const char* filename, const char* content);

// Print functions
void mlp_print_num(long num);
void mlp_print_str(const char* str);

#endif
