#include "cli_io.h"
#include <stdlib.h>
#include <string.h>

// Print operasyonu oluştur
PrintOp* cli_create_print(OutputType type, const char* format, int newline) {
    PrintOp* print = malloc(sizeof(PrintOp));
    print->type = type;
    print->value = NULL; // Runtime'da doldurulacak
    print->format = format ? strdup(format) : NULL;
    print->newline = newline;
    return print;
}

// Input operasyonu oluştur
InputOp* cli_create_input(const char* prompt, OutputType expected, size_t max_size) {
    InputOp* input = malloc(sizeof(InputOp));
    input->prompt = prompt ? strdup(prompt) : NULL;
    input->expected = expected;
    input->buffer = NULL; // Runtime'da ayrılacak
    input->max_size = max_size;
    return input;
}

// CLI argümanları oluştur
CliArgs* cli_create_args(int argc, char** argv) {
    CliArgs* args = malloc(sizeof(CliArgs));
    args->argc = argc;
    args->argv = argv; // main()'den geliyor
    return args;
}

// Argüman erişimi oluştur
ArgAccess* cli_create_arg_access(int index) {
    ArgAccess* access = malloc(sizeof(ArgAccess));
    access->index = index;
    access->value = NULL; // Runtime'da doldurulacak
    return access;
}

// Renkli çıktı oluştur
ColorOutput* cli_create_color_output(AnsiColor color, const char* text, int bold) {
    ColorOutput* output = malloc(sizeof(ColorOutput));
    output->color = color;
    output->text = strdup(text);
    output->bold = bold;
    return output;
}

// Bellek temizleme
void print_op_free(PrintOp* print) {
    if (!print) return;
    if (print->format) free(print->format);
    free(print);
}

void input_op_free(InputOp* input) {
    if (!input) return;
    if (input->prompt) free(input->prompt);
    free(input);
}

void cli_args_free(CliArgs* args) {
    if (!args) return;
    // argv'yi free etme - main()'e ait
    free(args);
}

void arg_access_free(ArgAccess* access) {
    if (!access) return;
    // value'yu free etme - argv'den geliyor
    free(access);
}

void color_output_free(ColorOutput* color) {
    if (!color) return;
    free(color->text);
    free(color);
}
