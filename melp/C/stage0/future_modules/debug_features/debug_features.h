#ifndef DEBUG_FEATURES_H
#define DEBUG_FEATURES_H

#include <stddef.h>
#include "../../lexer.h"

// Debug mode
typedef enum DebugMode {
    DEBUG_OFF,              // Production mode
    DEBUG_ON                // Debug mode
} DebugMode;

// Debug label (etiket)
typedef struct DebugLabel {
    char* name;             // Label adı
    int line_number;        // Satır numarası
    void* location;         // Code location
} DebugLabel;

// Debug goto
typedef struct DebugGoto {
    char* target_label;     // Hedef label
    int is_conditional;     // Koşullu mu?
    void* condition;        // Koşul (if içindeyse)
} DebugGoto;

// Debug pause (breakpoint)
typedef struct DebugPause {
    int line_number;        // Satır numarası
    char* message;          // Pause mesajı (optional)
    int is_conditional;     // Koşullu mu?
    void* condition;        // Koşul
} DebugPause;

// Debug print
typedef struct DebugPrint {
    char* var_name;         // Değişken adı
    void* value;            // Değer
    char* format;           // Format string (optional)
} DebugPrint;

// Debug if block
typedef struct DebugIf {
    void* condition;        // Koşul
    void* then_block;       // Then bloğu
    void* else_block;       // Else bloğu (optional)
    int contains_goto;      // Goto içeriyor mu?
    int contains_pause;     // Pause içeriyor mu?
} DebugIf;

// Debug context (global state)
typedef struct DebugContext {
    DebugMode mode;                 // Debug mode
    DebugLabel** labels;            // Label listesi
    int label_count;                // Label sayısı
    DebugPause** breakpoints;       // Breakpoint listesi
    int breakpoint_count;           // Breakpoint sayısı
    int current_line;               // Current line
    int is_paused;                  // Paused mı?
} DebugContext;

// API fonksiyonları
DebugLabel* debug_label_create(const char* name, int line_number);
DebugGoto* debug_goto_create(const char* target_label);
DebugPause* debug_pause_create(int line_number);
DebugPrint* debug_print_create(const char* var_name);
DebugIf* debug_if_create(void* condition);
DebugContext* debug_context_create(void);

void debug_label_free(DebugLabel* label);
void debug_goto_free(DebugGoto* dgoto);
void debug_pause_free(DebugPause* pause);
void debug_print_free(DebugPrint* print);
void debug_if_free(DebugIf* dif);
void debug_context_free(DebugContext* ctx);

// Debug context operasyonları
void debug_context_set_mode(DebugContext* ctx, DebugMode mode);
void debug_context_add_label(DebugContext* ctx, DebugLabel* label);
void debug_context_add_breakpoint(DebugContext* ctx, DebugPause* pause);
DebugLabel* debug_context_find_label(DebugContext* ctx, const char* name);

// Debug goto operasyonları
void debug_goto_set_condition(DebugGoto* dgoto, void* condition);
int debug_goto_should_jump(DebugGoto* dgoto);

// Debug pause operasyonları
void debug_pause_set_message(DebugPause* pause, const char* message);
void debug_pause_set_condition(DebugPause* pause, void* condition);
int debug_pause_should_break(DebugPause* pause);

// Debug print operasyonları
void debug_print_set_format(DebugPrint* print, const char* format);
void debug_print_execute(DebugPrint* print);

// Debug if operasyonları
void debug_if_set_then_block(DebugIf* dif, void* block);
void debug_if_set_else_block(DebugIf* dif, void* block);

// Debug execution
void debug_execute_goto(DebugContext* ctx, DebugGoto* dgoto);
void debug_execute_pause(DebugContext* ctx, DebugPause* pause);
void debug_execute_print(DebugContext* ctx, DebugPrint* print);

// Production mode (strip debug code)
int should_strip_debug(DebugMode mode);
void strip_debug_code(void* ast);

// Utility
int is_debug_mode(DebugContext* ctx);
void debug_set_current_line(DebugContext* ctx, int line);

#endif // DEBUG_FEATURES_H
