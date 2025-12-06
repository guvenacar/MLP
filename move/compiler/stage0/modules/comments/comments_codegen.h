#ifndef COMMENTS_CODEGEN_H
#define COMMENTS_CODEGEN_H

#include <stdio.h>

// Comments generate no code - they are stripped during lexing
// This file exists for consistency with modular architecture

// Generate assembly comments (optional, for debugging)
void codegen_comment(FILE* f, const char* comment_text);

#endif // COMMENTS_CODEGEN_H
