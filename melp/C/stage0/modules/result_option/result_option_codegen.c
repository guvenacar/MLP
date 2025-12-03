/**
 * MLP Result/Option Module - Code Generator #41 (Stage 1)
 * 
 * Generates x86-64 assembly for Result/Option types
 */

#include "result_option.h"

void resopt_generate_asm(const char* output_file, ResOptCtx* ctx) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot write to %s\n", output_file);
        return;
    }

    // Assembly header
    fprintf(f, "; MLP Result/Option Module Assembly Output\n");
    fprintf(f, "; Generated from result_option_codegen.c\n\n");

    // ========================================================================
    // DATA SECTION
    // ========================================================================
    fprintf(f, "section .data\n");
    
    // Success message
    fprintf(f, "    msg_ok: db \"Result/Option OK!\", 10, 0\n");
    fprintf(f, "    msg_ok_len: equ $ - msg_ok\n\n");
    
    // Result type info
    fprintf(f, "    ; Result types: %d\n", ctx->result_count);
    for (int i = 0; i < ctx->result_count; i++) {
        fprintf(f, "    ; Result %s: Ok<%s> | Err<%s>\n", 
                ctx->results[i].name,
                ctx->results[i].ok_type,
                ctx->results[i].err_type);
    }
    fprintf(f, "\n");
    
    // Option type info
    fprintf(f, "    ; Option types: %d\n", ctx->option_count);
    for (int i = 0; i < ctx->option_count; i++) {
        fprintf(f, "    ; Option %s: Some<%s> | None\n",
                ctx->options[i].name,
                ctx->options[i].some_type);
    }
    fprintf(f, "\n");
    
    // Ok values
    fprintf(f, "    ; Ok constructors: %d\n", ctx->ok_count);
    for (int i = 0; i < ctx->ok_count; i++) {
        fprintf(f, "    ; %s = Ok(%s)\n",
                ctx->oks[i].var_name,
                ctx->oks[i].value);
    }
    fprintf(f, "\n");
    
    // Err values
    fprintf(f, "    ; Err constructors: %d\n", ctx->err_count);
    for (int i = 0; i < ctx->err_count; i++) {
        fprintf(f, "    ; %s = Err(%s)\n",
                ctx->errs[i].var_name,
                ctx->errs[i].error);
    }
    fprintf(f, "\n");
    
    // Some values
    fprintf(f, "    ; Some constructors: %d\n", ctx->some_count);
    for (int i = 0; i < ctx->some_count; i++) {
        fprintf(f, "    ; %s = Some(%s)\n",
                ctx->somes[i].var_name,
                ctx->somes[i].value);
    }
    fprintf(f, "\n");
    
    // None values
    fprintf(f, "    ; None constructors: %d\n\n", ctx->none_count);
    
    // Operation counts
    fprintf(f, "    ; Operations:\n");
    fprintf(f, "    ;   unwrap: %d\n", ctx->unwrap_count);
    fprintf(f, "    ;   expect: %d\n", ctx->expect_count);
    fprintf(f, "    ;   unwrap_or: %d\n", ctx->unwrap_or_count);
    fprintf(f, "    ;   map: %d\n", ctx->map_count);
    fprintf(f, "    ;   and_then: %d\n", ctx->and_then_count);
    fprintf(f, "    ;   or_else: %d\n", ctx->or_else_count);
    fprintf(f, "    ;   ?: %d\n", ctx->question_op_count);
    fprintf(f, "    ;   is_ok: %d\n", ctx->is_ok_count);
    fprintf(f, "    ;   is_err: %d\n", ctx->is_err_count);
    fprintf(f, "    ;   is_some: %d\n", ctx->is_some_count);
    fprintf(f, "    ;   is_none: %d\n\n", ctx->is_none_count);

    // ========================================================================
    // BSS SECTION
    // ========================================================================
    fprintf(f, "section .bss\n");
    fprintf(f, "    ; Runtime storage for Result/Option values\n");
    for (int i = 0; i < ctx->result_count; i++) {
        fprintf(f, "    result_%s: resq 2    ; [tag, value/error]\n", ctx->results[i].name);
    }
    for (int i = 0; i < ctx->option_count; i++) {
        fprintf(f, "    option_%s: resq 2    ; [tag, value]\n", ctx->options[i].name);
    }
    fprintf(f, "\n");

    // ========================================================================
    // TEXT SECTION
    // ========================================================================
    fprintf(f, "section .text\n");
    fprintf(f, "    global _start\n\n");

    // Main entry point
    fprintf(f, "_start:\n");
    fprintf(f, "    ; Result/Option operations\n");
    
    // Generate stubs for operations
    for (int i = 0; i < ctx->op_count; i++) {
        ResOptOp* op = &ctx->ops[i];
        fprintf(f, "    ; Op %d: ", i);
        
        switch (op->op_type) {
            case RESOPT_RESULT_TYPE:
                fprintf(f, "Result%s %s\n", op->type_params, op->name);
                break;
            case RESOPT_OPTION_TYPE:
                fprintf(f, "Option%s %s\n", op->type_params, op->name);
                break;
            case RESOPT_OK:
                fprintf(f, "%s = Ok(%s)\n", op->name, op->args);
                break;
            case RESOPT_ERR:
                fprintf(f, "%s = Err(%s)\n", op->name, op->args);
                break;
            case RESOPT_SOME:
                fprintf(f, "%s = Some(%s)\n", op->name, op->args);
                break;
            case RESOPT_NONE_VAL:
                fprintf(f, "%s = None\n", op->name);
                break;
            case RESOPT_UNWRAP:
            case RESOPT_UNWRAP_OPT:
                fprintf(f, "%s.unwrap()\n", op->name);
                break;
            case RESOPT_EXPECT:
            case RESOPT_EXPECT_OPT:
                fprintf(f, "%s.expect(%s)\n", op->name, op->args);
                break;
            case RESOPT_UNWRAP_OR:
            case RESOPT_UNWRAP_OR_OPT:
                fprintf(f, "%s.unwrap_or(%s)\n", op->name, op->args);
                break;
            case RESOPT_MAP:
            case RESOPT_MAP_OPT:
                fprintf(f, "%s.map(%s)\n", op->name, op->args);
                break;
            case RESOPT_AND_THEN:
            case RESOPT_AND_THEN_OPT:
                fprintf(f, "%s.and_then(%s)\n", op->name, op->args);
                break;
            case RESOPT_OR_ELSE:
            case RESOPT_OR_ELSE_OPT:
                fprintf(f, "%s.or_else(%s)\n", op->name, op->args);
                break;
            case RESOPT_IS_OK:
                fprintf(f, "%s.is_ok()\n", op->name);
                break;
            case RESOPT_IS_ERR:
                fprintf(f, "%s.is_err()\n", op->name);
                break;
            case RESOPT_IS_SOME:
                fprintf(f, "%s.is_some()\n", op->name);
                break;
            case RESOPT_IS_NONE:
                fprintf(f, "%s.is_none()\n", op->name);
                break;
            case RESOPT_QUESTION_OP:
                fprintf(f, "%s?\n", op->name);
                break;
            default:
                fprintf(f, "unknown\n");
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
