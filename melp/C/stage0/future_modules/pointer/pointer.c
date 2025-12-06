#include "pointer.h"
#include <stdlib.h>
#include <string.h>

// Pointer oluştur
Pointer* pointer_create(const char* name, PointerType type, const char* target_type) {
    Pointer* ptr = malloc(sizeof(Pointer));
    ptr->name = strdup(name);
    ptr->type = type;
    ptr->target_type = target_type ? strdup(target_type) : NULL;
    ptr->is_null = 0;
    
    // Hedef boyutunu belirle
    switch (type) {
        case PTR_NUMERIC:
            ptr->pointee_size = 8;
            break;
        case PTR_STRING:
            ptr->pointee_size = 24; // SSO struct
            break;
        case PTR_BOOLEAN:
            ptr->pointee_size = 1;
            break;
        case PTR_STRUCT:
            ptr->pointee_size = 0; // Runtime'da hesaplanacak
            break;
        case PTR_VOID:
            ptr->pointee_size = 1; // Byte olarak işle
            break;
    }
    
    return ptr;
}

// Adres alma operasyonu oluştur
AddressOf* pointer_create_address_of(const char* var_name) {
    AddressOf* addr = malloc(sizeof(AddressOf));
    addr->variable_name = strdup(var_name);
    addr->result = NULL; // Parser tarafından doldurulacak
    return addr;
}

// Dereference operasyonu oluştur
Dereference* pointer_create_deref(const char* ptr_name) {
    Dereference* deref = malloc(sizeof(Dereference));
    deref->pointer_name = strdup(ptr_name);
    deref->value = NULL; // Runtime'da hesaplanacak
    return deref;
}

// Pointer aritmetiği oluştur
PointerArith* pointer_create_arith(const char* ptr_name, PtrArithOp op, long offset) {
    PointerArith* arith = malloc(sizeof(PointerArith));
    arith->pointer_name = strdup(ptr_name);
    arith->op = op;
    arith->offset = offset;
    arith->other_ptr = NULL;
    return arith;
}

// Bellek temizleme
void pointer_free(Pointer* ptr) {
    if (!ptr) return;
    free(ptr->name);
    if (ptr->target_type) free(ptr->target_type);
    free(ptr);
}

void address_of_free(AddressOf* addr) {
    if (!addr) return;
    free(addr->variable_name);
    if (addr->result) pointer_free(addr->result);
    free(addr);
}

void dereference_free(Dereference* deref) {
    if (!deref) return;
    free(deref->pointer_name);
    free(deref);
}

void pointer_arith_free(PointerArith* arith) {
    if (!arith) return;
    free(arith->pointer_name);
    if (arith->other_ptr) free(arith->other_ptr);
    free(arith);
}
