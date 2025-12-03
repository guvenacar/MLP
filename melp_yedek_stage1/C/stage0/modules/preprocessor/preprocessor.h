#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <stddef.h>
#include <stdbool.h>

// Preprocessor ve Macro Sistemi
// Compile-time code generation, macro expansion, preprocessor directives

// Macro türleri
typedef enum {
    MACRO_OBJECT_LIKE = 0,   // #define PI 3.14
    MACRO_FUNCTION_LIKE = 1, // #define MAX(a,b) ((a)>(b)?(a):(b))
    MACRO_VARIADIC = 2,      // #define LOG(fmt, ...) printf(fmt, __VA_ARGS__)
    MACRO_BUILTIN = 3        // __FILE__, __LINE__, __DATE__
} MacroType;

// Preprocessor direktif türleri
typedef enum {
    DIRECTIVE_DEFINE = 0,    // #define
    DIRECTIVE_UNDEF = 1,     // #undef
    DIRECTIVE_INCLUDE = 2,   // #include
    DIRECTIVE_IF = 3,        // #if
    DIRECTIVE_IFDEF = 4,     // #ifdef
    DIRECTIVE_IFNDEF = 5,    // #ifndef
    DIRECTIVE_ELIF = 6,      // #elif
    DIRECTIVE_ELSE = 7,      // #else
    DIRECTIVE_ENDIF = 8,     // #endif
    DIRECTIVE_ERROR = 9,     // #error
    DIRECTIVE_WARNING = 10,  // #warning
    DIRECTIVE_PRAGMA = 11,   // #pragma
    DIRECTIVE_LINE = 12      // #line
} DirectiveType;

// Macro parametresi
typedef struct MacroParam {
    char* name;              // Parametre adı
    bool is_variadic;        // ... variadic mi?
    char* default_value;     // Varsayılan değer (opsiyonel)
} MacroParam;

// Macro tanımı
typedef struct MacroDefinition {
    char* name;              // Macro adı
    MacroType type;          // Macro türü
    MacroParam** params;     // Parametreler (function-like için)
    size_t param_count;      // Parametre sayısı
    char* body;              // Macro gövdesi
    size_t expansion_count;  // Genişletme sayısı (debugging için)
    bool is_defined;         // Tanımlı mı?
    int line_number;         // Tanım satırı
    char* file_name;         // Tanım dosyası
} MacroDefinition;

// Macro çağrısı
typedef struct MacroInvocation {
    MacroDefinition* macro;  // Çağrılan macro
    char** arguments;        // Argümanlar
    size_t arg_count;        // Argüman sayısı
    char* expanded_text;     // Genişletilmiş metin
    int line_number;         // Çağrı satırı
} MacroInvocation;

// Preprocessor direktifi
typedef struct Directive {
    DirectiveType type;      // Direktif türü
    char* argument;          // Direktif argümanı
    void* condition;         // Koşul (if/ifdef için)
    bool is_active;          // Aktif mi?
    int line_number;         // Direktif satırı
} Directive;

// Conditional compilation stack
typedef struct ConditionStack {
    Directive** conditions;  // Koşul stack'i
    size_t depth;            // Stack derinliği
    size_t capacity;         // Stack kapasitesi
    bool* is_active;         // Her seviyede aktif mi?
} ConditionStack;

// Include guard
typedef struct IncludeGuard {
    char* macro_name;        // Guard macro adı
    char* file_path;         // Dosya yolu
    bool is_included;        // Include edildi mi?
} IncludeGuard;

// Preprocessor context
typedef struct PreprocessorContext {
    MacroDefinition** macros;     // Tanımlı macro'lar
    size_t macro_count;           // Macro sayısı
    size_t macro_capacity;        // Macro kapasitesi
    ConditionStack* cond_stack;   // Conditional stack
    IncludeGuard** guards;        // Include guard'lar
    size_t guard_count;           // Guard sayısı
    char** include_paths;         // Include path'leri
    size_t include_path_count;    // Path sayısı
    int current_line;             // Şu anki satır
    char* current_file;           // Şu anki dosya
} PreprocessorContext;

// Token paste operation
typedef struct TokenPaste {
    char* left_token;        // Sol token
    char* right_token;       // Sağ token
    char* result;            // Birleştirilmiş sonuç
} TokenPaste;

// Stringification operation
typedef struct Stringification {
    char* token;             // Token
    char* stringified;       // String'e çevrilmiş
} Stringification;

// Macro expansion result
typedef struct ExpansionResult {
    char* expanded_text;     // Genişletilmiş metin
    size_t length;           // Uzunluk
    bool has_error;          // Hata var mı?
    char* error_message;     // Hata mesajı
} ExpansionResult;

// Macro param işlemleri
MacroParam* macro_param_create(char* name, bool is_variadic);
void macro_param_free(MacroParam* param);

// Macro definition işlemleri
MacroDefinition* macro_definition_create(char* name, MacroType type);
void macro_definition_free(MacroDefinition* def);
void macro_definition_add_param(MacroDefinition* def, MacroParam* param);
void macro_definition_set_body(MacroDefinition* def, char* body);

// Macro invocation işlemleri
MacroInvocation* macro_invocation_create(MacroDefinition* macro);
void macro_invocation_free(MacroInvocation* inv);
void macro_invocation_add_arg(MacroInvocation* inv, char* arg);
ExpansionResult* macro_invocation_expand(MacroInvocation* inv);

// Directive işlemleri
Directive* directive_create(DirectiveType type);
void directive_free(Directive* dir);
void directive_set_argument(Directive* dir, char* arg);
bool directive_evaluate_condition(Directive* dir, PreprocessorContext* ctx);

// Condition stack işlemleri
ConditionStack* condition_stack_create(void);
void condition_stack_free(ConditionStack* stack);
void condition_stack_push(ConditionStack* stack, Directive* dir);
Directive* condition_stack_pop(ConditionStack* stack);
bool condition_stack_is_active(ConditionStack* stack);

// Include guard işlemleri
IncludeGuard* include_guard_create(char* macro_name, char* file_path);
void include_guard_free(IncludeGuard* guard);
bool include_guard_check(IncludeGuard* guard);

// Preprocessor context işlemleri
PreprocessorContext* preprocessor_context_create(void);
void preprocessor_context_free(PreprocessorContext* ctx);
void preprocessor_context_add_macro(PreprocessorContext* ctx, MacroDefinition* macro);
MacroDefinition* preprocessor_context_find_macro(PreprocessorContext* ctx, char* name);
void preprocessor_context_remove_macro(PreprocessorContext* ctx, char* name);
void preprocessor_context_add_include_path(PreprocessorContext* ctx, char* path);

// Token paste işlemleri
TokenPaste* token_paste_create(char* left, char* right);
void token_paste_free(TokenPaste* paste);
char* token_paste_execute(TokenPaste* paste);

// Stringification işlemleri
Stringification* stringification_create(char* token);
void stringification_free(Stringification* str);
char* stringification_execute(Stringification* str);

// Expansion işlemleri
ExpansionResult* expansion_result_create(void);
void expansion_result_free(ExpansionResult* result);

// Preprocessor ana işlemler
char* preprocess_file(char* file_path, PreprocessorContext* ctx);
char* preprocess_text(char* text, PreprocessorContext* ctx);
bool process_directive(char* line, PreprocessorContext* ctx);
char* expand_macros(char* text, PreprocessorContext* ctx);

// Built-in macro'lar
void register_builtin_macros(PreprocessorContext* ctx);
char* get_builtin_macro_value(char* name, PreprocessorContext* ctx);

#endif // PREPROCESSOR_H
