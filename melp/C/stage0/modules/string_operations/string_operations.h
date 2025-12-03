#ifndef STRING_OPERATIONS_H
#define STRING_OPERATIONS_H

typedef enum {
    STRING_OP_CONCAT,      // string1 + string2
    STRING_OP_LENGTH,      // length(string)
    STRING_OP_SUBSTRING,   // substring(string, start, end)
    STRING_OP_COMPARE,     // string1 == string2
    STRING_OP_INDEXOF,     // indexOf(string, substring)
    STRING_OP_CHARAT,      // charAt(string, index)
    STRING_OP_UPPER,       // toUpper(string)
    STRING_OP_LOWER        // toLower(string)
} StringOperationType;

typedef struct {
    StringOperationType op_type;
    void* operand1;        // First operand (expression or string)
    void* operand2;        // Second operand (for binary ops)
    void* operand3;        // Third operand (for substring)
} StringOperation;

// Create string operations
StringOperation* string_op_create(StringOperationType type);
void string_op_set_operand1(StringOperation* op, void* operand);
void string_op_set_operand2(StringOperation* op, void* operand);
void string_op_set_operand3(StringOperation* op, void* operand);
void string_op_free(StringOperation* op);

#endif
