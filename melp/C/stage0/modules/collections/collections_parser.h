#ifndef COLLECTIONS_PARSER_H
#define COLLECTIONS_PARSER_H

#include "collections.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser functions for collections
TupleLiteral* parse_collections_tuple_literal(Parser* parser);
ListLiteral* parse_collections_list_literal(Parser* parser);
TupleAccess* parse_tuple_access(Parser* parser, void* tuple_expr);
ListAccess* parse_list_access(Parser* parser, void* list_expr);

// Context checking
int is_tuple_literal_context(Parser* parser);
int is_list_literal_context(Parser* parser);

// Helper functions (to be implemented by expression parser)
void* parse_expression(Parser* parser);
void advance(Parser* parser);

#endif
