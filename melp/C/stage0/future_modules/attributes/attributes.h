/**
 * MLP Attributes Module - Header #42 (Stage 1)
 * 
 * MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (41 modül)
 * 
 * İçerik:
 * - Compile-time attributes: @inline, @cold, @hot
 * - Deprecation: @deprecated, @deprecated("message")
 * - Testing: @test, @ignore, @bench
 * - Derive macros: @derive(Debug), @derive(Clone, Copy)
 * - Serialization: @serialize, @deserialize, @skip
 * - Validation: @validate, @required, @range(min, max)
 * - Lints: @allow(warning), @warn(lint), @deny(lint)
 * - Documentation: @doc("description")
 */

#ifndef MLP_ATTRIBUTES_H
#define MLP_ATTRIBUTES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// ATTRIBUTE TYPES
// ============================================================================

typedef enum {
    ATTR_NONE,
    // Compile-time optimization
    ATTR_INLINE,
    ATTR_HOT,
    ATTR_COLD,
    ATTR_NO_MANGLE,
    ATTR_CONST,
    ATTR_PURE,
    // Testing
    ATTR_TEST,
    ATTR_BENCH,
    ATTR_IGNORE,
    ATTR_SHOULD_PANIC,
    // Deprecation
    ATTR_DEPRECATED,
    ATTR_DEPRECATED_MSG,
    // Derive macros
    ATTR_DERIVE,
    ATTR_DERIVE_DEBUG,
    ATTR_DERIVE_CLONE,
    ATTR_DERIVE_COPY,
    ATTR_DERIVE_EQ,
    ATTR_DERIVE_PARTIAL_EQ,
    ATTR_DERIVE_ORD,
    ATTR_DERIVE_HASH,
    // Serialization
    ATTR_SERIALIZE,
    ATTR_DESERIALIZE,
    ATTR_SKIP,
    ATTR_RENAME,
    // Validation
    ATTR_VALIDATE,
    ATTR_REQUIRED,
    ATTR_RANGE,
    ATTR_LENGTH,
    ATTR_PATTERN,
    // Lints
    ATTR_ALLOW,
    ATTR_WARN,
    ATTR_DENY,
    ATTR_FORBID,
    // Documentation
    ATTR_DOC,
    ATTR_DOC_HIDDEN,
    ATTR_DOC_INLINE,
    // Conditional compilation
    ATTR_CFG,
    ATTR_CFG_ATTR,
    // FFI
    ATTR_REPR,
    ATTR_PACKED,
    ATTR_ALIGN,
    // Misc
    ATTR_MUST_USE,
    ATTR_NO_STD,
    ATTR_NO_CORE
} AttrType;

// ============================================================================
// DATA STRUCTURES
// ============================================================================

typedef struct {
    AttrType type;
    char name[64];
    char params[256];
    char target[128];  // function/struct/field name
    int line;
} Attribute;

typedef struct {
    char target[64];
    int attr_count;
    AttrType attrs[32];
} TargetAttrs;

typedef struct {
    Attribute attrs[512];
    int attr_count;
    TargetAttrs targets[128];
    int target_count;
    int inline_count;
    int hot_count;
    int cold_count;
    int test_count;
    int bench_count;
    int deprecated_count;
    int derive_count;
    int serialize_count;
    int deserialize_count;
    int validate_count;
    int lint_count;
    int doc_count;
    int cfg_count;
    int repr_count;
} AttributeCtx;

// ============================================================================
// PARSER STATISTICS
// ============================================================================

typedef struct {
    int total_attributes;
    int inline_attrs;
    int hot_attrs;
    int cold_attrs;
    int test_attrs;
    int bench_attrs;
    int ignore_attrs;
    int deprecated_attrs;
    int derive_attrs;
    int serialize_attrs;
    int deserialize_attrs;
    int validate_attrs;
    int allow_attrs;
    int warn_attrs;
    int deny_attrs;
    int doc_attrs;
    int cfg_attrs;
    int repr_attrs;
    int must_use_attrs;
} AttrParseStats;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================

// Context management
AttributeCtx* attr_ctx_create(void);
void attr_ctx_free(AttributeCtx* ctx);

// Context operations
void attr_add(AttributeCtx* ctx, AttrType type, const char* name, const char* params, const char* target, int line);
void attr_add_to_target(AttributeCtx* ctx, const char* target, AttrType type);

// Parser functions (from attributes_parser.c)
void attr_parser_init(const char* source);
void attr_parse_file(AttributeCtx* ctx, AttrParseStats* stats);

// Codegen functions (from attributes_codegen.c)
void attr_generate_asm(const char* output_file, AttributeCtx* ctx);

#endif /* MLP_ATTRIBUTES_H */
