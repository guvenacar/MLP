/**
 * MODÜL #57: DECORATOR_SYSTEM
 * 
 * Python/TypeScript-style decorators for MLP
 * Function wrapping, method modification, class enhancement
 * 
 * STANDALONE MODULE - No dependencies on central compiler
 */

#ifndef DECORATOR_SYSTEM_H
#define DECORATOR_SYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// For strdup on some systems
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// ============================================================================
// DECORATOR TYPES
// ============================================================================

typedef enum {
    DECORATOR_FUNCTION,      // @decorator function_name()
    DECORATOR_METHOD,        // @decorator class.method()
    DECORATOR_CLASS,         // @decorator class ClassName
    DECORATOR_PROPERTY,      // @property getter/setter
    DECORATOR_STATIC_METHOD, // @staticmethod
    DECORATOR_CLASS_METHOD,  // @classmethod
    DECORATOR_ASYNC          // @async
} DecoratorType;

// ============================================================================
// DECORATOR ARGUMENT TYPES
// ============================================================================

typedef enum {
    ARG_NONE,
    ARG_STRING,
    ARG_NUMBER,
    ARG_BOOLEAN,
    ARG_ARRAY,
    ARG_OBJECT
} DecoratorArgType;

typedef struct DecoratorArg {
    DecoratorArgType type;
    union {
        char* string_val;
        double number_val;
        bool bool_val;
    } value;
    struct DecoratorArg* next;
} DecoratorArg;

// ============================================================================
// DECORATOR DEFINITION
// ============================================================================

typedef struct Decorator {
    char* name;              // Decorator name (e.g., "cached", "timeit")
    DecoratorType type;      // What it decorates
    DecoratorArg* args;      // Optional arguments
    char* target_name;       // Function/class/method name
    int line_number;         // Source location
    struct Decorator* next;  // For stacking decorators
} Decorator;

// ============================================================================
// DECORATOR CONTEXT (Compile-time state)
// ============================================================================

typedef struct {
    Decorator* decorators;           // List of all decorators
    int decorator_count;
    int function_decorator_count;
    int class_decorator_count;
    int property_decorator_count;
    int stacked_decorator_count;     // Multiple decorators on same target
} DecoratorContext;

// ============================================================================
// BUILT-IN DECORATORS
// ============================================================================

typedef enum {
    BUILTIN_CACHED,          // @cached - memoization
    BUILTIN_TIMEIT,          // @timeit - execution time
    BUILTIN_DEPRECATED,      // @deprecated - deprecation warning
    BUILTIN_READONLY,        // @readonly - immutable property
    BUILTIN_OVERRIDE,        // @override - method overriding check
    BUILTIN_SINGLETON,       // @singleton - single instance class
    BUILTIN_PROPERTY,        // @property - getter/setter
    BUILTIN_STATICMETHOD,    // @staticmethod - no self parameter
    BUILTIN_CLASSMETHOD,     // @classmethod - receives class as first param
    BUILTIN_ASYNC,           // @async - async function
    BUILTIN_SYNCHRONIZED,    // @synchronized - thread-safe
    BUILTIN_VALIDATE         // @validate - input validation
} BuiltinDecoratorType;

// ============================================================================
// DECORATOR METADATA (Runtime information)
// ============================================================================

typedef struct {
    char* original_name;     // Original function/class name
    char* wrapper_name;      // Generated wrapper function name
    bool has_cache;          // Uses caching?
    bool is_timed;           // Measures execution time?
    bool is_deprecated;      // Shows deprecation warning?
    bool is_readonly;        // Property is readonly?
    bool is_synchronized;    // Thread-safe?
    int call_count;          // Number of times called
} DecoratorMetadata;

// ============================================================================
// PUBLIC API
// ============================================================================

// Context management
DecoratorContext* decorator_context_new(void);
void decorator_context_free(DecoratorContext* ctx);

// Decorator creation
Decorator* decorator_new(const char* name, DecoratorType type, const char* target);
void decorator_add_arg(Decorator* dec, DecoratorArgType type, void* value);
void decorator_free(Decorator* dec);

// Decorator registration
void decorator_register(DecoratorContext* ctx, Decorator* dec);
Decorator* decorator_find(DecoratorContext* ctx, const char* target_name);
bool decorator_is_stacked(DecoratorContext* ctx, const char* target_name);

// Decorator arguments
DecoratorArg* decorator_arg_new(DecoratorArgType type, void* value);
void decorator_arg_free(DecoratorArg* arg);

// Built-in decorators
bool decorator_is_builtin(const char* name);
BuiltinDecoratorType decorator_get_builtin_type(const char* name);
const char* decorator_builtin_name(BuiltinDecoratorType type);

// Decorator metadata
DecoratorMetadata* decorator_metadata_new(const char* original_name);
void decorator_metadata_free(DecoratorMetadata* meta);

// Code generation helpers
char* decorator_generate_wrapper_name(const char* original_name, const char* decorator_name);
void decorator_generate_wrapper(DecoratorContext* ctx, Decorator* dec, FILE* out);

// Decorator validation
bool decorator_validate(DecoratorContext* ctx, Decorator* dec);
bool decorator_can_stack(Decorator* dec1, Decorator* dec2);

// Statistics
void decorator_print_stats(DecoratorContext* ctx);

// ============================================================================
// DECORATOR PATTERNS
// ============================================================================

// Memoization pattern
typedef struct {
    char* key;
    void* value;
    struct CacheEntry* next;
} CacheEntry;

typedef struct {
    CacheEntry* entries;
    int size;
    int capacity;
} DecoratorCache;

DecoratorCache* decorator_cache_new(int capacity);
void decorator_cache_put(DecoratorCache* cache, const char* key, void* value);
void* decorator_cache_get(DecoratorCache* cache, const char* key);
void decorator_cache_free(DecoratorCache* cache);

// Timing pattern
typedef struct {
    char* function_name;
    double total_time;
    int call_count;
    double avg_time;
} TimingInfo;

TimingInfo* decorator_timing_new(const char* function_name);
void decorator_timing_record(TimingInfo* timing, double elapsed);
void decorator_timing_print(TimingInfo* timing);
void decorator_timing_free(TimingInfo* timing);

// ============================================================================
// DEBUG & TESTING
// ============================================================================

void decorator_dump(Decorator* dec);
void decorator_context_dump(DecoratorContext* ctx);

#endif // DECORATOR_SYSTEM_H
