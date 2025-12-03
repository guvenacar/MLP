#include "struct_parser.h"
#include "../arithmetic/arithmetic_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Parse struct definition
// Syntax: struct Person { name: string, age: numeric }
StructDef* parse_struct_definition(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Expect 'struct' keyword (would be a new token type)
    // For now, assume we're at struct keyword
    
    // Parse struct name
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected struct name\n");
        token_free(token);
        return NULL;
    }
    
    StructDef* def = struct_create_definition(token->value);
    token_free(token);
    
    // Expect '{'  (would need LBRACE token)
    token = lexer_next_token(lexer);
    // Placeholder: skip brace check for now
    token_free(token);
    
    // Parse members until '}'
    while (1) {
        token = lexer_next_token(lexer);
        
        // Check for closing brace (would be RBRACE token)
        if (token->type == TOKEN_EOF) {
            token_free(token);
            break;
        }
        
        // Parse member: name: type
        if (token->type != TOKEN_IDENTIFIER) {
            token_free(token);
            break;
        }
        
        char* member_name = strdup(token->value);
        token_free(token);
        
        // Expect ':'
        token = lexer_next_token(lexer);
        // Skip colon check
        token_free(token);
        
        // Parse type
        token = lexer_next_token(lexer);
        if (token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected type for member %s\n", member_name);
            free(member_name);
            token_free(token);
            break;
        }
        
        struct_add_member(def, member_name, token->value);
        free(member_name);
        token_free(token);
        
        // Check for comma (next member) or closing brace
        token = lexer_next_token(lexer);
        if (token->type == TOKEN_COMMA) {
            token_free(token);
            continue;
        } else {
            token_free(token);
            break;
        }
    }
    
    struct_calculate_layout(def);
    return def;
}

// Parse struct instance
// Syntax: Person p = Person { name: "Ali", age: 25 }
StructInstance* parse_struct_instance(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Parse struct type name
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected struct type name\n");
        token_free(token);
        return NULL;
    }
    
    char* struct_type = strdup(token->value);
    token_free(token);
    
    // Parse instance variable name
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected instance variable name\n");
        free(struct_type);
        token_free(token);
        return NULL;
    }
    
    char* instance_name = strdup(token->value);
    token_free(token);
    
    // Expect '='
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_ASSIGN) {
        fprintf(stderr, "Error: Expected '=' for struct initialization\n");
        free(struct_type);
        free(instance_name);
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse struct initializer: struct_type { ... }
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected struct type in initializer\n");
        free(struct_type);
        free(instance_name);
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Expect '{' and parse field initializers
    // Simplified: just skip to '}'
    while (1) {
        token = lexer_next_token(lexer);
        if (token->type == TOKEN_EOF) {
            token_free(token);
            break;
        }
        token_free(token);
        // In real implementation, parse field: value pairs
        break;  // Placeholder
    }
    
    // Create instance (requires finding definition - placeholder)
    StructInstance* inst = malloc(sizeof(StructInstance));
    inst->struct_name = struct_type;
    inst->instance_name = instance_name;
    inst->definition = NULL;  // Would lookup from symbol table
    
    return inst;
}

// Parse member access
// Syntax: p.name or p.age
MemberAccess* parse_member_access(Lexer* lexer) {
    Token* token = lexer_next_token(lexer);
    
    // Parse instance name
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected struct instance name\n");
        token_free(token);
        return NULL;
    }
    
    char* instance = strdup(token->value);
    token_free(token);
    
    // Expect '.'
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_DOT) {
        fprintf(stderr, "Error: Expected '.' for member access\n");
        free(instance);
        token_free(token);
        return NULL;
    }
    token_free(token);
    
    // Parse member name
    token = lexer_next_token(lexer);
    if (token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected member name\n");
        free(instance);
        token_free(token);
        return NULL;
    }
    
    MemberAccess* access = struct_create_member_access(instance, token->value);
    free(instance);
    token_free(token);
    
    return access;
}
