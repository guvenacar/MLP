#ifndef PREPROCESSOR_PARSER_H
#define PREPROCESSOR_PARSER_H

#include "preprocessor.h"

// Preprocessor Parser Interface
// Stage 1'de implement edilecek

// Preprocessor direktifini parse eder
// Örnek: #define MAX(a,b) ((a)>(b)?(a):(b))
Directive* parse_directive(char* line);

// Macro tanımını parse eder
// Örnekler:
// - #define PI 3.14 (object-like)
// - #define MAX(a,b) ((a)>(b)?(a):(b)) (function-like)
// - #define LOG(fmt, ...) printf(fmt, __VA_ARGS__) (variadic)
MacroDefinition* parse_macro_definition(char* line);

// Macro çağrısını parse eder
// Örnek: MAX(x, y)
MacroInvocation* parse_macro_invocation(char* text);

// Conditional directive'i parse eder
// Örnekler:
// - #if CONDITION
// - #ifdef MACRO
// - #ifndef MACRO
Directive* parse_conditional(char* line);

// Include directive'i parse eder
// Örnekler:
// - #include "file.h"
// - #include <stdlib.h>
Directive* parse_include(char* line);

#endif // PREPROCESSOR_PARSER_H
