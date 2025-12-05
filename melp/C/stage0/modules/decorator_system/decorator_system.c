/**
 * MODÜL #57: DECORATOR_SYSTEM - Core Implementation
 * 
 * Decorator engine, built-in decorators, metadata management
 */

#define _GNU_SOURCE
#include "decorator_system.h"
#include <time.h>

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

DecoratorContext* decorator_context_new(void) {
    DecoratorContext* ctx = malloc(sizeof(DecoratorContext));
    ctx->decorators = NULL;
    ctx->decorator_count = 0;
    ctx->function_decorator_count = 0;
    ctx->class_decorator_count = 0;
    ctx->property_decorator_count = 0;
    ctx->stacked_decorator_count = 0;
    return ctx;
}

void decorator_context_free(DecoratorContext* ctx) {
    Decorator* current = ctx->decorators;
    while (current) {
        Decorator* next = current->next;
        decorator_free(current);
        current = next;
    }
    free(ctx);
}

// ============================================================================
// DECORATOR CREATION
// ============================================================================

Decorator* decorator_new(const char* name, DecoratorType type, const char* target) {
    Decorator* dec = malloc(sizeof(Decorator));
    dec->name = strdup(name);
    dec->type = type;
    dec->target_name = strdup(target);
    dec->args = NULL;
    dec->line_number = 0;
    dec->next = NULL;
    return dec;
}

void decorator_add_arg(Decorator* dec, DecoratorArgType type, void* value) {
    DecoratorArg* arg = decorator_arg_new(type, value);
    
    if (!dec->args) {
        dec->args = arg;
    } else {
        DecoratorArg* current = dec->args;
        while (current->next) {
            current = current->next;
        }
        current->next = arg;
    }
}

void decorator_free(Decorator* dec) {
    free(dec->name);
    free(dec->target_name);
    
    DecoratorArg* current_arg = dec->args;
    while (current_arg) {
        DecoratorArg* next = current_arg->next;
        decorator_arg_free(current_arg);
        current_arg = next;
    }
    
    free(dec);
}

// ============================================================================
// DECORATOR REGISTRATION
// ============================================================================

void decorator_register(DecoratorContext* ctx, Decorator* dec) {
    // Check if stacking decorators
    if (decorator_find(ctx, dec->target_name)) {
        ctx->stacked_decorator_count++;
    }
    
    // Add to list
    dec->next = ctx->decorators;
    ctx->decorators = dec;
    ctx->decorator_count++;
    
    // Update type counts
    switch (dec->type) {
        case DECORATOR_FUNCTION:
        case DECORATOR_ASYNC:
            ctx->function_decorator_count++;
            break;
        case DECORATOR_CLASS:
            ctx->class_decorator_count++;
            break;
        case DECORATOR_PROPERTY:
            ctx->property_decorator_count++;
            break;
        default:
            break;
    }
}

Decorator* decorator_find(DecoratorContext* ctx, const char* target_name) {
    Decorator* current = ctx->decorators;
    while (current) {
        if (strcmp(current->target_name, target_name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

bool decorator_is_stacked(DecoratorContext* ctx, const char* target_name) {
    int count = 0;
    Decorator* current = ctx->decorators;
    while (current) {
        if (strcmp(current->target_name, target_name) == 0) {
            count++;
        }
        current = current->next;
    }
    return count > 1;
}

// ============================================================================
// DECORATOR ARGUMENTS
// ============================================================================

DecoratorArg* decorator_arg_new(DecoratorArgType type, void* value) {
    DecoratorArg* arg = malloc(sizeof(DecoratorArg));
    arg->type = type;
    arg->next = NULL;
    
    switch (type) {
        case ARG_STRING:
            arg->value.string_val = strdup((char*)value);
            break;
        case ARG_NUMBER:
            arg->value.number_val = *(double*)value;
            break;
        case ARG_BOOLEAN:
            arg->value.bool_val = *(bool*)value;
            break;
        default:
            break;
    }
    
    return arg;
}

void decorator_arg_free(DecoratorArg* arg) {
    if (arg->type == ARG_STRING) {
        free(arg->value.string_val);
    }
    free(arg);
}

// ============================================================================
// BUILT-IN DECORATORS
// ============================================================================

bool decorator_is_builtin(const char* name) {
    const char* builtins[] = {
        "cached", "timeit", "deprecated", "readonly", "override",
        "singleton", "property", "staticmethod", "classmethod",
        "async", "synchronized", "validate"
    };
    
    for (int i = 0; i < 12; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

BuiltinDecoratorType decorator_get_builtin_type(const char* name) {
    if (strcmp(name, "cached") == 0) return BUILTIN_CACHED;
    if (strcmp(name, "timeit") == 0) return BUILTIN_TIMEIT;
    if (strcmp(name, "deprecated") == 0) return BUILTIN_DEPRECATED;
    if (strcmp(name, "readonly") == 0) return BUILTIN_READONLY;
    if (strcmp(name, "override") == 0) return BUILTIN_OVERRIDE;
    if (strcmp(name, "singleton") == 0) return BUILTIN_SINGLETON;
    if (strcmp(name, "property") == 0) return BUILTIN_PROPERTY;
    if (strcmp(name, "staticmethod") == 0) return BUILTIN_STATICMETHOD;
    if (strcmp(name, "classmethod") == 0) return BUILTIN_CLASSMETHOD;
    if (strcmp(name, "async") == 0) return BUILTIN_ASYNC;
    if (strcmp(name, "synchronized") == 0) return BUILTIN_SYNCHRONIZED;
    if (strcmp(name, "validate") == 0) return BUILTIN_VALIDATE;
    return BUILTIN_CACHED; // Default
}

const char* decorator_builtin_name(BuiltinDecoratorType type) {
    switch (type) {
        case BUILTIN_CACHED: return "cached";
        case BUILTIN_TIMEIT: return "timeit";
        case BUILTIN_DEPRECATED: return "deprecated";
        case BUILTIN_READONLY: return "readonly";
        case BUILTIN_OVERRIDE: return "override";
        case BUILTIN_SINGLETON: return "singleton";
        case BUILTIN_PROPERTY: return "property";
        case BUILTIN_STATICMETHOD: return "staticmethod";
        case BUILTIN_CLASSMETHOD: return "classmethod";
        case BUILTIN_ASYNC: return "async";
        case BUILTIN_SYNCHRONIZED: return "synchronized";
        case BUILTIN_VALIDATE: return "validate";
        default: return "unknown";
    }
}

// ============================================================================
// DECORATOR METADATA
// ============================================================================

DecoratorMetadata* decorator_metadata_new(const char* original_name) {
    DecoratorMetadata* meta = malloc(sizeof(DecoratorMetadata));
    meta->original_name = strdup(original_name);
    meta->wrapper_name = NULL;
    meta->has_cache = false;
    meta->is_timed = false;
    meta->is_deprecated = false;
    meta->is_readonly = false;
    meta->is_synchronized = false;
    meta->call_count = 0;
    return meta;
}

void decorator_metadata_free(DecoratorMetadata* meta) {
    free(meta->original_name);
    if (meta->wrapper_name) free(meta->wrapper_name);
    free(meta);
}

// ============================================================================
// CODE GENERATION HELPERS
// ============================================================================

char* decorator_generate_wrapper_name(const char* original_name, const char* decorator_name) {
    char* wrapper = malloc(256);
    snprintf(wrapper, 256, "__%s_%s_wrapper", decorator_name, original_name);
    return wrapper;
}

void decorator_generate_wrapper(DecoratorContext* ctx, Decorator* dec, FILE* out) {
    const char* wrapper_name = decorator_generate_wrapper_name(dec->target_name, dec->name);
    
    fprintf(out, "; Decorator wrapper for @%s on %s\n", dec->name, dec->target_name);
    fprintf(out, "%s:\n", wrapper_name);
    
    BuiltinDecoratorType builtin = decorator_get_builtin_type(dec->name);
    
    switch (builtin) {
        case BUILTIN_TIMEIT:
            fprintf(out, "    ; Save start time\n");
            fprintf(out, "    rdtsc\n");
            fprintf(out, "    push rax\n");
            fprintf(out, "    call %s\n", dec->target_name);
            fprintf(out, "    ; Calculate elapsed\n");
            fprintf(out, "    rdtsc\n");
            fprintf(out, "    pop rbx\n");
            fprintf(out, "    sub rax, rbx\n");
            break;
            
        case BUILTIN_DEPRECATED:
            fprintf(out, "    ; Print deprecation warning\n");
            fprintf(out, "    lea rdi, [rel deprecated_msg]\n");
            fprintf(out, "    call puts\n");
            fprintf(out, "    call %s\n", dec->target_name);
            break;
            
        case BUILTIN_CACHED:
            fprintf(out, "    ; Check cache\n");
            fprintf(out, "    lea rdi, [rel cache_table]\n");
            fprintf(out, "    call cache_lookup\n");
            fprintf(out, "    test rax, rax\n");
            fprintf(out, "    jnz .cache_hit\n");
            fprintf(out, "    call %s\n", dec->target_name);
            fprintf(out, "    ; Store in cache\n");
            fprintf(out, "    call cache_store\n");
            fprintf(out, ".cache_hit:\n");
            break;
            
        default:
            fprintf(out, "    call %s\n", dec->target_name);
            break;
    }
    
    fprintf(out, "    ret\n\n");
}

// ============================================================================
// DECORATOR VALIDATION
// ============================================================================

bool decorator_validate(DecoratorContext* ctx, Decorator* dec) {
    // Check if decorator name is valid
    if (!dec->name || strlen(dec->name) == 0) {
        return false;
    }
    
    // Check if target exists
    if (!dec->target_name || strlen(dec->target_name) == 0) {
        return false;
    }
    
    // Validate decorator type compatibility
    if (dec->type == DECORATOR_PROPERTY) {
        // Property decorators can only be on getter/setter methods
        if (!strstr(dec->target_name, "get_") && !strstr(dec->target_name, "set_")) {
            return false;
        }
    }
    
    return true;
}

bool decorator_can_stack(Decorator* dec1, Decorator* dec2) {
    // Some decorators cannot be stacked together
    
    // @staticmethod and @classmethod are mutually exclusive
    if ((strcmp(dec1->name, "staticmethod") == 0 && strcmp(dec2->name, "classmethod") == 0) ||
        (strcmp(dec1->name, "classmethod") == 0 && strcmp(dec2->name, "staticmethod") == 0)) {
        return false;
    }
    
    // @property cannot be stacked with function decorators
    if ((strcmp(dec1->name, "property") == 0 && dec2->type == DECORATOR_FUNCTION) ||
        (strcmp(dec2->name, "property") == 0 && dec1->type == DECORATOR_FUNCTION)) {
        return false;
    }
    
    return true;
}

// ============================================================================
// STATISTICS
// ============================================================================

void decorator_print_stats(DecoratorContext* ctx) {
    printf("\n=== DECORATOR SYSTEM STATISTICS ===\n");
    printf("Total decorators: %d\n", ctx->decorator_count);
    printf("Function decorators: %d\n", ctx->function_decorator_count);
    printf("Class decorators: %d\n", ctx->class_decorator_count);
    printf("Property decorators: %d\n", ctx->property_decorator_count);
    printf("Stacked decorators: %d\n", ctx->stacked_decorator_count);
    
    // Count built-in vs custom
    int builtin_count = 0;
    Decorator* current = ctx->decorators;
    while (current) {
        if (decorator_is_builtin(current->name)) {
            builtin_count++;
        }
        current = current->next;
    }
    printf("Built-in decorators: %d\n", builtin_count);
    printf("Custom decorators: %d\n", ctx->decorator_count - builtin_count);
}

// ============================================================================
// DECORATOR CACHE (Memoization)
// ============================================================================

DecoratorCache* decorator_cache_new(int capacity) {
    DecoratorCache* cache = malloc(sizeof(DecoratorCache));
    cache->entries = NULL;
    cache->size = 0;
    cache->capacity = capacity;
    return cache;
}

void decorator_cache_put(DecoratorCache* cache, const char* key, void* value) {
    CacheEntry* entry = malloc(sizeof(CacheEntry));
    entry->key = strdup(key);
    entry->value = value;
    entry->next = cache->entries;
    cache->entries = entry;
    cache->size++;
}

void* decorator_cache_get(DecoratorCache* cache, const char* key) {
    CacheEntry* current = cache->entries;
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL;
}

void decorator_cache_free(DecoratorCache* cache) {
    CacheEntry* current = cache->entries;
    while (current) {
        CacheEntry* next = current->next;
        free(current->key);
        free(current);
        current = next;
    }
    free(cache);
}

// ============================================================================
// TIMING PATTERN
// ============================================================================

TimingInfo* decorator_timing_new(const char* function_name) {
    TimingInfo* timing = malloc(sizeof(TimingInfo));
    timing->function_name = strdup(function_name);
    timing->total_time = 0.0;
    timing->call_count = 0;
    timing->avg_time = 0.0;
    return timing;
}

void decorator_timing_record(TimingInfo* timing, double elapsed) {
    timing->total_time += elapsed;
    timing->call_count++;
    timing->avg_time = timing->total_time / timing->call_count;
}

void decorator_timing_print(TimingInfo* timing) {
    printf("Function: %s\n", timing->function_name);
    printf("  Total time: %.6f seconds\n", timing->total_time);
    printf("  Call count: %d\n", timing->call_count);
    printf("  Average time: %.6f seconds\n", timing->avg_time);
}

void decorator_timing_free(TimingInfo* timing) {
    free(timing->function_name);
    free(timing);
}

// ============================================================================
// DEBUG
// ============================================================================

void decorator_dump(Decorator* dec) {
    printf("Decorator: @%s\n", dec->name);
    printf("  Type: %d\n", dec->type);
    printf("  Target: %s\n", dec->target_name);
    printf("  Line: %d\n", dec->line_number);
    
    if (dec->args) {
        printf("  Arguments:\n");
        DecoratorArg* arg = dec->args;
        while (arg) {
            switch (arg->type) {
                case ARG_STRING:
                    printf("    - %s (string)\n", arg->value.string_val);
                    break;
                case ARG_NUMBER:
                    printf("    - %f (number)\n", arg->value.number_val);
                    break;
                case ARG_BOOLEAN:
                    printf("    - %s (boolean)\n", arg->value.bool_val ? "true" : "false");
                    break;
                default:
                    printf("    - (unknown)\n");
                    break;
            }
            arg = arg->next;
        }
    }
}

void decorator_context_dump(DecoratorContext* ctx) {
    printf("\n=== DECORATOR CONTEXT DUMP ===\n");
    Decorator* current = ctx->decorators;
    while (current) {
        decorator_dump(current);
        printf("\n");
        current = current->next;
    }
}
