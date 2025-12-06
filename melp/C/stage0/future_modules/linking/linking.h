#ifndef LINKING_H
#define LINKING_H

#include <stdbool.h>

// Symbol types
typedef enum {
    SYM_FUNCTION,       // Function symbol
    SYM_VARIABLE,       // Global variable
    SYM_CONSTANT,       // Constant
    SYM_EXTERNAL,       // External symbol (from library)
    SYM_UNDEFINED       // Undefined symbol
} SymbolType;

// Symbol visibility
typedef enum {
    VIS_PRIVATE,        // Private to compilation unit
    VIS_PUBLIC,         // Public (exported)
    VIS_EXTERNAL        // External (imported)
} SymbolVisibility;

// Symbol table entry
typedef struct Symbol {
    char* name;                     // Symbol name
    SymbolType type;                // Symbol type
    SymbolVisibility visibility;    // Symbol visibility
    unsigned long address;          // Symbol address (after linking)
    unsigned long size;             // Symbol size
    char* section;                  // Section name (.text, .data, .bss)
    int object_file_id;             // Which object file is this from?
    bool is_defined;                // Is this symbol defined?
    struct Symbol* next;            // For hash table chaining
} Symbol;

// Relocation entry
typedef struct Relocation {
    char* symbol_name;              // Symbol to relocate
    unsigned long offset;           // Offset in section
    char* section;                  // Section containing relocation
    int relocation_type;            // Relocation type (R_X86_64_*, etc.)
    long addend;                    // Addend for relocation
    struct Relocation* next;
} Relocation;

// Object file representation
typedef struct ObjectFile {
    char* filename;                 // Object file name
    int id;                         // Unique ID
    Symbol* symbols;                // Symbol table
    Relocation* relocations;        // Relocation table
    unsigned char* text_section;    // .text section data
    unsigned char* data_section;    // .data section data
    unsigned char* bss_section;     // .bss section data
    unsigned long text_size;
    unsigned long data_size;
    unsigned long bss_size;
    struct ObjectFile* next;
} ObjectFile;

// Linker context
typedef struct LinkerContext {
    ObjectFile* object_files;       // List of object files
    Symbol* global_symbols;         // Global symbol table
    Relocation* unresolved_relocs;  // Unresolved relocations
    unsigned long text_base;        // Base address for .text
    unsigned long data_base;        // Base address for .data
    unsigned long bss_base;         // Base address for .bss
    char* output_file;              // Output executable name
    bool verbose;                   // Verbose output
} LinkerContext;

// Linker functions
LinkerContext* linker_context_create();
void linker_context_destroy(LinkerContext* ctx);

// Load object files
bool linker_load_object(LinkerContext* ctx, const char* filename);
bool linker_load_archive(LinkerContext* ctx, const char* filename);

// Symbol resolution
bool linker_resolve_symbols(LinkerContext* ctx);
Symbol* linker_lookup_symbol(LinkerContext* ctx, const char* name);
bool linker_check_undefined_symbols(LinkerContext* ctx);

// Relocation
bool linker_apply_relocations(LinkerContext* ctx);
bool linker_relocate_symbol(LinkerContext* ctx, Relocation* reloc);

// Section layout
void linker_layout_sections(LinkerContext* ctx);
unsigned long linker_allocate_section(LinkerContext* ctx, const char* section);

// Output generation
bool linker_generate_executable(LinkerContext* ctx, const char* output_file);
bool linker_generate_elf(LinkerContext* ctx, const char* output_file);

// Utility functions
void linker_print_symbols(LinkerContext* ctx);
void linker_print_relocations(LinkerContext* ctx);
void linker_print_stats(LinkerContext* ctx);

#endif
