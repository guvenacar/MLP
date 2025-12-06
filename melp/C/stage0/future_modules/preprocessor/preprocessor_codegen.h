#ifndef PREPROCESSOR_CODEGEN_H
#define PREPROCESSOR_CODEGEN_H

#include "preprocessor.h"

// Preprocessor Codegen Interface
// x86-64 NASM assembly generation

// Macro expansion için assembly üretir
// Compile-time'da macro'lar genişletilir
void codegen_macro_expansion(MacroDefinition* macro, MacroInvocation* inv);

// Conditional compilation için assembly üretir
// #if/#ifdef ile işaretlenmiş kod blokları
void codegen_conditional_section(Directive* dir, char* code_block);

// Include dosyası için assembly üretir
// Include edilen dosyaları işle
void codegen_include(Directive* dir, PreprocessorContext* ctx);

// Token paste (##) için assembly üretir
// Token birleştirme işlemi
void codegen_token_paste(TokenPaste* paste);

// Stringification (#) için assembly üretir
// Token'ı string'e çevirme
void codegen_stringification(Stringification* str);

// Built-in macro değerleri için assembly üretir
// __FILE__, __LINE__, __DATE__, __TIME__
void codegen_builtin_macro(char* macro_name, PreprocessorContext* ctx);

#endif // PREPROCESSOR_CODEGEN_H
