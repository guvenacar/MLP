#include "struct_codegen.h"
#include <string.h>

// Yapı tanımı için kod üret (sadece belge amaçlı, bellekte değil)
void codegen_struct_definition(FILE* out, StructDef* def) {
    fprintf(out, "    ; Struct tanımı: %s (toplam: %zu byte)\n", 
            def->name, def->total_size);
    
    StructMember* member = def->members;
    while (member != NULL) {
        fprintf(out, "    ;   %s: %s (offset: %zu, boyut: %zu)\n",
                member->name, member->type, member->offset, member->size);
        member = member->next;
    }
}

// Yapı için bellek ayır (stack veya heap)
void codegen_struct_allocation(FILE* out, StructInstance* inst) {
    StructDef* def = inst->definition;
    
    fprintf(out, "    ; Struct örneği: %s (tip: %s)\n", 
            inst->instance_name, inst->struct_name);
    
    // Stack üzerinde yer ayır
    fprintf(out, "    sub rsp, %zu    ; %s için alan\n", 
            def->total_size, inst->instance_name);
    
    // Struct base pointer'ı sakla (rbp - offset)
    fprintf(out, "    lea rax, [rbp - %zu]\n", def->total_size);
    fprintf(out, "    mov [rbp - %zu], rax    ; %s base pointer\n",
            def->total_size + 8, inst->instance_name);
}

// Üye erişimi için kod üret
void codegen_member_access(FILE* out, MemberAccess* access) {
    if (!access->member) return;
    
    fprintf(out, "    ; Üye erişimi: %s.%s\n", 
            access->struct_instance, access->member_name);
    
    // Base pointer'ı yükle
    fprintf(out, "    mov rax, [rbp - 8]    ; %s base\n", 
            access->struct_instance);
    
    // Offset ekle
    fprintf(out, "    add rax, %zu    ; .%s offset\n",
            access->member->offset, access->member_name);
    
    // Değeri yükle
    if (access->member->size == 8) {
        fprintf(out, "    mov rax, [rax]    ; 8-byte değer\n");
    } else if (access->member->size == 1) {
        fprintf(out, "    movzx rax, byte [rax]    ; 1-byte değer\n");
    } else {
        fprintf(out, "    ; Özel boyut: %zu byte\n", access->member->size);
    }
}

// Üye değer ataması için kod üret
void codegen_member_assignment(FILE* out, MemberAccess* access, const char* value) {
    if (!access->member) return;
    
    fprintf(out, "    ; Üye ataması: %s.%s = %s\n",
            access->struct_instance, access->member_name, value);
    
    // Base pointer'ı yükle
    fprintf(out, "    mov rbx, [rbp - 8]    ; %s base\n",
            access->struct_instance);
    
    // Offset ekle
    fprintf(out, "    add rbx, %zu    ; .%s offset\n",
            access->member->offset, access->member_name);
    
    // Değeri ata (rax'te değer olduğu varsayılır)
    if (access->member->size == 8) {
        fprintf(out, "    mov [rbx], rax    ; 8-byte atama\n");
    } else if (access->member->size == 1) {
        fprintf(out, "    mov byte [rbx], al    ; 1-byte atama\n");
    }
}
