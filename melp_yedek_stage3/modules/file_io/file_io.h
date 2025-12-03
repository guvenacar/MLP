#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdio.h>
#include <stddef.h>
#include "../../lexer.h"

// Dosya açma modları
typedef enum FileMode {
    FILE_READ,          // "r" - Okuma
    FILE_WRITE,         // "w" - Yazma (üzerine yaz)
    FILE_APPEND,        // "a" - Ekleme (sona ekle)
    FILE_READ_WRITE,    // "r+" - Okuma/Yazma
    FILE_WRITE_READ,    // "w+" - Yazma/Okuma (sıfırdan)
    FILE_APPEND_READ    // "a+" - Ekleme/Okuma
} FileMode;

// Dosya işlemi
typedef struct FileOp {
    char* filename;         // Dosya adı
    FileMode mode;          // Açma modu
    FILE* handle;           // Dosya handle (runtime)
    int is_binary;          // Binary mod mu? (text vs binary)
} FileOp;

// Dosya okuma
typedef struct FileRead {
    char* file_var;         // Dosya değişkeni
    void* buffer;           // Okuma buffer'ı
    size_t size;            // Okunacak boyut
    size_t count;           // Okunacak eleman sayısı
} FileRead;

// Dosya yazma
typedef struct FileWrite {
    char* file_var;         // Dosya değişkeni
    void* data;             // Yazılacak veri
    size_t size;            // Yazılacak boyut
    size_t count;           // Yazılacak eleman sayısı
} FileWrite;

// Dosya kapatma
typedef struct FileClose {
    char* file_var;         // Kapatılacak dosya değişkeni
} FileClose;

// Dosya konumlandırma (seek)
typedef enum SeekOrigin {
    SEEK_SET_POS = 0,       // Baştan
    SEEK_CUR_POS = 1,       // Mevcut konumdan
    SEEK_END_POS = 2        // Sondan
} SeekOrigin;

typedef struct FileSeek {
    char* file_var;         // Dosya değişkeni
    long offset;            // Offset (byte)
    SeekOrigin origin;      // Başlangıç noktası
} FileSeek;

// Dosya kontrolü
typedef struct FileCheck {
    char* filename;         // Kontrol edilecek dosya
    int exists;             // Var mı? (runtime)
    long size;              // Boyut (runtime)
} FileCheck;

// API fonksiyonları
FileOp* file_create_op(const char* filename, FileMode mode, int is_binary);
FileRead* file_create_read(const char* file_var, size_t size, size_t count);
FileWrite* file_create_write(const char* file_var, size_t size, size_t count);
FileClose* file_create_close(const char* file_var);
FileSeek* file_create_seek(const char* file_var, long offset, SeekOrigin origin);
FileCheck* file_create_check(const char* filename);

void file_op_free(FileOp* op);
void file_read_free(FileRead* read);
void file_write_free(FileWrite* write);
void file_close_free(FileClose* close);
void file_seek_free(FileSeek* seek);
void file_check_free(FileCheck* check);

#endif // FILE_IO_H
