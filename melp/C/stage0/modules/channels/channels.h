#ifndef CHANNELS_H
#define CHANNELS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

// ============================================================================
// MLP CHANNELS SYSTEM - MODULE #59
// ============================================================================
// Go-style channels for concurrent communication
// Buffered/unbuffered channels, send/receive, select statement
// MLP syntax: dim ch as Channel<i32>, send(ch, 42), value = receive(ch)

// ============================================================================
// CHANNEL TYPES
// ============================================================================

typedef enum {
    CHANNEL_UNBUFFERED,  // Synchronous (capacity = 0)
    CHANNEL_BUFFERED     // Asynchronous (capacity > 0)
} ChannelType;

typedef enum {
    CHANNEL_OPEN,        // Channel is open
    CHANNEL_CLOSED       // Channel is closed
} ChannelState;

// ============================================================================
// CHANNEL STRUCTURE
// ============================================================================

typedef struct Channel {
    char* type_name;             // Type parameter (e.g., "i32")
    char* mangled_name;          // Mangled name (e.g., "Channel_i32")
    
    ChannelType type;            // Buffered or unbuffered
    ChannelState state;          // Open or closed
    
    void** buffer;               // Ring buffer for messages
    size_t capacity;             // Buffer capacity (0 = unbuffered)
    size_t element_size;         // Size of each element
    size_t head;                 // Read position
    size_t tail;                 // Write position
    size_t count;                // Current number of elements
    
    pthread_mutex_t mutex;       // Mutex for synchronization
    pthread_cond_t not_full;     // Condition: buffer not full
    pthread_cond_t not_empty;    // Condition: buffer not empty
    
    size_t send_count;           // Total sends
    size_t receive_count;        // Total receives
    size_t blocked_senders;      // Number of blocked senders
    size_t blocked_receivers;    // Number of blocked receivers
} Channel;

// ============================================================================
// SELECT OPERATION
// ============================================================================

typedef enum {
    SELECT_SEND,     // Send operation
    SELECT_RECEIVE,  // Receive operation
    SELECT_DEFAULT   // Default case (non-blocking)
} SelectOpType;

typedef struct SelectCase {
    SelectOpType op_type;        // Type of operation
    Channel* channel;            // Channel for operation
    void* data;                  // Data to send (for SEND) or receive buffer (for RECEIVE)
    bool ready;                  // Is this case ready?
    int case_index;              // Case index (for user identification)
    
    // Parser fields
    char* channel_name;          // Channel name (parsed)
    char* value_expr;            // Value expression (parsed)
    char* result_var;            // Result variable (parsed)
} SelectCase;

typedef struct SelectResult {
    int selected_case;           // Index of selected case (-1 if none)
    bool default_taken;          // Was default case taken?
    void* received_data;         // Data received (for RECEIVE cases)
} SelectResult;

// ============================================================================
// CHANNEL CONTEXT
// ============================================================================

typedef struct ChannelContext {
    Channel** channels;          // Array of all channels
    size_t channel_count;        // Number of channels
    size_t channel_capacity;     // Capacity of channels array
    
    size_t total_sends;          // Total send operations
    size_t total_receives;       // Total receive operations
    size_t total_closes;         // Total close operations
    size_t total_selects;        // Total select operations
    
    pthread_t* threads;          // Array of thread handles
    size_t thread_count;         // Number of active threads
    size_t thread_capacity;      // Capacity of threads array
} ChannelContext;

// ============================================================================
// CONTEXT MANAGEMENT
// ============================================================================

ChannelContext* channel_context_create(void);
void channel_context_destroy(ChannelContext* ctx);
void channel_context_reset(ChannelContext* ctx);

// ============================================================================
// CHANNEL OPERATIONS
// ============================================================================

// Create new channel
Channel* channel_create(ChannelContext* ctx, const char* type_name, 
                       size_t element_size, size_t capacity);

// Send data to channel (blocking)
bool channel_send(ChannelContext* ctx, Channel* ch, void* data);

// Send data to channel (non-blocking)
bool channel_try_send(ChannelContext* ctx, Channel* ch, void* data);

// Receive data from channel (blocking)
void* channel_receive(ChannelContext* ctx, Channel* ch);

// Receive data from channel (non-blocking)
void* channel_try_receive(ChannelContext* ctx, Channel* ch, bool* ok);

// Close channel
void channel_close(ChannelContext* ctx, Channel* ch);

// Check if channel is closed
bool channel_is_closed(Channel* ch);

// Get channel length (number of elements)
size_t channel_len(Channel* ch);

// Get channel capacity
size_t channel_cap(Channel* ch);

// ============================================================================
// SELECT STATEMENT
// ============================================================================

// Create select cases
SelectCase* select_case_send(Channel* ch, void* data, int case_index);
SelectCase* select_case_receive(Channel* ch, int case_index);
SelectCase* select_case_default(int case_index);

// Execute select statement (blocks until one case is ready)
SelectResult* channel_select(ChannelContext* ctx, SelectCase** cases, size_t case_count);

// Free select result
void select_result_free(SelectResult* result);

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

// Mangle channel name
char* channel_mangle_name(const char* type_name);

// Register channel in context
void channel_register(ChannelContext* ctx, Channel* ch);

// Get channel statistics
void channel_stats(Channel* ch, size_t* sends, size_t* receives, 
                   size_t* blocked_send, size_t* blocked_recv);

// ============================================================================
// PARSER STRUCTURES
// ============================================================================

// Channel operation types
typedef enum {
    CHAN_OP_SEND,
    CHAN_OP_RECEIVE,
    CHAN_OP_CLOSE
} ChannelOpType;

// Channel declaration (parsed from MLP source)
typedef struct ChannelDecl {
    char* var_name;              // Variable name
    char* element_type;          // Element type (e.g., "i32")
    size_t capacity;             // Buffer capacity
    bool is_buffered;            // Is buffered channel?
} ChannelDecl;

// Channel operation (parsed from MLP source)
typedef struct ChannelOp {
    ChannelOpType op_type;       // Operation type
    char* channel_name;          // Channel variable name
    char* value_expr;            // Value expression (for SEND)
    char* result_var;            // Result variable (for RECEIVE)
    bool is_blocking;            // Blocking or non-blocking?
} ChannelOp;

// Select statement (parsed from MLP source)
typedef struct SelectStmt {
    SelectCase** cases;          // Array of select cases
    size_t case_count;           // Number of cases
    bool has_default;            // Has default case?
} SelectStmt;

// Parser functions
ChannelDecl* channel_parse_declaration(const char* line);
void channel_decl_free(ChannelDecl* decl);
ChannelOp* channel_parse_operation(const char* line);
void channel_op_free(ChannelOp* op);
SelectStmt* channel_parse_select(const char** lines, size_t line_count);
void select_stmt_free(SelectStmt* stmt);
bool channel_is_channel_type(const char* type_str);
char* channel_extract_element_type(const char* channel_type);

// ============================================================================
// CODEGEN FUNCTIONS
// ============================================================================

char* codegen_channel_declaration(ChannelDecl* decl, ChannelContext* ctx);
char* codegen_channel_send(ChannelOp* op, ChannelContext* ctx);
char* codegen_channel_receive(ChannelOp* op, ChannelContext* ctx);
char* codegen_channel_close(ChannelOp* op, ChannelContext* ctx);
char* codegen_channel_select(SelectStmt* stmt, ChannelContext* ctx);
char* codegen_channel_operation(ChannelOp* op, ChannelContext* ctx);
char* codegen_channel_stats(const char* channel_name);

// ============================================================================
// THREAD UTILITIES
// ============================================================================

// Register thread in context
void channel_register_thread(ChannelContext* ctx, pthread_t thread);

// Wait for all threads
void channel_wait_threads(ChannelContext* ctx);

#endif // CHANNELS_H
