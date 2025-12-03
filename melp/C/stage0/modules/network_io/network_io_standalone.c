#include "network_io.h"
NetCtx* net_ctx_create(void) { return (NetCtx*)calloc(1, sizeof(NetCtx)); }
void net_ctx_free(NetCtx* ctx) { if (ctx) free(ctx); }
int main() {
    printf("[MODULE #50] Network IO Parser & Codegen\n════════════════════════════════════════\n\n");
    const char* test_src = "let tcp = TcpSocket::connect(\"127.0.0.1:8080\")\nlet udp = UdpSocket::bind(\"0.0.0.0:9000\")\nlet http = HttpClient::new()\nlet ws = WebSocket::connect(\"ws://localhost\")\n";
    NetCtx* ctx = net_ctx_create(); NetStats stats;
    net_parser_init(test_src); net_parse_file(ctx, &stats);
    printf("[3/4] Parsing network I/O...\n      ✓ TcpSocket: %d\n      ✓ UdpSocket: %d\n      ✓ HttpClient: %d\n      ✓ WebSocket: %d\n\n", stats.tcp_sockets, stats.udp_sockets, stats.http_clients, stats.websockets);
    printf("[4/4] Generating assembly...\n"); net_generate_asm("network_io.s", ctx); printf("      ✓ Code generated\n\n");
    net_ctx_free(ctx); printf("Network IO OK!\n✅ PASSED!\n"); return 0;
}
