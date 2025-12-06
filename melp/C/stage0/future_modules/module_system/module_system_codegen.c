#include "module_system_codegen.h"
#include <string.h>

// Module için kod üret
void codegen_module(FILE* out, Module* mod) {
    fprintf(out, "    ; Module: %s\n", mod->name);
    
    // Module header
    codegen_module_header(out, mod);
    
    // Export table
    codegen_export_table(out, mod);
    
    // Module initialization
    codegen_module_init(out, mod);
}

// Import için kod üret
void codegen_import(FILE* out, Import* imp) {
    fprintf(out, "    ; Import from: %s\n", imp->module_name);
    
    if (imp->is_wildcard) {
        fprintf(out, "    ; Import * as %s\n", 
                imp->alias ? imp->alias : imp->module_name);
    } else {
        for (int i = 0; i < imp->symbol_count; i++) {
            fprintf(out, "    ; Import: %s\n", imp->symbols[i]);
        }
    }
    
    // Import resolution
    codegen_import_resolution(out, imp);
}

// Export için kod üret
void codegen_export(FILE* out, Export* exp) {
    fprintf(out, "    ; Export: %s\n", exp->symbol_name);
    
    if (exp->is_default) {
        fprintf(out, "    ; Default export\n");
    }
    
    // Export sembolü global yap
    fprintf(out, "global .export_%s\n", exp->symbol_name);
    fprintf(out, ".export_%s:\n", exp->symbol_name);
    fprintf(out, "    ; Symbol: %s\n", exp->symbol_name);
}

// Module initialization
void codegen_module_init(FILE* out, Module* mod) {
    fprintf(out, "\n.module_%s_init:\n", mod->name);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    // Import'ları yükle
    fprintf(out, "    ; Load imports\n");
    for (int i = 0; i < mod->import_count; i++) {
        fprintf(out, "    call .module_%s_init\n", 
                mod->imports[i]->name);
    }
    
    // Module'ü initialized olarak işaretle
    fprintf(out, "    mov byte [rel .module_%s_loaded], 1\n", mod->name);
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Module header
void codegen_module_header(FILE* out, Module* mod) {
    fprintf(out, "\n; ==========================================\n");
    fprintf(out, "; Module: %s\n", mod->name);
    fprintf(out, "; Path: %s\n", mod->path ? mod->path : "N/A");
    fprintf(out, "; ==========================================\n\n");
    
    fprintf(out, "section .data\n");
    fprintf(out, ".module_%s_name: db \"%s\", 0\n", mod->name, mod->name);
    fprintf(out, ".module_%s_loaded: db 0\n", mod->name);
    fprintf(out, "section .text\n");
}

// Export table
void codegen_export_table(FILE* out, Module* mod) {
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".module_%s_exports:\n", mod->name);
    
    for (int i = 0; i < mod->export_count; i++) {
        Export* exp = ((Export**)mod->exports)[i];
        fprintf(out, "    dq .export_%s    ; %s\n", 
                exp->symbol_name, exp->symbol_name);
    }
    
    fprintf(out, ".module_%s_export_count: dq %d\n", 
            mod->name, mod->export_count);
    
    fprintf(out, "section .text\n");
}

// Import resolution
void codegen_import_resolution(FILE* out, Import* imp) {
    fprintf(out, "    ; Resolve imports from: %s\n", imp->module_name);
    
    // Module'ü yükle (lazy loading)
    fprintf(out, "    lea rdi, [rel .module_%s_name]\n", imp->module_name);
    fprintf(out, "    call .module_load\n");
    
    // Sembolleri resolve et
    for (int i = 0; i < imp->symbol_count; i++) {
        fprintf(out, "    ; Resolve: %s\n", imp->symbols[i]);
        fprintf(out, "    lea rdi, [rel .str_%s]\n", imp->symbols[i]);
        fprintf(out, "    call .module_get_symbol\n");
        
        // Sembolü local namespace'e ekle
        fprintf(out, "    mov [rel .import_%s], rax\n", imp->symbols[i]);
    }
}

// Public symbol
void codegen_public_symbol(FILE* out, Symbol* sym) {
    fprintf(out, "global %s\n", sym->name);
    fprintf(out, "%s:\n", sym->name);
    fprintf(out, "    ; Public symbol: %s\n", sym->name);
}

// Private symbol
void codegen_private_symbol(FILE* out, Symbol* sym) {
    fprintf(out, ".%s:    ; Private\n", sym->name);
    fprintf(out, "    ; Private symbol: %s\n", sym->name);
}

// Module loader
void codegen_module_loader(FILE* out, ModuleCache* cache) {
    fprintf(out, "\n.module_load:\n");
    fprintf(out, "    ; rdi = module name\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    // Cache'te var mı kontrol et
    fprintf(out, "    ; Check cache\n");
    fprintf(out, "    call .cache_get_module\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jnz .module_load_done    ; Zaten yüklü\n");
    
    // Module'ü yükle
    fprintf(out, "    ; Load module\n");
    fprintf(out, "    call .load_module_file\n");
    
    // Cache'e ekle
    fprintf(out, "    call .cache_add_module\n");
    
    fprintf(out, ".module_load_done:\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Lazy loading
void codegen_lazy_load(FILE* out, Module* mod) {
    fprintf(out, ".lazy_load_%s:\n", mod->name);
    
    // Yüklenmiş mi kontrol et
    fprintf(out, "    mov al, [rel .module_%s_loaded]\n", mod->name);
    fprintf(out, "    test al, al\n");
    fprintf(out, "    jnz .lazy_done_%s\n", mod->name);
    
    // Yükle
    fprintf(out, "    call .module_%s_init\n", mod->name);
    
    fprintf(out, ".lazy_done_%s:\n", mod->name);
    fprintf(out, "    ret\n");
}
