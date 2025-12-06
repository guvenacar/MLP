#ifndef ITERATOR_SYSTEM_H
#define ITERATOR_SYSTEM_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// Iterator types
typedef enum {
    ITER_ARRAY,      // Array iterator
    ITER_RANGE,      // Range iterator (start..end)
    ITER_MAP,        // Map transformation
    ITER_FILTER,     // Filter predicate
    ITER_CHAIN,      // Chain multiple iterators
    ITER_ZIP,        // Zip two iterators
    ITER_ENUMERATE,  // Add index to iterator
    ITER_UNKNOWN
} IteratorType;

// Iterator operations
typedef enum {
    ITER_OP_NEW,         // Create new iterator
    ITER_OP_NEXT,        // Get next element
    ITER_OP_HAS_NEXT,    // Check if has more elements
    ITER_OP_MAP,         // Transform elements
    ITER_OP_FILTER,      // Filter elements
    ITER_OP_REDUCE,      // Reduce to single value
    ITER_OP_COLLECT,     // Collect into collection
    ITER_OP_CHAIN,       // Chain iterators
    ITER_OP_ZIP,         // Zip iterators
    ITER_OP_ENUMERATE,   // Add enumeration
    ITER_OP_TAKE,        // Take first n elements
    ITER_OP_SKIP,        // Skip first n elements
    ITER_OP_FOREACH,     // Execute for each element
    ITER_OP_COUNT,       // Count elements
    ITER_OP_UNKNOWN
} IteratorOp;

// Iterator token
typedef struct {
    IteratorType type;
    char* value;
    int line;
    int column;
} IterToken;

// Iterator declaration
typedef struct {
    char* var_name;
    IteratorType type;
    char* source;        // Source collection/range
    char* transform;     // Transformation function (for map/filter)
    bool is_lazy;        // Lazy evaluation flag
} IteratorDecl;

// Iterator operation node
typedef struct {
    IteratorOp op;
    char* iterator_name;
    char* function;      // Function for map/filter/reduce
    char* args[5];
    int arg_count;
    char* result_var;    // Result variable for collect/reduce
} IteratorOpNode;

// Iterator context
typedef struct {
    IteratorDecl** declarations;
    int decl_count;
    int decl_capacity;
    
    IteratorOpNode** operations;
    int op_count;
    int op_capacity;
    
    char* error_message;
} IteratorContext;

// Lifecycle functions
IteratorContext* iter_context_create(void);
void iter_context_free(IteratorContext* ctx);

// Parser functions
bool iter_parse_declaration(IteratorContext* ctx, const char* input);
bool iter_parse_operation(IteratorContext* ctx, const char* input);
IteratorType iter_get_type(const char* type_str);
IteratorOp iter_get_operation(const char* op_str);

// Codegen functions
void iter_codegen_declaration(FILE* out, IteratorDecl* decl);
void iter_codegen_operation(FILE* out, IteratorOpNode* op);
void iter_codegen_full(IteratorContext* ctx, const char* output_file);

// Utility functions
void iter_add_declaration(IteratorContext* ctx, IteratorDecl* decl);
void iter_add_operation(IteratorContext* ctx, IteratorOpNode* op);
IteratorDecl* iter_find_declaration(IteratorContext* ctx, const char* name);

#endif
