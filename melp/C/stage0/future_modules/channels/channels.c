#include "channels.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

// Forward declaration for strdup
extern char* strdup(const char*);

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

ChannelContext* channel_context_create(void) {
    ChannelContext* ctx = (ChannelContext*)malloc(sizeof(ChannelContext));
    if (!ctx) return NULL;
    
    ctx->channels = (Channel**)malloc(sizeof(Channel*) * 16);
    ctx->channel_count = 0;
    ctx->channel_capacity = 16;
    
    ctx->total_sends = 0;
    ctx->total_receives = 0;
    ctx->total_closes = 0;
    ctx->total_selects = 0;
    
    ctx->threads = (pthread_t*)malloc(sizeof(pthread_t) * 16);
    ctx->thread_count = 0;
    ctx->thread_capacity = 16;
    
    return ctx;
}

void channel_context_destroy(ChannelContext* ctx) {
    if (!ctx) return;
    
    // Close and free all channels
    for (size_t i = 0; i < ctx->channel_count; i++) {
        if (ctx->channels[i]) {
            Channel* ch = ctx->channels[i];
            
            // Destroy mutex and conditions
            pthread_mutex_destroy(&ch->mutex);
            pthread_cond_destroy(&ch->not_full);
            pthread_cond_destroy(&ch->not_empty);
            
            // Free buffer
            if (ch->buffer) {
                for (size_t j = 0; j < ch->capacity; j++) {
                    if (ch->buffer[j]) free(ch->buffer[j]);
                }
                free(ch->buffer);
            }
            
            free(ch->type_name);
            free(ch->mangled_name);
            free(ch);
        }
    }
    
    free(ctx->channels);
    free(ctx->threads);
    free(ctx);
}

void channel_context_reset(ChannelContext* ctx) {
    if (!ctx) return;
    channel_context_destroy(ctx);
    *ctx = *channel_context_create();
}

// ============================================================================
// CHANNEL OPERATIONS
// ============================================================================

Channel* channel_create(ChannelContext* ctx, const char* type_name, 
                       size_t element_size, size_t capacity) {
    Channel* ch = (Channel*)malloc(sizeof(Channel));
    if (!ch) return NULL;
    
    ch->type_name = strdup(type_name);
    ch->mangled_name = channel_mangle_name(type_name);
    ch->type = (capacity == 0) ? CHANNEL_UNBUFFERED : CHANNEL_BUFFERED;
    ch->state = CHANNEL_OPEN;
    
    ch->capacity = capacity;
    ch->element_size = element_size;
    ch->head = 0;
    ch->tail = 0;
    ch->count = 0;
    
    // Allocate buffer
    if (capacity > 0) {
        ch->buffer = (void**)malloc(sizeof(void*) * capacity);
        for (size_t i = 0; i < capacity; i++) {
            ch->buffer[i] = NULL;
        }
    } else {
        ch->buffer = NULL;
    }
    
    // Initialize synchronization
    pthread_mutex_init(&ch->mutex, NULL);
    pthread_cond_init(&ch->not_full, NULL);
    pthread_cond_init(&ch->not_empty, NULL);
    
    ch->send_count = 0;
    ch->receive_count = 0;
    ch->blocked_senders = 0;
    ch->blocked_receivers = 0;
    
    // Register in context
    channel_register(ctx, ch);
    
    return ch;
}

bool channel_send(ChannelContext* ctx, Channel* ch, void* data) {
    if (!ch || !data) return false;
    
    pthread_mutex_lock(&ch->mutex);
    
    // Check if closed
    if (ch->state == CHANNEL_CLOSED) {
        pthread_mutex_unlock(&ch->mutex);
        return false;
    }
    
    // For unbuffered channels, wait for receiver
    if (ch->type == CHANNEL_UNBUFFERED) {
        // Wait until there's a receiver
        ch->blocked_senders++;
        while (ch->count > 0 && ch->state == CHANNEL_OPEN) {
            pthread_cond_wait(&ch->not_full, &ch->mutex);
        }
        ch->blocked_senders--;
        
        if (ch->state == CHANNEL_CLOSED) {
            pthread_mutex_unlock(&ch->mutex);
            return false;
        }
    } else {
        // For buffered channels, wait if full
        ch->blocked_senders++;
        while (ch->count >= ch->capacity && ch->state == CHANNEL_OPEN) {
            pthread_cond_wait(&ch->not_full, &ch->mutex);
        }
        ch->blocked_senders--;
        
        if (ch->state == CHANNEL_CLOSED) {
            pthread_mutex_unlock(&ch->mutex);
            return false;
        }
    }
    
    // Copy data to buffer
    void* copied_data = malloc(ch->element_size);
    memcpy(copied_data, data, ch->element_size);
    
    if (ch->type == CHANNEL_BUFFERED) {
        ch->buffer[ch->tail] = copied_data;
        ch->tail = (ch->tail + 1) % ch->capacity;
    } else {
        // For unbuffered, store temporarily
        if (!ch->buffer) {
            ch->buffer = (void**)malloc(sizeof(void*));
        }
        ch->buffer[0] = copied_data;
    }
    
    ch->count++;
    ch->send_count++;
    ctx->total_sends++;
    
    // Signal receivers
    pthread_cond_signal(&ch->not_empty);
    pthread_mutex_unlock(&ch->mutex);
    
    return true;
}

bool channel_try_send(ChannelContext* ctx, Channel* ch, void* data) {
    if (!ch || !data) return false;
    
    pthread_mutex_lock(&ch->mutex);
    
    // Check if closed
    if (ch->state == CHANNEL_CLOSED) {
        pthread_mutex_unlock(&ch->mutex);
        return false;
    }
    
    // Check if can send without blocking
    if (ch->type == CHANNEL_UNBUFFERED) {
        if (ch->count > 0) {
            pthread_mutex_unlock(&ch->mutex);
            return false;  // Would block
        }
    } else {
        if (ch->count >= ch->capacity) {
            pthread_mutex_unlock(&ch->mutex);
            return false;  // Would block
        }
    }
    
    // Send without blocking
    void* copied_data = malloc(ch->element_size);
    memcpy(copied_data, data, ch->element_size);
    
    if (ch->type == CHANNEL_BUFFERED) {
        ch->buffer[ch->tail] = copied_data;
        ch->tail = (ch->tail + 1) % ch->capacity;
    } else {
        if (!ch->buffer) {
            ch->buffer = (void**)malloc(sizeof(void*));
        }
        ch->buffer[0] = copied_data;
    }
    
    ch->count++;
    ch->send_count++;
    ctx->total_sends++;
    
    pthread_cond_signal(&ch->not_empty);
    pthread_mutex_unlock(&ch->mutex);
    
    return true;
}

void* channel_receive(ChannelContext* ctx, Channel* ch) {
    if (!ch) return NULL;
    
    pthread_mutex_lock(&ch->mutex);
    
    // Wait for data or close
    ch->blocked_receivers++;
    while (ch->count == 0 && ch->state == CHANNEL_OPEN) {
        pthread_cond_wait(&ch->not_empty, &ch->mutex);
    }
    ch->blocked_receivers--;
    
    // Check if closed and empty
    if (ch->state == CHANNEL_CLOSED && ch->count == 0) {
        pthread_mutex_unlock(&ch->mutex);
        return NULL;
    }
    
    // Get data from buffer
    void* data = NULL;
    
    if (ch->type == CHANNEL_BUFFERED) {
        data = ch->buffer[ch->head];
        ch->buffer[ch->head] = NULL;
        ch->head = (ch->head + 1) % ch->capacity;
    } else {
        // For unbuffered
        data = ch->buffer[0];
        ch->buffer[0] = NULL;
    }
    
    ch->count--;
    ch->receive_count++;
    ctx->total_receives++;
    
    // Signal senders
    pthread_cond_signal(&ch->not_full);
    pthread_mutex_unlock(&ch->mutex);
    
    return data;
}

void* channel_try_receive(ChannelContext* ctx, Channel* ch, bool* ok) {
    if (!ch) {
        if (ok) *ok = false;
        return NULL;
    }
    
    pthread_mutex_lock(&ch->mutex);
    
    // Check if can receive without blocking
    if (ch->count == 0) {
        pthread_mutex_unlock(&ch->mutex);
        if (ok) *ok = false;
        return NULL;
    }
    
    // Get data from buffer
    void* data = NULL;
    
    if (ch->type == CHANNEL_BUFFERED) {
        data = ch->buffer[ch->head];
        ch->buffer[ch->head] = NULL;
        ch->head = (ch->head + 1) % ch->capacity;
    } else {
        data = ch->buffer[0];
        ch->buffer[0] = NULL;
    }
    
    ch->count--;
    ch->receive_count++;
    ctx->total_receives++;
    
    pthread_cond_signal(&ch->not_full);
    pthread_mutex_unlock(&ch->mutex);
    
    if (ok) *ok = true;
    return data;
}

void channel_close(ChannelContext* ctx, Channel* ch) {
    if (!ch) return;
    
    pthread_mutex_lock(&ch->mutex);
    
    if (ch->state == CHANNEL_CLOSED) {
        pthread_mutex_unlock(&ch->mutex);
        return;
    }
    
    ch->state = CHANNEL_CLOSED;
    ctx->total_closes++;
    
    // Wake up all blocked senders and receivers
    pthread_cond_broadcast(&ch->not_full);
    pthread_cond_broadcast(&ch->not_empty);
    
    pthread_mutex_unlock(&ch->mutex);
}

bool channel_is_closed(Channel* ch) {
    if (!ch) return true;
    
    pthread_mutex_lock(&ch->mutex);
    bool closed = (ch->state == CHANNEL_CLOSED);
    pthread_mutex_unlock(&ch->mutex);
    
    return closed;
}

size_t channel_len(Channel* ch) {
    if (!ch) return 0;
    
    pthread_mutex_lock(&ch->mutex);
    size_t len = ch->count;
    pthread_mutex_unlock(&ch->mutex);
    
    return len;
}

size_t channel_cap(Channel* ch) {
    if (!ch) return 0;
    return ch->capacity;
}

// ============================================================================
// SELECT STATEMENT
// ============================================================================

SelectCase* select_case_send(Channel* ch, void* data, int case_index) {
    SelectCase* sc = (SelectCase*)malloc(sizeof(SelectCase));
    sc->op_type = SELECT_SEND;
    sc->channel = ch;
    sc->data = data;
    sc->ready = false;
    sc->case_index = case_index;
    sc->channel_name = NULL;
    sc->value_expr = NULL;
    sc->result_var = NULL;
    return sc;
}

SelectCase* select_case_receive(Channel* ch, int case_index) {
    SelectCase* sc = (SelectCase*)malloc(sizeof(SelectCase));
    sc->op_type = SELECT_RECEIVE;
    sc->channel = ch;
    sc->data = NULL;
    sc->ready = false;
    sc->case_index = case_index;
    sc->channel_name = NULL;
    sc->value_expr = NULL;
    sc->result_var = NULL;
    return sc;
}

SelectCase* select_case_default(int case_index) {
    SelectCase* sc = (SelectCase*)malloc(sizeof(SelectCase));
    sc->op_type = SELECT_DEFAULT;
    sc->channel = NULL;
    sc->data = NULL;
    sc->ready = true;  // Default is always ready
    sc->case_index = case_index;
    sc->channel_name = NULL;
    sc->value_expr = NULL;
    sc->result_var = NULL;
    return sc;
}

SelectResult* channel_select(ChannelContext* ctx, SelectCase** cases, size_t case_count) {
    if (!cases || case_count == 0) return NULL;
    
    ctx->total_selects++;
    
    SelectResult* result = (SelectResult*)malloc(sizeof(SelectResult));
    result->selected_case = -1;
    result->default_taken = false;
    result->received_data = NULL;
    
    // Check for default case
    SelectCase* default_case = NULL;
    for (size_t i = 0; i < case_count; i++) {
        if (cases[i]->op_type == SELECT_DEFAULT) {
            default_case = cases[i];
            break;
        }
    }
    
    // Try non-blocking operations
    while (true) {
        // Shuffle cases for fairness (simple randomization)
        for (size_t i = 0; i < case_count; i++) {
            size_t j = rand() % case_count;
            SelectCase* temp = cases[i];
            cases[i] = cases[j];
            cases[j] = temp;
        }
        
        // Try each case
        for (size_t i = 0; i < case_count; i++) {
            SelectCase* sc = cases[i];
            
            if (sc->op_type == SELECT_SEND) {
                if (channel_try_send(ctx, sc->channel, sc->data)) {
                    result->selected_case = sc->case_index;
                    return result;
                }
            } else if (sc->op_type == SELECT_RECEIVE) {
                bool ok;
                void* data = channel_try_receive(ctx, sc->channel, &ok);
                if (ok) {
                    result->selected_case = sc->case_index;
                    result->received_data = data;
                    return result;
                }
            }
        }
        
        // If we have default case and nothing is ready, take default
        if (default_case) {
            result->selected_case = default_case->case_index;
            result->default_taken = true;
            return result;
        }
        
        // Small sleep to avoid busy-waiting
        struct timespec ts = {0, 1000000};  // 1ms
        nanosleep(&ts, NULL);
    }
    
    return result;
}

void select_result_free(SelectResult* result) {
    if (result) {
        if (result->received_data) free(result->received_data);
        free(result);
    }
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

char* channel_mangle_name(const char* type_name) {
    size_t len = strlen("Channel_") + strlen(type_name) + 1;
    char* mangled = (char*)malloc(len);
    snprintf(mangled, len, "Channel_%s", type_name);
    return mangled;
}

void channel_register(ChannelContext* ctx, Channel* ch) {
    if (!ctx || !ch) return;
    
    // Expand if needed
    if (ctx->channel_count >= ctx->channel_capacity) {
        ctx->channel_capacity *= 2;
        ctx->channels = (Channel**)realloc(ctx->channels, 
                                           sizeof(Channel*) * ctx->channel_capacity);
    }
    
    ctx->channels[ctx->channel_count++] = ch;
}

void channel_stats(Channel* ch, size_t* sends, size_t* receives, 
                   size_t* blocked_send, size_t* blocked_recv) {
    if (!ch) return;
    
    pthread_mutex_lock(&ch->mutex);
    
    if (sends) *sends = ch->send_count;
    if (receives) *receives = ch->receive_count;
    if (blocked_send) *blocked_send = ch->blocked_senders;
    if (blocked_recv) *blocked_recv = ch->blocked_receivers;
    
    pthread_mutex_unlock(&ch->mutex);
}

// ============================================================================
// THREAD UTILITIES
// ============================================================================

void channel_register_thread(ChannelContext* ctx, pthread_t thread) {
    if (!ctx) return;
    
    // Expand if needed
    if (ctx->thread_count >= ctx->thread_capacity) {
        ctx->thread_capacity *= 2;
        ctx->threads = (pthread_t*)realloc(ctx->threads, 
                                           sizeof(pthread_t) * ctx->thread_capacity);
    }
    
    ctx->threads[ctx->thread_count++] = thread;
}

void channel_wait_threads(ChannelContext* ctx) {
    if (!ctx) return;
    
    for (size_t i = 0; i < ctx->thread_count; i++) {
        pthread_join(ctx->threads[i], NULL);
    }
}
