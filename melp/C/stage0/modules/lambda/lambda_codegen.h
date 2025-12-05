#ifndef LAMBDA_CODEGEN_H
#define LAMBDA_CODEGEN_H

#include "lambda.h"
#include <stdio.h>

// Code generation fonksiyonları
void codegen_lambda(FILE* out, Lambda* lambda);
void codegen_lambda_call(FILE* out, LambdaCall* call);
void codegen_closure_struct(FILE* out, Lambda* lambda);
void codegen_closure_capture(FILE* out, Lambda* lambda);
void codegen_higher_order(FILE* out, HigherOrder* ho);

// Yardımcı fonksiyonlar
void codegen_lambda_function(FILE* out, Lambda* lambda, int lambda_id);
void codegen_capture_access(FILE* out, CapturedVar* var);
const char* get_lambda_mangled_name(Lambda* lambda, int lambda_id);

#endif // LAMBDA_CODEGEN_H
