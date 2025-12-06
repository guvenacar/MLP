#ifndef DEBUG_FEATURES_CODEGEN_H
#define DEBUG_FEATURES_CODEGEN_H

#include "debug_features.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_debug_label(FILE* out, DebugLabel* label, DebugContext* ctx);
void codegen_debug_goto(FILE* out, DebugGoto* dgoto, DebugContext* ctx);
void codegen_debug_pause(FILE* out, DebugPause* pause, DebugContext* ctx);
void codegen_debug_print(FILE* out, DebugPrint* print, DebugContext* ctx);
void codegen_debug_if(FILE* out, DebugIf* dif, DebugContext* ctx);

// Debug mode control
void codegen_debug_mode_check(FILE* out, DebugContext* ctx);
void codegen_strip_debug(FILE* out);

// Label & goto
void codegen_label_definition(FILE* out, DebugLabel* label);
void codegen_goto_jump(FILE* out, DebugGoto* dgoto);

// Breakpoint
void codegen_breakpoint(FILE* out, DebugPause* pause);

// Debug print
void codegen_debug_var_print(FILE* out, DebugPrint* print);

// Debug if
void codegen_debug_if_condition(FILE* out, DebugIf* dif);

#endif // DEBUG_FEATURES_CODEGEN_H
