// c_compiler/c_lexer.h

#ifndef C_LEXER_H
#define C_LEXER_H

// Token Tipleri (Python'daki TOKEN_TIPLERI sözlüğüne karşılık gelir)
// Enum kullanmak, C'de daha temiz ve hızlıdır.
typedef enum {
    TOKEN_EOF, // 0

    // --- Değer Tipleri ---
    TOKEN_SAYI,
    TOKEN_METIN,
    TOKEN_IDENTIFIER,

    // --- Tip Tanımlamaları ---
    TOKEN_TANIMLA_SAYI,   // SAYISAL
    TOKEN_TANIMLA_METIN,  // METIN
    TOKEN_TANIMLA_BOOL,   // BOOL

    // --- Yapısal Anahtar Kelimeler ---
    TOKEN_YAPI_YAZDIR,    // print
    TOKEN_YAPI_KOSUL_EGER, // if
    TOKEN_YAPI_KOSUL_ISE,  // then
    TOKEN_YAPI_KOSUL_DEGILSE, // else
    TOKEN_YAPI_ISLEC,     // function
    TOKEN_YAPI_DONUS,     // return
    TOKEN_YAPI_DONGU,     // while
    TOKEN_YAPI_DO,        // do
    TOKEN_YAPI_DONGU_BITIR, // break
    TOKEN_YAPI_DONGU_DEVAM, // continue
    TOKEN_YAPI_FOR,       // for
    TOKEN_YAPI_TO,        // to
    TOKEN_YAPI_STEP,      // step
    TOKEN_YAPI_SON,       // end
    TOKEN_YAPI_STRUCT,    // struct
    TOKEN_YAPI_LIST,      // list - Phase 2

    // Phase 5.4: Language Features
    TOKEN_YAPI_ENUM,      // enum
    TOKEN_YAPI_SWITCH,    // switch
    TOKEN_YAPI_CASE,      // case
    TOKEN_YAPI_DEFAULT,   // default

    // --- Built-in Functions - Phase 3 ---
    // File I/O
    TOKEN_BUILTIN_READ_FILE,
    TOKEN_BUILTIN_WRITE_FILE,
    TOKEN_BUILTIN_APPEND_FILE,
    TOKEN_BUILTIN_FILE_EXISTS,
    TOKEN_BUILTIN_FILE_SIZE,
    TOKEN_BUILTIN_READ_LINES,

    // String Operations
    TOKEN_BUILTIN_STRING_SPLIT,
    TOKEN_BUILTIN_STRING_JOIN,
    TOKEN_BUILTIN_STRING_REPLACE,
    TOKEN_BUILTIN_STRING_TRIM,
    TOKEN_BUILTIN_STRING_UPPER,
    TOKEN_BUILTIN_STRING_LOWER,
    TOKEN_BUILTIN_STRING_FIND,
    TOKEN_BUILTIN_STRING_STARTS_WITH,
    TOKEN_BUILTIN_STRING_ENDS_WITH,

    // Type Conversions (Phase 4)
    TOKEN_BUILTIN_INT_TO_STRING,
    TOKEN_BUILTIN_STRING_TO_INT,
    TOKEN_BUILTIN_CHAR_TO_STRING,
    TOKEN_BUILTIN_STRING_CONCAT,

    // Math Operations (Phase 4)
    TOKEN_BUILTIN_MATH_ABS,
    TOKEN_BUILTIN_MATH_MIN,
    TOKEN_BUILTIN_MATH_MAX,
    TOKEN_BUILTIN_MATH_POW,

    // Enhanced String Operations (Phase 5.1)
    TOKEN_BUILTIN_STRING_LENGTH,
    TOKEN_BUILTIN_STRING_SUBSTRING,
    TOKEN_BUILTIN_STRING_INDEX_OF,
    TOKEN_BUILTIN_STRING_LAST_INDEX_OF,

    // Error Handling (Phase 5.2)
    TOKEN_BUILTIN_EXIT_WITH_CODE,
    TOKEN_BUILTIN_PANIC,
    TOKEN_BUILTIN_ASSERT,
    TOKEN_BUILTIN_GET_ERROR_CODE,
    TOKEN_BUILTIN_SET_ERROR_CODE,

    // Memory Management (Phase 5.2)
    TOKEN_BUILTIN_MLP_MALLOC,
    TOKEN_BUILTIN_MLP_FREE,
    TOKEN_BUILTIN_MLP_REALLOC,
    TOKEN_BUILTIN_MLP_CALLOC,
    TOKEN_BUILTIN_GET_ALLOCATED_BYTES,
    TOKEN_BUILTIN_CHECK_MEMORY_LEAKS,

    // System Utilities (Phase 5.2)
    TOKEN_BUILTIN_GET_ENV,
    TOKEN_BUILTIN_CURRENT_TIMESTAMP,
    TOKEN_BUILTIN_SLEEP_MS,

    // Binary File I/O (Phase 5.3)
    TOKEN_BUILTIN_READ_BINARY,
    TOKEN_BUILTIN_WRITE_BINARY,
    TOKEN_BUILTIN_GET_FILE_INFO,
    TOKEN_BUILTIN_COPY_FILE,

    // Directory Operations (Phase 5.3)
    TOKEN_BUILTIN_LIST_DIRECTORY,
    TOKEN_BUILTIN_CREATE_DIRECTORY,
    TOKEN_BUILTIN_REMOVE_DIRECTORY,
    TOKEN_BUILTIN_DIRECTORY_EXISTS,
    TOKEN_BUILTIN_GET_CURRENT_DIR,
    TOKEN_BUILTIN_CHANGE_DIRECTORY,

    // Process Control (Phase 5.3)
    TOKEN_BUILTIN_EXECUTE_COMMAND,
    TOKEN_BUILTIN_GET_COMMAND_OUTPUT,
    TOKEN_BUILTIN_GET_PROCESS_ID,
    TOKEN_BUILTIN_GET_PARENT_PROCESS_ID,

    // Time & Date Utilities (Phase 5.3)
    TOKEN_BUILTIN_FORMAT_TIMESTAMP,
    TOKEN_BUILTIN_PARSE_TIMESTAMP,
    TOKEN_BUILTIN_GET_MILLISECONDS,
    TOKEN_BUILTIN_GET_TIME_STRING,

    // Path Utilities (Phase 5.3)
    TOKEN_BUILTIN_JOIN_PATH,
    TOKEN_BUILTIN_GET_FILE_EXTENSION,
    TOKEN_BUILTIN_GET_FILE_NAME,
    TOKEN_BUILTIN_GET_DIRECTORY,

    // Hash Map (Phase 4)
    TOKEN_YAPI_MAP,  // "map" keyword
    TOKEN_COLON,     // : for key:value syntax

    // --- Operatörler ve Ayırıcılar ---
    // --- Operatörler ve Ayırıcılar ---
    TOKEN_LEFT_PAREN,   // (
    TOKEN_RIGHT_PAREN,  // )
    TOKEN_LEFT_BRACKET,  // [
    TOKEN_RIGHT_BRACKET, // ]
    TOKEN_COMMA,        // ,
    TOKEN_ASSIGN,       // =
    TOKEN_SEMICOLON,    // ;  <-- YENİ
    TOKEN_DOT,          // .  <-- Struct field access

    // Tek Karakterli Operatörler
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MUL,          // *
    TOKEN_DIV,          // /
    TOKEN_GT,             // >
    TOKEN_LT,             // <
    
    // Çok Karakterli Operatörler
    TOKEN_OP_ESIT_KARSILASTIRMA, // ==
    TOKEN_NOT_ESIT,       // !=
    TOKEN_GTE,            // >=
    TOKEN_LTE,            // <=
    TOKEN_MOD,            // % modulo
    TOKEN_AND,            // and keyword
    TOKEN_OR,             // or keyword
    TOKEN_NOT,            // not keyword
    
    // Phase 5.5: Bitwise operators
    TOKEN_BITWISE_AND,    // & bitwise AND
    TOKEN_BITWISE_OR,     // | bitwise OR
    TOKEN_BITWISE_XOR,    // ^ bitwise XOR
    TOKEN_BITWISE_NOT,    // ~ bitwise NOT (unary)
    TOKEN_LSHIFT,         // << left shift
    TOKEN_RSHIFT,         // >> right shift

} TokenType;

// Python'daki Token sınıfına karşılık gelen C struct'ı
typedef struct {
    TokenType type;
    char* value; // Token'ın string değeri (örn: "topla" veya "123")
    int line;    // Token'ın bulunduğu satır numarası (1'den başlar)
    int column;  // Token'ın bulunduğu kolon numarası (1'den başlar)
} Token;

// Lexer Fonksiyon Bildirimleri
void initLexer(const char* source); // <-- BU SATIRI EKLEYİN (Eksik Prototipti)
Token* getNextToken();
Token* peekNextToken(); // Look ahead one token without consuming it

#endif // C_LEXER_H