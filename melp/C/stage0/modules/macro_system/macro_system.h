#ifndef MACRO_SYSTEM_H
#define MACRO_SYSTEM_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// MACRO TYPES
// ============================================================================

// Macro system types (Rust + C-style)
typedef enum {
    MACRO_DECLARATIVE,      // macro_rules! (pattern matching macros)
    MACRO_PROCEDURAL,       // proc_macro (function-like)
    MACRO_DERIVE,           // #[derive(Debug, Clone)]
    MACRO_ATTRIBUTE,        // #[test], #[inline]
    MACRO_FUNCTION_LIKE,    // println!(), vec![]
    MACRO_BUILTIN           // __LINE__, __FILE__, compile-time builtins
} MacroSystemType;

// Macro expansion mode
typedef enum {
    EXPAND_EAGER,           // Expand immediately
    EXPAND_LAZY,            // Expand on demand
    EXPAND_RECURSIVE        // Allow recursive expansion
} ExpansionMode;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

// Macro pattern (for declarative macros)
typedef struct {
    char pattern[256];      // Match pattern: ($x:expr) => { ... }
    char expansion[512];    // Expansion template
    int priority;           // Match priority (higher = first)
} MacroPattern;

// Declarative macro (macro_rules!)
typedef struct {
    char name[128];
    MacroPattern patterns[16];
    int pattern_count;
    ExpansionMode mode;
    int line;
} DeclarativeMacro;

// Procedural macro (compile-time function)
typedef struct {
    char name[128];
    char input_type[64];    // TokenStream, DeriveInput, etc.
    char output_type[64];   // TokenStream
    char body[512];         // Macro body (compile-time code)
    int line;
} ProceduralMacro;

// Derive macro (#[derive(...)])
typedef struct {
    char trait_name[64];    // Debug, Clone, Copy, etc.
    char struct_name[128];  // Target struct
    char generated_code[512]; // Generated implementation
    int line;
} DeriveMacro;

// Attribute macro (#[attribute])
typedef struct {
    char attr_name[64];     // test, inline, deprecated, etc.
    char target_name[128];  // Function/struct name
    char target_type[32];   // function, struct, enum, etc.
    char parameters[256];   // Attribute parameters
    char transformed_code[512]; // Transformed code
    int line;
} AttributeMacro;

// Macro invocation (macro call site)
typedef struct {
    char macro_name[128];
    char arguments[512];    // Macro arguments
    MacroSystemType type;
    char expanded[1024];    // Expanded output
    int line;
} MacroInvocation;

// Built-in macros
typedef struct {
    char name[64];
    char value[256];        // __LINE__ -> "42", __FILE__ -> "main.mlp"
    bool is_compile_time;
} BuiltinMacro;

// ============================================================================
// CONTEXT
// ============================================================================

typedef struct {
    // Declarative macros (macro_rules!)
    DeclarativeMacro declarative[64];
    int declarative_count;
    
    // Procedural macros
    ProceduralMacro procedural[32];
    int procedural_count;
    
    // Derive macros
    DeriveMacro derives[32];
    int derive_count;
    
    // Attribute macros
    AttributeMacro attributes[64];
    int attribute_count;
    
    // Macro invocations
    MacroInvocation invocations[128];
    int invocation_count;
    
    // Built-in macros
    BuiltinMacro builtins[16];
    int builtin_count;
    
    // Expansion statistics
    int total_expansions;
    int max_recursion_depth;
    int current_recursion_depth;
    
    // Error tracking
    char error_message[256];
    int error_line;
} MacroSystemCtx;

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

MacroSystemCtx* macro_ctx_create(void);
void macro_ctx_free(MacroSystemCtx* ctx);

// ============================================================================
// DECLARATIVE MACROS (macro_rules!)
// ============================================================================

// Define: macro_rules! name { pattern => expansion, ... }
bool macro_define_declarative(MacroSystemCtx* ctx, const char* name,
                               const char* pattern, const char* expansion,
                               int line);

// Match and expand declarative macro
char* macro_expand_declarative(MacroSystemCtx* ctx, const char* name,
                                const char* arguments, int line);

// ============================================================================
// PROCEDURAL MACROS
// ============================================================================

// Define: proc_macro fn_name(input: TokenStream) -> TokenStream
bool macro_define_procedural(MacroSystemCtx* ctx, const char* name,
                              const char* input_type, const char* output_type,
                              const char* body, int line);

// Execute procedural macro
char* macro_execute_procedural(MacroSystemCtx* ctx, const char* name,
                                const char* input, int line);

// ============================================================================
// DERIVE MACROS
// ============================================================================

// Define: #[derive(TraitName)]
bool macro_define_derive(MacroSystemCtx* ctx, const char* trait_name,
                          const char* struct_name, int line);

// Generate derived implementation
char* macro_generate_derive(MacroSystemCtx* ctx, const char* trait_name,
                            const char* struct_name, int line);

// ============================================================================
// ATTRIBUTE MACROS
// ============================================================================

// Define: #[attribute_name(params)]
bool macro_define_attribute(MacroSystemCtx* ctx, const char* attr_name,
                             const char* target_name, const char* target_type,
                             const char* parameters, int line);

// Apply attribute transformation
char* macro_apply_attribute(MacroSystemCtx* ctx, const char* attr_name,
                            const char* target_code, int line);

// ============================================================================
// BUILT-IN MACROS
// ============================================================================

void macro_init_builtins(MacroSystemCtx* ctx, const char* file_name, int line);
char* macro_get_builtin(MacroSystemCtx* ctx, const char* name);

// ============================================================================
// MACRO INVOCATION
// ============================================================================

// Parse and expand macro invocation
bool macro_invoke(MacroSystemCtx* ctx, const char* invocation, int line);

// Expand all macros in source code
char* macro_expand_all(MacroSystemCtx* ctx, const char* source);

// ============================================================================
// PARSER FUNCTIONS
// ============================================================================

void macro_parser_init(const char* source);
bool macro_parse_file(MacroSystemCtx* ctx);
bool macro_parse_declarative(MacroSystemCtx* ctx, const char* line);
bool macro_parse_derive(MacroSystemCtx* ctx, const char* line);
bool macro_parse_attribute(MacroSystemCtx* ctx, const char* line);
bool macro_parse_invocation(MacroSystemCtx* ctx, const char* line);

// ============================================================================
// CODEGEN FUNCTIONS
// ============================================================================

void macro_generate_asm(const char* output_file, MacroSystemCtx* ctx);
void macro_codegen_expansion(FILE* out, MacroInvocation* invoc);
void macro_codegen_derive(FILE* out, DeriveMacro* derive);
void macro_codegen_attribute(FILE* out, AttributeMacro* attr);

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

bool macro_pattern_match(const char* pattern, const char* input, char** captures);
char* macro_substitute(const char* template, char** captures, int capture_count);
void macro_report_stats(MacroSystemCtx* ctx);
const char* macro_type_str(MacroSystemType type);

#endif // MACRO_SYSTEM_H
