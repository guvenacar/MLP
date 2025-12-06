#ifndef OPERATOR_OVERLOADING_PARSER_H
#define OPERATOR_OVERLOADING_PARSER_H

#include "operator_overloading.h"
#include "../../lexer.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
OperatorOverload* parse_operator_overload(Lexer* lexer);
CustomOperator* parse_custom_operator(Lexer* lexer);
OperatorType parse_operator_symbol(Lexer* lexer);

#endif // OPERATOR_OVERLOADING_PARSER_H
