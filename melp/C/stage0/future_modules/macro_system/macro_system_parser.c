/**
 * MLP Macro System Module - Parser
 * 
 * Parses macro definitions and invocations
 */

#include "macro_system.h"
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

static void read_identifier(char* buffer, size_t max_len) {
    size_t i = 0;
    while (i < max_len - 1 && (isalnum(parse_source[parse_pos]) || 
           parse_source[parse_pos] == '_')) {
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

static void read_block(char* buffer, size_t max_len) {
    // Read until 'end'
    size_t i = 0;
    while (i < max_len - 1) {
        if (match_keyword("end")) {
            break;
        }
        buffer[i++] = parse_source[parse_pos++];
        if (parse_source[parse_pos] == '\n') {
            current_line++;
        }
    }
    buffer[i] = '\0';
}

// ============================================================================
// PARSER FUNCTIONS
// ============================================================================

void macro_parser_init(const char* source) {
    parse_source = source;
    parse_pos = 0;
    current_line = 1;
}

// Parse: macro_rules! name { pattern => expansion }
bool macro_parse_declarative(MacroSystemCtx* ctx, const char* line) {
    skip_whitespace();
    
    if (!match_keyword("macro_rules")) {
        return false;
    }
    consume_keyword("macro_rules");
    
    // Expect '!'
    if (parse_source[parse_pos] != '!') {
        return false;
    }
    parse_pos++;
    skip_whitespace();
    
    // Read macro name
    char name[128] = {0};
    read_identifier(name, sizeof(name));
    
    // Expect '{'
    if (parse_source[parse_pos] != '{') {
        return false;
    }
    parse_pos++;
    skip_whitespace();
    
    // Read pattern (simplified: until '=>')
    char pattern[256] = {0};
    read_until(pattern, sizeof(pattern), '=');
    
    // Expect '=>'
    if (parse_source[parse_pos] == '=' && parse_source[parse_pos + 1] == '>') {
        parse_pos += 2;
        skip_whitespace();
    }
    
    // Read expansion (until '}')
    char expansion[512] = {0};
    read_until(expansion, sizeof(expansion), '}');
    
    // Define macro
    return macro_define_declarative(ctx, name, pattern, expansion, current_line);
}

// Parse: #[derive(TraitName)]
bool macro_parse_derive(MacroSystemCtx* ctx, const char* line) {
    skip_whitespace();
    
    if (parse_source[parse_pos] != '#') {
        return false;
    }
    parse_pos++;
    
    if (parse_source[parse_pos] != '[') {
        return false;
    }
    parse_pos++;
    skip_whitespace();
    
    if (!match_keyword("derive")) {
        return false;
    }
    consume_keyword("derive");
    
    // Expect '('
    if (parse_source[parse_pos] != '(') {
        return false;
    }
    parse_pos++;
    skip_whitespace();
    
    // Read trait names (comma-separated)
    char traits[256] = {0};
    read_until(traits, sizeof(traits), ')');
    
    // Skip to next line to find struct name
    skip_line();
    skip_whitespace();
    
    if (!match_keyword("struct")) {
        return false;
    }
    consume_keyword("struct");
    
    char struct_name[128] = {0};
    read_identifier(struct_name, sizeof(struct_name));
    
    // Parse trait list and create derives
    char* trait = strtok(traits, ",");
    while (trait) {
        // Trim whitespace
        while (*trait == ' ') trait++;
        char* end = trait + strlen(trait) - 1;
        while (end > trait && *end == ' ') *end-- = '\0';
        
        macro_define_derive(ctx, trait, struct_name, current_line);
        trait = strtok(NULL, ",");
    }
    
    return true;
}

// Parse: #[attribute_name(params)]
bool macro_parse_attribute(MacroSystemCtx* ctx, const char* line) {
    skip_whitespace();
    
    if (parse_source[parse_pos] != '#') {
        return false;
    }
    parse_pos++;
    
    if (parse_source[parse_pos] != '[') {
        return false;
    }
    parse_pos++;
    skip_whitespace();
    
    // Read attribute name
    char attr_name[64] = {0};
    read_identifier(attr_name, sizeof(attr_name));
    
    // Optional parameters
    char parameters[256] = {0};
    if (parse_source[parse_pos] == '(') {
        parse_pos++;
        read_until(parameters, sizeof(parameters), ')');
        parse_pos++; // skip ')'
    }
    
    // Expect ']'
    if (parse_source[parse_pos] != ']') {
        return false;
    }
    parse_pos++;
    skip_whitespace();
    
    // Read target (next line)
    skip_line();
    skip_whitespace();
    
    char target_type[32] = "function";
    char target_name[128] = {0};
    
    if (match_keyword("func")) {
        consume_keyword("func");
        strcpy(target_type, "function");
    } else if (match_keyword("struct")) {
        consume_keyword("struct");
        strcpy(target_type, "struct");
    }
    
    read_identifier(target_name, sizeof(target_name));
    
    return macro_define_attribute(ctx, attr_name, target_name, target_type,
                                   parameters, current_line);
}

// Parse: macro_name!(arguments)
bool macro_parse_invocation(MacroSystemCtx* ctx, const char* line) {
    skip_whitespace();
    
    char name[128] = {0};
    read_identifier(name, sizeof(name));
    
    // Expect '!'
    if (parse_source[parse_pos] != '!') {
        return false;
    }
    parse_pos++;
    
    // Expect '(' or '['
    char open_delim = parse_source[parse_pos];
    if (open_delim != '(' && open_delim != '[' && open_delim != '{') {
        return false;
    }
    parse_pos++;
    
    // Determine closing delimiter
    char close_delim = (open_delim == '(') ? ')' : 
                       (open_delim == '[') ? ']' : '}';
    
    // Read arguments
    char arguments[512] = {0};
    read_until(arguments, sizeof(arguments), close_delim);
    
    // Create invocation string
    char invocation[640];
    snprintf(invocation, sizeof(invocation), "%s!(%s)", name, arguments);
    
    return macro_invoke(ctx, invocation, current_line);
}

// Main parse function
bool macro_parse_file(MacroSystemCtx* ctx) {
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
        
        // Save position for backtracking
        size_t saved_pos = parse_pos;
        int saved_line = current_line;
        
        // Try to parse macro constructs
        
        // Try declarative macro
        if (macro_parse_declarative(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Restore position
        parse_pos = saved_pos;
        current_line = saved_line;
        
        // Try derive
        if (macro_parse_derive(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Restore position
        parse_pos = saved_pos;
        current_line = saved_line;
        
        // Try attribute
        if (macro_parse_attribute(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Restore position
        parse_pos = saved_pos;
        current_line = saved_line;
        
        // Try macro invocation
        if (macro_parse_invocation(ctx, &parse_source[parse_pos])) {
            skip_line();
            continue;
        }
        
        // Unknown construct, skip line
        skip_line();
    }
    
    return true;
}
