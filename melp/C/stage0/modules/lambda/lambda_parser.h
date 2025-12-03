#ifndef LAMBDA_PARSER_H
#define LAMBDA_PARSER_H

#include "lambda.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
Lambda* parse_lambda(Lexer* lexer);
LambdaCall* parse_lambda_call(Lexer* lexer);
HigherOrder* parse_higher_order(Lexer* lexer);
CapturedVar* parse_capture_list(Lexer* lexer);

#endif // LAMBDA_PARSER_H
