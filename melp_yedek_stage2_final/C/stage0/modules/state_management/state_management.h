#ifndef STATE_MANAGEMENT_H
#define STATE_MANAGEMENT_H

#include <stddef.h>
#include "../../lexer.h"

// State tanımı
typedef struct State {
    char* name;             // State adı
    void* data;             // State verisi
    size_t data_size;       // Veri boyutu
    void* on_enter;         // Enter callback
    void* on_exit;          // Exit callback
} State;

// State machine
typedef struct StateMachine {
    char* name;             // Machine adı
    State** states;         // State listesi
    int state_count;        // State sayısı
    State* current_state;   // Mevcut state
    State* initial_state;   // Başlangıç state'i
} StateMachine;

// Transition (geçiş)
typedef struct Transition {
    State* from;            // Kaynak state
    State* to;              // Hedef state
    char* event;            // Tetikleyici event
    void* guard;            // Guard koşulu (predicate)
    void* action;           // Geçiş aksiyonu
} Transition;

// Event (olay)
typedef struct Event {
    char* name;             // Event adı
    void* payload;          // Event verisi
    size_t payload_size;    // Veri boyutu
} Event;

// State context (history, veri)
typedef struct StateContext {
    State** history;        // State geçmişi
    int history_size;       // Geçmiş boyutu
    int history_capacity;   // Geçmiş kapasitesi
    void* shared_data;      // Paylaşılan veri
} StateContext;

// API fonksiyonları
State* state_create(const char* name);
StateMachine* state_machine_create(const char* name);
Transition* transition_create(State* from, State* to, const char* event);
Event* event_create(const char* name, void* payload, size_t payload_size);
StateContext* state_context_create(void);

void state_free(State* state);
void state_machine_free(StateMachine* machine);
void transition_free(Transition* trans);
void event_free(Event* event);
void state_context_free(StateContext* ctx);

// State machine operasyonları
void state_machine_add_state(StateMachine* machine, State* state);
void state_machine_set_initial(StateMachine* machine, State* state);
void state_machine_start(StateMachine* machine);
int state_machine_transition(StateMachine* machine, const char* event);
State* state_machine_current(StateMachine* machine);

// State operasyonları
void state_set_enter_callback(State* state, void* callback);
void state_set_exit_callback(State* state, void* callback);
void state_enter(State* state);
void state_exit(State* state);

// Context operasyonları
void state_context_push(StateContext* ctx, State* state);
State* state_context_pop(StateContext* ctx);
State* state_context_peek(StateContext* ctx);

#endif // STATE_MANAGEMENT_H
