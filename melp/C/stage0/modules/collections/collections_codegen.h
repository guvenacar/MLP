#ifndef COLLECTIONS_CODEGEN_H
#define COLLECTIONS_CODEGEN_H

#include "collections.h"
#include "../../codegen.h"

// Codegen functions for collections
void codegen_tuple_literal(CodeGen* gen, TupleLiteral* tuple);
void codegen_list_literal(CodeGen* gen, ListLiteral* list);
void codegen_tuple_access(CodeGen* gen, TupleAccess* access);
void codegen_list_access(CodeGen* gen, ListAccess* access);

// Helper functions (to be implemented by expression codegen)
void codegen_generate_expression(CodeGen* gen, void* expr);
void codegen_emit(CodeGen* gen, const char* line);

#endif
