#ifndef MODULE_SYSTEM_CODEGEN_H
#define MODULE_SYSTEM_CODEGEN_H

#include "module_system.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_module(FILE* out, Module* mod);
void codegen_import(FILE* out, Import* imp);
void codegen_export(FILE* out, Export* exp);
void codegen_module_init(FILE* out, Module* mod);

// Module linking
void codegen_module_header(FILE* out, Module* mod);
void codegen_export_table(FILE* out, Module* mod);
void codegen_import_resolution(FILE* out, Import* imp);

// Symbol visibility
void codegen_public_symbol(FILE* out, Symbol* sym);
void codegen_private_symbol(FILE* out, Symbol* sym);

// Module loading
void codegen_module_loader(FILE* out, ModuleCache* cache);
void codegen_lazy_load(FILE* out, Module* mod);

#endif // MODULE_SYSTEM_CODEGEN_H
