#ifndef CLI_IO_H
#define CLI_IO_H

#include <stddef.h>
#include "../../lexer.h"

// Çıktı tipleri
typedef enum OutputType {
    OUT_STRING,         // String çıktısı
    OUT_NUMBER,         // Sayısal çıktısı
    OUT_BOOLEAN,        // Boolean çıktısı
    OUT_FORMATTED       // Formatlanmış çıktı
} OutputType;

// Print operasyonu
typedef struct PrintOp {
    OutputType type;        // Çıktı tipi
    void* value;            // Yazdırılacak değer
    char* format;           // Format string (opsiyonel)
    int newline;            // Satır sonu ekle mi?
} PrintOp;

// Input operasyonu
typedef struct InputOp {
    char* prompt;           // Kullanıcıya gösterilecek mesaj
    OutputType expected;    // Beklenen veri tipi
    void* buffer;           // Girdi buffer'ı
    size_t max_size;        // Maksimum boyut
} InputOp;

// CLI argümanları
typedef struct CliArgs {
    int argc;               // Argüman sayısı
    char** argv;            // Argüman dizisi
} CliArgs;

// Argüman erişimi
typedef struct ArgAccess {
    int index;              // Argüman indexi
    char* value;            // Argüman değeri (runtime)
} ArgAccess;

// Çıktı renklendirme (ANSI)
typedef enum AnsiColor {
    COLOR_RESET = 0,
    COLOR_BLACK,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_BLUE,
    COLOR_MAGENTA,
    COLOR_CYAN,
    COLOR_WHITE
} AnsiColor;

typedef struct ColorOutput {
    AnsiColor color;        // Renk
    char* text;             // Metin
    int bold;               // Kalın mı?
} ColorOutput;

// API fonksiyonları
PrintOp* cli_create_print(OutputType type, const char* format, int newline);
InputOp* cli_create_input(const char* prompt, OutputType expected, size_t max_size);
CliArgs* cli_create_args(int argc, char** argv);
ArgAccess* cli_create_arg_access(int index);
ColorOutput* cli_create_color_output(AnsiColor color, const char* text, int bold);

void print_op_free(PrintOp* print);
void input_op_free(InputOp* input);
void cli_args_free(CliArgs* args);
void arg_access_free(ArgAccess* access);
void color_output_free(ColorOutput* color);

#endif // CLI_IO_H
