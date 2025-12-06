/**
 * MLP Attributes Module - Code Generator #42 (Stage 1)
 * 
 * Generates x86-64 assembly for attribute annotations
 */

#include "attributes.h"

static const char* attr_type_to_string(AttrType type) {
    switch (type) {
        case ATTR_INLINE: return "inline";
        case ATTR_HOT: return "hot";
        case ATTR_COLD: return "cold";
        case ATTR_NO_MANGLE: return "no_mangle";
        case ATTR_TEST: return "test";
        case ATTR_BENCH: return "bench";
        case ATTR_IGNORE: return "ignore";
        case ATTR_DEPRECATED: return "deprecated";
        case ATTR_DERIVE: return "derive";
        case ATTR_SERIALIZE: return "serialize";
        case ATTR_DESERIALIZE: return "deserialize";
        case ATTR_VALIDATE: return "validate";
        case ATTR_REQUIRED: return "required";
        case ATTR_RANGE: return "range";
        case ATTR_ALLOW: return "allow";
        case ATTR_WARN: return "warn";
        case ATTR_DENY: return "deny";
        case ATTR_DOC: return "doc";
        case ATTR_CFG: return "cfg";
        case ATTR_REPR: return "repr";
        case ATTR_MUST_USE: return "must_use";
        default: return "unknown";
    }
}

void attr_generate_asm(const char* output_file, AttributeCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot write to %s\n", output_file);
        return;
    }

    // Assembly header
    fprintf(f, "; MLP Attributes Module Assembly Output\n");
    fprintf(f, "; Generated from attributes_codegen.c\n\n");

    // ========================================================================
    // DATA SECTION
    // ========================================================================
    fprintf(f, "section .data\n");
    
    // Success message
    fprintf(f, "    msg_ok: db \"Attributes OK!\", 10, 0\n");
    fprintf(f, "    msg_ok_len: equ $ - msg_ok\n\n");
    
    // Attribute summary
    fprintf(f, "    ; Total attributes: %d\n\n", ctx->attr_count);
    
    // Optimization attributes
    fprintf(f, "    ; Optimization attributes:\n");
    fprintf(f, "    ;   @inline: %d\n", ctx->inline_count);
    fprintf(f, "    ;   @hot: %d\n", ctx->hot_count);
    fprintf(f, "    ;   @cold: %d\n\n", ctx->cold_count);
    
    // Testing attributes
    fprintf(f, "    ; Testing attributes:\n");
    fprintf(f, "    ;   @test: %d\n", ctx->test_count);
    fprintf(f, "    ;   @bench: %d\n\n", ctx->bench_count);
    
    // Other categories
    fprintf(f, "    ; Other attributes:\n");
    fprintf(f, "    ;   @deprecated: %d\n", ctx->deprecated_count);
    fprintf(f, "    ;   @derive: %d\n", ctx->derive_count);
    fprintf(f, "    ;   @serialize: %d\n", ctx->serialize_count);
    fprintf(f, "    ;   @deserialize: %d\n", ctx->deserialize_count);
    fprintf(f, "    ;   @validate: %d\n", ctx->validate_count);
    fprintf(f, "    ;   lints: %d\n", ctx->lint_count);
    fprintf(f, "    ;   @doc: %d\n", ctx->doc_count);
    fprintf(f, "    ;   @cfg: %d\n", ctx->cfg_count);
    fprintf(f, "    ;   @repr: %d\n\n", ctx->repr_count);
    
    // List all attributes
    fprintf(f, "    ; Attribute details:\n");
    for (int i = 0; i < ctx->attr_count; i++) {
        Attribute* attr = &ctx->attrs[i];
        fprintf(f, "    ;   [%d] @%s", i, attr->name);
        if (attr->params[0]) {
            fprintf(f, "(%s)", attr->params);
        }
        if (attr->target[0]) {
            fprintf(f, " -> %s", attr->target);
        }
        fprintf(f, "\n");
    }
    fprintf(f, "\n");

    // ========================================================================
    // BSS SECTION
    // ========================================================================
    fprintf(f, "section .bss\n");
    fprintf(f, "    ; Runtime storage for attribute metadata\n");
    fprintf(f, "    attr_flags: resb 64\n\n");

    // ========================================================================
    // TEXT SECTION
    // ========================================================================
    fprintf(f, "section .text\n");
    fprintf(f, "    global _start\n\n");

    // Main entry point
    fprintf(f, "_start:\n");
    fprintf(f, "    ; Process attributes\n");
    
    // Generate code for each attribute
    for (int i = 0; i < ctx->attr_count; i++) {
        Attribute* attr = &ctx->attrs[i];
        fprintf(f, "    ; Attribute %d: @%s", i, attr->name);
        if (attr->params[0]) {
            fprintf(f, "(%s)", attr->params);
        }
        fprintf(f, "\n");
        
        // Generate attribute-specific code
        switch (attr->type) {
            case ATTR_INLINE:
                fprintf(f, "    ;   -> inline optimization hint\n");
                break;
            case ATTR_HOT:
                fprintf(f, "    ;   -> hot path optimization\n");
                break;
            case ATTR_COLD:
                fprintf(f, "    ;   -> cold path optimization\n");
                break;
            case ATTR_TEST:
                fprintf(f, "    ;   -> mark as test function\n");
                break;
            case ATTR_BENCH:
                fprintf(f, "    ;   -> mark as benchmark\n");
                break;
            case ATTR_DEPRECATED:
                fprintf(f, "    ;   -> deprecation warning\n");
                break;
            case ATTR_DERIVE:
                fprintf(f, "    ;   -> derive macro: %s\n", attr->params);
                break;
            case ATTR_SERIALIZE:
                fprintf(f, "    ;   -> serialization support\n");
                break;
            case ATTR_DESERIALIZE:
                fprintf(f, "    ;   -> deserialization support\n");
                break;
            case ATTR_VALIDATE:
                fprintf(f, "    ;   -> validation rule\n");
                break;
            case ATTR_ALLOW:
            case ATTR_WARN:
            case ATTR_DENY:
                fprintf(f, "    ;   -> lint rule: %s\n", attr->params);
                break;
            case ATTR_DOC:
                fprintf(f, "    ;   -> documentation: %s\n", attr->params);
                break;
            case ATTR_CFG:
                fprintf(f, "    ;   -> conditional compilation: %s\n", attr->params);
                break;
            case ATTR_REPR:
                fprintf(f, "    ;   -> memory layout: %s\n", attr->params);
                break;
            default:
                fprintf(f, "    ;   -> generic attribute\n");
                break;
        }
    }
    
    fprintf(f, "\n");
    fprintf(f, "    ; Print success message\n");
    fprintf(f, "    mov rax, 1              ; sys_write\n");
    fprintf(f, "    mov rdi, 1              ; stdout\n");
    fprintf(f, "    mov rsi, msg_ok\n");
    fprintf(f, "    mov rdx, msg_ok_len\n");
    fprintf(f, "    syscall\n\n");

    fprintf(f, "    ; Exit\n");
    fprintf(f, "    mov rax, 60             ; sys_exit\n");
    fprintf(f, "    xor rdi, rdi            ; exit code 0\n");
    fprintf(f, "    syscall\n");

    fclose(f);
}
