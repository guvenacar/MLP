/**
 * MLP Concurrency Module - Parser #40 (Stage 1)
 * 
 * MODÜLER ZİNCİR: Tüm önceki modüllerle entegre (39 modül)
 * 
 * Parser for concurrency constructs:
 * - Thread: spawn, join, detach, go
 * - Mutex: new, lock, unlock, try_lock
 * - Channel: new, send, recv
 * - Atomic: new, load, store, fetch_add, compare_exchange
 * - Sync: Semaphore, Barrier, CondVar
 * - Control: parallel_for, select
 */

#include "concurrency.h"
#include <ctype.h>

// ============================================================================
// LEXER
// ============================================================================

typedef enum {
    TOK_EOF, TOK_IDENT, TOK_STRING, TOK_NUMBER, TOK_KEYWORD,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE, TOK_LBRACKET, TOK_RBRACKET,
    TOK_ARROW_LEFT, TOK_ARROW_RIGHT, TOK_DOUBLE_COLON,
    TOK_COLON, TOK_COMMA, TOK_DOT, TOK_LT, TOK_GT, TOK_NEWLINE, TOK_OTHER
} TokType;

typedef struct {
    TokType type;
    char value[512];
    int line;
} Token;

static const char* src_text;
static int src_pos;
static int src_line;
static Token current_tok;

// ============================================================================
// LEXER IMPLEMENTATION
// ============================================================================

static char peek_char(void) { return src_text[src_pos]; }
static char peek_next(void) { return src_text[src_pos] ? src_text[src_pos + 1] : 0; }

static char next_char(void) {
    char c = src_text[src_pos];
    if (c) {
        if (c == '\n') src_line++;
        src_pos++;
    }
    return c;
}

static void skip_spaces(void) {
    while (peek_char() && peek_char() != '\n' && isspace(peek_char())) {
        next_char();
    }
}

static void skip_line_comment(void) {
    while (peek_char() && peek_char() != '\n') next_char();
}

static int is_keyword(const char* s) {
    const char* keywords[] = {
        "spawn", "thread", "join", "detach", "go",
        "Mutex", "mutex", "lock", "unlock", "try_lock",
        "Channel", "channel", "chan", "send", "recv", "receive", "close",
        "Atomic", "atomic", "load", "store", "fetch_add", "compare_exchange",
        "Semaphore", "semaphore", "wait", "signal",
        "Barrier", "barrier", "wait_all",
        "CondVar", "condvar", "notify", "notify_all",
        "parallel_for", "parallel", "select", "case", "default",
        "let", "numeric", "text", "boolean", "void",
        "new", "function", "end", NULL
    };
    for (int i = 0; keywords[i]; i++) {
        if (strcmp(s, keywords[i]) == 0) return 1;
    }
    return 0;
}

static Token next_token(void) {
    Token t = {TOK_EOF, "", src_line};
    skip_spaces();
    
    char c = peek_char();
    if (!c) return t;
    t.line = src_line;
    
    // Skip comments
    if (c == '-' && peek_next() == '-') {
        skip_line_comment();
        return next_token();
    }
    
    if (c == '\n') { t.type = TOK_NEWLINE; t.value[0] = '\n'; t.value[1] = '\0'; next_char(); return t; }
    
    // Arrow <- (channel receive)
    if (c == '<' && peek_next() == '-') {
        next_char(); next_char();
        t.type = TOK_ARROW_LEFT;
        strcpy(t.value, "<-");
        return t;
    }
    // Arrow ->
    if (c == '-' && peek_next() == '>') {
        next_char(); next_char();
        t.type = TOK_ARROW_RIGHT;
        strcpy(t.value, "->");
        return t;
    }
    // Double colon ::
    if (c == ':' && peek_next() == ':') {
        next_char(); next_char();
        t.type = TOK_DOUBLE_COLON;
        strcpy(t.value, "::");
        return t;
    }
    
    if (c == '(') { next_char(); t.type = TOK_LPAREN; strcpy(t.value, "("); return t; }
    if (c == ')') { next_char(); t.type = TOK_RPAREN; strcpy(t.value, ")"); return t; }
    if (c == '{') { next_char(); t.type = TOK_LBRACE; strcpy(t.value, "{"); return t; }
    if (c == '}') { next_char(); t.type = TOK_RBRACE; strcpy(t.value, "}"); return t; }
    if (c == '[') { next_char(); t.type = TOK_LBRACKET; strcpy(t.value, "["); return t; }
    if (c == ']') { next_char(); t.type = TOK_RBRACKET; strcpy(t.value, "]"); return t; }
    if (c == ':') { next_char(); t.type = TOK_COLON; strcpy(t.value, ":"); return t; }
    if (c == ',') { next_char(); t.type = TOK_COMMA; strcpy(t.value, ","); return t; }
    if (c == '.') { next_char(); t.type = TOK_DOT; strcpy(t.value, "."); return t; }
    if (c == '<') { next_char(); t.type = TOK_LT; strcpy(t.value, "<"); return t; }
    if (c == '>') { next_char(); t.type = TOK_GT; strcpy(t.value, ">"); return t; }
    
    // String
    if (c == '"') {
        next_char();
        int i = 0;
        while (peek_char() && peek_char() != '"' && i < 510) {
            t.value[i++] = next_char();
        }
        t.value[i] = '\0';
        if (peek_char() == '"') next_char();
        t.type = TOK_STRING;
        return t;
    }
    
    // Number
    if (isdigit(c)) {
        int i = 0;
        while ((isdigit(peek_char()) || peek_char() == '.') && i < 510) {
            t.value[i++] = next_char();
        }
        t.value[i] = '\0';
        t.type = TOK_NUMBER;
        return t;
    }
    
    // Identifier/Keyword
    if (isalpha(c) || c == '_') {
        int i = 0;
        while ((isalnum(peek_char()) || peek_char() == '_') && i < 510) {
            t.value[i++] = next_char();
        }
        t.value[i] = '\0';
        t.type = is_keyword(t.value) ? TOK_KEYWORD : TOK_IDENT;
        return t;
    }
    
    t.type = TOK_OTHER;
    t.value[0] = next_char();
    t.value[1] = '\0';
    return t;
}

static void advance(void) { current_tok = next_token(); }

// ============================================================================
// PARSER IMPLEMENTATION
// ============================================================================

// Parse: spawn thread { ... }
static void parse_spawn(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();  // skip 'spawn'
    
    char thread_name[64] = "anonymous";
    
    if (current_tok.type == TOK_KEYWORD && strcmp(current_tok.value, "thread") == 0) {
        advance();
    }
    
    if (current_tok.type == TOK_IDENT) {
        strncpy(thread_name, current_tok.value, 63);
        thread_name[63] = '\0';
        advance();
    }
    
    conc_add_thread(ctx, thread_name);
    conc_add_op(ctx, CONC_SPAWN, thread_name, NULL, src_line);
    printf("      ✓ spawn thread: %s\n", thread_name);
    stats->spawns++;
    
    // Skip block
    if (current_tok.type == TOK_LBRACE) {
        int depth = 1;
        advance();
        while (current_tok.type != TOK_EOF && depth > 0) {
            if (current_tok.type == TOK_LBRACE) depth++;
            else if (current_tok.type == TOK_RBRACE) depth--;
            advance();
        }
    }
}

// Parse: go func() { ... }
static void parse_go(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();  // skip 'go'
    
    conc_add_op(ctx, CONC_GO, "goroutine", NULL, src_line);
    printf("      ✓ go (goroutine)\n");
    stats->goroutines++;
    ctx->goroutine_count++;
    
    while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
        if (current_tok.type == TOK_LBRACE) {
            int depth = 1;
            advance();
            while (current_tok.type != TOK_EOF && depth > 0) {
                if (current_tok.type == TOK_LBRACE) depth++;
                else if (current_tok.type == TOK_RBRACE) depth--;
                advance();
            }
            break;
        }
        advance();
    }
}

// Parse: join(thread)
static void parse_join(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();  // skip 'join'
    
    char thread_name[64] = "";
    if (current_tok.type == TOK_LPAREN) {
        advance();
        if (current_tok.type == TOK_IDENT) {
            strncpy(thread_name, current_tok.value, 63);
            thread_name[63] = '\0';
        }
        while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF) advance();
        if (current_tok.type == TOK_RPAREN) advance();
    }
    
    conc_add_op(ctx, CONC_JOIN, thread_name, NULL, src_line);
    printf("      ✓ join(%s)\n", thread_name);
    stats->joins++;
}

// Parse: detach(thread)
static void parse_detach(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();  // skip 'detach'
    
    char thread_name[64] = "";
    if (current_tok.type == TOK_LPAREN) {
        advance();
        if (current_tok.type == TOK_IDENT) {
            strncpy(thread_name, current_tok.value, 63);
            thread_name[63] = '\0';
        }
        while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF) advance();
        if (current_tok.type == TOK_RPAREN) advance();
    }
    
    conc_add_op(ctx, CONC_DETACH, thread_name, NULL, src_line);
    printf("      ✓ detach(%s)\n", thread_name);
    stats->detaches++;
}

// Parse: let m: Mutex = Mutex::new()
static void parse_mutex_decl(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* var_name) {
    conc_add_mutex(ctx, var_name);
    conc_add_op(ctx, CONC_MUTEX_NEW, var_name, NULL, src_line);
    printf("      ✓ Mutex::new(): %s\n", var_name);
    stats->mutex_news++;
}

// Parse: m.lock(), m.unlock(), m.try_lock()
static void parse_mutex_op(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* var_name, const char* op) {
    if (strcmp(op, "lock") == 0) {
        conc_add_op(ctx, CONC_MUTEX_LOCK, var_name, NULL, src_line);
        printf("      ✓ %s.lock()\n", var_name);
        stats->mutex_locks++;
    } else if (strcmp(op, "unlock") == 0) {
        conc_add_op(ctx, CONC_MUTEX_UNLOCK, var_name, NULL, src_line);
        printf("      ✓ %s.unlock()\n", var_name);
        stats->mutex_unlocks++;
    } else if (strcmp(op, "try_lock") == 0) {
        conc_add_op(ctx, CONC_MUTEX_TRYLOCK, var_name, NULL, src_line);
        printf("      ✓ %s.try_lock()\n", var_name);
        stats->mutex_trylocks++;
    }
}

// Parse: let ch: Channel<T> = Channel::new()
static void parse_channel_decl(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* var_name, const char* type_param) {
    conc_add_channel(ctx, var_name, type_param);
    conc_add_op(ctx, CONC_CHANNEL_NEW, var_name, type_param, src_line);
    printf("      ✓ Channel<%s>::new(): %s\n", type_param, var_name);
    stats->channel_news++;
}

// Parse: ch.send(value)
static void parse_channel_send(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* ch_name, const char* value) {
    conc_add_op(ctx, CONC_CHANNEL_SEND, ch_name, value, src_line);
    printf("      ✓ %s.send(%s)\n", ch_name, value);
    stats->channel_sends++;
}

// Parse: ch.recv()
static void parse_channel_recv(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* ch_name) {
    conc_add_op(ctx, CONC_CHANNEL_RECV, ch_name, NULL, src_line);
    printf("      ✓ %s.recv()\n", ch_name);
    stats->channel_recvs++;
}

// Parse: let a: Atomic<T> = Atomic::new(init)
static void parse_atomic_decl(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* var_name, const char* type_param, const char* init) {
    conc_add_atomic(ctx, var_name, type_param, init);
    conc_add_op(ctx, CONC_ATOMIC_NEW, var_name, init, src_line);
    printf("      ✓ Atomic<%s>::new(%s): %s\n", type_param, init, var_name);
    stats->atomic_news++;
}

// Parse: a.load(), a.store(v), a.fetch_add(v), a.compare_exchange(exp, new)
static void parse_atomic_op(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* var_name, const char* op, const char* args) {
    if (strcmp(op, "load") == 0) {
        conc_add_op(ctx, CONC_ATOMIC_LOAD, var_name, NULL, src_line);
        printf("      ✓ %s.load()\n", var_name);
        stats->atomic_loads++;
    } else if (strcmp(op, "store") == 0) {
        conc_add_op(ctx, CONC_ATOMIC_STORE, var_name, args, src_line);
        printf("      ✓ %s.store(%s)\n", var_name, args);
        stats->atomic_stores++;
    } else if (strcmp(op, "fetch_add") == 0) {
        conc_add_op(ctx, CONC_ATOMIC_FETCH_ADD, var_name, args, src_line);
        printf("      ✓ %s.fetch_add(%s)\n", var_name, args);
        stats->atomic_fetch_adds++;
    } else if (strcmp(op, "compare_exchange") == 0) {
        conc_add_op(ctx, CONC_ATOMIC_COMPARE_EXCHANGE, var_name, args, src_line);
        printf("      ✓ %s.compare_exchange(%s)\n", var_name, args);
        stats->atomic_cas++;
    }
}

// Parse sync primitives
static void parse_semaphore(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    conc_add_op(ctx, CONC_SEMAPHORE_NEW, "semaphore", NULL, src_line);
    printf("      ✓ Semaphore::new()\n");
    stats->semaphores++;
    ctx->semaphore_count++;
}

static void parse_barrier(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    conc_add_op(ctx, CONC_BARRIER_NEW, "barrier", NULL, src_line);
    printf("      ✓ Barrier::new()\n");
    stats->barriers++;
    ctx->barrier_count++;
}

static void parse_condvar(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    conc_add_op(ctx, CONC_CONDVAR_NEW, "condvar", NULL, src_line);
    printf("      ✓ CondVar::new()\n");
    stats->condvars++;
    ctx->condvar_count++;
}

// Parse: parallel_for i in range { ... }
static void parse_parallel_for(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();  // skip 'parallel_for' or 'parallel'
    
    conc_add_op(ctx, CONC_PARALLEL_FOR, "parallel_for", NULL, src_line);
    printf("      ✓ parallel_for\n");
    stats->parallel_fors++;
    ctx->parallel_for_count++;
    
    while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
        if (current_tok.type == TOK_LBRACE) {
            int depth = 1;
            advance();
            while (current_tok.type != TOK_EOF && depth > 0) {
                if (current_tok.type == TOK_LBRACE) depth++;
                else if (current_tok.type == TOK_RBRACE) depth--;
                advance();
            }
            break;
        }
        advance();
    }
}

// Parse: select { case ch1 <- x: ... }
static void parse_select(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();  // skip 'select'
    
    conc_add_op(ctx, CONC_SELECT, "select", NULL, src_line);
    printf("      ✓ select\n");
    stats->selects++;
    ctx->select_count++;
    
    if (current_tok.type == TOK_LBRACE) {
        int depth = 1;
        advance();
        while (current_tok.type != TOK_EOF && depth > 0) {
            if (current_tok.type == TOK_LBRACE) depth++;
            else if (current_tok.type == TOK_RBRACE) depth--;
            advance();
        }
    }
}

// Parse let declaration
static void parse_let(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();  // skip 'let'
    
    if (current_tok.type != TOK_IDENT) return;
    
    char var_name[64];
    strncpy(var_name, current_tok.value, 63);
    var_name[63] = '\0';
    advance();
    
    if (current_tok.type != TOK_COLON) return;
    advance();
    
    if (current_tok.type != TOK_KEYWORD && current_tok.type != TOK_IDENT) return;
    
    char type_name[64];
    strncpy(type_name, current_tok.value, 63);
    type_name[63] = '\0';
    advance();
    
    char type_param[32] = "any";
    
    // Generic: Type<T>
    if (current_tok.type == TOK_LT) {
        advance();
        if (current_tok.type == TOK_IDENT || current_tok.type == TOK_KEYWORD) {
            strncpy(type_param, current_tok.value, 31);
            type_param[31] = '\0';
            advance();
        }
        if (current_tok.type == TOK_GT) advance();
    }
    
    // Find ::new()
    while (current_tok.type != TOK_NEWLINE && current_tok.type != TOK_EOF) {
        if (current_tok.type == TOK_DOUBLE_COLON) {
            advance();
            if (current_tok.type == TOK_KEYWORD && strcmp(current_tok.value, "new") == 0) {
                advance();
                
                char init_val[64] = "0";
                if (current_tok.type == TOK_LPAREN) {
                    advance();
                    if (current_tok.type == TOK_NUMBER || current_tok.type == TOK_IDENT) {
                        strncpy(init_val, current_tok.value, 63);
                        init_val[63] = '\0';
                    }
                    while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF) advance();
                    if (current_tok.type == TOK_RPAREN) advance();
                }
                
                if (strcmp(type_name, "Mutex") == 0 || strcmp(type_name, "mutex") == 0) {
                    parse_mutex_decl(ctx, stats, var_name);
                } else if (strcmp(type_name, "Channel") == 0 || strcmp(type_name, "channel") == 0) {
                    parse_channel_decl(ctx, stats, var_name, type_param);
                } else if (strcmp(type_name, "Atomic") == 0 || strcmp(type_name, "atomic") == 0) {
                    parse_atomic_decl(ctx, stats, var_name, type_param, init_val);
                } else if (strcmp(type_name, "Semaphore") == 0 || strcmp(type_name, "semaphore") == 0) {
                    parse_semaphore(ctx, stats);
                } else if (strcmp(type_name, "Barrier") == 0 || strcmp(type_name, "barrier") == 0) {
                    parse_barrier(ctx, stats);
                } else if (strcmp(type_name, "CondVar") == 0 || strcmp(type_name, "condvar") == 0) {
                    parse_condvar(ctx, stats);
                }
                return;
            }
        }
        advance();
    }
}

// Parse method call: var.method(args)
static void parse_method_call(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    char var_name[64];
    strncpy(var_name, current_tok.value, 63);
    var_name[63] = '\0';
    advance();
    
    if (current_tok.type != TOK_DOT) return;
    advance();
    
    if (current_tok.type != TOK_KEYWORD && current_tok.type != TOK_IDENT) return;
    
    char method_name[64];
    strncpy(method_name, current_tok.value, 63);
    method_name[63] = '\0';
    advance();
    
    char args[256] = "";
    if (current_tok.type == TOK_LPAREN) {
        advance();
        int i = 0;
        while (current_tok.type != TOK_RPAREN && current_tok.type != TOK_EOF && i < 250) {
            int len = strlen(current_tok.value);
            if (i + len < 250) {
                strcat(args, current_tok.value);
                i += len;
            }
            advance();
        }
        if (current_tok.type == TOK_RPAREN) advance();
    }
    
    // Determine operation type
    if (strcmp(method_name, "lock") == 0 || strcmp(method_name, "unlock") == 0 || strcmp(method_name, "try_lock") == 0) {
        parse_mutex_op(ctx, stats, var_name, method_name);
    } else if (strcmp(method_name, "send") == 0) {
        parse_channel_send(ctx, stats, var_name, args);
    } else if (strcmp(method_name, "recv") == 0 || strcmp(method_name, "receive") == 0) {
        parse_channel_recv(ctx, stats, var_name);
    } else if (strcmp(method_name, "load") == 0 || strcmp(method_name, "store") == 0 ||
               strcmp(method_name, "fetch_add") == 0 || strcmp(method_name, "compare_exchange") == 0) {
        parse_atomic_op(ctx, stats, var_name, method_name, args);
    }
}

// Parse arrow: <- ch or ch <- value
static void parse_arrow_op(ConcurrencyCtx* ctx, ConcParseStats* stats, const char* prev_ident) {
    if (current_tok.type == TOK_ARROW_LEFT) {
        advance();
        if (current_tok.type == TOK_IDENT) {
            parse_channel_recv(ctx, stats, current_tok.value);
            advance();
        }
    } else if (current_tok.type == TOK_ARROW_RIGHT) {
        advance();
        if (current_tok.type == TOK_IDENT || current_tok.type == TOK_NUMBER) {
            parse_channel_send(ctx, stats, prev_ident, current_tok.value);
            advance();
        }
    }
}

// ============================================================================
// PUBLIC FUNCTIONS
// ============================================================================

void conc_parser_init(const char* source) {
    src_text = source;
    src_pos = 0;
    src_line = 1;
}

void conc_parse_file(ConcurrencyCtx* ctx, ConcParseStats* stats) {
    advance();
    
    char prev_ident[64] = "";
    
    while (current_tok.type != TOK_EOF) {
        if (current_tok.type == TOK_KEYWORD) {
            if (strcmp(current_tok.value, "spawn") == 0) {
                parse_spawn(ctx, stats);
            } else if (strcmp(current_tok.value, "go") == 0) {
                parse_go(ctx, stats);
            } else if (strcmp(current_tok.value, "join") == 0) {
                parse_join(ctx, stats);
            } else if (strcmp(current_tok.value, "detach") == 0) {
                parse_detach(ctx, stats);
            } else if (strcmp(current_tok.value, "let") == 0) {
                parse_let(ctx, stats);
            } else if (strcmp(current_tok.value, "parallel_for") == 0 || strcmp(current_tok.value, "parallel") == 0) {
                parse_parallel_for(ctx, stats);
            } else if (strcmp(current_tok.value, "select") == 0) {
                parse_select(ctx, stats);
            } else {
                advance();
            }
        } else if (current_tok.type == TOK_IDENT) {
            strncpy(prev_ident, current_tok.value, 63);
            prev_ident[63] = '\0';
            
            Token saved = current_tok;
            advance();
            if (current_tok.type == TOK_DOT) {
                // Reset and parse method call
                current_tok = saved;
                parse_method_call(ctx, stats);
            } else if (current_tok.type == TOK_ARROW_LEFT || current_tok.type == TOK_ARROW_RIGHT) {
                parse_arrow_op(ctx, stats, prev_ident);
            }
        } else if (current_tok.type == TOK_ARROW_LEFT) {
            parse_arrow_op(ctx, stats, "");
        } else {
            advance();
        }
    }
}
