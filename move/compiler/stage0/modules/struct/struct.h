#ifndef STRUCT_H
#define STRUCT_H

#include <stddef.h>
#include "../../lexer.h"

// Yapı üyesi tanımı
typedef struct StructMember {
    char* name;                    // Üye adı
    char* type;                    // Üye tipi (numeric, string, boolean, başka struct)
    size_t offset;                 // Yapı içindeki offset (byte cinsinden)
    size_t size;                   // Üye boyutu
    struct StructMember* next;     // Sonraki üye (linked list)
} StructMember;

// Yapı tanımı
typedef struct StructDef {
    char* name;                    // Yapı adı
    StructMember* members;         // Üye listesi
    size_t total_size;             // Toplam yapı boyutu
    size_t alignment;              // Hizalama gereksinimi (8 byte)
} StructDef;

// Yapı örneği (instantiation)
typedef struct StructInstance {
    char* struct_name;             // Hangi yapıdan türetildi
    char* instance_name;           // Örnek değişken adı
    StructDef* definition;         // Yapı tanımına referans
} StructInstance;

// Üye erişimi
typedef struct MemberAccess {
    char* struct_instance;         // Yapı örneği adı
    char* member_name;             // Erişilen üye adı
    StructMember* member;          // Üye tanımına referans
} MemberAccess;

// API fonksiyonları
StructDef* struct_create_definition(const char* name);
void struct_add_member(StructDef* def, const char* name, const char* type);
void struct_calculate_layout(StructDef* def);
StructInstance* struct_create_instance(StructDef* def, const char* instance_name);
MemberAccess* struct_create_member_access(const char* instance, const char* member);

void struct_def_free(StructDef* def);
void struct_instance_free(StructInstance* inst);
void struct_member_access_free(MemberAccess* access);

#endif // STRUCT_H
