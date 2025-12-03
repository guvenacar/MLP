#ifndef STRING_OPS_H
#define STRING_OPS_H

// String operations
typedef enum {
    STR_OP_CONCAT,      // a + b (string concatenation)
    STR_OP_LENGTH,      // length(s)
    STR_OP_SUBSTR,      // substr(s, start, len)
    STR_OP_SPLIT,       // split(s, delimiter)
    STR_OP_REPLACE,     // replace(s, old, new)
    STR_OP_TRIM,        // trim(s)
    STR_OP_UPPER,       // upper(s)
    STR_OP_LOWER,       // lower(s)
    STR_OP_CONTAINS,    // contains(s, substring)
    STR_OP_INDEXOF      // indexof(s, substring)
} StringOp;

// String operation expression
typedef struct {
    StringOp op;
    struct Expression* left;    // First operand
    struct Expression* right;   // Second operand (NULL for unary ops)
    struct Expression* third;   // Third operand (for substr, replace)
} StringOpExpr;

#endif
