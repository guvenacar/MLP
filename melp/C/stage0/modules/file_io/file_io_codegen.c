#include "file_io_codegen.h"

// Dosya açma için kod üret
void codegen_file_open(FILE* out, FileOp* op) {
    fprintf(out, "    ; Dosya açma: %s (mod: %d)\n", op->filename, op->mode);
    
    // Dosya adı string'ini yükle
    fprintf(out, "    lea rdi, [.str_filename_%p]    ; Dosya adı\n", (void*)op);
    
    // Mod string'ini yükle
    const char* mode_str = get_fopen_mode_string(op->mode, op->is_binary);
    fprintf(out, "    lea rsi, [.str_mode_%p]    ; Mod: %s\n", (void*)op, mode_str);
    
    // fopen çağrısı
    fprintf(out, "    call fopen    ; Dosya aç\n");
    
    // NULL kontrolü (açma başarısız mı?)
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .file_open_error    ; Açma hatası\n");
    
    // Handle'ı kaydet
    fprintf(out, "    mov [rbp - 8], rax    ; Dosya handle\n");
    
    // String sabitlerini ekle (data section)
    fprintf(out, "section .data\n");
    fprintf(out, "    .str_filename_%p: db \"%s\", 0\n", (void*)op, op->filename);
    fprintf(out, "    .str_mode_%p: db \"%s\", 0\n", (void*)op, mode_str);
    fprintf(out, "section .text\n");
}

// Dosya okuma için kod üret
void codegen_file_read(FILE* out, FileRead* read) {
    fprintf(out, "    ; Dosya okuma: %zu x %zu byte\n", read->count, read->size);
    
    // fread parametreleri: (buffer, size, count, file)
    fprintf(out, "    mov rdi, [rbp - 16]    ; Buffer adresi\n");
    fprintf(out, "    mov rsi, %zu    ; Eleman boyutu\n", read->size);
    fprintf(out, "    mov rdx, %zu    ; Eleman sayısı\n", read->count);
    fprintf(out, "    mov rcx, [rbp - 8]     ; Dosya handle\n");
    fprintf(out, "    call fread    ; Dosyadan oku\n");
    
    // Okunan eleman sayısı rax'te
    fprintf(out, "    ; rax = okunan eleman sayısı\n");
}

// Dosya yazma için kod üret
void codegen_file_write(FILE* out, FileWrite* write) {
    fprintf(out, "    ; Dosya yazma: %zu x %zu byte\n", write->count, write->size);
    
    // fwrite parametreleri: (data, size, count, file)
    fprintf(out, "    mov rdi, [rbp - 16]    ; Veri adresi\n");
    fprintf(out, "    mov rsi, %zu    ; Eleman boyutu\n", write->size);
    fprintf(out, "    mov rdx, %zu    ; Eleman sayısı\n", write->count);
    fprintf(out, "    mov rcx, [rbp - 8]     ; Dosya handle\n");
    fprintf(out, "    call fwrite    ; Dosyaya yaz\n");
    
    // Yazılan eleman sayısı rax'te
    fprintf(out, "    ; rax = yazılan eleman sayısı\n");
}

// Dosya kapatma için kod üret
void codegen_file_close(FILE* out, FileClose* close) {
    fprintf(out, "    ; Dosya kapatma: %s\n", close->file_var);
    
    // NULL kontrolü
    fprintf(out, "    mov rdi, [rbp - 8]    ; Dosya handle\n");
    fprintf(out, "    test rdi, rdi\n");
    fprintf(out, "    jz .file_already_closed    ; Zaten kapalı\n");
    
    // fclose çağrısı
    fprintf(out, "    call fclose    ; Dosyayı kapat\n");
    
    // Handle'ı NULL yap
    fprintf(out, "    mov qword [rbp - 8], 0    ; NULL yap\n");
    fprintf(out, ".file_already_closed:\n");
}

// Dosya konumlandırma için kod üret
void codegen_file_seek(FILE* out, FileSeek* seek) {
    fprintf(out, "    ; Dosya konumlandırma: offset=%ld, origin=%d\n",
            seek->offset, seek->origin);
    
    // fseek parametreleri: (file, offset, origin)
    fprintf(out, "    mov rdi, [rbp - 8]    ; Dosya handle\n");
    fprintf(out, "    mov rsi, %ld    ; Offset\n", seek->offset);
    fprintf(out, "    mov rdx, %d    ; Origin (SEEK_SET/CUR/END)\n", seek->origin);
    fprintf(out, "    call fseek    ; Konumlandır\n");
    
    // Dönüş değeri: 0 = başarılı, -1 = hata
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jnz .file_seek_error\n");
}

// Dosya kontrolü için kod üret
void codegen_file_check(FILE* out, FileCheck* check) {
    fprintf(out, "    ; Dosya kontrolü: %s\n", check->filename);
    
    // Dosyayı açmayı dene (kontrol için)
    fprintf(out, "    lea rdi, [.str_check_%p]    ; Dosya adı\n", (void*)check);
    fprintf(out, "    lea rsi, [.str_r]    ; Okuma modu\n");
    fprintf(out, "    call fopen\n");
    
    // NULL mı? (yoksa)
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .file_not_exists    ; Dosya yok\n");
    
    // Dosya var - boyutunu al
    fprintf(out, "    mov rdi, rax    ; Dosya handle\n");
    fprintf(out, "    push rdi    ; Kaydet\n");
    fprintf(out, "    mov rsi, 0    ; Offset 0\n");
    fprintf(out, "    mov rdx, 2    ; SEEK_END\n");
    fprintf(out, "    call fseek\n");
    fprintf(out, "    pop rdi    ; Handle geri al\n");
    fprintf(out, "    push rdi\n");
    fprintf(out, "    call ftell    ; Boyut\n");
    fprintf(out, "    mov rbx, rax    ; Boyutu sakla\n");
    fprintf(out, "    pop rdi\n");
    fprintf(out, "    call fclose    ; Kapat\n");
    fprintf(out, "    mov rax, 1    ; exists = true\n");
    fprintf(out, "    jmp .file_check_done\n");
    fprintf(out, ".file_not_exists:\n");
    fprintf(out, "    xor rax, rax    ; exists = false\n");
    fprintf(out, "    xor rbx, rbx    ; size = 0\n");
    fprintf(out, ".file_check_done:\n");
    
    // String sabitleri
    fprintf(out, "section .data\n");
    fprintf(out, "    .str_check_%p: db \"%s\", 0\n", (void*)check, check->filename);
    fprintf(out, "    .str_r: db \"r\", 0\n");
    fprintf(out, "section .text\n");
}

// Dosya modunu string'e çevir
const char* get_fopen_mode_string(FileMode mode, int is_binary) {
    if (is_binary) {
        switch (mode) {
            case FILE_READ: return "rb";
            case FILE_WRITE: return "wb";
            case FILE_APPEND: return "ab";
            case FILE_READ_WRITE: return "rb+";
            case FILE_WRITE_READ: return "wb+";
            case FILE_APPEND_READ: return "ab+";
        }
    } else {
        switch (mode) {
            case FILE_READ: return "r";
            case FILE_WRITE: return "w";
            case FILE_APPEND: return "a";
            case FILE_READ_WRITE: return "r+";
            case FILE_WRITE_READ: return "w+";
            case FILE_APPEND_READ: return "a+";
        }
    }
    return "r"; // Varsayılan
}

// Hata kontrolü için yardımcı
void codegen_error_check(FILE* out, const char* operation) {
    fprintf(out, "    ; Hata kontrolü: %s\n", operation);
    fprintf(out, "    test rax, rax\n");
    fprintf(out, "    jz .file_error_%s\n", operation);
}
