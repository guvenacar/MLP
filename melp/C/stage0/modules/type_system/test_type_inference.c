#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "type_system.h"
#include "type_system_parser.h"
#include "../../lexer.h"

void test_infer_integer() {
    printf("Test 1: Integer inference... ");
    
    const char* code = "42";
    Lexer* lexer = lexer_create(code);
    TypeContext* ctx = type_context_create();
    
    Type* type = parse_infer_type(lexer, ctx);
    
    assert(type != NULL);
    assert(type->kind == TYPE_INT);
    
    printf("✅ PASSED\n");
    
    type_free(type);
    type_context_destroy(ctx);
    lexer_free(lexer);
}

void test_infer_float() {
    printf("Test 2: Float inference... ");
    
    const char* code = "3.14";
    Lexer* lexer = lexer_create(code);
    TypeContext* ctx = type_context_create();
    
    Type* type = parse_infer_type(lexer, ctx);
    
    assert(type != NULL);
    assert(type->kind == TYPE_FLOAT);
    
    printf("✅ PASSED\n");
    
    type_free(type);
    type_context_destroy(ctx);
    lexer_free(lexer);
}

void test_infer_string() {
    printf("Test 3: String inference... ");
    
    const char* code = "\"hello\"";
    Lexer* lexer = lexer_create(code);
    TypeContext* ctx = type_context_create();
    
    Type* type = parse_infer_type(lexer, ctx);
    
    assert(type != NULL);
    assert(type->kind == TYPE_STRING);
    
    printf("✅ PASSED\n");
    
    type_free(type);
    type_context_destroy(ctx);
    lexer_free(lexer);
}

void test_infer_boolean() {
    printf("Test 4: Boolean inference... ");
    
    const char* code = "true";
    Lexer* lexer = lexer_create(code);
    TypeContext* ctx = type_context_create();
    
    Type* type = parse_infer_type(lexer, ctx);
    
    assert(type != NULL);
    if (type->kind != TYPE_BOOL) {
        printf("\n❌ FAILED: Expected TYPE_BOOL(%d) but got %d\n", TYPE_BOOL, type->kind);
        exit(1);
    }
    
    printf("✅ PASSED\n");
    
    type_free(type);
    type_context_destroy(ctx);
    lexer_free(lexer);
}

void test_infer_array() {
    printf("Test 5: Array inference... ");
    
    const char* code = "[1, 2, 3]";
    Lexer* lexer = lexer_create(code);
    TypeContext* ctx = type_context_create();
    
    Type* type = parse_infer_type(lexer, ctx);
    
    assert(type != NULL);
    assert(type->kind == TYPE_ARRAY);
    assert(type->element_type != NULL);
    assert(type->element_type->kind == TYPE_INT);
    assert(type->array_size == 3);
    
    printf("✅ PASSED\n");
    
    type_free(type);
    type_context_destroy(ctx);
    lexer_free(lexer);
}

int main() {
    printf("\n🧪 Type Inference Unit Tests\n");
    printf("============================\n\n");
    
    test_infer_integer();
    test_infer_float();
    test_infer_string();
    test_infer_boolean();
    test_infer_array();
    
    printf("\n✅ All tests passed!\n\n");
    
    return 0;
}
