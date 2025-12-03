// ============================================================================
// MELP Modular Codegen - codegen_emit.c
// Auto-extracted from codegen.c
// ============================================================================

#include "codegen.h"

// İçerik: codegen_emit, codegen_add_string
// Temel emit fonksiyonları

void codegen_emit(Codegen* gen, const char* line) {
    fprintf(gen->out, "%s\n", line);
}

int codegen_add_string(Codegen* gen, const char* value) {
    int id = gen->string_counter++;
    StringLiteral* str = malloc(sizeof(StringLiteral));
    str->id = id;
    str->value = malloc(strlen(value) + 1);
    strcpy(str->value, value);
    str->next = gen->strings;
    gen->strings = str;
    return id;
}

