#ifndef SMART_POINTERS_H
#define SMART_POINTERS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

// Smart pointer types
typedef enum {
    SP_BOX,      // Box<T> - unique ownership
    SP_RC,       // Rc<T> - reference counted
    SP_ARC,      // Arc<T> - atomic reference counted
    SP_WEAK,     // Weak<T> - weak reference
    SP_UNKNOWN
} SmartPtrType;

// Smart pointer operations
typedef enum {
    SP_OP_NEW,        // Box::new(value)
    SP_OP_RC_NEW,     // Rc::new(value)
    SP_OP_ARC_NEW,    // Arc::new(value)
    SP_OP_CLONE,      // ptr.clone()
    SP_OP_DOWNGRADE,  // Rc::downgrade()
    SP_OP_UPGRADE,    // weak.upgrade()
    SP_OP_DEREF,      // *ptr
    SP_OP_DROP,       // Drop trait
    SP_OP_STRONG_COUNT, // Rc::strong_count()
    SP_OP_WEAK_COUNT,   // Rc::weak_count()
    SP_OP_UNKNOWN
} SmartPtrOp;

// Token structure
typedef struct {
    char* value;
    int line;
    int column;
} SPToken;

// Smart pointer declaration
typedef struct {
    SmartPtrType type;
    char* var_name;
    char* inner_type;  // T in Box<T>
    char* init_value;
} SmartPtrDecl;

// Smart pointer operation node
typedef struct {
    SmartPtrOp op;
    char* ptr_name;
    char* args[3];
    int arg_count;
} SmartPtrOpNode;

// Context for smart pointers
typedef struct {
    SmartPtrDecl** declarations;
    int decl_count;
    int decl_capacity;
    
    SmartPtrOpNode** operations;
    int op_count;
    int op_capacity;
    
    char* error_message;
} SmartPtrContext;

// Lifecycle functions
SmartPtrContext* sp_context_create(void);
void sp_context_free(SmartPtrContext* ctx);

// Parser functions
bool sp_parse_declaration(SmartPtrContext* ctx, const char* input);
bool sp_parse_operation(SmartPtrContext* ctx, const char* input);
SmartPtrType sp_get_type(const char* type_str);
SmartPtrOp sp_get_operation(const char* op_str);

// Codegen functions
void sp_codegen_declaration(FILE* out, SmartPtrDecl* decl);
void sp_codegen_operation(FILE* out, SmartPtrOpNode* op);
void sp_codegen_full(SmartPtrContext* ctx, const char* output_file);

// Utility functions
void sp_add_declaration(SmartPtrContext* ctx, SmartPtrDecl* decl);
void sp_add_operation(SmartPtrContext* ctx, SmartPtrOpNode* op);
SmartPtrDecl* sp_find_declaration(SmartPtrContext* ctx, const char* name);

#endif // SMART_POINTERS_H
