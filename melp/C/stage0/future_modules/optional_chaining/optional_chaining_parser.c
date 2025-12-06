/**
 * MLP Optional Chaining Module - Parser #72
 */

#include "optional_chaining.h"

void optchain_parse(const char* input, OptChainCtx* ctx) {
    ctx->chain_count = 0;
    
    char* input_copy = strdup(input);
    char* line = strtok(input_copy, "\n");
    
    while (line) {
        while (*line == ' ' || *line == '\t') line++;
        if (*line == '\0' || *line == '#' || strncmp(line, "//", 2) == 0) {
            line = strtok(NULL, "\n");
            continue;
        }
        
        if (strstr(line, "?.")) {
            OptionalChain* chain = &ctx->chains[ctx->chain_count++];
            
            // Find base
            const char* q = strstr(line, "?.");
            if (q) {
                int base_len = q - line;
                const char* start = line;
                const char* equals = strchr(line, '=');
                if (equals && equals < q) {
                    start = equals + 1;
                    base_len = q - start;
                }
                
                while (*start == ' ') { start++; base_len--; }
                strncpy(chain->base, start, base_len);
                chain->base[base_len] = '\0';
                
                // Find accessor
                const char* acc_start = q + 2;
                if (strchr(acc_start, '(')) {
                    chain->type = CHAIN_METHOD;
                    sscanf(acc_start, "%63[^(]", chain->accessor);
                    
                    const char* lparen = strchr(acc_start, '(');
                    const char* rparen = strchr(acc_start, ')');
                    if (lparen && rparen) {
                        int len = rparen - lparen - 1;
                        if (len > 0) {
                            strncpy(chain->args, lparen + 1, len);
                            chain->args[len] = '\0';
                        }
                    }
                } else if (strchr(acc_start, '[')) {
                    chain->type = CHAIN_INDEX;
                    sscanf(acc_start, "[%63[^]]]", chain->accessor);
                } else {
                    chain->type = CHAIN_FIELD;
                    sscanf(acc_start, "%63s", chain->accessor);
                }
                
                // Count chain length
                chain->chain_length = 1;
                const char* p = q + 2;
                while ((p = strstr(p, "?.")) != NULL) {
                    chain->chain_length++;
                    p += 2;
                }
            }
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(input_copy);
}
