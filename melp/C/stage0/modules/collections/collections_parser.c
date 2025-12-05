#include "collections_parser.h"
#include "../expression/expression_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Parse tuple literal: <expr1, expr2, expr3>
// Heterogeneous, immutable, stack-allocated
TupleLiteral* parse_collections_tuple_literal(Parser* parser) {
    // Expect '<'
    if (parser->current_token->type != TOKEN_LESS) {
        fprintf(stderr, "Error: Expected '<' to start tuple literal\n");
        return NULL;
    }
    parser_advance(parser);  // consume '<'
    
    // Check for empty tuple: <>
    if (parser->current_token->type == TOKEN_GREATER) {
        parser_advance(parser);  // consume '>'
        TupleLiteral* tuple = malloc(sizeof(TupleLiteral));
        tuple->count = 0;
        tuple->elements = NULL;
        tuple->element_types = NULL;
        return tuple;
    }
    
    // Parse elements
    int capacity = 4;
    int count = 0;
    void** elements = malloc(sizeof(void*) * capacity);
    VarType* types = malloc(sizeof(VarType) * capacity);
    
    while (1) {
        // Resize if needed
        if (count >= capacity) {
            capacity *= 2;
            elements = realloc(elements, sizeof(void*) * capacity);
            types = realloc(types, sizeof(VarType) * capacity);
        }
        
        // Parse expression (will be Expression*)
        // For now, store as void* - actual parsing depends on expression module
        elements[count] = parse_expression(parser);
        
        // Infer type from expression (simplified - needs proper type inference)
        types[count] = VAR_NUMERIC;  // TODO: Actual type inference
        count++;
        
        // Check for comma (more elements) or end
        if (parser->current_token->type == TOKEN_COMMA) {
            parser_advance(parser);  // consume ','
        } else if (parser->current_token->type == TOKEN_GREATER) {
            break;
        } else {
            fprintf(stderr, "Error: Expected ',' or '>' in tuple literal\n");
            free(elements);
            free(types);
            return NULL;
        }
    }
    
    // Expect '>'
    if (parser->current_token->type != TOKEN_GREATER) {
        fprintf(stderr, "Error: Expected '>' to end tuple literal\n");
        free(elements);
        free(types);
        return NULL;
    }
    parser_advance(parser);  // consume '>'
    
    // Create tuple literal
    TupleLiteral* tuple = malloc(sizeof(TupleLiteral));
    tuple->count = count;
    tuple->elements = elements;
    tuple->element_types = types;
    
    return tuple;
}

// Parse list literal: (expr1, expr2, expr3)
// Heterogeneous, mutable, heap-allocated
// NOTE: Disambiguation needed - ( can also mean function call or grouping
ListLiteral* parse_collections_list_literal(Parser* parser) {
    // Expect '('
    if (parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' to start list literal\n");
        return NULL;
    }
    parser_advance(parser);  // consume '('
    
    // Check for empty list: ()
    if (parser->current_token->type == TOKEN_RPAREN) {
        parser_advance(parser);  // consume ')'
        ListLiteral* list = malloc(sizeof(ListLiteral));
        list->count = 0;
        list->elements = NULL;
        list->element_types = NULL;
        return list;
    }
    
    // Parse elements
    int capacity = 4;
    int count = 0;
    void** elements = malloc(sizeof(void*) * capacity);
    VarType* types = malloc(sizeof(VarType) * capacity);
    
    while (1) {
        // Resize if needed
        if (count >= capacity) {
            capacity *= 2;
            elements = realloc(elements, sizeof(void*) * capacity);
            types = realloc(types, sizeof(VarType) * capacity);
        }
        
        // Parse expression
        elements[count] = parse_expression(parser);
        
        // Infer type (simplified)
        types[count] = VAR_NUMERIC;  // TODO: Actual type inference
        count++;
        
        // Check for comma (more elements) or end
        if (parser->current_token->type == TOKEN_COMMA) {
            parser_advance(parser);  // consume ','
        } else if (parser->current_token->type == TOKEN_RPAREN) {
            break;
        } else {
            fprintf(stderr, "Error: Expected ',' or ')' in list literal\n");
            free(elements);
            free(types);
            return NULL;
        }
    }
    
    // Expect ')'
    if (parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Error: Expected ')' to end list literal\n");
        free(elements);
        free(types);
        return NULL;
    }
    parser_advance(parser);  // consume ')'
    
    // Create list literal
    ListLiteral* list = malloc(sizeof(ListLiteral));
    list->count = count;
    list->elements = elements;
    list->element_types = types;
    
    return list;
}

// Parse tuple access: tuple<index>
TupleAccess* parse_tuple_access(Parser* parser, void* tuple_expr) {
    // Expect '<'
    if (parser->current_token->type != TOKEN_LESS) {
        fprintf(stderr, "Error: Expected '<' for tuple access\n");
        return NULL;
    }
    parser_advance(parser);  // consume '<'
    
    // Parse index (must be compile-time constant integer)
    if (parser->current_token->type != TOKEN_NUMBER) {
        fprintf(stderr, "Error: Tuple index must be compile-time constant\n");
        return NULL;
    }
    
    int index = atoi(parser->current_token->value);
    parser_advance(parser);  // consume number
    
    // Expect '>'
    if (parser->current_token->type != TOKEN_GREATER) {
        fprintf(stderr, "Error: Expected '>' after tuple index\n");
        return NULL;
    }
    parser_advance(parser);  // consume '>'
    
    // Create tuple access
    TupleAccess* access = malloc(sizeof(TupleAccess));
    access->tuple_expr = tuple_expr;
    access->index = index;
    
    return access;
}

// Parse list access: list(index)
ListAccess* parse_list_access(Parser* parser, void* list_expr) {
    // Expect '('
    if (parser->current_token->type != TOKEN_LPAREN) {
        fprintf(stderr, "Error: Expected '(' for list access\n");
        return NULL;
    }
    parser_advance(parser);  // consume '('
    
    // Parse index expression (can be runtime)
    void* index_expr = parse_expression(parser);
    
    // Expect ')'
    if (parser->current_token->type != TOKEN_RPAREN) {
        fprintf(stderr, "Error: Expected ')' after list index\n");
        return NULL;
    }
    parser_advance(parser);  // consume ')'
    
    // Create list access
    ListAccess* access = malloc(sizeof(ListAccess));
    access->list_expr = list_expr;
    access->index_expr = index_expr;
    
    return access;
}

// Check if next tokens form tuple literal (peek ahead)
int is_tuple_literal_context(Parser* parser) {
    // Heuristic: < followed by expression and comma or >
    // This helps disambiguate from comparison operator
    if (parser->current_token->type != TOKEN_LESS) {
        return 0;
    }
    
    // TODO: Implement proper lookahead
    // For now, assume < at start of statement/expression is tuple
    return 1;
}

// Check if next tokens form list literal (peek ahead)
int is_list_literal_context(Parser* parser) {
    // Heuristic: ( followed by expression and comma
    // This helps disambiguate from function call or grouping
    if (parser->current_token->type != TOKEN_LPAREN) {
        return 0;
    }
    
    // TODO: Implement proper lookahead
    // If ( followed by expr, comma -> likely list
    // If ( followed by expr, ) -> likely grouping
    return 0;  // Default to grouping for now
}
