/**
 * MLP Attributes Module - Parser #42 (Stage 1)
 * 
 * Lexer and parser implementation for attribute annotations
 */

#include "attributes.h"
#include <ctype.h>

// ============================================================================
// LEXER
// ============================================================================

typedef enum {
    TOK_EOF,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_STRING,
    TOK_AT,              // @
    TOK_LPAREN,          // (
    TOK_RPAREN,          // )
    TOK_COMMA,           // ,
    TOK_EQUAL,           // =
    TOK_FN,
    TOK_STRUCT,
    TOK_ENUM,
    TOK_LET
} TokType;

static const char* source_code = NULL;
static int pos = 0;
static char current_token[256];
static TokType current_tok_type;

static void skip_whitespace() {
    while (source_code[pos] && isspace(source_code[pos])) {
        pos++;
    }
}

static void skip_comment() {
    if (source_code[pos] == '/' && source_code[pos+1] == '/') {
        while (source_code[pos] && source_code[pos] != '\n') pos++;
    }
}

static int is_keyword(const char* word) {
    return strcmp(word, "fn") == 0 || strcmp(word, "struct") == 0 ||
           strcmp(word, "enum") == 0 || strcmp(word, "let") == 0;
}

static TokType get_keyword_type(const char* word) {
    if (strcmp(word, "fn") == 0) return TOK_FN;
    if (strcmp(word, "struct") == 0) return TOK_STRUCT;
    if (strcmp(word, "enum") == 0) return TOK_ENUM;
    if (strcmp(word, "let") == 0) return TOK_LET;
    return TOK_IDENT;
}

static void next_token() {
    skip_whitespace();
    skip_comment();
    skip_whitespace();

    if (!source_code[pos]) {
        current_tok_type = TOK_EOF;
        current_token[0] = '\0';
        return;
    }

    // Special characters
    if (source_code[pos] == '@') {
        current_tok_type = TOK_AT;
        strcpy(current_token, "@");
        pos++;
        return;
    }
    if (source_code[pos] == '(') {
        current_tok_type = TOK_LPAREN;
        strcpy(current_token, "(");
        pos++;
        return;
    }
    if (source_code[pos] == ')') {
        current_tok_type = TOK_RPAREN;
        strcpy(current_token, ")");
        pos++;
        return;
    }
    if (source_code[pos] == ',') {
        current_tok_type = TOK_COMMA;
        strcpy(current_token, ",");
        pos++;
        return;
    }
    if (source_code[pos] == '=') {
        current_tok_type = TOK_EQUAL;
        strcpy(current_token, "=");
        pos++;
        return;
    }

    // String literals
    if (source_code[pos] == '"') {
        pos++;
        int i = 0;
        while (source_code[pos] && source_code[pos] != '"' && i < 255) {
            current_token[i++] = source_code[pos++];
        }
        current_token[i] = '\0';
        if (source_code[pos] == '"') pos++;
        current_tok_type = TOK_STRING;
        return;
    }

    // Numbers
    if (isdigit(source_code[pos])) {
        int i = 0;
        while (isdigit(source_code[pos]) && i < 255) {
            current_token[i++] = source_code[pos++];
        }
        current_token[i] = '\0';
        current_tok_type = TOK_NUMBER;
        return;
    }

    // Identifiers and keywords
    if (isalpha(source_code[pos]) || source_code[pos] == '_') {
        int i = 0;
        while ((isalnum(source_code[pos]) || source_code[pos] == '_') && i < 255) {
            current_token[i++] = source_code[pos++];
        }
        current_token[i] = '\0';
        current_tok_type = get_keyword_type(current_token);
        return;
    }

    // Unknown
    current_token[0] = source_code[pos++];
    current_token[1] = '\0';
    current_tok_type = TOK_IDENT;
}

// ============================================================================
// ATTRIBUTE MAPPING
// ============================================================================

static AttrType get_attr_type(const char* name) {
    // Compile-time optimization
    if (strcmp(name, "inline") == 0) return ATTR_INLINE;
    if (strcmp(name, "hot") == 0) return ATTR_HOT;
    if (strcmp(name, "cold") == 0) return ATTR_COLD;
    if (strcmp(name, "no_mangle") == 0) return ATTR_NO_MANGLE;
    if (strcmp(name, "const") == 0) return ATTR_CONST;
    if (strcmp(name, "pure") == 0) return ATTR_PURE;
    
    // Testing
    if (strcmp(name, "test") == 0) return ATTR_TEST;
    if (strcmp(name, "bench") == 0) return ATTR_BENCH;
    if (strcmp(name, "ignore") == 0) return ATTR_IGNORE;
    if (strcmp(name, "should_panic") == 0) return ATTR_SHOULD_PANIC;
    
    // Deprecation
    if (strcmp(name, "deprecated") == 0) return ATTR_DEPRECATED;
    
    // Derive
    if (strcmp(name, "derive") == 0) return ATTR_DERIVE;
    
    // Serialization
    if (strcmp(name, "serialize") == 0) return ATTR_SERIALIZE;
    if (strcmp(name, "deserialize") == 0) return ATTR_DESERIALIZE;
    if (strcmp(name, "skip") == 0) return ATTR_SKIP;
    if (strcmp(name, "rename") == 0) return ATTR_RENAME;
    
    // Validation
    if (strcmp(name, "validate") == 0) return ATTR_VALIDATE;
    if (strcmp(name, "required") == 0) return ATTR_REQUIRED;
    if (strcmp(name, "range") == 0) return ATTR_RANGE;
    if (strcmp(name, "length") == 0) return ATTR_LENGTH;
    if (strcmp(name, "pattern") == 0) return ATTR_PATTERN;
    
    // Lints
    if (strcmp(name, "allow") == 0) return ATTR_ALLOW;
    if (strcmp(name, "warn") == 0) return ATTR_WARN;
    if (strcmp(name, "deny") == 0) return ATTR_DENY;
    if (strcmp(name, "forbid") == 0) return ATTR_FORBID;
    
    // Documentation
    if (strcmp(name, "doc") == 0) return ATTR_DOC;
    if (strcmp(name, "doc_hidden") == 0) return ATTR_DOC_HIDDEN;
    if (strcmp(name, "doc_inline") == 0) return ATTR_DOC_INLINE;
    
    // Conditional
    if (strcmp(name, "cfg") == 0) return ATTR_CFG;
    if (strcmp(name, "cfg_attr") == 0) return ATTR_CFG_ATTR;
    
    // FFI
    if (strcmp(name, "repr") == 0) return ATTR_REPR;
    if (strcmp(name, "packed") == 0) return ATTR_PACKED;
    if (strcmp(name, "align") == 0) return ATTR_ALIGN;
    
    // Misc
    if (strcmp(name, "must_use") == 0) return ATTR_MUST_USE;
    if (strcmp(name, "no_std") == 0) return ATTR_NO_STD;
    if (strcmp(name, "no_core") == 0) return ATTR_NO_CORE;
    
    return ATTR_NONE;
}

// ============================================================================
// PARSER HELPERS
// ============================================================================

static void parse_attr_params(char* dest, int max_len) {
    dest[0] = '\0';
    if (current_tok_type != TOK_LPAREN) return;
    
    next_token();
    while (current_tok_type != TOK_RPAREN && current_tok_type != TOK_EOF) {
        if (strlen(dest) + strlen(current_token) + 2 < max_len) {
            if (dest[0]) strcat(dest, ", ");
            strcat(dest, current_token);
        }
        next_token();
        if (current_tok_type == TOK_COMMA) {
            next_token();
        }
    }
    
    if (current_tok_type == TOK_RPAREN) {
        next_token();
    }
}

// ============================================================================
// PARSER
// ============================================================================

void attr_parser_init(const char* source) {
    source_code = source;
    pos = 0;
    next_token();
}

void attr_parse_file(AttributeCtx* ctx, AttrParseStats* stats) {
    memset(stats, 0, sizeof(AttrParseStats));
    
    char pending_target[128] = {0};
    
    while (current_tok_type != TOK_EOF) {
        // Parse @ attributes
        if (current_tok_type == TOK_AT) {
            next_token();
            
            if (current_tok_type == TOK_IDENT) {
                char attr_name[64];
                strcpy(attr_name, current_token);
                AttrType attr_type = get_attr_type(attr_name);
                
                next_token();
                
                // Parse parameters if present
                char params[256] = {0};
                if (current_tok_type == TOK_LPAREN) {
                    parse_attr_params(params, sizeof(params));
                }
                
                // Add attribute
                attr_add(ctx, attr_type, attr_name, params, pending_target, 0);
                stats->total_attributes++;
                
                // Update statistics
                switch (attr_type) {
                    case ATTR_INLINE: stats->inline_attrs++; ctx->inline_count++; break;
                    case ATTR_HOT: stats->hot_attrs++; ctx->hot_count++; break;
                    case ATTR_COLD: stats->cold_attrs++; ctx->cold_count++; break;
                    case ATTR_TEST: stats->test_attrs++; ctx->test_count++; break;
                    case ATTR_BENCH: stats->bench_attrs++; ctx->bench_count++; break;
                    case ATTR_IGNORE: stats->ignore_attrs++; break;
                    case ATTR_DEPRECATED:
                    case ATTR_DEPRECATED_MSG:
                        stats->deprecated_attrs++; ctx->deprecated_count++; break;
                    case ATTR_DERIVE:
                    case ATTR_DERIVE_DEBUG:
                    case ATTR_DERIVE_CLONE:
                    case ATTR_DERIVE_COPY:
                        stats->derive_attrs++; ctx->derive_count++; break;
                    case ATTR_SERIALIZE: 
                        stats->serialize_attrs++; ctx->serialize_count++; break;
                    case ATTR_DESERIALIZE:
                        stats->deserialize_attrs++; ctx->deserialize_count++; break;
                    case ATTR_VALIDATE:
                    case ATTR_REQUIRED:
                    case ATTR_RANGE:
                        stats->validate_attrs++; ctx->validate_count++; break;
                    case ATTR_ALLOW: stats->allow_attrs++; ctx->lint_count++; break;
                    case ATTR_WARN: stats->warn_attrs++; ctx->lint_count++; break;
                    case ATTR_DENY: stats->deny_attrs++; ctx->lint_count++; break;
                    case ATTR_DOC: stats->doc_attrs++; ctx->doc_count++; break;
                    case ATTR_CFG: stats->cfg_attrs++; ctx->cfg_count++; break;
                    case ATTR_REPR: stats->repr_attrs++; ctx->repr_count++; break;
                    case ATTR_MUST_USE: stats->must_use_attrs++; break;
                    default: break;
                }
            }
        }
        // Parse target (fn, struct, enum)
        else if (current_tok_type == TOK_FN || 
                 current_tok_type == TOK_STRUCT || 
                 current_tok_type == TOK_ENUM) {
            next_token();
            
            if (current_tok_type == TOK_IDENT) {
                strcpy(pending_target, current_token);
                next_token();
            }
        }
        else {
            next_token();
        }
    }
}
