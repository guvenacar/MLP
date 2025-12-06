#define _POSIX_C_SOURCE 200809L
#include "type_classes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// TYPE CLASS SYSTEM IMPLEMENTATION
// ============================================================================

TypeClassContext* tc_context_create(void) {
    TypeClassContext* ctx = malloc(sizeof(TypeClassContext));
    if (!ctx) return NULL;
    
    ctx->class_capacity = 16;
    ctx->classes = malloc(sizeof(TypeClass*) * ctx->class_capacity);
    if (!ctx->classes) {
        free(ctx);
        return NULL;
    }
    
    ctx->instance_capacity = 32;
    ctx->instances = malloc(sizeof(TypeInstance*) * ctx->instance_capacity);
    if (!ctx->instances) {
        free(ctx->classes);
        free(ctx);
        return NULL;
    }
    
    ctx->class_count = 0;
    ctx->instance_count = 0;
    ctx->total_checks = 0;
    ctx->violations = 0;
    
    return ctx;
}

void tc_context_destroy(TypeClassContext* ctx) {
    if (!ctx) return;
    
    for (size_t i = 0; i < ctx->class_count; i++) {
        tc_class_free(ctx->classes[i]);
    }
    free(ctx->classes);
    
    for (size_t i = 0; i < ctx->instance_count; i++) {
        tc_instance_free(ctx->instances[i]);
    }
    free(ctx->instances);
    
    free(ctx);
}

TypeClass* tc_class_create(const char* name, TypeKind kind) {
    TypeClass* tc = malloc(sizeof(TypeClass));
    if (!tc) return NULL;
    
    tc->name = strdup(name);
    if (!tc->name) {
        free(tc);
        return NULL;
    }
    
    tc->kind = kind;
    tc->methods = NULL;
    tc->method_count = 0;
    tc->laws = NULL;
    tc->law_count = 0;
    tc->super_classes = NULL;
    tc->super_count = 0;
    
    return tc;
}

void tc_class_add_method(TypeClass* tc, const char* method) {
    if (!tc || !method) return;
    
    tc->methods = realloc(tc->methods, sizeof(char*) * (tc->method_count + 1));
    tc->methods[tc->method_count++] = strdup(method);
}

void tc_class_add_law(TypeClass* tc, const char* law) {
    if (!tc || !law) return;
    
    tc->laws = realloc(tc->laws, sizeof(char*) * (tc->law_count + 1));
    tc->laws[tc->law_count++] = strdup(law);
}

void tc_class_add_super(TypeClass* tc, const char* super) {
    if (!tc || !super) return;
    
    tc->super_classes = realloc(tc->super_classes, sizeof(char*) * (tc->super_count + 1));
    tc->super_classes[tc->super_count++] = strdup(super);
}

void tc_class_free(TypeClass* tc) {
    if (!tc) return;
    
    free(tc->name);
    
    for (size_t i = 0; i < tc->method_count; i++) {
        free(tc->methods[i]);
    }
    free(tc->methods);
    
    for (size_t i = 0; i < tc->law_count; i++) {
        free(tc->laws[i]);
    }
    free(tc->laws);
    
    for (size_t i = 0; i < tc->super_count; i++) {
        free(tc->super_classes[i]);
    }
    free(tc->super_classes);
    
    free(tc);
}

TypeInstance* tc_instance_create(const char* class_name, const char* type_name) {
    TypeInstance* inst = malloc(sizeof(TypeInstance));
    if (!inst) return NULL;
    
    inst->class_name = strdup(class_name);
    inst->type_name = strdup(type_name);
    
    if (!inst->class_name || !inst->type_name) {
        free(inst->class_name);
        free(inst->type_name);
        free(inst);
        return NULL;
    }
    
    inst->method_impls = NULL;
    inst->impl_count = 0;
    inst->valid = true;
    
    return inst;
}

void tc_instance_add_impl(TypeInstance* inst, const char* impl) {
    if (!inst || !impl) return;
    
    inst->method_impls = realloc(inst->method_impls, sizeof(char*) * (inst->impl_count + 1));
    inst->method_impls[inst->impl_count++] = strdup(impl);
}

void tc_instance_free(TypeInstance* inst) {
    if (!inst) return;
    
    free(inst->class_name);
    free(inst->type_name);
    
    for (size_t i = 0; i < inst->impl_count; i++) {
        free(inst->method_impls[i]);
    }
    free(inst->method_impls);
    
    free(inst);
}

void tc_register_class(TypeClassContext* ctx, TypeClass* tc) {
    if (!ctx || !tc) return;
    
    if (ctx->class_count >= ctx->class_capacity) {
        ctx->class_capacity *= 2;
        ctx->classes = realloc(ctx->classes, sizeof(TypeClass*) * ctx->class_capacity);
    }
    
    ctx->classes[ctx->class_count++] = tc;
}

void tc_register_instance(TypeClassContext* ctx, TypeInstance* inst) {
    if (!ctx || !inst) return;
    
    if (ctx->instance_count >= ctx->instance_capacity) {
        ctx->instance_capacity *= 2;
        ctx->instances = realloc(ctx->instances, sizeof(TypeInstance*) * ctx->instance_capacity);
    }
    
    ctx->instances[ctx->instance_count++] = inst;
}

TypeClass* tc_find_class(TypeClassContext* ctx, const char* name) {
    if (!ctx || !name) return NULL;
    
    for (size_t i = 0; i < ctx->class_count; i++) {
        if (strcmp(ctx->classes[i]->name, name) == 0) {
            return ctx->classes[i];
        }
    }
    
    return NULL;
}

TypeInstance* tc_find_instance(TypeClassContext* ctx, const char* class_name, const char* type_name) {
    if (!ctx || !class_name || !type_name) return NULL;
    
    for (size_t i = 0; i < ctx->instance_count; i++) {
        TypeInstance* inst = ctx->instances[i];
        if (strcmp(inst->class_name, class_name) == 0 &&
            strcmp(inst->type_name, type_name) == 0) {
            return inst;
        }
    }
    
    return NULL;
}

bool tc_has_instance(TypeClassContext* ctx, const char* class_name, const char* type_name) {
    return tc_find_instance(ctx, class_name, type_name) != NULL;
}

bool tc_check_laws(TypeClassContext* ctx, TypeInstance* inst) {
    if (!ctx || !inst) return false;
    
    ctx->total_checks++;
    
    TypeClass* tc = tc_find_class(ctx, inst->class_name);
    if (!tc) {
        ctx->violations++;
        return false;
    }
    
    // Check method count matches
    if (inst->impl_count < tc->method_count) {
        fprintf(stderr, "WARNING: Instance '%s' for '%s' missing methods\n",
                inst->type_name, inst->class_name);
        inst->valid = false;
        ctx->violations++;
        return false;
    }
    
    return true;
}

// ============================================================================
// PARSER
// ============================================================================

TypeClass* tc_parse_class(const char* line) {
    if (!line) return NULL;
    
    // Expected format:
    // "class Functor f where"
    // "class Monad m : Applicative m where"
    
    char keyword[32];
    char name[128];
    char rest[256] = {0};
    
    int result = sscanf(line, "%31s %127s %255[^\n]", keyword, name, rest);
    if (result < 2) return NULL;
    
    if (strcmp(keyword, "class") != 0) return NULL;
    
    // Default to higher-kinded type
    TypeClass* tc = tc_class_create(name, KIND_TYPE_TO_TYPE);
    
    // Parse superclasses if present
    if (strstr(rest, ":") != NULL) {
        char* colon = strchr(rest, ':');
        if (colon) {
            char super[128];
            sscanf(colon + 1, "%127s", super);
            
            // Remove "where"
            char* where = strstr(super, "where");
            if (where) *where = '\0';
            
            if (strlen(super) > 0) {
                tc_class_add_super(tc, super);
            }
        }
    }
    
    return tc;
}

TypeInstance* tc_parse_instance(const char* line) {
    if (!line) return NULL;
    
    // Expected format:
    // "instance Functor Maybe"
    // "instance Monad IO"
    
    char keyword[32];
    char class_name[128];
    char type_name[128];
    
    int result = sscanf(line, "%31s %127s %127s", keyword, class_name, type_name);
    if (result != 3) return NULL;
    
    if (strcmp(keyword, "instance") != 0) return NULL;
    
    return tc_instance_create(class_name, type_name);
}

// ============================================================================
// CODE GENERATION
// ============================================================================

char* codegen_typeclass_check(TypeClass* tc) {
    if (!tc) return NULL;
    
    char* code = malloc(512);
    if (!code) return NULL;
    
    int offset = snprintf(code, 512,
        "    ; Type class '%s'\n"
        "    ; kind=%d, methods=%zu, laws=%zu\n",
        tc->name,
        tc->kind,
        tc->method_count,
        tc->law_count
    );
    
    for (size_t i = 0; i < tc->super_count && offset < 500; i++) {
        offset += snprintf(code + offset, 512 - offset,
            "    ; superclass: %s\n", tc->super_classes[i]);
    }
    
    return code;
}
