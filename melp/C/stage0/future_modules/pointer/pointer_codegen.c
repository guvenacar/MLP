#include "pointer_codegen.h"

// Pointer tanımı için kod üret
void codegen_pointer_declaration(FILE* out, Pointer* ptr) {
    fprintf(out, "    ; Pointer tanımı: %s (tip: %d, boyut: %zu)\n",
            ptr->name, ptr->type, ptr->pointee_size);
    
    // Stack'te pointer için 8 byte (64-bit adres) ayır
    fprintf(out, "    sub rsp, 8    ; %s için alan\n", ptr->name);
    
    // NULL ile başlat
    if (ptr->is_null) {
        fprintf(out, "    mov qword [rbp - 8], 0    ; NULL\n");
    }
}

// Adres alma operasyonu için kod üret
void codegen_address_of(FILE* out, AddressOf* addr) {
    fprintf(out, "    ; Adres alma: &%s\n", addr->variable_name);
    
    // Değişkenin stack üzerindeki adresini hesapla
    fprintf(out, "    lea rax, [rbp - 16]    ; %s adresi\n", 
            addr->variable_name);
    
    // Pointer değişkenine ata
    fprintf(out, "    mov [rbp - 8], rax    ; pointer'a kaydet\n");
}

// Dereference operasyonu için kod üret
void codegen_dereference(FILE* out, Dereference* deref) {
    fprintf(out, "    ; Dereference: *%s\n", deref->pointer_name);
    
    // Pointer değerini yükle (adres)
    fprintf(out, "    mov rax, [rbp - 8]    ; %s içeriği (adres)\n",
            deref->pointer_name);
    
    // NULL kontrolü (opsiyonel)
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .null_ptr_error    ; NULL ise hata\n");
    
    // Adresteki değeri oku
    fprintf(out, "    mov rax, [rax]    ; Değeri oku\n");
}

// Pointer aritmetiği için kod üret
void codegen_pointer_arith(FILE* out, PointerArith* arith) {
    fprintf(out, "    ; Pointer aritmetiği: %s ", arith->pointer_name);
    
    switch (arith->op) {
        case PTR_ADD:
            fprintf(out, "+ %ld\n", arith->offset);
            // Pointer değerini yükle
            fprintf(out, "    mov rax, [rbp - 8]    ; pointer\n");
            // Offset'i ekle (eleman boyutu ile çarp)
            fprintf(out, "    mov rbx, %ld    ; offset\n", arith->offset);
            fprintf(out, "    imul rbx, 8    ; eleman boyutu ile çarp\n");
            fprintf(out, "    add rax, rbx    ; yeni adres\n");
            break;
            
        case PTR_SUB:
            fprintf(out, "- %ld\n", arith->offset);
            fprintf(out, "    mov rax, [rbp - 8]    ; pointer\n");
            fprintf(out, "    mov rbx, %ld    ; offset\n", arith->offset);
            fprintf(out, "    imul rbx, 8    ; eleman boyutu ile çarp\n");
            fprintf(out, "    sub rax, rbx    ; yeni adres\n");
            break;
            
        case PTR_DIFF:
            fprintf(out, "- %s\n", arith->other_ptr);
            // İki pointer arasındaki fark (eleman sayısı)
            fprintf(out, "    mov rax, [rbp - 8]    ; ptr1\n");
            fprintf(out, "    mov rbx, [rbp - 16]   ; ptr2\n");
            fprintf(out, "    sub rax, rbx    ; byte fark\n");
            fprintf(out, "    mov rbx, 8    ; eleman boyutu\n");
            fprintf(out, "    cqo\n");
            fprintf(out, "    idiv rbx    ; eleman sayısı\n");
            break;
    }
}

// NULL kontrolü için kod üret
void codegen_null_check(FILE* out, const char* ptr_name) {
    fprintf(out, "    ; NULL kontrolü: %s\n", ptr_name);
    fprintf(out, "    mov rax, [rbp - 8]    ; pointer değeri\n");
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .null_ptr_error    ; NULL ise hata\n");
}
