#include "parser.h"
#include "statement_parser.h"
#include "modules/control_flow/control_flow.h"
#include "modules/control_flow/control_flow_parser.h"
#include "modules/variable/variable_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Statement parser - dispatches to appropriate module parsers

Statement* parse_statement(Parser* parser) {
    if (!parser || !parser->current_token) {
        return NULL;
    }
    
    Token* current = parser->current_token;
    
    // Check for control flow statements
    if (current->type == TOKEN_IF) {
        // Create ControlFlowParser from Parser
        ControlFlowParser* cf_parser = cf_parser_create(parser->lexer);
        cf_parser->current_token = parser->current_token;
        parser->current_token = NULL; // Transfer ownership
        
        Statement* stmt = parse_if_statement(cf_parser);
        
        // Transfer token back to parser
        parser->current_token = cf_parser->current_token;
        cf_parser->current_token = NULL;
        cf_parser_free(cf_parser);
        
        return stmt;
    }
    
    if (current->type == TOKEN_WHILE) {
        ControlFlowParser* cf_parser = cf_parser_create(parser->lexer);
        cf_parser->current_token = parser->current_token;
        parser->current_token = NULL;
        
        Statement* stmt = parse_while_statement(cf_parser);
        
        parser->current_token = cf_parser->current_token;
        cf_parser->current_token = NULL;
        cf_parser_free(cf_parser);
        
        return stmt;
    }
    
    if (current->type == TOKEN_FOR) {
        ControlFlowParser* cf_parser = cf_parser_create(parser->lexer);
        cf_parser->current_token = parser->current_token;
        parser->current_token = NULL;
        
        Statement* stmt = parse_for_statement(cf_parser);
        
        parser->current_token = cf_parser->current_token;
        cf_parser->current_token = NULL;
        cf_parser_free(cf_parser);
        
        return stmt;
    }
    
    // If not a control flow statement, skip for now
    // In the future, handle assignments, function calls, etc.
    fprintf(stderr, "Warning: Unhandled statement type\n");
    token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
    
    return NULL;
}

// Note: statement_free is defined in control_flow_parser.c
