#ifndef COMMENTS_H
#define COMMENTS_H

// Comments module - handles single and multi-line comments
// Comments are processed at lexer level and stripped from token stream

// Comment types
typedef enum {
    COMMENT_SINGLE,      // -- single line comment
    COMMENT_MULTI,       // --- multi-line comment ---
    COMMENT_DOC_SINGLE,  // --# single line doc comment
    COMMENT_DOC_MULTI    // ---# multi-line doc comment ---#
} CommentType;

// Documentation comment structure
typedef struct {
    CommentType type;
    char* content;
    int line;
    struct DocComment* next;
} DocComment;

// Check if current position starts a comment
int is_comment_start(const char* source, int pos);

// Check if current position starts a doc comment
int is_doc_comment_start(const char* source, int pos);

// Skip a comment and return new position
int skip_comment(const char* source, int pos, int* line);

// Extract documentation comment (returns content, caller must free)
char* extract_doc_comment(const char* source, int pos, int* new_pos, int* line);

#endif // COMMENTS_H
