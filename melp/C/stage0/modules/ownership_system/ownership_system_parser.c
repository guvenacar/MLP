/**
 * MLP Ownership System Module - Parser
 * 
 * Parses ownership syntax, borrow checker rules, and lifetime annotations
 */

#include "ownership_system.h"
#include <ctype.h>

// ============================================================================
// PARSER STATE
// ============================================================================

static const char* parse_source = NULL;
static size_t parse_pos = 0;
static int current_line = 1;

// ============================================================================
// PARSER UTILITIES
// ============================================================================

static void skip_whitespace() {
    while (parse_source[parse_pos] == ' ' || 
           parse_source[parse_pos] == '\t' || 
           parse_source[parse_pos] == '\r') {
        parse_pos++;
    }
}

static void skip_line() {
    while (parse_source[parse_pos] != '\n' && parse_source[parse_pos] != '\0') {
        parse_pos++;
    }
    if (parse_source[parse_pos] == '\n') {
        parse_pos++;
        current_line++;
    }
}

static bool match_keyword(const char* keyword) {
    size_t len = strlen(keyword);
    if (strncmp(&parse_source[parse_pos], keyword, len) == 0) {
        char next = parse_source[parse_pos + len];
        if (!isalnum(next) && next != '_') {
            return true;
        }
    }
    return false;
}

static void consume_keyword(const char* keyword) {
    parse_pos += strlen(keyword);
    skip_whitespace();
}

static bool is_identifier_char(char c) {
    return isalnum(c) || c == '_';
}

static void read_identifier(char* buffer, size_t max_len) {
    size_t i = 0;
    while (i < max_len - 1 && is_identifier_char(parse_source[parse_pos])) {
        buffer[i++] = parse_source[parse_pos++];
    }
    buffer[i] = '\0';
    skip_whitespace();
}

static void read_until(char* buffer, size_t max_len, char terminator) {
    size_t i = 0;
    while (i < max_len - 1 && parse_source[parse_pos] != terminator && 
           parse_source[parse_pos] != '\0') {
        buffer[i++] = parse_source[parse_pos++];
    }
    buffer[i] = '\0';
}

// ============================================================================
// OWNERSHIP PARSER FUNCTIONS
// ============================================================================

void ownership_parser_init(const char* source) {
    parse_source = source;
    parse_pos = 0;
    current_line = 1;
}

// Parse: numeric x = 42
// Parse: numeric& y = borrow x
// Parse: numeric&mut z = borrow_mut x
bool ownership_parse_declaration(OwnershipCtx* ctx, char* var_name, char* type_name, 
                                  OwnershipType* ownership) {
    skip_whitespace();
    
    // Read type
    read_identifier(type_name, 128);
    if (strlen(type_name) == 0) return false;
    
    // Check for reference markers
    *ownership = OWNER_OWNED;
    
    if (parse_source[parse_pos] == '&') {
        parse_pos++;
        if (strncmp(&parse_source[parse_pos], "mut", 3) == 0 &&
            !is_identifier_char(parse_source[parse_pos + 3])) {
            *ownership = OWNER_MUT_BORROW;
            parse_pos += 3;
        } else {
            *ownership = OWNER_BORROWED;
        }
        skip_whitespace();
    }
    
    // Read variable name
    read_identifier(var_name, 128);
    if (strlen(var_name) == 0) return false;
    
    return true;
}

// Parse: borrow x
// Parse: borrow_mut x
bool ownership_parse_borrow(OwnershipCtx* ctx, const char* line) {
    skip_whitespace();
    
    bool is_mutable = false;
    char borrowed_as[128] = {0};
    char var_name[128] = {0};
    char lifetime_name[64] = {0};
    
    // Check for "borrow" or "borrow_mut"
    if (match_keyword("borrow_mut")) {
        is_mutable = true;
        consume_keyword("borrow_mut");
    } else if (match_keyword("borrow")) {
        consume_keyword("borrow");
    } else {
        return false;
    }
    
    // Read variable to borrow
    read_identifier(var_name, sizeof(var_name));
    
    // Optional: as new_name
    if (match_keyword("as")) {
        consume_keyword("as");
        read_identifier(borrowed_as, sizeof(borrowed_as));
    } else {
        // Default: borrowed_var = "ref_" + var_name
        snprintf(borrowed_as, sizeof(borrowed_as), "ref_%s", var_name);
    }
    
    // Optional: with lifetime 'a
    if (match_keyword("with")) {
        consume_keyword("with");
        if (match_keyword("lifetime")) {
            consume_keyword("lifetime");
            if (parse_source[parse_pos] == '\'') {
                parse_pos++;
                read_identifier(lifetime_name, sizeof(lifetime_name));
            }
        }
    }
    
    // Create borrow
    return ownership_create_borrow(ctx, var_name, borrowed_as, is_mutable, 
                                   strlen(lifetime_name) > 0 ? lifetime_name : "elided",
                                   current_line);
}

// Parse: move x to y
// Parse: copy x to y
// Parse: clone x to y
bool ownership_parse_move(OwnershipCtx* ctx, const char* line) {
    skip_whitespace();
    
    MoveSemantics semantics = MOVE_MOVE;
    char from_var[128] = {0};
    char to_var[128] = {0};
    
    // Determine semantics
    if (match_keyword("move")) {
        semantics = MOVE_MOVE;
        consume_keyword("move");
    } else if (match_keyword("copy")) {
        semantics = MOVE_COPY;
        consume_keyword("copy");
    } else if (match_keyword("clone")) {
        semantics = MOVE_CLONE;
        consume_keyword("clone");
    } else {
        return false;
    }
    
    // Read source variable
    read_identifier(from_var, sizeof(from_var));
    
    // Expect "to"
    if (!match_keyword("to")) {
        return false;
    }
    consume_keyword("to");
    
    // Read destination variable
    read_identifier(to_var, sizeof(to_var));
    
    // Execute move
    return ownership_execute_move(ctx, from_var, to_var, semantics, current_line);
}

// Parse: lifetime 'a from line X to Y
// Parse: lifetime 'static
bool ownership_parse_lifetime(OwnershipCtx* ctx, const char* line) {
    skip_whitespace();
    
    if (!match_keyword("lifetime")) {
        return false;
    }
    consume_keyword("lifetime");
    
    char lifetime_name[64] = {0};
    LifetimeType type = LIFETIME_NAMED;
    int start_line = current_line;
    
    // Read lifetime name ('a, 'b, 'static)
    if (parse_source[parse_pos] == '\'') {
        parse_pos++;
        read_identifier(lifetime_name, sizeof(lifetime_name));
        
        // Check for 'static
        if (strcmp(lifetime_name, "static") == 0) {
            type = LIFETIME_STATIC;
        }
    } else {
        return false;
    }
    
    // Optional: from line X to Y
    if (match_keyword("from")) {
        consume_keyword("from");
        if (match_keyword("line")) {
            consume_keyword("line");
            // Parse line number (simple)
            char num_buf[16];
            read_identifier(num_buf, sizeof(num_buf));
            start_line = atoi(num_buf);
        }
    }
    
    // Create lifetime
    Lifetime* lt = ownership_create_lifetime(ctx, lifetime_name, type, start_line);
    return lt != NULL;
}

// Parse: drop x
bool ownership_parse_drop(OwnershipCtx* ctx, const char* line) {
    skip_whitespace();
    
    if (!match_keyword("drop")) {
        return false;
    }
    consume_keyword("drop");
    
    char var_name[128] = {0};
    read_identifier(var_name, sizeof(var_name));
    
    // Find variable and mark as dropped/moved
    OwnershipInfo* info = ownership_find_var(ctx, var_name);
    if (info) {
        info->is_moved = true;
        info->last_use_line = current_line;
        return true;
    }
    
    return false;
}

// Main parse function
bool ownership_parse_file(OwnershipCtx* ctx) {
    ctx->current_line = 1;
    
    while (parse_source[parse_pos] != '\0') {
        skip_whitespace();
        
        // Skip empty lines
        if (parse_source[parse_pos] == '\n') {
            parse_pos++;
            current_line++;
            continue;
        }
        
        // Skip comments
        if (parse_source[parse_pos] == '-' && parse_source[parse_pos + 1] == '-') {
            skip_line();
            continue;
        }
        
        // Try to parse ownership constructs
        char var_name[128];
        char type_name[128];
        OwnershipType ownership;
        
        // Save position for backtracking
        size_t saved_pos = parse_pos;
        int saved_line = current_line;
        
        // Try declaration
        if (ownership_parse_declaration(ctx, var_name, type_name, &ownership)) {
            if (ownership == OWNER_OWNED) {
                ownership_add_owner(ctx, var_name, type_name, current_line);
            }
            skip_line();
            continue;
        }
        
        // Restore position
        parse_pos = saved_pos;
        current_line = saved_line;
        
        // Try borrow
        if (ownership_parse_borrow(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Restore position
        parse_pos = saved_pos;
        current_line = saved_line;
        
        // Try move/copy/clone
        if (ownership_parse_move(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Restore position
        parse_pos = saved_pos;
        current_line = saved_line;
        
        // Try lifetime
        if (ownership_parse_lifetime(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Restore position
        parse_pos = saved_pos;
        current_line = saved_line;
        
        // Try drop
        if (ownership_parse_drop(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Unknown construct, skip line
        skip_line();
    }
    
    // Run borrow checker validation
    ownership_check_dangling(ctx);
    
    return ctx->violation_count == 0;
}
