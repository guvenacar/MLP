#include <stdlib.h>
#include <string.h>
#include "functions.h"

FunctionDeclaration* function_create(const char* name, FunctionReturnType return_type) {
    FunctionDeclaration* func = malloc(sizeof(FunctionDeclaration));
    func->name = strdup(name);
    func->params = NULL;
    func->return_type = return_type;
    func->body = NULL;
    func->param_count = 0;
    func->local_var_count = 0;
    return func;
}

void function_add_param(FunctionDeclaration* func, const char* name, FunctionParamType type) {
    FunctionParam* param = malloc(sizeof(FunctionParam));
    param->name = strdup(name);
    param->type = type;
    param->next = NULL;
    
    if (func->params == NULL) {
        func->params = param;
    } else {
        FunctionParam* curr = func->params;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = param;
    }
    func->param_count++;
}

void function_set_body(FunctionDeclaration* func, Statement* body) {
    func->body = body;
}

void function_free(FunctionDeclaration* func) {
    if (!func) return;
    
    free(func->name);
    
    FunctionParam* param = func->params;
    while (param) {
        FunctionParam* next = param->next;
        free(param->name);
        free(param);
        param = next;
    }
    
    // Note: body statements freed elsewhere in AST cleanup
    free(func);
}

FunctionCall* function_call_create(const char* name) {
    FunctionCall* call = malloc(sizeof(FunctionCall));
    call->function_name = strdup(name);
    call->arguments = NULL;
    call->arg_count = 0;
    return call;
}

void function_call_add_arg(FunctionCall* call, struct Expression* arg) {
    call->arguments = realloc(call->arguments, 
                             sizeof(struct Expression*) * (call->arg_count + 1));
    call->arguments[call->arg_count] = arg;
    call->arg_count++;
}

void function_call_free(FunctionCall* call) {
    if (!call) return;
    
    free(call->function_name);
    
    // Note: arguments freed elsewhere in AST cleanup
    free(call->arguments);
    free(call);
}

ReturnStatement* return_statement_create(struct Expression* value) {
    ReturnStatement* ret = malloc(sizeof(ReturnStatement));
    ret->return_value = value;
    return ret;
}

void return_statement_free(ReturnStatement* ret) {
    if (!ret) return;
    // Note: return_value freed elsewhere in AST cleanup
    free(ret);
}
