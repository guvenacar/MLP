#include "smart_pointers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// ============================================================================
// MLP SMART POINTER PARSER - MODULE #51
// ============================================================================
// Parses MLP syntax:
//   dim ptr as Rc<Node>
//   dim arc_data as Arc<Data>
//   dim boxed as Box<i32>
//   dim weak_ref as Weak<Node>
//
// Operations:
//   call rc_clone(ptr)
//   call rc_drop(ptr)
//   call rc_downgrade(ptr)
//   call weak_upgrade(weak_ref)

// ============================================================================
// PARSER STATE
// ============================================================================

typedef struct {
    const char* input;
    size_t pos;
    size_t length;
    char current_char;
    int line;
    int column;
} Parser;

// ============================================================================
// PARSER UTILITIES
// ============================================================================

static void parser_init(Parser* p, const char* input) {
    p->input = input;
    p->pos = 0;
    p->length = strlen(input);
    p->current_char = input[0];
    p->line = 1;
    p->column = 1;
}

static void parser_advance(Parser* p) {
    if (p->pos >= p->length) {
        p->current_char = '\0';
        return;
    }
    
    if (p->current_char == '\n') {
        p->line++;
        p->column = 1;
    } else {
        p->column++;
    }
    
    p->pos++;
    p->current_char = p->input[p->pos];
}

static void parser_skip_whitespace(Parser* p) {
    while (p->current_char == ' ' || p->current_char == '\t' || 
           p->current_char == '\r') {
        parser_advance(p);
    }
}

static void parser_skip_line(Parser* p) {
    while (p->current_char != '\n' && p->current_char != '\0') {
        parser_advance(p);
    }
    if (p->current_char == '\n') {
        parser_advance(p);
    }
}

static bool parser_match(Parser* p, const char* str) {
    size_t len = strlen(str);
    if (p->pos + len > p->length) return false;
    return strncmp(&p->input[p->pos], str, len) == 0;
}

static bool parser_match_keyword(Parser* p, const char* keyword) {
    size_t len = strlen(keyword);
    if (p->pos + len > p->length) return false;
    
    // Check keyword matches
    if (strncmp(&p->input[p->pos], keyword, len) != 0) return false;
    
    // Check next char is not alphanumeric (word boundary)
    if (p->pos + len < p->length) {
        char next = p->input[p->pos + len];
        if (isalnum(next) || next == '_') return false;
    }
    
    return true;
}

static char* parser_read_identifier(Parser* p) {
    parser_skip_whitespace(p);
    
    size_t start = p->pos;
    while (isalnum(p->current_char) || p->current_char == '_') {
        parser_advance(p);
    }
    
    size_t len = p->pos - start;
    if (len == 0) return NULL;
    
    char* id = (char*)malloc(len + 1);
    strncpy(id, &p->input[start], len);
    id[len] = '\0';
    
    return id;
}

static SmartPointerKind parser_detect_smart_pointer_kind(const char* type_str) {
    if (strncmp(type_str, "Rc<", 3) == 0) return SP_RC;
    if (strncmp(type_str, "Arc<", 4) == 0) return SP_ARC;
    if (strncmp(type_str, "Box<", 4) == 0) return SP_BOX;
    if (strncmp(type_str, "Weak<", 5) == 0) return SP_WEAK;
    return SP_RC; // Default
}

static char* parser_extract_inner_type(const char* type_str) {
    // Extract T from "Rc<T>", "Arc<T>", "Box<T>", "Weak<T>"
    const char* start = strchr(type_str, '<');
    if (!start) return NULL;
    start++; // Skip '<'
    
    const char* end = strchr(start, '>');
    if (!end) return NULL;
    
    size_t len = end - start;
    char* inner = (char*)malloc(len + 1);
    strncpy(inner, start, len);
    inner[len] = '\0';
    
    return inner;
}

// ============================================================================
// SMART POINTER DECLARATION PARSING
// ============================================================================

typedef struct {
    SmartPointerKind kind;
    char* var_name;
    char* inner_type;
    char* init_expr;  // Optional initialization
} SmartPointerDecl;

static SmartPointerDecl* smart_pointer_parse_declaration(Parser* p) {
    parser_skip_whitespace(p);
    
    // Check for "dim" keyword (MLP syntax)
    if (!parser_match_keyword(p, "dim")) {
        return NULL;
    }
    
    // Skip "dim"
    for (int i = 0; i < 3; i++) parser_advance(p);
    parser_skip_whitespace(p);
    
    // Read variable name
    char* var_name = parser_read_identifier(p);
    if (!var_name) {
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Check for "as" keyword
    if (!parser_match_keyword(p, "as")) {
        free(var_name);
        return NULL;
    }
    
    // Skip "as"
    for (int i = 0; i < 2; i++) parser_advance(p);
    parser_skip_whitespace(p);
    
    // Read smart pointer type: Rc<T>, Arc<T>, Box<T>, Weak<T>
    size_t type_start = p->pos;
    
    // Find end of type (either '=' or newline)
    while (p->current_char != '=' && p->current_char != '\n' && p->current_char != '\0') {
        parser_advance(p);
    }
    
    size_t type_len = p->pos - type_start;
    char* type_str = (char*)malloc(type_len + 1);
    strncpy(type_str, &p->input[type_start], type_len);
    type_str[type_len] = '\0';
    
    // Trim trailing whitespace
    while (type_len > 0 && isspace(type_str[type_len - 1])) {
        type_str[--type_len] = '\0';
    }
    
    // Detect smart pointer kind
    SmartPointerKind kind = parser_detect_smart_pointer_kind(type_str);
    
    // Extract inner type
    char* inner_type = parser_extract_inner_type(type_str);
    free(type_str);
    
    if (!inner_type) {
        free(var_name);
        return NULL;
    }
    
    // Create declaration
    SmartPointerDecl* decl = (SmartPointerDecl*)malloc(sizeof(SmartPointerDecl));
    decl->kind = kind;
    decl->var_name = var_name;
    decl->inner_type = inner_type;
    decl->init_expr = NULL;
    
    // Check for initialization
    parser_skip_whitespace(p);
    if (p->current_char == '=') {
        parser_advance(p);  // Skip '='
        parser_skip_whitespace(p);
        
        // Read until newline
        size_t expr_start = p->pos;
        while (p->current_char != '\n' && p->current_char != '\0') {
            parser_advance(p);
        }
        
        size_t expr_len = p->pos - expr_start;
        if (expr_len > 0) {
            decl->init_expr = (char*)malloc(expr_len + 1);
            strncpy(decl->init_expr, &p->input[expr_start], expr_len);
            decl->init_expr[expr_len] = '\0';
            
            // Trim trailing whitespace
            while (expr_len > 0 && isspace(decl->init_expr[expr_len - 1])) {
                decl->init_expr[--expr_len] = '\0';
            }
        }
    }
    
    return decl;
}

// ============================================================================
// SMART POINTER OPERATION PARSING
// ============================================================================

typedef enum {
    SP_OP_PARSE_RC_NEW,
    SP_OP_PARSE_RC_CLONE,
    SP_OP_PARSE_RC_DROP,
    SP_OP_PARSE_RC_DOWNGRADE,
    SP_OP_PARSE_ARC_NEW,
    SP_OP_PARSE_ARC_CLONE,
    SP_OP_PARSE_ARC_DROP,
    SP_OP_PARSE_ARC_DOWNGRADE,
    SP_OP_PARSE_BOX_NEW,
    SP_OP_PARSE_BOX_DROP,
    SP_OP_PARSE_WEAK_UPGRADE,
    SP_OP_PARSE_WEAK_DROP,
    SP_OP_PARSE_UNKNOWN
} SmartPointerOpKind;

typedef struct {
    SmartPointerOpKind op_kind;
    char* target_var;
    char* arg;  // Optional argument
} SmartPointerOp;

static SmartPointerOp* smart_pointer_parse_operation(Parser* p) {
    parser_skip_whitespace(p);
    
    // Check for "call" keyword
    if (!parser_match_keyword(p, "call")) {
        return NULL;
    }
    
    // Skip "call"
    for (int i = 0; i < 4; i++) parser_advance(p);
    parser_skip_whitespace(p);
    
    // Read function name
    char* func_name = parser_read_identifier(p);
    if (!func_name) {
        return NULL;
    }
    
    SmartPointerOpKind op_kind = SP_OP_PARSE_UNKNOWN;
    
    // Detect operation kind
    if (strcmp(func_name, "rc_new") == 0) op_kind = SP_OP_PARSE_RC_NEW;
    else if (strcmp(func_name, "rc_clone") == 0) op_kind = SP_OP_PARSE_RC_CLONE;
    else if (strcmp(func_name, "rc_drop") == 0) op_kind = SP_OP_PARSE_RC_DROP;
    else if (strcmp(func_name, "rc_downgrade") == 0) op_kind = SP_OP_PARSE_RC_DOWNGRADE;
    else if (strcmp(func_name, "arc_new") == 0) op_kind = SP_OP_PARSE_ARC_NEW;
    else if (strcmp(func_name, "arc_clone") == 0) op_kind = SP_OP_PARSE_ARC_CLONE;
    else if (strcmp(func_name, "arc_drop") == 0) op_kind = SP_OP_PARSE_ARC_DROP;
    else if (strcmp(func_name, "arc_downgrade") == 0) op_kind = SP_OP_PARSE_ARC_DOWNGRADE;
    else if (strcmp(func_name, "box_new") == 0) op_kind = SP_OP_PARSE_BOX_NEW;
    else if (strcmp(func_name, "box_drop") == 0) op_kind = SP_OP_PARSE_BOX_DROP;
    else if (strcmp(func_name, "weak_upgrade") == 0) op_kind = SP_OP_PARSE_WEAK_UPGRADE;
    else if (strcmp(func_name, "weak_drop") == 0) op_kind = SP_OP_PARSE_WEAK_DROP;
    
    free(func_name);
    
    if (op_kind == SP_OP_PARSE_UNKNOWN) {
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Expect '('
    if (p->current_char != '(') {
        return NULL;
    }
    parser_advance(p);
    parser_skip_whitespace(p);
    
    // Read argument
    char* arg = parser_read_identifier(p);
    if (!arg) {
        return NULL;
    }
    
    parser_skip_whitespace(p);
    
    // Expect ')'
    if (p->current_char != ')') {
        free(arg);
        return NULL;
    }
    parser_advance(p);
    
    // Create operation
    SmartPointerOp* op = (SmartPointerOp*)malloc(sizeof(SmartPointerOp));
    op->op_kind = op_kind;
    op->target_var = NULL;  // Set later if needed
    op->arg = arg;
    
    return op;
}

// ============================================================================
// MAIN PARSE FUNCTION
// ============================================================================

typedef struct {
    SmartPointerDecl** declarations;
    size_t decl_count;
    SmartPointerOp** operations;
    size_t op_count;
} SmartPointerParseResult;

SmartPointerParseResult* smart_pointer_parse(const char* input) {
    Parser parser;
    parser_init(&parser, input);
    
    SmartPointerParseResult* result = (SmartPointerParseResult*)malloc(sizeof(SmartPointerParseResult));
    result->declarations = (SmartPointerDecl**)malloc(sizeof(SmartPointerDecl*) * 16);
    result->decl_count = 0;
    result->operations = (SmartPointerOp**)malloc(sizeof(SmartPointerOp*) * 16);
    result->op_count = 0;
    
    while (parser.current_char != '\0') {
        parser_skip_whitespace(&parser);
        
        if (parser.current_char == '\0') break;
        
        // Skip comments
        if (parser.current_char == '-' && parser.pos + 1 < parser.length && 
            parser.input[parser.pos + 1] == '-') {
            parser_skip_line(&parser);
            continue;
        }
        
        // Skip empty lines
        if (parser.current_char == '\n') {
            parser_advance(&parser);
            continue;
        }
        
        // Try parse declaration
        SmartPointerDecl* decl = smart_pointer_parse_declaration(&parser);
        if (decl) {
            result->declarations[result->decl_count++] = decl;
            continue;
        }
        
        // Try parse operation
        SmartPointerOp* op = smart_pointer_parse_operation(&parser);
        if (op) {
            result->operations[result->op_count++] = op;
            continue;
        }
        
        // Unknown line, skip it
        parser_skip_line(&parser);
    }
    
    return result;
}

void smart_pointer_parse_result_free(SmartPointerParseResult* result) {
    if (!result) return;
    
    for (size_t i = 0; i < result->decl_count; i++) {
        SmartPointerDecl* decl = result->declarations[i];
        free(decl->var_name);
        free(decl->inner_type);
        if (decl->init_expr) free(decl->init_expr);
        free(decl);
    }
    free(result->declarations);
    
    for (size_t i = 0; i < result->op_count; i++) {
        SmartPointerOp* op = result->operations[i];
        if (op->target_var) free(op->target_var);
        if (op->arg) free(op->arg);
        free(op);
    }
    free(result->operations);
    
    free(result);
}
