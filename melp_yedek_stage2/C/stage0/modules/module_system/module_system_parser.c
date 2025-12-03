#include "module_system_parser.h"
#include "../../parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Helper functions
static void advance(Parser* parser) {
    if (parser->current_token) {
        token_free(parser->current_token);
    }
    parser->current_token = lexer_next_token(parser->lexer);
}

static int match(Parser* parser, TokenType type) {
    if (parser->current_token && parser->current_token->type == type) {
        advance(parser);
        return 1;
    }
    return 0;
}

static int expect(Parser* parser, TokenType type, const char* message) {
    if (parser->current_token && parser->current_token->type == type) {
        advance(parser);
        return 1;
    }
    fprintf(stderr, "Parse error: %s\n", message);
    return 0;
}

// Forward declarations
Import* parse_import_statement(Parser* parser);
Export* parse_export_statement(Parser* parser);

// Parse module definition: module ModuleName { ... }
Module* parse_module_definition(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "module") != 0) {
        return NULL;
    }
    advance(parser);  // consume "module"
    
    // Parse module name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected module name after 'module'\n");
        return NULL;
    }
    
    char* mod_name = strdup(parser->current_token->value);
    advance(parser);
    
    Module* mod = module_create(mod_name, "");
    free(mod_name);
    
    // Expect '{'
    if (!expect(parser, TOKEN_LBRACKET, "Expected '{' after module name")) {
        module_free(mod);
        return NULL;
    }
    
    // Parse module body until '}'
    while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
        // Skip declarations inside module (handled by other parsers)
        if (parser->current_token->type == TOKEN_IDENTIFIER) {
            const char* keyword = parser->current_token->value;
            
            if (strcmp(keyword, "import") == 0) {
                Import* imp = parse_import_statement(parser);
                if (imp) {
                    import_free(imp);  // For now, just parse and discard
                }
            } else if (strcmp(keyword, "export") == 0) {
                Export* exp = parse_export_statement(parser);
                if (exp) {
                    module_add_export(mod, exp);
                }
            } else {
                // Skip other declarations
                advance(parser);
            }
        } else {
            advance(parser);
        }
    }
    
    if (!expect(parser, TOKEN_RBRACKET, "Expected '}' at end of module")) {
        module_free(mod);
        return NULL;
    }
    
    return mod;
}

// Parse import statement: import { symbol1, symbol2 } from "module"
// Or: import * as alias from "module"
Import* parse_import_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "import") != 0) {
        return NULL;
    }
    advance(parser);  // consume "import"
    
    Import* imp = NULL;
    
    // Check for wildcard import: import * as alias from "module"
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "*") == 0) {
        advance(parser);
        
        // Expect "as"
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
            strcmp(parser->current_token->value, "as") != 0) {
            fprintf(stderr, "Error: Expected 'as' after '*'\n");
            return NULL;
        }
        advance(parser);
        
        // Parse alias name
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected alias name after 'as'\n");
            return NULL;
        }
        
        char* alias = strdup(parser->current_token->value);
        advance(parser);
        
        // Expect "from"
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
            strcmp(parser->current_token->value, "from") != 0) {
            fprintf(stderr, "Error: Expected 'from' after alias\n");
            free(alias);
            return NULL;
        }
        advance(parser);
        
        // Parse module path (string literal or identifier)
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected module path after 'from'\n");
            free(alias);
            return NULL;
        }
        
        imp = import_create(parser->current_token->value);
        imp->is_wildcard = 1;
        import_set_alias(imp, alias);
        free(alias);
        advance(parser);
        
    } else if (parser->current_token && parser->current_token->type == TOKEN_LBRACKET) {
        // Named imports: import { sym1, sym2 } from "module"
        advance(parser);  // consume '{'
        
        // Parse symbols
        char* symbols[64];
        int count = 0;
        
        while (parser->current_token && parser->current_token->type != TOKEN_RBRACKET) {
            if (parser->current_token->type == TOKEN_IDENTIFIER) {
                symbols[count++] = strdup(parser->current_token->value);
                advance(parser);
                
                // Check for comma
                if (parser->current_token && parser->current_token->type == TOKEN_COMMA) {
                    advance(parser);
                }
            } else {
                advance(parser);
            }
        }
        
        if (!expect(parser, TOKEN_RBRACKET, "Expected '}' after import list")) {
            for (int i = 0; i < count; i++) free(symbols[i]);
            return NULL;
        }
        
        // Expect "from"
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
            strcmp(parser->current_token->value, "from") != 0) {
            fprintf(stderr, "Error: Expected 'from' after import list\n");
            for (int i = 0; i < count; i++) free(symbols[i]);
            return NULL;
        }
        advance(parser);
        
        // Parse module path
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected module path after 'from'\n");
            for (int i = 0; i < count; i++) free(symbols[i]);
            return NULL;
        }
        
        imp = import_create(parser->current_token->value);
        for (int i = 0; i < count; i++) {
            import_add_symbol(imp, symbols[i]);
            free(symbols[i]);
        }
        advance(parser);
        
    } else {
        // Simple import: import "module"
        if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected module path after 'import'\n");
            return NULL;
        }
        
        imp = import_create(parser->current_token->value);
        advance(parser);
    }
    
    return imp;
}

// Parse export statement: export func name() or export default value
Export* parse_export_statement(Parser* parser) {
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER ||
        strcmp(parser->current_token->value, "export") != 0) {
        return NULL;
    }
    advance(parser);  // consume "export"
    
    int is_default = 0;
    
    // Check for "default"
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER &&
        strcmp(parser->current_token->value, "default") == 0) {
        is_default = 1;
        advance(parser);
    }
    
    // Parse exported symbol name
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected symbol name after 'export'\n");
        return NULL;
    }
    
    char* symbol = strdup(parser->current_token->value);
    advance(parser);
    
    Export* exp = export_create(symbol, NULL);
    exp->is_default = is_default;
    free(symbol);
    
    return exp;
}

// Parse symbol declaration with visibility: public func test()
Symbol* parse_symbol_declaration_with_visibility(Parser* parser) {
    Visibility vis = VIS_PRIVATE;  // Default visibility
    
    // Check for visibility modifiers
    if (parser->current_token && parser->current_token->type == TOKEN_IDENTIFIER) {
        const char* keyword = parser->current_token->value;
        
        if (strcmp(keyword, "public") == 0) {
            vis = VIS_PUBLIC;
            advance(parser);
        } else if (strcmp(keyword, "private") == 0) {
            vis = VIS_PRIVATE;
            advance(parser);
        } else if (strcmp(keyword, "internal") == 0) {
            vis = VIS_INTERNAL;
            advance(parser);
        }
    }
    
    // Parse symbol name (simplified - just parse identifier)
    if (!parser->current_token || parser->current_token->type != TOKEN_IDENTIFIER) {
        return NULL;
    }
    
    char* name = strdup(parser->current_token->value);
    advance(parser);
    
    Symbol* sym = symbol_create(name, vis);
    free(name);
    
    return sym;
}

// Legacy compatibility wrappers
Module* parse_module(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_module_definition(&parser);
}

Import* parse_import(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_import_statement(&parser);
}

Export* parse_export(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_export_statement(&parser);
}

Symbol* parse_symbol_declaration(Lexer* lexer) {
    Parser parser;
    parser.lexer = lexer;
    parser.current_token = lexer_next_token(lexer);
    return parse_symbol_declaration_with_visibility(&parser);
}
