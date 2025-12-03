#include "null_safety.h"
#include <stdlib.h>
#include <string.h>

// Optional oluştur (değer ile)
Optional* optional_create(void* value, const char* type_name, size_t value_size) {
    Optional* opt = malloc(sizeof(Optional));
    opt->state = OPT_SOME;
    opt->value = value;
    opt->type_name = strdup(type_name);
    opt->value_size = value_size;
    return opt;
}

// Optional oluştur (None)
Optional* optional_none(const char* type_name) {
    Optional* opt = malloc(sizeof(Optional));
    opt->state = OPT_NONE;
    opt->value = NULL;
    opt->type_name = strdup(type_name);
    opt->value_size = 0;
    return opt;
}

// Null kontrol oluştur
NullCheck* null_check_create(void* expr, NullCheckType type) {
    NullCheck* check = malloc(sizeof(NullCheck));
    check->expr = expr;
    check->type = type;
    check->result = 0;
    return check;
}

// Null-coalescing oluştur
NullCoalesce* null_coalesce_create(void* left, void* right) {
    NullCoalesce* coal = malloc(sizeof(NullCoalesce));
    coal->left = left;
    coal->right = right;
    coal->result = NULL;
    return coal;
}

// Safe navigation oluştur
SafeNav* safe_nav_create(void* object, const char* member) {
    SafeNav* nav = malloc(sizeof(SafeNav));
    nav->object = object;
    nav->member = strdup(member);
    nav->chain = 0;
    return nav;
}

// Null assertion oluştur
NullAssert* null_assert_create(void* expr, const char* message) {
    NullAssert* assert = malloc(sizeof(NullAssert));
    assert->expr = expr;
    assert->message = message ? strdup(message) : strdup("Null assertion failed");
    return assert;
}

// Optional'ın değeri var mı?
int optional_has_value(Optional* opt) {
    if (!opt) return 0;
    return opt->state == OPT_SOME;
}

// Optional'dan değer al
void* optional_get(Optional* opt) {
    if (!opt || opt->state != OPT_SOME) return NULL;
    return opt->value;
}

// Optional'dan değer al veya default
void* optional_get_or(Optional* opt, void* default_value) {
    if (!opt || opt->state != OPT_SOME) return default_value;
    return opt->value;
}

// Optional map (transform)
Optional* optional_map(Optional* opt, void* (*transform)(void*)) {
    if (!opt || opt->state != OPT_SOME || !transform) {
        return optional_none(opt ? opt->type_name : "unknown");
    }
    
    void* new_value = transform(opt->value);
    return optional_create(new_value, opt->type_name, opt->value_size);
}

// Optional filter
Optional* optional_filter(Optional* opt, int (*predicate)(void*)) {
    if (!opt || opt->state != OPT_SOME || !predicate) {
        return optional_none(opt ? opt->type_name : "unknown");
    }
    
    if (predicate(opt->value)) {
        return optional_create(opt->value, opt->type_name, opt->value_size);
    }
    
    return optional_none(opt->type_name);
}

// Bellek temizleme
void optional_free(Optional* opt) {
    if (!opt) return;
    free(opt->type_name);
    free(opt);
}

void null_check_free(NullCheck* check) {
    if (!check) return;
    free(check);
}

void null_coalesce_free(NullCoalesce* coal) {
    if (!coal) return;
    free(coal);
}

void safe_nav_free(SafeNav* nav) {
    if (!nav) return;
    free(nav->member);
    free(nav);
}

void null_assert_free(NullAssert* assert) {
    if (!assert) return;
    free(assert->message);
    free(assert);
}
