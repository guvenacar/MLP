/**
 * MLP Optional Chaining Module - Main #72
 */

#include "optional_chaining.h"

int main() {
    printf("=== MLP Optional Chaining Module Test ===\n\n");
    
    const char* input = 
        "# Optional chaining\n"
        "value = obj?.field\n"
        "result = user?.getName()\n"
        "item = array?.[0]\n"
        "nested = obj?.child?.value\n";
    
    OptChainCtx ctx;
    optchain_parse(input, &ctx);
    
    printf("Parsed %d optional chains:\n", ctx.chain_count);
    for (int i = 0; i < ctx.chain_count; i++) {
        OptionalChain* chain = &ctx.chains[i];
        const char* type = chain->type == CHAIN_FIELD ? "field" :
                          chain->type == CHAIN_METHOD ? "method" : "index";
        printf("  %d. %s?.%s [%s]\n", i + 1, chain->base, chain->accessor, type);
    }
    
    optchain_generate_asm("optional_chaining.s", &ctx);
    printf("\n✓ Assembly generated\n");
    
    printf("\n=== Optional Chaining Module: SUCCESS ===\n");
    return 0;
}
