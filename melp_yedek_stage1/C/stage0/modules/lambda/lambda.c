#include "lambda.h"
#include <stdlib.h>
#include <string.h>

// Lambda oluştur
Lambda* lambda_create(LambdaType type, int param_count) {
    Lambda* lambda = malloc(sizeof(Lambda));
    lambda->type = type;
    lambda->parameters = malloc(sizeof(char*) * param_count);
    lambda->param_count = param_count;
    lambda->body = NULL;
    lambda->captures = NULL;
    lambda->closure_size = 0;
    
    for (int i = 0; i < param_count; i++) {
        lambda->parameters[i] = NULL;
    }
    
    return lambda;
}

// Lambda'ya parametre ekle
void lambda_add_parameter(Lambda* lambda, const char* param) {
    if (!lambda) return;
    
    // İlk boş slot'u bul
    for (int i = 0; i < lambda->param_count; i++) {
        if (lambda->parameters[i] == NULL) {
            lambda->parameters[i] = strdup(param);
            break;
        }
    }
}

// Lambda'ya capture ekle
void lambda_add_capture(Lambda* lambda, const char* var_name, CaptureMode mode) {
    if (!lambda) return;
    
    CapturedVar* var = malloc(sizeof(CapturedVar));
    var->name = strdup(var_name);
    var->mode = mode;
    var->offset = lambda->closure_size;
    var->next = NULL;
    
    // Capture boyutunu güncelle
    if (mode == CAPTURE_BY_VALUE) {
        lambda->closure_size += 8; // Değer kopyası
    } else {
        lambda->closure_size += 8; // Pointer
    }
    
    // Listeye ekle
    if (lambda->captures == NULL) {
        lambda->captures = var;
    } else {
        CapturedVar* current = lambda->captures;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = var;
    }
}

// Lambda çağrısı oluştur
LambdaCall* lambda_create_call(Lambda* lambda, int arg_count) {
    LambdaCall* call = malloc(sizeof(LambdaCall));
    call->lambda = lambda;
    call->arguments = malloc(sizeof(void*) * arg_count);
    call->arg_count = arg_count;
    
    for (int i = 0; i < arg_count; i++) {
        call->arguments[i] = NULL;
    }
    
    return call;
}

// Higher-order function oluştur
HigherOrder* higher_order_create(const char* func_name, Lambda* lambda) {
    HigherOrder* ho = malloc(sizeof(HigherOrder));
    ho->function_name = strdup(func_name);
    ho->lambda = lambda;
    ho->collection = NULL;
    return ho;
}

// Bellek temizleme
void lambda_free(Lambda* lambda) {
    if (!lambda) return;
    
    for (int i = 0; i < lambda->param_count; i++) {
        if (lambda->parameters[i]) {
            free(lambda->parameters[i]);
        }
    }
    free(lambda->parameters);
    
    CapturedVar* current = lambda->captures;
    while (current != NULL) {
        CapturedVar* next = current->next;
        captured_var_free(current);
        current = next;
    }
    
    free(lambda);
}

void lambda_call_free(LambdaCall* call) {
    if (!call) return;
    free(call->arguments);
    free(call);
}

void higher_order_free(HigherOrder* ho) {
    if (!ho) return;
    free(ho->function_name);
    free(ho);
}

void captured_var_free(CapturedVar* var) {
    if (!var) return;
    free(var->name);
    free(var);
}
