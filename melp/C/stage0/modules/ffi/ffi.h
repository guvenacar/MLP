/**
 * MLP FFI Module - Header #43 (Stage 1)
 * 
 * MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (42 modül)
 * 
 * İçerik:
 * - External functions: extern "C" fn name()
 * - Type mapping: c_int, c_char, c_void, c_float, c_double
 * - Raw pointers: *const T, *mut T
 * - Memory layout: #[repr(C)], #[repr(packed)]
 * - Link directives: #[link(name = "lib")]
 * - Unsafe blocks: unsafe { ... }
 * - Null pointers: ptr::null(), ptr::null_mut()
 * - FFI string conversion: CString, CStr
 */

#ifndef MLP_FFI_H
#define MLP_FFI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// FFI OPERATION TYPES
// ============================================================================

typedef enum {
    FFI_NONE,
    // External declarations
    FFI_EXTERN_C,
    FFI_EXTERN_FN,
    FFI_LINK,
    // C types
    FFI_C_INT,
    FFI_C_CHAR,
    FFI_C_VOID,
    FFI_C_FLOAT,
    FFI_C_DOUBLE,
    FFI_C_LONG,
    FFI_C_SIZE_T,
    // Raw pointers
    FFI_RAW_PTR_CONST,
    FFI_RAW_PTR_MUT,
    FFI_NULL_PTR,
    FFI_NULL_MUT,
    // Memory layout
    FFI_REPR_C,
    FFI_REPR_PACKED,
    FFI_REPR_ALIGN,
    // Unsafe operations
    FFI_UNSAFE_BLOCK,
    FFI_UNSAFE_FN,
    // String conversion
    FFI_CSTRING,
    FFI_CSTR,
    FFI_FROM_PTR,
    FFI_AS_PTR,
    // Pointer operations
    FFI_DEREF,
    FFI_ADDR_OF,
    FFI_CAST
} FFIOpType;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

typedef struct {
    char name[64];
    FFIOpType op_type;
    char params[256];
    char return_type[64];
    int line;
} FFIOp;

typedef struct {
    char name[64];
    char library[128];
} LinkDirective;

typedef struct {
    char name[64];
    char signature[256];
    char return_type[64];
    int is_unsafe;
} ExternFn;

typedef struct {
    char type_name[64];
    char c_type[64];
} TypeMapping;

typedef struct {
    char struct_name[64];
    char repr[32];  // "C", "packed", "align(N)"
} ReprDirective;

typedef struct {
    FFIOp ops[512];
    int op_count;
    LinkDirective links[32];
    int link_count;
    ExternFn extern_fns[64];
    int extern_fn_count;
    TypeMapping type_maps[64];
    int type_map_count;
    ReprDirective reprs[64];
    int repr_count;
    int unsafe_block_count;
    int raw_ptr_count;
    int null_ptr_count;
    int cstring_count;
} FFICtx;

// ============================================================================
// PARSER STATISTICS
// ============================================================================

typedef struct {
    int extern_c_blocks;
    int extern_fns;
    int link_directives;
    int c_types;
    int raw_ptrs;
    int null_ptrs;
    int repr_directives;
    int unsafe_blocks;
    int unsafe_fns;
    int cstring_conversions;
    int pointer_ops;
} FFIParseStats;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// Context management
FFICtx* ffi_ctx_create(void);
void ffi_ctx_free(FFICtx* ctx);

// Context operations
void ffi_add_op(FFICtx* ctx, FFIOpType type, const char* name, const char* params, const char* return_type, int line);
void ffi_add_link(FFICtx* ctx, const char* library);
void ffi_add_extern_fn(FFICtx* ctx, const char* name, const char* signature, const char* return_type, int is_unsafe);
void ffi_add_repr(FFICtx* ctx, const char* struct_name, const char* repr);

// Parser functions (from ffi_parser.c)
void ffi_parser_init(const char* source);
void ffi_parse_file(FFICtx* ctx, FFIParseStats* stats);

// Codegen functions (from ffi_codegen.c)
void ffi_generate_asm(const char* output_file, FFICtx* ctx);

#endif /* MLP_FFI_H */
