#ifndef STATE_MANAGEMENT_PARSER_H
#define STATE_MANAGEMENT_PARSER_H

#include "state_management.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
StateMachine* parse_state_machine(Lexer* lexer);
State* parse_state(Lexer* lexer);
Transition* parse_transition(Lexer* lexer);
Event* parse_event(Lexer* lexer);

#endif // STATE_MANAGEMENT_PARSER_H
