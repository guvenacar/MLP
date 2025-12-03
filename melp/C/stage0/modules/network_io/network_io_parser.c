#include "network_io.h"
#include <ctype.h>
static const char* src = NULL; static int p = 0;
void net_parser_init(const char* source) { src = source; p = 0; }
void net_parse_file(NetCtx* ctx, NetStats* stats) {
    memset(stats, 0, sizeof(NetStats));
    char tok[64];
    while (src[p]) {
        while (src[p] && isspace(src[p])) p++;
        int i = 0;
        while (isalnum(src[p]) || src[p] == '_') tok[i++] = src[p++];
        tok[i] = '\0';
        if (strcmp(tok, "TcpSocket") == 0) { stats->tcp_sockets++; ctx->tcp_count++; }
        else if (strcmp(tok, "UdpSocket") == 0) { stats->udp_sockets++; ctx->udp_count++; }
        else if (strcmp(tok, "HttpClient") == 0) { stats->http_clients++; ctx->http_count++; }
        else if (strcmp(tok, "WebSocket") == 0) { stats->websockets++; }
        else if (src[p]) p++;
    }
}
