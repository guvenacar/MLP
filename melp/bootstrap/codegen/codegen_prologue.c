// MELP Modular Codegen - Prologue/Epilogue
// Assembly başlangıç ve bitiş bölümleri

#include "codegen.h"

// ============================================================================
// Emit Prologue (Assembly başlangıç)
// ============================================================================

void codegen_emit_prologue(Codegen* gen) {
    codegen_emit(gen, "section .data");
    codegen_emit(gen, "    ; String literals will be added here");
    codegen_emit(gen, "");
    codegen_emit(gen, "section .text");
    
    // Runtime functions
    codegen_emit(gen, "extern print_number");
    codegen_emit(gen, "extern print_double");
    codegen_emit(gen, "extern print_string");
    codegen_emit(gen, "extern string_length");
    codegen_emit(gen, "extern string_concat");
    codegen_emit(gen, "extern string_equal");
    codegen_emit(gen, "extern string_not_equal");
    codegen_emit(gen, "extern int_to_string");
    codegen_emit(gen, "extern malloc");
    codegen_emit(gen, "extern free");
    
    // Array functions
    codegen_emit(gen, "extern mlp_array_alloc");
    codegen_emit(gen, "extern mlp_array_free");
    codegen_emit(gen, "extern mlp_array_length");
    codegen_emit(gen, "extern mlp_array_resize");
    codegen_emit(gen, "extern mlp_range");
    codegen_emit(gen, "extern mlp_range1");
    codegen_emit(gen, "extern mlp_range2");
    
    // File functions
    codegen_emit(gen, "extern mlp_file_read");
    codegen_emit(gen, "extern mlp_file_write");
    codegen_emit(gen, "extern mlp_file_exists");
    codegen_emit(gen, "extern mlp_file_append");
    
    // String functions
    codegen_emit(gen, "extern mlp_substring");
    codegen_emit(gen, "extern mlp_indexOf");
    codegen_emit(gen, "extern mlp_charAt");
    codegen_emit(gen, "extern mlp_string_length");
    
    // CLI functions
    codegen_emit(gen, "extern mlp_get_argv");
    codegen_emit(gen, "extern mlp_get_argc");
    
    // Exception functions
    codegen_emit(gen, "extern setjmp");
    codegen_emit(gen, "extern strcmp");
    codegen_emit(gen, "extern mlp_exception_push");
    codegen_emit(gen, "extern mlp_exception_pop");
    codegen_emit(gen, "extern mlp_throw");
    codegen_emit(gen, "extern mlp_exception_type");
    codegen_emit(gen, "extern mlp_exception_message");
    codegen_emit(gen, "extern mlp_exception_code");
    codegen_emit(gen, "extern mlp_exception_has_handler");
    codegen_emit(gen, "extern mlp_exception_has_parent_handler");
    
    // GC functions
    codegen_emit(gen, "extern gc_init");
    codegen_emit(gen, "extern gc_alloc");
    codegen_emit(gen, "extern gc_free");
    codegen_emit(gen, "extern gc_retain");
    codegen_emit(gen, "extern gc_release");
    codegen_emit(gen, "extern gc_collect");
    codegen_emit(gen, "extern gc_full_collect");
    codegen_emit(gen, "extern gc_shutdown");
    codegen_emit(gen, "extern gc_get_object_count");
    codegen_emit(gen, "extern gc_get_total_bytes");
    codegen_emit(gen, "extern gc_get_collections");
    codegen_emit(gen, "extern gc_enable");
    codegen_emit(gen, "extern gc_disable");
    
    // Type conversion functions
    codegen_emit(gen, "extern mlp_to_numeric");
    codegen_emit(gen, "extern mlp_to_text");
    codegen_emit(gen, "extern mlp_to_decimal");
    codegen_emit(gen, "extern mlp_decimal_to_text");
    codegen_emit(gen, "extern mlp_parse_int");
    codegen_emit(gen, "extern mlp_parse_float");
    
    // Input functions
    codegen_emit(gen, "extern mlp_input");
    codegen_emit(gen, "extern mlp_input_prompt");
    
    // Math functions
    codegen_emit(gen, "extern mlp_sin");
    codegen_emit(gen, "extern mlp_cos");
    codegen_emit(gen, "extern mlp_tan");
    codegen_emit(gen, "extern mlp_sqrt");
    codegen_emit(gen, "extern mlp_pow");
    codegen_emit(gen, "extern mlp_abs");
    codegen_emit(gen, "extern mlp_abs_int");
    codegen_emit(gen, "extern mlp_floor");
    codegen_emit(gen, "extern mlp_ceil");
    codegen_emit(gen, "extern mlp_round");
    codegen_emit(gen, "extern mlp_log");
    codegen_emit(gen, "extern mlp_log10");
    codegen_emit(gen, "extern mlp_exp");
    codegen_emit(gen, "extern mlp_fmod");
    codegen_emit(gen, "extern mlp_pi");
    codegen_emit(gen, "extern mlp_e");
    
    // String utility functions
    codegen_emit(gen, "extern mlp_split");
    codegen_emit(gen, "extern mlp_trim");
    codegen_emit(gen, "extern mlp_replace");
    codegen_emit(gen, "extern mlp_to_upper");
    codegen_emit(gen, "extern mlp_to_lower");
    codegen_emit(gen, "extern mlp_starts_with");
    codegen_emit(gen, "extern mlp_ends_with");
    codegen_emit(gen, "extern mlp_contains");
    
    // Assert function
    codegen_emit(gen, "extern mlp_assert");
    
    // Bitwise operations
    codegen_emit(gen, "extern mlp_band");
    codegen_emit(gen, "extern mlp_bor");
    codegen_emit(gen, "extern mlp_bxor");
    codegen_emit(gen, "extern mlp_bnot");
    codegen_emit(gen, "extern mlp_shl");
    codegen_emit(gen, "extern mlp_shr");
    codegen_emit(gen, "extern mlp_ushr");
    
    codegen_emit(gen, "global _start");
}

// ============================================================================
// Built-in Function Check
// ============================================================================

int is_builtin_function(const char* name) {
    static const char* builtins[] = {
        "print_number", "print_string", "string_length", "string_concat",
        "string_equal", "string_not_equal", "int_to_string",
        "malloc", "free",
        "mlp_array_alloc", "mlp_array_free", "mlp_array_length", "mlp_array_resize",
        "mlp_file_read", "mlp_file_write", "mlp_file_exists", "mlp_file_append",
        "mlp_substring", "mlp_indexOf", "mlp_charAt", "mlp_string_length",
        "mlp_get_argv", "mlp_get_argc",
        "setjmp", "strcmp",
        "mlp_exception_push", "mlp_exception_pop", "mlp_throw",
        "mlp_exception_type", "mlp_exception_message", "mlp_exception_code",
        "mlp_exception_has_handler", "mlp_exception_has_parent_handler",
        "gc_init", "gc_alloc", "gc_free", "gc_retain", "gc_release",
        "gc_collect", "gc_full_collect", "gc_shutdown",
        "gc_get_object_count", "gc_get_total_bytes", "gc_get_collections",
        "mlp_to_numeric", "mlp_to_text", "mlp_to_decimal", "mlp_decimal_to_text",
        "mlp_parse_int", "mlp_parse_float",
        "mlp_input", "mlp_input_prompt",
        "mlp_sin", "mlp_cos", "mlp_tan", "mlp_sqrt", "mlp_pow",
        "mlp_abs", "mlp_abs_int", "mlp_floor", "mlp_ceil", "mlp_round",
        "mlp_log", "mlp_log10", "mlp_exp", "mlp_fmod", "mlp_pi", "mlp_e",
        "mlp_split", "mlp_trim", "mlp_replace", "mlp_to_upper", "mlp_to_lower",
        "mlp_starts_with", "mlp_ends_with", "mlp_contains",
        "mlp_assert",
        "mlp_band", "mlp_bor", "mlp_bxor", "mlp_bnot", "mlp_shl", "mlp_shr", "mlp_ushr",
        NULL
    };
    
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// ============================================================================
// Emit Epilogue (Program bitiş)
// ============================================================================

void codegen_emit_epilogue(Codegen* gen) {
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Shutdown GC");
    codegen_emit(gen, "    call gc_shutdown");
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Exit program");
    codegen_emit(gen, "    mov rax, 60        ; sys_exit");
    codegen_emit(gen, "    xor rdi, rdi       ; exit code 0");
    codegen_emit(gen, "    syscall");
}
