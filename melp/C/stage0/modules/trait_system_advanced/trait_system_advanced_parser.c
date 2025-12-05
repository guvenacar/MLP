#include "trait_system_advanced.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// Forward declaration for strdup
extern char* strdup(const char*);

// ============================================================================
// PARSER UTILITIES
// ============================================================================

static bool is_identifier_char(char c) {
    return isalnum(c) || c == '_';
}

static char* extract_identifier(const char* str, size_t* pos) {
    size_t start = *pos;
    while (str[*pos] && is_identifier_char(str[*pos])) {
        (*pos)++;
    }
    
    size_t len = *pos - start;
    char* id = (char*)malloc(len + 1);
    memcpy(id, str + start, len);
    id[len] = '\0';
    
    return id;
}

static void skip_whitespace(const char* str, size_t* pos) {
    while (str[*pos] && isspace(str[*pos])) {
        (*pos)++;
    }
}

static bool match_keyword(const char* str, size_t* pos, const char* keyword) {
    size_t kw_len = strlen(keyword);
    if (strncmp(str + *pos, keyword, kw_len) == 0) {
        if (!is_identifier_char(str[*pos + kw_len])) {
            *pos += kw_len;
            return true;
        }
    }
    return false;
}

bool is_trait_keyword(const char* str) {
    return (strncmp(str, "trait", 5) == 0 && !is_identifier_char(str[5]));
}

// ============================================================================
// TRAIT LIST PARSER
// ============================================================================

char** parse_trait_list(const char* str, size_t* count) {
    // Parse "Display + Clone + Debug"
    if (!str || !count) return NULL;
    
    *count = 0;
    char** traits = (char**)malloc(sizeof(char*) * 8);
    
    const char* start = str;
    const char* end = str;
    
    while (*end) {
        // Find next '+' or end
        if (*end == '+' || *end == '\0') {
            // Extract trait name
            size_t len = end - start;
            char* trait = (char*)malloc(len + 1);
            memcpy(trait, start, len);
            trait[len] = '\0';
            
            // Trim whitespace
            char* trimmed = trait;
            while (*trimmed && isspace(*trimmed)) trimmed++;
            
            char* trait_end = trimmed + strlen(trimmed) - 1;
            while (trait_end > trimmed && isspace(*trait_end)) *trait_end-- = '\0';
            
            traits[(*count)++] = strdup(trimmed);
            free(trait);
            
            if (*end == '\0') break;
            end++;
            start = end;
        } else {
            end++;
        }
    }
    
    return traits;
}

// ============================================================================
// TRAIT BOUND PARSER
// ============================================================================

TraitBound* trait_parse_bound(const char* bound_str) {
    // Parse: "T: Display + Clone" or "T: 'static" or "T: ?Sized"
    if (!bound_str) return NULL;
    
    size_t pos = 0;
    skip_whitespace(bound_str, &pos);
    
    // Extract type parameter
    char* type_param = extract_identifier(bound_str, &pos);
    if (!type_param) return NULL;
    
    skip_whitespace(bound_str, &pos);
    
    // Expect ':'
    if (bound_str[pos] != ':') {
        free(type_param);
        return NULL;
    }
    pos++;
    
    skip_whitespace(bound_str, &pos);
    
    TraitBound* bound = trait_bound_create(type_param);
    free(type_param);
    
    // Check for optional marker '?'
    if (bound_str[pos] == '?') {
        pos++;
        bound->is_optional = true;
    }
    
    // Check for lifetime
    if (bound_str[pos] == '\'') {
        pos++;
        char* lifetime = extract_identifier(bound_str, &pos);
        trait_bound_set_lifetime(bound, lifetime);
        free(lifetime);
        return bound;
    }
    
    // Parse trait list
    size_t trait_count;
    char** traits = parse_trait_list(bound_str + pos, &trait_count);
    
    for (size_t i = 0; i < trait_count; i++) {
        trait_bound_add_trait(bound, traits[i]);
        free(traits[i]);
    }
    free(traits);
    
    return bound;
}

// ============================================================================
// ASSOCIATED TYPE PARSER
// ============================================================================

AssociatedType* trait_parse_assoc_type(const char* line) {
    // Parse: "type Item = i32;" or "type Output;"
    
    size_t pos = 0;
    skip_whitespace(line, &pos);
    
    if (!match_keyword(line, &pos, "type")) {
        return NULL;
    }
    
    skip_whitespace(line, &pos);
    
    // Extract type name
    char* name = extract_identifier(line, &pos);
    if (!name) return NULL;
    
    AssociatedType* assoc = assoc_type_create(name);
    free(name);
    
    skip_whitespace(line, &pos);
    
    // Check for '='
    if (line[pos] == '=') {
        pos++;
        skip_whitespace(line, &pos);
        
        // Extract concrete type
        char* concrete = extract_identifier(line, &pos);
        if (concrete) {
            assoc_type_set_concrete(assoc, concrete);
            free(concrete);
        }
    }
    
    return assoc;
}

// ============================================================================
// TRAIT DEFINITION PARSER
// ============================================================================

TraitDef* trait_parse_definition(const char** lines, size_t line_count) {
    // Parse:
    // trait Display {
    //     fn display(&self);
    // }
    
    if (line_count == 0) return NULL;
    
    size_t pos = 0;
    skip_whitespace(lines[0], &pos);
    
    if (!match_keyword(lines[0], &pos, "trait")) {
        return NULL;
    }
    
    skip_whitespace(lines[0], &pos);
    
    // Extract trait name
    char* trait_name = extract_identifier(lines[0], &pos);
    if (!trait_name) return NULL;
    
    TraitDef* trait = trait_def_create(trait_name);
    free(trait_name);
    
    skip_whitespace(lines[0], &pos);
    
    // Check for super traits (: Parent)
    if (lines[0][pos] == ':') {
        pos++;
        skip_whitespace(lines[0], &pos);
        
        // Parse super trait list
        size_t super_count;
        char** super_traits = parse_trait_list(lines[0] + pos, &super_count);
        
        for (size_t i = 0; i < super_count; i++) {
            trait_def_add_super_trait(trait, super_traits[i]);
            free(super_traits[i]);
        }
        free(super_traits);
    }
    
    // Parse body (lines 1 to line_count-1)
    for (size_t i = 1; i < line_count; i++) {
        const char* line = lines[i];
        pos = 0;
        skip_whitespace(line, &pos);
        
        // Skip closing brace
        if (line[pos] == '}') break;
        
        // Check for associated type
        if (match_keyword(line, &pos, "type")) {
            AssociatedType* assoc = trait_parse_assoc_type(line);
            if (assoc) {
                trait_def_add_assoc_type(trait, assoc);
            }
        }
        // Check for method (fn keyword)
        else if (match_keyword(line, &pos, "fn")) {
            trait_def_add_method(trait, line);
        }
    }
    
    return trait;
}

// ============================================================================
// TRAIT IMPLEMENTATION PARSER
// ============================================================================

TraitImpl* trait_parse_implementation(const char** lines, size_t line_count) {
    // Parse:
    // impl Display for Point {
    //     type Item = i32;
    //     fn display(&self) { ... }
    // }
    
    if (line_count == 0) return NULL;
    
    size_t pos = 0;
    skip_whitespace(lines[0], &pos);
    
    if (!match_keyword(lines[0], &pos, "impl")) {
        return NULL;
    }
    
    skip_whitespace(lines[0], &pos);
    
    // Extract trait name
    char* trait_name = extract_identifier(lines[0], &pos);
    if (!trait_name) return NULL;
    
    skip_whitespace(lines[0], &pos);
    
    // Expect 'for'
    if (!match_keyword(lines[0], &pos, "for")) {
        free(trait_name);
        return NULL;
    }
    
    skip_whitespace(lines[0], &pos);
    
    // Extract type name
    char* type_name = extract_identifier(lines[0], &pos);
    if (!type_name) {
        free(trait_name);
        return NULL;
    }
    
    TraitImpl* impl = trait_impl_create(trait_name, type_name);
    free(trait_name);
    free(type_name);
    
    // Parse body
    for (size_t i = 1; i < line_count; i++) {
        const char* line = lines[i];
        pos = 0;
        skip_whitespace(line, &pos);
        
        if (line[pos] == '}') break;
        
        // Check for associated type
        if (match_keyword(line, &pos, "type")) {
            AssociatedType* assoc = trait_parse_assoc_type(line);
            if (assoc) {
                trait_impl_add_assoc_type(impl, assoc);
            }
        }
        // Check for method
        else if (match_keyword(line, &pos, "fn")) {
            trait_impl_add_method(impl, line);
        }
    }
    
    return impl;
}

// ============================================================================
// TRAIT STATEMENT PARSER
// ============================================================================

TraitStmt* trait_parse_statement(const char* line) {
    if (!line) return NULL;
    
    size_t pos = 0;
    skip_whitespace(line, &pos);
    
    TraitStmt* stmt = (TraitStmt*)malloc(sizeof(TraitStmt));
    
    // Check for trait definition
    if (match_keyword(line, &pos, "trait")) {
        stmt->stmt_type = TRAIT_DECL;
        const char* lines[] = {line};
        stmt->data.trait_def = trait_parse_definition(lines, 1);
        return stmt;
    }
    
    // Check for impl
    if (match_keyword(line, &pos, "impl")) {
        stmt->stmt_type = TRAIT_IMPL;
        const char* lines[] = {line};
        stmt->data.trait_impl = trait_parse_implementation(lines, 1);
        return stmt;
    }
    
    // Check for type (associated type)
    if (match_keyword(line, &pos, "type")) {
        stmt->stmt_type = ASSOC_TYPE_DECL;
        stmt->data.assoc_type = trait_parse_assoc_type(line);
        return stmt;
    }
    
    // Check for where clause bound
    if (match_keyword(line, &pos, "where")) {
        stmt->stmt_type = TRAIT_BOUND;
        skip_whitespace(line, &pos);
        stmt->data.trait_bound = trait_parse_bound(line + pos);
        return stmt;
    }
    
    free(stmt);
    return NULL;
}

void trait_stmt_free(TraitStmt* stmt) {
    if (!stmt) return;
    
    switch (stmt->stmt_type) {
        case TRAIT_DECL:
            trait_def_free(stmt->data.trait_def);
            break;
        case TRAIT_IMPL:
            trait_impl_free(stmt->data.trait_impl);
            break;
        case TRAIT_BOUND:
            trait_bound_free(stmt->data.trait_bound);
            break;
        case ASSOC_TYPE_DECL:
            assoc_type_free(stmt->data.assoc_type);
            break;
    }
    
    free(stmt);
}
