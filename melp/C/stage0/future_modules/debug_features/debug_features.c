#include "debug_features.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Debug label oluştur
DebugLabel* debug_label_create(const char* name, int line_number) {
    DebugLabel* label = malloc(sizeof(DebugLabel));
    label->name = strdup(name);
    label->line_number = line_number;
    label->location = NULL;
    return label;
}

// Debug goto oluştur
DebugGoto* debug_goto_create(const char* target_label) {
    DebugGoto* dgoto = malloc(sizeof(DebugGoto));
    dgoto->target_label = strdup(target_label);
    dgoto->is_conditional = 0;
    dgoto->condition = NULL;
    return dgoto;
}

// Debug pause oluştur
DebugPause* debug_pause_create(int line_number) {
    DebugPause* pause = malloc(sizeof(DebugPause));
    pause->line_number = line_number;
    pause->message = NULL;
    pause->is_conditional = 0;
    pause->condition = NULL;
    return pause;
}

// Debug print oluştur
DebugPrint* debug_print_create(const char* var_name) {
    DebugPrint* print = malloc(sizeof(DebugPrint));
    print->var_name = strdup(var_name);
    print->value = NULL;
    print->format = NULL;
    return print;
}

// Debug if oluştur
DebugIf* debug_if_create(void* condition) {
    DebugIf* dif = malloc(sizeof(DebugIf));
    dif->condition = condition;
    dif->then_block = NULL;
    dif->else_block = NULL;
    dif->contains_goto = 0;
    dif->contains_pause = 0;
    return dif;
}

// Debug context oluştur
DebugContext* debug_context_create(void) {
    DebugContext* ctx = malloc(sizeof(DebugContext));
    ctx->mode = DEBUG_OFF;
    ctx->label_count = 0;
    ctx->labels = malloc(sizeof(DebugLabel*) * 32);
    ctx->breakpoint_count = 0;
    ctx->breakpoints = malloc(sizeof(DebugPause*) * 32);
    ctx->current_line = 0;
    ctx->is_paused = 0;
    return ctx;
}

// Debug context mode ayarla
void debug_context_set_mode(DebugContext* ctx, DebugMode mode) {
    if (!ctx) return;
    ctx->mode = mode;
}

// Debug context'e label ekle
void debug_context_add_label(DebugContext* ctx, DebugLabel* label) {
    if (!ctx || !label) return;
    ctx->labels[ctx->label_count] = label;
    ctx->label_count++;
}

// Debug context'e breakpoint ekle
void debug_context_add_breakpoint(DebugContext* ctx, DebugPause* pause) {
    if (!ctx || !pause) return;
    ctx->breakpoints[ctx->breakpoint_count] = pause;
    ctx->breakpoint_count++;
}

// Debug context'te label bul
DebugLabel* debug_context_find_label(DebugContext* ctx, const char* name) {
    if (!ctx || !name) return NULL;
    
    for (int i = 0; i < ctx->label_count; i++) {
        if (strcmp(ctx->labels[i]->name, name) == 0) {
            return ctx->labels[i];
        }
    }
    
    return NULL;
}

// Debug goto condition ayarla
void debug_goto_set_condition(DebugGoto* dgoto, void* condition) {
    if (!dgoto) return;
    dgoto->condition = condition;
    dgoto->is_conditional = 1;
}

// Debug goto atlamalı mı?
int debug_goto_should_jump(DebugGoto* dgoto) {
    if (!dgoto) return 0;
    
    if (!dgoto->is_conditional) return 1;
    
    // Condition check (Stage 0: Her zaman true)
    return 1;
}

// Debug pause mesaj ayarla
void debug_pause_set_message(DebugPause* pause, const char* message) {
    if (!pause) return;
    pause->message = strdup(message);
}

// Debug pause condition ayarla
void debug_pause_set_condition(DebugPause* pause, void* condition) {
    if (!pause) return;
    pause->condition = condition;
    pause->is_conditional = 1;
}

// Debug pause break yapmalı mı?
int debug_pause_should_break(DebugPause* pause) {
    if (!pause) return 0;
    
    if (!pause->is_conditional) return 1;
    
    // Condition check (Stage 0: Her zaman true)
    return 1;
}

// Debug print format ayarla
void debug_print_set_format(DebugPrint* print, const char* format) {
    if (!print) return;
    print->format = strdup(format);
}

// Debug print execute
void debug_print_execute(DebugPrint* print) {
    if (!print) return;
    
    // Print variable value
    printf("[DEBUG] %s = ", print->var_name);
    
    if (print->format) {
        printf("%s\n", print->format);
    } else {
        printf("<value>\n");
    }
}

// Debug if then block ayarla
void debug_if_set_then_block(DebugIf* dif, void* block) {
    if (!dif) return;
    dif->then_block = block;
}

// Debug if else block ayarla
void debug_if_set_else_block(DebugIf* dif, void* block) {
    if (!dif) return;
    dif->else_block = block;
}

// Debug goto execute
void debug_execute_goto(DebugContext* ctx, DebugGoto* dgoto) {
    if (!ctx || !dgoto) return;
    
    if (ctx->mode == DEBUG_OFF) return;
    
    if (!debug_goto_should_jump(dgoto)) return;
    
    // Label bul
    DebugLabel* label = debug_context_find_label(ctx, dgoto->target_label);
    
    if (label) {
        printf("[DEBUG] goto %s (line %d)\n", 
               label->name, label->line_number);
        // Jump implementation (Stage 0: Skip)
    }
}

// Debug pause execute
void debug_execute_pause(DebugContext* ctx, DebugPause* pause) {
    if (!ctx || !pause) return;
    
    if (ctx->mode == DEBUG_OFF) return;
    
    if (!debug_pause_should_break(pause)) return;
    
    printf("[DEBUG] BREAKPOINT at line %d", pause->line_number);
    
    if (pause->message) {
        printf(": %s", pause->message);
    }
    
    printf("\n");
    
    ctx->is_paused = 1;
}

// Debug print execute (context)
void debug_execute_print(DebugContext* ctx, DebugPrint* print) {
    if (!ctx || !print) return;
    
    if (ctx->mode == DEBUG_OFF) return;
    
    debug_print_execute(print);
}

// Production mode'da debug kodu strip edilmeli mi?
int should_strip_debug(DebugMode mode) {
    return mode == DEBUG_OFF;
}

// Debug kodu strip et (production)
void strip_debug_code(void* ast) {
    // AST'den debug node'larını çıkar
    // Stage 0: Placeholder
}

// Debug mode kontrolü
int is_debug_mode(DebugContext* ctx) {
    return ctx ? ctx->mode == DEBUG_ON : 0;
}

// Current line ayarla
void debug_set_current_line(DebugContext* ctx, int line) {
    if (!ctx) return;
    ctx->current_line = line;
}

// Bellek temizleme
void debug_label_free(DebugLabel* label) {
    if (!label) return;
    free(label->name);
    free(label);
}

void debug_goto_free(DebugGoto* dgoto) {
    if (!dgoto) return;
    free(dgoto->target_label);
    free(dgoto);
}

void debug_pause_free(DebugPause* pause) {
    if (!pause) return;
    if (pause->message) free(pause->message);
    free(pause);
}

void debug_print_free(DebugPrint* print) {
    if (!print) return;
    free(print->var_name);
    if (print->format) free(print->format);
    free(print);
}

void debug_if_free(DebugIf* dif) {
    if (!dif) return;
    free(dif);
}

void debug_context_free(DebugContext* ctx) {
    if (!ctx) return;
    
    for (int i = 0; i < ctx->label_count; i++) {
        debug_label_free(ctx->labels[i]);
    }
    
    for (int i = 0; i < ctx->breakpoint_count; i++) {
        debug_pause_free(ctx->breakpoints[i]);
    }
    
    free(ctx->labels);
    free(ctx->breakpoints);
    free(ctx);
}
