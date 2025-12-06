/**
 * MLP Async/Await Module - Main #70 (Stage 1)
 * 
 * Test async/await functionality
 */

#include "async_await.h"

int main() {
    printf("=== MLP Async/Await Module Test ===\n\n");
    
    const char* input = 
        "# Modern async/await syntax\n"
        "\n"
        "# Async function declaration\n"
        "async fn fetch_data(url: text) -> Result<text, Error> {\n"
        "    response.await\n"
        "}\n"
        "\n"
        "# Async with await\n"
        "result = fetch_data(url).await\n"
        "\n"
        "# Future type\n"
        "Future<numeric> fut\n"
        "\n"
        "# Spawn task\n"
        "spawn(fetch_data)\n"
        "\n"
        "# Async closure\n"
        "async || {\n"
        "    value.await\n"
        "}\n"
        "\n"
        "# Await with timeout\n"
        "data.await.timeout(5000)\n";
    
    AsyncAwaitCtx ctx;
    asyncawait_parse(input, &ctx);
    
    printf("Parsed:\n");
    printf("  Async functions: %d\n", ctx.async_fn_count);
    printf("  Await expressions: %d\n", ctx.await_count);
    printf("  Futures: %d\n", ctx.future_count);
    printf("  Tasks: %d\n", ctx.task_count);
    printf("\n");
    
    printf("Async Functions:\n");
    for (int i = 0; i < ctx.async_fn_count; i++) {
        AsyncFn* fn = &ctx.async_fns[i];
        printf("  %d. async fn %s(%s) -> %s\n", 
               i + 1, fn->name, fn->params, fn->return_type);
    }
    printf("\n");
    
    printf("Await Expressions:\n");
    for (int i = 0; i < ctx.await_count; i++) {
        AwaitExpr* await = &ctx.awaits[i];
        printf("  %d. %s.await", i + 1, await->expr);
        if (await->has_timeout) {
            printf(" (timeout: %dms)", await->timeout_ms);
        }
        printf("\n");
    }
    printf("\n");
    
    asyncawait_generate_asm("async_await.s", &ctx);
    printf("✓ Assembly generated: async_await.s\n");
    
    printf("\n=== Async/Await Module: SUCCESS ===\n");
    return 0;
}
