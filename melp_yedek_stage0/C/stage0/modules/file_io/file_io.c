#include "file_io.h"
#include <stdlib.h>
#include <string.h>

// Dosya işlemi oluştur
FileOp* file_create_op(const char* filename, FileMode mode, int is_binary) {
    FileOp* op = malloc(sizeof(FileOp));
    op->filename = strdup(filename);
    op->mode = mode;
    op->handle = NULL; // Runtime'da açılacak
    op->is_binary = is_binary;
    return op;
}

// Dosya okuma oluştur
FileRead* file_create_read(const char* file_var, size_t size, size_t count) {
    FileRead* read = malloc(sizeof(FileRead));
    read->file_var = strdup(file_var);
    read->buffer = NULL; // Runtime'da ayrılacak
    read->size = size;
    read->count = count;
    return read;
}

// Dosya yazma oluştur
FileWrite* file_create_write(const char* file_var, size_t size, size_t count) {
    FileWrite* write = malloc(sizeof(FileWrite));
    write->file_var = strdup(file_var);
    write->data = NULL; // Runtime'da doldurulacak
    write->size = size;
    write->count = count;
    return write;
}

// Dosya kapatma oluştur
FileClose* file_create_close(const char* file_var) {
    FileClose* close = malloc(sizeof(FileClose));
    close->file_var = strdup(file_var);
    return close;
}

// Dosya konumlandırma oluştur
FileSeek* file_create_seek(const char* file_var, long offset, SeekOrigin origin) {
    FileSeek* seek = malloc(sizeof(FileSeek));
    seek->file_var = strdup(file_var);
    seek->offset = offset;
    seek->origin = origin;
    return seek;
}

// Dosya kontrolü oluştur
FileCheck* file_create_check(const char* filename) {
    FileCheck* check = malloc(sizeof(FileCheck));
    check->filename = strdup(filename);
    check->exists = 0; // Runtime'da kontrol edilecek
    check->size = 0;
    return check;
}

// Bellek temizleme
void file_op_free(FileOp* op) {
    if (!op) return;
    free(op->filename);
    if (op->handle) fclose(op->handle);
    free(op);
}

void file_read_free(FileRead* read) {
    if (!read) return;
    free(read->file_var);
    free(read);
}

void file_write_free(FileWrite* write) {
    if (!write) return;
    free(write->file_var);
    free(write);
}

void file_close_free(FileClose* close) {
    if (!close) return;
    free(close->file_var);
    free(close);
}

void file_seek_free(FileSeek* seek) {
    if (!seek) return;
    free(seek->file_var);
    free(seek);
}

void file_check_free(FileCheck* check) {
    if (!check) return;
    free(check->filename);
    free(check);
}
