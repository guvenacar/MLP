#include "comments_codegen.h"

// Generate assembly comments (optional, for debugging)
void codegen_comment(FILE* f, const char* comment_text) {
    if (!f || !comment_text) return;
    
    // Output as assembly comment
    fprintf(f, "    ; %s\n", comment_text);
}
