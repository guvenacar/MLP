#include "state_management.h"
#include <stdlib.h>
#include <string.h>

// State oluştur
State* state_create(const char* name) {
    State* state = malloc(sizeof(State));
    state->name = strdup(name);
    state->data = NULL;
    state->data_size = 0;
    state->on_enter = NULL;
    state->on_exit = NULL;
    return state;
}

// State machine oluştur
StateMachine* state_machine_create(const char* name) {
    StateMachine* machine = malloc(sizeof(StateMachine));
    machine->name = strdup(name);
    machine->states = NULL;
    machine->state_count = 0;
    machine->current_state = NULL;
    machine->initial_state = NULL;
    return machine;
}

// Transition oluştur
Transition* transition_create(State* from, State* to, const char* event) {
    Transition* trans = malloc(sizeof(Transition));
    trans->from = from;
    trans->to = to;
    trans->event = strdup(event);
    trans->guard = NULL;
    trans->action = NULL;
    return trans;
}

// Event oluştur
Event* event_create(const char* name, void* payload, size_t payload_size) {
    Event* event = malloc(sizeof(Event));
    event->name = strdup(name);
    event->payload = payload;
    event->payload_size = payload_size;
    return event;
}

// State context oluştur
StateContext* state_context_create(void) {
    StateContext* ctx = malloc(sizeof(StateContext));
    ctx->history_capacity = 16;
    ctx->history = malloc(sizeof(State*) * ctx->history_capacity);
    ctx->history_size = 0;
    ctx->shared_data = NULL;
    return ctx;
}

// State machine'e state ekle
void state_machine_add_state(StateMachine* machine, State* state) {
    if (!machine) return;
    
    machine->states = realloc(machine->states, 
                              sizeof(State*) * (machine->state_count + 1));
    machine->states[machine->state_count] = state;
    machine->state_count++;
}

// İlk state'i belirle
void state_machine_set_initial(StateMachine* machine, State* state) {
    if (!machine) return;
    machine->initial_state = state;
}

// State machine'i başlat
void state_machine_start(StateMachine* machine) {
    if (!machine || !machine->initial_state) return;
    
    machine->current_state = machine->initial_state;
    state_enter(machine->current_state);
}

// State geçişi yap
int state_machine_transition(StateMachine* machine, const char* event) {
    if (!machine || !machine->current_state) return 0;
    
    // Şimdilik basit implementasyon
    // TODO: Transition tablosu ile event'e göre geçiş
    
    return 1;
}

// Mevcut state'i al
State* state_machine_current(StateMachine* machine) {
    if (!machine) return NULL;
    return machine->current_state;
}

// State enter callback ayarla
void state_set_enter_callback(State* state, void* callback) {
    if (!state) return;
    state->on_enter = callback;
}

// State exit callback ayarla
void state_set_exit_callback(State* state, void* callback) {
    if (!state) return;
    state->on_exit = callback;
}

// State'e gir
void state_enter(State* state) {
    if (!state) return;
    // on_enter callback çağrılacak
}

// State'ten çık
void state_exit(State* state) {
    if (!state) return;
    // on_exit callback çağrılacak
}

// Context'e state ekle (history)
void state_context_push(StateContext* ctx, State* state) {
    if (!ctx) return;
    
    if (ctx->history_size >= ctx->history_capacity) {
        ctx->history_capacity *= 2;
        ctx->history = realloc(ctx->history, 
                               sizeof(State*) * ctx->history_capacity);
    }
    
    ctx->history[ctx->history_size] = state;
    ctx->history_size++;
}

// Context'ten state çıkar
State* state_context_pop(StateContext* ctx) {
    if (!ctx || ctx->history_size == 0) return NULL;
    
    ctx->history_size--;
    return ctx->history[ctx->history_size];
}

// Context'teki son state'e bak
State* state_context_peek(StateContext* ctx) {
    if (!ctx || ctx->history_size == 0) return NULL;
    return ctx->history[ctx->history_size - 1];
}

// Bellek temizleme
void state_free(State* state) {
    if (!state) return;
    free(state->name);
    free(state);
}

void state_machine_free(StateMachine* machine) {
    if (!machine) return;
    free(machine->name);
    if (machine->states) {
        for (int i = 0; i < machine->state_count; i++) {
            state_free(machine->states[i]);
        }
        free(machine->states);
    }
    free(machine);
}

void transition_free(Transition* trans) {
    if (!trans) return;
    free(trans->event);
    free(trans);
}

void event_free(Event* event) {
    if (!event) return;
    free(event->name);
    free(event);
}

void state_context_free(StateContext* ctx) {
    if (!ctx) return;
    free(ctx->history);
    free(ctx);
}
