/**
 * MLP Async/Await Module - Parser #70 (Stage 1)
 * 
 * Parses async/await syntax
 */

#include "async_await.h"

void asyncawait_parse(const char* input, AsyncAwaitCtx* ctx) {
    ctx->async_fn_count = 0;
    ctx->await_count = 0;
    ctx->future_count = 0;
    ctx->task_count = 0;
    
    char* input_copy = strdup(input);
    char* line = strtok(input_copy, "\n");
    
    while (line) {
        // Skip empty and comments
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '\0' || *line == '#' || strncmp(line, "//", 2) == 0) {
            line = strtok(NULL, "\n");
            continue;
        }
        
        if (strstr(line, "async fn") || strstr(line, "async ||") || strstr(line, "async {")) {
            parse_async_fn(line, ctx);
        } else if (strstr(line, ".await")) {
            parse_await(line, ctx);
        } else if (strstr(line, "Future<")) {
            parse_future(line, ctx);
        } else if (strstr(line, "spawn") || strstr(line, "task::")) {
            parse_task(line, ctx);
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(input_copy);
}

void parse_async_fn(const char* line, AsyncAwaitCtx* ctx) {
    AsyncFn* fn = &ctx->async_fns[ctx->async_fn_count++];
    
    if (strstr(line, "async fn")) {
        fn->type = ASYNC_FN_DECL;
        sscanf(line, " async fn %63[^(]", fn->name);
        
        // Parse return type
        const char* arrow = strstr(line, "->");
        if (arrow) {
            sscanf(arrow + 2, " %63s", fn->return_type);
        } else {
            strcpy(fn->return_type, "void");
        }
        
        // Parse params
        const char* lparen = strchr(line, '(');
        const char* rparen = strchr(line, ')');
        if (lparen && rparen) {
            int len = rparen - lparen - 1;
            if (len > 0) {
                strncpy(fn->params, lparen + 1, len);
                fn->params[len] = '\0';
            }
        }
    } else if (strstr(line, "async ||")) {
        fn->type = ASYNC_CLOSURE;
        strcpy(fn->name, "_async_closure");
    } else {
        fn->type = ASYNC_BLOCK;
        strcpy(fn->name, "_async_block");
    }
    
    fn->state = FUTURE_PENDING;
}

void parse_await(const char* line, AsyncAwaitCtx* ctx) {
    AwaitExpr* await = &ctx->awaits[ctx->await_count++];
    
    // Extract expression before .await
    const char* await_pos = strstr(line, ".await");
    if (await_pos) {
        int len = await_pos - line;
        // Skip assignment if present
        const char* equals = strchr(line, '=');
        if (equals && equals < await_pos) {
            sscanf(line, " %63s = ", await->result_var);
            line = equals + 1;
            len = await_pos - line;
        }
        
        strncpy(await->expr, line, len);
        await->expr[len] = '\0';
        
        // Trim whitespace
        char* p = await->expr;
        while (*p && (*p == ' ' || *p == '\t')) p++;
        memmove(await->expr, p, strlen(p) + 1);
    }
    
    // Check for timeout
    await->has_timeout = strstr(line, "timeout") != NULL;
    if (await->has_timeout) {
        sscanf(line, "%*[^timeout]timeout(%d)", &await->timeout_ms);
    }
}

void parse_future(const char* line, AsyncAwaitCtx* ctx) {
    Future* future = &ctx->futures[ctx->future_count++];
    
    sscanf(line, " Future<%63[^>]> %63s", future->inner_type, future->name);
    future->state = FUTURE_PENDING;
}

void parse_task(const char* line, AsyncAwaitCtx* ctx) {
    Task* task = &ctx->tasks[ctx->task_count++];
    
    if (strstr(line, "spawn")) {
        sscanf(line, " spawn(%63[^)])", task->async_fn);
        task->is_detached = false;
    } else if (strstr(line, "spawn_detached")) {
        sscanf(line, " spawn_detached(%63[^)])", task->async_fn);
        task->is_detached = true;
    }
    
    task->priority = 0; // Default priority
}
