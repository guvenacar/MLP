#include "struct.h"
#include <stdlib.h>
#include <string.h>

// Yapı tanımı oluştur
StructDef* struct_create_definition(const char* name) {
    StructDef* def = malloc(sizeof(StructDef));
    def->name = strdup(name);
    def->members = NULL;
    def->total_size = 0;
    def->alignment = 8; // x86-64 default hizalama
    return def;
}

// Yapıya üye ekle
void struct_add_member(StructDef* def, const char* name, const char* type) {
    StructMember* member = malloc(sizeof(StructMember));
    member->name = strdup(name);
    member->type = strdup(type);
    member->next = NULL;
    
    // Tip boyutlarını belirle
    if (strcmp(type, "numeric") == 0) {
        member->size = 8; // INT64 veya DOUBLE
    } else if (strcmp(type, "string") == 0) {
        member->size = 24; // SSO struct boyutu
    } else if (strcmp(type, "boolean") == 0) {
        member->size = 1; // bool
    } else {
        member->size = 8; // Pointer boyutu (başka struct için)
    }
    
    // Listeye ekle
    if (def->members == NULL) {
        def->members = member;
    } else {
        StructMember* current = def->members;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = member;
    }
}

// Yapı layout'unu hesapla (offsetler ve toplam boyut)
void struct_calculate_layout(StructDef* def) {
    size_t offset = 0;
    StructMember* current = def->members;
    
    while (current != NULL) {
        // Hizalama uygula
        size_t align = (current->size < 8) ? current->size : 8;
        if (offset % align != 0) {
            offset += align - (offset % align);
        }
        
        current->offset = offset;
        offset += current->size;
        current = current->next;
    }
    
    // Toplam boyutu hizala
    if (offset % def->alignment != 0) {
        offset += def->alignment - (offset % def->alignment);
    }
    
    def->total_size = offset;
}

// Yapı örneği oluştur
StructInstance* struct_create_instance(StructDef* def, const char* instance_name) {
    StructInstance* inst = malloc(sizeof(StructInstance));
    inst->struct_name = strdup(def->name);
    inst->instance_name = strdup(instance_name);
    inst->definition = def;
    return inst;
}

// Üye erişimi oluştur
MemberAccess* struct_create_member_access(const char* instance, const char* member) {
    MemberAccess* access = malloc(sizeof(MemberAccess));
    access->struct_instance = strdup(instance);
    access->member_name = strdup(member);
    access->member = NULL; // Parser tarafından doldurulacak
    return access;
}

// Bellek temizleme
void struct_def_free(StructDef* def) {
    if (!def) return;
    
    free(def->name);
    
    StructMember* current = def->members;
    while (current != NULL) {
        StructMember* next = current->next;
        free(current->name);
        free(current->type);
        free(current);
        current = next;
    }
    
    free(def);
}

void struct_instance_free(StructInstance* inst) {
    if (!inst) return;
    free(inst->struct_name);
    free(inst->instance_name);
    free(inst);
}

void struct_member_access_free(MemberAccess* access) {
    if (!access) return;
    free(access->struct_instance);
    free(access->member_name);
    free(access);
}
