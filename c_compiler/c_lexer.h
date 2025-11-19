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
    TOKEN_YAPI_DONGU_BITIR, // break
    TOKEN_YAPI_DONGU_DEVAM, // continue
    TOKEN_YAPI_FOR,       // for
    TOKEN_YAPI_TO,        // to
    TOKEN_YAPI_STEP,      // step
    TOKEN_YAPI_SON,       // end
    TOKEN_YAPI_STRUCT,    // struct

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

#endif // C_LEXER_H