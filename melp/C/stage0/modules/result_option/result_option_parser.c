/**
 * MLP Result/Option Module - Parser #41 (Stage 1)
 * 
 * Lexer and parser implementation for Result/Option types
 */

#include "result_option.h"
#include <ctype.h>

// ============================================================================
// LEXER
// ============================================================================

typedef enum {
    TOK_EOF,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_STRING,
    TOK_LET,
    TOK_RESULT,
    TOK_OPTION,
    TOK_OK,
    TOK_ERR,
    TOK_SOME,
    TOK_NONE,
    TOK_UNWRAP,
    TOK_EXPECT,
    TOK_UNWRAP_OR,
    TOK_UNWRAP_OR_ELSE,
    TOK_MAP,
    TOK_AND_THEN,
    TOK_OR_ELSE,
    TOK_IS_OK,
    TOK_IS_ERR,
    TOK_IS_SOME,
    TOK_IS_NONE,
    TOK_COLON,
    TOK_COMMA,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LT,
    TOK_GT,
    TOK_EQUAL,
    TOK_DOT,
    TOK_QUESTION,
    TOK_ARROW,
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

static int is_keyword(const char* word, const char* keyword) {
    return strcmp(word, keyword) == 0;
}

static TokType get_keyword_type(const char* word) {
    if (is_keyword(word, "let")) return TOK_LET;
    if (is_keyword(word, "Result")) return TOK_RESULT;
    if (is_keyword(word, "Option")) return TOK_OPTION;
    if (is_keyword(word, "Ok")) return TOK_OK;
    if (is_keyword(word, "Err")) return TOK_ERR;
    if (is_keyword(word, "Some")) return TOK_SOME;
    if (is_keyword(word, "None")) return TOK_NONE;
    if (is_keyword(word, "unwrap")) return TOK_UNWRAP;
    if (is_keyword(word, "expect")) return TOK_EXPECT;
    if (is_keyword(word, "unwrap_or")) return TOK_UNWRAP_OR;
    if (is_keyword(word, "unwrap_or_else")) return TOK_UNWRAP_OR_ELSE;
    if (is_keyword(word, "map")) return TOK_MAP;
    if (is_keyword(word, "and_then")) return TOK_AND_THEN;
    if (is_keyword(word, "or_else")) return TOK_OR_ELSE;
    if (is_keyword(word, "is_ok")) return TOK_IS_OK;
    if (is_keyword(word, "is_err")) return TOK_IS_ERR;
    if (is_keyword(word, "is_some")) return TOK_IS_SOME;
    if (is_keyword(word, "is_none")) return TOK_IS_NONE;
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

    // Operators
    if (source_code[pos] == ':') {
        current_tok_type = TOK_COLON;
        strcpy(current_token, ":");
        pos++;
        return;
    }
    if (source_code[pos] == ',') {
        current_tok_type = TOK_COMMA;
        strcpy(current_token, ",");
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
    if (source_code[pos] == '{') {
        current_tok_type = TOK_LBRACE;
        strcpy(current_token, "{");
        pos++;
        return;
    }
    if (source_code[pos] == '}') {
        current_tok_type = TOK_RBRACE;
        strcpy(current_token, "}");
        pos++;
        return;
    }
    if (source_code[pos] == '<') {
        current_tok_type = TOK_LT;
        strcpy(current_token, "<");
        pos++;
        return;
    }
    if (source_code[pos] == '>') {
        current_tok_type = TOK_GT;
        strcpy(current_token, ">");
        pos++;
        return;
    }
    if (source_code[pos] == '=') {
        current_tok_type = TOK_EQUAL;
        strcpy(current_token, "=");
        pos++;
        return;
    }
    if (source_code[pos] == '.') {
        current_tok_type = TOK_DOT;
        strcpy(current_token, ".");
        pos++;
        return;
    }
    if (source_code[pos] == '?') {
        current_tok_type = TOK_QUESTION;
        strcpy(current_token, "?");
        pos++;
        return;
    }
    if (source_code[pos] == '-' && source_code[pos+1] == '>') {
        current_tok_type = TOK_ARROW;
        strcpy(current_token, "->");
        pos += 2;
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
// PARSER HELPERS
// ============================================================================

static void parse_type_params(char* dest, int max_len) {
    dest[0] = '\0';
    if (current_tok_type != TOK_LT) return;
    
    strcat(dest, "<");
    next_token();
    
    while (current_tok_type != TOK_GT && current_tok_type != TOK_EOF) {
        if (strlen(dest) + strlen(current_token) + 2 < max_len) {
            strcat(dest, current_token);
        }
        next_token();
        if (current_tok_type == TOK_COMMA) {
            strcat(dest, ", ");
            next_token();
        }
    }
    
    if (current_tok_type == TOK_GT) {
        strcat(dest, ">");
        next_token();
    }
}

static void parse_function_call(char* dest, int max_len) {
    dest[0] = '\0';
    if (current_tok_type != TOK_LPAREN) return;
    
    next_token();
    while (current_tok_type != TOK_RPAREN && current_tok_type != TOK_EOF) {
        if (strlen(dest) + strlen(current_token) + 2 < max_len) {
            if (dest[0]) strcat(dest, " ");
            strcat(dest, current_token);
        }
        next_token();
    }
    
    if (current_tok_type == TOK_RPAREN) {
        next_token();
    }
}

// ============================================================================
// PARSER
// ============================================================================

void resopt_parser_init(const char* source) {
    source_code = source;
    pos = 0;
    next_token();
}

void resopt_parse_file(ResOptCtx* ctx, ResOptParseStats* stats) {
    memset(stats, 0, sizeof(ResOptParseStats));
    
    while (current_tok_type != TOK_EOF) {
        // let var: Result<T, E> = ...
        if (current_tok_type == TOK_LET) {
            next_token();
            char var_name[64] = {0};
            if (current_tok_type == TOK_IDENT) {
                strcpy(var_name, current_token);
                next_token();
            }
            
            if (current_tok_type == TOK_COLON) {
                next_token();
                
                // Result<T, E>
                if (current_tok_type == TOK_RESULT) {
                    stats->result_types++;
                    next_token();
                    char type_params[128] = {0};
                    parse_type_params(type_params, sizeof(type_params));
                    
                    resopt_add_op(ctx, RESOPT_RESULT_TYPE, var_name, "", type_params, 0);
                    resopt_add_result(ctx, var_name, "T", "E");
                    
                    if (current_tok_type == TOK_EQUAL) {
                        next_token();
                        // Ok(value)
                        if (current_tok_type == TOK_OK) {
                            stats->ok_constructors++;
                            next_token();
                            char args[256] = {0};
                            parse_function_call(args, sizeof(args));
                            resopt_add_ok(ctx, var_name, args);
                            resopt_add_op(ctx, RESOPT_OK, var_name, args, "", 0);
                        }
                        // Err(error)
                        else if (current_tok_type == TOK_ERR) {
                            stats->err_constructors++;
                            next_token();
                            char args[256] = {0};
                            parse_function_call(args, sizeof(args));
                            resopt_add_err(ctx, var_name, args);
                            resopt_add_op(ctx, RESOPT_ERR, var_name, args, "", 0);
                        }
                    }
                }
                // Option<T>
                else if (current_tok_type == TOK_OPTION) {
                    stats->option_types++;
                    next_token();
                    char type_params[128] = {0};
                    parse_type_params(type_params, sizeof(type_params));
                    
                    resopt_add_op(ctx, RESOPT_OPTION_TYPE, var_name, "", type_params, 0);
                    resopt_add_option(ctx, var_name, "T");
                    
                    if (current_tok_type == TOK_EQUAL) {
                        next_token();
                        // Some(value)
                        if (current_tok_type == TOK_SOME) {
                            stats->some_constructors++;
                            next_token();
                            char args[256] = {0};
                            parse_function_call(args, sizeof(args));
                            resopt_add_some(ctx, var_name, args);
                            resopt_add_op(ctx, RESOPT_SOME, var_name, args, "", 0);
                        }
                        // None
                        else if (current_tok_type == TOK_NONE) {
                            stats->none_constructors++;
                            ctx->none_count++;
                            resopt_add_op(ctx, RESOPT_NONE_VAL, var_name, "", "", 0);
                            next_token();
                        }
                    }
                }
            }
        }
        // Method calls: var.unwrap(), var.is_ok(), etc.
        else if (current_tok_type == TOK_IDENT) {
            char var_name[64];
            strcpy(var_name, current_token);
            next_token();
            
            if (current_tok_type == TOK_DOT) {
                next_token();
                
                if (current_tok_type == TOK_UNWRAP) {
                    stats->unwraps++;
                    ctx->unwrap_count++;
                    next_token();
                    resopt_add_op(ctx, RESOPT_UNWRAP, var_name, "", "", 0);
                    if (current_tok_type == TOK_LPAREN) parse_function_call(var_name, 64);
                }
                else if (current_tok_type == TOK_EXPECT) {
                    stats->expects++;
                    ctx->expect_count++;
                    next_token();
                    char args[256] = {0};
                    parse_function_call(args, sizeof(args));
                    resopt_add_op(ctx, RESOPT_EXPECT, var_name, args, "", 0);
                }
                else if (current_tok_type == TOK_UNWRAP_OR) {
                    stats->unwrap_ors++;
                    ctx->unwrap_or_count++;
                    next_token();
                    char args[256] = {0};
                    parse_function_call(args, sizeof(args));
                    resopt_add_op(ctx, RESOPT_UNWRAP_OR, var_name, args, "", 0);
                }
                else if (current_tok_type == TOK_UNWRAP_OR_ELSE) {
                    stats->unwrap_or_elses++;
                    next_token();
                    char args[256] = {0};
                    parse_function_call(args, sizeof(args));
                    resopt_add_op(ctx, RESOPT_UNWRAP_OR_ELSE, var_name, args, "", 0);
                }
                else if (current_tok_type == TOK_MAP) {
                    stats->maps++;
                    ctx->map_count++;
                    next_token();
                    char args[256] = {0};
                    parse_function_call(args, sizeof(args));
                    resopt_add_op(ctx, RESOPT_MAP, var_name, args, "", 0);
                }
                else if (current_tok_type == TOK_AND_THEN) {
                    stats->and_thens++;
                    ctx->and_then_count++;
                    next_token();
                    char args[256] = {0};
                    parse_function_call(args, sizeof(args));
                    resopt_add_op(ctx, RESOPT_AND_THEN, var_name, args, "", 0);
                }
                else if (current_tok_type == TOK_OR_ELSE) {
                    stats->or_elses++;
                    ctx->or_else_count++;
                    next_token();
                    char args[256] = {0};
                    parse_function_call(args, sizeof(args));
                    resopt_add_op(ctx, RESOPT_OR_ELSE, var_name, args, "", 0);
                }
                else if (current_tok_type == TOK_IS_OK) {
                    stats->is_oks++;
                    ctx->is_ok_count++;
                    next_token();
                    resopt_add_op(ctx, RESOPT_IS_OK, var_name, "", "", 0);
                    if (current_tok_type == TOK_LPAREN) parse_function_call(var_name, 64);
                }
                else if (current_tok_type == TOK_IS_ERR) {
                    stats->is_errs++;
                    ctx->is_err_count++;
                    next_token();
                    resopt_add_op(ctx, RESOPT_IS_ERR, var_name, "", "", 0);
                    if (current_tok_type == TOK_LPAREN) parse_function_call(var_name, 64);
                }
                else if (current_tok_type == TOK_IS_SOME) {
                    stats->is_somes++;
                    ctx->is_some_count++;
                    next_token();
                    resopt_add_op(ctx, RESOPT_IS_SOME, var_name, "", "", 0);
                    if (current_tok_type == TOK_LPAREN) parse_function_call(var_name, 64);
                }
                else if (current_tok_type == TOK_IS_NONE) {
                    stats->is_nones++;
                    ctx->is_none_count++;
                    next_token();
                    resopt_add_op(ctx, RESOPT_IS_NONE, var_name, "", "", 0);
                    if (current_tok_type == TOK_LPAREN) parse_function_call(var_name, 64);
                }
            }
            // ? operator
            else if (current_tok_type == TOK_QUESTION) {
                stats->question_ops++;
                ctx->question_op_count++;
                resopt_add_op(ctx, RESOPT_QUESTION_OP, var_name, "", "", 0);
                next_token();
            }
        }
        else {
            next_token();
        }
    }
}
