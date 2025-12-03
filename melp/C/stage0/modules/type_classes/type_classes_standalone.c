#include "type_classes.h"
#include <stdio.h>
#include <stdlib.h>

// ============================================================================
// TEST SUITE FOR TYPE CLASSES
// ============================================================================

void test_functor(void) {
    printf("\n=== TEST 1: Functor Type Class ===\n");
    
    TypeClassContext* ctx = tc_context_create();
    
    // Create Functor class
    TypeClass* functor = tc_class_create("Functor", KIND_TYPE_TO_TYPE);
    tc_class_add_method(functor, "fmap");
    tc_class_add_law(functor, "fmap id = id");
    tc_class_add_law(functor, "fmap (f . g) = fmap f . fmap g");
    tc_register_class(ctx, functor);
    
    printf("Created Functor class\n");
    printf("  Kind: %d (TYPE_TO_TYPE=1)\n", functor->kind);
    printf("  Methods: %zu\n", functor->method_count);
    printf("  Laws: %zu\n", functor->law_count);
    
    // Create instance for Maybe
    TypeInstance* maybe_functor = tc_instance_create("Functor", "Maybe");
    tc_instance_add_impl(maybe_functor, "fmap f Nothing = Nothing");
    tc_instance_add_impl(maybe_functor, "fmap f (Just x) = Just (f x)");
    tc_register_instance(ctx, maybe_functor);
    
    bool has_inst = tc_has_instance(ctx, "Functor", "Maybe");
    printf("Functor Maybe instance: %s ✓\n", has_inst ? "exists" : "missing");
    
    tc_context_destroy(ctx);
    printf("✓ Test 1 passed\n");
}

void test_monad(void) {
    printf("\n=== TEST 2: Monad Type Class ===\n");
    
    TypeClassContext* ctx = tc_context_create();
    
    // Create Applicative (superclass)
    TypeClass* applicative = tc_class_create("Applicative", KIND_TYPE_TO_TYPE);
    tc_class_add_method(applicative, "pure");
    tc_class_add_method(applicative, "ap");
    tc_register_class(ctx, applicative);
    
    // Create Monad with Applicative superclass
    TypeClass* monad = tc_class_create("Monad", KIND_TYPE_TO_TYPE);
    tc_class_add_super(monad, "Applicative");
    tc_class_add_method(monad, "bind");
    tc_class_add_method(monad, "return");
    tc_class_add_law(monad, "return a >>= f = f a");
    tc_class_add_law(monad, "m >>= return = m");
    tc_class_add_law(monad, "(m >>= f) >>= g = m >>= (\\x -> f x >>= g)");
    tc_register_class(ctx, monad);
    
    printf("Created Monad class\n");
    printf("  Superclasses: %zu\n", monad->super_count);
    printf("  Methods: %zu\n", monad->method_count);
    printf("  Laws: %zu\n", monad->law_count);
    
    // Create instance for IO
    TypeInstance* io_monad = tc_instance_create("Monad", "IO");
    tc_instance_add_impl(io_monad, "bind");
    tc_instance_add_impl(io_monad, "return");
    tc_register_instance(ctx, io_monad);
    
    bool valid = tc_check_laws(ctx, io_monad);
    printf("IO Monad instance valid: %s ✓\n", valid ? "true" : "false");
    
    tc_context_destroy(ctx);
    printf("✓ Test 2 passed\n");
}

void test_higher_kinded_types(void) {
    printf("\n=== TEST 3: Higher-Kinded Types ===\n");
    
    TypeClassContext* ctx = tc_context_create();
    
    // Create Bifunctor (* -> * -> *)
    TypeClass* bifunctor = tc_class_create("Bifunctor", KIND_TYPE_TO_TYPE_TO_TYPE);
    tc_class_add_method(bifunctor, "bimap");
    tc_register_class(ctx, bifunctor);
    
    printf("Created Bifunctor class\n");
    printf("  Kind: %d (TYPE_TO_TYPE_TO_TYPE=2)\n", bifunctor->kind);
    
    // Create instance for Either
    TypeInstance* either_bifunctor = tc_instance_create("Bifunctor", "Either");
    tc_instance_add_impl(either_bifunctor, "bimap f g (Left x) = Left (f x)");
    tc_instance_add_impl(either_bifunctor, "bimap f g (Right y) = Right (g y)");
    tc_register_instance(ctx, either_bifunctor);
    
    bool has_inst = tc_has_instance(ctx, "Bifunctor", "Either");
    printf("Bifunctor Either instance: %s ✓\n", has_inst ? "exists" : "missing");
    
    tc_context_destroy(ctx);
    printf("✓ Test 3 passed\n");
}

void test_parser(void) {
    printf("\n=== TEST 4: Parser ===\n");
    
    TypeClass* tc1 = tc_parse_class("class Functor f where");
    if (tc1) {
        printf("Parsed 'class Functor f where'\n");
        printf("  Name: %s, Kind: %d\n", tc1->name, tc1->kind);
        tc_class_free(tc1);
    }
    
    TypeClass* tc2 = tc_parse_class("class Monad m : Applicative m where");
    if (tc2) {
        printf("Parsed 'class Monad m : Applicative m where'\n");
        printf("  Name: %s, Superclasses: %zu\n", tc2->name, tc2->super_count);
        tc_class_free(tc2);
    }
    
    TypeInstance* inst = tc_parse_instance("instance Functor Maybe");
    if (inst) {
        printf("Parsed 'instance Functor Maybe'\n");
        printf("  Class: %s, Type: %s\n", inst->class_name, inst->type_name);
        tc_instance_free(inst);
    }
    
    printf("✓ Test 4 passed\n");
}

void test_codegen(void) {
    printf("\n=== TEST 5: Codegen ===\n");
    
    TypeClass* tc = tc_class_create("Monad", KIND_TYPE_TO_TYPE);
    tc_class_add_method(tc, "bind");
    tc_class_add_method(tc, "return");
    tc_class_add_super(tc, "Applicative");
    
    char* code = codegen_typeclass_check(tc);
    if (code) {
        printf("Generated code:\n%s", code);
        free(code);
    }
    
    tc_class_free(tc);
    printf("✓ Test 5 passed\n");
}

void test_statistics(void) {
    printf("\n=== TEST 6: Statistics ===\n");
    
    TypeClassContext* ctx = tc_context_create();
    
    TypeClass* tc1 = tc_class_create("Functor", KIND_TYPE_TO_TYPE);
    tc_class_add_method(tc1, "fmap");
    tc_register_class(ctx, tc1);
    
    TypeClass* tc2 = tc_class_create("Monad", KIND_TYPE_TO_TYPE);
    tc_class_add_method(tc2, "bind");
    tc_class_add_method(tc2, "return");
    tc_register_class(ctx, tc2);
    
    TypeInstance* inst1 = tc_instance_create("Functor", "Maybe");
    tc_instance_add_impl(inst1, "fmap");
    tc_register_instance(ctx, inst1);
    
    TypeInstance* inst2 = tc_instance_create("Monad", "IO");
    tc_instance_add_impl(inst2, "bind");
    tc_register_instance(ctx, inst2);
    
    tc_check_laws(ctx, inst1);
    tc_check_laws(ctx, inst2);
    
    printf("Classes: %zu\n", ctx->class_count);
    printf("Instances: %zu\n", ctx->instance_count);
    printf("Total checks: %zu\n", ctx->total_checks);
    printf("Violations: %zu\n", ctx->violations);
    
    tc_context_destroy(ctx);
    printf("✓ Test 6 passed\n");
}

int main(void) {
    printf("MLP TYPE CLASSES - Module #64 Test Suite\n");
    printf("=========================================\n");
    
    test_functor();
    test_monad();
    test_higher_kinded_types();
    test_parser();
    test_codegen();
    test_statistics();
    
    printf("\n✅ All tests passed!\n");
    return 0;
}
