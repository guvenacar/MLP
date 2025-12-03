#include "module_system.h"
#include <stdlib.h>
#include <string.h>

// Module oluştur
Module* module_create(const char* name, const char* path) {
    Module* mod = malloc(sizeof(Module));
    mod->name = strdup(name);
    mod->path = path ? strdup(path) : NULL;
    mod->exports = NULL;
    mod->export_count = 0;
    mod->imports = NULL;
    mod->import_count = 0;
    mod->is_loaded = 0;
    return mod;
}

// Import oluştur
Import* import_create(const char* module_name) {
    Import* imp = malloc(sizeof(Import));
    imp->module_name = strdup(module_name);
    imp->symbols = NULL;
    imp->symbol_count = 0;
    imp->alias = NULL;
    imp->is_wildcard = 0;
    return imp;
}

// Export oluştur
Export* export_create(const char* symbol_name, void* value) {
    Export* exp = malloc(sizeof(Export));
    exp->symbol_name = strdup(symbol_name);
    exp->value = value;
    exp->is_default = 0;
    return exp;
}

// Module path oluştur
ModulePath* module_path_create(const char* path) {
    ModulePath* mp = malloc(sizeof(ModulePath));
    mp->path = strdup(path);
    mp->is_relative = (path[0] == '.' || path[0] == '/');
    mp->is_builtin = is_builtin_module(path);
    return mp;
}

// Module cache oluştur
ModuleCache* module_cache_create(void) {
    ModuleCache* cache = malloc(sizeof(ModuleCache));
    cache->capacity = 16;
    cache->modules = malloc(sizeof(Module*) * cache->capacity);
    cache->module_count = 0;
    return cache;
}

// Symbol oluştur
Symbol* symbol_create(const char* name, Visibility vis) {
    Symbol* sym = malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->visibility = vis;
    sym->value = NULL;
    sym->type = NULL;
    return sym;
}

// Module'e import ekle
void module_add_import(Module* mod, Module* imported) {
    if (!mod) return;
    
    mod->imports = realloc(mod->imports,
                          sizeof(Module*) * (mod->import_count + 1));
    mod->imports[mod->import_count] = imported;
    mod->import_count++;
}

// Module'e export ekle
void module_add_export(Module* mod, Export* exp) {
    if (!mod) return;
    
    mod->exports = realloc(mod->exports,
                          sizeof(Export*) * (mod->export_count + 1));
    ((Export**)mod->exports)[mod->export_count] = exp;
    mod->export_count++;
}

// Module yükle
Module* module_load(const char* path, ModuleCache* cache) {
    if (!path || !cache) return NULL;
    
    // Cache'te var mı?
    Module* cached = cache_get_module(cache, path);
    if (cached) return cached;
    
    // Yeni modül oluştur
    Module* mod = module_create(path, path);
    mod->is_loaded = 1;
    
    // Cache'e ekle
    cache_add_module(cache, mod);
    
    return mod;
}

// Module yüklenmiş mi?
int module_is_loaded(Module* mod) {
    return mod ? mod->is_loaded : 0;
}

// Import'a sembol ekle
void import_add_symbol(Import* imp, const char* symbol) {
    if (!imp) return;
    
    imp->symbols = realloc(imp->symbols,
                          sizeof(char*) * (imp->symbol_count + 1));
    imp->symbols[imp->symbol_count] = strdup(symbol);
    imp->symbol_count++;
}

// Import alias ayarla
void import_set_alias(Import* imp, const char* alias) {
    if (!imp) return;
    imp->alias = strdup(alias);
}

// Module path resolve et
char* resolve_module_path(const char* from, const char* to) {
    if (!to) return NULL;
    
    // Basit implementasyon
    if (to[0] == '.' || to[0] == '/') {
        // Relative veya absolute path
        return strdup(to);
    }
    
    // Package name
    static char buffer[256];
    snprintf(buffer, sizeof(buffer), "modules/%s", to);
    return strdup(buffer);
}

// Built-in module kontrolü
int is_builtin_module(const char* name) {
    if (!name) return 0;
    
    // Built-in modül isimleri
    const char* builtins[] = {"std", "io", "math", "string", NULL};
    
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return 1;
        }
    }
    
    return 0;
}

// Cache'e modül ekle
void cache_add_module(ModuleCache* cache, Module* mod) {
    if (!cache || !mod) return;
    
    if (cache->module_count >= cache->capacity) {
        cache->capacity *= 2;
        cache->modules = realloc(cache->modules,
                                sizeof(Module*) * cache->capacity);
    }
    
    cache->modules[cache->module_count] = mod;
    cache->module_count++;
}

// Cache'ten modül al
Module* cache_get_module(ModuleCache* cache, const char* name) {
    if (!cache || !name) return NULL;
    
    for (int i = 0; i < cache->module_count; i++) {
        if (strcmp(cache->modules[i]->name, name) == 0) {
            return cache->modules[i];
        }
    }
    
    return NULL;
}

// Cache'te var mı?
int cache_has_module(ModuleCache* cache, const char* name) {
    return cache_get_module(cache, name) != NULL;
}

// Module'den sembol al
Symbol* module_get_symbol(Module* mod, const char* name) {
    if (!mod || !name) return NULL;
    
    // Basit lookup (gerçekte export listesinde aranacak)
    return NULL;
}

// Sembol erişilebilir mi?
int symbol_is_accessible(Symbol* sym, Visibility context) {
    if (!sym) return 0;
    
    if (sym->visibility == VIS_PUBLIC) return 1;
    if (sym->visibility == VIS_PRIVATE) return 0;
    
    // Internal: Aynı paket içinde erişilebilir
    return 1;
}

// Bellek temizleme
void module_free(Module* mod) {
    if (!mod) return;
    
    free(mod->name);
    if (mod->path) free(mod->path);
    
    if (mod->exports) {
        for (int i = 0; i < mod->export_count; i++) {
            export_free(((Export**)mod->exports)[i]);
        }
        free(mod->exports);
    }
    
    if (mod->imports) free(mod->imports);
    free(mod);
}

void import_free(Import* imp) {
    if (!imp) return;
    
    free(imp->module_name);
    if (imp->alias) free(imp->alias);
    
    if (imp->symbols) {
        for (int i = 0; i < imp->symbol_count; i++) {
            free(imp->symbols[i]);
        }
        free(imp->symbols);
    }
    
    free(imp);
}

void export_free(Export* exp) {
    if (!exp) return;
    free(exp->symbol_name);
    free(exp);
}

void module_path_free(ModulePath* path) {
    if (!path) return;
    free(path->path);
    free(path);
}

void module_cache_free(ModuleCache* cache) {
    if (!cache) return;
    
    for (int i = 0; i < cache->module_count; i++) {
        module_free(cache->modules[i]);
    }
    
    free(cache->modules);
    free(cache);
}

void symbol_free(Symbol* sym) {
    if (!sym) return;
    free(sym->name);
    if (sym->type) free(sym->type);
    free(sym);
}
