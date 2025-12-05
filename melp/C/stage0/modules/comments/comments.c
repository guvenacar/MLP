#include "comments.h"
#include <string.h>
#include <stdlib.h>

// Check if current position starts a comment
int is_comment_start(const char* source, int pos) {
    if (source[pos] == '-' && source[pos + 1] == '-') {
        // Check for multi-line comment (---)
        if (source[pos + 2] == '-') {
            return 1; // multi-line
        }
        return 1; // single-line
    }
    return 0;
}

// Check if current position starts a doc comment
int is_doc_comment_start(const char* source, int pos) {
    if (source[pos] == '-' && source[pos + 1] == '-') {
        // Check for doc comment marker (#)
        if (source[pos + 2] == '#') {
            return 1; // --# doc comment
        }
        // Check for multi-line doc comment (---#)
        if (source[pos + 2] == '-' && source[pos + 3] == '#') {
            return 1; // ---# doc comment
        }
    }
    return 0;
}

// Skip a comment and return new position
// Updates line counter for multi-line comments
int skip_comment(const char* source, int pos, int* line) {
    if (source[pos] != '-' || source[pos + 1] != '-') {
        return pos; // Not a comment
    }
    
    // Check for doc comment (--# or ---#) - currently just skip them
    // In future, we'll extract and store these
    if (source[pos + 2] == '#') {
        // Single-line doc comment: skip until newline
        pos += 3; // Skip --#
        while (source[pos] != '\0' && source[pos] != '\n') {
            pos++;
        }
        return pos;
    }
    
    // Check for multi-line comment (---)
    if (source[pos + 2] == '-') {
        // Check for multi-line doc comment (---#)
        if (source[pos + 3] == '#') {
            // Multi-line doc comment: skip until closing ---#
            pos += 4; // Skip opening ---#
            
            while (source[pos] != '\0') {
                // Check for closing ---#
                if (source[pos] == '-' && source[pos + 1] == '-' && 
                    source[pos + 2] == '-' && source[pos + 3] == '#') {
                    pos += 4; // Skip closing ---#
                    break;
                }
                
                if (source[pos] == '\n') {
                    (*line)++;
                }
                pos++;
            }
            return pos;
        }
        
        // Regular multi-line comment: skip until closing ---
        pos += 3; // Skip opening ---
        
        while (source[pos] != '\0') {
            // Check for closing ---
            if (source[pos] == '-' && source[pos + 1] == '-' && source[pos + 2] == '-') {
                pos += 3; // Skip closing ---
                break;
            }
            
            if (source[pos] == '\n') {
                (*line)++;
            }
            pos++;
        }
    } else {
        // Single-line comment: skip until newline
        pos += 2; // Skip --
        
        while (source[pos] != '\0' && source[pos] != '\n') {
            pos++;
        }
        
        // Don't skip the newline itself - let lexer handle it
    }
    
    return pos;
}

// Extract documentation comment (returns content, caller must free)
char* extract_doc_comment(const char* source, int pos, int* new_pos, int* line) {
    if (source[pos] != '-' || source[pos + 1] != '-') {
        return NULL;
    }
    
    // Check for single-line doc comment (--#)
    if (source[pos + 2] == '#') {
        pos += 3; // Skip --#
        int start = pos;
        
        // Find end of line
        while (source[pos] != '\0' && source[pos] != '\n') {
            pos++;
        }
        
        int len = pos - start;
        char* content = malloc(len + 1);
        strncpy(content, &source[start], len);
        content[len] = '\0';
        
        *new_pos = pos;
        return content;
    }
    
    // Check for multi-line doc comment (---#)
    if (source[pos + 2] == '-' && source[pos + 3] == '#') {
        pos += 4; // Skip ---#
        int start = pos;
        
        // Find closing ---#
        while (source[pos] != '\0') {
            if (source[pos] == '-' && source[pos + 1] == '-' && 
                source[pos + 2] == '-' && source[pos + 3] == '#') {
                
                int len = pos - start;
                char* content = malloc(len + 1);
                strncpy(content, &source[start], len);
                content[len] = '\0';
                
                pos += 4; // Skip closing ---#
                *new_pos = pos;
                return content;
            }
            
            if (source[pos] == '\n') {
                (*line)++;
            }
            pos++;
        }
    }
    
    return NULL;
}
