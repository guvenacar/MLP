#ifndef MODULE_SYSTEM_H
#define MODULE_SYSTEM_H

#include <stddef.h>
#include "../../lexer.h"

// Module tanımı
typedef struct Module {
    char* name;             // Modül adı
    char* path;             // Dosya yolu
    void** exports;         // Export edilen semboller
    int export_count;       // Export sayısı
    struct Module** imports;// Import edilen modüller
    int import_count;       // Import sayısı
    int is_loaded;          // Yüklenmiş mi?
} Module;

// Import ifadesi
typedef struct Import {
    char* module_name;      // Modül adı
    char** symbols;         // Import edilen semboller
    int symbol_count;       // Sembol sayısı
    char* alias;            // Alias (as ...)
    int is_wildcard;        // import * from ...
} Import;

// Export ifadesi
typedef struct Export {
    char* symbol_name;      // Export edilen sembol
    void* value;            // Sembol değeri
    int is_default;         // Default export mu?
} Export;

// Module path resolver
typedef struct ModulePath {
    char* path;             // Modül yolu
    int is_relative;        // Relative mi? (./mod vs mod)
    int is_builtin;         // Built-in modül mü?
} ModulePath;

// Module cache
typedef struct ModuleCache {
    Module** modules;       // Yüklenmiş modüller
    int module_count;       // Modül sayısı
    int capacity;           // Kapasite
} ModuleCache;

// Visibility
typedef enum Visibility {
    VIS_PUBLIC,             // public (dışarıdan erişilebilir)
    VIS_PRIVATE,            // private (sadece modül içi)
    VIS_INTERNAL            // internal (paket içi)
} Visibility;

// Symbol (export edilen)
typedef struct Symbol {
    char* name;             // Sembol adı
    Visibility visibility;  // Görünürlük
    void* value;            // Değer
    char* type;             // Tip
} Symbol;

// API fonksiyonları
Module* module_create(const char* name, const char* path);
Import* import_create(const char* module_name);
Export* export_create(const char* symbol_name, void* value);
ModulePath* module_path_create(const char* path);
ModuleCache* module_cache_create(void);
Symbol* symbol_create(const char* name, Visibility vis);

void module_free(Module* mod);
void import_free(Import* imp);
void export_free(Export* exp);
void module_path_free(ModulePath* path);
void module_cache_free(ModuleCache* cache);
void symbol_free(Symbol* sym);

// Module operasyonları
void module_add_import(Module* mod, Module* imported);
void module_add_export(Module* mod, Export* exp);
Module* module_load(const char* path, ModuleCache* cache);
int module_is_loaded(Module* mod);

// Import operasyonları
void import_add_symbol(Import* imp, const char* symbol);
void import_set_alias(Import* imp, const char* alias);

// Module path resolver
char* resolve_module_path(const char* from, const char* to);
int is_builtin_module(const char* name);

// Module cache operasyonları
void cache_add_module(ModuleCache* cache, Module* mod);
Module* cache_get_module(ModuleCache* cache, const char* name);
int cache_has_module(ModuleCache* cache, const char* name);

// Symbol lookup
Symbol* module_get_symbol(Module* mod, const char* name);
int symbol_is_accessible(Symbol* sym, Visibility context);

#endif // MODULE_SYSTEM_H
