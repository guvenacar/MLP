#ifndef EXCEPTION_HANDLING_CODEGEN_H
#define EXCEPTION_HANDLING_CODEGEN_H

#include "exception_handling.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_try_catch_finally(FILE* out, TryCatchFinally* tcf);
void codegen_catch_block(FILE* out, CatchBlock* cb, int index);
void codegen_throw(FILE* out, Throw* thr);
void codegen_exception_create(FILE* out, Exception* exc);

// Try-catch mekanizması
void codegen_try_setup(FILE* out);
void codegen_catch_dispatch(FILE* out, TryCatchFinally* tcf);
void codegen_finally_block(FILE* out, void* finally);
void codegen_exception_cleanup(FILE* out);

// Exception yardımcı fonksiyonlar
void codegen_exception_match(FILE* out, const char* type);
void codegen_exception_rethrow(FILE* out);
void codegen_unhandled_exception(FILE* out);

// setjmp/longjmp entegrasyonu
void codegen_setjmp(FILE* out);
void codegen_longjmp(FILE* out, int value);

#endif // EXCEPTION_HANDLING_CODEGEN_H
