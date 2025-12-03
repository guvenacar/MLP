#ifndef EXCEPTION_HANDLING_PARSER_H
#define EXCEPTION_HANDLING_PARSER_H

#include "exception_handling.h"
#include "../../lexer.h"
#include "../parser_core/parser_core.h"

// Parser fonksiyonları - Stage 1'de implement edilecek
TryCatchFinally* parse_try_catch_finally(Lexer* lexer);
CatchBlock* parse_catch(Lexer* lexer);
Throw* parse_throw(Lexer* lexer);
Exception* parse_exception_type(Lexer* lexer);

#endif // EXCEPTION_HANDLING_PARSER_H
