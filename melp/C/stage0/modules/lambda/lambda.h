#ifndef LAMBDA_H
#define LAMBDA_H

#include <stddef.h>
#include "../../lexer.h"

// Lambda/Closure tipi
typedef enum LambdaType {
    LAMBDA_SIMPLE,      // Basit lambda (capture yok)
    LAMBDA_CLOSURE      // Closure (capture var)
} LambdaType;

// Capture tipi
typedef enum CaptureMode {
    CAPTURE_BY_VALUE,   // Değer ile capture
    CAPTURE_BY_REF      // Referans ile capture
} CaptureMode;

// Capture edilmiş değişken
typedef struct CapturedVar {
    char* name;                 // Değişken adı
    CaptureMode mode;           // Capture modu
    size_t offset;              // Closure struct'taki offset
    struct CapturedVar* next;   // Sonraki captured var
} CapturedVar;

// Lambda tanımı
typedef struct Lambda {
    LambdaType type;            // Lambda tipi
    char** parameters;          // Parametre isimleri
    int param_count;            // Parametre sayısı
    void* body;                 // Lambda gövdesi
    CapturedVar* captures;      // Capture edilmiş değişkenler
    size_t closure_size;        // Closure struct boyutu
} Lambda;

// Lambda çağrısı
typedef struct LambdaCall {
    Lambda* lambda;             // Çağrılacak lambda
    void** arguments;           // Argümanlar
    int arg_count;              // Argüman sayısı
} LambdaCall;

// Higher-order function
typedef struct HigherOrder {
    char* function_name;        // map, filter, reduce vb.
    Lambda* lambda;             // Uygulanacak lambda
    void* collection;           // İşlenecek koleksiyon
} HigherOrder;

// API fonksiyonları
Lambda* lambda_create(LambdaType type, int param_count);
void lambda_add_parameter(Lambda* lambda, const char* param);
void lambda_add_capture(Lambda* lambda, const char* var_name, CaptureMode mode);
LambdaCall* lambda_create_call(Lambda* lambda, int arg_count);
HigherOrder* higher_order_create(const char* func_name, Lambda* lambda);

void lambda_free(Lambda* lambda);
void lambda_call_free(LambdaCall* call);
void higher_order_free(HigherOrder* ho);
void captured_var_free(CapturedVar* var);

#endif // LAMBDA_H
