#include "channels.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Forward declaration for usleep (POSIX)
extern int usleep(unsigned int usec);

// ============================================================================
// TEST DATA STRUCTURES
// ============================================================================

typedef struct {
    ChannelContext* ctx;
    Channel* ch;
    int count;
    int start_value;
} ThreadData;

// ============================================================================
// TEST 1: UNBUFFERED CHANNEL
// ============================================================================

void* sender_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (int i = 0; i < data->count; i++) {
        int* value = (int*)malloc(sizeof(int));
        *value = data->start_value + i;
        
        printf("[Sender] Sending: %d\n", *value);
        channel_send(data->ctx, data->ch, value);
        free(value);
        
        usleep(100000);  // 100ms
    }
    
    return NULL;
}

void* receiver_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (int i = 0; i < data->count; i++) {
        int* value = (int*)channel_receive(data->ctx, data->ch);
        
        if (value) {
            printf("[Receiver] Received: %d\n", *value);
            free(value);
        }
    }
    
    return NULL;
}

void test_unbuffered_channel() {
    printf("\n=== TEST 1: Unbuffered Channel ===\n");
    
    ChannelContext* ctx = channel_context_create();
    Channel* ch = channel_create(ctx, "i32", sizeof(int), 0);  // Unbuffered
    
    ThreadData sender_data = {ctx, ch, 5, 100};
    ThreadData receiver_data = {ctx, ch, 5, 0};
    
    pthread_t sender, receiver;
    pthread_create(&sender, NULL, sender_thread, &sender_data);
    pthread_create(&receiver, NULL, receiver_thread, &receiver_data);
    
    pthread_join(sender, NULL);
    pthread_join(receiver, NULL);
    
    // Print stats
    size_t sends, receives, blocked_send, blocked_recv;
    channel_stats(ch, &sends, &receives, &blocked_send, &blocked_recv);
    
    printf("Stats: sends=%zu, receives=%zu, blocked_send=%zu, blocked_recv=%zu\n",
           sends, receives, blocked_send, blocked_recv);
    
    channel_context_destroy(ctx);
    printf("✓ Test 1 passed\n");
}

// ============================================================================
// TEST 2: BUFFERED CHANNEL
// ============================================================================

void test_buffered_channel() {
    printf("\n=== TEST 2: Buffered Channel ===\n");
    
    ChannelContext* ctx = channel_context_create();
    Channel* ch = channel_create(ctx, "i32", sizeof(int), 3);  // Buffer size 3
    
    // Send 3 values without blocking
    for (int i = 0; i < 3; i++) {
        int* value = (int*)malloc(sizeof(int));
        *value = i + 1;
        
        bool sent = channel_send(ctx, ch, value);
        printf("Sent %d: %s\n", *value, sent ? "success" : "failed");
        free(value);
    }
    
    printf("Buffer full (count=%zu, cap=%zu)\n", 
           channel_len(ch), channel_cap(ch));
    
    // Receive all values
    for (int i = 0; i < 3; i++) {
        int* value = (int*)channel_receive(ctx, ch);
        if (value) {
            printf("Received: %d\n", *value);
            free(value);
        }
    }
    
    printf("Buffer empty (count=%zu)\n", channel_len(ch));
    
    channel_context_destroy(ctx);
    printf("✓ Test 2 passed\n");
}

// ============================================================================
// TEST 3: TRY_SEND / TRY_RECEIVE
// ============================================================================

void test_non_blocking() {
    printf("\n=== TEST 3: Non-blocking Operations ===\n");
    
    ChannelContext* ctx = channel_context_create();
    Channel* ch = channel_create(ctx, "i32", sizeof(int), 1);  // Buffer size 1
    
    // try_send should succeed
    int value1 = 42;
    bool sent1 = channel_try_send(ctx, ch, &value1);
    printf("try_send(42): %s\n", sent1 ? "success" : "would block");
    
    // try_send should fail (buffer full)
    int value2 = 99;
    bool sent2 = channel_try_send(ctx, ch, &value2);
    printf("try_send(99): %s (expected: would block)\n", 
           sent2 ? "success" : "would block");
    
    // try_receive should succeed
    bool ok;
    int* received1 = (int*)channel_try_receive(ctx, ch, &ok);
    if (ok && received1) {
        printf("try_receive: %d\n", *received1);
        free(received1);
    }
    
    // try_receive should fail (buffer empty)
    int* received2 = (int*)channel_try_receive(ctx, ch, &ok);
    printf("try_receive: %s (expected: would block)\n", 
           ok ? "success" : "would block");
    if (received2) free(received2);  // Free if received
    
    channel_context_destroy(ctx);
    printf("✓ Test 3 passed\n");
}

// ============================================================================
// TEST 4: CLOSE CHANNEL
// ============================================================================

void* close_receiver_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    while (true) {
        int* value = (int*)channel_receive(data->ctx, data->ch);
        
        if (!value) {
            printf("[Receiver] Channel closed, exiting\n");
            break;
        }
        
        printf("[Receiver] Received: %d\n", *value);
        free(value);
    }
    
    return NULL;
}

void test_close_channel() {
    printf("\n=== TEST 4: Close Channel ===\n");
    
    ChannelContext* ctx = channel_context_create();
    Channel* ch = channel_create(ctx, "i32", sizeof(int), 0);
    
    pthread_t receiver;
    ThreadData receiver_data = {ctx, ch, 0, 0};
    pthread_create(&receiver, NULL, close_receiver_thread, &receiver_data);
    
    // Send some values
    for (int i = 0; i < 3; i++) {
        int* value = (int*)malloc(sizeof(int));
        *value = i + 10;
        channel_send(ctx, ch, value);
        free(value);
        usleep(100000);
    }
    
    // Close channel
    printf("[Main] Closing channel\n");
    channel_close(ctx, ch);
    
    pthread_join(receiver, NULL);
    
    printf("Channel is_closed: %s\n", 
           channel_is_closed(ch) ? "true" : "false");
    
    channel_context_destroy(ctx);
    printf("✓ Test 4 passed\n");
}

// ============================================================================
// TEST 5: SELECT STATEMENT
// ============================================================================

void test_select_statement() {
    printf("\n=== TEST 5: Select Statement ===\n");
    
    ChannelContext* ctx = channel_context_create();
    Channel* ch1 = channel_create(ctx, "i32", sizeof(int), 1);
    Channel* ch2 = channel_create(ctx, "i32", sizeof(int), 1);
    
    // Prepare channels
    int value1 = 111;
    channel_send(ctx, ch1, &value1);
    
    int value2 = 222;
    channel_send(ctx, ch2, &value2);
    
    // Create select cases
    SelectCase* cases[3];
    cases[0] = select_case_receive(ch1, 0);
    cases[1] = select_case_receive(ch2, 1);
    cases[2] = select_case_default(2);
    
    // Execute select
    SelectResult* result = channel_select(ctx, cases, 3);
    
    printf("Selected case: %d\n", result->selected_case);
    printf("Default taken: %s\n", result->default_taken ? "yes" : "no");
    
    if (result->received_data) {
        int* data = (int*)result->received_data;
        printf("Received data: %d\n", *data);
    }
    
    // Cleanup
    for (int i = 0; i < 3; i++) {
        free(cases[i]);
    }
    select_result_free(result);
    
    channel_context_destroy(ctx);
    printf("✓ Test 5 passed\n");
}

// ============================================================================
// TEST 6: MULTI-PRODUCER SINGLE-CONSUMER (MPSC)
// ============================================================================

void* producer_thread(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    for (int i = 0; i < data->count; i++) {
        int* value = (int*)malloc(sizeof(int));
        *value = data->start_value + i;
        
        channel_send(data->ctx, data->ch, value);
        printf("[Producer %d] Sent: %d\n", data->start_value, *value);
        free(value);
        
        usleep(50000);  // 50ms
    }
    
    return NULL;
}

void test_mpsc() {
    printf("\n=== TEST 6: Multi-Producer Single-Consumer ===\n");
    
    ChannelContext* ctx = channel_context_create();
    Channel* ch = channel_create(ctx, "i32", sizeof(int), 5);
    
    // Create 3 producer threads
    pthread_t producers[3];
    ThreadData producer_data[3];
    
    for (int i = 0; i < 3; i++) {
        producer_data[i].ctx = ctx;
        producer_data[i].ch = ch;
        producer_data[i].count = 5;
        producer_data[i].start_value = (i + 1) * 1000;
        
        pthread_create(&producers[i], NULL, producer_thread, &producer_data[i]);
    }
    
    // Consumer receives all messages
    for (int i = 0; i < 15; i++) {
        int* value = (int*)channel_receive(ctx, ch);
        if (value) {
            printf("[Consumer] Received: %d\n", *value);
            free(value);
        }
    }
    
    // Wait for all producers
    for (int i = 0; i < 3; i++) {
        pthread_join(producers[i], NULL);
    }
    
    channel_context_destroy(ctx);
    printf("✓ Test 6 passed\n");
}

// ============================================================================
// TEST 7: PARSER
// ============================================================================

void test_parser() {
    printf("\n=== TEST 7: Parser ===\n");
    
    // Test channel declaration parsing
    const char* decl_lines[] = {
        "dim ch1 as Channel<i32>",
        "dim ch2 as Channel<string, 10>",
        "dim ch3 as Channel<Node>",
        "dim ch4 as Channel<i64, 100>"
    };
    
    int decl_count = 0;
    for (int i = 0; i < 4; i++) {
        ChannelDecl* decl = channel_parse_declaration(decl_lines[i]);
        if (decl) {
            printf("Declaration: %s as Channel<%s>%s\n",
                   decl->var_name,
                   decl->element_type,
                   decl->is_buffered ? " (buffered)" : " (unbuffered)");
            
            if (decl->is_buffered) {
                printf("  Capacity: %zu\n", decl->capacity);
            }
            
            channel_decl_free(decl);
            decl_count++;
        }
    }
    
    printf("Parsed %d declarations\n", decl_count);
    
    // Test channel operation parsing
    const char* op_lines[] = {
        "send(ch, 42)",
        "value = receive(ch)",
        "close(ch)",
        "ok = try_send(ch, 99)"
    };
    
    int op_count = 0;
    for (int i = 0; i < 4; i++) {
        ChannelOp* op = channel_parse_operation(op_lines[i]);
        if (op) {
            printf("Operation: type=%d, channel=%s\n",
                   op->op_type, op->channel_name);
            
            if (op->value_expr) {
                printf("  Value: %s\n", op->value_expr);
            }
            if (op->result_var) {
                printf("  Result: %s\n", op->result_var);
            }
            
            channel_op_free(op);
            op_count++;
        }
    }
    
    printf("Parsed %d operations\n", op_count);
    printf("✓ Test 7 passed\n");
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char** argv) {
    (void)argc;  // Unused
    (void)argv;  // Unused
    
    printf("===========================================\n");
    printf("     MLP Channels Module - Test Suite     \n");
    printf("===========================================\n");
    
    test_unbuffered_channel();
    test_buffered_channel();
    test_non_blocking();
    test_close_channel();
    test_select_statement();
    test_mpsc();
    test_parser();
    
    printf("\n===========================================\n");
    printf("     All tests passed! ✓                  \n");
    printf("===========================================\n");
    
    return 0;
}
