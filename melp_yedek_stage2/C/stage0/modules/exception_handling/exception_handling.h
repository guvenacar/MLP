#ifndef EXCEPTION_HANDLING_H
#define EXCEPTION_HANDLING_H

#include <stddef.h>
#include <setjmp.h>
#include "../../lexer.h"

// Exception tipi
typedef struct Exception {
    char* type;             // Exception tipi (ValueError, IOError, etc.)
    char* message;          // Hata mesajı
    char* file;             // Dosya adı
    int line;               // Satır numarası
    void* data;             // Ek veri
    struct Exception* cause;// Neden olan exception (chaining)
} Exception;

// Try-catch-finally bloğu
typedef struct TryCatchFinally {
    void* try_block;        // Try bloğu
    void* catch_blocks;     // Catch blokları (array)
    int catch_count;        // Catch sayısı
    void* finally_block;    // Finally bloğu
    jmp_buf* jump_buffer;   // setjmp/longjmp için
} TryCatchFinally;

// Catch bloğu
typedef struct CatchBlock {
    char* exception_type;   // Yakalanan exception tipi
    char* var_name;         // Exception değişken adı
    void* handler;          // Handler kodu
} CatchBlock;

// Throw ifadesi
typedef struct Throw {
    Exception* exception;   // Fırlatılan exception
} Throw;

// Exception context (stack)
typedef struct ExceptionContext {
    TryCatchFinally** stack;    // Try-catch stack
    int stack_size;             // Stack boyutu
    int stack_capacity;         // Stack kapasitesi
    Exception* current;         // Mevcut exception
} ExceptionContext;

// Exception handler callback
typedef void (*ExceptionHandler)(Exception*);

// API fonksiyonları
Exception* exception_create(const char* type, const char* message);
TryCatchFinally* try_catch_finally_create(void);
CatchBlock* catch_block_create(const char* type, const char* var_name);
Throw* throw_create(Exception* exception);
ExceptionContext* exception_context_create(void);

void exception_free(Exception* exc);
void try_catch_finally_free(TryCatchFinally* tcf);
void catch_block_free(CatchBlock* cb);
void throw_free(Throw* thr);
void exception_context_free(ExceptionContext* ctx);

// Exception operasyonları
void exception_set_location(Exception* exc, const char* file, int line);
void exception_set_cause(Exception* exc, Exception* cause);
void exception_print(Exception* exc);

// Try-catch operasyonları
void try_catch_add_catch(TryCatchFinally* tcf, CatchBlock* cb);
void try_catch_set_finally(TryCatchFinally* tcf, void* finally_block);

// Context operasyonları
void exception_context_push(ExceptionContext* ctx, TryCatchFinally* tcf);
TryCatchFinally* exception_context_pop(ExceptionContext* ctx);
TryCatchFinally* exception_context_peek(ExceptionContext* ctx);
void exception_context_set_current(ExceptionContext* ctx, Exception* exc);
Exception* exception_context_get_current(ExceptionContext* ctx);

// Global exception handler
void set_global_exception_handler(ExceptionHandler handler);
void trigger_global_exception_handler(Exception* exc);

#endif // EXCEPTION_HANDLING_H
