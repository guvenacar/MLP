#include "channels.h"
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
        // Check that next char is not identifier char
        if (!is_identifier_char(str[*pos + kw_len])) {
            *pos += kw_len;
            return true;
        }
    }
    return false;
}

// ============================================================================
// CHANNEL DECLARATION PARSER
// ============================================================================

ChannelDecl* channel_parse_declaration(const char* line) {
    // Parse: dim ch as Channel<i32>
    // Parse: dim ch as Channel<string, 10>  (buffered)
    
    size_t pos = 0;
    skip_whitespace(line, &pos);
    
    // Match "dim"
    if (!match_keyword(line, &pos, "dim")) {
        return NULL;
    }
    skip_whitespace(line, &pos);
    
    // Extract variable name
    char* var_name = extract_identifier(line, &pos);
    if (!var_name) return NULL;
    
    skip_whitespace(line, &pos);
    
    // Match "as"
    if (!match_keyword(line, &pos, "as")) {
        free(var_name);
        return NULL;
    }
    skip_whitespace(line, &pos);
    
    // Match "Channel"
    if (!match_keyword(line, &pos, "Channel")) {
        free(var_name);
        return NULL;
    }
    
    // Match "<"
    skip_whitespace(line, &pos);
    if (line[pos] != '<') {
        free(var_name);
        return NULL;
    }
    pos++;
    
    // Extract type parameter
    skip_whitespace(line, &pos);
    size_t type_start = pos;
    int bracket_depth = 0;
    
    while (line[pos]) {
        if (line[pos] == '<') bracket_depth++;
        if (line[pos] == '>') {
            if (bracket_depth == 0) break;
            bracket_depth--;
        }
        if (line[pos] == ',' && bracket_depth == 0) break;
        pos++;
    }
    
    size_t type_len = pos - type_start;
    char* element_type = (char*)malloc(type_len + 1);
    memcpy(element_type, line + type_start, type_len);
    element_type[type_len] = '\0';
    
    // Trim trailing whitespace
    while (type_len > 0 && isspace(element_type[type_len - 1])) {
        element_type[--type_len] = '\0';
    }
    
    // Check for capacity (buffered channel)
    size_t capacity = 0;
    skip_whitespace(line, &pos);
    
    if (line[pos] == ',') {
        pos++;
        skip_whitespace(line, &pos);
        
        // Parse capacity number
        char* endptr;
        capacity = strtoul(line + pos, &endptr, 10);
        pos = endptr - line;
        
        skip_whitespace(line, &pos);
    }
    
    // Match ">"
    if (line[pos] != '>') {
        free(var_name);
        free(element_type);
        return NULL;
    }
    
    // Create declaration
    ChannelDecl* decl = (ChannelDecl*)malloc(sizeof(ChannelDecl));
    decl->var_name = var_name;
    decl->element_type = element_type;
    decl->capacity = capacity;
    decl->is_buffered = (capacity > 0);
    
    return decl;
}

void channel_decl_free(ChannelDecl* decl) {
    if (decl) {
        free(decl->var_name);
        free(decl->element_type);
        free(decl);
    }
}

// ============================================================================
// CHANNEL OPERATION PARSER
// ============================================================================

ChannelOp* channel_parse_operation(const char* line) {
    // Parse various channel operations:
    // send(ch, value)
    // value = receive(ch)
    // close(ch)
    // len = channel_len(ch)
    // cap = channel_cap(ch)
    
    size_t pos = 0;
    skip_whitespace(line, &pos);
    
    // Try to match operation
    if (match_keyword(line, &pos, "send")) {
        // Parse: send(ch, value)
        skip_whitespace(line, &pos);
        if (line[pos] != '(') return NULL;
        pos++;
        
        skip_whitespace(line, &pos);
        char* ch_name = extract_identifier(line, &pos);
        
        skip_whitespace(line, &pos);
        if (line[pos] != ',') {
            free(ch_name);
            return NULL;
        }
        pos++;
        
        skip_whitespace(line, &pos);
        size_t value_start = pos;
        
        // Find closing paren
        int paren_depth = 1;
        while (line[pos] && paren_depth > 0) {
            if (line[pos] == '(') paren_depth++;
            if (line[pos] == ')') paren_depth--;
            if (paren_depth > 0) pos++;
        }
        
        size_t value_len = pos - value_start;
        char* value_expr = (char*)malloc(value_len + 1);
        memcpy(value_expr, line + value_start, value_len);
        value_expr[value_len] = '\0';
        
        ChannelOp* op = (ChannelOp*)malloc(sizeof(ChannelOp));
        op->op_type = CHAN_OP_SEND;
        op->channel_name = ch_name;
        op->value_expr = value_expr;
        op->result_var = NULL;
        op->is_blocking = true;
        
        return op;
        
    } else if (match_keyword(line, &pos, "try_send")) {
        // Parse: ok = try_send(ch, value)
        // First check if there's assignment
        char* result_var = NULL;
        
        // Look for '=' before '('
        size_t check_pos = pos;
        while (line[check_pos] && line[check_pos] != '(') {
            if (line[check_pos] == '=') {
                // Extract result variable
                size_t var_end = check_pos;
                while (var_end > pos && isspace(line[var_end - 1])) var_end--;
                
                size_t var_len = var_end - pos;
                result_var = (char*)malloc(var_len + 1);
                memcpy(result_var, line + pos, var_len);
                result_var[var_len] = '\0';
                
                pos = check_pos + 1;
                skip_whitespace(line, &pos);
                
                // Re-match try_send
                if (!match_keyword(line, &pos, "try_send")) {
                    free(result_var);
                    return NULL;
                }
                break;
            }
            check_pos++;
        }
        
        skip_whitespace(line, &pos);
        if (line[pos] != '(') {
            if (result_var) free(result_var);
            return NULL;
        }
        pos++;
        
        skip_whitespace(line, &pos);
        char* ch_name = extract_identifier(line, &pos);
        
        skip_whitespace(line, &pos);
        if (line[pos] != ',') {
            free(ch_name);
            if (result_var) free(result_var);
            return NULL;
        }
        pos++;
        
        skip_whitespace(line, &pos);
        size_t value_start = pos;
        
        int paren_depth = 1;
        while (line[pos] && paren_depth > 0) {
            if (line[pos] == '(') paren_depth++;
            if (line[pos] == ')') paren_depth--;
            if (paren_depth > 0) pos++;
        }
        
        size_t value_len = pos - value_start;
        char* value_expr = (char*)malloc(value_len + 1);
        memcpy(value_expr, line + value_start, value_len);
        value_expr[value_len] = '\0';
        
        ChannelOp* op = (ChannelOp*)malloc(sizeof(ChannelOp));
        op->op_type = CHAN_OP_SEND;
        op->channel_name = ch_name;
        op->value_expr = value_expr;
        op->result_var = result_var;
        op->is_blocking = false;
        
        return op;
        
    } else if (match_keyword(line, &pos, "receive") || 
               match_keyword(line, &pos, "try_receive")) {
        
        bool is_blocking = (strncmp(line + (pos - 7), "receive", 7) == 0);
        
        // Parse: value = receive(ch)
        // First, look backwards for assignment
        size_t assign_pos = 0;
        for (size_t i = 0; i < pos; i++) {
            if (line[i] == '=') {
                assign_pos = i;
                break;
            }
        }
        
        char* result_var = NULL;
        if (assign_pos > 0) {
            size_t var_end = assign_pos;
            while (var_end > 0 && isspace(line[var_end - 1])) var_end--;
            
            size_t var_start = 0;
            for (size_t i = var_end; i > 0; i--) {
                if (!is_identifier_char(line[i - 1])) {
                    var_start = i;
                    break;
                }
            }
            
            size_t var_len = var_end - var_start;
            result_var = (char*)malloc(var_len + 1);
            memcpy(result_var, line + var_start, var_len);
            result_var[var_len] = '\0';
        }
        
        skip_whitespace(line, &pos);
        if (line[pos] != '(') {
            if (result_var) free(result_var);
            return NULL;
        }
        pos++;
        
        skip_whitespace(line, &pos);
        char* ch_name = extract_identifier(line, &pos);
        
        skip_whitespace(line, &pos);
        if (line[pos] != ')') {
            free(ch_name);
            if (result_var) free(result_var);
            return NULL;
        }
        
        ChannelOp* op = (ChannelOp*)malloc(sizeof(ChannelOp));
        op->op_type = CHAN_OP_RECEIVE;
        op->channel_name = ch_name;
        op->value_expr = NULL;
        op->result_var = result_var;
        op->is_blocking = is_blocking;
        
        return op;
        
    } else if (match_keyword(line, &pos, "close")) {
        // Parse: close(ch)
        skip_whitespace(line, &pos);
        if (line[pos] != '(') return NULL;
        pos++;
        
        skip_whitespace(line, &pos);
        char* ch_name = extract_identifier(line, &pos);
        
        skip_whitespace(line, &pos);
        if (line[pos] != ')') {
            free(ch_name);
            return NULL;
        }
        
        ChannelOp* op = (ChannelOp*)malloc(sizeof(ChannelOp));
        op->op_type = CHAN_OP_CLOSE;
        op->channel_name = ch_name;
        op->value_expr = NULL;
        op->result_var = NULL;
        op->is_blocking = false;
        
        return op;
    }
    
    return NULL;
}

void channel_op_free(ChannelOp* op) {
    if (op) {
        free(op->channel_name);
        if (op->value_expr) free(op->value_expr);
        if (op->result_var) free(op->result_var);
        free(op);
    }
}

// ============================================================================
// SELECT STATEMENT PARSER
// ============================================================================

SelectStmt* channel_parse_select(const char** lines, size_t line_count) {
    // Parse select statement:
    // select
    //   case send(ch1, val):
    //     ...
    //   case val = receive(ch2):
    //     ...
    //   default:
    //     ...
    // end select
    
    if (line_count == 0) return NULL;
    
    size_t pos = 0;
    skip_whitespace(lines[0], &pos);
    
    if (!match_keyword(lines[0], &pos, "select")) {
        return NULL;
    }
    
    SelectStmt* stmt = (SelectStmt*)malloc(sizeof(SelectStmt));
    stmt->cases = (SelectCase**)malloc(sizeof(SelectCase*) * 16);
    stmt->case_count = 0;
    stmt->has_default = false;
    
    size_t line_idx = 1;
    int case_index = 0;
    
    while (line_idx < line_count) {
        const char* line = lines[line_idx];
        pos = 0;
        skip_whitespace(line, &pos);
        
        if (match_keyword(line, &pos, "end")) {
            skip_whitespace(line, &pos);
            if (match_keyword(line, &pos, "select")) {
                break;
            }
        }
        
        if (match_keyword(line, &pos, "case")) {
            skip_whitespace(line, &pos);
            
            // Check for default
            if (match_keyword(line, &pos, "default")) {
                SelectCase* sc = select_case_default(case_index++);
                stmt->cases[stmt->case_count++] = sc;
                stmt->has_default = true;
            } else {
                // Parse send or receive operation
                ChannelOp* op = channel_parse_operation(line + pos);
                
                if (op && op->op_type == CHAN_OP_SEND) {
                    SelectCase* sc = select_case_send(NULL, NULL, case_index++);
                    sc->channel_name = strdup(op->channel_name);
                    sc->value_expr = op->value_expr ? strdup(op->value_expr) : NULL;
                    stmt->cases[stmt->case_count++] = sc;
                    
                    channel_op_free(op);
                    
                } else if (op && op->op_type == CHAN_OP_RECEIVE) {
                    SelectCase* sc = select_case_receive(NULL, case_index++);
                    sc->channel_name = strdup(op->channel_name);
                    sc->result_var = op->result_var ? strdup(op->result_var) : NULL;
                    stmt->cases[stmt->case_count++] = sc;
                    
                    channel_op_free(op);
                }
            }
        }
        
        line_idx++;
    }
    
    return stmt;
}

void select_stmt_free(SelectStmt* stmt) {
    if (stmt) {
        for (size_t i = 0; i < stmt->case_count; i++) {
            SelectCase* sc = stmt->cases[i];
            if (sc) {
                if (sc->channel_name) free(sc->channel_name);
                if (sc->value_expr) free(sc->value_expr);
                if (sc->result_var) free(sc->result_var);
                free(sc);
            }
        }
        free(stmt->cases);
        free(stmt);
    }
}

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

bool channel_is_channel_type(const char* type_str) {
    return (strncmp(type_str, "Channel<", 8) == 0);
}

char* channel_extract_element_type(const char* channel_type) {
    // Extract "i32" from "Channel<i32>" or "Channel<i32, 10>"
    
    const char* start = strchr(channel_type, '<');
    if (!start) return NULL;
    start++;
    
    while (*start && isspace(*start)) start++;
    
    const char* end = start;
    int bracket_depth = 0;
    
    while (*end) {
        if (*end == '<') bracket_depth++;
        if (*end == '>') {
            if (bracket_depth == 0) break;
            bracket_depth--;
        }
        if (*end == ',' && bracket_depth == 0) break;
        end++;
    }
    
    size_t len = end - start;
    while (len > 0 && isspace(start[len - 1])) len--;
    
    char* element_type = (char*)malloc(len + 1);
    memcpy(element_type, start, len);
    element_type[len] = '\0';
    
    return element_type;
}
