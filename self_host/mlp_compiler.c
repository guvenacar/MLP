/* MLP Bootstrap Compiler - Single File Version */
/* This is the LAST C code - MLP self-host coming soon! */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <stdbool.h>
#include <locale.h>
#include <wchar.h>
#include <wctype.h>
#include <libgen.h>

#include "../runtime/hashmap.h"  // HashMap support for function registry

// ========== Function Registry Structures ==========

// Forward declaration (ASTNode will be defined later)
typedef struct ASTNode ASTNode;

// Parameter information
typedef struct {
    char* name;             // Parameter name ("b")
    ASTNode* default_value; // Default value AST node (NULL if no default)
} ParameterInfo;

// Function signature
typedef struct {
    char* name;             // Function name ("add")
    int param_count;        // Total parameter count
    ParameterInfo* params;  // Parameter details array
} FunctionSignature;

// Global Function Registry (HashMap)
HashMap* function_registry = NULL;

// Forward declarations for registry functions (implemented after ASTNode definition)
void register_function_signature(ASTNode* node);
void pre_scan_functions(ASTNode* root);

// ========== c_lexer.h ==========
// c_compiler/c_lexer.h

#ifndef C_LEXER_H
#define C_LEXER_H

// Token Typeleri (Python'daki TOKEN_TIPLERI sözlüğüne karşılık gelir)
// Enum kullanmak, C'de daha temiz ve hızlıdır.
typedef enum {
    TOKEN_EOF, // 0

    // --- Değer Typeleri ---
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_INTERPOLATED_STRING,  // Phase 5.9: String interpolation "Hello {name}"
    TOKEN_IDENTIFIER,
    TOKEN_TRUE,       // true literal
    TOKEN_FALSE,      // false literal
    TOKEN_NULL,       // null literal

    // --- Type Tanımlamaları ---
    TOKEN_TYPE_NUMERIC,   // SAYISAL
    TOKEN_TYPE_STRING,  // METIN
    TOKEN_TYPE_BOOLEAN,   // BOOL

    // --- Yapısal Anahtar Kelimeler ---
    TOKEN_PRINT,    // print
    TOKEN_IF, // if
    TOKEN_THEN,  // then
    TOKEN_ELSE, // else
    TOKEN_FUNCTION,     // function
    TOKEN_ASYNC,        // async (Phase 8: Async/Await)
    TOKEN_AWAIT,        // await (Phase 8: Async/Await)
    TOKEN_RETURN,     // return
    TOKEN_WHILE,     // while
    TOKEN_YAPI_DO,        // do
    TOKEN_WHILE_BITIR, // break
    TOKEN_WHILE_DEVAM, // continue
    TOKEN_FOR,       // for
    TOKEN_TO,        // to
    TOKEN_STEP,      // step
    TOKEN_END,       // end (generic)
    
    // Self-documenting ends (Phase 6+)
    TOKEN_END_IF,     // "end eğer" / "end if"
    TOKEN_END_WHILE,    // "end döngü" / "end while"
    TOKEN_END_FOR,      // "end for"
    TOKEN_END_FUNCTION,    // "end function"
    TOKEN_END_STRUCT,   // "end struct"
    TOKEN_END_ENUM,     // "end enum"
    TOKEN_END_SWITCH,   // "end switch"
    
    TOKEN_YAPI_STRUCT,    // struct
    TOKEN_LIST,      // list - Phase 2
    TOKEN_OPTIONAL,  // optional - Phase 6.2: Generic optional type
    TOKEN_MAP,       // map - Phase 4: Hash maps

    // Phase 5.4: Language Features
    TOKEN_YAPI_ENUM,      // enum
    TOKEN_YAPI_SWITCH,    // switch
    TOKEN_YAPI_CASE,      // case
    TOKEN_YAPI_DEFAULT,   // default

    // Phase 5.6: Additional Features
    TOKEN_CONST,     // const
    TOKEN_TYPEOF,    // typeof - Phase 5.7: Type introspection
    TOKEN_IMPORT,    // import - Phase 5.10: Module system
    
    // Phase 7.1: Lambda Expressions
    TOKEN_LAMBDA,    // lambda - lambda(x, y) => x + y
    TOKEN_ARROW,     // => - lambda arrow operator
    
    // Phase 5.8: Range and For-Each
    TOKEN_IN,        // in - for X in Y
    TOKEN_RANGE,     // range - range(start, end, step)

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
    
    // Type Casting Helpers (Phase 5.7)
    TOKEN_BUILTIN_NUM,              // num(str) - Convert string to numeric
    TOKEN_BUILTIN_STR,              // str(num) - Convert numeric to string
    TOKEN_BUILTIN_READ_INPUT,       // read_input() - Read line from stdin
    TOKEN_BUILTIN_READ_LINE,        // read_line() - Alias for read_input
    TOKEN_BUILTIN_READ_INT,         // read_int() - Read integer from stdin

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
    
    // Modern String API Wrappers (Phase 5.6)
    TOKEN_BUILTIN_LEN,              // len(str) - Python style
    TOKEN_BUILTIN_ORD,              // ord(ch) - Python style
    TOKEN_BUILTIN_CHR,              // chr(code) - Python style
    TOKEN_BUILTIN_CHAR_CODE,        // char_code(str, idx)
    TOKEN_BUILTIN_CHAR_AT,          // char_at(str, idx)
    TOKEN_BUILTIN_SUBSTRING,        // substring(str, start, len)
    TOKEN_BUILTIN_CONTAINS,         // contains(str, needle)
    TOKEN_BUILTIN_STARTSWITH,       // startswith(str, prefix)
    TOKEN_BUILTIN_ENDSWITH,         // endswith(str, suffix)
    TOKEN_BUILTIN_LEFT,             // left(str, len) - VB.NET style
    TOKEN_BUILTIN_RIGHT,            // right(str, len) - VB.NET style
    TOKEN_BUILTIN_MID,              // mid(str, start, len) - VB.NET style

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
    TOKEN_COLON,        // :  <-- Map key:value separator

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

    // Phase 5.7: Compound assignment operators
    TOKEN_PLUS_ASSIGN,    // +=
    TOKEN_MINUS_ASSIGN,   // -=
    TOKEN_MUL_ASSIGN,     // *=
    TOKEN_DIV_ASSIGN,     // /=
    TOKEN_INCREMENT,      // ++
    TOKEN_DECREMENT,      // --

    // Phase 5.8: Control flow & debugging
    TOKEN_QUESTION,       // ? (ternary operator)
    TOKEN_BREAK,          // break keyword
    TOKEN_CONTINUE,       // continue keyword
    TOKEN_STOP,           // stop keyword (debugging breakpoint)
    TOKEN_GOTO,           // goto keyword
    TOKEN_LABEL,          // label: (label definition)

} TokenType;

// Python'daki Token sınıfına karşılık gelen C struct'ı
typedef struct {
    TokenType type;
    char* value; // Token'ın string değeri (örn: "topla" or "123")
    int line;    // Token'ın bulunduğu satır numarası (1'den başlar)
    int column;  // Token'ın bulunduğu kolon numarası (1'den başlar)
    // Phase 5.9: String interpolation support
    char** interpolation_parts;  // String parçaları ["Hello ", "{name}", "!"]
    int interpolation_count;     // Parça sayısı
} Token;

// Lexer Fonksiyon Bildirimleri
void initLexer(const char* source); // <-- BU SATIRI EKLEYİN (Eksik Prototipti)
Token* getNextToken();
Token* check_keyword(const char* word); // Phase 8: Keyword checker (forward declaration)
Token* peekNextToken(); // Look ahead one token without consuming it

#endif // C_LEXER_H
// ========== c_ast.h ==========
// c_compiler/c_ast.h

#ifndef C_AST_H
#define C_AST_H


// --- 1. Düğüm Typelerini Tanımla (Enum) ---
typedef enum {
    AST_BLOK,
    AST_SAYI,
    AST_METIN,
    AST_INTERPOLATED_STRING,  // Phase 5.9: String interpolation
    AST_IMPORT,              // Phase 5.10: Module import
    AST_DEGISKEN,
    AST_IKILI_ISLEM,
    AST_VARIABLE_DECLARATION,
    AST_ASSIGNMENT_KOMUTU,
    AST_PRINT_STATEMENT,
    AST_IF_STATEMENT_KOMUTU, // <-- YENİ (IF)
    AST_WHILE_LOOP_KOMUTU, // <-- YENİ (WHILE - while)
    AST_FOR_KOMUTU, // <-- YENİ (FOR - for loop)
    AST_FOR_IN_RANGE,       // Phase 5.8: for X in range(...) loop
    AST_FOR_EACH,           // Phase 5.8: for X in array/list loop
    AST_WHILE_LOOP_BITIR_KOMUTU, // <-- YENİ (WHILE_BITIR - break)
    AST_WHILE_LOOP_DEVAM_KOMUTU, // <-- YENİ (continue)
    AST_FUNCTION_DECLARATION,
    AST_RETURN_STATEMENT_KOMUTU,
    AST_ISLEC_CAGIRMA,
    AST_TYPEOF_EXPR,        // typeof operator - Phase 5.7
    AST_ARRAY_TANIMLAMA,    // Array bildirimi: SAYISAL arr[10];
    AST_ARRAY_ERISIM,       // Array erişim: arr[5]
    AST_ARRAY_ATAMA,        // Array atama: arr[5] = 10;
    AST_ARRAY_LITERAL,      // Array literal: [1, 2, 3, 4, 5]
    AST_STRUCT_DECLARATION,   // Struct tanımlama: YAPI Nokta İSE ... SON
    AST_STRUCT_FIELD_ACCESS,// Struct field erişim: p.x
    AST_STRUCT_FIELD_ATAMA, // Struct field atama: p.x = 10;
    AST_STRUCT_VARIABLE,    // Struct değişken: Nokta p;
    
    // Phase 6: Dynamic Lists (Modern Generic Syntax)
    AST_LIST_TANIMLAMA,     // List tanımlama: List<int> numbers = List<int>();
    AST_LIST_LITERAL,       // List literal: (1, 2, 3, 4, 5)
    AST_LIST_ADD,           // List add: numbers.add(10)
    AST_LIST_GET,           // List get: numbers.get(0)
    AST_LIST_SET,           // List set: numbers.set(0, 10)
    AST_LIST_REMOVE,        // List remove: numbers.remove(0)
    AST_LIST_INSERT,        // List insert: numbers.insert(0, 10)
    AST_LIST_SIZE,          // List size: numbers.length() or numbers.size()
    AST_LIST_CLEAR,         // List clear: numbers.clear()

    // Phase 6.2: Optional<T> - Generic Optional Type
    AST_OPTIONAL_TANIMLAMA,  // Optional declaration: optional<string> text = optional<string>()
    AST_OPTIONAL_SOME,       // Optional with value: optional<int> x = some(42)
    AST_OPTIONAL_HAS_VALUE,  // Check if has value: text.has_value()
    AST_OPTIONAL_VALUE,      // Get value: text.value()
    AST_OPTIONAL_VALUE_OR,   // Get value or default: text.value_or("default")

    // Phase 3: Built-in Functions
    AST_BUILTIN_CALL,       // Built-in function call: read_file("path")

    // Phase 4: Hash Maps
    AST_MAP_TANIMLAMA,      // Map declaration: map[string:int] ages = map();
    AST_MAP_SET,            // Map set: ages.set("Alice", 25)
    AST_MAP_GET,            // Map get: ages.get("Alice")
    AST_MAP_HAS,            // Map has: ages.has("Bob")
    AST_MAP_REMOVE,         // Map remove: ages.remove("Charlie")
    AST_MAP_SIZE,           // Map size: ages.size()
    AST_MAP_CLEAR,          // Map clear: ages.clear()

    // Phase 5.4: Language Features
    AST_ENUM_DECLARATION,     // Enum definition: enum Color then RED = 0 ... end
    AST_ENUM_VALUE,         // Enum value reference: Color.RED or just RED
    AST_SWITCH_KOMUTU,      // Switch statement
    AST_CASE_KOMUTU,        // Case clause

    // Phase 5.8: Control flow & debugging
    AST_TERNARY,            // Ternary operator: condition ? true_val : false_val
    AST_BREAK,              // break keyword
    AST_CONTINUE,           // continue keyword
    AST_STOP,               // stop keyword (debugging breakpoint - int3)
    AST_GOTO,               // goto label
    AST_LABEL,              // label:

    // Phase 7.1: Lambda Expressions
    AST_LAMBDA,             // Lambda expression: lambda(x, y) => x + y
    
    // Phase 8: Async/Await
    AST_ASYNC_FUNCTION,     // async function declaration
    AST_AWAIT_EXPR,         // await expression
} ASTNodeType;

// İleri Bildirimler (C'de iç içe struct'lar için gerekli)
typedef struct ASTNode ASTNode;
typedef struct Blok ASTBlok;

// --- 2. Temel AST Düğümü Yapısı ---
// Tüm düğümlerin ortak özelliklerini tutar
struct ASTNode {
    ASTNodeType type;
    
    // YENİ: Düğümlere Özgü Veri
    // Bu, C'deki "Union" yapısının kullanımını gerektirir (ileri aşamada)
    // Şimdilik sadece ana struct'ları tanımlayalım:
    union {
        // Blok yapısı (Blok, Döngü, İşleç Gövdesi)
        struct {
            ASTNode** komutlar; // Komut dizisi (Heap'ten ayrılacak)
            int sayisi;         // Komut sayısı
        } blok_data;
        
        // Sayi/Metin yapısı
        struct {
            char* deger; // Token'dan gelen değer
        } sabit_data;

        // Phase 5.9: String Interpolation
        struct {
            char** parts;     // String parçaları: literal ve {variable} patterns
            int part_count;   // Parça sayısı
        } interpolation_data;

        // Phase 5.10: Import Statement
        struct {
            char* file_path;  // İmport edilecek dosya yolu
        } import_data;

        // İkili İşlem yapısı (a + b)
        struct {
            ASTNode* sol;
            ASTNode* sag;
            TokenType operator_type;
        } ikili_islem_data;
        
        // YENİ: Değişken/Identifier
        struct {
            Token* ad;
        } degisken_data;

        // Değişken Tanımlama (SAYISAL a = 5;)
        struct {
            Token* tip;
            Token* ad;
            ASTNode* ifade; // Atanan değer
            bool is_const;  // Phase 5.6: const değişken mi?
        } tanimlama_data;
        
        // YENİ: Atama Komutu (sayac = 10;)
        struct {
            Token* ad;
            ASTNode* ifade; // Atanan değer
        } atama_data;
        
        // YENİ: Koşul Komutu (IF ifade İSE blok [DEĞİLSE blok])
        struct {
            ASTNode* kosul;       // (sayac == 5)
            ASTNode* ise_blok;    // IF doğruysa çalışacak blok
            ASTNode* degilse_blok; // (Opsiyonel) IF yanlışsa çalışacak blok
        } kosul_data;
        
        // YENİ: Döngü Komutu (WHILE blok SON - while loop)
        struct {
            ASTNode* kosul;     // While condition (NULL = infinite loop)
            ASTNode* govde;     // Döngü gövdesi
        } dongu_data;

        // YENİ: For Döngüsü (for i = 0 to 10 [step 2])
        struct {
            Token* degisken;    // Loop variable (i)
            ASTNode* baslangic; // Start value (0)
            ASTNode* bitis;     // End value (10)
            ASTNode* adim;      // Step value (optional, default 1)
            ASTNode* govde;     // Loop body
        } for_data;

        // İşleç Tanımlama (Fonksiyon)
        struct {
            Token* ad;
            Token** parametreler; // YENİ: Parametre adları (Token listesi)
            int parametre_sayisi; // YENİ
            int* parametre_is_array; // Phase 7.6: Which parameters are arrays (1=array, 0=regular)
            ASTNode** parametre_default_degerleri; // Phase 5.11: Default parameter values (NULL if no default)
            ASTNode* govde;
        } islec_tanimlama_data;

        // Phase 7.1: Lambda Expression
        struct {
            Token** parametreler; // Lambda parametreleri
            int parametre_sayisi;
            ASTNode* govde;       // Tek ifade veya blok
            bool is_expression;   // true = tek ifade, false = blok
            // Phase 7.9: Closure support
            char** captured_vars; // Captured variable names
            int captured_count;   // Number of captured variables
        } lambda_data;
        
        // Phase 8: Async Function (async function name() ... end)
        struct {
            Token* ad;            // Function name
            Token** parametreler; // Parameters
            int parametre_sayisi;
            ASTNode* govde;       // Function body
        } async_function_data;
        
        // Phase 8: Await Expression (await promise_expr)
        struct {
            ASTNode* ifade;       // Promise expression to await
        } await_data;
        
        // İşleç Çağırma (topla(5, 7))
        struct {
            Token* hedef_ad;
            ASTNode** argumanlar;
            int arguman_sayisi;
        } islec_cagirma_data;
        
        // typeof Operator (typeof(expr)) - Phase 5.7
        struct {
            ASTNode* ifade;  // Expression to get type of
        } typeof_data;
        
        // Diğer komutlar (PRINT, DÖNÜŞ, ATAMA)
        struct {
            ASTNode* ifade;
        } tek_ifade_data;

        // Array Tanımlama (SAYISAL arr[10];)
        struct {
            Token* tip;        // SAYISAL, METIN vb.
            Token* ad;         // Array ismi
            ASTNode* boyut;    // Array boyutu (sabit ifade)
        } array_tanimlama_data;

        // Array Erişim (arr[5], arr[i+1])
        struct {
            Token* ad;         // Array ismi
            ASTNode* indeks;   // İndeks ifadesi
        } array_erisim_data;

        // Array Atama (arr[5] = 10;)
        struct {
            Token* ad;         // Array ismi
            ASTNode* indeks;   // İndeks ifadesi
            ASTNode* deger;    // Atanacak değer
        } array_atama_data;

        // Array Literal ([1, 2, 3, 4, 5])
        struct {
            ASTNode** elemanlar;  // Array elemanları
            int eleman_sayisi;    // Eleman sayısı
        } array_literal_data;

        // Struct Tanımlama (YAPI Nokta İSE SAYISAL x; SAYISAL y; SON)
        struct {
            Token* ad;              // Struct ismi (Nokta)
            Token** field_tipleri;  // Field tipleri (SAYISAL, METIN, vb.)
            Token** field_adlari;   // Field isimleri (x, y, vb.)
            int field_sayisi;       // Field sayısı
        } struct_tanimlama_data;

        // Struct Field Access (p.x)
        struct {
            Token* struct_ad;  // Struct değişken ismi (p)
            Token* field_ad;   // Field ismi (x)
        } struct_field_access_data;

        // Struct Field Atama (p.x = 10;)
        struct {
            Token* struct_ad;  // Struct değişken ismi (p)
            Token* field_ad;   // Field ismi (x)
            ASTNode* deger;    // Atanacak değer
        } struct_field_atama_data;

        // Struct Değişken (Nokta p;)
        struct {
            Token* struct_tip; // Struct tipi (Nokta)
            Token* ad;         // Değişken ismi (p)
        } struct_degisken_data;

        // ===== Phase 6: Dynamic Lists (Modern Generic Syntax) =====

        // List Tanımlama (numeric items() or numeric items(10) or numeric items() = (1,2,3))
        struct {
            Token* element_tipi;  // Element tipi (numeric, string, boolean)
            Token* degisken_adi;  // List değişken ismi (items)
            ASTNode* capacity;    // Initial capacity (NULL for default) or literal
            ASTNode* initial_value; // List literal for initialization (NULL if none)
        } list_tanimlama_data;

        // List Literal ((1, 2, 3, 4, 5))
        struct {
            ASTNode** elemanlar;  // List elemanları
            int eleman_sayisi;    // Eleman sayısı
        } list_literal_data;

        // List Add (numbers.add(10))
        struct {
            Token* list_adi;      // List ismi (numbers)
            ASTNode* deger;       // Eklenecek değer
        } list_add_data;

        // List Get (numbers.get(0))
        struct {
            Token* list_adi;      // List ismi (numbers)
            ASTNode* indeks;      // İndeks ifadesi
        } list_get_data;

        // List Set (numbers.set(0, 10))
        struct {
            Token* list_adi;      // List ismi (numbers)
            ASTNode* indeks;      // İndeks ifadesi
            ASTNode* deger;       // Yeni değer
        } list_set_data;

        // List Remove (numbers.remove(0))
        struct {
            Token* list_adi;      // List ismi (numbers)
            ASTNode* indeks;      // İndeks ifadesi
        } list_remove_data;

        // List Insert (numbers.insert(0, 10))
        struct {
            Token* list_adi;      // List ismi (numbers)
            ASTNode* indeks;      // İndeks ifadesi
            ASTNode* deger;       // Eklenecek değer
        } list_insert_data;

        // List Size (numbers.length() or numbers.size())
        struct {
            Token* list_adi;      // List ismi (numbers)
        } list_size_data;

        // List Clear (numbers.clear())
        struct {
            Token* list_adi;      // List ismi (numbers)
        } list_clear_data;

        // ===== Phase 6.2: Optional<T> - Generic Optional Type =====

        // Optional Tanımlama (optional<string> text = optional<string>())
        struct {
            Token* element_tipi;   // Element tipi (int, string, struct name)
            Token* degisken_adi;   // Optional değişken ismi (text)
        } optional_tanimlama_data;

        // Optional some(value) constructor
        struct {
            Token* optional_adi;   // Optional variable name
            ASTNode* deger;        // Value to wrap
        } optional_some_data;

        // Optional has_value() method
        struct {
            Token* optional_adi;   // Optional variable name
        } optional_has_value_data;

        // Optional value() method
        struct {
            Token* optional_adi;   // Optional variable name
        } optional_value_data;

        // Optional value_or(default) method
        struct {
            Token* optional_adi;   // Optional variable name
            ASTNode* default_deger; // Default value if empty
        } optional_value_or_data;

        // Phase 3: Built-in Function Call
        struct {
            TokenType function_type;  // Which built-in function
            ASTNode* arg1;           // First argument
            ASTNode* arg2;           // Second argument (optional)
            ASTNode* arg3;           // Third argument (optional, for join)
        } builtin_call_data;

        // ===== Phase 4: Hash Maps =====

        // Map Tanımlama (map[string:int] ages = map();)
        struct {
            Token* key_tipi;      // Key type (string, int)
            Token* value_tipi;    // Value type (string, int)
            Token* degisken_adi;  // Map variable name (ages)
        } map_tanimlama_data;

        // Map Set (ages.set("Alice", 25))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
            ASTNode* value;       // Value expression
        } map_set_data;

        // Map Get (ages.get("Alice"))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
        } map_get_data;

        // Map Has (ages.has("Bob"))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
        } map_has_data;

        // Map Remove (ages.remove("Charlie"))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
        } map_remove_data;

        // Map Size (ages.size())
        struct {
            Token* map_adi;       // Map name (ages)
        } map_size_data;

        // Map Clear (ages.clear())
        struct {
            Token* map_adi;       // Map name (ages)
        } map_clear_data;

        // ===== Phase 5.4: Language Features =====

        // Enum Tanımlama (enum Color then RED = 0 GREEN = 1 end)
        struct {
            Token* ad;              // Enum ismi (Color)
            Token** value_adlari;   // Değer isimleri (RED, GREEN, BLUE)
            int* value_degerleri;   // Değerler (0, 1, 2) - NULL ise otomatik
            int value_sayisi;       // Değer sayısı
        } enum_tanimlama_data;

        // Enum Value (Color.RED or sadece RED)
        struct {
            Token* enum_ad;         // Enum ismi (opsiyonel, NULL olabilir)
            Token* value_ad;        // Değer ismi (RED)
        } enum_value_data;

        // Switch Statement
        struct {
            ASTNode* ifade;         // Switch edilecek ifade
            ASTNode** cases;        // Case listesi
            int case_sayisi;        // Case sayısı
            ASTNode* default_blok;  // Default bloğu (opsiyonel)
        } switch_data;

        // Case Clause
        struct {
            ASTNode* deger;         // Case değeri (sabit)
            ASTNode* blok;          // Case bloğu
        } case_data;

        // ===== Phase 5.8: Control flow & debugging =====

        // Ternary operator (condition ? true_val : false_val)
        struct {
            ASTNode* kosul;         // Condition expression
            ASTNode* dogru_deger;   // True value
            ASTNode* yanlis_deger;  // False value
        } ternary_data;

        // Goto statement (goto label_name)
        struct {
            Token* label_adi;       // Label name
        } goto_data;

        // Label definition (label_name:)
        struct {
            Token* label_adi;       // Label name
        } label_data;

        // Phase 5.8: For-in-range loop (for i in range(...))
        struct {
            Token* degisken;    // Loop variable (i)
            ASTNode* start;     // Start value (0 or expression)
            ASTNode* end;       // End value (10 or expression)
            ASTNode* step;      // Step value (optional, default 1)
            ASTNode* govde;     // Loop body
        } for_in_range_data;

        // Phase 5.8: For-each loop (for item in array/list)
        struct {
            Token* degisken;    // Loop variable (item)
            Token* array_adi;   // Array/list name to iterate over
            ASTNode* govde;     // Loop body
        } for_each_data;
    };
};

// Fonksiyon Prototipleri
ASTNode* parse(const char* source_code); // Ana ayrıştırma fonksiyonu
void freeAST(ASTNode* node);             // AST temizleme (Bellek yönetimi)

// Phase 7.9: Closure support
void find_free_variables(ASTNode* node, char** free_vars, int* free_count, 
                         Token** lambda_params, int param_count);

// Phase 3: Built-in function helper
ASTNode* createAST_BuiltinCall(TokenType func_type, ASTNode* arg1, ASTNode* arg2, ASTNode* arg3);

// Phase 4: Hash map helpers
ASTNode* createAST_MapTanimlama(Token* key_tipi, Token* value_tipi, Token* degisken_adi);
ASTNode* createAST_MapSet(Token* map_adi, ASTNode* key, ASTNode* value);
ASTNode* createAST_MapGet(Token* map_adi, ASTNode* key);
ASTNode* createAST_MapHas(Token* map_adi, ASTNode* key);
ASTNode* createAST_MapRemove(Token* map_adi, ASTNode* key);
ASTNode* createAST_MapSize(Token* map_adi);
ASTNode* createAST_MapClear(Token* map_adi);

#endif // C_AST_H
// ========== c_parser.h ==========
// c_compiler/c_parser.h

#ifndef C_PARSER_H
#define C_PARSER_H


ASTNode* parse(const char* source_code);

// Parser'ın temel ayrıştırma fonksiyonları
ASTNode* ifade(); 
ASTNode* ikili_islem(int onceki_oncelik); 
ASTNode* birincil(); // Zaten mevcuttu, emin olmak için buraya ekliyorum

// Parser'ın ana işlevleri
ASTNode* parse(const char* source_code);

// Lexer'ın başlatılması (c_lexer.c'den)
void initLexer(const char* source);
Token* getNextToken();

#endif // C_PARSER_H
// ========== backend_common.h ==========
// backend_common.h
// Common interface for all MLP backends (C, Assembly, LLVM, etc.)

#ifndef BACKEND_COMMON_H
#define BACKEND_COMMON_H


// Backend capabilities
typedef struct {
    bool supports_auto_free;      // Otomatik bellek yönetimi (scope-based free)
    bool supports_cross_platform; // Cross-platform derleme
    bool supports_debugging;      // Debug symbols üretimi
    bool requires_external_tools; // GCC, NASM gibi harici araçlar gerekiyor mu
} BackendCapabilities;

// Backend interface - Her backend bu yapıyı implement eder
typedef struct {
    // Backend bilgileri
    const char* name;              // "C" or "Assembly"
    const char* version;           // Backend versiyonu (örn: "1.0")
    const char* file_extension;    // Çıktı dosya uzantısı (".c" or ".asm")
    const char* description;       // Kısa açıklama
    
    // Backend yetenekleri
    BackendCapabilities capabilities;
    
    // Backend fonksiyonları
    void (*initialize)(void);                                    // Backend'i başlat
    void (*generate)(ASTNode* root, const char* output_file);   // AST'den kod üret
    void (*cleanup)(void);                                       // Backend'i temizle
    
    // Opsiyonel: Debug/diagnostics
    void (*print_stats)(void);                                   // İstatistikleri yazdır (opsiyonel)
} Backend;

// Backend registry - Tüm backend'leri döndüren fonksiyonlar
Backend* get_c_backend(void);      // C backend'i al
Backend* get_asm_backend(void);    // Assembly backend'i al

#endif // BACKEND_COMMON_H

// ========== c_generator.h ==========
// c_compiler/c_generator.h

#ifndef C_GENERATOR_H
#define C_GENERATOR_H


// --- Generator Durum Yönetimi ---

// Üretilen Assembly kodunu dinamik olarak tutmak için basit bir yapı
// (İleride 'realloc' ile büyütülecek)
typedef struct {
    char* code;   // Dinamik olarak ayrılan kod metni
    size_t size;  // Mevcut boyut
    size_t capacity; // Ayrılmış kapasite
} AsmCode;

// --- Ana Fonksiyon Prototipi ---

/**
 * Ana AST düğümünü (root) alır ve tam Assembly kodunu döndürür.
 */
char* generate_asm(ASTNode* root);

// ========== Function Registry Implementation ==========

// Register a function signature into the registry (without generating assembly)
void register_function_signature(ASTNode* node) {
    if (node->type != AST_FUNCTION_DECLARATION) return;

    char* func_name = node->islec_tanimlama_data.ad->value;
    
    // Check if already registered (avoid duplicate definitions)
    if (hashmap_get(function_registry, func_name) != NULL) {
        fprintf(stderr, "WARNING: Function '%s' already registered, skipping duplicate\n", func_name);
        return;
    }

    FunctionSignature* sig = (FunctionSignature*)malloc(sizeof(FunctionSignature));
    sig->name = strdup(func_name);
    sig->param_count = node->islec_tanimlama_data.parametre_sayisi;
    sig->params = (ParameterInfo*)malloc(sizeof(ParameterInfo) * sig->param_count);

    for (int i = 0; i < sig->param_count; i++) {
        sig->params[i].name = strdup(node->islec_tanimlama_data.parametreler[i]->value);
        
        // Check for default value in AST
        if (node->islec_tanimlama_data.parametre_default_degerleri && 
            node->islec_tanimlama_data.parametre_default_degerleri[i] != NULL) {
            sig->params[i].default_value = node->islec_tanimlama_data.parametre_default_degerleri[i];
        } else {
            sig->params[i].default_value = NULL;
        }
    }

    hashmap_put(function_registry, sig->name, sig);
}

// Pre-scan AST to register all function signatures before code generation
void pre_scan_functions(ASTNode* root) {
    if (!root) return;

    if (root->type == AST_BLOK) {
        // Scan top-level statements for function declarations
        for (int i = 0; i < root->blok_data.sayisi; i++) {
            ASTNode* stmt = root->blok_data.komutlar[i];
            if (stmt && stmt->type == AST_FUNCTION_DECLARATION) {
                register_function_signature(stmt);
            }
        }
    }
}

// ==========================================================

#endif // C_GENERATOR_H
// ========== c_backend.h ==========
// c_backend.h
// C Code Generator Backend for MLP Compiler
// Converts AST to C source code

#ifndef C_BACKEND_H
#define C_BACKEND_H


// Backend initialization and cleanup
void c_backend_init(void);
void c_backend_cleanup(void);

// Main generation function
void c_backend_generate(ASTNode* root, const char* output_file);

// Helper: Get backend interface
Backend* get_c_backend(void);

// === Internal functions (for implementation) ===

// Code generation helpers
void generate_c_header(FILE* f);
void generate_c_program(FILE* f, ASTNode* node);
void generate_c_statement(FILE* f, ASTNode* node, int indent);
void generate_c_expression(FILE* f, ASTNode* node);

// Type mapping (MLP type -> C type)
const char* map_mlp_type_to_c(const char* mlp_type);

// Indentation helper
void write_indent(FILE* f, int level);

// Scope tracking (for auto-free)
typedef struct {
    char* var_name;
    char* var_type;
    bool needs_free; // Heap allocation gerekiyorsa true
} ScopeVariable;

void scope_push_variable(const char* var_name, const char* var_type, bool needs_free);
void scope_pop_all(void);
void generate_scope_cleanup(FILE* f, int indent);

#endif // C_BACKEND_H

// ========== main.c ==========
// c_compiler/main.c


// Global variable to store the absolute path of the input file's directory
char* base_dir = NULL;

void printAST_helper(ASTNode* node, int indent) {
    if (node == NULL) {
        printf("%*sNULL\n", indent, "");
        return;
    }

    if (node->type == AST_SAYI) {
        printf("%*sSAYI(%s)\n", indent, "", node->sabit_data.deger);
    } else if (node->type == AST_IKILI_ISLEM) {
        const char* op_str = "?";
        switch (node->ikili_islem_data.operator_type) {
            case TOKEN_PLUS: op_str = "+"; break;
            case TOKEN_MINUS: op_str = "-"; break;
            case TOKEN_MUL: op_str = "*"; break;
            case TOKEN_DIV: op_str = "/"; break;
            case TOKEN_GT: op_str = ">"; break;
            case TOKEN_LT: op_str = "<"; break;
            case TOKEN_OP_ESIT_KARSILASTIRMA: op_str = "=="; break;
            default: break;
        }
        printf("%*sIKILI_ISLEM(%s)\n", indent, "", op_str);
        printf("%*s  Sol:\n", indent, "");
        printAST_helper(node->ikili_islem_data.sol, indent + 4);
        printf("%*s  Sag:\n", indent, "");
        printAST_helper(node->ikili_islem_data.sag, indent + 4);
    } else if (node->type == AST_DEGISKEN) {
        printf("%*sDEGISKEN(%s)\n", indent, "", node->degisken_data.ad->value);
    } else if (node->type == AST_ISLEC_CAGIRMA) {
        printf("%*sISLEC_CAGIRMA(%s)\n", indent, "", node->islec_cagirma_data.hedef_ad->value);
        // TODO: Argümanları da bas
    } else if (node->type == AST_BLOK) {
        printf("%*sBLOK (Komut Sayısı: %d)\n", indent, "", node->blok_data.sayisi);
        for (int i = 0; i < node->blok_data.sayisi; i++) {
            printf("%*s  Komut[%d]:\n", indent, "", i);
            printAST_helper(node->blok_data.komutlar[i], indent + 4);
        }
    } else if (node->type == AST_PRINT_STATEMENT) {
        printf("%*sPRINT_KOMUTU\n", indent, "");
        printf("%*s  İfade:\n", indent, "");
        printAST_helper(node->tek_ifade_data.ifade, indent + 4);
    } else if (node->type == AST_VARIABLE_DECLARATION) {
        printf("%*sDEGISKEN_TANIMLAMA\n", indent, "");
        printf("%*s  Type: %s\n", indent, "", node->tanimlama_data.tip->value);
        printf("%*s  Ad: %s\n", indent, "", node->tanimlama_data.ad->value);
        printf("%*s  İfade:\n", indent, "");
        printAST_helper(node->tanimlama_data.ifade, indent + 4);
    } else if (node->type == AST_ASSIGNMENT_KOMUTU) {
        printf("%*sATAMA_KOMUTU\n", indent, "");
        printf("%*s  Hedef Ad: %s\n", indent, "", node->atama_data.ad->value);
        printf("%*s  İfade:\n", indent, "");
        printAST_helper(node->atama_data.ifade, indent + 4);
    } else if (node->type == AST_IF_STATEMENT_KOMUTU) {
        printf("%*sKOSUL_KOMUTU (IF)\n", indent, "");
        printf("%*s  Koşul:\n", indent, "");
        printAST_helper(node->kosul_data.kosul, indent + 4);
        printf("%*s  İse Bloğu:\n", indent, "");
        printAST_helper(node->kosul_data.ise_blok, indent + 4);
        if (node->kosul_data.degilse_blok) {
            printf("%*s  Değilse Bloğu:\n", indent, "");
            printAST_helper(node->kosul_data.degilse_blok, indent + 4);
        }
    } else if (node->type == AST_WHILE_LOOP_KOMUTU) {
        printf("%*sDONGU_KOMUTU\n", indent, "");
        printf("%*s  Gövde:\n", indent, "");
        printAST_helper(node->dongu_data.govde, indent + 4);
    }
    else if (node->type == AST_WHILE_LOOP_BITIR_KOMUTU) {
        printf("%*sDONGU_BITIR_KOMUTU\n", indent, "");
    } else if (node->type == AST_FUNCTION_DECLARATION) {
        printf("%*sISLEC_TANIMLAMA\n", indent, "");
        printf("%*s  Ad: %s\n", indent, "", node->islec_tanimlama_data.ad->value);
        printf("%*s  Gövde:\n", indent, "");
        printAST_helper(node->islec_tanimlama_data.govde, indent + 4);
    }else {
        printf("%*sAST { Type: %d }\n", indent, "", node->type);
    }
}

// Function to be called from the generator
const char* get_base_dir() {
    return base_dir;
}

int main(int argc, char* argv[]) {
    fprintf(stderr, "BAŞLANGIÇ: main() başladı\n");
    
    // Initialize function registry
    fprintf(stderr, "BAŞLANGIÇ: hashmap_create çağrılıyor\n");
    function_registry = hashmap_create(128);
    fprintf(stderr, "BAŞLANGIÇ: hashmap_create tamamlandı, registry=%p\n", (void*)function_registry);
    
    if (argc < 3) {
        fprintf(stderr, "Kullanım: %s <girdi_dosyasi.tyd> <cikti_dosyasi.asm>\n", argv[0]);
        return 1;
    }

    const char* girdi_dosya_adi = argv[1];
    const char* cikti_dosya_adi = argv[2];

    fprintf(stderr, "BAŞLANGIÇ: girdi=%s, çıktı=%s\n", girdi_dosya_adi, cikti_dosya_adi);

    // --- Resolve absolute path and set base_dir ---
    fprintf(stderr, "BAŞLANGIÇ: realpath çağrılıyor\n");
    char* absolute_input_path = realpath(girdi_dosya_adi, NULL);
    fprintf(stderr, "BAŞLANGIÇ: realpath tamamlandı, path=%s\n", absolute_input_path ? absolute_input_path : "NULL");
    if (absolute_input_path == NULL) {
        perror("Gerçek dosya yolu çözümlenemedi");
        return 1;
    }
    
    // dirname() can modify its argument, so we pass a copy.
    char* path_copy = strdup(absolute_input_path);
    base_dir = dirname(path_copy);
    // Note: base_dir now points to internal static storage of dirname 
    // or to path_copy. To be safe, let's copy it.
    base_dir = strdup(base_dir);
    // free(path_copy); // We can free the copy of the path
    // No, dirname may point inside path_copy, so we can't free it yet.
    // Let's leak it for now, or manage memory more carefully.
    // For this compiler, a small leak on startup is acceptable.

    // TÜRKÇE KARAKTER DESTEĞİNİ AKTİFLEŞTİR
    setlocale(LC_ALL, "");

    // --- Dosyadan Okuma ---
    FILE* girdi_dosyasi = fopen(absolute_input_path, "r");
    if (girdi_dosyasi == NULL) {
        perror("Girdi dosyası açılamadı");
        free(absolute_input_path);
        // free(base_dir) is tricky because of dirname behavior
        return 1;
    }
    free(absolute_input_path); // We have the content, no need for the path anymore

    fseek(girdi_dosyasi, 0, SEEK_END);
    long dosya_boyutu = ftell(girdi_dosyasi);
    fseek(girdi_dosyasi, 0, SEEK_SET);

    char* kaynak_kodu = (char*)malloc(dosya_boyutu + 1);
    fread(kaynak_kodu, 1, dosya_boyutu, girdi_dosyasi);
    kaynak_kodu[dosya_boyutu] = '\0';
    fclose(girdi_dosyasi);

    fprintf(stderr, "Derleniyor: %s\n", girdi_dosya_adi);

    // 1. Parser'ı çalıştır
    ASTNode* root = parse(kaynak_kodu);
    free(kaynak_kodu); // Kaynak kodunu serbest bırak

    fprintf(stderr, "Ayrıştırma Başarılı. Şimdi Assembly Üretiliyor...\n");

    // 2. Pre-scan: Register all function signatures before code generation
    pre_scan_functions(root);

    // 3. Generator'ı Çalıştır (now with full function registry)
    char* asm_code = generate_asm(root);

    // --- Dosyaya Yazma ---
    FILE* cikti_dosyasi = fopen(cikti_dosya_adi, "w");
    if (cikti_dosyasi == NULL) {
        perror("Çıktı dosyası oluşturulamadı");
        free(asm_code);
        return 1;
    }
    fprintf(cikti_dosyasi, "%s", asm_code);
    fclose(cikti_dosyasi);

    free(asm_code);
    // free(base_dir); // Clean up the duplicated base directory path
    // free(path_copy);

    fprintf(stderr, "Başarıyla derlendi: %s -> %s\n", girdi_dosya_adi, cikti_dosya_adi);

    return 0;
}
// ========== c_lexer.c ==========
// c_compiler/c_lexer.c


// --- Lexer Durum Yönetimi ---

// Kaynak kodu (Source Code) dize olarak tutulur.
static const char* source_code = NULL;

// Okuma pozisyonu (Lexer'ın kaynak kodda nerede olduğunu takip eder).
static int current_position = 0;
static int eof_reached = 0; // <-- YENİ: EOF bayrağı

// Line ve kolon tracking (hata mesajları için)
static int current_line = 1;    // Line numarası (1'den başlar)
static int current_column = 1;  // Column numarası (1'den başlar)

// --- Bellek Yönetimi ve Yardımcı Fonksiyonlar ---

/**
 * Yeni bir Token nesnesi oluşturur ve bellek ayırır.
 * NOT: Bu fonksiyon, dinamik bellek (malloc) kullandığı için,
 * oluşturulan Token'ların program sonunda serbest bırakılması (free) gerekir.
 */
Token* createToken(TokenType type, const char* value) {
    // Token struct'ı için bellek ayır
    Token* token = (Token*)malloc(sizeof(Token));
    if (token == NULL) {
        perror("Hafıza ayırma hatası (Token)");
        exit(EXIT_FAILURE);
    }

    token->type = type;
    
    // Phase 5.9: Initialize interpolation fields
    token->interpolation_parts = NULL;
    token->interpolation_count = 0;

    // Değer (value) string'ini dinamik olarak kopyala
    if (value != NULL) {
        token->value = strdup(value);
        if (token->value == NULL) {
            perror("Hafıza ayırma hatası (Token Value)");
            exit(EXIT_FAILURE);
        }
    } else {
        token->value = NULL;
    }

    // Pozisyon bilgisini ekle
    token->line = current_line;
    token->column = current_column;

    return token;
}

typedef struct {
    const char* keyword;
    TokenType type;
} KeywordMap;

// English Base IR keywords for MLP compiler
// Multi-language support via preprocessor (stage0/mlp_preprocessor.py)
// All source languages (Turkish, Russian, Chinese, etc.) are translated to English
KeywordMap keywords[] = {
    // Type Declarations (MLP keywords - NO int/float/bool!)
    {"numeric",  TOKEN_TYPE_NUMERIC},
    {"string",   TOKEN_TYPE_STRING},
    {"boolean",  TOKEN_TYPE_BOOLEAN},
    {"const",    TOKEN_CONST},
    {"typeof",   TOKEN_TYPEOF},
    {"import",   TOKEN_IMPORT},    // Phase 5.10: Module system
    {"lambda",   TOKEN_LAMBDA},    // Phase 7.1: Lambda expressions
    {"in",       TOKEN_IN},        // Phase 5.8: for X in Y
    {"range",    TOKEN_RANGE},     // Phase 5.8: range(start, end, step)

    // Control Flow Keywords
    {"print",    TOKEN_PRINT},
    {"if",       TOKEN_IF},
    {"then",     TOKEN_THEN},
    {"else",     TOKEN_ELSE},
    {"function", TOKEN_FUNCTION},
    {"return",   TOKEN_RETURN},
    {"while",    TOKEN_WHILE},
    {"do",       TOKEN_YAPI_DO},
    {"for",      TOKEN_FOR},
    {"to",       TOKEN_TO},
    {"step",     TOKEN_STEP},
    {"break",    TOKEN_BREAK},
    {"end",      TOKEN_END},       // Generic end (will check for compound)

    // Boolean and NULL literals
    {"true",     TOKEN_TRUE},
    {"false",    TOKEN_FALSE},
    {"null",     TOKEN_NULL},

    {"and",      TOKEN_AND},
    {"or",       TOKEN_OR},
    {"not",      TOKEN_NOT},
    {"struct",   TOKEN_YAPI_STRUCT},
    {"list",     TOKEN_LIST},      // Phase 2: Dynamic lists
    {"optional", TOKEN_OPTIONAL},  // Phase 6.2: Generic optional type
    {"map",      TOKEN_MAP},       // Phase 4: Hash maps

    // Phase 5.8: Control flow & debugging
    {"stop",     TOKEN_STOP},
    {"goto",     TOKEN_GOTO},

    // Phase 5.4: Language Features
    {"enum",     TOKEN_YAPI_ENUM},
    {"switch",   TOKEN_YAPI_SWITCH},
    {"case",     TOKEN_YAPI_CASE},
    {"default",  TOKEN_YAPI_DEFAULT},

    // Phase 3: Built-in Functions
    // File I/O
    {"read_file",    TOKEN_BUILTIN_READ_FILE},
    {"write_file",   TOKEN_BUILTIN_WRITE_FILE},
    {"append_file",  TOKEN_BUILTIN_APPEND_FILE},
    {"file_exists",  TOKEN_BUILTIN_FILE_EXISTS},
    {"file_size",    TOKEN_BUILTIN_FILE_SIZE},
    {"read_lines",   TOKEN_BUILTIN_READ_LINES},

    // String Operations
    {"string_split",       TOKEN_BUILTIN_STRING_SPLIT},
    {"string_join",        TOKEN_BUILTIN_STRING_JOIN},
    {"string_replace",     TOKEN_BUILTIN_STRING_REPLACE},
    {"string_trim",        TOKEN_BUILTIN_STRING_TRIM},
    {"string_upper",       TOKEN_BUILTIN_STRING_UPPER},
    {"string_lower",       TOKEN_BUILTIN_STRING_LOWER},
    {"string_find",        TOKEN_BUILTIN_STRING_FIND},
    {"string_starts_with", TOKEN_BUILTIN_STRING_STARTS_WITH},
    {"string_ends_with",   TOKEN_BUILTIN_STRING_ENDS_WITH},

    // Phase 4: Type Conversions
    {"int_to_string",   TOKEN_BUILTIN_INT_TO_STRING},
    {"string_to_int",   TOKEN_BUILTIN_STRING_TO_INT},
    {"char_to_string",  TOKEN_BUILTIN_CHAR_TO_STRING},
    {"string_concat",   TOKEN_BUILTIN_STRING_CONCAT},
    
    // Phase 5.7: Type Casting Helpers
    {"num",             TOKEN_BUILTIN_NUM},
    {"str",             TOKEN_BUILTIN_STR},
    {"read_input",      TOKEN_BUILTIN_READ_INPUT},
    {"read_line",       TOKEN_BUILTIN_READ_LINE},
    {"read_int",        TOKEN_BUILTIN_READ_INT},

    // Phase 4: Math Operations
    {"math_abs", TOKEN_BUILTIN_MATH_ABS},
    {"math_min", TOKEN_BUILTIN_MATH_MIN},
    {"math_max", TOKEN_BUILTIN_MATH_MAX},
    {"math_pow", TOKEN_BUILTIN_MATH_POW},

    // Phase 5.1: Enhanced String Operations
    {"string_length",        TOKEN_BUILTIN_STRING_LENGTH},
    {"string_substring",     TOKEN_BUILTIN_STRING_SUBSTRING},
    {"string_index_of",      TOKEN_BUILTIN_STRING_INDEX_OF},
    {"string_last_index_of", TOKEN_BUILTIN_STRING_LAST_INDEX_OF},
    
    // Phase 5.6: Modern String API Wrappers
    {"len",                  TOKEN_BUILTIN_LEN},
    {"ord",                  TOKEN_BUILTIN_ORD},
    {"chr",                  TOKEN_BUILTIN_CHR},
    {"char_code",            TOKEN_BUILTIN_CHAR_CODE},
    {"char_at",              TOKEN_BUILTIN_CHAR_AT},
    {"substring",            TOKEN_BUILTIN_SUBSTRING},
    {"contains",             TOKEN_BUILTIN_CONTAINS},
    {"startswith",           TOKEN_BUILTIN_STARTSWITH},
    {"endswith",             TOKEN_BUILTIN_ENDSWITH},
    {"left",                 TOKEN_BUILTIN_LEFT},
    {"right",                TOKEN_BUILTIN_RIGHT},
    {"mid",                  TOKEN_BUILTIN_MID},

    // Phase 5.2: Error Handling
    {"exit_with_code",       TOKEN_BUILTIN_EXIT_WITH_CODE},
    {"panic",                TOKEN_BUILTIN_PANIC},
    {"assert",               TOKEN_BUILTIN_ASSERT},
    {"get_error_code",       TOKEN_BUILTIN_GET_ERROR_CODE},
    {"set_error_code",       TOKEN_BUILTIN_SET_ERROR_CODE},

    // Phase 5.2: Memory Management
    {"mlp_malloc",           TOKEN_BUILTIN_MLP_MALLOC},
    {"mlp_free",             TOKEN_BUILTIN_MLP_FREE},
    {"mlp_realloc",          TOKEN_BUILTIN_MLP_REALLOC},
    {"mlp_calloc",           TOKEN_BUILTIN_MLP_CALLOC},
    {"get_allocated_bytes",  TOKEN_BUILTIN_GET_ALLOCATED_BYTES},
    {"check_memory_leaks",   TOKEN_BUILTIN_CHECK_MEMORY_LEAKS},

    // Phase 5.2: System Utilities
    {"get_env",              TOKEN_BUILTIN_GET_ENV},
    {"current_timestamp",    TOKEN_BUILTIN_CURRENT_TIMESTAMP},
    {"sleep_ms",             TOKEN_BUILTIN_SLEEP_MS},

    // Phase 5.3: Binary File I/O
    {"read_binary",          TOKEN_BUILTIN_READ_BINARY},
    {"write_binary",         TOKEN_BUILTIN_WRITE_BINARY},
    {"get_file_info",        TOKEN_BUILTIN_GET_FILE_INFO},
    {"copy_file",            TOKEN_BUILTIN_COPY_FILE},

    // Phase 5.3: Directory Operations
    {"list_directory",       TOKEN_BUILTIN_LIST_DIRECTORY},
    {"create_directory",     TOKEN_BUILTIN_CREATE_DIRECTORY},
    {"remove_directory",     TOKEN_BUILTIN_REMOVE_DIRECTORY},
    {"directory_exists",     TOKEN_BUILTIN_DIRECTORY_EXISTS},
    {"get_current_dir",      TOKEN_BUILTIN_GET_CURRENT_DIR},
    {"change_directory",     TOKEN_BUILTIN_CHANGE_DIRECTORY},

    // Phase 5.3: Process Control
    {"execute_command",      TOKEN_BUILTIN_EXECUTE_COMMAND},
    {"get_command_output",   TOKEN_BUILTIN_GET_COMMAND_OUTPUT},
    {"get_process_id",       TOKEN_BUILTIN_GET_PROCESS_ID},
    {"get_parent_process_id", TOKEN_BUILTIN_GET_PARENT_PROCESS_ID},

    // Phase 5.3: Time & Date Utilities
    {"format_timestamp",     TOKEN_BUILTIN_FORMAT_TIMESTAMP},
    {"parse_timestamp",      TOKEN_BUILTIN_PARSE_TIMESTAMP},
    {"get_milliseconds",     TOKEN_BUILTIN_GET_MILLISECONDS},
    {"get_time_string",      TOKEN_BUILTIN_GET_TIME_STRING},

    // Phase 5.3: Path Utilities
    {"join_path",            TOKEN_BUILTIN_JOIN_PATH},
    {"get_file_extension",   TOKEN_BUILTIN_GET_FILE_EXTENSION},
    {"get_file_name",        TOKEN_BUILTIN_GET_FILE_NAME},
    {"get_directory",        TOKEN_BUILTIN_GET_DIRECTORY},

    // Phase 4: Hash Map
    {"map", TOKEN_MAP},

    // Boolean and null literals
    {"true",     TOKEN_IDENTIFIER},  // Will be handled as identifier
    {"false",    TOKEN_IDENTIFIER},  // Will be handled as identifier
    // null handled in generator as 0

    {NULL, TOKEN_EOF}
};

/**
 * Lexer'ı başlatır ve kaynak kodu hazırlar.
 */
void initLexer(const char* source) {
    // UTF-8 desteği için locale ayarla
    setlocale(LC_ALL, "en_US.UTF-8");

    source_code = source;
    current_position = 0;
    eof_reached = 0; // <-- RESET

    // Pozisyon tracking'i reset et
    current_line = 1;
    current_column = 1;
}


// --- Ana Lexer Mantığı ---

/**
 * Pozisyon ilerletme helper fonksiyonu
 * current_position'ı 1 artırır ve line/column'u günceller
 */
static inline void advance_position() {
    if (source_code[current_position] == '\0') {
        return; // EOF'ta ilerleme yok
    }

    if (source_code[current_position] == '\n') {
        current_line++;
        current_column = 1;
    } else if (source_code[current_position] == '\t') {
        current_column += 4; // Tab = 4 space
    } else {
        current_column++;
    }

    current_position++;
}

/**
 * Helper: Check if character is valid for identifier (alphanumeric or underscore or UTF-8)
 */
static int isIdentifierChar(char c) {
    return isalnum((unsigned char)c) || c == '_' || (unsigned char)c >= 0x80;  // UTF-8 support
}

/**
 * Tüm yorumları ve boşlukları atlayan helper fonksiyon
 * Hem tek satır (--) hem de çok satır ({- ... -}) yorumları destekler
 */
static void skip_whitespace_and_comments() {
    while (1) {
        // Boşlukları atla
        while (source_code[current_position] != '\0' && isspace(source_code[current_position])) {
            advance_position();
        }

        // Tek satır yorum: -- or //
        if ((source_code[current_position] == '-' && source_code[current_position + 1] == '-') ||
            (source_code[current_position] == '/' && source_code[current_position + 1] == '/')) {
            // Line sonuna kadar atla
            while (source_code[current_position] != '\n' && source_code[current_position] != '\0') {
                advance_position();
            }
            continue; // Tekrar başa dön (boşluk ve yorum kontrolü için)
        }

        // Çok satır yorum: {- ... -}
        if (source_code[current_position] == '{' && source_code[current_position + 1] == '-') {
            // {- işaretini atla
            advance_position(); // {
            advance_position(); // -

            // -} bulana kadar devam et
            while (source_code[current_position] != '\0') {
                if (source_code[current_position] == '-' && source_code[current_position + 1] == '}') {
                    // -} işaretini atla
                    advance_position(); // -
                    advance_position(); // }
                    break;
                }
                advance_position();
            }
            continue; // Tekrar başa dön (boşluk ve yorum kontrolü için)
        }

        // Ne boşluk ne de yorum varsa, çık
        break;
    }
}

/**
 * Python'daki sonraki_tokeni_al() metoduna karşılık gelir.
 * Kaynak koddan bir sonraki tokeni okur ve döndürür.
 */
Token* getNextToken() {

    // 1. EOF zaten döndürüldüyse hemen EOF döndür.
    if (eof_reached) {
        return createToken(TOKEN_EOF, NULL);
    }

    // 2. Boşlukları ve Yorumları Atla
    skip_whitespace_and_comments();


    // 3. Dosya Sonuna (EOF) ulaşıldı
    if (source_code[current_position] == '\0') {
        eof_reached = 1; // Bayrağı ayarla
        return createToken(TOKEN_EOF, NULL);
    }

    // --- YENİ: UTF-8 UYUMLU KARAKTER OKUMA ---
    wchar_t current_wc = L'\0'; // Geniş karakter (wide character)
    int char_len = mbtowc(&current_wc, &source_code[current_position], 4); // En fazla 4 byte oku

    // mbtowc başarısız olsa bile UTF-8 multi-byte karakterler için devam et
    if (char_len <= 0) {
        // Eğer byte UTF-8 multi-byte başlangıcı ise (>= 0x80), devam et
        if ((unsigned char)source_code[current_position] >= 0x80) {
            current_wc = L' '; // Dummy value - identifier check'e geçsin
            char_len = 1;
        } else {
            // Gerçekten EOF
            eof_reached = 1;
            return createToken(TOKEN_EOF, NULL);
        }
    }
    // -----------------------------------------

    // YENİ: 4. Sayısal Sabit (INTEGER) Okuma
    if (iswdigit(current_wc)) {
        int start = current_position;
        // Tüm rakamları okumaya devam et
        while (source_code[current_position] != '\0') {
            wchar_t temp_wc;
            int temp_len = mbtowc(&temp_wc, &source_code[current_position], 4);
            if (temp_len > 0 && iswdigit(temp_wc)) {
                current_position += temp_len;
            } else {
                break;
            }
        }
        
        // Token değerini (string'i) yakala
        int len = current_position - start;
        char* value = (char*)malloc(len + 1);
        strncpy(value, source_code + start, len);
        value[len] = '\0';
        Token* token = createToken(TOKEN_NUMBER, value);
        free(value); // createToken zaten kopyalıyor
        return token;
    }


    // UTF-8 harf kontrol (ASCII harf or UTF-8 multi-byte: 0x80-0xFF)
    if (iswalpha(current_wc) || (unsigned char)source_code[current_position] >= 0x80) {
        int start = current_position;
        // İsimler harf, rakam, UTF-8 karakterler or alt çizgi içerebilir
        while (source_code[current_position] != '\0') {
            unsigned char byte = (unsigned char)source_code[current_position];

            // ASCII harf/rakam/underscore
            if (isalnum(byte) || byte == '_') {
                current_position++;
                continue;
            }

            // UTF-8 multi-byte karakter başlangıcı (0xC0-0xFF)
            if (byte >= 0xC0) {
                // UTF-8 continuation bytes'ları da ekle
                current_position++;
                while (current_position < strlen(source_code)) {
                    unsigned char cont = (unsigned char)source_code[current_position];
                    if (cont >= 0x80 && cont < 0xC0) {  // Continuation byte (10xxxxxx)
                        current_position++;
                    } else {
                        break;
                    }
                }
                continue;
            }

            // Hiçbiri değilse dur
            break;
        }

        // Token değerini (string'i) yakala
        int len = current_position - start;
        char* value = (char*)malloc(len + 1);
        strncpy(value, source_code + start, len);
        value[len] = '\0';

        // Phase 8: Use check_keyword() to handle all keywords including async/await
        Token* token = check_keyword(value);
        if (token == NULL) {
            // Not a keyword - create identifier token
            token = createToken(TOKEN_IDENTIFIER, value);
        }
        free(value);
        return token;
    }

    // Compound keyword handling is inside check_keyword()
    
    char current_char = source_code[current_position]; // Tek baytlık operatörler için hala gerekli

    // Phase 5.6: CHARACTER LITERAL ('A', '\n', etc.) - Returns integer ASCII value
    if (current_char == '\'' && source_code[current_position + 1] != '\0') {
        int start_pos = current_position;
        current_position++; // Açılış tırnağını atla
        int char_value;
        int is_escape = 0;

        if (source_code[current_position] == '\\') {
            is_escape = 1;
            current_position++;
            switch (source_code[current_position]) {
                case 'n':  char_value = 10; break;
                case 't':  char_value = 9;  break;
                case 'r':  char_value = 13; break;
                case '\\': char_value = 92; break;
                case '\'': char_value = 39; break;
                case '"':  char_value = 34; break;
                case '0':  char_value = 0;  break;
                default:   char_value = (unsigned char)source_code[current_position]; break;
            }
        } else {
            char_value = (unsigned char)source_code[current_position];
        }
        current_position++;

        // Kapanış tırnağını kontrol et - tek karakter ise int döndür
        if (source_code[current_position] == '\'') {
            current_position++;
            char value_str[16];
            sprintf(value_str, "%d", char_value);
            return createToken(TOKEN_NUMBER, value_str);
        } else {
            // Tek karakterli değilse, string olarak işle - geri sar
            current_position = start_pos;
        }
    }

    // STRING LITERAL Okuma ("merhaba" or 'multi-char string' or """multi-line""")
    if (current_char == '"' || current_char == '\'') {
        char quote_char = current_char;
        
        // Phase 5.8: Check for triple-quote multi-line string (""")
        int is_multiline = 0;
        if (quote_char == '"' && 
            source_code[current_position + 1] == '"' && 
            source_code[current_position + 2] == '"') {
            is_multiline = 1;
            current_position += 3; // Skip opening """
        } else {
            current_position++; // Skip single quote
        }

        // Escape sequence'leri işleyerek string'i oluştur
        char* value = (char*)malloc(4096); // Larger buffer for multi-line strings
        int value_index = 0;
        int has_interpolation = 0; // Phase 5.9: Check for {variable} patterns

        if (is_multiline) {
            // Multi-line string: read until closing """
            while (source_code[current_position] != '\0') {
                // Check for closing """
                if (source_code[current_position] == '"' &&
                    source_code[current_position + 1] == '"' &&
                    source_code[current_position + 2] == '"') {
                    current_position += 3; // Skip closing """
                    break;
                }
                
                // Phase 5.9: Check for interpolation pattern {variable}
                if (source_code[current_position] == '{' &&
                    source_code[current_position + 1] != '\0' &&
                    source_code[current_position + 1] != '}') {
                    has_interpolation = 1;
                }
                
                // Include everything literally (including newlines)
                value[value_index++] = source_code[current_position];
                current_position++;
                
                if (value_index >= 4095) {
                    fprintf(stderr, "ERROR [Lexer]: Multi-line string too long (max 4096 chars)\n");
                    free(value);
                    return createToken(TOKEN_EOF, NULL);
                }
            }
            
            if (source_code[current_position - 3] != '"' || 
                source_code[current_position - 2] != '"' ||
                source_code[current_position - 1] != '"') {
                fprintf(stderr, "ERROR [Lexer]: Unclosed multi-line string (missing closing \"\"\")\n");
                free(value);
                return createToken(TOKEN_EOF, NULL);
            }
        } else {
            // Single-line string: original logic
            // Kapanış tırnağına kadar oku ve escape sequence'leri dönüştür
            while (source_code[current_position] != '\0' && source_code[current_position] != quote_char) {
                // Phase 5.9: Check for interpolation pattern {variable}
                if (source_code[current_position] == '{' &&
                    source_code[current_position + 1] != '\0' &&
                    source_code[current_position + 1] != '}') {
                    has_interpolation = 1;
                }
                
                // Escape sequence desteği (\n, \t, \", \\, vb.)
                if (source_code[current_position] == '\\' && source_code[current_position + 1] != '\0') {
                    current_position++; // Backslash'i atla
                    char escape_char = source_code[current_position];

                // Escape karakterini gerçek karaktere dönüştür
                switch (escape_char) {
                    case 'n':  value[value_index++] = '\n'; break;  // Newline
                    case 't':  value[value_index++] = '\t'; break;  // Tab
                    case 'r':  value[value_index++] = '\r'; break;  // Carriage return
                    case '\\': value[value_index++] = '\\'; break;  // Backslash
                    case '"':  value[value_index++] = '"';  break;  // Quote
                    case '\'': value[value_index++] = '\''; break;  // Single quote
                    case '0':  value[value_index++] = '\0'; break;  // Null character
                    default:
                        // Bilinmeyen escape - olduğu gibi ekle
                        value[value_index++] = '\\';
                        value[value_index++] = escape_char;
                        break;
                }
                current_position++;
            } else {
                value[value_index++] = source_code[current_position];
                current_position++;
            }
            }
            
            if (source_code[current_position] == '\0') {
                fprintf(stderr, "ERROR [Lexer]: Kapanmamış string literal\n");
                free(value);
                return createToken(TOKEN_EOF, NULL);
            }

            value[value_index] = '\0'; // String'i sonlandır
            current_position++; // Kapanış tırnağını atla
        }

        value[value_index] = '\0'; // String'i sonlandır (multi-line için de)
        
        // Phase 5.9: Create appropriate token type based on interpolation
        TokenType token_type = has_interpolation ? TOKEN_INTERPOLATED_STRING : TOKEN_STRING;
        Token* token = createToken(token_type, value);
        free(value);
        return token;
    }

    // --- YENİ: İki Karakterli Operatörler (İleri Bakış ile) ---
    char next_char = source_code[current_position + 1];

    // == (Eşit Karşılaştırma)
    if (current_char == '=' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_OP_ESIT_KARSILASTIRMA, "==");
    }
    
    // != (Eşit Değil)
    if (current_char == '!' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_NOT_ESIT, "!=");
    }

    // Phase 7.1: => (Lambda arrow) - Must be before single '=' check!
    if (current_char == '=' && next_char == '>') {
        current_position += 2;
        return createToken(TOKEN_ARROW, "=>");
    }

    // >= (Büyük Eşit)
    if (current_char == '>' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_GTE, ">=");
    }
    
    // <= (Küçük Eşit)
    if (current_char == '<' && next_char == '=') {
        current_position += 2;
        return createToken(TOKEN_LTE, "<=");
    }
    
    // Phase 5.5: Bitwise shift operators
    // << (Left Shift)
    if (current_char == '<' && next_char == '<') {
        current_position += 2;
        return createToken(TOKEN_LSHIFT, "<<");
    }
    
    // >> (Right Shift)
    if (current_char == '>' && next_char == '>') {
        current_position += 2;
        return createToken(TOKEN_RSHIFT, ">>");
    }

    // --- YENİ: Tek Karakterli Operatörler ve Ayırıcılar ---
    if (current_char == '(') {
        current_position++;
        return createToken(TOKEN_LEFT_PAREN, "(");
    }
    if (current_char == ')') {
        current_position++;
        return createToken(TOKEN_RIGHT_PAREN, ")");
    }
    if (current_char == '[') {
        current_position++;
        return createToken(TOKEN_LEFT_BRACKET, "[");
    }
    if (current_char == ']') {
        current_position++;
        return createToken(TOKEN_RIGHT_BRACKET, "]");
    }
    if (current_char == '+') {
        current_position++;
        if (source_code[current_position] == '=') {
            current_position++;
            return createToken(TOKEN_PLUS_ASSIGN, "+=");
        }
        return createToken(TOKEN_PLUS, "+");
    }
    if (current_char == '-') {
        current_position++;
        if (source_code[current_position] == '=') {
            current_position++;
            return createToken(TOKEN_MINUS_ASSIGN, "-=");
        }
        return createToken(TOKEN_MINUS, "-");
    }
    if (current_char == '*') {
        current_position++;
        if (source_code[current_position] == '=') {
            current_position++;
            return createToken(TOKEN_MUL_ASSIGN, "*=");
        }
        return createToken(TOKEN_MUL, "*");
    }
    if (current_char == '/') {
        current_position++;
        if (source_code[current_position] == '=') {
            current_position++;
            return createToken(TOKEN_DIV_ASSIGN, "/=");
        }
        return createToken(TOKEN_DIV, "/");
    }
    if (current_char == '%') {
        current_position++;
        return createToken(TOKEN_MOD, "%");
    }
    if (current_char == '>') {
        current_position++;
        return createToken(TOKEN_GT, ">");
    }
    if (current_char == '<') {
        current_position++;
        return createToken(TOKEN_LT, "<");
    }
    if (current_char == '=') {
        current_position++;
        return createToken(TOKEN_ASSIGN, "=");
    }
    if (current_char == ';') {
        current_position++;
        return createToken(TOKEN_SEMICOLON, ";"); 
    }
    if (current_char == ',') {
        current_position++;
        return createToken(TOKEN_COMMA, ",");
    }
    if (current_char == '.') {
        current_position++;
        return createToken(TOKEN_DOT, ".");
    }
    if (current_char == ':') {
        current_position++;
        return createToken(TOKEN_COLON, ":");
    }
    
    // Phase 5.8: Ternary operator
    if (current_char == '?') {
        current_position++;
        return createToken(TOKEN_QUESTION, "?");
    }

    // Phase 5.5: Bitwise operators
    if (current_char == '&') {
        current_position++;
        return createToken(TOKEN_BITWISE_AND, "&");
    }
    if (current_char == '|') {
        current_position++;
        return createToken(TOKEN_BITWISE_OR, "|");
    }
    if (current_char == '^') {
        current_position++;
        return createToken(TOKEN_BITWISE_XOR, "^");
    }
    if (current_char == '~') {
        current_position++;
        return createToken(TOKEN_BITWISE_NOT, "~");
    }

    // 4. Tanınmayan Karakter (HATA YÖNETİMİ DÜZELTİLDİ)
    char error_val[5] = {0}; // UTF-8 karakteri en fazla 4 byte + null terminator
    strncpy(error_val, &source_code[current_position], char_len);

    current_position += char_len; 
    
    fprintf(stderr, "ERROR [Lexer]: Tanınmayan karakter: %s\n", error_val);
    
    return createToken(TOKEN_EOF, NULL);
}

// ============================================================================
// BASE ENGLISH KEYWORDS ONLY
// All natural language keywords (Turkish, Russian, etc.) are handled by
// mlp_preprocessor.py using diller_comprehensive.json
// This lexer only processes the normalized English base IR
// ============================================================================

Token* check_keyword(const char* word) {
    // Type declarations (English base - BASE_LANGUAGE.md)
    if (strcmp(word, "numeric") == 0) return createToken(TOKEN_TYPE_NUMERIC, word);
    if (strcmp(word, "string") == 0) return createToken(TOKEN_TYPE_STRING, word);
    if (strcmp(word, "boolean") == 0) return createToken(TOKEN_TYPE_BOOLEAN, word);
    if (strcmp(word, "const") == 0) return createToken(TOKEN_CONST, word);
    if (strcmp(word, "typeof") == 0) return createToken(TOKEN_TYPEOF, word);
    if (strcmp(word, "in") == 0) return createToken(TOKEN_IN, word);
    if (strcmp(word, "range") == 0) return createToken(TOKEN_RANGE, word);
    
    // Control flow (English base)
    if (strcmp(word, "print") == 0) return createToken(TOKEN_PRINT, word);
    if (strcmp(word, "if") == 0) return createToken(TOKEN_IF, word);
    if (strcmp(word, "then") == 0) return createToken(TOKEN_THEN, word);
    if (strcmp(word, "else") == 0) return createToken(TOKEN_ELSE, word);
    if (strcmp(word, "function") == 0) return createToken(TOKEN_FUNCTION, word);
    if (strcmp(word, "async") == 0) return createToken(TOKEN_ASYNC, word);      // Phase 8
    if (strcmp(word, "await") == 0) return createToken(TOKEN_AWAIT, word);      // Phase 8
    if (strcmp(word, "return") == 0) return createToken(TOKEN_RETURN, word);
    if (strcmp(word, "while") == 0) return createToken(TOKEN_WHILE, word);
    if (strcmp(word, "break") == 0) return createToken(TOKEN_WHILE_BITIR, word);
    
    // Self-documenting ends (English base)
    // Phase 8: Compound keywords disabled - they break lexer position
    // Only use plain 'end' for now
    if (strcmp(word, "end") == 0) {
        return createToken(TOKEN_END, word);
    }
    
    if (strcmp(word, "struct") == 0) return createToken(TOKEN_YAPI_STRUCT, word);

    // Not a keyword - return NULL (will be treated as identifier)
    return NULL;
}
// Peek next token without consuming it
// Used for lookahead in parser (e.g., struct instance detection)
Token* peekNextToken() {
    // Save current lexer state
    int saved_pos = current_position;
    int saved_line = current_line;
    int saved_col = current_column;
    int saved_eof = eof_reached;
    
    // Get next token
    Token* peeked = getNextToken();
    
    // Restore lexer state
    current_position = saved_pos;
    current_line = saved_line;
    current_column = saved_col;
    eof_reached = saved_eof;
    
    return peeked;
}

// ========== c_parser.c ==========
// c_compiler/c_parser.c
// (KILAVUZA UYUMLU SÜRÜM: Tanımlamalar hariç ; YOK)


// --- Parser Durum Yönetimi ---
static Token* current_token = NULL;

// ===== Parser-Level Variable Type Tracking =====
#define MAX_PARSE_VARS 100

typedef struct {
    char* name;
    char* type;  // "List", "HashMap", "numeric", "string", etc.
} ParseVar;

ParseVar parse_vars[MAX_PARSE_VARS];
int parse_var_count = 0;

void register_parse_var(const char* name, const char* type) {
    if (parse_var_count >= MAX_PARSE_VARS) return;
    parse_vars[parse_var_count].name = strdup(name);
    parse_vars[parse_var_count].type = strdup(type);
    parse_var_count++;
}

const char* get_parse_var_type(const char* name) {
    for (int i = parse_var_count - 1; i >= 0; i--) {
        if (strcmp(parse_vars[i].name, name) == 0) {
            return parse_vars[i].type;
        }
    }
    return NULL;
}

void clear_parse_vars() {
    for (int i = 0; i < parse_var_count; i++) {
        free(parse_vars[i].name);
        free(parse_vars[i].type);
    }
    parse_var_count = 0;
}

// Token tip ismini döndüren helper fonksiyon
static const char* getTokenTypeName(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_NUMBER: return "SAYI";
        case TOKEN_STRING: return "METIN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_TRUE: return "true";
        case TOKEN_FALSE: return "false";
        case TOKEN_NULL: return "null";
        case TOKEN_TYPE_NUMERIC: return "SAYISAL";
        case TOKEN_TYPE_STRING: return "METIN (tip)";
        case TOKEN_TYPE_BOOLEAN: return "BOOL";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_IF: return "IF";
        case TOKEN_THEN: return "THEN";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_FUNCTION: return "FUNCTION";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_YAPI_DO: return "DO";
        case TOKEN_FOR: return "FOR";
        case TOKEN_TO: return "TO";
        case TOKEN_STEP: return "STEP";
        case TOKEN_END: return "END";
        case TOKEN_LEFT_PAREN: return "(";
        case TOKEN_RIGHT_PAREN: return ")";
        case TOKEN_COMMA: return ",";
        case TOKEN_ASSIGN: return "=";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_PLUS: return "+";
        case TOKEN_MINUS: return "-";
        case TOKEN_MUL: return "*";
        case TOKEN_DIV: return "/";
        case TOKEN_GT: return ">";
        case TOKEN_LT: return "<";
        case TOKEN_OP_ESIT_KARSILASTIRMA: return "==";
        case TOKEN_NOT_ESIT: return "!=";
        case TOKEN_GTE: return ">=";
        case TOKEN_LTE: return "<=";
        case TOKEN_MOD: return "%";
        case TOKEN_AND: return "and";
        case TOKEN_OR: return "or";
        case TOKEN_NOT: return "not";
        case TOKEN_BITWISE_AND: return "&";
        case TOKEN_BITWISE_OR: return "|";
        case TOKEN_BITWISE_XOR: return "^";
        case TOKEN_BITWISE_NOT: return "~";
        case TOKEN_LSHIFT: return "<<";
        case TOKEN_RSHIFT: return ">>";
        case TOKEN_QUESTION: return "?";
        case TOKEN_BREAK: return "break";
        case TOKEN_CONTINUE: return "continue";
        case TOKEN_STOP: return "stop";
        case TOKEN_GOTO: return "goto";
        case TOKEN_LAMBDA: return "lambda";
        case TOKEN_ARROW: return "=>";
        default: return "UNKNOWN";
    }
}

// --- Hata Yönetimi ---
void parseError(const char* message, const char* expected) {
    fprintf(stderr, "\n╔════════════════════════════════════════════════════════════╗\n");
    fprintf(stderr, "║ ERROR [Parser]: Syntax Error!                          ║\n");
    fprintf(stderr, "╚════════════════════════════════════════════════════════════╝\n\n");

    if (current_token != NULL) {
        fprintf(stderr, "📍 Location: Line %d, Column %d\n\n", current_token->line, current_token->column);
        fprintf(stderr, "❌ Found: ");
        if (current_token->value) {
            fprintf(stderr, "\"%s\" (%s)\n", current_token->value, getTokenTypeName(current_token->type));
        } else {
            fprintf(stderr, "%s\n", getTokenTypeName(current_token->type));
        }
    } else {
        fprintf(stderr, "📍 Location: Dosya sonu\n\n");
        fprintf(stderr, "❌ Found: Dosya Sonu or NULL\n");
    }

    fprintf(stderr, "✓ Expected: %s\n\n", expected);

    if (message && strlen(message) > 0) {
        fprintf(stderr, "💡 Description: %s\n\n", message);
    }

    exit(EXIT_FAILURE);
}

// Kural Kontrolü (Hata ayıklama için)
void specs_check_no_semicolon(const char* komut_adi) {
    if (current_token && current_token->type == TOKEN_SEMICOLON) {
        fprintf(stderr, "SPECS.md İhlali: %s sonunda ';' kullanılamaz!\n", komut_adi);
        exit(1);
    }
}

// --- Token Yönetimi ---
void consume(TokenType expected_type) {
    if (current_token == NULL) {
        parseError("Dosya beklenenden önce bitti.", "Herhangi bir Token");
    }
    if (current_token->type == expected_type) {
        if (current_token->value != NULL) {
            free(current_token->value);
        }
        free(current_token);
        current_token = getNextToken();
    } else {
        char expected_str[100];
        sprintf(expected_str, "%s (%s)", getTokenTypeName(expected_type),
                expected_type == TOKEN_SEMICOLON ? "SAYISAL, METIN, BOOL değişken tanımlarında gerekli" : "Token");
        parseError("Token tipi uyuşmuyor.", expected_str);
    }
}

// --- AST Düğümü Oluşturucular ---
// (Dosyanızda bulunan tüm createAST_... fonksiyonları buradadır)
// (createAST_Sayi, createAST_Metin, createAST_Degisken, createAST_IslecCagirma, 
//  createAST_DegiskenTanimlama, createAST_KosulKomutu, createAST_IslecTanimlama...)

ASTNode* createAST_Sayi(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_SAYI;
    node->sabit_data.deger = (token->value != NULL) ? strdup(token->value) : NULL;
    return node;
}

ASTNode* createAST_Metin(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_METIN;
    node->sabit_data.deger = (token->value != NULL) ? strdup(token->value) : NULL;
    return node;
}

// Phase 5.9: Parse interpolated string and split into parts
ASTNode* createAST_InterpolatedString(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_INTERPOLATED_STRING;
    
    // Parse string and extract parts: literals and {variables}
    const char* str = token->value;
    int capacity = 10;
    char** parts = (char**)malloc(sizeof(char*) * capacity);
    int count = 0;
    
    char buffer[1024];
    int buf_idx = 0;
    int i = 0;
    
    while (str[i] != '\0') {
        if (str[i] == '{' && str[i+1] != '\0' && str[i+1] != '}') {
            // Save current literal part if any
            if (buf_idx > 0) {
                buffer[buf_idx] = '\0';
                if (count >= capacity) {
                    capacity *= 2;
                    parts = (char**)realloc(parts, sizeof(char*) * capacity);
                }
                parts[count++] = strdup(buffer);
                buf_idx = 0;
            }
            
            // Extract variable name from {variable}
            i++; // Skip {
            char var_name[256];
            int var_idx = 0;
            while (str[i] != '\0' && str[i] != '}') {
                var_name[var_idx++] = str[i++];
            }
            var_name[var_idx] = '\0';
            
            if (str[i] == '}') i++; // Skip }
            
            // Store as {variable} format for later identification
            if (count >= capacity) {
                capacity *= 2;
                parts = (char**)realloc(parts, sizeof(char*) * capacity);
            }
            char var_pattern[270];
            sprintf(var_pattern, "{%s}", var_name);
            parts[count++] = strdup(var_pattern);
        } else {
            buffer[buf_idx++] = str[i++];
        }
    }
    
    // Save final literal part if any
    if (buf_idx > 0) {
        buffer[buf_idx] = '\0';
        if (count >= capacity) {
            capacity *= 2;
            parts = (char**)realloc(parts, sizeof(char*) * capacity);
        }
        parts[count++] = strdup(buffer);
    }
    
    node->interpolation_data.parts = parts;
    node->interpolation_data.part_count = count;
    return node;
}

ASTNode* createAST_Degisken(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_DEGISKEN;
    node->degisken_data.ad = (Token*)malloc(sizeof(Token));
    node->degisken_data.ad->type = token->type;
    node->degisken_data.ad->value = strdup(token->value);
    return node;
}

ASTNode* createAST_IslecCagirma(Token* ad_token, ASTNode** argumanlar, int arguman_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ISLEC_CAGIRMA;
    node->islec_cagirma_data.hedef_ad = (Token*)malloc(sizeof(Token));
    node->islec_cagirma_data.hedef_ad->type = ad_token->type;
    node->islec_cagirma_data.hedef_ad->value = strdup(ad_token->value);
    node->islec_cagirma_data.argumanlar = argumanlar;
    node->islec_cagirma_data.arguman_sayisi = arguman_sayisi;
    return node;
}

ASTNode* createAST_DegiskenTanimlama(Token* tip, Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_VARIABLE_DECLARATION;
    node->tanimlama_data.tip = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.tip->type = tip->type;
    node->tanimlama_data.tip->value = strdup(tip->value);
    node->tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.ad->type = ad->type;
    node->tanimlama_data.ad->value = strdup(ad->value);
    node->tanimlama_data.ifade = ifade;
    node->tanimlama_data.is_const = false; // Default: not const
    return node;
}

ASTNode* createAST_AtamaKomutu(Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ASSIGNMENT_KOMUTU;
    node->atama_data.ad = (Token*)malloc(sizeof(Token));
    node->atama_data.ad->type = ad->type;
    node->atama_data.ad->value = strdup(ad->value);
    node->atama_data.ifade = ifade;
    return node;
}

ASTNode* createAST_ArrayTanimlama(Token* tip, Token* ad, ASTNode* boyut) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_TANIMLAMA;
    node->array_tanimlama_data.tip = (Token*)malloc(sizeof(Token));
    node->array_tanimlama_data.tip->type = tip->type;
    node->array_tanimlama_data.tip->value = strdup(tip->value);
    node->array_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->array_tanimlama_data.ad->type = ad->type;
    node->array_tanimlama_data.ad->value = strdup(ad->value);
    node->array_tanimlama_data.boyut = boyut;
    return node;
}

ASTNode* createAST_ArrayErisim(Token* ad, ASTNode* indeks) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_ERISIM;
    node->array_erisim_data.ad = (Token*)malloc(sizeof(Token));
    node->array_erisim_data.ad->type = ad->type;
    node->array_erisim_data.ad->value = strdup(ad->value);
    node->array_erisim_data.indeks = indeks;
    return node;
}

ASTNode* createAST_ArrayAtama(Token* ad, ASTNode* indeks, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_ATAMA;
    node->array_atama_data.ad = (Token*)malloc(sizeof(Token));
    node->array_atama_data.ad->type = ad->type;
    node->array_atama_data.ad->value = strdup(ad->value);
    node->array_atama_data.indeks = indeks;
    node->array_atama_data.deger = deger;
    return node;
}

// Array Literal ([1, 2, 3, 4, 5])
ASTNode* createAST_ArrayLiteral(ASTNode** elemanlar, int eleman_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_LITERAL;
    node->array_literal_data.elemanlar = elemanlar;
    node->array_literal_data.eleman_sayisi = eleman_sayisi;
    return node;
}

// Struct Tanımlama (YAPI Nokta İSE SAYISAL x; SAYISAL y; SON)
ASTNode* createAST_StructTanimlama(Token* ad, Token** field_tipleri, Token** field_adlari, int field_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_DECLARATION;
    node->struct_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->struct_tanimlama_data.ad->type = ad->type;
    node->struct_tanimlama_data.ad->value = strdup(ad->value);
    node->struct_tanimlama_data.field_tipleri = field_tipleri;
    node->struct_tanimlama_data.field_adlari = field_adlari;
    node->struct_tanimlama_data.field_sayisi = field_sayisi;
    return node;
}

// Struct Field Access (p.x)
ASTNode* createAST_StructFieldAccess(Token* struct_ad, Token* field_ad) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_FIELD_ACCESS;
    node->struct_field_access_data.struct_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_access_data.struct_ad->type = struct_ad->type;
    node->struct_field_access_data.struct_ad->value = strdup(struct_ad->value);
    node->struct_field_access_data.field_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_access_data.field_ad->type = field_ad->type;
    node->struct_field_access_data.field_ad->value = strdup(field_ad->value);
    return node;
}

// Struct Field Atama (p.x = 10;)
ASTNode* createAST_StructFieldAtama(Token* struct_ad, Token* field_ad, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_FIELD_ATAMA;
    node->struct_field_atama_data.struct_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_atama_data.struct_ad->type = struct_ad->type;
    node->struct_field_atama_data.struct_ad->value = strdup(struct_ad->value);
    node->struct_field_atama_data.field_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_atama_data.field_ad->type = field_ad->type;
    node->struct_field_atama_data.field_ad->value = strdup(field_ad->value);
    node->struct_field_atama_data.deger = deger;
    return node;
}

// Struct Değişken (Nokta p;)
ASTNode* createAST_StructDegisken(Token* struct_tip, Token* ad) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_VARIABLE;
    node->struct_degisken_data.struct_tip = (Token*)malloc(sizeof(Token));
    node->struct_degisken_data.struct_tip->type = struct_tip->type;
    node->struct_degisken_data.struct_tip->value = strdup(struct_tip->value);
    node->struct_degisken_data.ad = (Token*)malloc(sizeof(Token));
    node->struct_degisken_data.ad->type = ad->type;
    node->struct_degisken_data.ad->value = strdup(ad->value);
    return node;
}

// ===== Phase 2: List AST Creation Helpers =====

ASTNode* createAST_ListTanimlama(Token* element_tipi, Token* degisken_adi, ASTNode* capacity, ASTNode* initial_value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_TANIMLAMA;
    
    // Properly copy tokens to heap (matching ArrayTanimlama pattern)
    node->list_tanimlama_data.element_tipi = (Token*)malloc(sizeof(Token));
    node->list_tanimlama_data.element_tipi->type = element_tipi->type;
    node->list_tanimlama_data.element_tipi->value = strdup(element_tipi->value);
    
    node->list_tanimlama_data.degisken_adi = (Token*)malloc(sizeof(Token));
    node->list_tanimlama_data.degisken_adi->type = degisken_adi->type;
    node->list_tanimlama_data.degisken_adi->value = strdup(degisken_adi->value);
    
    node->list_tanimlama_data.capacity = capacity;           // Initial capacity or NULL
    node->list_tanimlama_data.initial_value = initial_value; // List literal or NULL
    return node;
}

// List Literal ((1, 2, 3, 4, 5))
ASTNode* createAST_ListLiteral(ASTNode** elemanlar, int eleman_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_LITERAL;
    node->list_literal_data.elemanlar = elemanlar;
    node->list_literal_data.eleman_sayisi = eleman_sayisi;
    return node;
}

// ===== Phase 6.2: Optional<T> Helper Functions =====

ASTNode* createAST_OptionalTanimlama(Token* element_tipi, Token* degisken_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_OPTIONAL_TANIMLAMA;
    node->optional_tanimlama_data.element_tipi = element_tipi;
    node->optional_tanimlama_data.degisken_adi = degisken_adi;
    return node;
}

ASTNode* createAST_OptionalHasValue(Token* optional_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_OPTIONAL_HAS_VALUE;
    node->optional_has_value_data.optional_adi = optional_adi;
    return node;
}

ASTNode* createAST_OptionalValue(Token* optional_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_OPTIONAL_VALUE;
    node->optional_value_data.optional_adi = optional_adi;
    return node;
}

ASTNode* createAST_OptionalValueOr(Token* optional_adi, ASTNode* default_deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_OPTIONAL_VALUE_OR;
    node->optional_value_or_data.optional_adi = optional_adi;
    node->optional_value_or_data.default_deger = default_deger;
    return node;
}

ASTNode* createAST_ListAdd(Token* list_adi, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_ADD;
    // AST must own its own copy to avoid dangling pointers
    node->list_add_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_add_data.list_adi->type = list_adi->type;
    node->list_add_data.list_adi->value = strdup(list_adi->value);  // Own copy
    node->list_add_data.deger = deger;
    return node;
}

ASTNode* createAST_ListGet(Token* list_adi, ASTNode* indeks) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_GET;
    // AST must own its own copy to avoid dangling pointers
    node->list_get_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_get_data.list_adi->type = list_adi->type;
    node->list_get_data.list_adi->value = strdup(list_adi->value);  // Own copy
    node->list_get_data.indeks = indeks;
    return node;
}

ASTNode* createAST_ListSize(Token* list_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_SIZE;
    // AST must own its own copy to avoid dangling pointers
    node->list_size_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_size_data.list_adi->type = list_adi->type;
    node->list_size_data.list_adi->value = strdup(list_adi->value);  // Own copy
    return node;
}

ASTNode* createAST_ListClear(Token* list_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_CLEAR;
    // AST must own its own copy to avoid dangling pointers
    node->list_clear_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_clear_data.list_adi->type = list_adi->type;
    node->list_clear_data.list_adi->value = strdup(list_adi->value);  // Own copy
    return node;
}

// Phase 6: New List methods
ASTNode* createAST_ListSet(Token* list_adi, ASTNode* indeks, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_SET;
    // AST must own its own copy to avoid dangling pointers
    node->list_set_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_set_data.list_adi->type = list_adi->type;
    node->list_set_data.list_adi->value = strdup(list_adi->value);  // Own copy
    node->list_set_data.indeks = indeks;
    node->list_set_data.deger = deger;
    return node;
}

ASTNode* createAST_ListRemove(Token* list_adi, ASTNode* indeks) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_REMOVE;
    // AST must own its own copy to avoid dangling pointers
    node->list_remove_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_remove_data.list_adi->type = list_adi->type;
    node->list_remove_data.list_adi->value = strdup(list_adi->value);  // Own copy
    node->list_remove_data.indeks = indeks;
    return node;
}

ASTNode* createAST_ListInsert(Token* list_adi, ASTNode* indeks, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_INSERT;
    // AST must own its own copy to avoid dangling pointers
    node->list_insert_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_insert_data.list_adi->type = list_adi->type;
    node->list_insert_data.list_adi->value = strdup(list_adi->value);  // Own copy
    node->list_insert_data.indeks = indeks;
    node->list_insert_data.deger = deger;
    return node;
}

// Phase 3: Built-in function call
ASTNode* createAST_BuiltinCall(TokenType func_type, ASTNode* arg1, ASTNode* arg2, ASTNode* arg3) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_BUILTIN_CALL;
    node->builtin_call_data.function_type = func_type;
    node->builtin_call_data.arg1 = arg1;
    node->builtin_call_data.arg2 = arg2;
    node->builtin_call_data.arg3 = arg3;
    return node;
}

// ===== Phase 4: Hash Map Helper Functions =====

ASTNode* createAST_MapTanimlama(Token* key_tipi, Token* value_tipi, Token* degisken_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_TANIMLAMA;
    node->map_tanimlama_data.key_tipi = (Token*)malloc(sizeof(Token));
    node->map_tanimlama_data.key_tipi->type = key_tipi->type;
    node->map_tanimlama_data.key_tipi->value = strdup(key_tipi->value);
    node->map_tanimlama_data.value_tipi = (Token*)malloc(sizeof(Token));
    node->map_tanimlama_data.value_tipi->type = value_tipi->type;
    node->map_tanimlama_data.value_tipi->value = strdup(value_tipi->value);
    node->map_tanimlama_data.degisken_adi = (Token*)malloc(sizeof(Token));
    node->map_tanimlama_data.degisken_adi->type = degisken_adi->type;
    node->map_tanimlama_data.degisken_adi->value = strdup(degisken_adi->value);
    return node;
}

ASTNode* createAST_MapSet(Token* map_adi, ASTNode* key, ASTNode* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_SET;
    node->map_set_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_set_data.map_adi->type = map_adi->type;
    node->map_set_data.map_adi->value = strdup(map_adi->value);
    node->map_set_data.key = key;
    node->map_set_data.value = value;
    return node;
}

ASTNode* createAST_MapGet(Token* map_adi, ASTNode* key) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_GET;
    node->map_get_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_get_data.map_adi->type = map_adi->type;
    node->map_get_data.map_adi->value = strdup(map_adi->value);
    node->map_get_data.key = key;
    return node;
}

ASTNode* createAST_MapHas(Token* map_adi, ASTNode* key) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_HAS;
    node->map_has_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_has_data.map_adi->type = map_adi->type;
    node->map_has_data.map_adi->value = strdup(map_adi->value);
    node->map_has_data.key = key;
    return node;
}

ASTNode* createAST_MapRemove(Token* map_adi, ASTNode* key) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_REMOVE;
    node->map_remove_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_remove_data.map_adi->type = map_adi->type;
    node->map_remove_data.map_adi->value = strdup(map_adi->value);
    node->map_remove_data.key = key;
    return node;
}

ASTNode* createAST_MapSize(Token* map_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_SIZE;
    node->map_size_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_size_data.map_adi->type = map_adi->type;
    node->map_size_data.map_adi->value = strdup(map_adi->value);
    return node;
}

ASTNode* createAST_MapClear(Token* map_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_CLEAR;
    node->map_clear_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_clear_data.map_adi->type = map_adi->type;
    node->map_clear_data.map_adi->value = strdup(map_adi->value);
    return node;
}

ASTNode* createAST_KosulKomutu(ASTNode* kosul, ASTNode* ise_blok, ASTNode* degilse_blok) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_IF_STATEMENT_KOMUTU;
    node->kosul_data.kosul = kosul;
    node->kosul_data.ise_blok = ise_blok;
    node->kosul_data.degilse_blok = degilse_blok;
    return node;
}

ASTNode* createAST_IslecTanimlama(Token* ad, Token** parametreler, int parametre_sayisi, int* parametre_is_array, ASTNode** parametre_defaults, ASTNode* govde) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_FUNCTION_DECLARATION;
    node->islec_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->islec_tanimlama_data.ad->type = ad->type;
    node->islec_tanimlama_data.ad->value = strdup(ad->value);
    node->islec_tanimlama_data.parametreler = parametreler;
    node->islec_tanimlama_data.parametre_sayisi = parametre_sayisi;
    node->islec_tanimlama_data.parametre_is_array = parametre_is_array; // Phase 7.6
    node->islec_tanimlama_data.parametre_default_degerleri = parametre_defaults; // Phase 5.11
    node->islec_tanimlama_data.govde = govde;
    return node;
}


// --- Ayrıştırma Fonksiyonları (İleri Bildirimler) ---
ASTNode* ifade();
ASTNode* birincil();
ASTNode* ikili_islem(int onceki_oncelik);
ASTNode* komut();
ASTNode* blok();
ASTNode* kosul_komutu();
ASTNode* dongu_komutu();
ASTNode* for_komutu();
ASTNode* islec_tanimlama();
ASTNode* async_function_tanimlama(); // Phase 8: Async function parser
ASTNode* donus_komutu();
ASTNode* list_tanimlama_parse();  // Phase 6: List<T> tanımlama parser
ASTNode* optional_tanimlama_parse();  // Phase 6.2: Optional<T> tanımlama parser


int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_QUESTION:  // Phase 5.8: Ternary operator (?:)
            return 2;
        case TOKEN_OR:
            return 3;
        case TOKEN_AND:
            return 4;
        case TOKEN_OP_ESIT_KARSILASTIRMA:
        case TOKEN_GT:
        case TOKEN_LT:
        case TOKEN_GTE:
        case TOKEN_LTE:
        case TOKEN_NOT_ESIT:
            return 5;
        case TOKEN_BITWISE_OR:
            return 6;
        case TOKEN_BITWISE_XOR:
            return 7;
        case TOKEN_BITWISE_AND:
            return 8;
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return 10;
        case TOKEN_LSHIFT:
        case TOKEN_RSHIFT:
            return 11;
        case TOKEN_MUL:
        case TOKEN_DIV:
        case TOKEN_MOD:
            return 20;
        default:
            return 0;
    }
}

ASTNode* ifade() {
    return ikili_islem(0);
}

ASTNode* birincil() {
    if (current_token == NULL) {
        parseError("Bir ifade expected.", "Sayı, Metin or Değişken");
    }

    // Phase 8: Await expression
    if (current_token->type == TOKEN_AWAIT) {
        consume(TOKEN_AWAIT);
        ASTNode* ifade_node = birincil(); // Parse awaited expression
        
        ASTNode* await_node = (ASTNode*)malloc(sizeof(ASTNode));
        await_node->type = AST_AWAIT_EXPR;
        await_node->await_data.ifade = ifade_node;
        
        return await_node;
    }

    if (current_token->type == TOKEN_NUMBER) {
        ASTNode* node = createAST_Sayi(current_token);
        consume(TOKEN_NUMBER);
        return node;
    }

    if (current_token->type == TOKEN_STRING) {
        ASTNode* node = createAST_Metin(current_token);
        consume(TOKEN_STRING);
        return node;
    }

    // Phase 5.9: Interpolated String
    if (current_token->type == TOKEN_INTERPOLATED_STRING) {
        ASTNode* node = createAST_InterpolatedString(current_token);
        consume(TOKEN_INTERPOLATED_STRING);
        return node;
    }

    // Boolean literals (true/false)
    if (current_token->type == TOKEN_TRUE) {
        Token* true_token = createToken(TOKEN_NUMBER, "1");
        ASTNode* node = createAST_Sayi(true_token);
        consume(TOKEN_TRUE);
        return node;
    }

    if (current_token->type == TOKEN_FALSE) {
        Token* false_token = createToken(TOKEN_NUMBER, "0");
        ASTNode* node = createAST_Sayi(false_token);
        consume(TOKEN_FALSE);
        return node;
    }

    // NULL literal (represented as 0 in MLP)
    if (current_token->type == TOKEN_NULL) {
        Token* null_token = createToken(TOKEN_NUMBER, "0");
        ASTNode* node = createAST_Sayi(null_token);
        consume(TOKEN_NULL);
        return node;
    }

    // Phase 7.1: Lambda expression
    if (current_token->type == TOKEN_LAMBDA) {
        consume(TOKEN_LAMBDA);
        consume(TOKEN_LEFT_PAREN);
        
        // Parse parameters
        Token** parametreler = NULL;
        int param_count = 0;
        
        if (current_token->type != TOKEN_RIGHT_PAREN) {
            parametreler = (Token**)malloc(sizeof(Token*) * 10);
            
            do {
                if (current_token->type != TOKEN_IDENTIFIER) {
                    parseError("Lambda parameter name", "IDENTIFIER");
                }
                
                parametreler[param_count] = (Token*)malloc(sizeof(Token));
                parametreler[param_count]->type = current_token->type;
                parametreler[param_count]->value = strdup(current_token->value);
                param_count++;
                
                consume(TOKEN_IDENTIFIER);
                
                if (current_token->type == TOKEN_COMMA) {
                    consume(TOKEN_COMMA);
                }
            } while (current_token->type == TOKEN_IDENTIFIER);
        }
        
        consume(TOKEN_RIGHT_PAREN);
        consume(TOKEN_ARROW);  // =>
        
        // Parse body (single expression for now)
        ASTNode* govde = ifade();
        
        // Phase 7.9: Don't detect captured vars here - do it in codegen
        // (Parser doesn't have scope information yet)
        
        // Create lambda node
        ASTNode* lambda = (ASTNode*)malloc(sizeof(ASTNode));
        lambda->type = AST_LAMBDA;
        lambda->lambda_data.parametreler = parametreler;
        lambda->lambda_data.parametre_sayisi = param_count;
        lambda->lambda_data.govde = govde;
        lambda->lambda_data.is_expression = true;
        lambda->lambda_data.captured_vars = NULL;  // Will be populated in codegen
        lambda->lambda_data.captured_count = 0;
        
        return lambda;
    }

    if (current_token->type == TOKEN_LEFT_PAREN) {
        consume(TOKEN_LEFT_PAREN);
        
        // Check if it's a list literal: (1, 2, 3)
        // Peek ahead to see if it's expression or list literal
        if (current_token->type == TOKEN_RIGHT_PAREN) {
            // Empty list literal: ()
            consume(TOKEN_RIGHT_PAREN);
            return createAST_ListLiteral(NULL, 0);
        }
        
        // Try to parse as list literal
        // If first element followed by comma, it's a list literal
        ASTNode* first = ifade();
        
        if (current_token->type == TOKEN_COMMA) {
            // It's a list literal: (expr, ...)
            int capacity = 10;
            ASTNode** elemanlar = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
            elemanlar[0] = first;
            int count = 1;
            
            while (current_token->type == TOKEN_COMMA) {
                consume(TOKEN_COMMA);
                if (current_token->type == TOKEN_RIGHT_PAREN) break; // Trailing comma
                
                if (count >= capacity) {
                    capacity *= 2;
                    elemanlar = (ASTNode**)realloc(elemanlar, sizeof(ASTNode*) * capacity);
                }
                elemanlar[count++] = ifade();
            }
            
            consume(TOKEN_RIGHT_PAREN);
            return createAST_ListLiteral(elemanlar, count);
        } else {
            // It's a parenthesized expression
            if (current_token->type != TOKEN_RIGHT_PAREN) {
                parseError("Kapanış Parantezi expected.", ")");
            }
            consume(TOKEN_RIGHT_PAREN);
            return first;
        }
    }

    // Array literal: [1, 2, 3, 4, 5]
    if (current_token->type == TOKEN_LEFT_BRACKET) {
        consume(TOKEN_LEFT_BRACKET);
        
        if (current_token->type == TOKEN_RIGHT_BRACKET) {
            // Empty array literal: []
            consume(TOKEN_RIGHT_BRACKET);
            return createAST_ArrayLiteral(NULL, 0);
        }
        
        int capacity = 10;
        ASTNode** elemanlar = (ASTNode**)malloc(sizeof(ASTNode*) * capacity);
        int count = 0;
        
        elemanlar[count++] = ifade();
        
        while (current_token->type == TOKEN_COMMA) {
            consume(TOKEN_COMMA);
            if (current_token->type == TOKEN_RIGHT_BRACKET) break; // Trailing comma
            
            if (count >= capacity) {
                capacity *= 2;
                elemanlar = (ASTNode**)realloc(elemanlar, sizeof(ASTNode*) * capacity);
            }
            elemanlar[count++] = ifade();
        }
        
        consume(TOKEN_RIGHT_BRACKET);
        return createAST_ArrayLiteral(elemanlar, count);
    }

    // Phase 5.7: typeof operator - typeof(expr)
    if (current_token->type == TOKEN_TYPEOF) {
        consume(TOKEN_TYPEOF);
        consume(TOKEN_LEFT_PAREN);
        ASTNode* expr = ifade();
        consume(TOKEN_RIGHT_PAREN);
        
        ASTNode* typeof_node = (ASTNode*)malloc(sizeof(ASTNode));
        typeof_node->type = AST_TYPEOF_EXPR;
        typeof_node->typeof_data.ifade = expr;
        return typeof_node;
    }

    // Phase 3: Built-in function calls
    if (current_token->type == TOKEN_BUILTIN_READ_FILE ||
        current_token->type == TOKEN_BUILTIN_WRITE_FILE ||
        current_token->type == TOKEN_BUILTIN_APPEND_FILE ||
        current_token->type == TOKEN_BUILTIN_FILE_EXISTS ||
        current_token->type == TOKEN_BUILTIN_FILE_SIZE ||
        current_token->type == TOKEN_BUILTIN_READ_LINES ||
        current_token->type == TOKEN_BUILTIN_STRING_SPLIT ||
        current_token->type == TOKEN_BUILTIN_STRING_JOIN ||
        current_token->type == TOKEN_BUILTIN_STRING_REPLACE ||
        current_token->type == TOKEN_BUILTIN_STRING_TRIM ||
        current_token->type == TOKEN_BUILTIN_STRING_UPPER ||
        current_token->type == TOKEN_BUILTIN_STRING_LOWER ||
        current_token->type == TOKEN_BUILTIN_STRING_FIND ||
        current_token->type == TOKEN_BUILTIN_STRING_STARTS_WITH ||
        current_token->type == TOKEN_BUILTIN_STRING_ENDS_WITH ||
        // Phase 4: Type Conversions
        current_token->type == TOKEN_BUILTIN_INT_TO_STRING ||
        current_token->type == TOKEN_BUILTIN_STRING_TO_INT ||
        current_token->type == TOKEN_BUILTIN_CHAR_TO_STRING ||
        current_token->type == TOKEN_BUILTIN_STRING_CONCAT ||
        // Phase 5.7: Type Casting Helpers
        current_token->type == TOKEN_BUILTIN_NUM ||
        current_token->type == TOKEN_BUILTIN_STR ||
        current_token->type == TOKEN_BUILTIN_READ_INPUT ||
        current_token->type == TOKEN_BUILTIN_READ_LINE ||
        current_token->type == TOKEN_BUILTIN_READ_INT ||
        // Phase 4: Math Operations
        current_token->type == TOKEN_BUILTIN_MATH_ABS ||
        current_token->type == TOKEN_BUILTIN_MATH_MIN ||
        current_token->type == TOKEN_BUILTIN_MATH_MAX ||
        current_token->type == TOKEN_BUILTIN_MATH_POW ||
        // Phase 5.1: Enhanced String Operations
        current_token->type == TOKEN_BUILTIN_STRING_LENGTH ||
        current_token->type == TOKEN_BUILTIN_STRING_SUBSTRING ||
        current_token->type == TOKEN_BUILTIN_STRING_INDEX_OF ||
        current_token->type == TOKEN_BUILTIN_STRING_LAST_INDEX_OF ||
        // Phase 5.6: Modern String API Wrappers
        current_token->type == TOKEN_BUILTIN_LEN ||
        current_token->type == TOKEN_BUILTIN_ORD ||
        current_token->type == TOKEN_BUILTIN_CHR ||
        current_token->type == TOKEN_BUILTIN_CHAR_CODE ||
        current_token->type == TOKEN_BUILTIN_CHAR_AT ||
        current_token->type == TOKEN_BUILTIN_SUBSTRING ||
        current_token->type == TOKEN_BUILTIN_CONTAINS ||
        current_token->type == TOKEN_BUILTIN_STARTSWITH ||
        current_token->type == TOKEN_BUILTIN_ENDSWITH ||
        current_token->type == TOKEN_BUILTIN_LEFT ||
        current_token->type == TOKEN_BUILTIN_RIGHT ||
        current_token->type == TOKEN_BUILTIN_MID ||
        // Phase 5.2: Error Handling
        current_token->type == TOKEN_BUILTIN_EXIT_WITH_CODE ||
        current_token->type == TOKEN_BUILTIN_PANIC ||
        current_token->type == TOKEN_BUILTIN_ASSERT ||
        current_token->type == TOKEN_BUILTIN_GET_ERROR_CODE ||
        current_token->type == TOKEN_BUILTIN_SET_ERROR_CODE ||
        // Phase 5.2: Memory Management
        current_token->type == TOKEN_BUILTIN_MLP_MALLOC ||
        current_token->type == TOKEN_BUILTIN_MLP_FREE ||
        current_token->type == TOKEN_BUILTIN_MLP_REALLOC ||
        current_token->type == TOKEN_BUILTIN_MLP_CALLOC ||
        current_token->type == TOKEN_BUILTIN_GET_ALLOCATED_BYTES ||
        current_token->type == TOKEN_BUILTIN_CHECK_MEMORY_LEAKS ||
        // Phase 5.2: System Utilities
        current_token->type == TOKEN_BUILTIN_GET_ENV ||
        current_token->type == TOKEN_BUILTIN_CURRENT_TIMESTAMP ||
        current_token->type == TOKEN_BUILTIN_SLEEP_MS ||
        // Phase 5.3: Binary File I/O
        current_token->type == TOKEN_BUILTIN_READ_BINARY ||
        current_token->type == TOKEN_BUILTIN_WRITE_BINARY ||
        current_token->type == TOKEN_BUILTIN_GET_FILE_INFO ||
        current_token->type == TOKEN_BUILTIN_COPY_FILE ||
        // Phase 5.3: Directory Operations
        current_token->type == TOKEN_BUILTIN_LIST_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_CREATE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_REMOVE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_DIRECTORY_EXISTS ||
        current_token->type == TOKEN_BUILTIN_GET_CURRENT_DIR ||
        current_token->type == TOKEN_BUILTIN_CHANGE_DIRECTORY ||
        // Phase 5.3: Process Control
        current_token->type == TOKEN_BUILTIN_EXECUTE_COMMAND ||
        current_token->type == TOKEN_BUILTIN_GET_COMMAND_OUTPUT ||
        current_token->type == TOKEN_BUILTIN_GET_PROCESS_ID ||
        current_token->type == TOKEN_BUILTIN_GET_PARENT_PROCESS_ID ||
        // Phase 5.3: Time & Date Utilities
        current_token->type == TOKEN_BUILTIN_FORMAT_TIMESTAMP ||
        current_token->type == TOKEN_BUILTIN_PARSE_TIMESTAMP ||
        current_token->type == TOKEN_BUILTIN_GET_MILLISECONDS ||
        current_token->type == TOKEN_BUILTIN_GET_TIME_STRING ||
        // Phase 5.3: Path Utilities
        current_token->type == TOKEN_BUILTIN_JOIN_PATH ||
        current_token->type == TOKEN_BUILTIN_GET_FILE_EXTENSION ||
        current_token->type == TOKEN_BUILTIN_GET_FILE_NAME ||
        current_token->type == TOKEN_BUILTIN_GET_DIRECTORY) {

        TokenType func_type = current_token->type;
        consume(current_token->type);

        consume(TOKEN_LEFT_PAREN);

        // Parse arguments
        ASTNode* arg1 = NULL;
        ASTNode* arg2 = NULL;
        ASTNode* arg3 = NULL;

        if (current_token->type != TOKEN_RIGHT_PAREN) {
            arg1 = ifade();

            if (current_token->type == TOKEN_COMMA) {
                consume(TOKEN_COMMA);
                arg2 = ifade();

                if (current_token->type == TOKEN_COMMA) {
                    consume(TOKEN_COMMA);
                    arg3 = ifade();
                }
            }
        }

        consume(TOKEN_RIGHT_PAREN);

        return createAST_BuiltinCall(func_type, arg1, arg2, arg3);
    }

    if (current_token->type == TOKEN_IDENTIFIER) {
        Token ad_token_kopya;
        ad_token_kopya.type = current_token->type;
        ad_token_kopya.value = strdup(current_token->value);  // Must strdup - lexer will reuse buffer
        consume(TOKEN_IDENTIFIER);

        // Fonksiyon çağrısı mı? (func(args))
        if (current_token->type == TOKEN_LEFT_PAREN) {
            consume(TOKEN_LEFT_PAREN);
            ASTNode** arguman_listesi = NULL;
            int a_sayisi = 0;
            if (current_token->type != TOKEN_RIGHT_PAREN) {
                arguman_listesi = (ASTNode**)malloc(sizeof(ASTNode*) * 10);
                do {
                    arguman_listesi[a_sayisi] = ifade();
                    a_sayisi++;
                } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
            }
            consume(TOKEN_RIGHT_PAREN);
            ASTNode* call_node = createAST_IslecCagirma(&ad_token_kopya, arguman_listesi, a_sayisi);
            free(ad_token_kopya.value);  // Free strdup'd string
            return call_node;
        }
        // Array erişimi mi? (arr[index])
        else if (current_token->type == TOKEN_LEFT_BRACKET) {
            consume(TOKEN_LEFT_BRACKET);
            ASTNode* indeks = ifade();
            consume(TOKEN_RIGHT_BRACKET);
            ASTNode* array_erisim = createAST_ArrayErisim(&ad_token_kopya, indeks);
            free(ad_token_kopya.value);  // Free strdup'd string
            return array_erisim;
        }
        // Struct field access or List method call? (p.x or list.add())
        else if (current_token->type == TOKEN_DOT) {
            consume(TOKEN_DOT);
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Field/method name", "IDENTIFIER");
            }
            Token field_or_method;
            field_or_method.type = current_token->type;
            field_or_method.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);

            // Check if this is a list method call (has parentheses)
            if (current_token->type == TOKEN_LEFT_PAREN) {
                consume(TOKEN_LEFT_PAREN);

                // Phase 6: List methods - add, get, set, remove, insert, length, clear
                if (strcmp(field_or_method.value, "add") == 0) {
                    ASTNode* deger = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* add_node = createAST_ListAdd(&ad_token_kopya, deger);
                    free(ad_token_kopya.value);  // AST has its own copy, safe to free
                    free(field_or_method.value);
                    return add_node;
                }
                else if (strcmp(field_or_method.value, "get") == 0) {
                    // Check variable type to determine if it's list or map
                    const char* var_type = get_parse_var_type(ad_token_kopya.value);
                    
                    if (var_type && strcmp(var_type, "HashMap") == 0) {
                        // map.get(key)
                        ASTNode* key = ifade();
                        consume(TOKEN_RIGHT_PAREN);
                        
                        ASTNode* get_node = createAST_MapGet(&ad_token_kopya, key);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return get_node;
                    } else {
                        // list.get(index)
                        ASTNode* indeks = ifade();
                        consume(TOKEN_RIGHT_PAREN);
                        
                        ASTNode* get_node = createAST_ListGet(&ad_token_kopya, indeks);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return get_node;
                    }
                }
                else if (strcmp(field_or_method.value, "set") == 0) {
                    // Check variable type to determine if it's list or map
                    const char* var_type = get_parse_var_type(ad_token_kopya.value);
                    
                    if (var_type && strcmp(var_type, "HashMap") == 0) {
                        // map.set(key, value)
                        ASTNode* key = ifade();
                        consume(TOKEN_COMMA);
                        ASTNode* value = ifade();
                        consume(TOKEN_RIGHT_PAREN);
                        
                        ASTNode* set_node = createAST_MapSet(&ad_token_kopya, key, value);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return set_node;
                    } else {
                        // list.set(index, value)
                        ASTNode* indeks = ifade();
                        consume(TOKEN_COMMA);
                        ASTNode* value = ifade();
                        consume(TOKEN_RIGHT_PAREN);
                        
                        ASTNode* set_node = createAST_ListSet(&ad_token_kopya, indeks, value);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return set_node;
                    }
                }
                else if (strcmp(field_or_method.value, "remove") == 0) {
                    // Check variable type
                    const char* var_type = get_parse_var_type(ad_token_kopya.value);
                    
                    if (var_type && strcmp(var_type, "HashMap") == 0) {
                        // map.remove(key)
                        ASTNode* key = ifade();
                        consume(TOKEN_RIGHT_PAREN);
                        
                        ASTNode* remove_node = createAST_MapRemove(&ad_token_kopya, key);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return remove_node;
                    } else {
                        // list.remove(index)
                        ASTNode* indeks = ifade();
                        consume(TOKEN_RIGHT_PAREN);
                        
                        ASTNode* remove_node = createAST_ListRemove(&ad_token_kopya, indeks);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return remove_node;
                    }
                }
                else if (strcmp(field_or_method.value, "insert") == 0) {
                    // list.insert(index, value)
                    ASTNode* indeks = ifade();
                    consume(TOKEN_COMMA);
                    ASTNode* value = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    
                    ASTNode* insert_node = createAST_ListInsert(&ad_token_kopya, indeks, value);
                    free(ad_token_kopya.value);  // AST has its own copy, safe to free
                    free(field_or_method.value);
                    return insert_node;
                }
                else if (strcmp(field_or_method.value, "length") == 0 || 
                         strcmp(field_or_method.value, "size") == 0) {
                    consume(TOKEN_RIGHT_PAREN);
                    
                    // Check variable type
                    const char* var_type = get_parse_var_type(ad_token_kopya.value);
                    
                    if (var_type && strcmp(var_type, "HashMap") == 0) {
                        // map.size()
                        ASTNode* size_node = createAST_MapSize(&ad_token_kopya);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return size_node;
                    } else {
                        // list.size()
                        ASTNode* size_node = createAST_ListSize(&ad_token_kopya);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return size_node;
                    }
                }
                else if (strcmp(field_or_method.value, "clear") == 0) {
                    consume(TOKEN_RIGHT_PAREN);
                    
                    // Check variable type
                    const char* var_type = get_parse_var_type(ad_token_kopya.value);
                    
                    if (var_type && strcmp(var_type, "HashMap") == 0) {
                        // map.clear()
                        ASTNode* clear_node = createAST_MapClear(&ad_token_kopya);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return clear_node;
                    } else {
                        // list.clear()
                        ASTNode* clear_node = createAST_ListClear(&ad_token_kopya);
                        free(ad_token_kopya.value);
                        free(field_or_method.value);
                        return clear_node;
                    }
                }
                // ===== Phase 4: Map Methods =====
                else if (strcmp(field_or_method.value, "has") == 0) {
                    // map.has(key)
                    ASTNode* key = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* has_node = createAST_MapHas(&ad_token_kopya, key);
                    free(ad_token_kopya.value);  // Free strdup'd string
                    free(field_or_method.value);
                    return has_node;
                }
                else {
                    fprintf(stderr, "Unknown list/map method: %s\n", field_or_method.value);
                    parseError("List/Map method", "add/get/set/remove/insert/length/size/clear/has");
                }
            }
            else {
                // Regular struct field access (no parentheses)
                ASTNode* field_access = createAST_StructFieldAccess(&ad_token_kopya, &field_or_method);
                free(ad_token_kopya.value);  // Free strdup'd string
                free(field_or_method.value);
                return field_access;
            }
        }
        // Normal değişken
        else {
            ASTNode* var_node = createAST_Degisken(&ad_token_kopya);
            free(ad_token_kopya.value);  // Free strdup'd string
            return var_node;
        }
    }

    parseError("Birincil ifade expected.", "Sayı, Parantez or Identifier");
    return NULL;
}

// Unary expressions (not, unary minus)
ASTNode* unary_ifade() {
    // Handle 'not' keyword (logical NOT)
    if (current_token->type == TOKEN_NOT) {
        consume(TOKEN_NOT);
        ASTNode* operand = unary_ifade();  // Recursive for multiple nots
        
        // Create a binary operation node: operand == 0
        // This makes 'not x' equivalent to 'x == 0'
        ASTNode* zero_node = (ASTNode*)malloc(sizeof(ASTNode));
        zero_node->type = AST_SAYI;
        zero_node->sabit_data.deger = strdup("0");
        
        ASTNode* comparison = (ASTNode*)malloc(sizeof(ASTNode));
        comparison->type = AST_IKILI_ISLEM;
        comparison->ikili_islem_data.sol = operand;
        comparison->ikili_islem_data.sag = zero_node;
        comparison->ikili_islem_data.operator_type = TOKEN_OP_ESIT_KARSILASTIRMA;
        
        return comparison;
    }
    
    // Phase 5.5: Handle bitwise NOT (~)
    if (current_token->type == TOKEN_BITWISE_NOT) {
        consume(TOKEN_BITWISE_NOT);
        ASTNode* operand = unary_ifade();  // Recursive
        
        // Create unary NOT operation
        // We'll encode it as -1 XOR operand (which is bitwise NOT)
        ASTNode* minus_one = (ASTNode*)malloc(sizeof(ASTNode));
        minus_one->type = AST_SAYI;
        minus_one->sabit_data.deger = strdup("-1");
        
        ASTNode* not_node = (ASTNode*)malloc(sizeof(ASTNode));
        not_node->type = AST_IKILI_ISLEM;
        not_node->ikili_islem_data.sol = operand;
        not_node->ikili_islem_data.sag = minus_one;
        not_node->ikili_islem_data.operator_type = TOKEN_BITWISE_XOR;
        
        return not_node;
    }
    
    // Otherwise, parse primary expression
    return birincil();
}

ASTNode* ikili_islem(int onceki_oncelik) {
    ASTNode* sol = unary_ifade();  // Changed from birincil() to unary_ifade()
    if (sol == NULL) return NULL;

    while (1) {
        TokenType op_type = current_token->type;
        int oncelik = get_precedence(op_type);
        if (oncelik <= onceki_oncelik) {
            break;
        }

        // Phase 5.8: Ternary operator (condition ? true_val : false_val)
        if (op_type == TOKEN_QUESTION) {
            consume(TOKEN_QUESTION);
            ASTNode* dogru_deger = ifade();  // True value
            consume(TOKEN_COLON);            // :
            ASTNode* yanlis_deger = ikili_islem(oncelik);  // False value

            ASTNode* ternary_node = (ASTNode*)malloc(sizeof(ASTNode));
            if (ternary_node == NULL) return NULL;
            ternary_node->type = AST_TERNARY;
            ternary_node->ternary_data.kosul = sol;
            ternary_node->ternary_data.dogru_deger = dogru_deger;
            ternary_node->ternary_data.yanlis_deger = yanlis_deger;
            sol = ternary_node;
            continue;
        }

        consume(op_type);
        ASTNode* sag = ikili_islem(oncelik);
        ASTNode* yeni_node = (ASTNode*)malloc(sizeof(ASTNode));
        if (yeni_node == NULL) return NULL;
        yeni_node->type = AST_IKILI_ISLEM;
        yeni_node->ikili_islem_data.sol = sol;
        yeni_node->ikili_islem_data.sag = sag;
        yeni_node->ikili_islem_data.operator_type = op_type;
        sol = yeni_node;
    }
    return sol;
}

ASTNode* komut() {
    // Phase 8: Async Function (MUST be before IDENTIFIER check)
    if (current_token->type == TOKEN_ASYNC) {
        return async_function_tanimlama();
    }
    
    // Phase 5.10: Import Statement (import "file.mlp")
    if (current_token->type == TOKEN_IMPORT) {
        consume(TOKEN_IMPORT);
        
        // Dosya yolu string literal olmalı
        if (current_token->type != TOKEN_STRING) {
            parseError("Import dosya yolu", "STRING");
        }
        
        char* file_path = strdup(current_token->value);
        consume(TOKEN_STRING);
        
        ASTNode* import_node = (ASTNode*)malloc(sizeof(ASTNode));
        import_node->type = AST_IMPORT;
        import_node->import_data.file_path = file_path;
        
        return import_node;
    }
    
    // 0. YAPI - Struct Tanımlama (YAPI Nokta İSE ... SON)
    if (current_token->type == TOKEN_YAPI_STRUCT) {
        consume(TOKEN_YAPI_STRUCT);

        // Struct adı
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Struct adı", "IDENTIFIER");
        }
        Token struct_ad;
        struct_ad.type = current_token->type;
        struct_ad.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Field'ları parse et (then keyword kaldırıldı)
        Token** field_tipleri = (Token**)malloc(sizeof(Token*) * 20);  // Max 20 field
        Token** field_adlari = (Token**)malloc(sizeof(Token*) * 20);
        int field_sayisi = 0;

        // SON'a kadar field'ları oku
        while (current_token->type != TOKEN_END) {
            // Field tipi (SAYISAL, METIN, vb.)
            if (current_token->type != TOKEN_TYPE_NUMERIC &&
                current_token->type != TOKEN_TYPE_STRING &&
                current_token->type != TOKEN_TYPE_BOOLEAN) {
                parseError("Field tipi", "SAYISAL/METIN/BOOL");
            }

            Token* field_tip = (Token*)malloc(sizeof(Token));
            field_tip->type = current_token->type;
            field_tip->value = strdup(current_token->value);
            field_tipleri[field_sayisi] = field_tip;
            consume(current_token->type);

            // Field adı
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Field adı", "IDENTIFIER");
            }
            Token* field_ad = (Token*)malloc(sizeof(Token));
            field_ad->type = current_token->type;
            field_ad->value = strdup(current_token->value);
            field_adlari[field_sayisi] = field_ad;
            consume(TOKEN_IDENTIFIER);

            // Semicolon kaldırıldı - Python tarzı
            // consume(TOKEN_SEMICOLON);

            field_sayisi++;
        }

        // Accept both "end struct" and plain "end"
        if (current_token->type == TOKEN_END_STRUCT) {
            consume(TOKEN_END_STRUCT);
        } else {
            consume(TOKEN_END);
        }

        ASTNode* struct_node = createAST_StructTanimlama(&struct_ad, field_tipleri, field_adlari, field_sayisi);
        free(struct_ad.value);
        return struct_node;
    }

    // Phase 5.4: Enum Tanımlama (enum Color then RED = 0 GREEN = 1 end)
    if (current_token->type == TOKEN_YAPI_ENUM) {
        consume(TOKEN_YAPI_ENUM);

        // Enum adı
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Enum adı", "IDENTIFIER");
        }
        Token* enum_ad = (Token*)malloc(sizeof(Token));
        enum_ad->type = current_token->type;
        enum_ad->value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Değerleri parse et (then keyword kaldırıldı)
        Token** value_adlari = (Token**)malloc(sizeof(Token*) * 100);  // Max 100 enum value
        int* value_degerleri = (int*)malloc(sizeof(int) * 100);
        int value_sayisi = 0;
        int next_value = 0;

        // end'e kadar değerleri oku
        while (current_token->type != TOKEN_END) {
            // Değer adı
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Enum değer adı", "IDENTIFIER");
            }
            Token* value_ad = (Token*)malloc(sizeof(Token));
            value_ad->type = current_token->type;
            value_ad->value = strdup(current_token->value);
            value_adlari[value_sayisi] = value_ad;
            consume(TOKEN_IDENTIFIER);

            // Opsiyonel: = değer
            if (current_token->type == TOKEN_ASSIGN) {
                consume(TOKEN_ASSIGN);
                if (current_token->type != TOKEN_NUMBER) {
                    parseError("Enum değeri", "SAYI");
                }
                next_value = atoi(current_token->value);
                consume(TOKEN_NUMBER);
            }
            value_degerleri[value_sayisi] = next_value;
            next_value++;
            value_sayisi++;
        }

        // Accept both "end enum" and plain "end"
        if (current_token->type == TOKEN_END_ENUM) {
            consume(TOKEN_END_ENUM);
        } else {
            consume(TOKEN_END);
        }

        // AST node oluştur
        ASTNode* enum_node = (ASTNode*)malloc(sizeof(ASTNode));
        enum_node->type = AST_ENUM_DECLARATION;
        enum_node->enum_tanimlama_data.ad = enum_ad;
        enum_node->enum_tanimlama_data.value_adlari = value_adlari;
        enum_node->enum_tanimlama_data.value_degerleri = value_degerleri;
        enum_node->enum_tanimlama_data.value_sayisi = value_sayisi;
        return enum_node;
    }

    // Phase 5.4: Switch Statement (switch expr then case ... end)
    if (current_token->type == TOKEN_YAPI_SWITCH) {
        consume(TOKEN_YAPI_SWITCH);

        // Switch ifadesi
        ASTNode* switch_ifade = ifade();

        // then
        consume(TOKEN_THEN);

        // Case'leri parse et
        ASTNode** cases = (ASTNode**)malloc(sizeof(ASTNode*) * 100);  // Max 100 case
        int case_sayisi = 0;
        ASTNode* default_blok = NULL;

        // end'e kadar case'leri oku
        while (current_token->type != TOKEN_END) {
            if (current_token->type == TOKEN_YAPI_CASE) {
                consume(TOKEN_YAPI_CASE);

                // Case değeri
                ASTNode* case_deger = ifade();

                // then
                consume(TOKEN_THEN);

                // Case bloğu
                ASTNode** case_komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
                int case_komut_sayisi = 0;

                while (current_token->type != TOKEN_END &&
                       current_token->type != TOKEN_YAPI_CASE &&
                       current_token->type != TOKEN_YAPI_DEFAULT) {
                    case_komutlar[case_komut_sayisi++] = komut();
                }

                // end (case bloğu için)
                if (current_token->type == TOKEN_END) {
                    consume(TOKEN_END);
                }

                // Case node oluştur
                ASTNode* case_blok = (ASTNode*)malloc(sizeof(ASTNode));
                case_blok->type = AST_BLOK;
                case_blok->blok_data.komutlar = case_komutlar;
                case_blok->blok_data.sayisi = case_komut_sayisi;

                ASTNode* case_node = (ASTNode*)malloc(sizeof(ASTNode));
                case_node->type = AST_CASE_KOMUTU;
                case_node->case_data.deger = case_deger;
                case_node->case_data.blok = case_blok;
                cases[case_sayisi++] = case_node;

            } else if (current_token->type == TOKEN_YAPI_DEFAULT) {
                consume(TOKEN_YAPI_DEFAULT);

                // then
                consume(TOKEN_THEN);

                // Default bloğu
                ASTNode** default_komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
                int default_komut_sayisi = 0;

                while (current_token->type != TOKEN_END) {
                    default_komutlar[default_komut_sayisi++] = komut();
                }

                // case/default iç bloklardan sade end yeterli
                consume(TOKEN_END);

                default_blok = (ASTNode*)malloc(sizeof(ASTNode));
                default_blok->type = AST_BLOK;
                default_blok->blok_data.komutlar = default_komutlar;
                default_blok->blok_data.sayisi = default_komut_sayisi;
            } else {
                parseError("case or default", "CASE/DEFAULT");
            }
        }

        // Accept both "end switch" and plain "end"
        if (current_token->type == TOKEN_END_SWITCH) {
            consume(TOKEN_END_SWITCH);
        } else {
            consume(TOKEN_END);
        }

        // Switch node oluştur
        ASTNode* switch_node = (ASTNode*)malloc(sizeof(ASTNode));
        switch_node->type = AST_SWITCH_KOMUTU;
        switch_node->switch_data.ifade = switch_ifade;
        switch_node->switch_data.cases = cases;
        switch_node->switch_data.case_sayisi = case_sayisi;
        switch_node->switch_data.default_blok = default_blok;
        return switch_node;
    }

    // Struct Değişken Tanımlama (Person p;)
    // Pattern: IDENTIFIER IDENTIFIER SEMICOLON
    if (current_token->type == TOKEN_IDENTIFIER) {
        Token* peeked = peekNextToken();
        int is_struct_var = (peeked && peeked->type == TOKEN_IDENTIFIER);
        // Free peeked token to avoid memory leak
        if (peeked) {
            if (peeked->value) free(peeked->value);
            free(peeked);
        }
        if (is_struct_var) {
            // This is: StructName varName;
            // Save struct type name
            Token struct_tip;
            struct_tip.type = current_token->type;
            struct_tip.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);

            // Get variable name
            Token ad;
            ad.type = current_token->type;
            ad.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);

            // Semicolon kaldırıldı - Python tarzı
            // consume(TOKEN_SEMICOLON);

            ASTNode* struct_var_node = createAST_StructDegisken(&struct_tip, &ad);
            free(struct_tip.value);
            free(ad.value);
            return struct_var_node;
        }
    }

    // ===== Phase 4: Map Tanımlama (map[KeyType:ValueType] var = map();) =====
    if (current_token->type == TOKEN_MAP) {
        consume(TOKEN_MAP);

        // Expect [
        consume(TOKEN_LEFT_BRACKET);

        // Key type
        Token key_tip;
        if (current_token->type == TOKEN_TYPE_NUMERIC ||
            current_token->type == TOKEN_TYPE_STRING ||
            current_token->type == TOKEN_IDENTIFIER) {
            key_tip.type = current_token->type;
            key_tip.value = strdup(current_token->value);
            consume(current_token->type);
        } else {
            parseError("Map key type", "int/string");
        }

        // Expect :
        consume(TOKEN_COLON);

        // Value type
        Token value_tip;
        if (current_token->type == TOKEN_TYPE_NUMERIC ||
            current_token->type == TOKEN_TYPE_STRING ||
            current_token->type == TOKEN_IDENTIFIER) {
            value_tip.type = current_token->type;
            value_tip.value = strdup(current_token->value);
            consume(current_token->type);
        } else {
            parseError("Map value type", "int/string");
        }

        // Expect ]
        consume(TOKEN_RIGHT_BRACKET);

        // Variable name
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Map variable name", "IDENTIFIER");
        }
        Token degisken_adi;
        degisken_adi.type = current_token->type;
        degisken_adi.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Expect =
        consume(TOKEN_ASSIGN);

        // Expect map()
        if (current_token->type != TOKEN_MAP) {
            parseError("map()", "map");
        }
        consume(TOKEN_MAP);
        consume(TOKEN_LEFT_PAREN);
        consume(TOKEN_RIGHT_PAREN);

        // Semicolon kaldırıldı - Python tarzı
        // consume(TOKEN_SEMICOLON);

        // Register map variable for parser-level type tracking
        register_parse_var(degisken_adi.value, "HashMap");

        ASTNode* map_node = createAST_MapTanimlama(&key_tip, &value_tip, &degisken_adi);
        free(key_tip.value);
        free(value_tip.value);
        free(degisken_adi.value);
        return map_node;
    }

    // 1. PRINT (Noktalı virgülsüz)
    if (current_token->type == TOKEN_PRINT) {
        consume(TOKEN_PRINT);
        ASTNode* ifade_dugumu = ifade();
        specs_check_no_semicolon("PRINT");
        ASTNode* yazdir_node = (ASTNode*)malloc(sizeof(ASTNode));
        yazdir_node->type = AST_PRINT_STATEMENT;
        yazdir_node->tek_ifade_data.ifade = ifade_dugumu;
        return yazdir_node;
    }

    // 3. Const Değişken Tanımlama (Phase 5.6: const int x = 10;)
    if (current_token->type == TOKEN_CONST) {
        consume(TOKEN_CONST);
        
        if (current_token->type != TOKEN_TYPE_NUMERIC &&
            current_token->type != TOKEN_TYPE_STRING &&
            current_token->type != TOKEN_TYPE_BOOLEAN) {
            parseError("Type belirteci (int, string, bool)", current_token->value);
        }
        
        Token tip_token;
        tip_token.type = current_token->type;
        tip_token.value = strdup(current_token->value);
        consume(current_token->type);

        Token ad_token;
        if (current_token->type != TOKEN_IDENTIFIER) parseError("Variable name", "IDENTIFIER");
        ad_token.type = current_token->type;
        ad_token.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        consume(TOKEN_ASSIGN);
        ASTNode* ifade_dugumu = ifade();
        // Semicolon kaldırıldı - Python tarzı
        // consume(TOKEN_SEMICOLON);

        ASTNode* tanimlama_node = createAST_DegiskenTanimlama(&tip_token, &ad_token, ifade_dugumu);
        tanimlama_node->tanimlama_data.is_const = true; // Mark as const
        free(tip_token.value);
        free(ad_token.value);
        return tanimlama_node;
    }

    // 4. Değişken Tanımlama (Noktalı virgüllü)
    // 2. Variable Declaration (Python-style: anywhere in function body)
    if (current_token->type == TOKEN_TYPE_NUMERIC ||
        current_token->type == TOKEN_TYPE_STRING ||
        current_token->type == TOKEN_TYPE_BOOLEAN ||
        current_token->type == TOKEN_LIST ||      // Phase 6: List definition
        current_token->type == TOKEN_OPTIONAL)    // Phase 6.2: Optional definition
    {
        // Phase 6: List tanımlama
        if (current_token->type == TOKEN_LIST) {
            return list_tanimlama_parse();  // Call list_tanimlama_parse() which handles List<T> syntax
        }

        // Phase 6.2: Optional tanımlama
        if (current_token->type == TOKEN_OPTIONAL) {
            return optional_tanimlama_parse();  // Call optional_tanimlama_parse() which handles Optional<T> syntax
        }
        
        Token tip_token;
        tip_token.type = current_token->type;
        tip_token.value = strdup(current_token->value);
        consume(current_token->type);

        Token ad_token;
        if (current_token->type != TOKEN_IDENTIFIER) parseError("Variable name", "IDENTIFIER");
        ad_token.type = current_token->type;
        ad_token.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Array tanımlaması mı? (numeric arr[10] veya numeric arr[] = [1,2,3])
        if (current_token->type == TOKEN_LEFT_BRACKET) {
            consume(TOKEN_LEFT_BRACKET);
            
            // Check for array literal initialization: numeric arr[] = [...]
            if (current_token->type == TOKEN_RIGHT_BRACKET) {
                consume(TOKEN_RIGHT_BRACKET);
                consume(TOKEN_ASSIGN);
                ASTNode* literal = ifade();  // Should be array literal [1,2,3]
                
                // Create array declaration with size from literal
                ASTNode* array_node = createAST_ArrayTanimlama(&tip_token, &ad_token, literal);
                free(tip_token.value);
                free(ad_token.value);
                return array_node;
            }
            
            // Regular array with size: numeric arr[10]
            ASTNode* boyut = ifade();  // Array boyutu
            consume(TOKEN_RIGHT_BRACKET);

            ASTNode* array_node = createAST_ArrayTanimlama(&tip_token, &ad_token, boyut);
            free(tip_token.value);
            free(ad_token.value);
            return array_node;
        }

        // List tanımlaması mı? (numeric items() or numeric items(10) or numeric items() = (1,2,3))
        if (current_token->type == TOKEN_LEFT_PAREN) {
            consume(TOKEN_LEFT_PAREN);
            
            ASTNode* capacity_node = NULL;
            ASTNode* literal_node = NULL;
            
            // Check for capacity: numeric items(10)
            if (current_token->type == TOKEN_NUMBER) {
                capacity_node = createAST_Sayi(current_token);
                consume(TOKEN_NUMBER);
                consume(TOKEN_RIGHT_PAREN);
                
                register_parse_var(ad_token.value, "List");
                ASTNode* list_node = createAST_ListTanimlama(&tip_token, &ad_token, capacity_node, NULL);
                free(tip_token.value);
                free(ad_token.value);
                return list_node;
            } else if (current_token->type == TOKEN_RIGHT_PAREN) {
                // Empty list: numeric items() or numeric items() = (1,2,3)
                consume(TOKEN_RIGHT_PAREN);
                
                // Check for literal initialization: numeric items() = (1,2,3)
                if (current_token->type == TOKEN_ASSIGN) {
                    consume(TOKEN_ASSIGN);
                    literal_node = ifade();  // Should be list literal (1,2,3)
                    
                    register_parse_var(ad_token.value, "List");
                    ASTNode* list_node = createAST_ListTanimlama(&tip_token, &ad_token, NULL, literal_node);
                    free(tip_token.value);
                    free(ad_token.value);
                    return list_node;
                }
                
                // Empty list with no initialization
                register_parse_var(ad_token.value, "List");
                ASTNode* list_node = createAST_ListTanimlama(&tip_token, &ad_token, NULL, NULL);
                free(tip_token.value);
                free(ad_token.value);
                return list_node;
            }
        }

        // Normal değişken tanımlaması (numeric x = 5)
        consume(TOKEN_ASSIGN);
        ASTNode* ifade_dugumu = ifade();

        // Semicolon kaldırıldı - Python tarzı
        // consume(TOKEN_SEMICOLON);

        ASTNode* tanimlama_node = createAST_DegiskenTanimlama(&tip_token, &ad_token, ifade_dugumu);
        free(tip_token.value);
        free(ad_token.value);
        return tanimlama_node;
    }

    // 3. IF (Noktalı virgülsüz)
    if (current_token->type == TOKEN_IF) {
        return kosul_komutu(); // Kendi içinde 'SON'u yönetir
    }

    // 4. FOR Loop (Noktalı virgülsüz)
    if (current_token->type == TOKEN_FOR) {
        return for_komutu(); // Kendi içinde 'end for'u yönetir
    }

    // 5. WHILE/WHILE (Noktalı virgülsüz)
    if (current_token->type == TOKEN_WHILE) {
        return dongu_komutu(); // Kendi içinde 'end while'ı yönetir
    }
    
    // 6. break (Noktalı virgülsüz)
    if (current_token->type == TOKEN_BREAK) {
        consume(TOKEN_BREAK);
        specs_check_no_semicolon("break");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_BREAK;
        return node;
    }

    // 7. continue (Noktalı virgülsüz)
    if (current_token->type == TOKEN_CONTINUE) {
        consume(TOKEN_CONTINUE);
        specs_check_no_semicolon("continue");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_CONTINUE;
        return node;
    }

    // Phase 5.8: stop (debugging breakpoint)
    if (current_token->type == TOKEN_STOP) {
        consume(TOKEN_STOP);
        specs_check_no_semicolon("stop");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_STOP;
        return node;
    }

    // Phase 5.8: goto label_name
    if (current_token->type == TOKEN_GOTO) {
        consume(TOKEN_GOTO);
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Label name expected after goto", "IDENTIFIER");
        }
        Token* label_adi = (Token*)malloc(sizeof(Token));
        label_adi->type = current_token->type;
        label_adi->value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);
        specs_check_no_semicolon("goto");
        
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_GOTO;
        node->goto_data.label_adi = label_adi;
        return node;
    }
    
    // 8. DÖNÜŞ (Noktalı virgülsüz)
    if (current_token->type == TOKEN_RETURN) {
        return donus_komutu(); // Kendi içinde yönetir
    }

    // 7. OPERATOR (Function Definition) (Noktalı virgülsüz)
    if (current_token->type == TOKEN_FUNCTION) {
        return islec_tanimlama(); // Kendi içinde yönetir
    }

    // 7.5. Built-in Function Call as Statement (Phase 5.2)
    // Handle built-in calls like: set_error_code(42), mlp_free(ptr), assert(1, "msg")
    if (current_token->type == TOKEN_BUILTIN_EXIT_WITH_CODE ||
        current_token->type == TOKEN_BUILTIN_PANIC ||
        current_token->type == TOKEN_BUILTIN_ASSERT ||
        current_token->type == TOKEN_BUILTIN_SET_ERROR_CODE ||
        current_token->type == TOKEN_BUILTIN_MLP_FREE ||
        current_token->type == TOKEN_BUILTIN_CHECK_MEMORY_LEAKS ||
        current_token->type == TOKEN_BUILTIN_SLEEP_MS ||
        // Phase 5.3: Binary File I/O (write_binary can be used as statement)
        current_token->type == TOKEN_BUILTIN_WRITE_BINARY ||
        current_token->type == TOKEN_BUILTIN_COPY_FILE ||
        // Phase 5.3: Directory Operations (can be used as statements)
        current_token->type == TOKEN_BUILTIN_CREATE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_REMOVE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_CHANGE_DIRECTORY ||
        // Phase 5.3: Process Control (execute_command can be statement)
        current_token->type == TOKEN_BUILTIN_EXECUTE_COMMAND) {
        // Parse built-in call as expression first
        ASTNode* builtin_node = ifade();
        // Return as statement (no semicolon required)
        specs_check_no_semicolon("Built-in fonksiyon çağrısı");
        return builtin_node;
    }

    // 8. Atama or İfade Komutu (Noktalı virgülsüz)
    if (current_token->type == TOKEN_IDENTIFIER) {
        // Phase 5.8: Check for label (identifier followed by colon)
        Token* peek_token = peekNextToken();
        int is_label = (peek_token && peek_token->type == TOKEN_COLON);
        // Free peeked token to avoid memory leak
        if (peek_token) {
            if (peek_token->value) free(peek_token->value);
            free(peek_token);
        }
        if (is_label) {
            Token* label_adi = (Token*)malloc(sizeof(Token));
            label_adi->type = current_token->type;
            label_adi->value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);
            consume(TOKEN_COLON);
            specs_check_no_semicolon("label");
            
            ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
            node->type = AST_LABEL;
            node->label_data.label_adi = label_adi;
            return node;
        }

        ASTNode* sol_node = ifade(); // 'birincil()' çağrılır

        // Phase 5.7: Compound Assignment (+=, -=, *=, /=)
        if (sol_node->type == AST_DEGISKEN &&
            (current_token->type == TOKEN_PLUS_ASSIGN || current_token->type == TOKEN_MINUS_ASSIGN ||
             current_token->type == TOKEN_MUL_ASSIGN || current_token->type == TOKEN_DIV_ASSIGN)) {
            TokenType compound_op = current_token->type;
            consume(compound_op);
            ASTNode* sag_ifade = ifade();

            Token* ad_token = sol_node->degisken_data.ad;

            // Create: x = x op value
            ASTNode* var_ref = (ASTNode*)malloc(sizeof(ASTNode));
            var_ref->type = AST_DEGISKEN;
            Token* var_token = (Token*)malloc(sizeof(Token));
            var_token->type = TOKEN_IDENTIFIER;
            var_token->value = strdup(ad_token->value);
            var_token->line = ad_token->line;
            var_token->column = ad_token->column;
            var_ref->degisken_data.ad = var_token;

            TokenType binary_op_type;
            switch (compound_op) {
                case TOKEN_PLUS_ASSIGN: binary_op_type = TOKEN_PLUS; break;
                case TOKEN_MINUS_ASSIGN: binary_op_type = TOKEN_MINUS; break;
                case TOKEN_MUL_ASSIGN: binary_op_type = TOKEN_MUL; break;
                case TOKEN_DIV_ASSIGN: binary_op_type = TOKEN_DIV; break;
                default: binary_op_type = TOKEN_PLUS; break;
            }

            ASTNode* binary_op = (ASTNode*)malloc(sizeof(ASTNode));
            binary_op->type = AST_IKILI_ISLEM;
            binary_op->ikili_islem_data.sol = var_ref;
            binary_op->ikili_islem_data.sag = sag_ifade;
            binary_op->ikili_islem_data.operator_type = binary_op_type;

            ASTNode* atama_node = createAST_AtamaKomutu(ad_token, binary_op);
            specs_check_no_semicolon("Compound assignment");
            free(sol_node);
            return atama_node;
        }

        // DURUM 7.1: ATAMA (örn: x = 5, arr[i] = 5, p.x = 10)
        // 'ifade()' bize bir AST_DEGISKEN (x), AST_ARRAY_ERISIM or AST_STRUCT_FIELD_ACCESS döndürdüyse
        if ((sol_node->type == AST_DEGISKEN || sol_node->type == AST_ARRAY_ERISIM || sol_node->type == AST_STRUCT_FIELD_ACCESS) &&
            current_token->type == TOKEN_ASSIGN) {
            consume(TOKEN_ASSIGN); // '=' tüket
            ASTNode* sag_ifade = ifade(); // Sağ tarafı (5) ayrıştır

            specs_check_no_semicolon("Atama komutu");

            // Normal değişken ataması
            if (sol_node->type == AST_DEGISKEN) {
                Token* ad_token = sol_node->degisken_data.ad; // Adı al
                ASTNode* atama_node = createAST_AtamaKomutu(ad_token, sag_ifade);

                // sol_node'un belleğini düzgün yönet
                free(ad_token->value);
                free(ad_token);
                free(sol_node);

                return atama_node;
            }
            // Array erişimi ataması (arr[i] = value)
            else if (sol_node->type == AST_ARRAY_ERISIM) {
                specs_check_no_semicolon("Array atama komutu");

                // sol_node AST_ARRAY_ERISIM, ondan ad ve indeks al
                Token* array_ad = sol_node->array_erisim_data.ad;
                ASTNode* array_indeks = sol_node->array_erisim_data.indeks;

                // Yeni AST_ARRAY_ATAMA node oluştur
                ASTNode* array_atama = createAST_ArrayAtama(array_ad, array_indeks, sag_ifade);

                // sol_node'u temizle (ama içindekiler array_atama'da kullanılıyor, dikkat!)
                free(sol_node); // Sadece wrapper'ı free et

                return array_atama;
            }
            // Struct field ataması (p.x = 10)
            else if (sol_node->type == AST_STRUCT_FIELD_ACCESS) {
                specs_check_no_semicolon("Struct field atama komutu");

                // sol_node AST_STRUCT_FIELD_ACCESS, ondan struct_ad ve field_ad al
                Token* struct_ad = sol_node->struct_field_access_data.struct_ad;
                Token* field_ad = sol_node->struct_field_access_data.field_ad;

                // Yeni AST_STRUCT_FIELD_ATAMA node oluştur
                ASTNode* field_atama = createAST_StructFieldAtama(struct_ad, field_ad, sag_ifade);

                // sol_node'u temizle
                free(sol_node);

                return field_atama;
            }
        }
        
        // DURUM 7.2: İFADE KOMUTU (örn: test(), set_error_code(42), mlp_free(ptr))
        // 'ifade()' bize bir AST_ISLEC_CAGIRMA (test()) or AST_BUILTIN_CALL döndürdüyse
        if (sol_node->type == AST_ISLEC_CAGIRMA || sol_node->type == AST_BUILTIN_CALL) {
            specs_check_no_semicolon("İfade komutu (fonksiyon çağrısı)");
            return sol_node; // Fonksiyon çağrısını komut olarak döndür
        }

        // DURUM 7.3: LIST & MAP METHOD CALLS (list.add(), map.set(), etc.)
        // Phase 2 & 4: List and Map method calls as statements
        if (sol_node->type == AST_LIST_ADD || 
            sol_node->type == AST_LIST_CLEAR ||
            sol_node->type == AST_LIST_SET ||
            sol_node->type == AST_LIST_REMOVE ||
            sol_node->type == AST_LIST_INSERT ||
            sol_node->type == AST_MAP_SET ||
            sol_node->type == AST_MAP_REMOVE ||
            sol_node->type == AST_MAP_CLEAR) {
            specs_check_no_semicolon("List/Map method call");
            return sol_node; // Method call as statement
        }

        // Error: 'x' (standalone) or 'x + 5' (expression) is not a statement.
        parseError("Invalid statement. Assignment (=) or function call without semicolon expected.", "Invalid Statement");
    }

    // Copy-paste error (and other errors) removed.
    // If none match, this is an error.
    parseError("Statement expected.", "PRINT, Type, Identifier, IF, WHILE or OPERATOR");
    return NULL;
}

ASTNode* kosul_komutu() {
    consume(TOKEN_IF);
    ASTNode* kosul_ifadesi = ifade(); 
    consume(TOKEN_THEN);
    ASTNode* ise_blogu = blok();
    ASTNode* degilse_blogu = NULL;
    if (current_token->type == TOKEN_ELSE) {
        consume(TOKEN_ELSE);
        degilse_blogu = blok();
    }
    // Accept both "end eğer" and plain "end"
    if (current_token->type == TOKEN_END_IF) {
        consume(TOKEN_END_IF);
    } else {
        consume(TOKEN_END);
    }
    specs_check_no_semicolon("IF SON");
    return createAST_KosulKomutu(kosul_ifadesi, ise_blogu, degilse_blogu);
}

ASTNode* dongu_komutu() {
    consume(TOKEN_WHILE);
    
    // Check if there's a condition or it's an infinite loop
    ASTNode* kosul_ifadesi = NULL;
    
    // If next token is NOT 'do'/'then' or block starter, parse condition
    // Infinite loop: while do ... end or while \n body... end
    // Conditioned: while condition do ... end
    if (current_token->type != TOKEN_YAPI_DO &&
        current_token->type != TOKEN_THEN &&
        current_token->type != TOKEN_END) {
        // Parse condition
        kosul_ifadesi = ifade();
    }
    
    // Optional 'do' or 'then' keyword after condition
    if (current_token->type == TOKEN_YAPI_DO || current_token->type == TOKEN_THEN) {
        consume(current_token->type);
    }
    
    ASTNode* govde_blogu = blok();
    
    // Accept both "end döngü" and plain "end"
    if (current_token->type == TOKEN_END_WHILE) {
        consume(TOKEN_END_WHILE);
    } else {
        consume(TOKEN_END);
    }
    
    specs_check_no_semicolon("WHILE SON");
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_WHILE_LOOP_KOMUTU;
    node->dongu_data.kosul = kosul_ifadesi;  // NULL = infinite loop
    node->dongu_data.govde = govde_blogu;
    return node;
}

ASTNode* for_komutu() {
    consume(TOKEN_FOR);
    
    // Check for two syntax forms:
    // 1. for i = 0 to 10 [step 2]       (traditional)
    // 2. for i in range(0, 10, 2)       (range-based - Phase 5.8)
    
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("Loop variable name", "IDENTIFIER");
    }
    Token degisken_token;
    degisken_token.type = current_token->type;
    degisken_token.value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER);
    
    // Check if it's 'in' keyword (range-based for)
    if (current_token->type == TOKEN_IN) {
        consume(TOKEN_IN);
        
        // Two possibilities:
        // 1. for i in range(...)  -> AST_FOR_IN_RANGE
        // 2. for i in array_name  -> AST_FOR_EACH
        
        if (current_token->type == TOKEN_RANGE) {
            // Range-based for loop
            consume(TOKEN_RANGE);
            consume(TOKEN_LEFT_PAREN);
            
            // Parse arguments: start, end, [step]
            ASTNode* start_expr = NULL;
            ASTNode* end_expr = NULL;
            ASTNode* step_expr = NULL;
            
            if (current_token->type == TOKEN_RIGHT_PAREN) {
                // range() with no args - error
                parseError("range() requires at least 1 argument", "expression");
            }
            
            // First argument (can be end if only 1 arg, or start if 2+ args)
            ASTNode* first_arg = ifade();
            
            if (current_token->type == TOKEN_COMMA) {
                // Two or more arguments: range(start, end, [step])
                consume(TOKEN_COMMA);
                start_expr = first_arg;
                end_expr = ifade();
                
                if (current_token->type == TOKEN_COMMA) {
                    // Three arguments: range(start, end, step)
                    consume(TOKEN_COMMA);
                    step_expr = ifade();
                }
            } else {
                // One argument: range(end) - start defaults to 0
                Token* zero_token = createToken(TOKEN_NUMBER, "0");
                start_expr = createAST_Sayi(zero_token);
                end_expr = first_arg;
            }
            
            consume(TOKEN_RIGHT_PAREN);
            
            // Optional 'do' or 'then' before body
            if (current_token->type == TOKEN_YAPI_DO || current_token->type == TOKEN_THEN) {
                consume(current_token->type);
            }
            
            // Parse body
            ASTNode* govde_blogu = blok();
            
            // Accept both "end for" and plain "end"
            if (current_token->type == TOKEN_END_FOR) {
                consume(TOKEN_END_FOR);
            } else {
                consume(TOKEN_END);
            }
            
            specs_check_no_semicolon("FOR IN RANGE SON");
            
            // Create AST_FOR_IN_RANGE node
            ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
            node->type = AST_FOR_IN_RANGE;
            node->for_in_range_data.degisken = (Token*)malloc(sizeof(Token));
            node->for_in_range_data.degisken->type = degisken_token.type;
            node->for_in_range_data.degisken->value = strdup(degisken_token.value);
            node->for_in_range_data.start = start_expr;
            node->for_in_range_data.end = end_expr;
            node->for_in_range_data.step = step_expr;
            node->for_in_range_data.govde = govde_blogu;
            
            free(degisken_token.value);
            return node;
        } else if (current_token->type == TOKEN_IDENTIFIER) {
            // For-each loop: for item in array_name
            Token array_token;
            array_token.type = current_token->type;
            array_token.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);
            
            // Optional 'do' or 'then' before body
            if (current_token->type == TOKEN_YAPI_DO || current_token->type == TOKEN_THEN) {
                consume(current_token->type);
            }
            
            // Parse body
            ASTNode* govde_blogu = blok();
            
            // Accept both "end for" and plain "end"
            if (current_token->type == TOKEN_END_FOR) {
                consume(TOKEN_END_FOR);
            } else {
                consume(TOKEN_END);
            }
            
            specs_check_no_semicolon("FOR EACH SON");
            
            // Create AST_FOR_EACH node
            ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
            node->type = AST_FOR_EACH;
            node->for_each_data.degisken = (Token*)malloc(sizeof(Token));
            node->for_each_data.degisken->type = degisken_token.type;
            node->for_each_data.degisken->value = strdup(degisken_token.value);
            node->for_each_data.array_adi = (Token*)malloc(sizeof(Token));
            node->for_each_data.array_adi->type = array_token.type;
            node->for_each_data.array_adi->value = strdup(array_token.value);
            node->for_each_data.govde = govde_blogu;
            
            free(degisken_token.value);
            free(array_token.value);
            return node;
        } else {
            parseError("'range' or array name expected after 'in'", "range or identifier");
        }
    }
    
    // Traditional for loop: for i = 0 to 10 [step 2]
    consume(TOKEN_ASSIGN);
    ASTNode* baslangic_ifade = ifade();
    
    consume(TOKEN_TO);
    ASTNode* bitis_ifade = ifade();
    
    // Optional: step
    ASTNode* adim_ifade = NULL;
    if (current_token->type == TOKEN_STEP) {
        consume(TOKEN_STEP);
        adim_ifade = ifade();
    }
    
    // Optional 'do' or 'then' before body
    if (current_token->type == TOKEN_YAPI_DO || current_token->type == TOKEN_THEN) {
        consume(current_token->type);
    }
    
    // Parse body
    ASTNode* govde_blogu = blok();
    
    // Accept both "end for" and plain "end"
    if (current_token->type == TOKEN_END_FOR) {
        consume(TOKEN_END_FOR);
    } else {
        consume(TOKEN_END);
    }
    
    specs_check_no_semicolon("FOR SON");
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_FOR_KOMUTU;
    node->for_data.degisken = (Token*)malloc(sizeof(Token));
    node->for_data.degisken->type = degisken_token.type;
    node->for_data.degisken->value = strdup(degisken_token.value);
    node->for_data.baslangic = baslangic_ifade;
    node->for_data.bitis = bitis_ifade;
    node->for_data.adim = adim_ifade;
    node->for_data.govde = govde_blogu;
    
    free(degisken_token.value);
    return node;
}

ASTNode* islec_tanimlama() {
    consume(TOKEN_FUNCTION); 
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("İşleç adı expected.", "IDENTIFIER");
    }
    Token ad_token;
    ad_token.type = current_token->type;
    ad_token.value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER); 

    consume(TOKEN_LEFT_PAREN);
    Token** parametre_listesi = NULL;
    ASTNode** parametre_defaults = NULL; // Phase 5.11: Default values
    int* parametre_is_array = NULL; // Phase 7.6: Track which params are arrays
    int p_sayisi = 0;
    if (current_token->type != TOKEN_RIGHT_PAREN) {
        parametre_listesi = (Token**)malloc(sizeof(Token*) * 10);
        parametre_defaults = (ASTNode**)malloc(sizeof(ASTNode*) * 10);
        parametre_is_array = (int*)malloc(sizeof(int) * 10);
        do {
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Parametre adı expected.", "IDENTIFIER");
            }
            Token* param_token = (Token*)malloc(sizeof(Token));
            param_token->type = current_token->type;
            param_token->value = strdup(current_token->value);
            parametre_listesi[p_sayisi] = param_token;
            consume(TOKEN_IDENTIFIER); // Consume parameter name first
            
            // Phase 7.6: Check for array parameter syntax: name[]
            if (current_token->type == TOKEN_LEFT_BRACKET) {
                consume(TOKEN_LEFT_BRACKET);
                if (current_token->type != TOKEN_RIGHT_BRACKET) {
                    parseError("Array parameter must be []", "]");
                }
                consume(TOKEN_RIGHT_BRACKET);
                parametre_is_array[p_sayisi] = 1; // Mark as array parameter
            } else {
                parametre_is_array[p_sayisi] = 0; // Regular parameter
            }
            
            // Phase 5.11: Check for default value (= expression)
            if (current_token->type == TOKEN_ASSIGN) {
                consume(TOKEN_ASSIGN);
                parametre_defaults[p_sayisi] = ifade(); // Parse default value
            } else {
                parametre_defaults[p_sayisi] = NULL; // No default
            }
            
            p_sayisi++;
        } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
    }
    consume(TOKEN_RIGHT_PAREN);

    // then keyword kaldırıldı - direkt blok başlar
    ASTNode* govde_blogu = blok();
    
    // Only accept plain "end" (compound "end function" breaks lexer position)
    consume(TOKEN_END);
    
    specs_check_no_semicolon("OPERATOR SON");
    
    ASTNode* node = createAST_IslecTanimlama(&ad_token, parametre_listesi, p_sayisi, parametre_is_array, parametre_defaults, govde_blogu);
    free(ad_token.value);
    return node;
}

// Phase 8: Async Function Parser
ASTNode* async_function_tanimlama() {
    consume(TOKEN_ASYNC);
    consume(TOKEN_FUNCTION);
    
    if (current_token == NULL) {
        parseError("Unexpected end of file after 'async function'", "IDENTIFIER");
    }
    
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("Async function name expected.", "IDENTIFIER");
    }
    Token* ad_token = (Token*)malloc(sizeof(Token));
    ad_token->type = current_token->type;
    ad_token->value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER);

    consume(TOKEN_LEFT_PAREN);
    Token** parametre_listesi = NULL;
    int p_sayisi = 0;
    
    if (current_token->type != TOKEN_RIGHT_PAREN) {
        parametre_listesi = (Token**)malloc(sizeof(Token*) * 10);
        do {
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Parameter name expected.", "IDENTIFIER");
            }
            Token* param_token = (Token*)malloc(sizeof(Token));
            param_token->type = current_token->type;
            param_token->value = strdup(current_token->value);
            parametre_listesi[p_sayisi++] = param_token;
            consume(TOKEN_IDENTIFIER);
        } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
    }
    consume(TOKEN_RIGHT_PAREN);

    // Parse function body
    ASTNode* govde_blogu = blok();
    
    // Only accept plain "end" (compound "end function" breaks lexer position)
    consume(TOKEN_END);
    
    specs_check_no_semicolon("ASYNC FUNCTION SON");
    
    // Create async function AST node
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_ASYNC_FUNCTION;
    node->async_function_data.ad = ad_token;
    node->async_function_data.parametreler = parametre_listesi;
    node->async_function_data.parametre_sayisi = p_sayisi;
    node->async_function_data.govde = govde_blogu;
    
    return node;
}

ASTNode* donus_komutu() {
    consume(TOKEN_RETURN);
    ASTNode* ifade_dugumu = ifade();
    specs_check_no_semicolon("DÖNÜŞ komutu");
    ASTNode* donus_node = (ASTNode*)malloc(sizeof(ASTNode));
    donus_node->type = AST_RETURN_STATEMENT_KOMUTU;
    donus_node->tek_ifade_data.ifade = ifade_dugumu;
    return donus_node;
}

ASTNode* blok() {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) exit(EXIT_FAILURE);
    node->type = AST_BLOK;
    node->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
    node->blok_data.sayisi = 0;

    while (current_token != NULL && // NULL kontrolü eklendi
           current_token->type != TOKEN_EOF &&
           current_token->type != TOKEN_END &&
           current_token->type != TOKEN_END_IF &&
           current_token->type != TOKEN_END_WHILE &&
           current_token->type != TOKEN_END_FOR &&
           current_token->type != TOKEN_END_FUNCTION &&
           current_token->type != TOKEN_END_STRUCT &&
           current_token->type != TOKEN_END_ENUM &&
           current_token->type != TOKEN_END_SWITCH &&
           current_token->type != TOKEN_ELSE &&
           current_token->type != TOKEN_YAPI_CASE &&
           current_token->type != TOKEN_YAPI_DEFAULT) 
    {
        // Semicolon artık kullanılmıyor - Python tarzı syntax

        ASTNode* k = komut();
        if (k != NULL) {
            node->blok_data.komutlar[node->blok_data.sayisi++] = k;
        }
    }
    return node;
}

// Phase 6.2: Optional<T> Tanımlama Parser
// Syntax: optional<string> text = optional<string>()
// or:     optional<string> text = null
ASTNode* optional_tanimlama_parse() {
    consume(TOKEN_OPTIONAL);

    // Expect < for generic type
    if (current_token->type != TOKEN_LT) {
        parseError("optional<Type>", "<");
    }
    consume(TOKEN_LT);

    // Element type (int, string, or custom type name)
    Token* element_tip = (Token*)malloc(sizeof(Token));
    if (current_token->type == TOKEN_TYPE_NUMERIC ||
        current_token->type == TOKEN_TYPE_STRING ||
        current_token->type == TOKEN_TYPE_BOOLEAN ||
        current_token->type == TOKEN_IDENTIFIER) {
        element_tip->type = current_token->type;
        element_tip->value = strdup(current_token->value);
        consume(current_token->type);
    } else {
        parseError("optional element type", "int/string/bool/StructName");
    }

    // Expect > to close generic type
    if (current_token->type != TOKEN_GT) {
        parseError("optional<Type>", ">");
    }
    consume(TOKEN_GT);

    // Variable name
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("optional variable name", "IDENTIFIER");
    }
    Token* degisken_adi = (Token*)malloc(sizeof(Token));
    degisken_adi->type = current_token->type;
    degisken_adi->value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER);

    // Optional: = initialization
    // If no =, create default (none/empty optional)
    if (current_token->type != TOKEN_ASSIGN) {
        // No initialization - create empty optional
        ASTNode* optional_node = createAST_OptionalTanimlama(element_tip, degisken_adi);
        return optional_node;
    }
    
    consume(TOKEN_ASSIGN);

    // Two options:
    // 1. optional<Type>() - empty constructor
    // 2. null - null value
    if (current_token->type == TOKEN_OPTIONAL) {
        consume(TOKEN_OPTIONAL);

        // Expect < again for constructor
        consume(TOKEN_LT);

        // Skip element type (already have it)
        if (current_token->type == TOKEN_TYPE_NUMERIC ||
            current_token->type == TOKEN_TYPE_STRING ||
            current_token->type == TOKEN_TYPE_BOOLEAN ||
            current_token->type == TOKEN_IDENTIFIER) {
            consume(current_token->type);
        }

        // Expect >
        consume(TOKEN_GT);

        // Expect ()
        consume(TOKEN_LEFT_PAREN);
        consume(TOKEN_RIGHT_PAREN);
    } else if (current_token->type == TOKEN_NULL) {
        // Allow null assignment
        consume(TOKEN_NULL);
    }

    ASTNode* optional_node = createAST_OptionalTanimlama(element_tip, degisken_adi);
    return optional_node;
}

// Phase 6: List<T> Tanımlama Parser
// Modern syntax: List<int> numbers = List<int>()
ASTNode* list_tanimlama_parse() {
    consume(TOKEN_LIST);

    // Expect < for generic type
    if (current_token->type != TOKEN_LT) {
        parseError("List<Type>", "<");
    }
    consume(TOKEN_LT);

    // Element type (int, string, or custom type name)
    Token* element_tip = (Token*)malloc(sizeof(Token));
    if (current_token->type == TOKEN_TYPE_NUMERIC ||
        current_token->type == TOKEN_TYPE_STRING ||
        current_token->type == TOKEN_TYPE_BOOLEAN ||
        current_token->type == TOKEN_IDENTIFIER) {
        element_tip->type = current_token->type;
        element_tip->value = strdup(current_token->value);
        consume(current_token->type);
    } else {
        parseError("List element type", "int/string/bool/StructName");
    }

    // Expect > to close generic type
    if (current_token->type != TOKEN_GT) {
        parseError("List<Type>", ">");
    }
    consume(TOKEN_GT);

    // Variable name
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("List variable name", "IDENTIFIER");
    }
    Token* degisken_adi = (Token*)malloc(sizeof(Token));
    degisken_adi->type = current_token->type;
    degisken_adi->value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER);

    // Expect =
    consume(TOKEN_ASSIGN);

    // Expect List<Type>()
    if (current_token->type != TOKEN_LIST) {
        parseError("List<Type>()", "List");
    }
    consume(TOKEN_LIST);
    
    // Expect < again for constructor
    consume(TOKEN_LT);
    
    // Skip element type (already have it)
    if (current_token->type == TOKEN_TYPE_NUMERIC ||
        current_token->type == TOKEN_TYPE_STRING ||
        current_token->type == TOKEN_TYPE_BOOLEAN ||
        current_token->type == TOKEN_IDENTIFIER) {
        consume(current_token->type);
    }
    
    // Expect >
    consume(TOKEN_GT);
    
    // Expect ()
    consume(TOKEN_LEFT_PAREN);
    consume(TOKEN_RIGHT_PAREN);

    // Semicolon kaldırıldı - Python tarzı
    // consume(TOKEN_SEMICOLON);

    ASTNode* list_node = createAST_ListTanimlama(element_tip, degisken_adi, NULL, NULL);
    // Tokens are now owned by AST
    return list_node;
}

// --- Ana Ayrıştırma Fonksiyonu ---

ASTNode* parse(const char* source_code) {
    initLexer(source_code);
    current_token = getNextToken();

    ASTNode* root = (ASTNode*)malloc(sizeof(ASTNode));
    root->type = AST_BLOK;
    root->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
    root->blok_data.sayisi = 0;

    while (current_token != NULL && current_token->type != TOKEN_EOF) {
        // 'blok' fonksiyonu EOF görene kadar ana kod akışını okur
        root->blok_data.komutlar[root->blok_data.sayisi++] = komut();
    }
    return root;
}
// ========== c_generator.c ==========
// c_compiler/c_generator.c


// Forward declaration from main.c
extern const char* get_base_dir();

// --- Generator Durum Yönetimi ---
static AsmCode data_section; // .data bölümü (örn: "Merhaba")
static AsmCode text_section; // .text bölümü (ana kod)
static AsmCode lambda_section; // Phase 7.9: Deferred lambda functions

// YENİ: Kapsam Yönetimi (Python'daki Kapsam sınıfına karşılık gelir)

#define MAX_DEGISKENLER 100
#define MAX_SCOPE_DEPTH 20

typedef struct {
    char* ad;
    char* asm_adresi; // Yığındaki adresi (örn: "[rbp-8]") or global için label
    char* tip;        // Değişkenin tipi ("SAYISAL" or "METIN")
    int scope_level;  // Hangi scope seviyesinde tanımlandı (0=global)
    bool is_global;   // Global değişken mi?
    bool is_const;    // Phase 5.6: Const değişken mi?
    int array_boyut;  // Phase 5.8: Array boyutu (0 = not an array)
} Degisken;

// Çok seviyeli Kapsam (Scope) - Stack bazlı
Degisken kapsam_haritasi[MAX_DEGISKENLER];
int kapsam_degisken_sayisi = 0;
int kapsam_yigin_ofseti = 0; // RBP'den ne kadar aşağı inildiği (örn: -8, -16)

// Scope seviyesi tracking
int current_scope_level = 0;
int scope_stack_offsets[MAX_SCOPE_DEPTH]; // Her scope seviyesinde stack offset

// YENİ: Etiket (Label) Sayacı
static int etiket_sayaci = 0;

// YENİ: String literal sayacı (benzersiz etiketler için)
static int metin_sayaci = 0;

// YENİ: Aktif Döngü Bitiş Etiketi
// (İç içe döngüler için bu bir yığın (stack) olmalıdır, şimdilik basit tutalım)
static int aktif_dongu_son_etiketi = -1;
static int aktif_dongu_baslangic_etiketi = -1;  // Phase 5.8: For continue support

// Argüman register'ları (Linux x86-64 ABI)
const char* arg_registerleri[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// --- Struct Metadata Yönetimi ---
#define MAX_STRUCT_FIELDS 20
#define MAX_STRUCTS 50

typedef struct {
    char* field_ad;
    char* field_tip;
    int offset;  // Struct başından itibaren byte offset
    int size;    // Field boyutu (byte)
} StructFieldInfo;

typedef struct {
    char* struct_ad;
    int field_sayisi;
    StructFieldInfo fields[MAX_STRUCT_FIELDS];
    int total_size;  // Toplam struct boyutu (byte)
} StructMetadata;

// Global struct metadata tablosu
StructMetadata struct_metadata_table[MAX_STRUCTS];
int struct_metadata_count = 0;

// --- Phase 5.4: Enum Metadata Yönetimi ---
#define MAX_ENUM_VALUES 100
#define MAX_ENUMS 50

typedef struct {
    char* value_ad;
    int value;
} EnumValueInfo;

typedef struct {
    char* enum_ad;
    int value_sayisi;
    EnumValueInfo values[MAX_ENUM_VALUES];
} EnumMetadata;

// Global enum metadata tablosu
EnumMetadata enum_metadata_table[MAX_ENUMS];
int enum_metadata_count = 0;

// Enum değeri bul
int enum_value_bul(const char* value_ad) {
    for (int i = 0; i < enum_metadata_count; i++) {
        for (int j = 0; j < enum_metadata_table[i].value_sayisi; j++) {
            if (strcmp(enum_metadata_table[i].values[j].value_ad, value_ad) == 0) {
                return enum_metadata_table[i].values[j].value;
            }
        }
    }
    return -1; // Bulunamadı
}

// Enum değeri mi kontrol et
int is_enum_value(const char* ad) {
    return enum_value_bul(ad) >= 0;
}

// Kapsam Yönetim Fonksiyonları

// Kapsam haritasında strdup ile ayrılmış belleği temizler
void kapsam_temizle() {
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        free(kapsam_haritasi[i].ad);
        free(kapsam_haritasi[i].asm_adresi);
        free(kapsam_haritasi[i].tip);
    }
    kapsam_degisken_sayisi = 0;
    current_scope_level = 0;
}

// Yeni bir scope'a gir (IF, WHILE, OPERATOR blokları için)
void kapsam_gir() {
    if (current_scope_level >= MAX_SCOPE_DEPTH - 1) {
        fprintf(stderr, "HATA [Generator]: Maksimum scope derinliği aşıldı!\n");
        exit(1);
    }

    // Mevcut stack offset'i kaydet
    scope_stack_offsets[current_scope_level] = kapsam_yigin_ofseti;
    current_scope_level++;
}

// Scope'tan çık - bu seviyedeki değişkenleri temizle
void kapsam_cik() {
    if (current_scope_level <= 0) {
        return; // Zaten en üst seviyedeyiz
    }

    current_scope_level--;

    // Bu seviyedeki değişkenleri temizle
    int new_var_count = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level <= current_scope_level) {
            // Bu değişkeni tut
            if (i != new_var_count) {
                kapsam_haritasi[new_var_count] = kapsam_haritasi[i];
            }
            new_var_count++;
        } else {
            // Bu değişkeni temizle
            free(kapsam_haritasi[i].ad);
            free(kapsam_haritasi[i].asm_adresi);
            free(kapsam_haritasi[i].tip);
        }
    }

    kapsam_degisken_sayisi = new_var_count;

    // Stack offset'i geri yükle
    kapsam_yigin_ofseti = scope_stack_offsets[current_scope_level];
}

// Scope'ta değişken ara (Phase 5.9: String interpolation için)
Degisken* kapsam_bul(const char* ad) {
    // En içteki scope'tan başlayarak ara
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return &kapsam_haritasi[i];
        }
    }
    return NULL; // Bulunamadı
}

// --- Struct Metadata Helper Fonksiyonları ---

// Field tipine göre byte cinsinden boyut döndürür
int get_field_size(const char* tip) {
    // Basitleştirilmiş: Tüm tipler 8 byte (pointer or int64)
    // string -> pointer (8 byte)
    // int -> int64 (8 byte)
    // bool -> int64 (8 byte)
    return 8;
}

// Struct tanımını metadata tablosuna kaydeder
void register_struct_metadata(char* struct_ad, Token** field_tipleri, Token** field_adlari, int field_sayisi) {
    if (struct_metadata_count >= MAX_STRUCTS) {
        fprintf(stderr, "HATA [Generator]: Maksimum struct sayısı aşıldı!\n");
        exit(1);
    }

    StructMetadata* meta = &struct_metadata_table[struct_metadata_count++];
    meta->struct_ad = strdup(struct_ad);
    meta->field_sayisi = field_sayisi;

    int current_offset = 0;
    for (int i = 0; i < field_sayisi; i++) {
        meta->fields[i].field_ad = strdup(field_adlari[i]->value);
        meta->fields[i].field_tip = strdup(field_tipleri[i]->value);
        meta->fields[i].offset = current_offset;
        meta->fields[i].size = get_field_size(field_tipleri[i]->value);
        current_offset += meta->fields[i].size;
    }

    meta->total_size = current_offset;
}

// Struct adına göre metadata bulur (NULL if not found)
StructMetadata* find_struct_metadata(const char* struct_ad) {
    for (int i = 0; i < struct_metadata_count; i++) {
        if (strcmp(struct_metadata_table[i].struct_ad, struct_ad) == 0) {
            return &struct_metadata_table[i];
        }
    }
    return NULL;
}

// Struct içinde field bulur (NULL if not found)
StructFieldInfo* find_field_in_struct(StructMetadata* meta, const char* field_ad) {
    for (int i = 0; i < meta->field_sayisi; i++) {
        if (strcmp(meta->fields[i].field_ad, field_ad) == 0) {
            return &meta->fields[i];
        }
    }
    return NULL;
}

// Değişkeni kaydeder ve yığındaki adresini döndürür
char* kapsam_degisken_yer_ayir_with_const(const char* ad, const char* tip, bool is_const) {
    kapsam_yigin_ofseti += 8; // Yığında 8 byte (64-bit) yer aç
    char* adres = (char*)malloc(32);
    sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);

    // Değişkeni haritaya kaydet
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(ad);
    d->asm_adresi = strdup(adres);
    d->tip = strdup(tip);
    d->scope_level = current_scope_level;
    d->is_global = (current_scope_level == 0);
    d->is_const = is_const; // Phase 5.6

    return adres;
}

// Backward compatibility wrapper
char* kapsam_degisken_yer_ayir(const char* ad, const char* tip) {
    return kapsam_degisken_yer_ayir_with_const(ad, tip, false);
}

// Bir değişkenin yığındaki adresini bulur
// NOT: Döndürülen string strdup ile oluşturulur, caller free etmeli!
char* kapsam_degisken_adresi_bul(const char* ad) {
    // Kapsam haritasını geriye doğru ara (en son tanımlananı bul)
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return strdup(kapsam_haritasi[i].asm_adresi);  // Return copy, caller frees
        }
    }

    fprintf(stderr, "HATA [Generator]: Tanımsız değişken: %s\n", ad);
    exit(1);
}

// Bir değişkenin tam bilgisini (Degisken pointer) bulur
Degisken* kapsam_degisken_bul(const char* ad) {
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return &kapsam_haritasi[i];
        }
    }
    return NULL;  // Bulunamadı
}

// Bir değişkenin tipini bulur
char* kapsam_degisken_tipi_bul(const char* ad) {
    if (ad == NULL) {
        fprintf(stderr, "HATA [Generator]: kapsam_degisken_tipi_bul'a NULL ad geldi\n");
        exit(1);
    }

    // Kapsam haritasını geriye doğru ara (en son tanımlananı bul)
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (kapsam_haritasi[i].ad != NULL && strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return kapsam_haritasi[i].tip;
        }
    }

    fprintf(stderr, "HATA [Generator]: Tanımsız değişken: %s (Kapsam: %d değişken)\n", ad, kapsam_degisken_sayisi);
    exit(1);
}

// --- Yardımcı Fonksiyonlar ---

// TYD fonksiyon adlarını C runtime fonksiyon adlarına map eder
const char* convert_function_name(const char* tyd_adi) {
    // Dosya fonksiyonları
    if (strcmp(tyd_adi, "DOSYA_AC") == 0) return "tyd_dosya_ac";
    if (strcmp(tyd_adi, "DOSYA_OKU") == 0) return "tyd_dosya_oku";
    if (strcmp(tyd_adi, "DOSYA_YAZ") == 0) return "tyd_dosya_yaz";
    if (strcmp(tyd_adi, "DOSYA_KAPAT") == 0) return "tyd_dosya_kapat";
    
    // String fonksiyonları
    if (strcmp(tyd_adi, "STRLEN") == 0) return "strlen";
    if (strcmp(tyd_adi, "SUBSTR") == 0) return "tyd_substr";
    if (strcmp(tyd_adi, "STRCAT") == 0) return "tyd_strcat";
    if (strcmp(tyd_adi, "STRSTR") == 0) return "strstr";
    if (strcmp(tyd_adi, "STRCMP") == 0) return "strcmp";
    
    return tyd_adi;
}

// AsmCode yapısına güvenli bir şekilde kod ekler
void asm_append(AsmCode* section, const char* code) {
    if (code == NULL) {
        fprintf(stderr, "HATA: asm_append'e NULL kod geldi!\n");
        return;
    }

    size_t code_len = strlen(code);
    size_t new_line_len = 1; // for '\n'
    size_t needed = section->size + code_len + new_line_len + 1; // +1 for null

    // Gerekirse kapasiteyi artır (GCC-style: 4KB başlangıç, 2x büyütme)
    if (needed > section->capacity) {
        // Yeni kapasite: başlangıçta 4KB, sonra 2x or tam ihtiyaç
        size_t new_capacity = (section->capacity == 0) ? 4096 : section->capacity * 2;

        // Eğer 2x bile yetmezse, tam ihtiyacı karşıla + %25 buffer
        if (new_capacity < needed) {
            new_capacity = needed + (needed / 4);
        }

        char* new_code = (char*)realloc(section->code, new_capacity);
        if (new_code == NULL) {
            fprintf(stderr, "HATA: Bellek ayırma başarısız (istenen: %zu bytes)\n", new_capacity);
            exit(EXIT_FAILURE);
        }

        section->code = new_code;
        section->capacity = new_capacity;

        // İlk ayırmada string'i sıfırla
        if (section->size == 0) {
            section->code[0] = '\0';
        }
    }

    // GÜVENLİ string ekleme (strcpy yerine memcpy + manuel null)
    memcpy(section->code + section->size, code, code_len);
    section->code[section->size + code_len] = '\n';
    section->code[section->size + code_len + 1] = '\0';
    section->size += (code_len + new_line_len);
}

// --- Ziyaretçi (Visitor) Fonksiyonları ---

void visit(ASTNode* node); // Ana ziyaretçi fonksiyonu (prototip)
void visit_IkiliIslem(ASTNode* node); // İleri bildirim
void visit_DegiskenTanimlama(ASTNode* node); // İleri bildirim
void visit_Degisken(ASTNode* node); // İleri bildirim
void visit_AtamaKomutu(ASTNode* node); // İleri bildirim
void visit_KosulKomutu(ASTNode* node); // İleri bildirim
void visit_DonguKomutu(ASTNode* node); // İleri bildirim
void visit_ForKomutu(ASTNode* node); // İleri bildirim
void visit_ForInRange(ASTNode* node); // Phase 5.8: for X in range(...)
void visit_ForEach(ASTNode* node);    // Phase 5.8: for X in array
void visit_DonguBitirKomutu(ASTNode* node); // İleri bildirim
void visit_DonguDevamKomutu(ASTNode* node); // Phase 5.8: Continue
void visit_IslecTanimlama(ASTNode* node); // İleri bildirim
void visit_Lambda(ASTNode* node); // Phase 7.1: Lambda expression
void visit_AsyncFunction(ASTNode* node); // Phase 8: Async function
void visit_AwaitExpr(ASTNode* node); // Phase 8: Await expression
void visit_IslecCagirma(ASTNode* node); // İleri bildirim
void visit_DonusKomutu(ASTNode* node); // İleri bildirim
void visit_TypeofExpr(ASTNode* node); // Phase 5.7: typeof operator
void visit_ArrayTanimlama(ASTNode* node); // İleri bildirim
void visit_ArrayErisim(ASTNode* node); // İleri bildirim
void visit_ArrayAtama(ASTNode* node); // İleri bildirim
void visit_ArrayLiteral(ASTNode* node); // İleri bildirim - Array literal
void visit_ListLiteral(ASTNode* node); // İleri bildirim - List literal
void visit_StructTanimlama(ASTNode* node); // İleri bildirim
void visit_StructFieldAccess(ASTNode* node); // İleri bildirim
void visit_StructFieldAtama(ASTNode* node); // İleri bildirim
void visit_StructDegisken(ASTNode* node); // İleri bildirim

// Phase 5.8: Control flow & debugging forward declarations
void visit_Ternary(ASTNode* node);
void visit_Stop(ASTNode* node);
void visit_Goto(ASTNode* node);
void visit_Label(ASTNode* node);

// Phase 2: List visitor forward declarations
void visit_ListTanimlama(ASTNode* node);
void visit_ListAdd(ASTNode* node);
void visit_ListGet(ASTNode* node);
void visit_ListSet(ASTNode* node);
void visit_ListRemove(ASTNode* node);
void visit_ListInsert(ASTNode* node);
void visit_ListSize(ASTNode* node);
void visit_ListClear(ASTNode* node);

// Phase 6.2: Optional<T> visitor forward declarations
void visit_OptionalTanimlama(ASTNode* node);
void visit_OptionalHasValue(ASTNode* node);
void visit_OptionalValue(ASTNode* node);
void visit_OptionalValueOr(ASTNode* node);

// Phase 4: Hash Map visitor forward declarations
void visit_MapTanimlama(ASTNode* node);
void visit_MapSet(ASTNode* node);
void visit_MapGet(ASTNode* node);
void visit_MapHas(ASTNode* node);
void visit_MapRemove(ASTNode* node);
void visit_MapSize(ASTNode* node);
void visit_MapClear(ASTNode* node);

// Phase 5.4: Enum ve Switch forward declarations
void visit_EnumTanimlama(ASTNode* node);
void visit_Switch(ASTNode* node);

void visit_Blok(ASTNode* node) {
    // Blok içindeki her komutu ziyaret et
    for (int i = 0; i < node->blok_data.sayisi; i++) {
        visit(node->blok_data.komutlar[i]);
    }
}

void visit_Yazdir(ASTNode* node) {

    // İfadenin tipine göre format string seç (ÖNCE tip belirle, sonra visit et)
    bool is_string = false;

    if (node->tek_ifade_data.ifade->type == AST_METIN) {
        is_string = true;
    } else if (node->tek_ifade_data.ifade->type == AST_INTERPOLATED_STRING) {
        // Phase 5.9: String interpolation always returns a string
        is_string = true;
    } else if (node->tek_ifade_data.ifade->type == AST_DEGISKEN) {
        // Değişken ise, tipini kontrol et
        if (node->tek_ifade_data.ifade->degisken_data.ad != NULL &&
            node->tek_ifade_data.ifade->degisken_data.ad->value != NULL) {
            char* degisken_adi = node->tek_ifade_data.ifade->degisken_data.ad->value;
            char* degisken_tipi = kapsam_degisken_tipi_bul(degisken_adi);
            if (degisken_tipi != NULL && (strcmp(degisken_tipi, "METIN") == 0 || strcmp(degisken_tipi, "string") == 0)) {
                is_string = true;
            }
        } else {
        }
    } else if (node->tek_ifade_data.ifade->type == AST_BUILTIN_CALL) {
        // Built-in call ise, string döndüren fonksiyonları kontrol et
        TokenType func_type = node->tek_ifade_data.ifade->builtin_call_data.function_type;
        if (func_type == TOKEN_BUILTIN_STR ||  // Phase 7.9+: str() returns string
            func_type == TOKEN_BUILTIN_GET_ENV ||
            func_type == TOKEN_BUILTIN_READ_BINARY ||
            func_type == TOKEN_BUILTIN_GET_FILE_INFO ||
            func_type == TOKEN_BUILTIN_GET_CURRENT_DIR ||
            func_type == TOKEN_BUILTIN_GET_COMMAND_OUTPUT ||
            func_type == TOKEN_BUILTIN_FORMAT_TIMESTAMP ||
            func_type == TOKEN_BUILTIN_GET_TIME_STRING ||
            func_type == TOKEN_BUILTIN_GET_FILE_EXTENSION ||
            func_type == TOKEN_BUILTIN_GET_FILE_NAME ||
            func_type == TOKEN_BUILTIN_GET_DIRECTORY) {
            is_string = true;
        }
    } else if (node->tek_ifade_data.ifade->type == AST_ISLEC_CAGIRMA) {
        // Function call - check if it's a string-returning function
        char* func_name = node->tek_ifade_data.ifade->islec_cagirma_data.hedef_ad->value;
        if (strcmp(func_name, "str") == 0 ||
            strcmp(func_name, "SUBSTR") == 0 ||
            strcmp(func_name, "STRCAT") == 0 ||
            strcmp(func_name, "STRING_BIRLESTIR") == 0 ||
            strcmp(func_name, "STRING_KARAKTER_AL") == 0 ||
            strcmp(func_name, "STRING_ALT") == 0 ||
            strcmp(func_name, "KODU_KARAKTERE") == 0) {
            is_string = true;
        }
    }

    // 1. İfadeyi ziyaret et (sonuç RAX'e yüklenecek)
    visit(node->tek_ifade_data.ifade);

    // 2. C 'printf' fonksiyonunu çağır
    asm_append(&text_section, "    ; --- PRINT Başlangıç ---");


    if (is_string) {
        // String yazdır (RAX'te string adresi var)
        asm_append(&text_section, "    mov rdi, format_metin"); // format_metin: "%s\n"
        asm_append(&text_section, "    mov rsi, rax");      // String adresi
    } else {
        // Sayı yazdır (RAX'te sayı var)
        asm_append(&text_section, "    mov rdi, format_sayi"); // format_sayi: "%ld\n"
        asm_append(&text_section, "    mov rsi, rax");      // Yazdırılacak değer
    }

    asm_append(&text_section, "    mov rax, 0");      // SSE kuralı
    asm_append(&text_section, "    call printf");
}

void visit_Sayi(ASTNode* node) {
    // Bir sayıyı RAX register'ına yükle
    // Geçici düzeltme (sprintf kullanarak):
    char buffer[64];
    sprintf(buffer, "    mov rax, %s", node->sabit_data.deger);
    asm_append(&text_section, buffer);
}

// Helper function: Escape special characters for NASM assembly
char* escape_string_for_asm(const char* input) {
    static char buffer[4096];  // Increased for UTF-8 escape sequences
    int j = 0;

    for (int i = 0; input[i] != '\0' && j < 4090; i++) {
        unsigned char c = (unsigned char)input[i];

        // Escape special characters
        if (c == '\n') {
            buffer[j++] = '\\';
            buffer[j++] = 'n';
        } else if (c == '\t') {
            buffer[j++] = '\\';
            buffer[j++] = 't';
        } else if (c == '\r') {
            buffer[j++] = '\\';
            buffer[j++] = 'r';
        } else if (c == '\\') {
            buffer[j++] = '\\';
            buffer[j++] = '\\';
        } else if (c >= 32 && c < 127) {
            // Printable ASCII (including quotes - handled by delimiter choice in visit_Metin)
            buffer[j++] = c;
        } else {
            // Non-ASCII or non-printable: escape as octal \xxx
            sprintf(buffer + j, "\\%03o", c);
            j += 4;
        }
    }
    buffer[j] = '\0';
    return buffer;
}

void visit_Metin(ASTNode* node) {
    // String literal'i .data bölümüne ekle ve adresini RAX'e yükle
    char etiket[64];
    char buffer[8192];

    // Benzersiz etiket oluştur
    sprintf(etiket, "str_%d", metin_sayaci++);

    // Check if string contains non-ASCII (UTF-8) or special characters
    int has_utf8 = 0;
    for (const char* p = node->sabit_data.deger; *p; p++) {
        if ((unsigned char)*p >= 128) {
            has_utf8 = 1;
            break;
        }
    }

    if (has_utf8) {
        // UTF-8 string: output as byte sequence
        sprintf(buffer, "%s: db ", etiket);
        int len = strlen(buffer);
        
        for (const char* p = node->sabit_data.deger; *p; p++) {
            if (*p == '\n') {
                len += sprintf(buffer + len, "10,");  // newline
            } else if (*p == '\t') {
                len += sprintf(buffer + len, "9,");   // tab
            } else if (*p == '\r') {
                len += sprintf(buffer + len, "13,");  // carriage return
            } else {
                len += sprintf(buffer + len, "%d,", (unsigned char)*p);
            }
        }
        sprintf(buffer + len, "0");  // null terminator
        asm_append(&data_section, buffer);
    } else {
        // ASCII-only: use quoted string with smart delimiter
        char* escaped = escape_string_for_asm(node->sabit_data.deger);
        int has_double_quote = (strchr(escaped, '"') != NULL);
        char delimiter = has_double_quote ? '\'' : '"';
        
        sprintf(buffer, "%s: db %c%s%c, 0", etiket, delimiter, escaped, delimiter);
        asm_append(&data_section, buffer);
    }

    // String'in adresini RAX'e yükle
    sprintf(buffer, "    mov rax, %s", etiket);
    asm_append(&text_section, buffer);
}

// Phase 5.9: String Interpolation
// Converts "Hello {name}, age {age}" into string_concat calls
void visit_InterpolatedString(ASTNode* node) {
    char buffer[512];
    
    if (node->interpolation_data.part_count == 0) {
        // Empty string
        sprintf(buffer, "    mov rax, 0");
        asm_append(&text_section, buffer);
        return;
    }
    
    // Process first part
    char* first_part = node->interpolation_data.parts[0];
    if (first_part[0] == '{' && first_part[strlen(first_part)-1] == '}') {
        // First part is a variable {var}
        char var_name[256];
        strncpy(var_name, first_part + 1, strlen(first_part) - 2);
        var_name[strlen(first_part) - 2] = '\0';
        
        // Load variable value
        Degisken* degisken = kapsam_bul(var_name);
        if (degisken == NULL) {
            fprintf(stderr, "ERROR: Undefined variable '%s' in interpolation\n", var_name);
            return;
        }
        
        sprintf(buffer, "    mov rax, %s", degisken->asm_adresi);
        asm_append(&text_section, buffer);
        
        // Check if variable is already a string
        char* tip = kapsam_degisken_tipi_bul(var_name);
        bool is_string_var = (tip != NULL && (strcmp(tip, "METIN") == 0 || strcmp(tip, "string") == 0));
        
        if (!is_string_var) {
            // Convert numeric to string using int_to_string
            sprintf(buffer, "    push rax");
            asm_append(&text_section, buffer);
            sprintf(buffer, "    mov rdi, rax");
            asm_append(&text_section, buffer);
            sprintf(buffer, "    call int_to_string  ; Convert numeric to string");
            asm_append(&text_section, buffer);
        }
        // Result in RAX (string pointer)
    } else {
        // First part is a literal
        // Create string literal
        char etiket[64];
        sprintf(etiket, "str_%d", metin_sayaci++);
        
        int has_utf8 = 0;
        for (const char* p = first_part; *p; p++) {
            if ((unsigned char)*p >= 128) {
                has_utf8 = 1;
                break;
            }
        }
        
        if (has_utf8) {
            sprintf(buffer, "%s: db ", etiket);
            int len = strlen(buffer);
            for (const char* p = first_part; *p; p++) {
                len += sprintf(buffer + len, "%d,", (unsigned char)*p);
            }
            sprintf(buffer + len, "0");
        } else {
            char* escaped = escape_string_for_asm(first_part);
            int has_quote = (strchr(escaped, '"') != NULL);
            char delim = has_quote ? '\'' : '"';
            sprintf(buffer, "%s: db %c%s%c, 0", etiket, delim, escaped, delim);
        }
        asm_append(&data_section, buffer);
        
        sprintf(buffer, "    mov rax, %s", etiket);
        asm_append(&text_section, buffer);
    }
    
    // Concatenate remaining parts
    for (int i = 1; i < node->interpolation_data.part_count; i++) {
        char* part = node->interpolation_data.parts[i];
        
        // Save current result (string so far)
        sprintf(buffer, "    push rax  ; Save accumulated string");
        asm_append(&text_section, buffer);
        
        if (part[0] == '{' && part[strlen(part)-1] == '}') {
            // This is a variable {var}
            char var_name[256];
            strncpy(var_name, part + 1, strlen(part) - 2);
            var_name[strlen(part) - 2] = '\0';
            
            // Load variable
            Degisken* degisken = kapsam_bul(var_name);
            if (degisken == NULL) {
                fprintf(stderr, "ERROR: Undefined variable '%s' in interpolation\n", var_name);
                return;
            }
            
            sprintf(buffer, "    mov rax, %s", degisken->asm_adresi);
            asm_append(&text_section, buffer);
            
            // Check if variable is already a string
            char* tip = kapsam_degisken_tipi_bul(var_name);
            bool is_string_var = (tip != NULL && (strcmp(tip, "METIN") == 0 || strcmp(tip, "string") == 0));
            
            if (!is_string_var) {
                // Convert numeric to string
                sprintf(buffer, "    mov rdi, rax");
                asm_append(&text_section, buffer);
                sprintf(buffer, "    call int_to_string  ; Convert to string");
                asm_append(&text_section, buffer);
            }
            // Result in RAX (string pointer)
        } else {
            // This is a literal string
            char etiket[64];
            sprintf(etiket, "str_%d", metin_sayaci++);
            
            int has_utf8 = 0;
            for (const char* p = part; *p; p++) {
                if ((unsigned char)*p >= 128) {
                    has_utf8 = 1;
                    break;
                }
            }
            
            if (has_utf8) {
                sprintf(buffer, "%s: db ", etiket);
                int len = strlen(buffer);
                for (const char* p = part; *p; p++) {
                    len += sprintf(buffer + len, "%d,", (unsigned char)*p);
                }
                sprintf(buffer + len, "0");
            } else {
                char* escaped = escape_string_for_asm(part);
                int has_quote = (strchr(escaped, '"') != NULL);
                char delim = has_quote ? '\'' : '"';
                sprintf(buffer, "%s: db %c%s%c, 0", etiket, delim, escaped, delim);
            }
            asm_append(&data_section, buffer);
            
            sprintf(buffer, "    mov rax, %s", etiket);
            asm_append(&text_section, buffer);
        }
        
        // RAX = new part (literal or converted variable)
        // Pop previous accumulated string into RDI
        sprintf(buffer, "    mov rsi, rax  ; Second string (new part)");
        asm_append(&text_section, buffer);
        sprintf(buffer, "    pop rdi       ; First string (accumulated)");
        asm_append(&text_section, buffer);
        
        // Call string_concat
        sprintf(buffer, "    call string_concat  ; Concatenate strings");
        asm_append(&text_section, buffer);
        // Result in RAX (concatenated string)
    }
    
    // Final result in RAX
}

// Phase 5.10: Import Module System
// Global variables for tracking imported files
char** imported_files = NULL;
int imported_files_count = 0;
int imported_files_capacity = 0;

// Check if a file has already been imported (prevent circular imports)
bool is_file_imported(const char* file_path) {
    for (int i = 0; i < imported_files_count; i++) {
        if (strcmp(imported_files[i], file_path) == 0) {
            return true;
        }
    }
    return false;
}

// Add file to imported list
void mark_file_imported(const char* file_path) {
    if (imported_files_capacity == 0) {
        imported_files_capacity = 10;
        imported_files = (char**)malloc(sizeof(char*) * imported_files_capacity);
    } else if (imported_files_count >= imported_files_capacity) {
        imported_files_capacity *= 2;
        imported_files = (char**)realloc(imported_files, sizeof(char*) * imported_files_capacity);
    }
    imported_files[imported_files_count++] = strdup(file_path);
}

void visit_Import(ASTNode* node) {
    char* file_path = node->import_data.file_path;
    
    // Comment in assembly
    char buffer[512];
    sprintf(buffer, "    ; --- IMPORT: %s ---", file_path);
    asm_append(&text_section, buffer);
    
    // Check for circular imports
    if (is_file_imported(file_path)) {
        sprintf(buffer, "    ; (already imported, skipping)");
        asm_append(&text_section, buffer);
        return;
    }
    
    // Mark as imported
    mark_file_imported(file_path);
    
    // Read the imported file
    FILE* file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "ERROR: Cannot open import file '%s'\n", file_path);
        exit(1);
    }
    
    // Read entire file into memory
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* import_source = (char*)malloc(file_size + 1);
    fread(import_source, 1, file_size, file);
    import_source[file_size] = '\0';
    fclose(file);
    
    // Save current lexer state
    const char* saved_source = source_code;
    int saved_position = current_position;
    int saved_line = current_line;
    int saved_column = current_column;
    Token* saved_current = current_token;
    
    // Initialize lexer with imported file
    initLexer(import_source);
    
    // Get first token
    current_token = getNextToken();
    
    // Parse the imported file (only top-level statements)
    while (current_token->type != TOKEN_EOF) {
        ASTNode* stmt = komut();
        if (stmt != NULL) {
            visit(stmt);
        }
    }
    
    // Restore lexer state
    free(import_source);
    initLexer(saved_source);
    current_position = saved_position;
    current_line = saved_line;
    current_column = saved_column;
    current_token = saved_current;
    
    sprintf(buffer, "    ; --- END IMPORT: %s ---", file_path);
    asm_append(&text_section, buffer);
}

void visit_DegiskenTanimlama(ASTNode* node) {
    char* degisken_adi = node->tanimlama_data.ad->value;
    char* degisken_tipi = node->tanimlama_data.tip->value;
    char buffer[256];

    sprintf(buffer, "    ; --- DegiskenTanimlama: %s ---", degisken_adi);
    asm_append(&text_section, buffer);

    // Global değişken mi? (scope level 0)
    if (current_scope_level == 0) {
        // Global değişken: .data section'a ekle
        sprintf(buffer, "global_%s: dq 0", degisken_adi);
        asm_append(&data_section, buffer);

        // Değişken için yığında değil, .data'da yer ayır
        char* global_adres = (char*)malloc(64);
        sprintf(global_adres, "[global_%s]", degisken_adi);

        // Scope'a ekle
        Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
        d->ad = strdup(degisken_adi);
        d->asm_adresi = global_adres;
        d->tip = strdup(degisken_tipi);
        d->scope_level = 0;
        d->is_global = true;
        d->is_const = node->tanimlama_data.is_const; // Phase 5.6

        // İfadeyi hesapla (sonuç RAX'te)
        visit(node->tanimlama_data.ifade);

        // Global değişkene ata
        sprintf(buffer, "    mov %s, rax", global_adres);
        asm_append(&text_section, buffer);
    } else {
        // Local değişken: Stack'te
        // 1. İfadeyi (sağ tarafı) hesapla (Sonuç RAX'e yüklenir)
        visit(node->tanimlama_data.ifade);

        // 2. Değişken için yığında (stack) yer ayır
        char* adres = kapsam_degisken_yer_ayir_with_const(degisken_adi, degisken_tipi, node->tanimlama_data.is_const);

        // 3. İfadenin sonucunu (RAX) yığındaki yeni adrese taşı
        sprintf(buffer, "    mov %s, rax", adres); // Örn: mov [rbp-8], rax
        asm_append(&text_section, buffer);
        free(adres); // kapsam_degisken_yer_ayir'dan gelen belleği temizle
    }
}

void visit_Degisken(ASTNode* node) {
    char* degisken_adi = node->degisken_data.ad->value;
    char buffer[128];

    // Phase 5.6: null keyword = 0
    if (strcmp(degisken_adi, "null") == 0) {
        asm_append(&text_section, "    xor rax, rax  ; null = 0");
        return;
    }

    // Phase 5.4: Önce enum değeri mi kontrol et
    int enum_val = enum_value_bul(degisken_adi);
    if (enum_val >= 0) {
        // Bu bir enum değeri, sabit olarak yükle
        sprintf(buffer, "    mov rax, %d  ; Enum value: %s", enum_val, degisken_adi);
        asm_append(&text_section, buffer);
        return;
    }

    char* adres = kapsam_degisken_adresi_bul(degisken_adi);

    // Değerin yığındaki adresinden (örn: [rbp-8]) RAX'e yükle
    sprintf(buffer, "    mov rax, %s", adres); // Örn: mov rax, [rbp-8]
    asm_append(&text_section, buffer);
    free(adres);
}

void visit_AtamaKomutu(ASTNode* node) {
    char* degisken_adi = node->atama_data.ad->value;
    char buffer[128];
    sprintf(buffer, "    ; --- AtamaKomutu: %s ---", degisken_adi);
    asm_append(&text_section, buffer);

    // Phase 5.6: Check if variable is const
    Degisken* var = kapsam_degisken_bul(degisken_adi);
    if (var && var->is_const) {
        fprintf(stderr, "\n╔════════════════════════════════════════════════════════════╗\n");
        fprintf(stderr, "║ HATA [Generator]: Const Değişken Hatası!                  ║\n");
        fprintf(stderr, "╚════════════════════════════════════════════════════════════╝\n\n");
        fprintf(stderr, "❌ Değişken: '%s'\n", degisken_adi);
        fprintf(stderr, "✗ Description: const değişkenlere yeni değer atanamaz!\n\n");
        exit(1);
    }

    // 1. İfadeyi (sağ tarafı) hesapla (Sonuç RAX'e yüklenir)
    visit(node->atama_data.ifade);

    // 2. Değişkenin adresini bul
    char* adres = kapsam_degisken_adresi_bul(degisken_adi);

    // 3. İfadenin sonucunu (RAX) değişkenin adresine taşı
    sprintf(buffer, "    mov %s, rax", adres); // Örn: mov [rbp-8], rax
    asm_append(&text_section, buffer);
    free(adres);
}

void visit_KosulKomutu(ASTNode* node) {
    // Benzersiz etiket kimlikleri al
    int etiket_degilse = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    char buffer[128];

    // 1. Koşul İfadesini (a == 10) hesapla
    // (visit_IkiliIslem sayesinde sonuç RAX'e 1 or 0 olarak yüklenecek)
    asm_append(&text_section, "    ; --- Kosul Komutu (IF) ---");
    visit(node->kosul_data.kosul);
    
    // 2. Koşulu kontrol et ve atla
    asm_append(&text_section, "    cmp rax, 0"); // Sonuç 0 mı (false) ?
    
    // Eğer 'DEĞİLSE' bloğu varsa oraya, yoksa 'SON' etiketine atla
    if (node->kosul_data.degilse_blok != NULL) {
        sprintf(buffer, "    je .L%d", etiket_degilse); // False ise DEĞİLSE'ye atla
    } else {
        sprintf(buffer, "    je .L%d", etiket_son); // False ise SON'a atla
    }
    asm_append(&text_section, buffer);

    // 3. 'İSE' Bloğunu (Doğruysa) çalıştır
    visit(node->kosul_data.ise_blok);
    
    // 'İSE' bloğu bittikten sonra 'SON' etiketine atla (DEĞİLSE'yi atlamak için)
    sprintf(buffer, "    jmp .L%d", etiket_son);
    asm_append(&text_section, buffer);

    // 4. 'DEĞİLSE' Bloğu (Varsa)
    if (node->kosul_data.degilse_blok != NULL) {
        sprintf(buffer, ".L%d:", etiket_degilse); // DEĞİLSE etiketi
        asm_append(&text_section, buffer);
        visit(node->kosul_data.degilse_blok);
    }
    
    // 5. 'SON' Etiketi
    sprintf(buffer, ".L%d:", etiket_son); // SON etiketi
    asm_append(&text_section, buffer);
}

void visit_DonguKomutu(ASTNode* node) {
    // Benzersiz etiket kimlikleri al
    int etiket_basla = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    char buffer[128];

    // 1. WHILE_BITIR için bu döngünün bitiş etiketini kaydet
    int onceki_aktif_dongu_son_etiketi = aktif_dongu_son_etiketi;
    int onceki_aktif_dongu_baslangic_etiketi = aktif_dongu_baslangic_etiketi;
    aktif_dongu_son_etiketi = etiket_son;
    aktif_dongu_baslangic_etiketi = etiket_basla;

    // 2. Döngü Başlangıç Etiketi
    sprintf(buffer, ".L%d:", etiket_basla); // .L_BASLA
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- While Loop ---");

    // 3. If there's a condition, evaluate it
    if (node->dongu_data.kosul != NULL) {
        visit(node->dongu_data.kosul);  // Condition result in rax
        asm_append(&text_section, "    test rax, rax");
        sprintf(buffer, "    jz .L%d", etiket_son);  // Jump to end if false
        asm_append(&text_section, buffer);
    }

    // 4. Döngü Gövdesi
    visit(node->dongu_data.govde);
    
    // 5. Gövde bittikten sonra başa atla
    sprintf(buffer, "    jmp .L%d", etiket_basla); // jmp .L_BASLA
    asm_append(&text_section, buffer);

    // 6. Döngü Bitiş Etiketi
    sprintf(buffer, ".L%d:", etiket_son); // .L_SON
    asm_append(&text_section, buffer);

    // 7. Döngü bitti, eski etiketi geri yükle (iç içe döngüler için)
    aktif_dongu_son_etiketi = onceki_aktif_dongu_son_etiketi;
    aktif_dongu_baslangic_etiketi = onceki_aktif_dongu_baslangic_etiketi;
}

void visit_ForKomutu(ASTNode* node) {
    // for i = 0 to 10 [step 2]
    
    // 1. Loop variable initialization
    char* var_name = node->for_data.degisken->value;
    
    // Allocate variable in scope
    char* var_addr = kapsam_degisken_yer_ayir(var_name, "SAYISAL");
    
    // Initialize: i = start
    visit(node->for_data.baslangic);  // Start value in rax
    char buffer[256];
    sprintf(buffer, "    mov %s, rax", var_addr);
    asm_append(&text_section, buffer);
    
    // 2. Labels
    int etiket_basla = etiket_sayaci++;
    int etiket_continue = etiket_sayaci++;  // Phase 5.8: For continue target
    int etiket_son = etiket_sayaci++;
    
    int onceki_aktif_dongu_son_etiketi = aktif_dongu_son_etiketi;
    int onceki_aktif_dongu_baslangic_etiketi = aktif_dongu_baslangic_etiketi;
    aktif_dongu_son_etiketi = etiket_son;
    aktif_dongu_baslangic_etiketi = etiket_continue;  // Continue jumps to increment, not loop start
    
    // 3. Loop start
    sprintf(buffer, ".L%d:", etiket_basla);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- For Loop ---");
    
    // 4. Check condition: i <= end
    sprintf(buffer, "    mov rax, %s", var_addr);  // Load i
    asm_append(&text_section, buffer);
    
    visit(node->for_data.bitis);  // End value in rax (saved to stack)
    asm_append(&text_section, "    mov rbx, rax");  // End in rbx
    sprintf(buffer, "    mov rax, %s", var_addr);  // i back in rax
    asm_append(&text_section, buffer);
    
    asm_append(&text_section, "    cmp rax, rbx");  // Compare i with end
    sprintf(buffer, "    jg .L%d", etiket_son);  // Jump if i > end
    asm_append(&text_section, buffer);
    
    // 5. Loop body
    visit(node->for_data.govde);
    
    // 6. Continue target label - increment happens here
    sprintf(buffer, ".L%d:", etiket_continue);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- For Loop Continue/Increment ---");
    
    // 7. Increment: i = i + step (default 1)
    if (node->for_data.adim != NULL) {
        visit(node->for_data.adim);  // Step in rax
        asm_append(&text_section, "    mov rcx, rax");  // Save step
        sprintf(buffer, "    mov rax, %s", var_addr);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    add rax, rcx");
        sprintf(buffer, "    mov %s, rax", var_addr);
        asm_append(&text_section, buffer);
    } else {
        sprintf(buffer, "    mov rax, %s", var_addr);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    inc rax");
        sprintf(buffer, "    mov %s, rax", var_addr);
        asm_append(&text_section, buffer);
    }
    
    // 8. Jump back to condition check (loop start)
    sprintf(buffer, "    jmp .L%d", etiket_basla);
    asm_append(&text_section, buffer);
    
    // 9. Loop end
    sprintf(buffer, ".L%d:", etiket_son);
    asm_append(&text_section, buffer);
    
    aktif_dongu_son_etiketi = onceki_aktif_dongu_son_etiketi;
    aktif_dongu_baslangic_etiketi = onceki_aktif_dongu_baslangic_etiketi;
}

// Phase 5.8: For-In-Range Loop
// for i in range(start, end, step)
void visit_ForInRange(ASTNode* node) {
    char* var_name = node->for_in_range_data.degisken->value;
    
    // Allocate loop variable in scope
    char* var_addr = kapsam_degisken_yer_ayir(var_name, "SAYISAL");
    
    // Initialize: i = start
    visit(node->for_in_range_data.start);  // Start value in rax
    char buffer[256];
    sprintf(buffer, "    mov %s, rax", var_addr);
    asm_append(&text_section, buffer);
    
    // Calculate and store end value (we need it for comparison)
    visit(node->for_in_range_data.end);  // End value in rax
    asm_append(&text_section, "    push rax  ; Save end value");
    
    // Calculate and store step value (default 1 if NULL)
    if (node->for_in_range_data.step != NULL) {
        visit(node->for_in_range_data.step);  // Step in rax
        asm_append(&text_section, "    push rax  ; Save step value");
    } else {
        asm_append(&text_section, "    push 1  ; Default step = 1");
    }
    
    // Labels
    int etiket_basla = etiket_sayaci++;
    int etiket_continue = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    int onceki_aktif_dongu_son_etiketi = aktif_dongu_son_etiketi;
    int onceki_aktif_dongu_baslangic_etiketi = aktif_dongu_baslangic_etiketi;
    aktif_dongu_son_etiketi = etiket_son;
    aktif_dongu_baslangic_etiketi = etiket_continue;
    
    // Loop start
    sprintf(buffer, ".L%d:", etiket_basla);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- For-In-Range Loop ---");
    
    // Check condition: i < end (range is exclusive like Python)
    sprintf(buffer, "    mov rax, %s", var_addr);  // Load i
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    mov rbx, [rsp+8]  ; Load end value from stack");
    asm_append(&text_section, "    cmp rax, rbx");  // Compare i with end
    sprintf(buffer, "    jge .L%d", etiket_son);  // Jump if i >= end (exclusive)
    asm_append(&text_section, buffer);
    
    // Loop body
    visit(node->for_in_range_data.govde);
    
    // Continue target - increment happens here
    sprintf(buffer, ".L%d:", etiket_continue);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- Range Loop Continue/Increment ---");
    
    // Increment: i = i + step
    sprintf(buffer, "    mov rax, %s", var_addr);  // Load i
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    add rax, [rsp]  ; Add step");
    sprintf(buffer, "    mov %s, rax", var_addr);  // Store updated i
    asm_append(&text_section, buffer);
    
    // Jump back to condition check
    sprintf(buffer, "    jmp .L%d", etiket_basla);
    asm_append(&text_section, buffer);
    
    // Loop end
    sprintf(buffer, ".L%d:", etiket_son);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    add rsp, 16  ; Clean up step and end from stack");
    
    aktif_dongu_son_etiketi = onceki_aktif_dongu_son_etiketi;
    aktif_dongu_baslangic_etiketi = onceki_aktif_dongu_baslangic_etiketi;
}

void visit_DonguBitirKomutu(ASTNode* node) {
    if (aktif_dongu_son_etiketi == -1) {
        fprintf(stderr, "HATA [Generator]: 'WHILE_BITIR' bir döngü içinde değil.\n");
        exit(1);
    }
    
    char buffer[128];
    sprintf(buffer, "    jmp .L%d", aktif_dongu_son_etiketi); // jmp .L_SON
    asm_append(&text_section, "    ; --- DonguBitir Komutu ---");
    asm_append(&text_section, buffer);
}

// Phase 5.8: Continue implementation
void visit_DonguDevamKomutu(ASTNode* node) {
    if (aktif_dongu_baslangic_etiketi == -1) {
        fprintf(stderr, "HATA [Generator]: 'continue' bir döngü içinde değil.\n");
        exit(1);
    }
    
    char buffer[128];
    sprintf(buffer, "    jmp .L%d", aktif_dongu_baslangic_etiketi); // jmp back to loop start
    asm_append(&text_section, "    ; --- Continue Komutu ---");
    asm_append(&text_section, buffer);
}

// Phase 5.8: For-each loop (for item in array)
void visit_ForEach(ASTNode* node) {
    char* var_name = node->for_each_data.degisken->value;
    char* array_name = node->for_each_data.array_adi->value;
    char buffer[256];
    
    sprintf(buffer, "    ; --- For-Each Loop: %s in %s ---", var_name, array_name);
    asm_append(&text_section, buffer);
    
    // Find array in scope
    Degisken* array_var = NULL;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (strcmp(kapsam_haritasi[i].ad, array_name) == 0) {
            array_var = &kapsam_haritasi[i];
            break;
        }
    }
    
    if (array_var == NULL) {
        fprintf(stderr, "HATA [Generator]: Array '%s' bulunamadı.\n", array_name);
        exit(1);
    }
    
    if (array_var->array_boyut == 0) {
        fprintf(stderr, "HATA [Generator]: '%s' bir array değil veya boyutu bilinmiyor.\n", array_name);
        exit(1);
    }
    
    int array_size = array_var->array_boyut;
    sprintf(buffer, "    ; Array size: %d elements", array_size);
    asm_append(&text_section, buffer);
    
    // Allocate loop variable (will hold array element value)
    char* var_addr = kapsam_degisken_yer_ayir(var_name, "SAYISAL");
    
    // Allocate index counter variable (internal, not visible to user)
    char internal_index_var[64];
    sprintf(internal_index_var, "__foreach_idx_%s", var_name);
    char* index_addr = kapsam_degisken_yer_ayir(internal_index_var, "SAYISAL");
    
    // Initialize index = 0
    asm_append(&text_section, "    mov rax, 0  ; Initialize loop index");
    sprintf(buffer, "    mov %s, rax", index_addr);
    asm_append(&text_section, buffer);
    
    // Get array base address
    char* array_base = kapsam_degisken_adresi_bul(array_name);
    
    // Labels
    int etiket_basla = etiket_sayaci++;
    int etiket_continue = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    int onceki_aktif_dongu_son_etiketi = aktif_dongu_son_etiketi;
    int onceki_aktif_dongu_baslangic_etiketi = aktif_dongu_baslangic_etiketi;
    aktif_dongu_son_etiketi = etiket_son;
    aktif_dongu_baslangic_etiketi = etiket_continue;
    
    // Loop start
    sprintf(buffer, ".L%d:", etiket_basla);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- For-Each Loop Start ---");
    
    // Check condition: index < array_size
    sprintf(buffer, "    mov rax, %s", index_addr);  // Load index
    asm_append(&text_section, buffer);
    sprintf(buffer, "    cmp rax, %d", array_size);  // Compare with size
    asm_append(&text_section, buffer);
    sprintf(buffer, "    jge .L%d", etiket_son);  // Jump if index >= size
    asm_append(&text_section, buffer);
    
    // Load array[index] into loop variable
    sprintf(buffer, "    mov rax, %s", index_addr);  // Load index
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    imul rax, 8  ; index * 8");
    sprintf(buffer, "    lea rbx, %s  ; Array base", array_base);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    add rbx, rax  ; base + offset");
    asm_append(&text_section, "    mov rax, [rbx]  ; Load array[index]");
    sprintf(buffer, "    mov %s, rax  ; Store in loop variable", var_addr);
    asm_append(&text_section, buffer);
    
    free(array_base);
    
    // Loop body
    visit(node->for_each_data.govde);
    
    // Continue target - increment happens here
    sprintf(buffer, ".L%d:", etiket_continue);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- For-Each Continue/Increment ---");
    
    // Increment index: index = index + 1
    sprintf(buffer, "    mov rax, %s", index_addr);  // Load index
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    add rax, 1");
    sprintf(buffer, "    mov %s, rax", index_addr);  // Store updated index
    asm_append(&text_section, buffer);
    
    // Jump back to condition check
    sprintf(buffer, "    jmp .L%d", etiket_basla);
    asm_append(&text_section, buffer);
    
    // Loop end
    sprintf(buffer, ".L%d:", etiket_son);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- For-Each Loop End ---");
    
    aktif_dongu_son_etiketi = onceki_aktif_dongu_son_etiketi;
    aktif_dongu_baslangic_etiketi = onceki_aktif_dongu_baslangic_etiketi;
}

// Phase 5.8: Stop (debugging breakpoint - int3)
void visit_Stop(ASTNode* node) {
    asm_append(&text_section, "    ; --- Stop (Debug Breakpoint) ---");
    asm_append(&text_section, "    int3");  // x86-64 breakpoint instruction
}

// Phase 5.8: Goto
void visit_Goto(ASTNode* node) {
    char* label_adi = node->goto_data.label_adi->value;
    char buffer[256];
    sprintf(buffer, "    jmp __label_%s", label_adi);
    asm_append(&text_section, "    ; --- Goto ---");
    asm_append(&text_section, buffer);
}

// Phase 5.8: Label definition
void visit_Label(ASTNode* node) {
    char* label_adi = node->label_data.label_adi->value;
    char buffer[256];
    sprintf(buffer, "__label_%s:", label_adi);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- Label ---");
}

// Phase 5.8: Ternary operator (condition ? true_val : false_val)
void visit_Ternary(ASTNode* node) {
    int etiket_false = etiket_sayaci++;
    int etiket_end = etiket_sayaci++;
    char buffer[256];

    asm_append(&text_section, "    ; --- Ternary Operator ---");
    
    // Evaluate condition
    visit(node->ternary_data.kosul);
    asm_append(&text_section, "    test rax, rax");
    sprintf(buffer, "    jz .L%d", etiket_false);  // Jump to false_val if condition is 0
    asm_append(&text_section, buffer);
    
    // True branch
    visit(node->ternary_data.dogru_deger);  // Result in rax
    sprintf(buffer, "    jmp .L%d", etiket_end);  // Skip false branch
    asm_append(&text_section, buffer);
    
    // False branch
    sprintf(buffer, ".L%d:", etiket_false);
    asm_append(&text_section, buffer);
    visit(node->ternary_data.yanlis_deger);  // Result in rax
    
    // End
    sprintf(buffer, ".L%d:", etiket_end);
    asm_append(&text_section, buffer);
}

void visit_IslecTanimlama(ASTNode* node) {
    char* islec_adi = node->islec_tanimlama_data.ad->value;
    char buffer[128];

    // NOTE: Function signature already registered in pre-scan phase
    // No need to register here again

    // ===== Generate assembly code for function =====

    // Fonksiyon için yeni kapsam aç
    int onceki_degisken_sayisi = kapsam_degisken_sayisi;
    int onceki_yigin_ofseti = kapsam_yigin_ofseti;

    // Global değişkenleri koru, sadece stack offset'i sıfırla
    // Global değişkenleri say (scope_level == 0)
    int global_sayisi = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            global_sayisi++;
        }
    }

    // Global değişkenleri başa taşı ve sayıyı ayarla
    int new_idx = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            if (i != new_idx) {
                kapsam_haritasi[new_idx] = kapsam_haritasi[i];
            }
            new_idx++;
        }
    }
    kapsam_degisken_sayisi = global_sayisi;
    kapsam_yigin_ofseti = 0;  // Fonksiyon stack'i sıfırdan başlar
    
    // Phase 7.6: Enter function scope so variables are local, not global
    kapsam_gir();

    // 1. Fonksiyon Etiketini Tanımla
    // Special handling for main - rename to mlp_main to avoid conflict with C entry point
    const char* label_name = (strcmp(islec_adi, "main") == 0) ? "mlp_main" : islec_adi;
    sprintf(buffer, "%s:", label_name);
    asm_append(&text_section, buffer);
    sprintf(buffer, "    ; --- Islec Tanimlama: %s ---", islec_adi);
    asm_append(&text_section, buffer);

    // 2. Fonksiyon Girişi (Prolog)
    asm_append(&text_section, "    push rbp");
    asm_append(&text_section, "    mov rbp, rsp");
    asm_append(&text_section, "    sub rsp, 256"); // ✅ Yeterli stack alanı (32 değişkene kadar)

    // 3. Parametreleri kaydet
    int param_sayisi = node->islec_tanimlama_data.parametre_sayisi;
    for (int i = 0; i < param_sayisi; i++) {
        char* param_adi = node->islec_tanimlama_data.parametreler[i]->value;
        
        // Phase 7.6: Check if this parameter is an array
        int is_array = 0;
        if (node->islec_tanimlama_data.parametre_is_array != NULL) {
            is_array = node->islec_tanimlama_data.parametre_is_array[i];
        }
        
        // Arrays are passed as pointers (already in register)
        // For array parameters, we store the pointer directly
        char* tip = is_array ? "ARRAY_SAYISAL" : "SAYISAL";
        char* adres = kapsam_degisken_yer_ayir(param_adi, tip);
        sprintf(buffer, "    mov %s, %s", adres, arg_registerleri[i]);
        asm_append(&text_section, buffer);
        free(adres);
    }

    // 4. Fonksiyon Gövdesi
    visit(node->islec_tanimlama_data.govde);

    // 5. Implicit return if no explicit return (function epilog)
    asm_append(&text_section, "    ; --- Implicit Return (Function Epilog) ---");
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");

    // Phase 7.6: Exit function scope
    kapsam_cik();

    // 6. Önceki kapsamı geri yükle
    kapsam_degisken_sayisi = onceki_degisken_sayisi;
    kapsam_yigin_ofseti = onceki_yigin_ofseti;
}

// Phase 7.1: Lambda Expression Code Generation
// Lambda counter for generating unique labels
static int lambda_counter = 0;
static int await_label_counter = 0; // Phase 8: Await loop labels
static int in_async_function = 0;   // Phase 8: Flag for async function context
static char* async_promise_var = NULL; // Phase 8: Current async function's promise variable
static char* async_function_name = NULL; // Phase 8.7: Current async function name for continuations
static int async_state_counter = 0;    // Phase 8.5: State machine await counter
static int async_state_machine_mode = 0; // Phase 8.5: 1 if generating state machine code
static char* async_state_var = NULL;     // Phase 8.5: Current state variable address

// Phase 7.9: Closure support - Find free variables in AST
void find_free_variables(ASTNode* node, char** free_vars, int* free_count, 
                         Token** lambda_params, int param_count) {
    if (node == NULL) return;
    
    // Check if this is a variable reference
    if (node->type == AST_DEGISKEN) {
        char* var_name = node->degisken_data.ad->value;
        
        // Check if it's a lambda parameter
        bool is_param = false;
        for (int i = 0; i < param_count; i++) {
            if (strcmp(lambda_params[i]->value, var_name) == 0) {
                is_param = true;
                break;
            }
        }
        
        // If not a parameter, check if it's an outer variable
        if (!is_param) {
            Degisken* var = kapsam_degisken_bul(var_name);
            if (var != NULL) {
                // This is potentially a captured variable!
                // Check if already in list
                bool already_captured = false;
                for (int i = 0; i < *free_count; i++) {
                    if (strcmp(free_vars[i], var_name) == 0) {
                        already_captured = true;
                        break;
                    }
                }
                
                if (!already_captured) {
                    free_vars[*free_count] = strdup(var_name);
                    (*free_count)++;
                }
            }
        }
        return;
    }
    
    // Recursively traverse AST
    switch (node->type) {
        case AST_IKILI_ISLEM:
            find_free_variables(node->ikili_islem_data.sol, free_vars, free_count, lambda_params, param_count);
            find_free_variables(node->ikili_islem_data.sag, free_vars, free_count, lambda_params, param_count);
            break;
        case AST_ISLEC_CAGIRMA:
            for (int i = 0; i < node->islec_cagirma_data.arguman_sayisi; i++) {
                find_free_variables(node->islec_cagirma_data.argumanlar[i], free_vars, free_count, lambda_params, param_count);
            }
            break;
        // Add more cases as needed
        default:
            break;
    }
}

void visit_Lambda(ASTNode* node) {
    char buffer[512];
    
    // Phase 7.9: Detect captured variables at codegen time (when scope is ready)
    char** captured_vars = NULL;
    int captured_count = 0;
    
    if (node->lambda_data.captured_vars == NULL) {
        // Detect captured variables now (first time only)
        captured_vars = (char**)malloc(sizeof(char*) * 50);
        find_free_variables(node->lambda_data.govde, captured_vars, &captured_count,
                           node->lambda_data.parametreler, node->lambda_data.parametre_sayisi);
        node->lambda_data.captured_vars = captured_vars;
        node->lambda_data.captured_count = captured_count;
    } else {
        // Already detected (use existing values)
        captured_vars = node->lambda_data.captured_vars;
        captured_count = node->lambda_data.captured_count;
    }
    
    // Closure struct approach:
    // If lambda captures variables, we allocate a closure struct on heap
    // containing the captured variable values, and pass it as hidden parameter
    // Closure struct approach:
    // If lambda captures variables, we allocate a closure struct on heap
    // containing the captured variable values, and pass it as hidden parameter
    
    // Generate unique label for this lambda
    char lambda_label[64];
    sprintf(lambda_label, "__lambda_%d", lambda_counter++);
    
    char closure_label[64];
    sprintf(closure_label, "__closure_%d", lambda_counter - 1);
    
    // =========================
    // CLOSURE GENERATION (ALWAYS - even for non-capturing lambdas)
    // =========================
    // Uniform calling convention: ALL lambdas are closures
    // Non-capturing lambda = closure with only function pointer
    sprintf(buffer, "    ; --- Closure Allocation (captures: %d) ---", captured_count);
    asm_append(&text_section, buffer);
    
    // 1. Allocate closure struct on heap
    // Closure struct layout:
    //   [0-7]   = function pointer (8 bytes)
    //   [8-15]  = captured_var_1 (8 bytes)  [only if captured_count > 0]
    //   [16-23] = captured_var_2 (8 bytes)  [only if captured_count > 0]
    //   ...
    int closure_size = 8 + (captured_count * 8);  // function_ptr + vars
    sprintf(buffer, "    mov rdi, %d  ; Closure size (func_ptr + %d vars)", 
            closure_size, captured_count);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    call malloc");
    sprintf(buffer, "    mov [rel %s], rax  ; Save closure base", closure_label);
    asm_append(&text_section, buffer);
    
    // 2. Store function pointer at offset 0
    sprintf(buffer, "    lea rbx, [rel %s]  ; Get lambda function address", lambda_label);
    asm_append(&text_section, buffer);
    sprintf(buffer, "    mov rcx, [rel %s]  ; Load closure base", closure_label);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    mov [rcx], rbx  ; Store function pointer at offset 0");
    
    // 3. Copy captured variables into closure struct (starting at offset 8)
    if (captured_count > 0) {
        for (int i = 0; i < captured_count; i++) {
            char* var_name = node->lambda_data.captured_vars[i];
            Degisken* var = kapsam_degisken_bul(var_name);
            
            if (var == NULL) {
                fprintf(stderr, "ERROR: Captured variable '%s' not found in scope!\n", var_name);
                exit(1);
            }
            
            sprintf(buffer, "    ; Capture '%s' at offset %d", var_name, 8 + (i * 8));
            asm_append(&text_section, buffer);
            
            sprintf(buffer, "    mov rbx, %s  ; Load variable", var->asm_adresi);
            asm_append(&text_section, buffer);
            
            sprintf(buffer, "    mov rcx, [rel %s]  ; Load closure base", closure_label);
            asm_append(&text_section, buffer);
            
            sprintf(buffer, "    mov [rcx+%d], rbx  ; Store in closure", 8 + (i * 8));
            asm_append(&text_section, buffer);
        }
    }
    
    // 4. Return closure pointer (RAX) - ALWAYS (uniform calling convention)
    sprintf(buffer, "    mov rax, [rel %s]  ; Lambda returns closure pointer", closure_label);
    asm_append(&text_section, buffer);
    
    // DON'T jump - lambda is deferred, not inline!
    // Lambda function will be appended to lambda_section at the end
    
    // =========================
    // LAMBDA FUNCTION DEFINITION (DEFERRED TO lambda_section)
    // =========================
    
    // Add closure storage to data section (ALWAYS)
    sprintf(buffer, "%s: dq 0  ; Closure pointer storage", closure_label);
    asm_append(&data_section, buffer);
    
    // Save current scope context
    int onceki_degisken_sayisi = kapsam_degisken_sayisi;
    int onceki_yigin_ofseti = kapsam_yigin_ofseti;
    
    // Keep globals, reset locals (like normal functions do)
    int global_sayisi = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            global_sayisi++;
        }
    }
    
    // Move globals to beginning
    int new_idx = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            if (i != new_idx) {
                kapsam_haritasi[new_idx] = kapsam_haritasi[i];
            }
            new_idx++;
        }
    }
    kapsam_degisken_sayisi = global_sayisi;
    kapsam_yigin_ofseti = 0;
    kapsam_gir();
    
    // Generate lambda function code in DEFERRED lambda_section
    sprintf(buffer, "\n%s:", lambda_label);
    asm_append(&lambda_section, buffer);
    
    if (captured_count > 0) {
        sprintf(buffer, "    ; --- Lambda with Closure (params: %d, captured: %d) ---", 
                node->lambda_data.parametre_sayisi, captured_count);
    } else {
        sprintf(buffer, "    ; --- Lambda Expression (params: %d) ---", 
                node->lambda_data.parametre_sayisi);
    }
    asm_append(&lambda_section, buffer);
    
    // Function prolog
    asm_append(&lambda_section, "    push rbp");
    asm_append(&lambda_section, "    mov rbp, rsp");
    asm_append(&lambda_section, "    sub rsp, 256");
    
    // Save parameters to stack
    int param_count = node->lambda_data.parametre_sayisi;
    
    // UNIFORM CALLING CONVENTION: First parameter (rdi) is ALWAYS closure pointer
    // (even for non-capturing lambdas, closure just contains function pointer)
    int param_offset = 1;  // Always 1 - closure pointer is first param
    
    // Save closure pointer (always, even if not used for captured vars)
    char* closure_var = kapsam_degisken_yer_ayir("__closure", "SAYISAL");
    sprintf(buffer, "    mov %s, rdi  ; Save closure pointer", closure_var);
    asm_append(&lambda_section, buffer);
    free(closure_var);
    
    for (int i = 0; i < param_count; i++) {
        char* param_name = node->lambda_data.parametreler[i]->value;
        char* adres = kapsam_degisken_yer_ayir(param_name, "SAYISAL");
        sprintf(buffer, "    mov %s, %s  ; Parameter '%s'", 
                adres, arg_registerleri[i + param_offset], param_name);
        asm_append(&lambda_section, buffer);
        free(adres);
    }
    
    // Add captured variables to scope (from closure struct)
    if (captured_count > 0) {
        Degisken* closure_var = kapsam_degisken_bul("__closure");
        
        for (int i = 0; i < captured_count; i++) {
            char* var_name = node->lambda_data.captured_vars[i];
            
            // Allocate space for captured variable on stack
            char* var_addr = kapsam_degisken_yer_ayir(var_name, "SAYISAL");
            
            // Load from closure struct (offset 8 + i*8)
            sprintf(buffer, "    ; Load captured '%s' from closure", var_name);
            asm_append(&lambda_section, buffer);
            
            sprintf(buffer, "    mov rbx, %s  ; Closure pointer", closure_var->asm_adresi);
            asm_append(&lambda_section, buffer);
            
            sprintf(buffer, "    mov rax, [rbx+%d]  ; Load from offset %d", 8 + (i * 8), 8 + (i * 8));
            asm_append(&lambda_section, buffer);
            
            sprintf(buffer, "    mov %s, rax  ; Store locally as '%s'", var_addr, var_name);
            asm_append(&lambda_section, buffer);
            
            free(var_addr);
        }
    }
    
    // CRITICAL: Swap text_section to lambda_section during body evaluation
    // This ensures body expression generates code into lambda_section
    // IMPORTANT: Swap only the pointers/state, not structs (to avoid stale pointers after realloc)
    
    // Save text_section state
    char* saved_text_code = text_section.code;
    size_t saved_text_size = text_section.size;
    size_t saved_text_capacity = text_section.capacity;
    
    // Move lambda_section state to text_section
    text_section.code = lambda_section.code;
    text_section.size = lambda_section.size;
    text_section.capacity = lambda_section.capacity;
    
    // Evaluate body expression (generates into text_section which now points to lambda_section's buffer)
    visit(node->lambda_data.govde);
    
    // Save the updated lambda_section state (text_section may have been reallocated)
    lambda_section.code = text_section.code;
    lambda_section.size = text_section.size;
    lambda_section.capacity = text_section.capacity;
    
    // Restore text_section state
    text_section.code = saved_text_code;
    text_section.size = saved_text_size;
    text_section.capacity = saved_text_capacity;
    
    // Return value is already in RAX
    asm_append(&lambda_section, "    ; --- Lambda Return ---");
    asm_append(&lambda_section, "    mov rsp, rbp");
    asm_append(&lambda_section, "    pop rbp");
    asm_append(&lambda_section, "    ret");
    
    // No skip label needed - lambda is deferred!
    
    // Restore scope
    kapsam_cik();
    // DON'T restore kapsam_degisken_sayisi - kapsam_cik() already updated it correctly!
    // Restoring would make freed pointers in kapsam_haritasi appear valid → double free!
    // Only restore stack offset
    kapsam_yigin_ofseti = onceki_yigin_ofseti;
}

// Phase 8.5: Count await expressions in AST (for state machine)
int count_awaits_in_node(ASTNode* node) {
    if (node == NULL) return 0;
    
    int count = 0;
    
    if (node->type == AST_AWAIT_EXPR) {
        count = 1;
    }
    
    // Traverse child nodes based on type
    switch (node->type) {
        case AST_BLOK:
            for (int i = 0; i < node->blok_data.sayisi; i++) {
                count += count_awaits_in_node(node->blok_data.komutlar[i]);
            }
            break;
            
        case AST_IF_STATEMENT_KOMUTU:
            count += count_awaits_in_node(node->kosul_data.kosul);
            count += count_awaits_in_node(node->kosul_data.ise_blok);
            if (node->kosul_data.degilse_blok) {
                count += count_awaits_in_node(node->kosul_data.degilse_blok);
            }
            break;
            
        case AST_WHILE_LOOP_KOMUTU:
            count += count_awaits_in_node(node->dongu_data.kosul);
            count += count_awaits_in_node(node->dongu_data.govde);
            break;
            
        case AST_FOR_KOMUTU:
            if (node->for_data.baslangic) {
                count += count_awaits_in_node(node->for_data.baslangic);
            }
            if (node->for_data.bitis) {
                count += count_awaits_in_node(node->for_data.bitis);
            }
            if (node->for_data.adim) {
                count += count_awaits_in_node(node->for_data.adim);
            }
            count += count_awaits_in_node(node->for_data.govde);
            break;
            
        case AST_RETURN_STATEMENT_KOMUTU:
            if (node->tek_ifade_data.ifade) {
                count += count_awaits_in_node(node->tek_ifade_data.ifade);
            }
            break;
            
        case AST_IKILI_ISLEM:
            count += count_awaits_in_node(node->ikili_islem_data.sol);
            count += count_awaits_in_node(node->ikili_islem_data.sag);
            break;
            
        case AST_AWAIT_EXPR:
            count++;  // Count this await
            count += count_awaits_in_node(node->await_data.ifade);
            break;
            
        case AST_ISLEC_CAGIRMA:
            for (int i = 0; i < node->islec_cagirma_data.arguman_sayisi; i++) {
                count += count_awaits_in_node(node->islec_cagirma_data.argumanlar[i]);
            }
            break;
            
        case AST_VARIABLE_DECLARATION:
            if (node->tanimlama_data.ifade) {
                count += count_awaits_in_node(node->tanimlama_data.ifade);
            }
            break;
            
        case AST_ASSIGNMENT_KOMUTU:
            count += count_awaits_in_node(node->atama_data.ifade);
            break;
            
        default:
            // Leaf nodes or nodes without await children
            break;
    }
    
    return count;
}

// Phase 8: Async Function Code Generation
void visit_AsyncFunction(ASTNode* node) {
    char buffer[256];
    char* func_name = node->async_function_data.ad->value;
    int param_count = node->async_function_data.parametre_sayisi;
    
    // Enter function scope
    int onceki_degisken_sayisi = kapsam_degisken_sayisi;
    int onceki_yigin_ofseti = kapsam_yigin_ofseti;
    
    // Keep globals, reset locals
    int global_sayisi = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            global_sayisi++;
        }
    }
    int new_idx = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            if (i != new_idx) {
                kapsam_haritasi[new_idx] = kapsam_haritasi[i];
            }
            new_idx++;
        }
    }
    kapsam_degisken_sayisi = global_sayisi;
    kapsam_yigin_ofseti = 0;
    kapsam_gir();
    
    // Special handling for main - rename to mlp_main
    const char* label_name = (strcmp(func_name, "main") == 0) ? "mlp_main" : func_name;
    sprintf(buffer, "\n%s:", label_name);
    asm_append(&text_section, buffer);
    sprintf(buffer, "    ; --- Async Function '%s' (params: %d) ---", func_name, param_count);
    asm_append(&text_section, buffer);
    
    // Function prolog
    asm_append(&text_section, "    push rbp");
    asm_append(&text_section, "    mov rbp, rsp");
    asm_append(&text_section, "    sub rsp, 256");
    
    // Save parameters to stack
    for (int i = 0; i < param_count && i < 6; i++) {
        char* param_name = node->async_function_data.parametreler[i]->value;
        char* adres = kapsam_degisken_yer_ayir(param_name, "SAYISAL");
        sprintf(buffer, "    mov %s, %s  ; Parameter '%s'", 
                adres, arg_registerleri[i], param_name);
        asm_append(&text_section, buffer);
        free(adres);
    }
    
    // Create promise at start of async function
    asm_append(&text_section, "    ; Create promise for async function");
    asm_append(&text_section, "    call promise_create");
    
    // Save promise pointer
    char* promise_var = kapsam_degisken_yer_ayir("__promise", "SAYISAL");
    sprintf(buffer, "    mov %s, rax  ; Save promise pointer", promise_var);
    asm_append(&text_section, buffer);
    
    // Phase 8.5: Check if state machine is needed (has await expressions)
    int await_count = count_awaits_in_node(node->async_function_data.govde);
    
    if (await_count == 0) {
        // No awaits - simple synchronous async function
        asm_append(&text_section, "    ; No await points - synchronous execution");
        
        // Set async context flags
        in_async_function = 1;
        async_promise_var = promise_var;
        
        // Execute function body
        visit(node->async_function_data.govde);
        
        // Clear async context
        in_async_function = 0;
        async_promise_var = NULL;
        
        // Fallthrough case handled below
    } else {
        // Has awaits - use blocking await (Phase 8.6)
        sprintf(buffer, "    ; Async function with %d await point(s) - blocking await", await_count);
        asm_append(&text_section, buffer);
        
        // Phase 8.6: Use simple blocking await for now
        // State machine infrastructure ready but waiting for event loop integration
        asm_append(&text_section, "    ; PHASE 8.7: Infrastructure ready, using blocking await");
        
        // Set async context flags
        in_async_function = 1;
        async_promise_var = promise_var;
        async_function_name = strdup(label_name);  // PHASE 8.7: Track function name
        async_state_counter = 0;
        async_state_machine_mode = 0;  // PHASE 8.7: Disabled - using blocking await
        
        // Execute function body with blocking await
        visit(node->async_function_data.govde);
        
        // Clear async context
        in_async_function = 0;
        async_promise_var = NULL;
        if (async_function_name) {
            free(async_function_name);
            async_function_name = NULL;
        }
    }
    
    // Fallthrough (no explicit return)
    // Resolve promise with implicit return value (RAX may be undefined)
    asm_append(&text_section, "    ; Fallthrough: Resolve promise with implicit return");
    asm_append(&text_section, "    xor rax, rax  ; Implicit return value = 0");
    sprintf(buffer, "    mov rdi, %s  ; Promise pointer", promise_var);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    mov rsi, rax  ; Return value");
    asm_append(&text_section, "    call promise_resolve");
    
    // Return promise pointer
    sprintf(buffer, "    mov rax, %s  ; Return promise", promise_var);
    asm_append(&text_section, buffer);
    free(promise_var);
    
    // Exit scope
    kapsam_cik();
    
    // Function epilog
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");
    
    // PHASE 8.7: Generate continuation trampolines for non-blocking await
    if (await_count > 0 && async_state_machine_mode) {
        asm_append(&text_section, "");
        sprintf(buffer, "    ; --- Continuation trampolines for %s ---", func_name);
        asm_append(&text_section, buffer);
        
        for (int state = 1; state < await_count; state++) {
            // Trampoline function: func_name_continue_N
            sprintf(buffer, "%s_continue_%d:", label_name, state);
            asm_append(&text_section, buffer);
            sprintf(buffer, "    ; Continuation for state %d", state);
            asm_append(&text_section, buffer);
            
            // Prolog
            asm_append(&text_section, "    push rbp");
            asm_append(&text_section, "    mov rbp, rsp");
            
            // RDI contains the promise value (callback argument)
            // Save it for later retrieval
            sprintf(buffer, "    push rdi  ; Save promise value");
            asm_append(&text_section, buffer);
            
            // PHASE 8.7: Get AsyncState from global and update state
            asm_append(&text_section, "    mov rax, [rel global_async_state_ptr]  ; Load AsyncState");
            sprintf(buffer, "    mov qword [rax+0], %d  ; Set state_number = %d", state, state);
            asm_append(&text_section, buffer);
            
            // Call main async function to resume
            sprintf(buffer, "    call %s  ; Resume async function", label_name);
            asm_append(&text_section, buffer);
            
            // Epilog
            asm_append(&text_section, "    pop rbp");
            asm_append(&text_section, "    ret");
            asm_append(&text_section, "");
        }
    }
    
    asm_append(&text_section, "");
}

// Phase 8: Await Expression Code Generation
void visit_AwaitExpr(ASTNode* node) {
    char buffer[256];
    
    asm_append(&text_section, "    ; --- Await Expression ---");
    
    // Evaluate the awaited expression (should be a promise)
    visit(node->await_data.ifade);
    
    // Promise pointer is now in RAX
    asm_append(&text_section, "    mov rbx, rax  ; Save promise pointer");
    
    // Phase 8.5: Check if we're in state machine mode
    if (async_state_machine_mode) {
        // PHASE 8.7: True non-blocking await with continuation callbacks
        async_state_counter++;  // Increment to next state
        int current_state = async_state_counter - 1;
        int next_state = async_state_counter;
        
        sprintf(buffer, "    ; PHASE 8.7: Non-blocking await - state %d -> %d", current_state, next_state);
        asm_append(&text_section, buffer);
        
        // Get AsyncState pointer
        // Problem: We need to access __async_state local variable!
        // It's stored at a specific stack offset, need to track it
        
        // For now, use a global pointer (will fix with proper context later)
        asm_append(&text_section, "    ; TODO: Get AsyncState from proper context");
        asm_append(&text_section, "    ; For now: register continuation callback");
        
        // Register continuation callback
        // promise_then(promise, callback_function)
        asm_append(&text_section, "    mov rdi, rbx  ; Promise pointer");
        
        // Get continuation function address
        sprintf(buffer, "    lea rsi, [rel %s_continue_%d]  ; Continuation function", 
                async_function_name ? async_function_name : "unknown", next_state);
        asm_append(&text_section, buffer);
        
        asm_append(&text_section, "    call promise_then  ; Register continuation");
        
        // Update AsyncState to next state (would need proper context access)
        asm_append(&text_section, "    ; TODO: Update AsyncState->state_number to next state");
        
        // Return from async function (yield control)
        sprintf(buffer, "    mov rax, %s  ; Return promise", async_promise_var);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    mov rsp, rbp");
        asm_append(&text_section, "    pop rbp");
        asm_append(&text_section, "    ret  ; Yield - will resume at continuation");
        
        // State label for resumption
        sprintf(buffer, "__state_%d:", next_state);
        asm_append(&text_section, buffer);
        sprintf(buffer, "    ; Resumed after await (state %d)", next_state);
        asm_append(&text_section, buffer);
        
        // Extract value from resolved promise
        asm_append(&text_section, "    mov rdi, rbx  ; Promise pointer");
        asm_append(&text_section, "    call promise_get_value");
        asm_append(&text_section, "    ; Result now in RAX");
        
        await_label_counter++;
    } else {
        // Simple blocking wait: check if promise is resolved
        sprintf(buffer, "__await_loop_%d:", await_label_counter);
        asm_append(&text_section, buffer);
        
        asm_append(&text_section, "    mov rdi, rbx  ; Promise pointer");
        asm_append(&text_section, "    call promise_is_resolved");
        asm_append(&text_section, "    test rax, rax");
        sprintf(buffer, "    jz __await_loop_%d  ; Loop until resolved", await_label_counter);
        asm_append(&text_section, buffer);
        
        // Promise resolved - extract value
        asm_append(&text_section, "    mov rdi, rbx  ; Promise pointer");
        asm_append(&text_section, "    call promise_get_value");
        asm_append(&text_section, "    ; Result now in RAX");
        
        await_label_counter++;
    }
}

void visit_IslecCagirma(ASTNode* node) {
    char* islec_adi = node->islec_cagirma_data.hedef_ad->value;
    int arg_sayisi = node->islec_cagirma_data.arguman_sayisi;
    char buffer[128];

    // Phase 7.2: Check if this is a lambda/function pointer variable
    // If islec_adi is a variable (not in function registry), treat as indirect call
    Degisken* var = kapsam_degisken_bul(islec_adi);
    if (var != NULL) {
        // It's a variable holding a function pointer or closure pointer
        sprintf(buffer, "    ; --- Indirect Call: %s(args) ---", islec_adi);
        asm_append(&text_section, buffer);
        
        // Load pointer (could be function pointer or closure pointer)
        sprintf(buffer, "    mov r10, %s  ; Load pointer", var->asm_adresi);
        asm_append(&text_section, buffer);
        
        // Phase 7.9: Closure struct layout:
        //   [r10+0]  = function pointer
        //   [r10+8]  = captured_var_1
        //   [r10+16] = captured_var_2
        //   ...
        
        // Extract function pointer from closure (offset 0)
        asm_append(&text_section, "    mov r11, [r10]  ; Extract function pointer");
        
        // Prepare arguments
        // rdi = closure pointer (r10)
        sprintf(buffer, "    mov rdi, r10  ; First arg = closure pointer");
        asm_append(&text_section, buffer);
        
        // Shift actual arguments to rsi, rdx, rcx, r8, r9
        for (int i = 0; i < arg_sayisi && i < 5; i++) {
            visit(node->islec_cagirma_data.argumanlar[i]);
            sprintf(buffer, "    mov %s, rax", arg_registerleri[i + 1]);
            asm_append(&text_section, buffer);
        }
        
        // Indirect call through r11 (function pointer)
        asm_append(&text_section, "    call r11  ; Indirect call via function pointer");
        return;
    }

    // ===== STRING FONKSİYONLARI KONTROLÜ =====
    if (strcmp(islec_adi, "STRLEN") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]); // String argümanı RAX'e
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call strlen");
        return; // ✅ Sonuç zaten RAX'te
    }
    else if (strcmp(islec_adi, "STRCAT") == 0 && arg_sayisi == 2) {
        // Argüman 1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // Arg1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // Arg2 -> rsi
        asm_append(&text_section, "    call tyd_strcat");
        return; // ✅ Sonuç RAX'te
    }
    else if (strcmp(islec_adi, "SUBSTR") == 0 && arg_sayisi == 3) {
        // Argüman 1 (str)
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 2 (pos)
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 3 (len)
        visit(node->islec_cagirma_data.argumanlar[2]);
        asm_append(&text_section, "    pop rsi");  // pos
        asm_append(&text_section, "    pop rdi");  // str
        asm_append(&text_section, "    mov rdx, rax"); // len
        asm_append(&text_section, "    call tyd_substr");
        return;
    }
    else if (strcmp(islec_adi, "STRCMP") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call strcmp");
        return;
    }
    else if (strcmp(islec_adi, "STRSTR") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call strstr");
        return;
    }

    // ===== DOSYA FONKSİYONLARI =====
    else if (strcmp(islec_adi, "DOSYA_AC") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call dosya_ac");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_OKU") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call dosya_oku");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_YAZ") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call dosya_yaz");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_KAPAT") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call dosya_kapat");
        return;
    }
    // YENİ: DIZIN_AL fonksiyonu (self-hosting için)
    else if (strcmp(islec_adi, "DIZIN_AL") == 0 && arg_sayisi == 0) {
        asm_append(&text_section, "    ; --- DIZIN_AL cagirma ---");
        asm_append(&text_section, "    mov rax, 0"); // SSE kuralı
        asm_append(&text_section, "    call runtime_dizin_al");
        // Sonuç (dizin yolu char*) zaten RAX'te olacak
        return;
    }

    // ===== BOOTSTRAP STRING FONKSİYONLARI =====
    // (Bu fonksiyonlar sadece TYD compiler'ını TYD'de yazmak için gerekli)

    else if (strcmp(islec_adi, "STRING_KARAKTER_AL") == 0 && arg_sayisi == 2) {
        // Argüman 1: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: indeks
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // indeks -> rsi
        asm_append(&text_section, "    call string_karakter_al");
        return; // Sonuç RAX'te (tek karakterlik string)
    }

    else if (strcmp(islec_adi, "STRING_ALT") == 0 && arg_sayisi == 3) {
        // Argüman 1: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: baslangic
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    push rax");
        // Argüman 3: uzunluk
        visit(node->islec_cagirma_data.argumanlar[2]);
        asm_append(&text_section, "    pop rsi");  // baslangic -> rsi
        asm_append(&text_section, "    pop rdi");  // string -> rdi
        asm_append(&text_section, "    mov rdx, rax"); // uzunluk -> rdx
        asm_append(&text_section, "    call string_alt");
        return; // Sonuç RAX'te (substring)
    }

    else if (strcmp(islec_adi, "KARAKTER_KODU") == 0 && arg_sayisi == 1) {
        // Argüman: tek karakterlik string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call karakter_kodu");
        return; // Sonuç RAX'te (ASCII kodu)
    }

    else if (strcmp(islec_adi, "KODU_KARAKTERE") == 0 && arg_sayisi == 1) {
        // Argüman: ASCII kodu
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call kodu_karaktere");
        return; // Sonuç RAX'te (tek karakterlik string)
    }

    else if (strcmp(islec_adi, "STRING_BIRLESTIR") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_birlestir");
        return; // Sonuç RAX'te (birleştirilmiş string)
    }

    else if (strcmp(islec_adi, "STRING_KARSILASTIR") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_karsilastir");
        return; // Sonuç RAX'te (0 = eşit, !=0 = farklı)
    }

    else if (strcmp(islec_adi, "STRING_UZUNLUK") == 0 && arg_sayisi == 1) {
        // Argüman: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call string_uzunluk");
        return; // Sonuç RAX'te (uzunluk)
    }

    else if (strcmp(islec_adi, "STRING_ESIT_MI") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_esit_mi");
        return; // Sonuç RAX'te (1 = eşit, 0 = farklı)
    }

    // ===== TYPE CONVERSION UTILITIES =====
    // Phase 7.9+: Simplified type conversion wrappers
    
    else if (strcmp(islec_adi, "str") == 0 && arg_sayisi == 1) {
        // str(numeric) - Convert numeric to string
        // Wrapper for int_to_string()
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax  ; Numeric value to convert");
        asm_append(&text_section, "    call int_to_string  ; Convert to string");
        return; // Result in RAX (string pointer)
    }
    
    else if (strcmp(islec_adi, "num") == 0 && arg_sayisi == 1) {
        // num(string) - Convert string to numeric
        // Wrapper for string_to_int()
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax  ; String to convert");
        asm_append(&text_section, "    call string_to_int  ; Convert to numeric");
        return; // Result in RAX (numeric value)
    }

    // ===== KULLANICI TANIMLI FONKSİYONLAR =====
    
    // Check if function is in registry
    FunctionSignature* sig = (FunctionSignature*)hashmap_get(function_registry, islec_adi);

    if (sig) {
        // Function found in registry - apply default parameter logic
        
        // Process ALL parameters (not just provided ones)
        // Push from right to left for stack convention
        for (int i = sig->param_count - 1; i >= 0; i--) {
            if (i < arg_sayisi) {
                // User provided this argument - use it
                visit(node->islec_cagirma_data.argumanlar[i]);
            } else {
                // User didn't provide this argument - check for default value
                if (sig->params[i].default_value != NULL) {
                    // Generate code for default value (e.g., mov rax, 20)
                    visit(sig->params[i].default_value);
                } else {
                    // Missing required parameter
                    fprintf(stderr, "ERROR: Function '%s' parameter '%s' is missing and has no default value!\n", 
                            islec_adi, sig->params[i].name);
                    exit(1);
                }
            }
            asm_append(&text_section, "    push rax");
        }
        
        // Pop arguments to registers (now in correct order: forward)
        for (int i = 0; i < sig->param_count; i++) {
            sprintf(buffer, "    pop %s", arg_registerleri[i]);
            asm_append(&text_section, buffer);
        }
    } else {
        // Function not in registry (might be built-in or undefined)
        // Use old behavior: just push provided arguments
        
        // 1. Push arguments to stack
        for (int i = 0; i < arg_sayisi; i++) {
            visit(node->islec_cagirma_data.argumanlar[i]);
            asm_append(&text_section, "    push rax");
        }

        // 2. Pop arguments to registers (reverse order)
        for (int i = arg_sayisi - 1; i >= 0; i--) {
            sprintf(buffer, "    pop %s", arg_registerleri[i]);
            asm_append(&text_section, buffer);
        }
    }

    // 3. Call the function
    sprintf(buffer, "    call %s", islec_adi);
    asm_append(&text_section, buffer);
}

void visit_ArrayTanimlama(ASTNode* node) {
    char* array_adi = node->array_tanimlama_data.ad->value;
    char* array_tipi = node->array_tanimlama_data.tip->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayTanimlama: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // NOTE: Don't visit() the boyut directly here!
    // If boyut is AST_ARRAY_LITERAL, we'll handle it specially below.
    // If boyut is AST_SAYI, we can extract the value without visiting.
    // Visiting boyut would cause AST_ARRAY_LITERAL to hit the error case in visit().

    // Array bilgisini kaydet
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(array_adi);
    d->tip = (char*)malloc(64);
    sprintf(d->tip, "ARRAY_%s", array_tipi);  // Örn: "ARRAY_SAYISAL"
    d->scope_level = current_scope_level;
    d->is_global = (current_scope_level == 0);
    d->array_boyut = 0;  // Will be set below

    // Boyut bilgisini sakla (şimdilik compile-time sabit olmalı)
    int array_boyut = 0;
    int has_literal = 0;
    
    if (node->array_tanimlama_data.boyut->type == AST_SAYI) {
        array_boyut = atoi(node->array_tanimlama_data.boyut->sabit_data.deger);
    } else if (node->array_tanimlama_data.boyut->type == AST_ARRAY_LITERAL) {
        // Array literal initialization: numeric arr[] = [1, 2, 3]
        array_boyut = node->array_tanimlama_data.boyut->array_literal_data.eleman_sayisi;
        has_literal = 1;
    } else {
        fprintf(stderr, "HATA [Generator]: Array boyutu compile-time sabit olmalı\n");
        exit(1);
    }

    d->array_boyut = array_boyut;  // Phase 5.8: Save size for for-each loops
    int total_bytes = array_boyut * 8;  // Her eleman 8 byte

    if (current_scope_level == 0) {
        // Global array: .bss section'da
        sprintf(buffer, "global_array_%s: resq %d  ; %d elements", array_adi, array_boyut, array_boyut);
        asm_append(&data_section, buffer);

        char* global_adres = (char*)malloc(64);
        sprintf(global_adres, "global_array_%s", array_adi);
        d->asm_adresi = global_adres;
        
        if (has_literal) {
            // Initialize global array from literal in _start or main
            sprintf(buffer, "    ; Initialize global array %s", array_adi);
            asm_append(&text_section, buffer);
            sprintf(buffer, "    lea rdi, [rel global_array_%s]", array_adi);
            asm_append(&text_section, buffer);
            
            // Directly initialize elements without calling visit()
            ASTNode** elemanlar = node->array_tanimlama_data.boyut->array_literal_data.elemanlar;
            for (int i = 0; i < array_boyut; i++) {
                sprintf(buffer, "    ; Element %d", i);
                asm_append(&text_section, buffer);
                visit(elemanlar[i]);  // Calculate value in RAX
                sprintf(buffer, "    mov [rdi + %d], rax", i * 8);
                asm_append(&text_section, buffer);
            }
        }
    } else {
        // Local array: Stack'te
        kapsam_yigin_ofseti += total_bytes;
        char* adres = (char*)malloc(32);
        sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);
        d->asm_adresi = strdup(adres);
        free(adres);

        sprintf(buffer, "    sub rsp, %d  ; Array allocation for %s[%d]",
                total_bytes, array_adi, array_boyut);
        asm_append(&text_section, buffer);
        
        if (has_literal) {
            // Initialize local array from literal - inline, no extra allocation
            sprintf(buffer, "    lea rdi, %s  ; Array base address", d->asm_adresi);
            asm_append(&text_section, buffer);
            
            // Directly initialize elements without calling visit()
            ASTNode** elemanlar = node->array_tanimlama_data.boyut->array_literal_data.elemanlar;
            for (int i = 0; i < array_boyut; i++) {
                sprintf(buffer, "    ; Element %d", i);
                asm_append(&text_section, buffer);
                visit(elemanlar[i]);  // Calculate value in RAX
                sprintf(buffer, "    mov [rdi + %d], rax", i * 8);
                asm_append(&text_section, buffer);
            }
        }
    }
}

void visit_ArrayErisim(ASTNode* node) {
    char* array_adi = node->array_erisim_data.ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayErisim: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // İndeks ifadesini hesapla (sonuç RAX'te)
    visit(node->array_erisim_data.indeks);

    // İndeksi offset'e çevir (indeks * 8)
    asm_append(&text_section, "    imul rax, 8  ; indeks * 8 (element size)");
    asm_append(&text_section, "    push rax  ; offset'i sakla");

    // Array base adresini bul
    char* array_base = kapsam_degisken_adresi_bul(array_adi);
    
    // Check if this is an array parameter (pointer) or local array
    Degisken* var = NULL;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (strcmp(kapsam_haritasi[i].ad, array_adi) == 0) {
            var = &kapsam_haritasi[i];
            break;
        }
    }
    
    // Array parameters (pointers) need mov, local arrays need lea
    int is_pointer = (var && var->tip && strncmp(var->tip, "ARRAY_", 6) == 0);
    
    if (is_pointer) {
        // Array parameter - load pointer value
        sprintf(buffer, "    mov rbx, %s  ; Load array pointer", array_base);
    } else {
        // Local array - compute address
        sprintf(buffer, "    lea rbx, %s  ; Array base adresi", array_base);
    }
    asm_append(&text_section, buffer);
    free(array_base);
    asm_append(&text_section, "    pop rax  ; offset'i geri al");
    asm_append(&text_section, "    add rbx, rax  ; base + offset");

    // Değeri oku
    asm_append(&text_section, "    mov rax, [rbx]  ; Array elemanını oku");
}

void visit_ArrayAtama(ASTNode* node) {
    char* array_adi = node->array_atama_data.ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayAtama: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // Değeri hesapla (sağ taraf)
    visit(node->array_atama_data.deger);
    asm_append(&text_section, "    push rax  ; Değeri sakla");

    // İndeks ifadesini hesapla
    visit(node->array_atama_data.indeks);

    // İndeksi offset'e çevir (indeks * 8)
    asm_append(&text_section, "    imul rax, 8  ; indeks * 8");
    asm_append(&text_section, "    mov rbx, rax  ; offset'i rbx'e");

    // Array base adresini bul
    char* array_base = kapsam_degisken_adresi_bul(array_adi);
    
    // Check if this is an array parameter (pointer) or local array
    Degisken* var = NULL;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (strcmp(kapsam_haritasi[i].ad, array_adi) == 0) {
            var = &kapsam_haritasi[i];
            break;
        }
    }
    
    // Array parameters (pointers) need mov, local arrays need lea
    int is_pointer = (var && var->tip && strncmp(var->tip, "ARRAY_", 6) == 0);

    // Base + offset hesapla
    if (is_pointer) {
        // Array parameter - load pointer value
        sprintf(buffer, "    mov rcx, %s  ; Load array pointer", array_base);
    } else {
        // Local array - compute address
        sprintf(buffer, "    lea rcx, %s  ; Array base adresi", array_base);
    }
    asm_append(&text_section, buffer);
    free(array_base);
    asm_append(&text_section, "    add rcx, rbx  ; base + offset");

    // Değeri yaz
    asm_append(&text_section, "    pop rax  ; Değeri geri al");
    asm_append(&text_section, "    mov [rcx], rax  ; Array elemanına yaz");
}

// Array Literal - [1, 2, 3, 4, 5]
// Creates array on stack and returns pointer in RAX
void visit_ArrayLiteral(ASTNode* node) {
    int eleman_sayisi = node->array_literal_data.eleman_sayisi;
    ASTNode** elemanlar = node->array_literal_data.elemanlar;
    char buffer[256];

    sprintf(buffer, "    ; --- Array Literal: [%d elements] ---", eleman_sayisi);
    asm_append(&text_section, buffer);

    if (eleman_sayisi == 0) {
        // Empty array - return null pointer
        asm_append(&text_section, "    mov rax, 0  ; Empty array");
        return;
    }

    // Allocate space on stack for array
    int total_bytes = eleman_sayisi * 8;
    sprintf(buffer, "    sub rsp, %d  ; Allocate array on stack", total_bytes);
    asm_append(&text_section, buffer);
    
    // Save array base address
    asm_append(&text_section, "    mov rbx, rsp  ; Array base address");

    // Initialize each element
    for (int i = 0; i < eleman_sayisi; i++) {
        sprintf(buffer, "    ; Element %d", i);
        asm_append(&text_section, buffer);
        
        // Calculate element value
        visit(elemanlar[i]);
        
        // Store in array
        sprintf(buffer, "    mov [rbx + %d], rax  ; Store element %d", i * 8, i);
        asm_append(&text_section, buffer);
    }

    // Return array pointer in RAX
    asm_append(&text_section, "    mov rax, rbx  ; Return array pointer");
}

// List Literal - (1, 2, 3, 4, 5)
// Creates dynamic list and returns pointer in RAX
void visit_ListLiteral(ASTNode* node) {
    int eleman_sayisi = node->list_literal_data.eleman_sayisi;
    ASTNode** elemanlar = node->list_literal_data.elemanlar;
    char buffer[256];

    sprintf(buffer, "    ; --- List Literal: (%d elements) ---", eleman_sayisi);
    asm_append(&text_section, buffer);

    // Create empty list
    asm_append(&text_section, "    call list_create  ; Create list");
    asm_append(&text_section, "    push rax  ; Save list pointer");

    // Add each element
    for (int i = 0; i < eleman_sayisi; i++) {
        sprintf(buffer, "    ; Add element %d", i);
        asm_append(&text_section, buffer);
        
        // Calculate element value
        visit(elemanlar[i]);
        
        // Call list_add(list, value)
        asm_append(&text_section, "    mov rdi, [rsp]  ; List pointer (arg1)");
        asm_append(&text_section, "    mov rsi, rax  ; Value (arg2)");
        asm_append(&text_section, "    call list_add");
    }

    // Return list pointer in RAX
    asm_append(&text_section, "    pop rax  ; Restore list pointer");
}

// Struct Tanımlama - Metadata sakla (şimdilik sadece comment)
void visit_StructTanimlama(ASTNode* node) {
    char* struct_adi = node->struct_tanimlama_data.ad->value;
    int field_sayisi = node->struct_tanimlama_data.field_sayisi;
    Token** field_tipleri = node->struct_tanimlama_data.field_tipleri;
    Token** field_adlari = node->struct_tanimlama_data.field_adlari;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Tanımlama: %s (%d fields) ---", struct_adi, field_sayisi);
    asm_append(&text_section, buffer);

    // Struct metadata'yı kaydet
    register_struct_metadata(struct_adi, field_tipleri, field_adlari, field_sayisi);

    // Metadata'dan bilgi al ve comment olarak yaz
    StructMetadata* meta = find_struct_metadata(struct_adi);
    if (meta) {
        for (int i = 0; i < field_sayisi; i++) {
            sprintf(buffer, "    ; Field %d: %s %s (offset %d, size %d)",
                    i, meta->fields[i].field_tip, meta->fields[i].field_ad,
                    meta->fields[i].offset, meta->fields[i].size);
            asm_append(&text_section, buffer);
        }
        sprintf(buffer, "    ; Total size: %d bytes", meta->total_size);
        asm_append(&text_section, buffer);
    }
}

// Struct Field Access - p.x (değeri RAX'e yükle)
void visit_StructFieldAccess(ASTNode* node) {
    char* struct_ad = node->struct_field_access_data.struct_ad->value;
    char* field_ad = node->struct_field_access_data.field_ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Field Access: %s.%s ---", struct_ad, field_ad);
    asm_append(&text_section, buffer);

    // Struct değişkenin base adresini bul
    Degisken* var = kapsam_degisken_bul(struct_ad);
    if (!var) {
        sprintf(buffer, "    ; ERROR: Değişken '%s' bulunamadı!", struct_ad);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Değişken '%s' bulunamadı!\n", struct_ad);
        asm_append(&text_section, "    mov rax, 0  ; Error placeholder");
        return;
    }

    // Struct tipinin metadata'sını bul
    StructMetadata* meta = find_struct_metadata(var->tip);
    if (!meta) {
        sprintf(buffer, "    ; ERROR: Struct tipi '%s' bulunamadı!", var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Struct tipi '%s' bulunamadı!\n", var->tip);
        asm_append(&text_section, "    mov rax, 0  ; Error placeholder");
        return;
    }

    // Field'ı metadata'da bul
    StructFieldInfo* field = find_field_in_struct(meta, field_ad);
    if (!field) {
        sprintf(buffer, "    ; ERROR: Field '%s' struct '%s' içinde bulunamadı!", field_ad, var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Field '%s' struct '%s' içinde bulunamadı!\n", field_ad, var->tip);
        asm_append(&text_section, "    mov rax, 0  ; Error placeholder");
        return;
    }

    // Base adres + offset hesapla ve değeri RAX'e yükle
    sprintf(buffer, "    ; Struct base: %s, field offset: %d", var->asm_adresi, field->offset);
    asm_append(&text_section, buffer);

    // Base adresinden offset kadar ilerideki değeri RAX'e yükle
    // var->asm_adresi = "[rbp-X]", field offset ekleyerek "[rbp-X+offset]" elde et
    int base_offset;
    sscanf(var->asm_adresi, "[rbp-%d]", &base_offset);
    int actual_offset = base_offset - field->offset;  // Stack aşağı doğru büyür

    sprintf(buffer, "    mov rax, [rbp-%d]  ; Load %s.%s", actual_offset, struct_ad, field_ad);
    asm_append(&text_section, buffer);
}

// Struct Field Atama - p.x = 10
void visit_StructFieldAtama(ASTNode* node) {
    char* struct_ad = node->struct_field_atama_data.struct_ad->value;
    char* field_ad = node->struct_field_atama_data.field_ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Field Atama: %s.%s ---", struct_ad, field_ad);
    asm_append(&text_section, buffer);

    // Struct değişkenin base adresini bul
    Degisken* var = kapsam_degisken_bul(struct_ad);
    if (!var) {
        sprintf(buffer, "    ; ERROR: Değişken '%s' bulunamadı!", struct_ad);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Değişken '%s' bulunamadı!\n", struct_ad);
        return;
    }

    // Struct tipinin metadata'sını bul
    StructMetadata* meta = find_struct_metadata(var->tip);
    if (!meta) {
        sprintf(buffer, "    ; ERROR: Struct tipi '%s' bulunamadı!", var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Struct tipi '%s' bulunamadı!\n", var->tip);
        return;
    }

    // Field'ı metadata'da bul
    StructFieldInfo* field = find_field_in_struct(meta, field_ad);
    if (!field) {
        sprintf(buffer, "    ; ERROR: Field '%s' struct '%s' içinde bulunamadı!", field_ad, var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Field '%s' struct '%s' içinde bulunamadı!\n", field_ad, var->tip);
        return;
    }

    // Sağ taraftaki değeri hesapla (sonuç RAX'e gelir)
    visit(node->struct_field_atama_data.deger);

    // Base adres + offset hesapla ve RAX'teki değeri yaz
    sprintf(buffer, "    ; Struct base: %s, field offset: %d", var->asm_adresi, field->offset);
    asm_append(&text_section, buffer);

    // Base adresinden offset kadar ilerideki memory'ye RAX'i yaz
    int base_offset;
    sscanf(var->asm_adresi, "[rbp-%d]", &base_offset);
    int actual_offset = base_offset - field->offset;  // Stack aşağı doğru büyür

    sprintf(buffer, "    mov [rbp-%d], rax  ; Store to %s.%s", actual_offset, struct_ad, field_ad);
    asm_append(&text_section, buffer);
}

// Struct Değişken - Person p;
void visit_StructDegisken(ASTNode* node) {
    char* struct_tip = node->struct_degisken_data.struct_tip->value;
    char* degisken_ad = node->struct_degisken_data.ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Değişken: %s %s ---", struct_tip, degisken_ad);
    asm_append(&text_section, buffer);

    // Struct metadata'yı bul
    StructMetadata* meta = find_struct_metadata(struct_tip);
    if (!meta) {
        sprintf(buffer, "    ; ERROR: Struct '%s' tanımlı değil!", struct_tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Struct '%s' tanımlı değil!\n", struct_tip);
        return;
    }

    // Stack'te struct için yer ayır (total_size kadar)
    // Her field için offset hesaplanmış, toplamda meta->total_size byte gerekli
    kapsam_yigin_ofseti += meta->total_size;
    char* adres = (char*)malloc(32);
    sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);

    // Değişkeni scope'a kaydet (struct tipi ile)
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(degisken_ad);
    d->asm_adresi = strdup(adres);
    d->tip = strdup(struct_tip);  // Type olarak struct adını sakla
    d->scope_level = current_scope_level;
    d->is_global = false;

    sprintf(buffer, "    ; Struct boyutu: %d bytes, stack adres: %s", meta->total_size, adres);
    asm_append(&text_section, buffer);

    // Struct'ı sıfırla (opsiyonel, güvenlik için)
    sprintf(buffer, "    ; Struct memory'yi sıfırla");
    asm_append(&text_section, buffer);
    for (int offset = 0; offset < meta->total_size; offset += 8) {
        sprintf(buffer, "    mov qword [rbp-%d], 0", kapsam_yigin_ofseti - offset);
        asm_append(&text_section, buffer);
    }

    free(adres);
}

// ===== Phase 2: List Visitor Functions =====

void visit_ListTanimlama(ASTNode* node) {
    char* element_tip = node->list_tanimlama_data.element_tipi->value;
    char* degisken_adi = node->list_tanimlama_data.degisken_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Tanimlama: %s ---", degisken_adi);
    asm_append(&text_section, buffer);

    // Determine element size based on type
    int element_size = 8; // Default: 8 bytes (numeric is int64)
    
    // Map MLP types to sizes (using English base keywords)
    if (strcmp(element_tip, "numeric") == 0 || strcmp(element_tip, "SAYISAL") == 0) {
        element_size = 8; // int64
    } else if (strcmp(element_tip, "string") == 0 || strcmp(element_tip, "METIN") == 0) {
        element_size = 8; // char* pointer
    } else if (strcmp(element_tip, "boolean") == 0 || strcmp(element_tip, "BOOL") == 0) {
        element_size = 8; // Treat as int64
    } else {
        element_size = 8; // Default for other types
    }

    // Call mlp_list_create(element_size)
    sprintf(buffer, "    mov rdi, %d  ; element_size", element_size);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    call mlp_list_create");
    asm_append(&text_section, "    ; RAX now contains MLP_List* pointer");

    // Store List pointer in variable
    char* adres = kapsam_degisken_yer_ayir(degisken_adi, "List");
    sprintf(buffer, "    mov %s, rax  ; Store List* in %s", adres, degisken_adi);
    asm_append(&text_section, buffer);
    
    // If we have a list literal initialization, add elements
    if (node->list_tanimlama_data.initial_value != NULL && 
        node->list_tanimlama_data.initial_value->type == AST_LIST_LITERAL) {
        
        sprintf(buffer, "    ; Initialize list %s from literal", degisken_adi);
        asm_append(&text_section, buffer);
        
        ASTNode** elemanlar = node->list_tanimlama_data.initial_value->list_literal_data.elemanlar;
        int eleman_sayisi = node->list_tanimlama_data.initial_value->list_literal_data.eleman_sayisi;
        
        sprintf(buffer, "    push rax  ; Save List* pointer");
        asm_append(&text_section, buffer);
        
        for (int i = 0; i < eleman_sayisi; i++) {
            sprintf(buffer, "    ; Add element %d", i);
            asm_append(&text_section, buffer);
            
            // Evaluate element value
            visit(elemanlar[i]);  // Value in RAX
            
            // Save value on stack
            asm_append(&text_section, "    push rax  ; Save element value");
            
            // Load List* pointer
            sprintf(buffer, "    mov rdi, [rsp+8]  ; Load List* (1st arg)");
            asm_append(&text_section, buffer);
            
            // Pass pointer to value
            asm_append(&text_section, "    mov rsi, rsp  ; Pass &value (2nd arg)");
            asm_append(&text_section, "    call mlp_list_add");
            
            // Clean up value from stack
            asm_append(&text_section, "    add rsp, 8  ; Remove element value");
        }
        
        // Clean up List* pointer
        asm_append(&text_section, "    add rsp, 8  ; Remove List* pointer");
    }
    
    free(adres);
}

void visit_ListAdd(ASTNode* node) {
    char* list_adi = node->list_add_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Add: %s.add(value) ---", list_adi);
    asm_append(&text_section, buffer);

    // Evaluate value expression (result in RAX)
    visit(node->list_add_data.deger);
    
    // Save value on stack (mlp_list_add expects pointer to element)
    asm_append(&text_section, "    push rax  ; Save value on stack");
    
    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi (1st arg)", list_adres);
    asm_append(&text_section, buffer);
    free(list_adres);
    
    // Pass pointer to value on stack
    asm_append(&text_section, "    mov rsi, rsp  ; Pass &value to rsi (2nd arg)");
    asm_append(&text_section, "    call mlp_list_add");
    
    // Clean up stack
    asm_append(&text_section, "    add rsp, 8  ; Remove value from stack");
}

void visit_ListGet(ASTNode* node) {
    char* list_adi = node->list_get_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Get: %s.get(index) ---", list_adi);
    asm_append(&text_section, buffer);

    // Evaluate index expression (result in RAX)
    visit(node->list_get_data.indeks);
    asm_append(&text_section, "    push rax  ; Save index");

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi (1st arg)", list_adres);
    asm_append(&text_section, buffer);
    free(list_adres);

    asm_append(&text_section, "    pop rsi  ; Load index to rsi (2nd arg)");
    asm_append(&text_section, "    call mlp_list_get");
    asm_append(&text_section, "    ; RAX now contains pointer to element");
    
    // Dereference pointer to get actual value (int64 or pointer)
    asm_append(&text_section, "    mov rax, [rax]  ; Dereference to get value");
}

void visit_ListSet(ASTNode* node) {
    char* list_adi = node->list_set_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Set: %s.set(index, value) ---", list_adi);
    asm_append(&text_section, buffer);

    // Evaluate value expression (result in RAX)
    visit(node->list_set_data.deger);
    asm_append(&text_section, "    push rax  ; Save value on stack");

    // Evaluate index expression (result in RAX)
    visit(node->list_set_data.indeks);
    asm_append(&text_section, "    push rax  ; Save index");

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi (1st arg)", list_adres);
    asm_append(&text_section, buffer);
    free(list_adres);

    asm_append(&text_section, "    pop rsi  ; Load index to rsi (2nd arg)");
    asm_append(&text_section, "    lea rdx, [rsp]  ; Pass &value to rdx (3rd arg)");
    asm_append(&text_section, "    call mlp_list_set");
    
    // Clean up stack
    asm_append(&text_section, "    add rsp, 8  ; Remove value from stack");
}

void visit_ListRemove(ASTNode* node) {
    char* list_adi = node->list_remove_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Remove: %s.remove(index) ---", list_adi);
    asm_append(&text_section, buffer);

    // Evaluate index expression (result in RAX)
    visit(node->list_remove_data.indeks);
    asm_append(&text_section, "    push rax  ; Save index");

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi (1st arg)", list_adres);
    asm_append(&text_section, buffer);
    free(list_adres);

    asm_append(&text_section, "    pop rsi  ; Load index to rsi (2nd arg)");
    asm_append(&text_section, "    call mlp_list_remove");
}

void visit_ListInsert(ASTNode* node) {
    char* list_adi = node->list_insert_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Insert: %s.insert(index, value) ---", list_adi);
    asm_append(&text_section, buffer);

    // Evaluate value expression (result in RAX)
    visit(node->list_insert_data.deger);
    asm_append(&text_section, "    push rax  ; Save value on stack");

    // Evaluate index expression (result in RAX)
    visit(node->list_insert_data.indeks);
    asm_append(&text_section, "    push rax  ; Save index");

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi (1st arg)", list_adres);
    asm_append(&text_section, buffer);
    free(list_adres);

    asm_append(&text_section, "    pop rsi  ; Load index to rsi (2nd arg)");
    asm_append(&text_section, "    lea rdx, [rsp]  ; Pass &value to rdx (3rd arg)");
    asm_append(&text_section, "    call mlp_list_insert");
    
    // Clean up stack
    asm_append(&text_section, "    add rsp, 8  ; Remove value from stack");
}

void visit_ListSize(ASTNode* node) {
    char* list_adi = node->list_size_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Length: %s.length() ---", list_adi);
    asm_append(&text_section, buffer);

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi", list_adres);
    asm_append(&text_section, buffer);
    free(list_adres);

    asm_append(&text_section, "    call mlp_list_length");
    asm_append(&text_section, "    ; RAX now contains length");
}

void visit_ListClear(ASTNode* node) {
    char* list_adi = node->list_clear_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Clear: %s.clear() ---", list_adi);
    asm_append(&text_section, buffer);

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi", list_adres);
    asm_append(&text_section, buffer);
    free(list_adres);

    asm_append(&text_section, "    call mlp_list_clear");
}

// ===== Phase 4: Hash Map Visit Functions =====

void visit_MapTanimlama(ASTNode* node) {
    char* key_tip = node->map_tanimlama_data.key_tipi->value;
    char* value_tip = node->map_tanimlama_data.value_tipi->value;
    char* degisken_adi = node->map_tanimlama_data.degisken_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Tanımlama: map[%s:%s] %s ---", key_tip, value_tip, degisken_adi);
    asm_append(&text_section, buffer);

    // Call hashmap_create(16) - initial capacity
    asm_append(&text_section, "    mov rdi, 16  ; Initial capacity");
    asm_append(&text_section, "    call hashmap_create");
    asm_append(&text_section, "    ; RAX now contains HashMap pointer");

    // Store HashMap pointer in variable
    char* adres = kapsam_degisken_yer_ayir(degisken_adi, "HashMap");
    sprintf(buffer, "    mov %s, rax  ; Store HashMap* in %s", adres, degisken_adi);
    asm_append(&text_section, buffer);
}

void visit_MapSet(ASTNode* node) {
    char* map_adi = node->map_set_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Set: %s.set(key, value) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate value expression (result in RAX)
    visit(node->map_set_data.value);
    asm_append(&text_section, "    push rax  ; Save value");

    // Evaluate key expression (result in RAX)
    visit(node->map_set_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);
    free(map_adres);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    pop rdx  ; Load value to rdx");
    asm_append(&text_section, "    call hashmap_put");
}

void visit_MapGet(ASTNode* node) {
    char* map_adi = node->map_get_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Get: %s.get(key) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate key expression (result in RAX)
    visit(node->map_get_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);
    free(map_adres);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    call hashmap_get");
    asm_append(&text_section, "    ; RAX now contains value (or NULL if not found)");
}

void visit_MapHas(ASTNode* node) {
    char* map_adi = node->map_has_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Has: %s.has(key) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate key expression (result in RAX)
    visit(node->map_has_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);
    free(map_adres);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    call hashmap_contains");
    asm_append(&text_section, "    ; RAX now contains 1 or 0");
}

void visit_MapRemove(ASTNode* node) {
    char* map_adi = node->map_remove_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Remove: %s.remove(key) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate key expression (result in RAX)
    visit(node->map_remove_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);
    free(map_adres);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    call hashmap_remove");
}

void visit_MapSize(ASTNode* node) {
    char* map_adi = node->map_size_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Size: %s.size() ---", map_adi);
    asm_append(&text_section, buffer);

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);
    free(map_adres);

    asm_append(&text_section, "    call hashmap_size");
    asm_append(&text_section, "    ; RAX now contains size");
}

void visit_MapClear(ASTNode* node) {
    char* map_adi = node->map_clear_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Clear: %s.clear() ---", map_adi);
    asm_append(&text_section, buffer);

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);
    free(map_adres);

    asm_append(&text_section, "    call hashmap_clear");
}

// Phase 5.4: Enum Tanımlama
void visit_EnumTanimlama(ASTNode* node) {
    char* enum_ad = node->enum_tanimlama_data.ad->value;
    int value_sayisi = node->enum_tanimlama_data.value_sayisi;
    char buffer[256];

    sprintf(buffer, "    ; --- Enum Definition: %s ---", enum_ad);
    asm_append(&text_section, buffer);

    // Enum metadata'yı global tabloya kaydet
    if (enum_metadata_count >= MAX_ENUMS) {
        fprintf(stderr, "HATA [Generator]: Maksimum enum sayısı aşıldı!\n");
        exit(1);
    }

    EnumMetadata* meta = &enum_metadata_table[enum_metadata_count++];
    meta->enum_ad = strdup(enum_ad);
    meta->value_sayisi = value_sayisi;

    for (int i = 0; i < value_sayisi; i++) {
        meta->values[i].value_ad = strdup(node->enum_tanimlama_data.value_adlari[i]->value);
        meta->values[i].value = node->enum_tanimlama_data.value_degerleri[i];

        sprintf(buffer, "    ; %s.%s = %d", enum_ad, meta->values[i].value_ad, meta->values[i].value);
        asm_append(&text_section, buffer);
    }
}

// Phase 5.4: Switch Statement
void visit_Switch(ASTNode* node) {
    char buffer[256];
    int switch_id = etiket_sayaci++;

    asm_append(&text_section, "    ; --- Switch Statement ---");

    // Switch ifadesini değerlendir (sonuç RAX'te)
    visit(node->switch_data.ifade);
    asm_append(&text_section, "    push rax  ; Switch value'yu sakla");

    // Her case için karşılaştırma ve jump
    for (int i = 0; i < node->switch_data.case_sayisi; i++) {
        ASTNode* case_node = node->switch_data.cases[i];

        sprintf(buffer, ".case_%d_%d:", switch_id, i);
        asm_append(&text_section, buffer);

        // Switch value'yu geri yükle
        asm_append(&text_section, "    mov rax, [rsp]  ; Switch value");

        // Case değerini değerlendir
        asm_append(&text_section, "    push rax");
        visit(case_node->case_data.deger);
        asm_append(&text_section, "    mov rbx, rax  ; Case value");
        asm_append(&text_section, "    pop rax");

        // Karşılaştır
        asm_append(&text_section, "    cmp rax, rbx");
        sprintf(buffer, "    jne .case_%d_%d_skip", switch_id, i);
        asm_append(&text_section, buffer);

        // Case bloğunu çalıştır
        visit(case_node->case_data.blok);

        // Switch sonuna atla
        sprintf(buffer, "    jmp .switch_%d_end", switch_id);
        asm_append(&text_section, buffer);

        sprintf(buffer, ".case_%d_%d_skip:", switch_id, i);
        asm_append(&text_section, buffer);
    }

    // Default bloğu (varsa)
    if (node->switch_data.default_blok != NULL) {
        sprintf(buffer, ".switch_%d_default:", switch_id);
        asm_append(&text_section, buffer);
        visit(node->switch_data.default_blok);
    }

    // Switch sonu
    sprintf(buffer, ".switch_%d_end:", switch_id);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    add rsp, 8  ; Switch value'yu temizle");
}

// Phase 3: Built-in function call
void visit_BuiltinCall(ASTNode* node) {
    TokenType func = node->builtin_call_data.function_type;
    char buffer[512];

    // Generate comment
    const char* func_name = "";
    switch (func) {
        case TOKEN_BUILTIN_READ_FILE: func_name = "read_file"; break;
        case TOKEN_BUILTIN_WRITE_FILE: func_name = "write_file"; break;
        case TOKEN_BUILTIN_APPEND_FILE: func_name = "append_file"; break;
        case TOKEN_BUILTIN_FILE_EXISTS: func_name = "file_exists"; break;
        case TOKEN_BUILTIN_FILE_SIZE: func_name = "file_size"; break;
        case TOKEN_BUILTIN_READ_LINES: func_name = "read_lines"; break;
        case TOKEN_BUILTIN_STRING_SPLIT: func_name = "string_split"; break;
        case TOKEN_BUILTIN_STRING_JOIN: func_name = "string_join"; break;
        case TOKEN_BUILTIN_STRING_REPLACE: func_name = "string_replace"; break;
        case TOKEN_BUILTIN_STRING_TRIM: func_name = "string_trim"; break;
        case TOKEN_BUILTIN_STRING_UPPER: func_name = "string_upper"; break;
        case TOKEN_BUILTIN_STRING_LOWER: func_name = "string_lower"; break;
        case TOKEN_BUILTIN_STRING_FIND: func_name = "string_find"; break;
        case TOKEN_BUILTIN_STRING_STARTS_WITH: func_name = "string_starts_with"; break;
        case TOKEN_BUILTIN_STRING_ENDS_WITH: func_name = "string_ends_with"; break;
        // Phase 4: Type Conversions
        case TOKEN_BUILTIN_INT_TO_STRING: func_name = "int_to_string"; break;
        case TOKEN_BUILTIN_STRING_TO_INT: func_name = "string_to_int"; break;
        case TOKEN_BUILTIN_CHAR_TO_STRING: func_name = "char_to_string"; break;
        case TOKEN_BUILTIN_STRING_CONCAT: func_name = "string_concat"; break;
        // Phase 5.7: Type Casting Helpers
        case TOKEN_BUILTIN_NUM: func_name = "num"; break;
        case TOKEN_BUILTIN_STR: func_name = "str"; break;
        case TOKEN_BUILTIN_READ_INPUT: func_name = "read_input"; break;
        case TOKEN_BUILTIN_READ_LINE: func_name = "read_line"; break;
        case TOKEN_BUILTIN_READ_INT: func_name = "read_int"; break;
        // Phase 4: Math Operations
        case TOKEN_BUILTIN_MATH_ABS: func_name = "math_abs"; break;
        case TOKEN_BUILTIN_MATH_MIN: func_name = "math_min"; break;
        case TOKEN_BUILTIN_MATH_MAX: func_name = "math_max"; break;
        case TOKEN_BUILTIN_MATH_POW: func_name = "math_pow"; break;
        // Phase 5.1: Enhanced String Operations
        case TOKEN_BUILTIN_STRING_LENGTH: func_name = "string_length"; break;
        case TOKEN_BUILTIN_STRING_SUBSTRING: func_name = "string_substring"; break;
        case TOKEN_BUILTIN_STRING_INDEX_OF: func_name = "string_index_of"; break;
        case TOKEN_BUILTIN_STRING_LAST_INDEX_OF: func_name = "string_last_index_of"; break;
        // Phase 5.6: Modern String API Wrappers
        case TOKEN_BUILTIN_LEN: func_name = "len"; break;
        case TOKEN_BUILTIN_ORD: func_name = "ord"; break;
        case TOKEN_BUILTIN_CHR: func_name = "chr"; break;
        case TOKEN_BUILTIN_CHAR_CODE: func_name = "char_code"; break;
        case TOKEN_BUILTIN_CHAR_AT: func_name = "char_at"; break;
        case TOKEN_BUILTIN_SUBSTRING: func_name = "substring"; break;
        case TOKEN_BUILTIN_CONTAINS: func_name = "contains"; break;
        case TOKEN_BUILTIN_STARTSWITH: func_name = "startswith"; break;
        case TOKEN_BUILTIN_ENDSWITH: func_name = "endswith"; break;
        case TOKEN_BUILTIN_LEFT: func_name = "left"; break;
        case TOKEN_BUILTIN_RIGHT: func_name = "right"; break;
        case TOKEN_BUILTIN_MID: func_name = "mid"; break;
        // Phase 5.2: Error Handling
        case TOKEN_BUILTIN_EXIT_WITH_CODE: func_name = "exit_with_code"; break;
        case TOKEN_BUILTIN_PANIC: func_name = "panic"; break;
        case TOKEN_BUILTIN_ASSERT: func_name = "mlp_assert"; break;
        case TOKEN_BUILTIN_GET_ERROR_CODE: func_name = "get_error_code"; break;
        case TOKEN_BUILTIN_SET_ERROR_CODE: func_name = "set_error_code"; break;
        // Phase 5.2: Memory Management
        case TOKEN_BUILTIN_MLP_MALLOC: func_name = "mlp_malloc"; break;
        case TOKEN_BUILTIN_MLP_FREE: func_name = "mlp_free"; break;
        case TOKEN_BUILTIN_MLP_REALLOC: func_name = "mlp_realloc"; break;
        case TOKEN_BUILTIN_MLP_CALLOC: func_name = "mlp_calloc"; break;
        case TOKEN_BUILTIN_GET_ALLOCATED_BYTES: func_name = "get_allocated_bytes"; break;
        case TOKEN_BUILTIN_CHECK_MEMORY_LEAKS: func_name = "check_memory_leaks"; break;
        // Phase 5.2: System Utilities
        case TOKEN_BUILTIN_GET_ENV: func_name = "get_env"; break;
        case TOKEN_BUILTIN_CURRENT_TIMESTAMP: func_name = "current_timestamp"; break;
        case TOKEN_BUILTIN_SLEEP_MS: func_name = "sleep_ms"; break;
        // Phase 5.3: Binary File I/O
        case TOKEN_BUILTIN_READ_BINARY: func_name = "read_binary"; break;
        case TOKEN_BUILTIN_WRITE_BINARY: func_name = "write_binary"; break;
        case TOKEN_BUILTIN_GET_FILE_INFO: func_name = "get_file_info"; break;
        case TOKEN_BUILTIN_COPY_FILE: func_name = "copy_file"; break;
        // Phase 5.3: Directory Operations
        case TOKEN_BUILTIN_LIST_DIRECTORY: func_name = "list_directory"; break;
        case TOKEN_BUILTIN_CREATE_DIRECTORY: func_name = "create_directory"; break;
        case TOKEN_BUILTIN_REMOVE_DIRECTORY: func_name = "remove_directory"; break;
        case TOKEN_BUILTIN_DIRECTORY_EXISTS: func_name = "directory_exists"; break;
        case TOKEN_BUILTIN_GET_CURRENT_DIR: func_name = "get_current_dir"; break;
        case TOKEN_BUILTIN_CHANGE_DIRECTORY: func_name = "change_directory"; break;
        // Phase 5.3: Process Control
        case TOKEN_BUILTIN_EXECUTE_COMMAND: func_name = "execute_command"; break;
        case TOKEN_BUILTIN_GET_COMMAND_OUTPUT: func_name = "get_command_output"; break;
        case TOKEN_BUILTIN_GET_PROCESS_ID: func_name = "get_process_id"; break;
        case TOKEN_BUILTIN_GET_PARENT_PROCESS_ID: func_name = "get_parent_process_id"; break;
        // Phase 5.3: Time & Date Utilities
        case TOKEN_BUILTIN_FORMAT_TIMESTAMP: func_name = "format_timestamp"; break;
        case TOKEN_BUILTIN_PARSE_TIMESTAMP: func_name = "parse_timestamp"; break;
        case TOKEN_BUILTIN_GET_MILLISECONDS: func_name = "get_milliseconds"; break;
        case TOKEN_BUILTIN_GET_TIME_STRING: func_name = "get_time_string"; break;
        // Phase 5.3: Path Utilities
        case TOKEN_BUILTIN_JOIN_PATH: func_name = "join_path"; break;
        case TOKEN_BUILTIN_GET_FILE_EXTENSION: func_name = "get_file_extension"; break;
        case TOKEN_BUILTIN_GET_FILE_NAME: func_name = "get_file_name"; break;
        case TOKEN_BUILTIN_GET_DIRECTORY: func_name = "get_directory"; break;
        default: func_name = "unknown"; break;
    }

    sprintf(buffer, "    ; --- Built-in: %s() ---", func_name);
    asm_append(&text_section, buffer);

    // Evaluate arguments and load into registers (x86-64 calling convention)
    // rdi = arg1, rsi = arg2, rdx = arg3

    if (node->builtin_call_data.arg1) {
        visit(node->builtin_call_data.arg1);  // Result in RAX
        asm_append(&text_section, "    mov rdi, rax  ; arg1");
    }

    if (node->builtin_call_data.arg2) {
        visit(node->builtin_call_data.arg2);  // Result in RAX
        asm_append(&text_section, "    mov rsi, rax  ; arg2");
    }

    if (node->builtin_call_data.arg3) {
        visit(node->builtin_call_data.arg3);  // Result in RAX
        asm_append(&text_section, "    mov rdx, rax  ; arg3");
    }

    // Call the built-in function
    sprintf(buffer, "    call %s", func_name);
    asm_append(&text_section, buffer);

    // Result is in RAX
}

// Phase 5.7: typeof operator - returns type name as string
void visit_TypeofExpr(ASTNode* node) {
    asm_append(&text_section, "    ; --- typeof(expr) ---");
    
    // For now, we'll implement a simple version that works with literals
    // Full runtime type detection requires type metadata which we don't track yet
    
    ASTNode* expr = node->typeof_data.ifade;
    
    // Check expression type at compile time
    if (expr->type == AST_SAYI) {
        // It's a number
        asm_append(&data_section, "typeof_numeric: db \"numeric\", 0");
        asm_append(&text_section, "    lea rax, [rel typeof_numeric]");
    } else if (expr->type == AST_METIN) {
        // It's a string
        asm_append(&data_section, "typeof_string: db \"string\", 0");
        asm_append(&text_section, "    lea rax, [rel typeof_string]");
    } else if (expr->type == AST_DEGISKEN) {
        // It's a variable - we need runtime type detection
        // For now, call a runtime function that will determine type
        visit(expr);  // Evaluate expression, result in RAX
        asm_append(&text_section, "    mov rdi, rax  ; Value to check");
        asm_append(&text_section, "    call mlp_get_type  ; Returns type string in RAX");
    } else {
        // For other expressions, evaluate and call runtime function
        visit(expr);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call mlp_get_type");
    }
    
    // Result (type name string) is in RAX
}

void visit_DonusKomutu(ASTNode* node) {
    asm_append(&text_section, "    ; --- Donus Komutu ---");

    // 1. Döndürülecek ifadeyi hesapla (Sonuç RAX'e yüklenir)
    visit(node->tek_ifade_data.ifade);

    // Phase 8: In async function, resolve promise and jump to end
    if (in_async_function && async_promise_var != NULL) {
        char buffer[256];
        // Save return value
        asm_append(&text_section, "    mov rbx, rax  ; Save return value");
        
        // Resolve promise
        sprintf(buffer, "    mov rdi, %s  ; Promise pointer", async_promise_var);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    mov rsi, rbx  ; Return value");
        asm_append(&text_section, "    call promise_resolve");
        
        // Return promise pointer (not the value!)
        sprintf(buffer, "    mov rax, %s  ; Return promise", async_promise_var);
        asm_append(&text_section, buffer);
        
        // Epilog and return
        asm_append(&text_section, "    mov rsp, rbp");
        asm_append(&text_section, "    pop rbp");
        asm_append(&text_section, "    ret");
        return;
    }

    // 2. Normal function: Fonksiyon Çıkışını (Epilog) üret
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");
}

/**
 * İkili İşlem (a + b) düğümünü ziyaret eder.
 * Sonuç her zaman RAX register'ında kalır.
 */
void visit_IkiliIslem(ASTNode* node) {
    // Phase 7.9+: String concatenation için + operatörünü özel olarak handle et
    if (node->ikili_islem_data.operator_type == TOKEN_PLUS) {
        // Sol ve sağ tarafın string olup olmadığını kontrol et
        bool sol_is_string = false;
        bool sag_is_string = false;
        
        // Sol taraf string mi?
        if (node->ikili_islem_data.sol->type == AST_METIN) {
            sol_is_string = true;
        } else if (node->ikili_islem_data.sol->type == AST_BUILTIN_CALL) {
            TokenType func_type = node->ikili_islem_data.sol->builtin_call_data.function_type;
            if (func_type == TOKEN_BUILTIN_STR) {
                sol_is_string = true;
            }
        } else if (node->ikili_islem_data.sol->type == AST_ISLEC_CAGIRMA) {
            char* func_name = node->ikili_islem_data.sol->islec_cagirma_data.hedef_ad->value;
            if (strcmp(func_name, "str") == 0) {
                sol_is_string = true;
            }
        } else if (node->ikili_islem_data.sol->type == AST_IKILI_ISLEM) {
            // Nested + operation (chained concatenation) - assume string concat
            if (node->ikili_islem_data.sol->ikili_islem_data.operator_type == TOKEN_PLUS) {
                sol_is_string = true;
            }
        }
        
        // Sağ taraf string mi?
        if (node->ikili_islem_data.sag->type == AST_METIN) {
            sag_is_string = true;
        } else if (node->ikili_islem_data.sag->type == AST_BUILTIN_CALL) {
            TokenType func_type = node->ikili_islem_data.sag->builtin_call_data.function_type;
            if (func_type == TOKEN_BUILTIN_STR) {
                sag_is_string = true;
            }
        } else if (node->ikili_islem_data.sag->type == AST_ISLEC_CAGIRMA) {
            char* func_name = node->ikili_islem_data.sag->islec_cagirma_data.hedef_ad->value;
            if (strcmp(func_name, "str") == 0) {
                sag_is_string = true;
            }
        }
        
        // Eğer en az biri string ise string_concat kullan
        if (sol_is_string || sag_is_string) {
            // 1. Sol tarafı hesapla
            visit(node->ikili_islem_data.sol);
            asm_append(&text_section, "    push rax"); // Sol sonucu stack'e
            
            // 2. Sağ tarafı hesapla
            visit(node->ikili_islem_data.sag);
            asm_append(&text_section, "    mov rsi, rax"); // Sağ -> RSI (2. parametre)
            asm_append(&text_section, "    pop rdi");       // Sol -> RDI (1. parametre)
            
            // 3. string_concat çağır
            asm_append(&text_section, "    call string_concat");
            // Sonuç RAX'te (birleştirilmiş string)
            return;
        }
    }
    
    // Normal numeric işlemler için eski kod
    // 1. Sağ tarafı (ifade 2) hesapla ve yığına (stack) it
    visit(node->ikili_islem_data.sag);
    asm_append(&text_section, "    push rax"); // Sağ tarafın sonucunu yığına kaydet

    // 2. Sol tarafı (ifade 1) hesapla (sonuç RAX'te kalır)
    visit(node->ikili_islem_data.sol);

    // 3. Sağ tarafı yığından çek (pop) ve işlemi yap
    asm_append(&text_section, "    pop rbx"); // Sağ tarafın sonucunu RBX'e al

    // Operatör tipine göre Assembly komutunu seç
    switch (node->ikili_islem_data.operator_type) {
        case TOKEN_PLUS:
            asm_append(&text_section, "    add rax, rbx"); // (Sol) + (Sağ)
            break;
        case TOKEN_MINUS:
            asm_append(&text_section, "    sub rax, rbx"); // (Sol) - (Sağ)
            break;
        case TOKEN_MUL:
            asm_append(&text_section, "    imul rax, rbx"); // (Sol) * (Sağ)
            break;
        case TOKEN_DIV:
            asm_append(&text_section, "    cqo"); // RAX'i RDX:RAX'e genişlet
            asm_append(&text_section, "    idiv rbx"); // (Sol) / (Sağ) -> Sonuç RAX'te
            break;
        
        // YENİ: KARŞILAŞTIRMA OPERATÖRLERİ
        case TOKEN_OP_ESIT_KARSILASTIRMA: // ==
            asm_append(&text_section, "    cmp rax, rbx"); // RAX ve RBX'i karşılaştır
            asm_append(&text_section, "    sete al");      // Eğer eşitse (Equal) AL = 1 yap
            asm_append(&text_section, "    movzx rax, al"); // RAX'i 1 or 0 yap
            break;
        case TOKEN_NOT_ESIT: // !=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setne al");     // Eğer eşit değilse (Not Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_LT: // <
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setl al");      // Eğer küçükse (Less) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_GT: // >
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setg al");      // Eğer büyükse (Greater) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_GTE: // >=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setge al");     // Eğer büyük or eşitse (Greater or Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_LTE: // <=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setle al");     // Eğer küçük or eşitse (Less or Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;

        // Phase 5.4: Modulo operator
        case TOKEN_MOD: // %
            asm_append(&text_section, "    cqo");          // RAX'i RDX:RAX'e genişlet (sign extend)
            asm_append(&text_section, "    idiv rbx");     // RAX / RBX -> bölüm RAX'te, kalan RDX'te
            asm_append(&text_section, "    mov rax, rdx"); // Kalanı RAX'e taşı
            break;

        // Phase 5.4: Logical operators
        case TOKEN_AND: // and
            asm_append(&text_section, "    test rax, rax"); // RAX sıfır mı?
            asm_append(&text_section, "    setne al");      // RAX != 0 ise AL = 1
            asm_append(&text_section, "    movzx rax, al");
            asm_append(&text_section, "    test rbx, rbx"); // RBX sıfır mı?
            asm_append(&text_section, "    setne bl");      // RBX != 0 ise BL = 1
            asm_append(&text_section, "    and al, bl");    // AL = AL AND BL
            asm_append(&text_section, "    movzx rax, al"); // Sonuç RAX'te
            break;

        case TOKEN_OR: // or
            asm_append(&text_section, "    or rax, rbx");   // RAX = RAX OR RBX
            asm_append(&text_section, "    test rax, rax"); // Sonuç sıfır mı?
            asm_append(&text_section, "    setne al");      // Sıfır değilse AL = 1
            asm_append(&text_section, "    movzx rax, al"); // Sonuç 0 or 1
            break;

        // Phase 5.5: Bitwise operators
        case TOKEN_BITWISE_AND: // &
            asm_append(&text_section, "    ; Bitwise AND (&)");
            asm_append(&text_section, "    and rax, rbx");
            break;
        
        case TOKEN_BITWISE_OR: // |
            asm_append(&text_section, "    ; Bitwise OR (|)");
            asm_append(&text_section, "    or rax, rbx");
            break;
        
        case TOKEN_BITWISE_XOR: // ^
            asm_append(&text_section, "    ; Bitwise XOR (^)");
            asm_append(&text_section, "    xor rax, rbx");
            break;
        
        case TOKEN_LSHIFT: // <<
            asm_append(&text_section, "    ; Left Shift (<<)");
            asm_append(&text_section, "    mov rcx, rbx    ; Shift amount to CL");
            asm_append(&text_section, "    shl rax, cl     ; Shift left");
            break;
        
        case TOKEN_RSHIFT: // >>
            asm_append(&text_section, "    ; Right Shift (>>)");
            asm_append(&text_section, "    mov rcx, rbx    ; Shift amount to CL");
            asm_append(&text_section, "    shr rax, cl     ; Shift right (unsigned)");
            break;

        default:
            fprintf(stderr, "HATA [Generator]: Desteklenmeyen ikili operatör tipi: %d\n",
                node->ikili_islem_data.operator_type);
            exit(1);
    }
}

// Ana ziyaretçi fonksiyonu (Tüm düğüm tiplerini yönlendirir)
void visit(ASTNode* node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case AST_BLOK:
            visit_Blok(node);
            break;
        case AST_SAYI:
            visit_Sayi(node);
            break;
        case AST_METIN:
            visit_Metin(node);
            break;
        case AST_INTERPOLATED_STRING:  // Phase 5.9
            visit_InterpolatedString(node);
            break;
        case AST_IMPORT:  // Phase 5.10: Module import
            visit_Import(node);
            break;
        case AST_PRINT_STATEMENT:
            visit_Yazdir(node);
            break;
        
        // YENİ: İkili İşlem
        case AST_IKILI_ISLEM:
            visit_IkiliIslem(node);
            break;
        
        // YENİ: Değişken Tanımlama
        case AST_VARIABLE_DECLARATION:
            visit_DegiskenTanimlama(node);
            break;
            
        // YENİ: Değişken Okuma
        case AST_DEGISKEN:
            visit_Degisken(node);
            break;
            
        // YENİ: Değişken Güncelleme
        case AST_ASSIGNMENT_KOMUTU:
            visit_AtamaKomutu(node);
            break;
            
        // YENİ: Koşul (IF) Komutu
        case AST_IF_STATEMENT_KOMUTU:
            visit_KosulKomutu(node);
            break;
            
        // YENİ: Döngü Komutu (while)
        case AST_WHILE_LOOP_KOMUTU:
            visit_DonguKomutu(node);
            break;
            
        // YENİ: For Komutu
        case AST_FOR_KOMUTU:
            visit_ForKomutu(node);
            break;
            
        // Phase 5.8: For-In-Range
        case AST_FOR_IN_RANGE:
            visit_ForInRange(node);
            break;
            
        case AST_FOR_EACH:
            visit_ForEach(node);
            break;
            
        // YENİ: Döngü Bitir Komutu (break)
        case AST_BREAK:
            visit_DonguBitirKomutu(node);
            break;

        // YENİ: Continue Komutu
        case AST_CONTINUE:
            visit_DonguDevamKomutu(node);
            break;

        // Phase 5.8: Stop (debugging breakpoint)
        case AST_STOP:
            visit_Stop(node);
            break;

        // Phase 5.8: Goto
        case AST_GOTO:
            visit_Goto(node);
            break;

        // Phase 5.8: Label
        case AST_LABEL:
            visit_Label(node);
            break;

        // Phase 5.8: Ternary operator
        case AST_TERNARY:
            visit_Ternary(node);
            break;

        // Phase 7.1: Lambda expression
        case AST_LAMBDA:
            visit_Lambda(node);
            break;

        // Phase 8: Async/Await
        case AST_ASYNC_FUNCTION:
            visit_AsyncFunction(node);
            break;
            
        case AST_AWAIT_EXPR:
            visit_AwaitExpr(node);
            break;

        // YENİ: İşleç Tanımlama
        case AST_FUNCTION_DECLARATION:
            visit_IslecTanimlama(node);
            break;

        // YENİ: İşleç Çağırma
        case AST_ISLEC_CAGIRMA:
            visit_IslecCagirma(node);
            break;
            
        // Phase 5.7: typeof operator
        case AST_TYPEOF_EXPR:
            visit_TypeofExpr(node);
            break;
            
        // YENİ: Dönüş Komutu
        case AST_RETURN_STATEMENT_KOMUTU:
            visit_DonusKomutu(node);
            break;

        // Array Tanımlama
        case AST_ARRAY_TANIMLAMA:
            visit_ArrayTanimlama(node);
            break;

        // Array Erişim
        case AST_ARRAY_ERISIM:
            visit_ArrayErisim(node);
            break;

        // Array Atama
        case AST_ARRAY_ATAMA:
            visit_ArrayAtama(node);
            break;

        case AST_ARRAY_LITERAL:
            fprintf(stderr, "HATA [Generator]: Array literal sadece array tanımlamasında kullanılabilir!\n");
            fprintf(stderr, "  Kullanım: numeric arr[] = [1, 2, 3]\n");
            fprintf(stderr, "  YANLIŞ: print [1, 2, 3]  (şimdilik desteklenmiyor)\n");
            exit(1);
            break;

        case AST_LIST_LITERAL:
            fprintf(stderr, "HATA [Generator]: List literal sadece list tanımlamasında kullanılabilir!\n");
            fprintf(stderr, "  Kullanım: numeric items() = (1, 2, 3)\n");
            fprintf(stderr, "  YANLIŞ: print (1, 2, 3)  (şimdilik desteklenmiyor)\n");
            exit(1);
            break;

        // Struct Tanımlama
        case AST_STRUCT_DECLARATION:
            visit_StructTanimlama(node);
            break;

        // Struct Field Access
        case AST_STRUCT_FIELD_ACCESS:
            visit_StructFieldAccess(node);
            break;

        // Struct Field Atama
        case AST_STRUCT_FIELD_ATAMA:
            visit_StructFieldAtama(node);
            break;

        // Struct Değişken
        case AST_STRUCT_VARIABLE:
            visit_StructDegisken(node);
            break;

        // Phase 2: Dynamic Lists
        case AST_LIST_TANIMLAMA:
            visit_ListTanimlama(node);
            break;

        // Note: AST_LIST_LITERAL is handled inline in ListTanimlama
        // Don't add a case for it here to avoid duplicate code generation

        case AST_LIST_ADD:
            visit_ListAdd(node);
            break;

        case AST_LIST_GET:
            visit_ListGet(node);
            break;

        case AST_LIST_SET:
            visit_ListSet(node);
            break;

        case AST_LIST_REMOVE:
            visit_ListRemove(node);
            break;

        case AST_LIST_INSERT:
            visit_ListInsert(node);
            break;

        case AST_LIST_SIZE:
            visit_ListSize(node);
            break;

        case AST_LIST_CLEAR:
            visit_ListClear(node);
            break;

        // Phase 6.2: Optional<T> operations
        case AST_OPTIONAL_TANIMLAMA:
            visit_OptionalTanimlama(node);
            break;

        case AST_OPTIONAL_HAS_VALUE:
            visit_OptionalHasValue(node);
            break;

        case AST_OPTIONAL_VALUE:
            visit_OptionalValue(node);
            break;

        case AST_OPTIONAL_VALUE_OR:
            visit_OptionalValueOr(node);
            break;

        // Phase 4: Hash Map operations
        case AST_MAP_TANIMLAMA:
            visit_MapTanimlama(node);
            break;

        case AST_MAP_SET:
            visit_MapSet(node);
            break;

        case AST_MAP_GET:
            visit_MapGet(node);
            break;

        case AST_MAP_HAS:
            visit_MapHas(node);
            break;

        case AST_MAP_REMOVE:
            visit_MapRemove(node);
            break;

        case AST_MAP_SIZE:
            visit_MapSize(node);
            break;

        case AST_MAP_CLEAR:
            visit_MapClear(node);
            break;

        // Phase 3: Built-in function call
        case AST_BUILTIN_CALL:
            visit_BuiltinCall(node);
            break;

        // Phase 5.4: Enum ve Switch
        case AST_ENUM_DECLARATION:
            visit_EnumTanimlama(node);
            break;

        case AST_SWITCH_KOMUTU:
            visit_Switch(node);
            break;

        case AST_CASE_KOMUTU:
            // Case'ler switch içinde işlenir
            break;

        default:
            fprintf(stderr, "HATA [Generator]: Tanınmayan AST düğüm tipi: %d\n", node->type);
            exit(1);
    }
}

// --- Ana Fonksiyon ---

// AsmCode yapısı için ayrılan belleği serbest bırakır
void free_asm_code(AsmCode* section) {
    if (section->code != NULL) {
        free(section->code);
        section->code = NULL;
    }
}

char* generate_asm(ASTNode* root) {
    // Kapsamı sıfırla
    kapsam_degisken_sayisi = 0;
    kapsam_yigin_ofseti = 0;
    etiket_sayaci = 0;
    metin_sayaci = 0;
    aktif_dongu_son_etiketi = -1;

    // Initialize sections
    data_section.code = NULL;
    data_section.size = 0;
    data_section.capacity = 0;

    text_section.code = NULL;
    text_section.size = 0;
    text_section.capacity = 0;
    
    lambda_section.code = NULL;
    lambda_section.size = 0;
    lambda_section.capacity = 0;

    // 1. .data bölümü
    asm_append(&data_section, "extern printf");
    asm_append(&data_section, "extern malloc");      // Phase 7.9: Heap allocation for closures
    asm_append(&data_section, "extern free");        // PHASE 8.7: Free AsyncState
    asm_append(&data_section, "extern strlen");      // ✅ Ekle
    asm_append(&data_section, "extern strcmp");      // ✅ Ekle
    asm_append(&data_section, "extern strstr");      // ✅ Ekle
    asm_append(&data_section, "extern tyd_substr");  // ✅ Ekle
    asm_append(&data_section, "extern tyd_strcat");  // ✅ Ekle
    asm_append(&data_section, "extern dosya_ac");
    asm_append(&data_section, "extern dosya_oku");
    asm_append(&data_section, "extern dosya_yaz");
    asm_append(&data_section, "extern dosya_kapat");
    asm_append(&data_section, "extern string_karakter_al");
    asm_append(&data_section, "extern string_alt");
    asm_append(&data_section, "extern karakter_kodu");
    asm_append(&data_section, "extern kodu_karaktere");
    asm_append(&data_section, "extern runtime_dizin_al");
    asm_append(&data_section, "extern runtime_dizin_al"); // Self-host için eklendi
    asm_append(&data_section, "extern tyd_fix_cwd"); // ✅ yeni

    // Phase 8: Promise and Event Loop functions (simple_runtime.c)
    asm_append(&data_section, "extern promise_create");
    asm_append(&data_section, "extern promise_resolve");
    asm_append(&data_section, "extern promise_reject");
    asm_append(&data_section, "extern promise_then");
    asm_append(&data_section, "extern promise_is_resolved");
    asm_append(&data_section, "extern promise_get_value");
    asm_append(&data_section, "extern promise_free");
    asm_append(&data_section, "extern event_loop_create");
    asm_append(&data_section, "extern event_loop_push_task");
    asm_append(&data_section, "extern event_loop_pop_task");
    asm_append(&data_section, "extern event_loop_run");
    asm_append(&data_section, "extern event_loop_stop");
    
    // Phase 8.8: Async I/O Primitives
    asm_append(&data_section, "extern async_sleep");
    asm_append(&data_section, "extern async_read_file");
    asm_append(&data_section, "extern async_write_file");
    asm_append(&data_section, "extern async_http_get");
    asm_append(&data_section, "extern promise_all");
    asm_append(&data_section, "extern promise_all_simple");

    // String fonksiyonları (runtime.c'deki wrapperlar)
    asm_append(&data_section, "extern string_birlestir");
    asm_append(&data_section, "extern string_karsilastir");
    asm_append(&data_section, "extern string_uzunluk");
    asm_append(&data_section, "extern string_esit_mi");
    asm_append(&data_section, "extern string_karakter_al");
    asm_append(&data_section, "extern string_alt");
    asm_append(&data_section, "extern string_length");
    asm_append(&data_section, "extern string_substring");
    asm_append(&data_section, "extern string_concat");
    asm_append(&data_section, "extern string_char_at");  // Phase 6.1: MLP Runtime helpers
    asm_append(&data_section, "extern char_code");       // Phase 6.1: MLP Runtime helpers
    asm_append(&data_section, "extern code_to_char");    // Phase 6.1: MLP Runtime helpers
    
    // Input functions (Phase 5.7)
    asm_append(&data_section, "extern read_input");
    asm_append(&data_section, "extern read_line");
    asm_append(&data_section, "extern read_int");
    
    // Type casting helpers (Phase 5.7)
    asm_append(&data_section, "extern num");
    asm_append(&data_section, "extern str");
    
    // Type introspection (Phase 5.7)
    asm_append(&data_section, "extern mlp_get_type");

    // GUI functions (gui_mock.c or real GUI backend)
    asm_append(&data_section, "extern gui_window_create");
    asm_append(&data_section, "extern gui_window_show");
    asm_append(&data_section, "extern gui_window_hide");
    asm_append(&data_section, "extern gui_window_destroy");
    asm_append(&data_section, "extern gui_button_create");
    asm_append(&data_section, "extern gui_label_create");
    asm_append(&data_section, "extern gui_label_set_text");
    asm_append(&data_section, "extern gui_poll_event");
    asm_append(&data_section, "extern gui_get_clicked_button");
    asm_append(&data_section, "extern gui_canvas_create");
    asm_append(&data_section, "extern gui_canvas_clear");
    asm_append(&data_section, "extern gui_canvas_set_color");
    asm_append(&data_section, "extern gui_canvas_draw_rect");
    asm_append(&data_section, "extern gui_canvas_draw_circle");
    asm_append(&data_section, "extern gui_canvas_render");
    asm_append(&data_section, "extern int_to_string");
    asm_append(&data_section, "extern string_to_int");

    // Phase 5.2: Error Handling
    asm_append(&data_section, "extern exit_with_code");
    asm_append(&data_section, "extern panic");
    asm_append(&data_section, "extern mlp_assert");
    asm_append(&data_section, "extern get_error_code");
    asm_append(&data_section, "extern set_error_code");

    // Phase 5.2: Memory Management
    asm_append(&data_section, "extern mlp_malloc");
    asm_append(&data_section, "extern mlp_free");
    asm_append(&data_section, "extern mlp_realloc");
    asm_append(&data_section, "extern mlp_calloc");
    asm_append(&data_section, "extern get_allocated_bytes");
    asm_append(&data_section, "extern check_memory_leaks");

    // Phase 5.2: System Utilities
    asm_append(&data_section, "extern get_env");
    asm_append(&data_section, "extern current_timestamp");
    asm_append(&data_section, "extern sleep_ms");

    // Phase 5.3: Binary File I/O
    asm_append(&data_section, "extern read_binary");
    asm_append(&data_section, "extern write_binary");
    asm_append(&data_section, "extern get_file_info");
    asm_append(&data_section, "extern copy_file");

    // Phase 5.3: Directory Operations
    asm_append(&data_section, "extern list_directory");
    asm_append(&data_section, "extern create_directory");
    asm_append(&data_section, "extern remove_directory");
    asm_append(&data_section, "extern directory_exists");
    asm_append(&data_section, "extern get_current_dir");
    asm_append(&data_section, "extern change_directory");

    // Phase 5.3: Process Control
    asm_append(&data_section, "extern execute_command");
    asm_append(&data_section, "extern get_command_output");
    asm_append(&data_section, "extern get_process_id");
    asm_append(&data_section, "extern get_parent_process_id");

    // Phase 5.3: Time & Date Utilities
    asm_append(&data_section, "extern format_timestamp");
    asm_append(&data_section, "extern parse_timestamp");
    asm_append(&data_section, "extern get_milliseconds");
    asm_append(&data_section, "extern get_time_string");

    // Phase 5.3: Path Utilities
    asm_append(&data_section, "extern join_path");
    asm_append(&data_section, "extern get_file_extension");
    asm_append(&data_section, "extern get_file_name");
    asm_append(&data_section, "extern get_directory");

    // Phase 6: List (Dynamic Array) Functions
    asm_append(&data_section, "extern mlp_list_create");
    asm_append(&data_section, "extern mlp_list_add");
    asm_append(&data_section, "extern mlp_list_get");
    asm_append(&data_section, "extern mlp_list_set");
    asm_append(&data_section, "extern mlp_list_remove");
    asm_append(&data_section, "extern mlp_list_insert");
    asm_append(&data_section, "extern mlp_list_clear");
    asm_append(&data_section, "extern mlp_list_length");
    asm_append(&data_section, "extern mlp_list_free");

    // Phase 4: Hash Map Functions
    asm_append(&data_section, "extern hashmap_create");
    asm_append(&data_section, "extern hashmap_put");
    asm_append(&data_section, "extern hashmap_get");
    asm_append(&data_section, "extern hashmap_remove");
    asm_append(&data_section, "extern hashmap_contains");
    asm_append(&data_section, "extern hashmap_size");
    asm_append(&data_section, "extern hashmap_clear");
    asm_append(&data_section, "extern hashmap_destroy");

    asm_append(&data_section, "section .data");
    asm_append(&data_section, "    format_sayi db \"%ld\", 10, 0"); // %d -> %ld
    asm_append(&data_section, "    format_metin db \"%s\", 10, 0");
    // Phase 8.6: Global event loop pointer for async main
    asm_append(&data_section, "    global_event_loop_ptr: dq 0");
    asm_append(&data_section, "    global_async_state_ptr: dq 0  ; PHASE 8.7: Current AsyncState for continuations");

    // 2. .text bölümü başlangıcı
    asm_append(&text_section, "section .text");
    asm_append(&text_section, "global main");

    // 3. Ana program girişi (Prolog)
    asm_append(&text_section, "main:");
    asm_append(&text_section, "    push rbp");
    asm_append(&text_section, "    mov rbp, rsp");
    asm_append(&text_section, "    sub rsp, 256"); // ✅ Yeterli stack alanı (32 değişkene kadar)
    asm_append(&text_section, "    call tyd_fix_cwd"); // ✅ çalışma dizinini düzelt

    // 4. İki geçişli ziyaret
    ASTNode* blok = root;
    if (blok->type == AST_BLOK) {
        // İlk geçiş: Ana program komutları (fonksiyon tanımları hariç)
        asm_append(&text_section, "    ; --- Ana Program Akışı ---");
        
        // Check if there's a main function - if yes, call it
        int has_main = 0;
        int main_is_async = 0;
        for (int i = 0; i < blok->blok_data.sayisi; i++) {
            ASTNode* node = blok->blok_data.komutlar[i];
            if ((node->type == AST_FUNCTION_DECLARATION || node->type == AST_ASYNC_FUNCTION)) {
                char* func_name = (node->type == AST_FUNCTION_DECLARATION) 
                    ? node->islec_tanimlama_data.ad->value 
                    : node->async_function_data.ad->value;
                if (strcmp(func_name, "main") == 0) {
                    has_main = 1;
                    main_is_async = (node->type == AST_ASYNC_FUNCTION);
                    break;
                }
            }
        }
        
        if (has_main) {
            if (main_is_async) {
                // Phase 8.6: Async main with blocking await
                // Note: Currently using blocking await in state machine
                // Future: Will use event loop for true non-blocking
                asm_append(&text_section, "    ; Async main (blocking await for now)");
                asm_append(&text_section, "    call mlp_main");
                asm_append(&text_section, "    ; Wait for promise resolution (blocking)");
                asm_append(&text_section, "    mov rbx, rax  ; Save promise");
                asm_append(&text_section, "__main_wait:");
                asm_append(&text_section, "    mov rdi, rbx");
                asm_append(&text_section, "    call promise_is_resolved");
                asm_append(&text_section, "    test rax, rax");
                asm_append(&text_section, "    jz __main_wait  ; Wait until resolved");
                asm_append(&text_section, "    mov rdi, rbx");
                asm_append(&text_section, "    call promise_get_value");
            } else {
                // Regular synchronous main
                asm_append(&text_section, "    call mlp_main");
            }
        } else {
            // No main - execute top level commands
            for (int i = 0; i < blok->blok_data.sayisi; i++) {
                ASTNode* node = blok->blok_data.komutlar[i];
                if (node->type != AST_FUNCTION_DECLARATION && node->type != AST_ASYNC_FUNCTION) {
                    visit(node);
                }
            }
        }
    } else {
        visit(root);
    }

    // 5. Program Çıkışı (Epilog)
    asm_append(&text_section, "    ; --- Program Sonu ---");
    asm_append(&text_section, "    xor rax, rax");
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");

    // 6. İkinci geçiş: Fonksiyon tanımları
    if (blok->type == AST_BLOK) {
        asm_append(&text_section, "");
        asm_append(&text_section, "; === Fonksiyon Tanımları ===");
        for (int i = 0; i < blok->blok_data.sayisi; i++) {
            ASTNode* node = blok->blok_data.komutlar[i];
            if (node->type == AST_FUNCTION_DECLARATION || node->type == AST_ASYNC_FUNCTION) {
                visit(node);
            }
        }
    }

    // 7. Tüm bölümleri birleştir
    size_t stack_section_size = strlen("\nsection .note.GNU-stack noalloc noexec nowrite progbits\n\ndb 0\n");
    size_t lambda_size = (lambda_section.code != NULL) ? lambda_section.size : 0;
    size_t total_size = data_section.size + text_section.size + lambda_size + stack_section_size + 50;  // +50 for safety
    char* final_code = (char*)malloc(total_size);
    if (final_code == NULL) {
        perror("Hafıza ayırma hatası (final assembly)");
        exit(EXIT_FAILURE);
    }

    strcpy(final_code, data_section.code);
    strcat(final_code, "\n");
    strcat(final_code, text_section.code);
    
    // Phase 7.9: Append deferred lambda functions
    if (lambda_section.code != NULL && lambda_section.size > 0) {
        strcat(final_code, "\n; === Lambda Functions ===\n");
        strcat(final_code, lambda_section.code);
    }

    // ✅ NASM uyumlu stack notu + 1 byte içerik ekle (boş bölüm strip olmasın)
    strcat(final_code, "\nsection .note.GNU-stack noalloc noexec nowrite progbits\n");
    strcat(final_code, "\ndb 0\n");

    free_asm_code(&data_section);
    free_asm_code(&text_section);
    free_asm_code(&lambda_section);
    kapsam_temizle();

    return final_code;
}


// ========== c_backend.c ==========
// c_backend.c
// C Code Generator Backend for MLP Compiler
// Version: 1.0 - Prototype


// === Scope tracking for auto-free ===
#define MAX_SCOPE_VARS 100

static ScopeVariable scope_stack[MAX_SCOPE_VARS];
static int scope_depth = 0;

// === Statistics ===
static int generated_lines = 0;
static int generated_functions = 0;

// === Backend initialization ===
void c_backend_init(void) {
    scope_depth = 0;
    generated_lines = 0;
    generated_functions = 0;
    
    printf("🔵 C Backend initialized (v1.0)\n");
}

// === Backend cleanup ===
void c_backend_cleanup(void) {
    // Scope stack'i temizle
    for (int i = 0; i < scope_depth; i++) {
        free(scope_stack[i].var_name);
        free(scope_stack[i].var_type);
    }
    scope_depth = 0;
}

// === Main generation function ===
void c_backend_generate(ASTNode* root, const char* output_file) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "❌ Error: Cannot create output file: %s\n", output_file);
        return;
    }
    
    printf("🔨 Generating C code: %s\n", output_file);
    
    // 1. Header
    generate_c_header(f);
    
    // 2. Main program
    generate_c_program(f, root);
    
    fclose(f);
    
    printf("✅ C code generation complete (%d lines)\n", generated_lines);
}

// === Generate C header (includes) ===
void generate_c_header(FILE* f) {
    fprintf(f, "// Generated by MLP Compiler - C Backend v1.0\n");
    fprintf(f, "// Date: 22 Kasim 2025\n\n");
    
    
    // MLP runtime fonksiyonları (şimdilik inline tanımlayalım)
    fprintf(f, "// MLP Runtime functions\n");
    fprintf(f, "void mlp_print_int(int64_t value) {\n");
    fprintf(f, "    printf(\"%%ld\\n\", value);\n");
    fprintf(f, "}\n\n");
    
    fprintf(f, "void mlp_print_string(const char* value) {\n");
    fprintf(f, "    printf(\"%%s\\n\", value);\n");
    fprintf(f, "}\n\n");
    
    generated_lines += 12;
}

// === Generate main program ===
void generate_c_program(FILE* f, ASTNode* node) {
    if (node->type != AST_BLOK) {
        fprintf(stderr, "❌ Error: Root node must be AST_BLOK\n");
        return;
    }
    
    // Main function başlat
    fprintf(f, "int main(void) {\n");
    generated_lines++;
    
    // Tüm statement'ları üret
    for (int i = 0; i < node->blok_data.sayisi; i++) {
        generate_c_statement(f, node->blok_data.komutlar[i], 1);
    }
    
    // Cleanup (scope-based free)
    generate_scope_cleanup(f, 1);
    
    // Return
    write_indent(f, 1);
    fprintf(f, "return 0;\n");
    generated_lines++;
    
    fprintf(f, "}\n");
    generated_lines++;
}

// === Generate C statement ===
void generate_c_statement(FILE* f, ASTNode* node, int indent) {
    if (!node) return;
    
    switch (node->type) {
        case AST_VARIABLE_DECLARATION: {
            // int x = 42;
            write_indent(f, indent);
            
            const char* c_type = map_mlp_type_to_c(node->tanimlama_data.tip->value);
            fprintf(f, "%s %s", c_type, node->tanimlama_data.ad->value);
            
            if (node->tanimlama_data.ifade) {
                fprintf(f, " = ");
                generate_c_expression(f, node->tanimlama_data.ifade);
            }
            
            fprintf(f, ";\n");
            generated_lines++;
            
            // Track variable (for future scope-based free)
            scope_push_variable(
                node->tanimlama_data.ad->value,
                node->tanimlama_data.tip->value,
                false // int/string stack değişkenleri free gerektirmez
            );
            break;
        }
        
        case AST_PRINT_STATEMENT: {
            // print x → mlp_print_int(x);
            write_indent(f, indent);
            
            // Expression'ın tipini anlamak için basit kontrol (ileride type inference)
            ASTNode* expr = node->tek_ifade_data.ifade;
            
            if (expr->type == AST_SAYI) {
                fprintf(f, "mlp_print_int(");
            } else if (expr->type == AST_METIN) {
                fprintf(f, "mlp_print_string(");
            } else {
                // Default: int varsayalım (ileride type tracking eklenecek)
                fprintf(f, "mlp_print_int(");
            }
            
            generate_c_expression(f, expr);
            fprintf(f, ");\n");
            generated_lines++;
            break;
        }
        
        case AST_ASSIGNMENT_KOMUTU: {
            // x = 10;
            write_indent(f, indent);
            fprintf(f, "%s = ", node->atama_data.ad->value);
            generate_c_expression(f, node->atama_data.ifade);
            fprintf(f, ";\n");
            generated_lines++;
            break;
        }
        
        case AST_IF_STATEMENT_KOMUTU: {
            // if (condition) { ... } else { ... }
            write_indent(f, indent);
            fprintf(f, "if (");
            generate_c_expression(f, node->kosul_data.kosul);
            fprintf(f, ") {\n");
            generated_lines++;
            
            // Then bloğu
            if (node->kosul_data.ise_blok) {
                if (node->kosul_data.ise_blok->type == AST_BLOK) {
                    for (int i = 0; i < node->kosul_data.ise_blok->blok_data.sayisi; i++) {
                        generate_c_statement(f, node->kosul_data.ise_blok->blok_data.komutlar[i], indent + 1);
                    }
                } else {
                    generate_c_statement(f, node->kosul_data.ise_blok, indent + 1);
                }
            }
            
            write_indent(f, indent);
            fprintf(f, "}");
            generated_lines++;
            
            // Else bloğu
            if (node->kosul_data.degilse_blok) {
                fprintf(f, " else {\n");
                generated_lines++;
                
                if (node->kosul_data.degilse_blok->type == AST_BLOK) {
                    for (int i = 0; i < node->kosul_data.degilse_blok->blok_data.sayisi; i++) {
                        generate_c_statement(f, node->kosul_data.degilse_blok->blok_data.komutlar[i], indent + 1);
                    }
                } else {
                    generate_c_statement(f, node->kosul_data.degilse_blok, indent + 1);
                }
                
                write_indent(f, indent);
                fprintf(f, "}\n");
                generated_lines++;
            } else {
                fprintf(f, "\n");
            }
            break;
        }
        
        case AST_WHILE_LOOP_KOMUTU: {
            // while (condition) { ... } or while (1) { ... }
            write_indent(f, indent);
            
            if (node->dongu_data.kosul) {
                fprintf(f, "while (");
                generate_c_expression(f, node->dongu_data.kosul);
                fprintf(f, ") {\n");
            } else {
                // Infinite loop
                fprintf(f, "while (1) {\n");
            }
            generated_lines++;
            
            // Loop body
            if (node->dongu_data.govde) {
                if (node->dongu_data.govde->type == AST_BLOK) {
                    for (int i = 0; i < node->dongu_data.govde->blok_data.sayisi; i++) {
                        generate_c_statement(f, node->dongu_data.govde->blok_data.komutlar[i], indent + 1);
                    }
                } else {
                    generate_c_statement(f, node->dongu_data.govde, indent + 1);
                }
            }
            
            write_indent(f, indent);
            fprintf(f, "}\n");
            generated_lines++;
            break;
        }
        
        case AST_WHILE_LOOP_BITIR_KOMUTU: {
            // break;
            write_indent(f, indent);
            fprintf(f, "break;\n");
            generated_lines++;
            break;
        }
        
        case AST_FUNCTION_DECLARATION: {
            // Function definition (şimdilik basit versiyon)
            fprintf(f, "\n// Function: %s (TODO: implement)\n", 
                    node->islec_tanimlama_data.ad->value);
            generated_lines++;
            generated_functions++;
            break;
        }
        
        default:
            write_indent(f, indent);
            fprintf(f, "// TODO: AST node type %d not implemented yet\n", node->type);
            generated_lines++;
            break;
    }
}

// === Generate C expression ===
void generate_c_expression(FILE* f, ASTNode* node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_SAYI:
            // Number literal: 42
            fprintf(f, "%s", node->sabit_data.deger);
            break;
            
        case AST_METIN:
            // String literal: "Hello"
            fprintf(f, "\"%s\"", node->sabit_data.deger);
            break;
            
        case AST_DEGISKEN:
            // Variable reference: x
            fprintf(f, "%s", node->degisken_data.ad->value);
            break;
            
        case AST_IKILI_ISLEM: {
            // Binary operation: a + b
            fprintf(f, "(");
            generate_c_expression(f, node->ikili_islem_data.sol);
            
            // Operator mapping
            switch (node->ikili_islem_data.operator_type) {
                case TOKEN_PLUS: fprintf(f, " + "); break;
                case TOKEN_MINUS: fprintf(f, " - "); break;
                case TOKEN_MUL: fprintf(f, " * "); break;
                case TOKEN_DIV: fprintf(f, " / "); break;
                case TOKEN_MOD: fprintf(f, " %% "); break;
                case TOKEN_OP_ESIT_KARSILASTIRMA: fprintf(f, " == "); break;
                case TOKEN_NOT_ESIT: fprintf(f, " != "); break;
                case TOKEN_GT: fprintf(f, " > "); break;
                case TOKEN_LT: fprintf(f, " < "); break;
                case TOKEN_GTE: fprintf(f, " >= "); break;
                case TOKEN_LTE: fprintf(f, " <= "); break;
                case TOKEN_AND: fprintf(f, " && "); break;
                case TOKEN_OR: fprintf(f, " || "); break;
                default: fprintf(f, " /* UNKNOWN_OP */ "); break;
            }
            
            generate_c_expression(f, node->ikili_islem_data.sag);
            fprintf(f, ")");
            break;
        }
        
        default:
            fprintf(f, "/* UNIMPLEMENTED_EXPR_%d */", node->type);
            break;
    }
}

// === Type mapping ===
const char* map_mlp_type_to_c(const char* mlp_type) {
    // MLP tip → C tip
    if (strcmp(mlp_type, "SAYISAL") == 0 || strcmp(mlp_type, "int") == 0) {
        return "int64_t";
    } else if (strcmp(mlp_type, "METIN") == 0 || strcmp(mlp_type, "string") == 0) {
        return "const char*";
    } else if (strcmp(mlp_type, "BOOL") == 0 || strcmp(mlp_type, "bool") == 0) {
        return "bool";
    }
    
    // Unknown type - default to int64_t
    return "int64_t";
}

// === Indentation helper ===
void write_indent(FILE* f, int level) {
    for (int i = 0; i < level; i++) {
        fprintf(f, "    "); // 4 spaces per indent
    }
}

// === Scope tracking ===
void scope_push_variable(const char* var_name, const char* var_type, bool needs_free) {
    if (scope_depth >= MAX_SCOPE_VARS) {
        fprintf(stderr, "⚠️  Warning: Scope stack overflow\n");
        return;
    }
    
    scope_stack[scope_depth].var_name = strdup(var_name);
    scope_stack[scope_depth].var_type = strdup(var_type);
    scope_stack[scope_depth].needs_free = needs_free;
    scope_depth++;
}

void scope_pop_all(void) {
    for (int i = 0; i < scope_depth; i++) {
        free(scope_stack[i].var_name);
        free(scope_stack[i].var_type);
    }
    scope_depth = 0;
}

void generate_scope_cleanup(FILE* f, int indent) {
    bool has_cleanup = false;
    
    // Check if any variables need cleanup
    for (int i = 0; i < scope_depth; i++) {
        if (scope_stack[i].needs_free) {
            has_cleanup = true;
            break;
        }
    }
    
    if (!has_cleanup) {
        return; // No cleanup needed
    }
    
    fprintf(f, "\n");
    write_indent(f, indent);
    fprintf(f, "// Auto-generated cleanup\n");
    generated_lines++;
    
    for (int i = scope_depth - 1; i >= 0; i--) {
        if (scope_stack[i].needs_free) {
            write_indent(f, indent);
            
            // Type-specific cleanup (şimdilik sadece list<T>)
            if (strstr(scope_stack[i].var_type, "list") != NULL) {
                fprintf(f, "mlp_list_free(%s);\n", scope_stack[i].var_name);
            }
            // Gelecek: string, map, vb.
            
            generated_lines++;
        }
    }
}

// === Print statistics ===
static void c_backend_print_stats(void) {
    printf("📊 C Backend Statistics:\n");
    printf("   - Generated lines: %d\n", generated_lines);
    printf("   - Generated functions: %d\n", generated_functions);
}

// === Backend interface implementation ===
static Backend c_backend_interface = {
    .name = "C",
    .version = "1.0",
    .file_extension = ".c",
    .description = "C code generator (Community Edition)",
    .capabilities = {
        .supports_auto_free = true,
        .supports_cross_platform = true,
        .supports_debugging = true,
        .requires_external_tools = true // GCC/Clang gerekiyor
    },
    .initialize = c_backend_init,
    .generate = c_backend_generate,
    .cleanup = c_backend_cleanup,
    .print_stats = c_backend_print_stats
};

Backend* get_c_backend(void) {
    return &c_backend_interface;
}

// ===== Phase 6.2: Optional<T> Visitor Stubs (Temporary) =====

void visit_OptionalTanimlama(ASTNode* node) {
    // Phase 6.2: Generic Optional<T> → Struct-based implementation
    // optional<numeric> x → OptionalNumeric x
    // optional<string> y → OptionalString y
    
    Token* element_tip = node->optional_tanimlama_data.element_tipi;
    Token* degisken_adi = node->optional_tanimlama_data.degisken_adi;
    
    // Determine struct name based on type
    char struct_name[64];
    if (strcmp(element_tip->value, "numeric") == 0) {
        strcpy(struct_name, "OptionalNumeric");
    } else if (strcmp(element_tip->value, "string") == 0) {
        strcpy(struct_name, "OptionalString");
    } else if (strcmp(element_tip->value, "boolean") == 0) {
        strcpy(struct_name, "OptionalBoolean");
    } else {
        // Custom type
        sprintf(struct_name, "Optional%s", element_tip->value);
    }
    
    // Generate struct variable declaration
    char buffer[256];
    sprintf(buffer, "    ; Optional<%s> %s -> %s %s", 
            element_tip->value, degisken_adi->value,
            struct_name, degisken_adi->value);
    asm_append(&text_section, buffer);
    
    // Allocate space on stack for struct (2 fields: value + has_value)
    // numeric/string = 8 bytes, boolean = 8 bytes (aligned)
    // Total: 16 bytes per optional
    kapsam_yigin_ofseti += 16;
    sprintf(buffer, "    sub rsp, 16  ; Allocate space for %s", struct_name);
    asm_append(&text_section, buffer);
    
    // Initialize has_value = false (0) - second field
    sprintf(buffer, "    mov qword [rbp - %d], 0  ; has_value = false", 
            kapsam_yigin_ofseti);
    asm_append(&text_section, buffer);
    
    // Initialize value = 0 (default) - first field
    sprintf(buffer, "    mov qword [rbp - %d], 0  ; value = 0 (default)", 
            kapsam_yigin_ofseti - 8);
    asm_append(&text_section, buffer);
    
    // Register variable in symbol table
    Degisken degisken;
    degisken.ad = strdup(degisken_adi->value);
    sprintf(buffer, "[rbp - %d]", kapsam_yigin_ofseti - 8);
    degisken.asm_adresi = strdup(buffer);
    degisken.tip = strdup(struct_name);
    degisken.scope_level = current_scope_level;
    degisken.is_global = false;
    degisken.is_const = false;
    degisken.array_boyut = 0;
    kapsam_haritasi[kapsam_degisken_sayisi++] = degisken;
}

void visit_OptionalHasValue(ASTNode* node) {
    // optional.has_value() → check has_value field
    Token* optional_adi = node->optional_has_value_data.optional_adi;
    
    // Find variable in symbol table
    int found = 0;
    int offset = 0;
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, optional_adi->value) == 0) {
            // Extract offset from "[rbp - X]"
            sscanf(kapsam_haritasi[i].asm_adresi, "[rbp - %d]", &offset);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        char buffer[256];
        sprintf(buffer, "    ; ERROR: Variable not found: %s", optional_adi->value);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    mov rax, 0");
        return;
    }
    
    // Load has_value field (offset + 8 from value field)
    char buffer[256];
    sprintf(buffer, "    mov rax, [rbp - %d]  ; %s.has_value", offset + 8, optional_adi->value);
    asm_append(&text_section, buffer);
}

void visit_OptionalValue(ASTNode* node) {
    // optional.value() → return value field
    Token* optional_adi = node->optional_value_data.optional_adi;
    
    // Find variable in symbol table
    int found = 0;
    int offset = 0;
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, optional_adi->value) == 0) {
            // Extract offset from "[rbp - X]"
            sscanf(kapsam_haritasi[i].asm_adresi, "[rbp - %d]", &offset);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        char buffer[256];
        sprintf(buffer, "    ; ERROR: Variable not found: %s", optional_adi->value);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    mov rax, 0");
        return;
    }
    
    // Load value field
    char buffer[256];
    sprintf(buffer, "    mov rax, [rbp - %d]  ; %s.value", offset, optional_adi->value);
    asm_append(&text_section, buffer);
}

void visit_OptionalValueOr(ASTNode* node) {
    // optional.value_or(default) → return value if has_value, else default
    Token* optional_adi = node->optional_value_or_data.optional_adi;
    ASTNode* default_deger = node->optional_value_or_data.default_deger;
    
    // Find variable in symbol table
    int found = 0;
    int offset = 0;
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, optional_adi->value) == 0) {
            // Extract offset from "[rbp - X]"
            sscanf(kapsam_haritasi[i].asm_adresi, "[rbp - %d]", &offset);
            found = 1;
            break;
        }
    }
    
    if (!found) {
        char buffer[256];
        sprintf(buffer, "    ; ERROR: Variable not found: %s", optional_adi->value);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    mov rax, 0");
        return;
    }
    
    // Check has_value
    char buffer[256];
    sprintf(buffer, "    mov rax, [rbp - %d]  ; Load has_value", offset + 8);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    test rax, rax");
    
    // Generate unique labels
    static int value_or_counter = 0;
    char label_has_value[64], label_end[64];
    sprintf(label_has_value, ".optional_has_value_%d", value_or_counter);
    sprintf(label_end, ".optional_value_or_end_%d", value_or_counter);
    value_or_counter++;
    
    sprintf(buffer, "    jnz %s  ; Jump if has_value == true", label_has_value);
    asm_append(&text_section, buffer);
    
    // No value: evaluate and return default
    asm_append(&text_section, "    ; Return default value");
    visit(default_deger);  // Result in RAX
    sprintf(buffer, "    jmp %s", label_end);
    asm_append(&text_section, buffer);
    
    // Has value: return value field
    sprintf(buffer, "%s:", label_has_value);
    asm_append(&text_section, buffer);
    sprintf(buffer, "    mov rax, [rbp - %d]  ; Return actual value", offset);
    asm_append(&text_section, buffer);
    
    sprintf(buffer, "%s:", label_end);
    asm_append(&text_section, buffer);
}
