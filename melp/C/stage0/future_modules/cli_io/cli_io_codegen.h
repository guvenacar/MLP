#ifndef CLI_IO_CODEGEN_H
#define CLI_IO_CODEGEN_H

#include "cli_io.h"
#include <stdio.h>

// Global helper generation
void cli_io_generate_helpers(FILE* out);

// Code generation fonksiyonları
void codegen_print(FILE* out, PrintOp* print);
void codegen_println(FILE* out, PrintOp* print);
void codegen_printf(FILE* out, PrintOp* print);
void codegen_input(FILE* out, InputOp* input);
void codegen_arg_access(FILE* out, ArgAccess* access);
void codegen_color_output(FILE* out, ColorOutput* color);

// Yardımcı fonksiyonlar
void codegen_puts_call(FILE* out, const char* str);
void codegen_printf_call(FILE* out, const char* format);
const char* get_ansi_color_code(AnsiColor color, int bold);

#endif // CLI_IO_CODEGEN_H
