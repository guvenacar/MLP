#ifndef POINTER_H
#define POINTER_H

#include <stddef.h>
#include "../../lexer.h"

// Pointer tipi
typedef enum PointerType {
    PTR_NUMERIC,      // numeric* (8 byte)
    PTR_STRING,       // string* (24 byte struct)
    PTR_BOOLEAN,      // boolean* (1 byte)
    PTR_STRUCT,       // StructName* (değişken boyut)
    PTR_VOID          // void* (generic pointer)
} PointerType;

// Pointer tanımı
typedef struct Pointer {
    char* name;              // Pointer değişken adı
    PointerType type;        // Pointer tipi
    char* target_type;       // Hedef tip adı (struct için)
    int is_null;             // NULL mı?
    size_t pointee_size;     // İşaret edilen değerin boyutu
} Pointer;

// Adres alma operasyonu (&)
typedef struct AddressOf {
    char* variable_name;     // Adresi alınacak değişken
    Pointer* result;         // Sonuç pointer
} AddressOf;

// Dereference operasyonu (*)
typedef struct Dereference {
    char* pointer_name;      // Dereference edilecek pointer
    void* value;             // Sonuç değer
} Dereference;

// Pointer aritmetiği
typedef enum PtrArithOp {
    PTR_ADD,                 // ptr + n
    PTR_SUB,                 // ptr - n
    PTR_DIFF                 // ptr1 - ptr2
} PtrArithOp;

typedef struct PointerArith {
    char* pointer_name;      // İşlem yapılacak pointer
    PtrArithOp op;          // Operasyon tipi
    long offset;            // Offset (eleman sayısı)
    char* other_ptr;        // İkinci pointer (PTR_DIFF için)
} PointerArith;

// API fonksiyonları
Pointer* pointer_create(const char* name, PointerType type, const char* target_type);
AddressOf* pointer_create_address_of(const char* var_name);
Dereference* pointer_create_deref(const char* ptr_name);
PointerArith* pointer_create_arith(const char* ptr_name, PtrArithOp op, long offset);

void pointer_free(Pointer* ptr);
void address_of_free(AddressOf* addr);
void dereference_free(Dereference* deref);
void pointer_arith_free(PointerArith* arith);

#endif // POINTER_H
