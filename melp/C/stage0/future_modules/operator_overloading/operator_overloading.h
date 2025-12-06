#ifndef OPERATOR_OVERLOADING_H
#define OPERATOR_OVERLOADING_H

#include <stddef.h>
#include "../../lexer.h"

// Operator tipi
typedef enum OperatorType {
    OP_ADD,             // +
    OP_SUB,             // -
    OP_MUL,             // *
    OP_DIV,             // /
    OP_MOD,             // %
    OP_EQ,              // ==
    OP_NE,              // !=
    OP_LT,              // <
    OP_LE,              // <=
    OP_GT,              // >
    OP_GE,              // >=
    OP_INDEX,           // []
    OP_CALL,            // ()
    OP_ASSIGN,          // =
    OP_UNARY_MINUS,     // -x
    OP_UNARY_PLUS,      // +x
    OP_NOT,             // !
    OP_BIT_AND,         // &
    OP_BIT_OR,          // |
    OP_BIT_XOR,         // ^
    OP_BIT_NOT,         // ~
    OP_LSHIFT,          // <<
    OP_RSHIFT,          // >>
    OP_INCR,            // ++
    OP_DECR             // --
} OperatorType;

// Operator overload tanımı
typedef struct OperatorOverload {
    OperatorType op;        // Operator tipi
    char* type_name;        // Uygulandığı tip
    void* implementation;   // Implementation fonksiyonu
    int is_binary;          // Binary mi unary mi?
    char* param_types[2];   // Parametre tipleri
    char* return_type;      // Dönüş tipi
} OperatorOverload;

// Operator registry (tüm overloadlar)
typedef struct OperatorRegistry {
    OperatorOverload** overloads;   // Overload listesi
    int count;                      // Sayı
    int capacity;                   // Kapasite
} OperatorRegistry;

// Operator dispatch (runtime lookup)
typedef struct OperatorDispatch {
    OperatorType op;        // Operator
    void* left_operand;     // Sol operand
    void* right_operand;    // Sağ operand (binary için)
    char* left_type;        // Sol operand tipi
    char* right_type;       // Sağ operand tipi
    void* result;           // Sonuç
} OperatorDispatch;

// Custom operator tanımı
typedef struct CustomOperator {
    char* symbol;           // Operator sembolü (örn: "@@")
    int precedence;         // Öncelik
    int is_left_assoc;      // Sol associative mi?
    void* implementation;   // Implementation
} CustomOperator;

// API fonksiyonları
OperatorOverload* operator_overload_create(OperatorType op, const char* type_name);
OperatorRegistry* operator_registry_create(void);
OperatorDispatch* operator_dispatch_create(OperatorType op);
CustomOperator* custom_operator_create(const char* symbol, int precedence);

void operator_overload_free(OperatorOverload* overload);
void operator_registry_free(OperatorRegistry* registry);
void operator_dispatch_free(OperatorDispatch* dispatch);
void custom_operator_free(CustomOperator* custom);

// Operator overload operasyonları
void operator_overload_set_params(OperatorOverload* overload, 
                                  const char* param1, const char* param2);
void operator_overload_set_return(OperatorOverload* overload, 
                                  const char* return_type);
void operator_overload_set_impl(OperatorOverload* overload, void* impl);

// Registry operasyonları
void registry_add_overload(OperatorRegistry* registry, OperatorOverload* overload);
OperatorOverload* registry_find_overload(OperatorRegistry* registry, 
                                         OperatorType op, 
                                         const char* left_type,
                                         const char* right_type);

// Dispatch operasyonları
void* dispatch_operator(OperatorDispatch* dispatch, OperatorRegistry* registry);
int can_dispatch(OperatorDispatch* dispatch, OperatorRegistry* registry);

// Custom operator operasyonları
void custom_operator_set_impl(CustomOperator* custom, void* impl);

// Operator info
const char* operator_type_to_string(OperatorType op);
int operator_is_binary(OperatorType op);

#endif // OPERATOR_OVERLOADING_H
