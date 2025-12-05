#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Remove comments from MLP source code
// Handles:
// - Single-line comments: -- text
// - Multi-line comments: --- text ---
char* remove_comments(const char* source) {
    size_t len = strlen(source);
    char* result = malloc(len + 1);
    size_t write_pos = 0;
    size_t read_pos = 0;
    
    int single_line_removed = 0;
    int multi_line_removed = 0;
    
    while (read_pos < len) {
        // Check for multi-line comment: ---
        if (read_pos + 2 < len && 
            source[read_pos] == '-' && 
            source[read_pos + 1] == '-' && 
            source[read_pos + 2] == '-') {
            
            // Find the closing ---
            read_pos += 3;  // Skip opening ---
            
            // Find closing ---
            while (read_pos + 2 < len) {
                if (source[read_pos] == '-' && 
                    source[read_pos + 1] == '-' && 
                    source[read_pos + 2] == '-') {
                    read_pos += 3;  // Skip closing ---
                    multi_line_removed++;
                    break;
                }
                read_pos++;
            }
            continue;
        }
        
        // Check for single-line comment: --
        if (read_pos + 1 < len && 
            source[read_pos] == '-' && 
            source[read_pos + 1] == '-') {
            
            // Skip until end of line
            while (read_pos < len && source[read_pos] != '\n') {
                read_pos++;
            }
            single_line_removed++;
            // Keep the newline
            if (read_pos < len && source[read_pos] == '\n') {
                result[write_pos++] = '\n';
                read_pos++;
            }
            continue;
        }
        
        // Regular character - copy it
        result[write_pos++] = source[read_pos++];
    }
    
    result[write_pos] = '\0';
    
    printf("  ✓ Single-line comments removed: %d\n", single_line_removed);
    printf("  ✓ Multi-line comments removed: %d\n", multi_line_removed);
    
    return result;
}

int main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "Usage: %s <input.mlp> <output.mlp>\n", argv[0]);
        return 1;
    }
    
    printf("🔧 Comments Module - Comment Remover\n");
    printf("======================================\n");
    printf("Input:  %s\n", argv[1]);
    printf("Output: %s\n\n", argv[2]);
    
    // Read source file
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Error: Cannot open %s\n", argv[1]);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *src = malloc(sz + 1);
    fread(src, 1, sz, f);
    src[sz] = 0;
    fclose(f);
    
    // Remove comments
    char* cleaned = remove_comments(src);
    
    // Write cleaned code
    FILE *out = fopen(argv[2], "w");
    if (!out) {
        fprintf(stderr, "Error: Cannot create %s\n", argv[2]);
        free(src);
        free(cleaned);
        return 1;
    }
    
    fprintf(out, "%s", cleaned);
    fclose(out);
    
    printf("\n✅ Comments removed successfully!\n");
    printf("   Cleaned code: %s\n", argv[2]);
    
    free(src);
    free(cleaned);
    
    return 0;
}
