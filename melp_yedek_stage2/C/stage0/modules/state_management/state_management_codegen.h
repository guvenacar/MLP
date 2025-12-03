#ifndef STATE_MANAGEMENT_CODEGEN_H
#define STATE_MANAGEMENT_CODEGEN_H

#include "state_management.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_state_machine(FILE* out, StateMachine* machine);
void codegen_state(FILE* out, State* state);
void codegen_transition(FILE* out, Transition* trans);
void codegen_event_dispatch(FILE* out, Event* event);

// State machine yardımcı fonksiyonlar
void codegen_state_table(FILE* out, StateMachine* machine);
void codegen_transition_table(FILE* out, StateMachine* machine);
void codegen_state_enter(FILE* out, State* state);
void codegen_state_exit(FILE* out, State* state);

// Event yönetimi
void codegen_event_handler(FILE* out, StateMachine* machine);
void codegen_event_queue(FILE* out);

#endif // STATE_MANAGEMENT_CODEGEN_H
