#include "type_system_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Parse type annotation
Type* parse_type_annotation(Lexer* lexer) {
    // TODO: Implement type annotation parsing
    // Bu fonksiyon şunları yapmalı:
    // 1. Temel türleri tanımla (int, float, string, bool, vb.)
    // 2. Composite türleri parse et (array[T], *T, func(...) -> T)
    // 3. User-defined türleri tanımla (struct, enum)
    // 4. Generic tür parametrelerini handle et

    fprintf(stderr, "Type annotation parsing not yet implemented\n");
    (void)lexer;
    return NULL;
}

// Parse and infer type from expression
Type* parse_infer_type(Lexer* lexer, TypeContext* ctx) {
    if (!lexer || !ctx) {
        return type_create(TYPE_ERROR);
    }

    Token* token = lexer_next_token(lexer);
    if (!token) {
        return type_create(TYPE_UNKNOWN);
    }

    Type* inferred_type = NULL;

    switch (token->type) {
        case TOKEN_NUMBER: {
            // Infer numeric type from literal
            const char* value = token->value;
            if (strchr(value, '.')) {
                // Has decimal point -> float
                inferred_type = type_create(TYPE_FLOAT);
            } else {
                // Integer
                inferred_type = type_create(TYPE_INT);
            }
            break;
        }

        case TOKEN_STRING:
            // String literal
            inferred_type = type_create(TYPE_STRING);
            break;

        case TOKEN_BOOLEAN:
        case TOKEN_TRUE:
        case TOKEN_FALSE:
            // Boolean literal
            inferred_type = type_create(TYPE_BOOL);
            break;

        case TOKEN_IDENTIFIER: {
            // Look up variable in symbol table
            const char* name = token->value;
            
            // Check for boolean literals (fallback for older lexers)
            if (name && (strcmp(name, "true") == 0 || strcmp(name, "false") == 0)) {
                inferred_type = type_create(TYPE_BOOL);
                break;
            }

            // Look up in symbol table
            Symbol* symbol = symbol_table_lookup(ctx->current_scope, name);
            if (symbol && symbol->type) {
                // Found in symbol table, clone the type
                inferred_type = type_create(symbol->type->kind);
                inferred_type->name = symbol->type->name ? strdup(symbol->type->name) : NULL;
                inferred_type->is_mutable = symbol->type->is_mutable;
                inferred_type->is_nullable = symbol->type->is_nullable;
            } else {
                // Unknown identifier - return unknown type
                inferred_type = type_create(TYPE_UNKNOWN);
            }
            break;
        }

        case TOKEN_LBRACKET: {
            // Array literal: [expr, expr, ...]
            Type* element_type = NULL;
            int element_count = 0;

            // Parse first element to infer array type
            Token* next = lexer_next_token(lexer);
            if (next && next->type != TOKEN_RBRACKET) {
                // Put token back by creating a simple lookahead
                // Infer from first element
                if (next->type == TOKEN_NUMBER) {
                    element_type = strchr(next->value, '.') ? 
                        type_create(TYPE_FLOAT) : type_create(TYPE_INT);
                    element_count = 1;
                } else if (next->type == TOKEN_STRING) {
                    element_type = type_create(TYPE_STRING);
                    element_count = 1;
                } else if (next->type == TOKEN_IDENTIFIER) {
                    Symbol* sym = symbol_table_lookup(ctx->current_scope, next->value);
                    element_type = sym && sym->type ? 
                        type_create(sym->type->kind) : type_create(TYPE_UNKNOWN);
                    element_count = 1;
                }
                token_free(next);

                // Skip remaining elements
                while (1) {
                    next = lexer_next_token(lexer);
                    if (!next || next->type == TOKEN_RBRACKET) {
                        break;
                    }
                    if (next->type == TOKEN_COMMA) {
                        element_count++;
                    }
                    token_free(next);
                }
            } else {
                // Empty array
                if (next) token_free(next);
            }

            inferred_type = type_create(TYPE_ARRAY);
            inferred_type->element_type = element_type ? element_type : type_create(TYPE_UNKNOWN);
            inferred_type->array_size = element_count;
            break;
        }

        case TOKEN_LPAREN: {
            // Could be:
            // 1. Parenthesized expression: (expr)
            // 2. Tuple: (expr, expr)
            // For now, infer as parenthesized expression
            inferred_type = parse_infer_type(lexer, ctx);
            
            Token* next = lexer_next_token(lexer);
            if (!next || next->type != TOKEN_RPAREN) {
                fprintf(stderr, "Error: Expected ')' after expression\n");
            }
            break;
        }

        default:
            // Unknown token - cannot infer
            inferred_type = type_create(TYPE_UNKNOWN);
            break;
    }

    token_free(token);
    return inferred_type ? inferred_type : type_create(TYPE_UNKNOWN);
}
