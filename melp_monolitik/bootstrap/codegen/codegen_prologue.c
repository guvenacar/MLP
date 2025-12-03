// ============================================================================
// MELP Modular Codegen - codegen_prologue.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_emit_prologue, codegen_emit_epilogue, is_builtin_function
// Programın başlangıç ve bitiş kodlarını üretir

// Forward declaration
void codegen_generate_statement(Codegen* gen, Statement* stmt);
void codegen_generate_expression_value(Codegen* gen, Expression* expr);
void codegen_generate_comparison(Codegen* gen, Expression* condition, int false_label);
void codegen_generate_for_in(Codegen* gen, Statement* stmt);

void codegen_emit_prologue(Codegen* gen) {
    codegen_emit(gen, "section .data");
    codegen_emit(gen, "    ; String literals will be added here");
    codegen_emit(gen, "");
    codegen_emit(gen, "section .text");
    codegen_emit(gen, "extern print_number");
    codegen_emit(gen, "extern print_double");  // TTO: For floating point
    codegen_emit(gen, "extern print_string");
    codegen_emit(gen, "extern string_length");
    codegen_emit(gen, "extern string_concat");
    codegen_emit(gen, "extern string_equal");
    codegen_emit(gen, "extern string_not_equal");
    codegen_emit(gen, "extern int_to_string");
    codegen_emit(gen, "extern malloc");
    codegen_emit(gen, "extern free");
    codegen_emit(gen, "extern mlp_array_alloc");
    codegen_emit(gen, "extern mlp_array_free");
    codegen_emit(gen, "extern mlp_array_length");
    codegen_emit(gen, "extern mlp_array_resize");
    codegen_emit(gen, "extern mlp_range");
    codegen_emit(gen, "extern mlp_range1");
    codegen_emit(gen, "extern mlp_range2");
    codegen_emit(gen, "extern mlp_file_read");
    codegen_emit(gen, "extern mlp_file_write");
    codegen_emit(gen, "extern mlp_file_exists");
    codegen_emit(gen, "extern mlp_file_append");
    codegen_emit(gen, "extern mlp_substring");
    codegen_emit(gen, "extern mlp_indexOf");
    codegen_emit(gen, "extern mlp_charAt");
    codegen_emit(gen, "extern mlp_string_length");
    codegen_emit(gen, "extern mlp_get_argv");
    codegen_emit(gen, "extern mlp_get_argc");
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
    // Phase 19: GC functions
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
    // Phase 20: Type conversion functions
    codegen_emit(gen, "extern mlp_to_numeric");
    codegen_emit(gen, "extern mlp_to_string");
    codegen_emit(gen, "extern mlp_to_decimal");
    codegen_emit(gen, "extern mlp_decimal_to_string");
    codegen_emit(gen, "extern mlp_parse_int");
    codegen_emit(gen, "extern mlp_parse_float");
    // Phase 20: Input functions
    codegen_emit(gen, "extern mlp_input");
    codegen_emit(gen, "extern mlp_input_prompt");
    // Phase 20: Math functions
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
    // Phase 20: String functions
    codegen_emit(gen, "extern mlp_split");
    codegen_emit(gen, "extern mlp_trim");
    codegen_emit(gen, "extern mlp_replace");
    codegen_emit(gen, "extern mlp_to_upper");
    codegen_emit(gen, "extern mlp_to_lower");
    codegen_emit(gen, "extern mlp_starts_with");
    codegen_emit(gen, "extern mlp_ends_with");
    codegen_emit(gen, "extern mlp_contains");
    // Phase 20: Assert function
    codegen_emit(gen, "extern mlp_assert");
    // Phase 20: Bitwise operations
    codegen_emit(gen, "extern mlp_band");
    codegen_emit(gen, "extern mlp_bor");
    codegen_emit(gen, "extern mlp_bxor");
    codegen_emit(gen, "extern mlp_bnot");
    codegen_emit(gen, "extern mlp_shl");
    codegen_emit(gen, "extern mlp_shr");
    codegen_emit(gen, "extern mlp_ushr");
    codegen_emit(gen, "global _start");
}

// Helper function to check if a function name is a builtin/runtime function
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
        // Phase 19: GC functions
        "gc_init", "gc_alloc", "gc_free", "gc_retain", "gc_release",
        "gc_collect", "gc_full_collect", "gc_shutdown",
        "gc_get_object_count", "gc_get_total_bytes", "gc_get_collections",
        // Phase 20: Type conversion functions
        "mlp_to_numeric", "mlp_to_string", "mlp_to_decimal", "mlp_decimal_to_string",
        "mlp_parse_int", "mlp_parse_float",
        // Phase 20: Input functions
        "mlp_input", "mlp_input_prompt",
        // Phase 20: Math functions
        "mlp_sin", "mlp_cos", "mlp_tan", "mlp_sqrt", "mlp_pow",
        "mlp_abs", "mlp_abs_int", "mlp_floor", "mlp_ceil", "mlp_round",
        "mlp_log", "mlp_log10", "mlp_exp", "mlp_fmod", "mlp_pi", "mlp_e",
        // Phase 20: String functions
        "mlp_split", "mlp_trim", "mlp_replace", "mlp_to_upper", "mlp_to_lower",
        "mlp_starts_with", "mlp_ends_with", "mlp_contains",
        // Phase 20: Assert function
        "mlp_assert",
        // Phase 20: Bitwise operations
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

void codegen_emit_epilogue(Codegen* gen) {
    codegen_emit(gen, "");
    // Phase 19: Shutdown GC before exit
    codegen_emit(gen, "    ; Shutdown GC");
    codegen_emit(gen, "    call gc_shutdown");
    codegen_emit(gen, "");
    codegen_emit(gen, "    ; Exit program");
    codegen_emit(gen, "    mov rax, 60        ; sys_exit");
    codegen_emit(gen, "    xor rdi, rdi       ; exit code 0");
    codegen_emit(gen, "    syscall");
}

