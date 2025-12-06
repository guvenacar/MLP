#include "state_management_codegen.h"
#include <string.h>

// State machine için kod üret
void codegen_state_machine(FILE* out, StateMachine* machine) {
    fprintf(out, "    ; State Machine: %s\n", machine->name);
    
    // State tablosu üret
    codegen_state_table(out, machine);
    
    // Transition tablosu üret
    codegen_transition_table(out, machine);
    
    // Event handler üret
    codegen_event_handler(out, machine);
    
    // Machine initialization
    fprintf(out, "\n.sm_%s_init:\n", machine->name);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    // Initial state'e geç
    if (machine->initial_state) {
        fprintf(out, "    lea rax, [rel .state_%s]\n", 
                machine->initial_state->name);
        fprintf(out, "    mov [rel .sm_%s_current], rax\n", machine->name);
        
        codegen_state_enter(out, machine->initial_state);
    }
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// State için kod üret
void codegen_state(FILE* out, State* state) {
    fprintf(out, "\n.state_%s:\n", state->name);
    fprintf(out, "    ; State: %s\n", state->name);
    
    // State data section
    fprintf(out, "section .data\n");
    fprintf(out, ".state_%s_name: db \"%s\", 0\n", state->name, state->name);
    fprintf(out, "section .text\n");
}

// Transition için kod üret
void codegen_transition(FILE* out, Transition* trans) {
    fprintf(out, "    ; Transition: %s -> %s on '%s'\n",
            trans->from->name, trans->to->name, trans->event);
    
    // Exit current state
    codegen_state_exit(out, trans->from);
    
    // Action (varsa)
    if (trans->action) {
        fprintf(out, "    ; Transition action\n");
        fprintf(out, "    call .transition_action\n");
    }
    
    // Enter new state
    codegen_state_enter(out, trans->to);
    
    // Update current state
    fprintf(out, "    lea rax, [rel .state_%s]\n", trans->to->name);
    fprintf(out, "    mov [rel .sm_current], rax\n");
}

// State tablosu üret
void codegen_state_table(FILE* out, StateMachine* machine) {
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".sm_%s_states:\n", machine->name);
    
    for (int i = 0; i < machine->state_count; i++) {
        fprintf(out, "    dq .state_%s\n", machine->states[i]->name);
    }
    
    fprintf(out, ".sm_%s_state_count: dq %d\n", 
            machine->name, machine->state_count);
    fprintf(out, ".sm_%s_current: dq 0\n", machine->name);
    
    fprintf(out, "section .text\n");
}

// Transition tablosu üret
void codegen_transition_table(FILE* out, StateMachine* machine) {
    fprintf(out, "\nsection .data\n");
    fprintf(out, ".sm_%s_transitions:\n", machine->name);
    fprintf(out, "    ; Transition table (from, to, event)\n");
    fprintf(out, "    ; TODO: Populate from transition list\n");
    fprintf(out, "section .text\n");
}

// State enter callback
void codegen_state_enter(FILE* out, State* state) {
    fprintf(out, ".state_%s_enter:\n", state->name);
    
    if (state->on_enter) {
        fprintf(out, "    ; on_enter callback\n");
        fprintf(out, "    call .state_%s_on_enter\n", state->name);
    } else {
        fprintf(out, "    ; No on_enter callback\n");
    }
    
    fprintf(out, "    ret\n");
}

// State exit callback
void codegen_state_exit(FILE* out, State* state) {
    fprintf(out, ".state_%s_exit:\n", state->name);
    
    if (state->on_exit) {
        fprintf(out, "    ; on_exit callback\n");
        fprintf(out, "    call .state_%s_on_exit\n", state->name);
    } else {
        fprintf(out, "    ; No on_exit callback\n");
    }
    
    fprintf(out, "    ret\n");
}

// Event handler
void codegen_event_handler(FILE* out, StateMachine* machine) {
    fprintf(out, "\n.sm_%s_handle_event:\n", machine->name);
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
    
    // Event name rdi'de
    fprintf(out, "    ; rdi = event name\n");
    
    // Current state'i al
    fprintf(out, "    mov rax, [rel .sm_%s_current]\n", machine->name);
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .event_no_state\n");
    
    // Transition table'da ara
    fprintf(out, "    ; Search transition table\n");
    fprintf(out, "    ; TODO: Implement transition lookup\n");
    
    fprintf(out, ".event_no_state:\n");
    fprintf(out, "    xor rax, rax    ; No transition\n");
    
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n");
}

// Event dispatch
void codegen_event_dispatch(FILE* out, Event* event) {
    fprintf(out, "    ; Dispatch event: %s\n", event->name);
    fprintf(out, "    lea rdi, [rel .event_%s]\n", event->name);
    fprintf(out, "    call .sm_handle_event\n");
}

// Event queue (basit FIFO)
void codegen_event_queue(FILE* out) {
    fprintf(out, "\nsection .bss\n");
    fprintf(out, ".event_queue: resq 256    ; 256 event capacity\n");
    fprintf(out, ".event_queue_head: resq 1\n");
    fprintf(out, ".event_queue_tail: resq 1\n");
    fprintf(out, "section .text\n");
    
    // Enqueue
    fprintf(out, "\n.event_enqueue:\n");
    fprintf(out, "    ; rdi = event pointer\n");
    fprintf(out, "    mov rax, [rel .event_queue_tail]\n");
    fprintf(out, "    mov [rel .event_queue + rax*8], rdi\n");
    fprintf(out, "    inc rax\n");
    fprintf(out, "    and rax, 255    ; Wrap around\n");
    fprintf(out, "    mov [rel .event_queue_tail], rax\n");
    fprintf(out, "    ret\n");
    
    // Dequeue
    fprintf(out, "\n.event_dequeue:\n");
    fprintf(out, "    mov rax, [rel .event_queue_head]\n");
    fprintf(out, "    mov rbx, [rel .event_queue_tail]\n");
    fprintf(out, "    cmp rax, rbx\n");
    fprintf(out, "    je .queue_empty\n");
    fprintf(out, "    mov rax, [rel .event_queue + rax*8]\n");
    fprintf(out, "    mov rbx, [rel .event_queue_head]\n");
    fprintf(out, "    inc rbx\n");
    fprintf(out, "    and rbx, 255\n");
    fprintf(out, "    mov [rel .event_queue_head], rbx\n");
    fprintf(out, "    ret\n");
    fprintf(out, ".queue_empty:\n");
    fprintf(out, "    xor rax, rax\n");
    fprintf(out, "    ret\n");
}
