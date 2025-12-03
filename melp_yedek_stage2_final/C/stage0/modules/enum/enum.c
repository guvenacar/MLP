#include "enum.h"
#include <stdlib.h>
#include <string.h>

// Enum variant oluştur
EnumVariant* enum_variant_create(const char* name, int tag) {
    EnumVariant* variant = malloc(sizeof(EnumVariant));
    variant->name = strdup(name);
    variant->tag = tag;
    variant->associated_data = NULL;
    variant->data_size = 0;
    variant->has_data = 0;
    return variant;
}

// Enum definition oluştur
EnumDef* enum_def_create(const char* name) {
    EnumDef* def = malloc(sizeof(EnumDef));
    def->name = strdup(name);
    def->variant_count = 0;
    def->variants = malloc(sizeof(EnumVariant*) * 16);
    def->is_tagged_union = 0;
    def->max_data_size = 0;
    return def;
}

// Enum value oluştur
EnumValue* enum_value_create(EnumDef* enum_def, int tag) {
    EnumValue* value = malloc(sizeof(EnumValue));
    value->enum_def = enum_def;
    value->current_tag = tag;
    value->current_data = NULL;
    return value;
}

// Enum match oluştur
EnumMatch* enum_match_create(EnumValue* value) {
    EnumMatch* match = malloc(sizeof(EnumMatch));
    match->value = value;
    match->arm_count = 0;
    match->arm_bodies = malloc(sizeof(void*) * 16);
    match->default_body = NULL;
    return match;
}

// Enum cast oluştur
EnumCast* enum_cast_create(EnumValue* value, const char* target) {
    EnumCast* cast = malloc(sizeof(EnumCast));
    cast->value = value;
    cast->target_variant = strdup(target);
    cast->is_safe = 1;
    return cast;
}

// Variant'a data ekle
void enum_variant_set_data(EnumVariant* variant, void* data, size_t size) {
    if (!variant) return;
    
    variant->associated_data = malloc(size);
    memcpy(variant->associated_data, data, size);
    variant->data_size = size;
    variant->has_data = 1;
}

// Variant data'sı var mı?
int enum_variant_has_data(EnumVariant* variant) {
    return variant ? variant->has_data : 0;
}

// Enum definition'a variant ekle
void enum_def_add_variant(EnumDef* def, EnumVariant* variant) {
    if (!def || !variant) return;
    
    def->variants[def->variant_count] = variant;
    def->variant_count++;
    
    // Max data size güncelle
    if (variant->data_size > def->max_data_size) {
        def->max_data_size = variant->data_size;
    }
    
    // Tagged union check
    if (variant->has_data) {
        def->is_tagged_union = 1;
    }
}

// Variant bul (by name)
EnumVariant* enum_def_find_variant(EnumDef* def, const char* name) {
    if (!def || !name) return NULL;
    
    for (int i = 0; i < def->variant_count; i++) {
        if (strcmp(def->variants[i]->name, name) == 0) {
            return def->variants[i];
        }
    }
    
    return NULL;
}

// Variant bul (by tag)
EnumVariant* enum_def_get_variant_by_tag(EnumDef* def, int tag) {
    if (!def) return NULL;
    
    for (int i = 0; i < def->variant_count; i++) {
        if (def->variants[i]->tag == tag) {
            return def->variants[i];
        }
    }
    
    return NULL;
}

// Enum value variant ayarla
void enum_value_set_variant(EnumValue* value, int tag, void* data) {
    if (!value) return;
    
    value->current_tag = tag;
    
    // Data varsa kopyala
    EnumVariant* variant = enum_def_get_variant_by_tag(value->enum_def, tag);
    if (variant && variant->has_data && data) {
        value->current_data = malloc(variant->data_size);
        memcpy(value->current_data, data, variant->data_size);
    }
}

// Enum value tag al
int enum_value_get_tag(EnumValue* value) {
    return value ? value->current_tag : -1;
}

// Enum value data al
void* enum_value_get_data(EnumValue* value) {
    return value ? value->current_data : NULL;
}

// Enum value variant kontrolü
int enum_value_is_variant(EnumValue* value, const char* variant_name) {
    if (!value || !variant_name) return 0;
    
    EnumVariant* variant = enum_def_find_variant(value->enum_def, variant_name);
    if (!variant) return 0;
    
    return value->current_tag == variant->tag;
}

// Enum match arm ekle
void enum_match_add_arm(EnumMatch* match, int tag, void* body) {
    if (!match) return;
    
    match->arm_bodies[match->arm_count] = body;
    match->arm_count++;
}

// Enum match execute
void* enum_match_execute(EnumMatch* match) {
    if (!match || !match->value) return NULL;
    
    int tag = match->value->current_tag;
    
    // Tag'e göre arm bul
    for (int i = 0; i < match->arm_count; i++) {
        EnumVariant* variant = enum_def_get_variant_by_tag(
            match->value->enum_def, tag);
        
        if (variant && variant->tag == tag) {
            return match->arm_bodies[i];
        }
    }
    
    // Default
    return match->default_body;
}

// Enum cast execute
void* enum_cast_execute(EnumCast* cast) {
    if (!cast || !enum_cast_is_valid(cast)) return NULL;
    
    // Cast işlemi (şimdilik data'yı dön)
    return cast->value->current_data;
}

// Enum cast geçerli mi?
int enum_cast_is_valid(EnumCast* cast) {
    if (!cast || !cast->value) return 0;
    
    // Target variant mevcut mu?
    EnumVariant* target = enum_def_find_variant(
        cast->value->enum_def, cast->target_variant);
    
    if (!target) return 0;
    
    // Safe cast: Current variant == target variant
    if (cast->is_safe) {
        return cast->value->current_tag == target->tag;
    }
    
    return 1;
}

// Enum variant adı al
const char* enum_variant_name(EnumValue* value) {
    if (!value) return NULL;
    
    EnumVariant* variant = enum_def_get_variant_by_tag(
        value->enum_def, value->current_tag);
    
    return variant ? variant->name : NULL;
}

// Enum variant sayısı
int enum_variant_count(EnumDef* def) {
    return def ? def->variant_count : 0;
}

// Bellek temizleme
void enum_variant_free(EnumVariant* variant) {
    if (!variant) return;
    free(variant->name);
    if (variant->associated_data) free(variant->associated_data);
    free(variant);
}

void enum_def_free(EnumDef* def) {
    if (!def) return;
    free(def->name);
    
    for (int i = 0; i < def->variant_count; i++) {
        enum_variant_free(def->variants[i]);
    }
    
    free(def->variants);
    free(def);
}

void enum_value_free(EnumValue* value) {
    if (!value) return;
    if (value->current_data) free(value->current_data);
    free(value);
}

void enum_match_free(EnumMatch* match) {
    if (!match) return;
    free(match->arm_bodies);
    free(match);
}

void enum_cast_free(EnumCast* cast) {
    if (!cast) return;
    free(cast->target_variant);
    free(cast);
}
