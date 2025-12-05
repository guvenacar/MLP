#ifndef FILE_IO_CODEGEN_H
#define FILE_IO_CODEGEN_H

#include "file_io.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_file_open(FILE* out, FileOp* op);
void codegen_file_read(FILE* out, FileRead* read);
void codegen_file_write(FILE* out, FileWrite* write);
void codegen_file_close(FILE* out, FileClose* close);
void codegen_file_seek(FILE* out, FileSeek* seek);
void codegen_file_check(FILE* out, FileCheck* check);

// Yardımcı fonksiyonlar
const char* get_fopen_mode_string(FileMode mode, int is_binary);
void codegen_error_check(FILE* out, const char* operation);

#endif // FILE_IO_CODEGEN_H
