#include "preprocessor_parser.h"
#include <stdlib.h>
#include <string.h>

// Directive parse - TODO Stage 1
Directive* parse_directive(char* line) {
    // Preprocessor direktifini parse et
    // #define, #include, #if, #ifdef, #ifndef, #else, #endif, vb.
    
    // TODO: Stage 1'de implement edilecek
    (void)line;
    return NULL;
}

// Macro definition parse - TODO Stage 1
MacroDefinition* parse_macro_definition(char* line) {
    // Macro tanımını parse et
    // #define PI 3.14
    // #define MAX(a,b) ((a)>(b)?(a):(b))
    // #define LOG(fmt, ...) printf(fmt, __VA_ARGS__)
    
    // TODO: Stage 1'de implement edilecek
    (void)line;
    return NULL;
}

// Macro invocation parse - TODO Stage 1
MacroInvocation* parse_macro_invocation(char* text) {
    // Macro çağrısını parse et
    // MAX(x, y)
    // LOG("value: %d", x)
    
    // TODO: Stage 1'de implement edilecek
    (void)text;
    return NULL;
}

// Conditional parse - TODO Stage 1
Directive* parse_conditional(char* line) {
    // Conditional directive parse et
    // #if CONDITION
    // #ifdef MACRO
    // #ifndef MACRO
    // #elif CONDITION
    // #else
    // #endif
    
    // TODO: Stage 1'de implement edilecek
    (void)line;
    return NULL;
}

// Include parse - TODO Stage 1
Directive* parse_include(char* line) {
    // Include directive parse et
    // #include "file.h"
    // #include <stdlib.h>
    
    // TODO: Stage 1'de implement edilecek
    (void)line;
    return NULL;
}
