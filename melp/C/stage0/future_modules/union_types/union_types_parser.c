#include "union_types.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Create context
UnionContext* union_context_create(void) {
    UnionContext* ctx = (UnionContext*)malloc(sizeof(UnionContext));
    ctx->type_defs = (UnionTypeDef**)malloc(sizeof(UnionTypeDef*) * 10);
    ctx->typedef_count = 0;
    ctx->typedef_capacity = 10;
    
    ctx->instances = (UnionInstance**)malloc(sizeof(UnionInstance*) * 20);
    ctx->instance_count = 0;
    ctx->instance_capacity = 20;
    
    ctx->operations = (UnionOpNode**)malloc(sizeof(UnionOpNode*) * 20);
    ctx->op_count = 0;
    ctx->op_capacity = 20;
    
    ctx->error_message = NULL;
    return ctx;
}

// Free context
void union_context_free(UnionContext* ctx) {
    if (!ctx) return;
    
    for (int i = 0; i < ctx->typedef_count; i++) {
        if (ctx->type_defs[i]) {
            free(ctx->type_defs[i]->name);
            for (int j = 0; j < ctx->type_defs[i]->variant_count; j++) {
                free(ctx->type_defs[i]->variants[j]->name);
                free(ctx->type_defs[i]->variants[j]->type);
                free(ctx->type_defs[i]->variants[j]);
            }
            free(ctx->type_defs[i]->variants);
            free(ctx->type_defs[i]);
        }
    }
    free(ctx->type_defs);
    
    for (int i = 0; i < ctx->instance_count; i++) {
        if (ctx->instances[i]) {
            free(ctx->instances[i]->var_name);
            free(ctx->instances[i]->type_name);
            free(ctx->instances[i]->active_variant);
            free(ctx->instances[i]->value);
            free(ctx->instances[i]);
        }
    }
    free(ctx->instances);
    
    for (int i = 0; i < ctx->op_count; i++) {
        if (ctx->operations[i]) {
            free(ctx->operations[i]->union_name);
            free(ctx->operations[i]->variant_name);
            for (int j = 0; j < ctx->operations[i]->arg_count; j++) {
                free(ctx->operations[i]->args[j]);
            }
            if (ctx->operations[i]->cases) {
                for (int j = 0; j < ctx->operations[i]->case_count; j++) {
                    free(ctx->operations[i]->cases[j]->pattern);
                    free(ctx->operations[i]->cases[j]->binding);
                    free(ctx->operations[i]->cases[j]->body);
                    free(ctx->operations[i]->cases[j]);
                }
                free(ctx->operations[i]->cases);
            }
            free(ctx->operations[i]);
        }
    }
    free(ctx->operations);
    
    if (ctx->error_message) free(ctx->error_message);
    free(ctx);
}

// Get union kind from string
UnionKind union_get_kind(const char* kind_str) {
    if (strstr(kind_str, "enum")) return UNION_ENUM_VARIANT;
    if (strstr(kind_str, "tagged")) return UNION_TAGGED;
    return UNION_BASIC;
}

// Get operation from string
UnionOp union_get_operation(const char* op_str) {
    if (strcmp(op_str, "define") == 0) return UNION_OP_DEFINE;
    if (strcmp(op_str, "create") == 0) return UNION_OP_CREATE;
    if (strcmp(op_str, "match") == 0) return UNION_OP_MATCH;
    if (strcmp(op_str, "get_tag") == 0) return UNION_OP_GET_TAG;
    if (strcmp(op_str, "is") == 0) return UNION_OP_IS_VARIANT;
    if (strcmp(op_str, "unwrap") == 0) return UNION_OP_UNWRAP;
    if (strcmp(op_str, "as_ref") == 0) return UNION_OP_AS_REF;
    return UNION_OP_UNKNOWN;
}

// Parse union type definition: "enum Result { Ok(i32), Err(String) }"
bool union_parse_typedef(UnionContext* ctx, const char* input) {
    char buffer[512];
    strncpy(buffer, input, 511);
    buffer[511] = '\0';
    
    // Find "enum" or "union" keyword
    char* keyword = strstr(buffer, "enum ");
    if (!keyword) keyword = strstr(buffer, "union ");
    if (!keyword) return false;
    
    UnionTypeDef* def = (UnionTypeDef*)malloc(sizeof(UnionTypeDef));
    def->variants = (UnionVariant**)malloc(sizeof(UnionVariant*) * 10);
    def->variant_count = 0;
    def->variant_capacity = 10;
    def->tag_size = 4;
    
    // Determine kind
    if (keyword == strstr(buffer, "enum ")) {
        def->kind = UNION_ENUM_VARIANT;
    } else {
        def->kind = UNION_BASIC;
    }
    
    // Extract name
    char* name_start = keyword + (def->kind == UNION_ENUM_VARIANT ? 5 : 6);
    while (isspace(*name_start)) name_start++;
    char* name_end = strchr(name_start, '{');
    if (!name_end) {
        free(def->variants);
        free(def);
        return false;
    }
    
    int name_len = name_end - name_start;
    def->name = (char*)malloc(name_len + 1);
    strncpy(def->name, name_start, name_len);
    def->name[name_len] = '\0';
    
    // Trim name
    char* p = def->name + strlen(def->name) - 1;
    while (p >= def->name && isspace(*p)) *p-- = '\0';
    
    // Parse variants
    char* variants_start = name_end + 1;
    char* variants_end = strchr(variants_start, '}');
    if (variants_end) {
        char* variant = strtok(variants_start, ",");
        int tag = 0;
        
        while (variant) {
            // Trim whitespace
            while (isspace(*variant)) variant++;
            
            UnionVariant* v = (UnionVariant*)malloc(sizeof(UnionVariant));
            v->tag_value = tag++;
            
            // Check if variant has data: "Ok(i32)" or just "None"
            char* paren = strchr(variant, '(');
            if (paren) {
                v->name = strndup(variant, paren - variant);
                char* type_start = paren + 1;
                char* type_end = strchr(type_start, ')');
                if (type_end) {
                    v->type = strndup(type_start, type_end - type_start);
                } else {
                    v->type = strdup("void");
                }
            } else {
                // No data, just variant name
                char* end = variant;
                while (*end && !isspace(*end) && *end != '}') end++;
                v->name = strndup(variant, end - variant);
                v->type = strdup("void");
            }
            
            def->variants[def->variant_count++] = v;
            variant = strtok(NULL, ",");
        }
    }
    
    union_add_typedef(ctx, def);
    return true;
}

// Parse union instance: "let result: Result = Result::Ok(42)"
bool union_parse_instance(UnionContext* ctx, const char* input) {
    char buffer[256];
    strncpy(buffer, input, 255);
    buffer[255] = '\0';
    
    UnionInstance* inst = (UnionInstance*)malloc(sizeof(UnionInstance));
    
    // Parse: "let var_name: Type = Type::Variant(value)"
    char* let = strstr(buffer, "let ");
    if (!let) {
        free(inst);
        return false;
    }
    
    // Extract variable name
    char* name_start = let + 4;
    while (isspace(*name_start)) name_start++;
    char* name_end = strchr(name_start, ':');
    if (!name_end) {
        free(inst);
        return false;
    }
    
    inst->var_name = strndup(name_start, name_end - name_start);
    
    // Extract type name
    char* type_start = name_end + 1;
    while (isspace(*type_start)) type_start++;
    char* type_end = strchr(type_start, '=');
    if (!type_end) {
        free(inst->var_name);
        free(inst);
        return false;
    }
    
    inst->type_name = strndup(type_start, type_end - type_start);
    char* p = inst->type_name + strlen(inst->type_name) - 1;
    while (p >= inst->type_name && isspace(*p)) *p-- = '\0';
    
    // Extract variant and value
    char* value_start = type_end + 1;
    while (isspace(*value_start)) value_start++;
    
    // Look for "::" to find variant
    char* double_colon = strstr(value_start, "::");
    if (double_colon) {
        char* variant_start = double_colon + 2;
        char* paren = strchr(variant_start, '(');
        if (paren) {
            inst->active_variant = strndup(variant_start, paren - variant_start);
            char* val_start = paren + 1;
            char* val_end = strchr(val_start, ')');
            if (val_end) {
                inst->value = strndup(val_start, val_end - val_start);
            } else {
                inst->value = strdup("");
            }
        } else {
            inst->active_variant = strdup(variant_start);
            inst->value = strdup("");
        }
    }
    
    union_add_instance(ctx, inst);
    return true;
}

// Parse union operation: "match result { Ok(x) => x, Err(e) => 0 }"
bool union_parse_operation(UnionContext* ctx, const char* input) {
    UnionOpNode* op = (UnionOpNode*)malloc(sizeof(UnionOpNode));
    op->arg_count = 0;
    op->cases = NULL;
    op->case_count = 0;
    
    // Check for match operation
    if (strstr(input, "match ")) {
        op->op = UNION_OP_MATCH;
        char* match_start = strstr(input, "match ");
        char* var_start = match_start + 6;
        while (isspace(*var_start)) var_start++;
        char* var_end = strchr(var_start, '{');
        if (var_end) {
            op->union_name = strndup(var_start, var_end - var_start);
            // Trim
            char* p = op->union_name + strlen(op->union_name) - 1;
            while (p >= op->union_name && isspace(*p)) *p-- = '\0';
        }
        op->variant_name = NULL;
    } else {
        op->op = UNION_OP_UNKNOWN;
        op->union_name = strdup(input);
        op->variant_name = NULL;
    }
    
    union_add_operation(ctx, op);
    return true;
}

// Add type definition
void union_add_typedef(UnionContext* ctx, UnionTypeDef* def) {
    if (ctx->typedef_count >= ctx->typedef_capacity) {
        ctx->typedef_capacity *= 2;
        ctx->type_defs = (UnionTypeDef**)realloc(ctx->type_defs,
                                                  sizeof(UnionTypeDef*) * ctx->typedef_capacity);
    }
    ctx->type_defs[ctx->typedef_count++] = def;
}

// Add instance
void union_add_instance(UnionContext* ctx, UnionInstance* inst) {
    if (ctx->instance_count >= ctx->instance_capacity) {
        ctx->instance_capacity *= 2;
        ctx->instances = (UnionInstance**)realloc(ctx->instances,
                                                   sizeof(UnionInstance*) * ctx->instance_capacity);
    }
    ctx->instances[ctx->instance_count++] = inst;
}

// Add operation
void union_add_operation(UnionContext* ctx, UnionOpNode* op) {
    if (ctx->op_count >= ctx->op_capacity) {
        ctx->op_capacity *= 2;
        ctx->operations = (UnionOpNode**)realloc(ctx->operations,
                                                  sizeof(UnionOpNode*) * ctx->op_capacity);
    }
    ctx->operations[ctx->op_count++] = op;
}

// Find type definition
UnionTypeDef* union_find_typedef(UnionContext* ctx, const char* name) {
    for (int i = 0; i < ctx->typedef_count; i++) {
        if (strcmp(ctx->type_defs[i]->name, name) == 0) {
            return ctx->type_defs[i];
        }
    }
    return NULL;
}

// Find instance
UnionInstance* union_find_instance(UnionContext* ctx, const char* name) {
    for (int i = 0; i < ctx->instance_count; i++) {
        if (strcmp(ctx->instances[i]->var_name, name) == 0) {
            return ctx->instances[i];
        }
    }
    return NULL;
}
