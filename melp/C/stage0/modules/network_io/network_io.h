#ifndef MLP_NETWORK_IO_H
#define MLP_NETWORK_IO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum { NET_TCP, NET_UDP, NET_HTTP, NET_WEBSOCKET, NET_CONNECT, NET_LISTEN, NET_SEND, NET_RECV } NetOpType;
typedef struct { char name[64]; NetOpType type; } NetOp;
typedef struct { NetOp ops[256]; int op_count; int tcp_count; int udp_count; int http_count; } NetCtx;
typedef struct { int tcp_sockets; int udp_sockets; int http_clients; int websockets; } NetStats;
NetCtx* net_ctx_create(void); void net_ctx_free(NetCtx* ctx);
void net_parser_init(const char* source); void net_parse_file(NetCtx* ctx, NetStats* stats);
void net_generate_asm(const char* output_file, NetCtx* ctx);
#endif
