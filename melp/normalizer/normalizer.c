#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: normalizer input.mlp output.mlp\n");
        return 1;
    }
    
    FILE* in = fopen(argv[1], "r");
    if (!in) {
        fprintf(stderr, "Error: Cannot open input file '%s'\n", argv[1]);
        return 1;
    }
    
    FILE* out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot open output file '%s'\n", argv[2]);
        fclose(in);
        return 1;
    }
    
    // Identity function - şimdilik sadece kopyala
    // Gelecekte: MLP → PMPL dönüşümü burada yapılacak
    char buf[4096];
    while (fgets(buf, sizeof(buf), in)) {
        fputs(buf, out);
    }
    
    fclose(in);
    fclose(out);
    
    printf("[Normalizer] Identity: %s → %s\n", argv[1], argv[2]);
    return 0;
}
