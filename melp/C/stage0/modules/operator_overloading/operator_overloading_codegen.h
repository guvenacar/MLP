#ifndef OPERATOR_OVERLOADING_CODEGEN_H
#define OPERATOR_OVERLOADING_CODEGEN_H

#include "operator_overloading.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_operator_overload(FILE* out, OperatorOverload* overload);
void codegen_operator_dispatch(FILE* out, OperatorDispatch* dispatch);
void codegen_custom_operator(FILE* out, CustomOperator* custom);
void codegen_operator_registry(FILE* out, OperatorRegistry* registry);

// Dispatch mekanizması
void codegen_dispatch_table(FILE* out, OperatorRegistry* registry);
void codegen_dispatch_lookup(FILE* out, OperatorType op);
void codegen_dispatch_call(FILE* out, OperatorOverload* overload);

// Type checking
void codegen_operator_type_check(FILE* out, const char* type);
void codegen_type_match(FILE* out, const char* expected, const char* actual);

// Operator implementation
void codegen_binary_operator(FILE* out, OperatorOverload* overload);
void codegen_unary_operator(FILE* out, OperatorOverload* overload);

// Custom operator
void codegen_custom_precedence(FILE* out, CustomOperator* custom);

#endif // OPERATOR_OVERLOADING_CODEGEN_H
