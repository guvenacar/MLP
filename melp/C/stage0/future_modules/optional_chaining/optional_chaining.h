/**
 * MLP Optional Chaining Module - Header #72 (Stage 1)
 * 
 * Optional chaining operator ?. and ?.()
 * - obj?.field
 * - obj?.method()
 * - array?.[index]
 */

#ifndef OPTIONAL_CHAINING_H
#define OPTIONAL_CHAINING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CHAINS 100

typedef enum {
    CHAIN_FIELD,      // obj?.field
    CHAIN_METHOD,     // obj?.method()
    CHAIN_INDEX,      // arr?.[i]
} ChainType;

typedef struct {
    ChainType type;
    char base[64];
    char accessor[64];
    char args[256];
    int chain_length;
} OptionalChain;

typedef struct {
    OptionalChain chains[MAX_CHAINS];
    int chain_count;
} OptChainCtx;

void optchain_parse(const char* input, OptChainCtx* ctx);
void optchain_generate_asm(const char* output_file, OptChainCtx* ctx);

#endif
