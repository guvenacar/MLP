#ifndef SWITCH_MATCH_CODEGEN_H
#define SWITCH_MATCH_CODEGEN_H

#include "switch_match.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_switch(FILE* out, Switch* sw);
void codegen_switch_match(FILE* out, SwitchMatch* match);
void codegen_case(FILE* out, Case* case_item, int case_num);
void codegen_range_check(FILE* out, Range* range);

// Yardımcı fonksiyonlar
void codegen_jump_table(FILE* out, Switch* sw);
void codegen_linear_search(FILE* out, Switch* sw);
int should_use_jump_table(Switch* sw);

#endif // SWITCH_MATCH_CODEGEN_H
