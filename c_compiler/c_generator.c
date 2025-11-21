// c_compiler/c_generator.c

#include "c_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> // bool tipi için
#include <ctype.h>   // tolower için

// Forward declaration from main.c
extern const char* get_base_dir();

// --- Generator Durum Yönetimi ---
static AsmCode data_section; // .data bölümü (örn: "Merhaba")
static AsmCode text_section; // .text bölümü (ana kod)

// YENİ: Kapsam Yönetimi (Python'daki Kapsam sınıfına karşılık gelir)

#define MAX_DEGISKENLER 100
#define MAX_SCOPE_DEPTH 20

typedef struct {
    char* ad;
    char* asm_adresi; // Yığındaki adresi (örn: "[rbp-8]") veya global için label
    char* tip;        // Değişkenin tipi ("SAYISAL" veya "METIN")
    int scope_level;  // Hangi scope seviyesinde tanımlandı (0=global)
    bool is_global;   // Global değişken mi?
} Degisken;

// Çok seviyeli Kapsam (Scope) - Stack bazlı
Degisken kapsam_haritasi[MAX_DEGISKENLER];
int kapsam_degisken_sayisi = 0;
int kapsam_yigin_ofseti = 0; // RBP'den ne kadar aşağı inildiği (örn: -8, -16)

// Scope seviyesi tracking
int current_scope_level = 0;
int scope_stack_offsets[MAX_SCOPE_DEPTH]; // Her scope seviyesinde stack offset

// YENİ: Etiket (Label) Sayacı
static int etiket_sayaci = 0;

// YENİ: String literal sayacı (benzersiz etiketler için)
static int metin_sayaci = 0;

// YENİ: Aktif Döngü Bitiş Etiketi
// (İç içe döngüler için bu bir yığın (stack) olmalıdır, şimdilik basit tutalım)
static int aktif_dongu_son_etiketi = -1; 

// Argüman register'ları (Linux x86-64 ABI)
const char* arg_registerleri[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

// --- Struct Metadata Yönetimi ---
#define MAX_STRUCT_FIELDS 20
#define MAX_STRUCTS 50

typedef struct {
    char* field_ad;
    char* field_tip;
    int offset;  // Struct başından itibaren byte offset
    int size;    // Field boyutu (byte)
} StructFieldInfo;

typedef struct {
    char* struct_ad;
    int field_sayisi;
    StructFieldInfo fields[MAX_STRUCT_FIELDS];
    int total_size;  // Toplam struct boyutu (byte)
} StructMetadata;

// Global struct metadata tablosu
StructMetadata struct_metadata_table[MAX_STRUCTS];
int struct_metadata_count = 0;

// Kapsam Yönetim Fonksiyonları

// Kapsam haritasında strdup ile ayrılmış belleği temizler
void kapsam_temizle() {
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        free(kapsam_haritasi[i].ad);
        free(kapsam_haritasi[i].asm_adresi);
        free(kapsam_haritasi[i].tip);
    }
    kapsam_degisken_sayisi = 0;
    current_scope_level = 0;
}

// Yeni bir scope'a gir (EĞER, DÖNGÜ, İŞLEÇ blokları için)
void kapsam_gir() {
    if (current_scope_level >= MAX_SCOPE_DEPTH - 1) {
        fprintf(stderr, "HATA [Generator]: Maksimum scope derinliği aşıldı!\n");
        exit(1);
    }

    // Mevcut stack offset'i kaydet
    scope_stack_offsets[current_scope_level] = kapsam_yigin_ofseti;
    current_scope_level++;
}

// Scope'tan çık - bu seviyedeki değişkenleri temizle
void kapsam_cik() {
    if (current_scope_level <= 0) {
        return; // Zaten en üst seviyedeyiz
    }

    current_scope_level--;

    // Bu seviyedeki değişkenleri temizle
    int new_var_count = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level <= current_scope_level) {
            // Bu değişkeni tut
            if (i != new_var_count) {
                kapsam_haritasi[new_var_count] = kapsam_haritasi[i];
            }
            new_var_count++;
        } else {
            // Bu değişkeni temizle
            free(kapsam_haritasi[i].ad);
            free(kapsam_haritasi[i].asm_adresi);
            free(kapsam_haritasi[i].tip);
        }
    }

    kapsam_degisken_sayisi = new_var_count;

    // Stack offset'i geri yükle
    kapsam_yigin_ofseti = scope_stack_offsets[current_scope_level];
}

// --- Struct Metadata Helper Fonksiyonları ---

// Field tipine göre byte cinsinden boyut döndürür
int get_field_size(const char* tip) {
    // Basitleştirilmiş: Tüm tipler 8 byte (pointer veya int64)
    // string -> pointer (8 byte)
    // int -> int64 (8 byte)
    // bool -> int64 (8 byte)
    return 8;
}

// Struct tanımını metadata tablosuna kaydeder
void register_struct_metadata(char* struct_ad, Token** field_tipleri, Token** field_adlari, int field_sayisi) {
    if (struct_metadata_count >= MAX_STRUCTS) {
        fprintf(stderr, "HATA [Generator]: Maksimum struct sayısı aşıldı!\n");
        exit(1);
    }

    StructMetadata* meta = &struct_metadata_table[struct_metadata_count++];
    meta->struct_ad = strdup(struct_ad);
    meta->field_sayisi = field_sayisi;

    int current_offset = 0;
    for (int i = 0; i < field_sayisi; i++) {
        meta->fields[i].field_ad = strdup(field_adlari[i]->value);
        meta->fields[i].field_tip = strdup(field_tipleri[i]->value);
        meta->fields[i].offset = current_offset;
        meta->fields[i].size = get_field_size(field_tipleri[i]->value);
        current_offset += meta->fields[i].size;
    }

    meta->total_size = current_offset;
}

// Struct adına göre metadata bulur (NULL if not found)
StructMetadata* find_struct_metadata(const char* struct_ad) {
    for (int i = 0; i < struct_metadata_count; i++) {
        if (strcmp(struct_metadata_table[i].struct_ad, struct_ad) == 0) {
            return &struct_metadata_table[i];
        }
    }
    return NULL;
}

// Struct içinde field bulur (NULL if not found)
StructFieldInfo* find_field_in_struct(StructMetadata* meta, const char* field_ad) {
    for (int i = 0; i < meta->field_sayisi; i++) {
        if (strcmp(meta->fields[i].field_ad, field_ad) == 0) {
            return &meta->fields[i];
        }
    }
    return NULL;
}

// Değişkeni kaydeder ve yığındaki adresini döndürür
char* kapsam_degisken_yer_ayir(const char* ad, const char* tip) {
    kapsam_yigin_ofseti += 8; // Yığında 8 byte (64-bit) yer aç
    char* adres = (char*)malloc(32);
    sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);

    // Değişkeni haritaya kaydet
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(ad);
    d->asm_adresi = strdup(adres);
    d->tip = strdup(tip);
    d->scope_level = current_scope_level;
    d->is_global = (current_scope_level == 0);

    return adres;
}

// Bir değişkenin yığındaki adresini bulur
char* kapsam_degisken_adresi_bul(const char* ad) {
    // Kapsam haritasını geriye doğru ara (en son tanımlananı bul)
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return kapsam_haritasi[i].asm_adresi;
        }
    }

    fprintf(stderr, "HATA [Generator]: Tanımsız değişken: %s\n", ad);
    exit(1);
}

// Bir değişkenin tam bilgisini (Degisken pointer) bulur
Degisken* kapsam_degisken_bul(const char* ad) {
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return &kapsam_haritasi[i];
        }
    }
    return NULL;  // Bulunamadı
}

// Bir değişkenin tipini bulur
char* kapsam_degisken_tipi_bul(const char* ad) {
    if (ad == NULL) {
        fprintf(stderr, "HATA [Generator]: kapsam_degisken_tipi_bul'a NULL ad geldi\n");
        exit(1);
    }

    // Kapsam haritasını geriye doğru ara (en son tanımlananı bul)
    for (int i = kapsam_degisken_sayisi - 1; i >= 0; i--) {
        if (kapsam_haritasi[i].ad != NULL && strcmp(kapsam_haritasi[i].ad, ad) == 0) {
            return kapsam_haritasi[i].tip;
        }
    }

    fprintf(stderr, "HATA [Generator]: Tanımsız değişken: %s (Kapsam: %d değişken)\n", ad, kapsam_degisken_sayisi);
    exit(1);
}

// --- Yardımcı Fonksiyonlar ---

// TYD fonksiyon adlarını C runtime fonksiyon adlarına map eder
const char* convert_function_name(const char* tyd_adi) {
    // Dosya fonksiyonları
    if (strcmp(tyd_adi, "DOSYA_AC") == 0) return "tyd_dosya_ac";
    if (strcmp(tyd_adi, "DOSYA_OKU") == 0) return "tyd_dosya_oku";
    if (strcmp(tyd_adi, "DOSYA_YAZ") == 0) return "tyd_dosya_yaz";
    if (strcmp(tyd_adi, "DOSYA_KAPAT") == 0) return "tyd_dosya_kapat";
    
    // String fonksiyonları
    if (strcmp(tyd_adi, "STRLEN") == 0) return "strlen";
    if (strcmp(tyd_adi, "SUBSTR") == 0) return "tyd_substr";
    if (strcmp(tyd_adi, "STRCAT") == 0) return "tyd_strcat";
    if (strcmp(tyd_adi, "STRSTR") == 0) return "strstr";
    if (strcmp(tyd_adi, "STRCMP") == 0) return "strcmp";
    
    return tyd_adi;
}

// AsmCode yapısına güvenli bir şekilde kod ekler
void asm_append(AsmCode* section, const char* code) {
    if (code == NULL) {
        fprintf(stderr, "HATA: asm_append'e NULL kod geldi!\n");
        return;
    }

    size_t code_len = strlen(code);
    size_t new_line_len = 1; // for '\n'
    size_t needed = section->size + code_len + new_line_len + 1; // +1 for null

    // Gerekirse kapasiteyi artır (GCC-style: 4KB başlangıç, 2x büyütme)
    if (needed > section->capacity) {
        // Yeni kapasite: başlangıçta 4KB, sonra 2x veya tam ihtiyaç
        size_t new_capacity = (section->capacity == 0) ? 4096 : section->capacity * 2;

        // Eğer 2x bile yetmezse, tam ihtiyacı karşıla + %25 buffer
        if (new_capacity < needed) {
            new_capacity = needed + (needed / 4);
        }

        char* new_code = (char*)realloc(section->code, new_capacity);
        if (new_code == NULL) {
            fprintf(stderr, "HATA: Bellek ayırma başarısız (istenen: %zu bytes)\n", new_capacity);
            exit(EXIT_FAILURE);
        }

        section->code = new_code;
        section->capacity = new_capacity;

        // İlk ayırmada string'i sıfırla
        if (section->size == 0) {
            section->code[0] = '\0';
        }
    }

    // GÜVENLİ string ekleme (strcpy yerine memcpy + manuel null)
    memcpy(section->code + section->size, code, code_len);
    section->code[section->size + code_len] = '\n';
    section->code[section->size + code_len + 1] = '\0';
    section->size += (code_len + new_line_len);
}

// --- Ziyaretçi (Visitor) Fonksiyonları ---

void visit(ASTNode* node); // Ana ziyaretçi fonksiyonu (prototip)
void visit_IkiliIslem(ASTNode* node); // İleri bildirim
void visit_DegiskenTanimlama(ASTNode* node); // İleri bildirim
void visit_Degisken(ASTNode* node); // İleri bildirim
void visit_AtamaKomutu(ASTNode* node); // İleri bildirim
void visit_KosulKomutu(ASTNode* node); // İleri bildirim
void visit_DonguKomutu(ASTNode* node); // İleri bildirim
void visit_ForKomutu(ASTNode* node); // İleri bildirim
void visit_DonguBitirKomutu(ASTNode* node); // İleri bildirim
void visit_IslecTanimlama(ASTNode* node); // İleri bildirim
void visit_IslecCagirma(ASTNode* node); // İleri bildirim
void visit_DonusKomutu(ASTNode* node); // İleri bildirim
void visit_ArrayTanimlama(ASTNode* node); // İleri bildirim
void visit_ArrayErisim(ASTNode* node); // İleri bildirim
void visit_ArrayAtama(ASTNode* node); // İleri bildirim
void visit_StructTanimlama(ASTNode* node); // İleri bildirim
void visit_StructFieldAccess(ASTNode* node); // İleri bildirim
void visit_StructFieldAtama(ASTNode* node); // İleri bildirim
void visit_StructDegisken(ASTNode* node); // İleri bildirim

// Phase 2: List visitor forward declarations
void visit_ListTanimlama(ASTNode* node);
void visit_ListAdd(ASTNode* node);
void visit_ListGet(ASTNode* node);
void visit_ListSize(ASTNode* node);
void visit_ListClear(ASTNode* node);

// Phase 4: Hash Map visitor forward declarations
void visit_MapTanimlama(ASTNode* node);
void visit_MapSet(ASTNode* node);
void visit_MapGet(ASTNode* node);
void visit_MapHas(ASTNode* node);
void visit_MapRemove(ASTNode* node);
void visit_MapSize(ASTNode* node);
void visit_MapClear(ASTNode* node);

void visit_Blok(ASTNode* node) {
    // Blok içindeki her komutu ziyaret et
    for (int i = 0; i < node->blok_data.sayisi; i++) {
        visit(node->blok_data.komutlar[i]);
    }
}

void visit_Yazdir(ASTNode* node) {

    // 1. İfadeyi ziyaret et (sonuç RAX'e yüklenecek)
    visit(node->tek_ifade_data.ifade);

    // 2. C 'printf' fonksiyonunu çağır
    asm_append(&text_section, "    ; --- YAZDIR Başlangıç ---");

    // İfadenin tipine göre format string seç
    bool is_string = false;

    if (node->tek_ifade_data.ifade->type == AST_METIN) {
        is_string = true;
    } else if (node->tek_ifade_data.ifade->type == AST_DEGISKEN) {
        // Değişken ise, tipini kontrol et
        if (node->tek_ifade_data.ifade->degisken_data.ad != NULL &&
            node->tek_ifade_data.ifade->degisken_data.ad->value != NULL) {
            char* degisken_adi = node->tek_ifade_data.ifade->degisken_data.ad->value;
            char* degisken_tipi = kapsam_degisken_tipi_bul(degisken_adi);
            if (degisken_tipi != NULL && strcmp(degisken_tipi, "METIN") == 0) {
                is_string = true;
            }
        } else {
        }
    } else if (node->tek_ifade_data.ifade->type == AST_BUILTIN_CALL) {
        // Built-in call ise, string döndüren fonksiyonları kontrol et
        TokenType func_type = node->tek_ifade_data.ifade->builtin_call_data.function_type;
        if (func_type == TOKEN_BUILTIN_GET_ENV ||
            func_type == TOKEN_BUILTIN_READ_BINARY ||
            func_type == TOKEN_BUILTIN_GET_FILE_INFO ||
            func_type == TOKEN_BUILTIN_GET_CURRENT_DIR) {
            is_string = true;
        }
    }


    if (is_string) {
        // String yazdır (RAX'te string adresi var)
        asm_append(&text_section, "    mov rdi, format_metin"); // format_metin: "%s\n"
        asm_append(&text_section, "    mov rsi, rax");      // String adresi
    } else {
        // Sayı yazdır (RAX'te sayı var)
        asm_append(&text_section, "    mov rdi, format_sayi"); // format_sayi: "%ld\n"
        asm_append(&text_section, "    mov rsi, rax");      // Yazdırılacak değer
    }

    asm_append(&text_section, "    mov rax, 0");      // SSE kuralı
    asm_append(&text_section, "    call printf");
}

void visit_Sayi(ASTNode* node) {
    // Bir sayıyı RAX register'ına yükle
    // Geçici düzeltme (sprintf kullanarak):
    char buffer[64];
    sprintf(buffer, "    mov rax, %s", node->sabit_data.deger);
    asm_append(&text_section, buffer);
}

// Helper function: Escape special characters for NASM assembly
char* escape_string_for_asm(const char* input) {
    static char buffer[2048];
    int j = 0;

    for (int i = 0; input[i] != '\0' && j < 2046; i++) {
        char c = input[i];

        // Escape special characters
        if (c == '\n') {
            buffer[j++] = '\\';
            buffer[j++] = 'n';
        } else if (c == '\t') {
            buffer[j++] = '\\';
            buffer[j++] = 't';
        } else if (c == '\r') {
            buffer[j++] = '\\';
            buffer[j++] = 'r';
        } else if (c == '\\') {
            buffer[j++] = '\\';
            buffer[j++] = '\\';
        } else if (c == '\"') {
            buffer[j++] = '\\';
            buffer[j++] = '\"';
        } else {
            buffer[j++] = c;
        }
    }
    buffer[j] = '\0';
    return buffer;
}

void visit_Metin(ASTNode* node) {
    // String literal'i .data bölümüne ekle ve adresini RAX'e yükle
    char etiket[64];
    char buffer[2048];

    // Benzersiz etiket oluştur
    sprintf(etiket, "str_%d", metin_sayaci++);

    // Escape special characters for assembly
    char* escaped = escape_string_for_asm(node->sabit_data.deger);

    // .data bölümüne string ekle (null-terminated)
    sprintf(buffer, "%s: db \"%s\", 0", etiket, escaped);
    asm_append(&data_section, buffer);

    // String'in adresini RAX'e yükle
    sprintf(buffer, "    mov rax, %s", etiket);
    asm_append(&text_section, buffer);
}

void visit_DegiskenTanimlama(ASTNode* node) {
    char* degisken_adi = node->tanimlama_data.ad->value;
    char* degisken_tipi = node->tanimlama_data.tip->value;
    char buffer[256];

    sprintf(buffer, "    ; --- DegiskenTanimlama: %s ---", degisken_adi);
    asm_append(&text_section, buffer);

    // Global değişken mi? (scope level 0)
    if (current_scope_level == 0) {
        // Global değişken: .data section'a ekle
        sprintf(buffer, "global_%s: dq 0", degisken_adi);
        asm_append(&data_section, buffer);

        // Değişken için yığında değil, .data'da yer ayır
        char* global_adres = (char*)malloc(64);
        sprintf(global_adres, "[global_%s]", degisken_adi);

        // Scope'a ekle
        Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
        d->ad = strdup(degisken_adi);
        d->asm_adresi = global_adres;
        d->tip = strdup(degisken_tipi);
        d->scope_level = 0;
        d->is_global = true;

        // İfadeyi hesapla (sonuç RAX'te)
        visit(node->tanimlama_data.ifade);

        // Global değişkene ata
        sprintf(buffer, "    mov %s, rax", global_adres);
        asm_append(&text_section, buffer);
    } else {
        // Local değişken: Stack'te
        // 1. İfadeyi (sağ tarafı) hesapla (Sonuç RAX'e yüklenir)
        visit(node->tanimlama_data.ifade);

        // 2. Değişken için yığında (stack) yer ayır
        char* adres = kapsam_degisken_yer_ayir(degisken_adi, degisken_tipi);

        // 3. İfadenin sonucunu (RAX) yığındaki yeni adrese taşı
        sprintf(buffer, "    mov %s, rax", adres); // Örn: mov [rbp-8], rax
        asm_append(&text_section, buffer);
        free(adres); // kapsam_degisken_yer_ayir'dan gelen belleği temizle
    }
}

void visit_Degisken(ASTNode* node) {
    char* degisken_adi = node->degisken_data.ad->value;
    char* adres = kapsam_degisken_adresi_bul(degisken_adi);
    
    // Değerin yığındaki adresinden (örn: [rbp-8]) RAX'e yükle
    char buffer[128];
    sprintf(buffer, "    mov rax, %s", adres); // Örn: mov rax, [rbp-8]
    asm_append(&text_section, buffer);
}

void visit_AtamaKomutu(ASTNode* node) {
    char* degisken_adi = node->atama_data.ad->value;
    char buffer[128];
    sprintf(buffer, "    ; --- AtamaKomutu: %s ---", degisken_adi);
    asm_append(&text_section, buffer);

    // 1. İfadeyi (sağ tarafı) hesapla (Sonuç RAX'e yüklenir)
    visit(node->atama_data.ifade);

    // 2. Değişkenin adresini bul
    char* adres = kapsam_degisken_adresi_bul(degisken_adi);
    
    // 3. İfadenin sonucunu (RAX) değişkenin adresine taşı
    sprintf(buffer, "    mov %s, rax", adres); // Örn: mov [rbp-8], rax
    asm_append(&text_section, buffer);
}

void visit_KosulKomutu(ASTNode* node) {
    // Benzersiz etiket kimlikleri al
    int etiket_degilse = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    char buffer[128];

    // 1. Koşul İfadesini (a == 10) hesapla
    // (visit_IkiliIslem sayesinde sonuç RAX'e 1 veya 0 olarak yüklenecek)
    asm_append(&text_section, "    ; --- Kosul Komutu (EĞER) ---");
    visit(node->kosul_data.kosul);
    
    // 2. Koşulu kontrol et ve atla
    asm_append(&text_section, "    cmp rax, 0"); // Sonuç 0 mı (false) ?
    
    // Eğer 'DEĞİLSE' bloğu varsa oraya, yoksa 'SON' etiketine atla
    if (node->kosul_data.degilse_blok != NULL) {
        sprintf(buffer, "    je .L%d", etiket_degilse); // False ise DEĞİLSE'ye atla
    } else {
        sprintf(buffer, "    je .L%d", etiket_son); // False ise SON'a atla
    }
    asm_append(&text_section, buffer);

    // 3. 'İSE' Bloğunu (Doğruysa) çalıştır
    visit(node->kosul_data.ise_blok);
    
    // 'İSE' bloğu bittikten sonra 'SON' etiketine atla (DEĞİLSE'yi atlamak için)
    sprintf(buffer, "    jmp .L%d", etiket_son);
    asm_append(&text_section, buffer);

    // 4. 'DEĞİLSE' Bloğu (Varsa)
    if (node->kosul_data.degilse_blok != NULL) {
        sprintf(buffer, ".L%d:", etiket_degilse); // DEĞİLSE etiketi
        asm_append(&text_section, buffer);
        visit(node->kosul_data.degilse_blok);
    }
    
    // 5. 'SON' Etiketi
    sprintf(buffer, ".L%d:", etiket_son); // SON etiketi
    asm_append(&text_section, buffer);
}

void visit_DonguKomutu(ASTNode* node) {
    // Benzersiz etiket kimlikleri al
    int etiket_basla = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    char buffer[128];

    // 1. DÖNGÜ_BITIR için bu döngünün bitiş etiketini kaydet
    int onceki_aktif_dongu_son_etiketi = aktif_dongu_son_etiketi;
    aktif_dongu_son_etiketi = etiket_son;

    // 2. Döngü Başlangıç Etiketi
    sprintf(buffer, ".L%d:", etiket_basla); // .L_BASLA
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- While Loop ---");

    // 3. If there's a condition, evaluate it
    if (node->dongu_data.kosul != NULL) {
        visit(node->dongu_data.kosul);  // Condition result in rax
        asm_append(&text_section, "    test rax, rax");
        sprintf(buffer, "    jz .L%d", etiket_son);  // Jump to end if false
        asm_append(&text_section, buffer);
    }

    // 4. Döngü Gövdesi
    visit(node->dongu_data.govde);
    
    // 5. Gövde bittikten sonra başa atla
    sprintf(buffer, "    jmp .L%d", etiket_basla); // jmp .L_BASLA
    asm_append(&text_section, buffer);

    // 6. Döngü Bitiş Etiketi
    sprintf(buffer, ".L%d:", etiket_son); // .L_SON
    asm_append(&text_section, buffer);

    // 7. Döngü bitti, eski etiketi geri yükle (iç içe döngüler için)
    aktif_dongu_son_etiketi = onceki_aktif_dongu_son_etiketi;
}

void visit_ForKomutu(ASTNode* node) {
    // for i = 0 to 10 [step 2]
    
    // 1. Loop variable initialization
    char* var_name = node->for_data.degisken->value;
    
    // Allocate variable in scope
    char* var_addr = kapsam_degisken_yer_ayir(var_name, "SAYISAL");
    
    // Initialize: i = start
    visit(node->for_data.baslangic);  // Start value in rax
    char buffer[256];
    sprintf(buffer, "    mov %s, rax", var_addr);
    asm_append(&text_section, buffer);
    
    // 2. Labels
    int etiket_basla = etiket_sayaci++;
    int etiket_son = etiket_sayaci++;
    
    int onceki_aktif_dongu_son_etiketi = aktif_dongu_son_etiketi;
    aktif_dongu_son_etiketi = etiket_son;
    
    // 3. Loop start
    sprintf(buffer, ".L%d:", etiket_basla);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    ; --- For Loop ---");
    
    // 4. Check condition: i <= end
    sprintf(buffer, "    mov rax, %s", var_addr);  // Load i
    asm_append(&text_section, buffer);
    
    visit(node->for_data.bitis);  // End value in rax (saved to stack)
    asm_append(&text_section, "    mov rbx, rax");  // End in rbx
    sprintf(buffer, "    mov rax, %s", var_addr);  // i back in rax
    asm_append(&text_section, buffer);
    
    asm_append(&text_section, "    cmp rax, rbx");  // Compare i with end
    sprintf(buffer, "    jg .L%d", etiket_son);  // Jump if i > end
    asm_append(&text_section, buffer);
    
    // 5. Loop body
    visit(node->for_data.govde);
    
    // 6. Increment: i = i + step (default 1)
    if (node->for_data.adim != NULL) {
        visit(node->for_data.adim);  // Step in rax
        asm_append(&text_section, "    mov rcx, rax");  // Save step
        sprintf(buffer, "    mov rax, %s", var_addr);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    add rax, rcx");
        sprintf(buffer, "    mov %s, rax", var_addr);
        asm_append(&text_section, buffer);
    } else {
        sprintf(buffer, "    mov rax, %s", var_addr);
        asm_append(&text_section, buffer);
        asm_append(&text_section, "    inc rax");
        sprintf(buffer, "    mov %s, rax", var_addr);
        asm_append(&text_section, buffer);
    }
    
    // 7. Jump back to start
    sprintf(buffer, "    jmp .L%d", etiket_basla);
    asm_append(&text_section, buffer);
    
    // 8. Loop end
    sprintf(buffer, ".L%d:", etiket_son);
    asm_append(&text_section, buffer);
    
    aktif_dongu_son_etiketi = onceki_aktif_dongu_son_etiketi;
}

void visit_DonguBitirKomutu(ASTNode* node) {
    if (aktif_dongu_son_etiketi == -1) {
        fprintf(stderr, "HATA [Generator]: 'DÖNGÜ_BITIR' bir döngü içinde değil.\n");
        exit(1);
    }
    
    char buffer[128];
    sprintf(buffer, "    jmp .L%d", aktif_dongu_son_etiketi); // jmp .L_SON
    asm_append(&text_section, "    ; --- DonguBitir Komutu ---");
    asm_append(&text_section, buffer);
}

void visit_IslecTanimlama(ASTNode* node) {
    char* islec_adi = node->islec_tanimlama_data.ad->value;
    char buffer[128];

    // Fonksiyon için yeni kapsam aç
    int onceki_degisken_sayisi = kapsam_degisken_sayisi;
    int onceki_yigin_ofseti = kapsam_yigin_ofseti;

    // Global değişkenleri koru, sadece stack offset'i sıfırla
    // Global değişkenleri say (scope_level == 0)
    int global_sayisi = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            global_sayisi++;
        }
    }

    // Global değişkenleri başa taşı ve sayıyı ayarla
    int new_idx = 0;
    for (int i = 0; i < kapsam_degisken_sayisi; i++) {
        if (kapsam_haritasi[i].scope_level == 0) {
            if (i != new_idx) {
                kapsam_haritasi[new_idx] = kapsam_haritasi[i];
            }
            new_idx++;
        }
    }
    kapsam_degisken_sayisi = global_sayisi;
    kapsam_yigin_ofseti = 0;  // Fonksiyon stack'i sıfırdan başlar

    // 1. Fonksiyon Etiketini Tanımla
    sprintf(buffer, "%s:", islec_adi);
    asm_append(&text_section, buffer);
    sprintf(buffer, "    ; --- Islec Tanimlama: %s ---", islec_adi);
    asm_append(&text_section, buffer);

    // 2. Fonksiyon Girişi (Prolog)
    asm_append(&text_section, "    push rbp");
    asm_append(&text_section, "    mov rbp, rsp");
    asm_append(&text_section, "    sub rsp, 256"); // ✅ Yeterli stack alanı (32 değişkene kadar)

    // 3. Parametreleri kaydet
    int param_sayisi = node->islec_tanimlama_data.parametre_sayisi;
    for (int i = 0; i < param_sayisi; i++) {
        char* param_adi = node->islec_tanimlama_data.parametreler[i]->value;
        char* adres = kapsam_degisken_yer_ayir(param_adi, "SAYISAL");
        sprintf(buffer, "    mov %s, %s", adres, arg_registerleri[i]);
        asm_append(&text_section, buffer);
        free(adres);
    }

    // 4. Fonksiyon Gövdesi
    visit(node->islec_tanimlama_data.govde);

    // 5. Önceki kapsamı geri yükle
    kapsam_degisken_sayisi = onceki_degisken_sayisi;
    kapsam_yigin_ofseti = onceki_yigin_ofseti;
}

void visit_IslecCagirma(ASTNode* node) {
    char* islec_adi = node->islec_cagirma_data.hedef_ad->value;
    int arg_sayisi = node->islec_cagirma_data.arguman_sayisi;
    char buffer[128];

    // ===== STRING FONKSİYONLARI KONTROLÜ =====
    if (strcmp(islec_adi, "STRLEN") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]); // String argümanı RAX'e
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call strlen");
        return; // ✅ Sonuç zaten RAX'te
    }
    else if (strcmp(islec_adi, "STRCAT") == 0 && arg_sayisi == 2) {
        // Argüman 1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // Arg1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // Arg2 -> rsi
        asm_append(&text_section, "    call tyd_strcat");
        return; // ✅ Sonuç RAX'te
    }
    else if (strcmp(islec_adi, "SUBSTR") == 0 && arg_sayisi == 3) {
        // Argüman 1 (str)
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 2 (pos)
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    push rax");
        
        // Argüman 3 (len)
        visit(node->islec_cagirma_data.argumanlar[2]);
        asm_append(&text_section, "    pop rsi");  // pos
        asm_append(&text_section, "    pop rdi");  // str
        asm_append(&text_section, "    mov rdx, rax"); // len
        asm_append(&text_section, "    call tyd_substr");
        return;
    }
    else if (strcmp(islec_adi, "STRCMP") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call strcmp");
        return;
    }
    else if (strcmp(islec_adi, "STRSTR") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call strstr");
        return;
    }

    // ===== DOSYA FONKSİYONLARI =====
    else if (strcmp(islec_adi, "DOSYA_AC") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call dosya_ac");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_OKU") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call dosya_oku");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_YAZ") == 0 && arg_sayisi == 2) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");
        asm_append(&text_section, "    mov rsi, rax");
        asm_append(&text_section, "    call dosya_yaz");
        return;
    }
    else if (strcmp(islec_adi, "DOSYA_KAPAT") == 0 && arg_sayisi == 1) {
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call dosya_kapat");
        return;
    }
    // YENİ: DIZIN_AL fonksiyonu (self-hosting için)
    else if (strcmp(islec_adi, "DIZIN_AL") == 0 && arg_sayisi == 0) {
        asm_append(&text_section, "    ; --- DIZIN_AL cagirma ---");
        asm_append(&text_section, "    mov rax, 0"); // SSE kuralı
        asm_append(&text_section, "    call runtime_dizin_al");
        // Sonuç (dizin yolu char*) zaten RAX'te olacak
        return;
    }

    // ===== BOOTSTRAP STRING FONKSİYONLARI =====
    // (Bu fonksiyonlar sadece TYD compiler'ını TYD'de yazmak için gerekli)

    else if (strcmp(islec_adi, "STRING_KARAKTER_AL") == 0 && arg_sayisi == 2) {
        // Argüman 1: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: indeks
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // indeks -> rsi
        asm_append(&text_section, "    call string_karakter_al");
        return; // Sonuç RAX'te (tek karakterlik string)
    }

    else if (strcmp(islec_adi, "STRING_ALT") == 0 && arg_sayisi == 3) {
        // Argüman 1: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: baslangic
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    push rax");
        // Argüman 3: uzunluk
        visit(node->islec_cagirma_data.argumanlar[2]);
        asm_append(&text_section, "    pop rsi");  // baslangic -> rsi
        asm_append(&text_section, "    pop rdi");  // string -> rdi
        asm_append(&text_section, "    mov rdx, rax"); // uzunluk -> rdx
        asm_append(&text_section, "    call string_alt");
        return; // Sonuç RAX'te (substring)
    }

    else if (strcmp(islec_adi, "KARAKTER_KODU") == 0 && arg_sayisi == 1) {
        // Argüman: tek karakterlik string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call karakter_kodu");
        return; // Sonuç RAX'te (ASCII kodu)
    }

    else if (strcmp(islec_adi, "KODU_KARAKTERE") == 0 && arg_sayisi == 1) {
        // Argüman: ASCII kodu
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call kodu_karaktere");
        return; // Sonuç RAX'te (tek karakterlik string)
    }

    else if (strcmp(islec_adi, "STRING_BIRLESTIR") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_birlestir");
        return; // Sonuç RAX'te (birleştirilmiş string)
    }

    else if (strcmp(islec_adi, "STRING_KARSILASTIR") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_karsilastir");
        return; // Sonuç RAX'te (0 = eşit, !=0 = farklı)
    }

    else if (strcmp(islec_adi, "STRING_UZUNLUK") == 0 && arg_sayisi == 1) {
        // Argüman: string
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    mov rdi, rax");
        asm_append(&text_section, "    call string_uzunluk");
        return; // Sonuç RAX'te (uzunluk)
    }

    else if (strcmp(islec_adi, "STRING_ESIT_MI") == 0 && arg_sayisi == 2) {
        // Argüman 1: string1
        visit(node->islec_cagirma_data.argumanlar[0]);
        asm_append(&text_section, "    push rax");
        // Argüman 2: string2
        visit(node->islec_cagirma_data.argumanlar[1]);
        asm_append(&text_section, "    pop rdi");  // string1 -> rdi
        asm_append(&text_section, "    mov rsi, rax"); // string2 -> rsi
        asm_append(&text_section, "    call string_esit_mi");
        return; // Sonuç RAX'te (1 = eşit, 0 = farklı)
    }

    // ===== KULLANICI TANIMLI FONKSİYONLAR =====
    // 1. Argümanları hesapla ve yığına (stack) it
    for (int i = 0; i < arg_sayisi; i++) {
        visit(node->islec_cagirma_data.argumanlar[i]);
        asm_append(&text_section, "    push rax");
    }

    // 2. Argümanları yığından register'lara çek (ters sırada)
    for (int i = arg_sayisi - 1; i >= 0; i--) {
        sprintf(buffer, "    pop %s", arg_registerleri[i]);
        asm_append(&text_section, buffer);
    }

    // 3. Fonksiyonu çağır
    sprintf(buffer, "    call %s", islec_adi);
    asm_append(&text_section, buffer);
}

void visit_ArrayTanimlama(ASTNode* node) {
    char* array_adi = node->array_tanimlama_data.ad->value;
    char* array_tipi = node->array_tanimlama_data.tip->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayTanimlama: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // Boyut ifadesini hesapla (sonuç RAX'te olacak)
    visit(node->array_tanimlama_data.boyut);

    // Boyut sabit olmalı (şimdilik sadece sabit boyutlu arrayler)
    // RAX'te boyut var
    // Array için yığında yer ayır
    // Şimdilik basit: Her eleman 8 byte (SAYISAL)
    // Toplam boyut = eleman_sayisi * 8

    // Array bilgisini kaydet
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(array_adi);
    d->tip = (char*)malloc(64);
    sprintf(d->tip, "ARRAY_%s", array_tipi);  // Örn: "ARRAY_SAYISAL"
    d->scope_level = current_scope_level;
    d->is_global = (current_scope_level == 0);

    // Boyut bilgisini sakla (şimdilik compile-time sabit olmalı)
    // HACK: Boyut değerini AST'den almayı deneyelim
    int array_boyut = 0;
    if (node->array_tanimlama_data.boyut->type == AST_SAYI) {
        array_boyut = atoi(node->array_tanimlama_data.boyut->sabit_data.deger);
    } else {
        fprintf(stderr, "HATA [Generator]: Array boyutu compile-time sabit olmalı\n");
        exit(1);
    }

    int total_bytes = array_boyut * 8;  // Her eleman 8 byte

    if (current_scope_level == 0) {
        // Global array: .bss section'da
        sprintf(buffer, "global_array_%s: resq %d  ; %d elements", array_adi, array_boyut, array_boyut);
        asm_append(&data_section, buffer);

        char* global_adres = (char*)malloc(64);
        sprintf(global_adres, "global_array_%s", array_adi);
        d->asm_adresi = global_adres;
    } else {
        // Local array: Stack'te
        kapsam_yigin_ofseti += total_bytes;
        char* adres = (char*)malloc(32);
        sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);
        d->asm_adresi = strdup(adres);
        free(adres);

        sprintf(buffer, "    sub rsp, %d  ; Array allocation for %s[%d]",
                total_bytes, array_adi, array_boyut);
        asm_append(&text_section, buffer);
    }
}

void visit_ArrayErisim(ASTNode* node) {
    char* array_adi = node->array_erisim_data.ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayErisim: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // İndeks ifadesini hesapla (sonuç RAX'te)
    visit(node->array_erisim_data.indeks);

    // İndeksi offset'e çevir (indeks * 8)
    asm_append(&text_section, "    imul rax, 8  ; indeks * 8 (element size)");
    asm_append(&text_section, "    push rax  ; offset'i sakla");

    // Array base adresini bul
    char* array_base = kapsam_degisken_adresi_bul(array_adi);

    // Base + offset hesapla
    sprintf(buffer, "    lea rbx, %s  ; Array base adresi", array_base);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    pop rax  ; offset'i geri al");
    asm_append(&text_section, "    add rbx, rax  ; base + offset");

    // Değeri oku
    asm_append(&text_section, "    mov rax, [rbx]  ; Array elemanını oku");
}

void visit_ArrayAtama(ASTNode* node) {
    char* array_adi = node->array_atama_data.ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- ArrayAtama: %s ---", array_adi);
    asm_append(&text_section, buffer);

    // Değeri hesapla (sağ taraf)
    visit(node->array_atama_data.deger);
    asm_append(&text_section, "    push rax  ; Değeri sakla");

    // İndeks ifadesini hesapla
    visit(node->array_atama_data.indeks);

    // İndeksi offset'e çevir (indeks * 8)
    asm_append(&text_section, "    imul rax, 8  ; indeks * 8");
    asm_append(&text_section, "    mov rbx, rax  ; offset'i rbx'e");

    // Array base adresini bul
    char* array_base = kapsam_degisken_adresi_bul(array_adi);

    // Base + offset hesapla
    sprintf(buffer, "    lea rcx, %s  ; Array base adresi", array_base);
    asm_append(&text_section, buffer);
    asm_append(&text_section, "    add rcx, rbx  ; base + offset");

    // Değeri yaz
    asm_append(&text_section, "    pop rax  ; Değeri geri al");
    asm_append(&text_section, "    mov [rcx], rax  ; Array elemanına yaz");
}

// Struct Tanımlama - Metadata sakla (şimdilik sadece comment)
void visit_StructTanimlama(ASTNode* node) {
    char* struct_adi = node->struct_tanimlama_data.ad->value;
    int field_sayisi = node->struct_tanimlama_data.field_sayisi;
    Token** field_tipleri = node->struct_tanimlama_data.field_tipleri;
    Token** field_adlari = node->struct_tanimlama_data.field_adlari;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Tanımlama: %s (%d fields) ---", struct_adi, field_sayisi);
    asm_append(&text_section, buffer);

    // Struct metadata'yı kaydet
    register_struct_metadata(struct_adi, field_tipleri, field_adlari, field_sayisi);

    // Metadata'dan bilgi al ve comment olarak yaz
    StructMetadata* meta = find_struct_metadata(struct_adi);
    if (meta) {
        for (int i = 0; i < field_sayisi; i++) {
            sprintf(buffer, "    ; Field %d: %s %s (offset %d, size %d)",
                    i, meta->fields[i].field_tip, meta->fields[i].field_ad,
                    meta->fields[i].offset, meta->fields[i].size);
            asm_append(&text_section, buffer);
        }
        sprintf(buffer, "    ; Total size: %d bytes", meta->total_size);
        asm_append(&text_section, buffer);
    }
}

// Struct Field Access - p.x (değeri RAX'e yükle)
void visit_StructFieldAccess(ASTNode* node) {
    char* struct_ad = node->struct_field_access_data.struct_ad->value;
    char* field_ad = node->struct_field_access_data.field_ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Field Access: %s.%s ---", struct_ad, field_ad);
    asm_append(&text_section, buffer);

    // Struct değişkenin base adresini bul
    Degisken* var = kapsam_degisken_bul(struct_ad);
    if (!var) {
        sprintf(buffer, "    ; ERROR: Değişken '%s' bulunamadı!", struct_ad);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Değişken '%s' bulunamadı!\n", struct_ad);
        asm_append(&text_section, "    mov rax, 0  ; Error placeholder");
        return;
    }

    // Struct tipinin metadata'sını bul
    StructMetadata* meta = find_struct_metadata(var->tip);
    if (!meta) {
        sprintf(buffer, "    ; ERROR: Struct tipi '%s' bulunamadı!", var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Struct tipi '%s' bulunamadı!\n", var->tip);
        asm_append(&text_section, "    mov rax, 0  ; Error placeholder");
        return;
    }

    // Field'ı metadata'da bul
    StructFieldInfo* field = find_field_in_struct(meta, field_ad);
    if (!field) {
        sprintf(buffer, "    ; ERROR: Field '%s' struct '%s' içinde bulunamadı!", field_ad, var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Field '%s' struct '%s' içinde bulunamadı!\n", field_ad, var->tip);
        asm_append(&text_section, "    mov rax, 0  ; Error placeholder");
        return;
    }

    // Base adres + offset hesapla ve değeri RAX'e yükle
    sprintf(buffer, "    ; Struct base: %s, field offset: %d", var->asm_adresi, field->offset);
    asm_append(&text_section, buffer);

    // Base adresinden offset kadar ilerideki değeri RAX'e yükle
    // var->asm_adresi = "[rbp-X]", field offset ekleyerek "[rbp-X+offset]" elde et
    int base_offset;
    sscanf(var->asm_adresi, "[rbp-%d]", &base_offset);
    int actual_offset = base_offset - field->offset;  // Stack aşağı doğru büyür

    sprintf(buffer, "    mov rax, [rbp-%d]  ; Load %s.%s", actual_offset, struct_ad, field_ad);
    asm_append(&text_section, buffer);
}

// Struct Field Atama - p.x = 10
void visit_StructFieldAtama(ASTNode* node) {
    char* struct_ad = node->struct_field_atama_data.struct_ad->value;
    char* field_ad = node->struct_field_atama_data.field_ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Field Atama: %s.%s ---", struct_ad, field_ad);
    asm_append(&text_section, buffer);

    // Struct değişkenin base adresini bul
    Degisken* var = kapsam_degisken_bul(struct_ad);
    if (!var) {
        sprintf(buffer, "    ; ERROR: Değişken '%s' bulunamadı!", struct_ad);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Değişken '%s' bulunamadı!\n", struct_ad);
        return;
    }

    // Struct tipinin metadata'sını bul
    StructMetadata* meta = find_struct_metadata(var->tip);
    if (!meta) {
        sprintf(buffer, "    ; ERROR: Struct tipi '%s' bulunamadı!", var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Struct tipi '%s' bulunamadı!\n", var->tip);
        return;
    }

    // Field'ı metadata'da bul
    StructFieldInfo* field = find_field_in_struct(meta, field_ad);
    if (!field) {
        sprintf(buffer, "    ; ERROR: Field '%s' struct '%s' içinde bulunamadı!", field_ad, var->tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Field '%s' struct '%s' içinde bulunamadı!\n", field_ad, var->tip);
        return;
    }

    // Sağ taraftaki değeri hesapla (sonuç RAX'e gelir)
    visit(node->struct_field_atama_data.deger);

    // Base adres + offset hesapla ve RAX'teki değeri yaz
    sprintf(buffer, "    ; Struct base: %s, field offset: %d", var->asm_adresi, field->offset);
    asm_append(&text_section, buffer);

    // Base adresinden offset kadar ilerideki memory'ye RAX'i yaz
    int base_offset;
    sscanf(var->asm_adresi, "[rbp-%d]", &base_offset);
    int actual_offset = base_offset - field->offset;  // Stack aşağı doğru büyür

    sprintf(buffer, "    mov [rbp-%d], rax  ; Store to %s.%s", actual_offset, struct_ad, field_ad);
    asm_append(&text_section, buffer);
}

// Struct Değişken - Person p;
void visit_StructDegisken(ASTNode* node) {
    char* struct_tip = node->struct_degisken_data.struct_tip->value;
    char* degisken_ad = node->struct_degisken_data.ad->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Struct Değişken: %s %s ---", struct_tip, degisken_ad);
    asm_append(&text_section, buffer);

    // Struct metadata'yı bul
    StructMetadata* meta = find_struct_metadata(struct_tip);
    if (!meta) {
        sprintf(buffer, "    ; ERROR: Struct '%s' tanımlı değil!", struct_tip);
        asm_append(&text_section, buffer);
        fprintf(stderr, "HATA [Generator]: Struct '%s' tanımlı değil!\n", struct_tip);
        return;
    }

    // Stack'te struct için yer ayır (total_size kadar)
    // Her field için offset hesaplanmış, toplamda meta->total_size byte gerekli
    kapsam_yigin_ofseti += meta->total_size;
    char* adres = (char*)malloc(32);
    sprintf(adres, "[rbp-%d]", kapsam_yigin_ofseti);

    // Değişkeni scope'a kaydet (struct tipi ile)
    Degisken* d = &kapsam_haritasi[kapsam_degisken_sayisi++];
    d->ad = strdup(degisken_ad);
    d->asm_adresi = strdup(adres);
    d->tip = strdup(struct_tip);  // Tip olarak struct adını sakla
    d->scope_level = current_scope_level;
    d->is_global = false;

    sprintf(buffer, "    ; Struct boyutu: %d bytes, stack adres: %s", meta->total_size, adres);
    asm_append(&text_section, buffer);

    // Struct'ı sıfırla (opsiyonel, güvenlik için)
    sprintf(buffer, "    ; Struct memory'yi sıfırla");
    asm_append(&text_section, buffer);
    for (int offset = 0; offset < meta->total_size; offset += 8) {
        sprintf(buffer, "    mov qword [rbp-%d], 0", kapsam_yigin_ofseti - offset);
        asm_append(&text_section, buffer);
    }

    free(adres);
}

// ===== Phase 2: List Visitor Functions =====

void visit_ListTanimlama(ASTNode* node) {
    char* element_tip = node->list_tanimlama_data.element_tipi->value;
    char* degisken_adi = node->list_tanimlama_data.degisken_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Tanımlama: list[%s] %s ---", element_tip, degisken_adi);
    asm_append(&text_section, buffer);

    // Call list_create()
    asm_append(&text_section, "    call list_create");
    asm_append(&text_section, "    ; RAX now contains List pointer");

    // Store List pointer in variable
    char* adres = kapsam_degisken_yer_ayir(degisken_adi, "List");
    sprintf(buffer, "    mov %s, rax  ; Store List* in %s", adres, degisken_adi);
    asm_append(&text_section, buffer);
}

void visit_ListAdd(ASTNode* node) {
    char* list_adi = node->list_add_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Add: %s.add(value) ---", list_adi);
    asm_append(&text_section, buffer);

    // Evaluate value expression (result in RAX)
    visit(node->list_add_data.deger);
    asm_append(&text_section, "    push rax  ; Save value for list_add");

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi", list_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    pop rsi  ; Load value to rsi");
    asm_append(&text_section, "    call list_add");
}

void visit_ListGet(ASTNode* node) {
    char* list_adi = node->list_get_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Get: %s.get(index) ---", list_adi);
    asm_append(&text_section, buffer);

    // Evaluate index expression (result in RAX)
    visit(node->list_get_data.indeks);
    asm_append(&text_section, "    push rax  ; Save index");

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi", list_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    pop rsi  ; Load index to rsi (as int)");
    asm_append(&text_section, "    call list_get");
    asm_append(&text_section, "    ; RAX now contains element value");
}

void visit_ListSize(ASTNode* node) {
    char* list_adi = node->list_size_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Size: %s.size() ---", list_adi);
    asm_append(&text_section, buffer);

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi", list_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    call list_size");
    asm_append(&text_section, "    ; RAX now contains size");
}

void visit_ListClear(ASTNode* node) {
    char* list_adi = node->list_clear_data.list_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- List Clear: %s.clear() ---", list_adi);
    asm_append(&text_section, buffer);

    // Get list pointer
    char* list_adres = kapsam_degisken_adresi_bul(list_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load List* to rdi", list_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    call list_clear");
}

// ===== Phase 4: Hash Map Visit Functions =====

void visit_MapTanimlama(ASTNode* node) {
    char* key_tip = node->map_tanimlama_data.key_tipi->value;
    char* value_tip = node->map_tanimlama_data.value_tipi->value;
    char* degisken_adi = node->map_tanimlama_data.degisken_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Tanımlama: map[%s:%s] %s ---", key_tip, value_tip, degisken_adi);
    asm_append(&text_section, buffer);

    // Call hashmap_create(16) - initial capacity
    asm_append(&text_section, "    mov rdi, 16  ; Initial capacity");
    asm_append(&text_section, "    call hashmap_create");
    asm_append(&text_section, "    ; RAX now contains HashMap pointer");

    // Store HashMap pointer in variable
    char* adres = kapsam_degisken_yer_ayir(degisken_adi, "HashMap");
    sprintf(buffer, "    mov %s, rax  ; Store HashMap* in %s", adres, degisken_adi);
    asm_append(&text_section, buffer);
}

void visit_MapSet(ASTNode* node) {
    char* map_adi = node->map_set_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Set: %s.set(key, value) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate value expression (result in RAX)
    visit(node->map_set_data.value);
    asm_append(&text_section, "    push rax  ; Save value");

    // Evaluate key expression (result in RAX)
    visit(node->map_set_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    pop rdx  ; Load value to rdx");
    asm_append(&text_section, "    call hashmap_put");
}

void visit_MapGet(ASTNode* node) {
    char* map_adi = node->map_get_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Get: %s.get(key) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate key expression (result in RAX)
    visit(node->map_get_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    call hashmap_get");
    asm_append(&text_section, "    ; RAX now contains value (or NULL if not found)");
}

void visit_MapHas(ASTNode* node) {
    char* map_adi = node->map_has_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Has: %s.has(key) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate key expression (result in RAX)
    visit(node->map_has_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    call hashmap_contains");
    asm_append(&text_section, "    ; RAX now contains 1 or 0");
}

void visit_MapRemove(ASTNode* node) {
    char* map_adi = node->map_remove_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Remove: %s.remove(key) ---", map_adi);
    asm_append(&text_section, buffer);

    // Evaluate key expression (result in RAX)
    visit(node->map_remove_data.key);
    asm_append(&text_section, "    push rax  ; Save key");

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    pop rsi  ; Load key to rsi");
    asm_append(&text_section, "    call hashmap_remove");
}

void visit_MapSize(ASTNode* node) {
    char* map_adi = node->map_size_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Size: %s.size() ---", map_adi);
    asm_append(&text_section, buffer);

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    call hashmap_size");
    asm_append(&text_section, "    ; RAX now contains size");
}

void visit_MapClear(ASTNode* node) {
    char* map_adi = node->map_clear_data.map_adi->value;
    char buffer[256];

    sprintf(buffer, "    ; --- Map Clear: %s.clear() ---", map_adi);
    asm_append(&text_section, buffer);

    // Get map pointer
    char* map_adres = kapsam_degisken_adresi_bul(map_adi);
    sprintf(buffer, "    mov rdi, %s  ; Load HashMap* to rdi", map_adres);
    asm_append(&text_section, buffer);

    asm_append(&text_section, "    call hashmap_clear");
}

// Phase 3: Built-in function call
void visit_BuiltinCall(ASTNode* node) {
    TokenType func = node->builtin_call_data.function_type;
    char buffer[512];

    // Generate comment
    const char* func_name = "";
    switch (func) {
        case TOKEN_BUILTIN_READ_FILE: func_name = "read_file"; break;
        case TOKEN_BUILTIN_WRITE_FILE: func_name = "write_file"; break;
        case TOKEN_BUILTIN_APPEND_FILE: func_name = "append_file"; break;
        case TOKEN_BUILTIN_FILE_EXISTS: func_name = "file_exists"; break;
        case TOKEN_BUILTIN_FILE_SIZE: func_name = "file_size"; break;
        case TOKEN_BUILTIN_READ_LINES: func_name = "read_lines"; break;
        case TOKEN_BUILTIN_STRING_SPLIT: func_name = "string_split"; break;
        case TOKEN_BUILTIN_STRING_JOIN: func_name = "string_join"; break;
        case TOKEN_BUILTIN_STRING_REPLACE: func_name = "string_replace"; break;
        case TOKEN_BUILTIN_STRING_TRIM: func_name = "string_trim"; break;
        case TOKEN_BUILTIN_STRING_UPPER: func_name = "string_upper"; break;
        case TOKEN_BUILTIN_STRING_LOWER: func_name = "string_lower"; break;
        case TOKEN_BUILTIN_STRING_FIND: func_name = "string_find"; break;
        case TOKEN_BUILTIN_STRING_STARTS_WITH: func_name = "string_starts_with"; break;
        case TOKEN_BUILTIN_STRING_ENDS_WITH: func_name = "string_ends_with"; break;
        // Phase 4: Type Conversions
        case TOKEN_BUILTIN_INT_TO_STRING: func_name = "int_to_string"; break;
        case TOKEN_BUILTIN_STRING_TO_INT: func_name = "string_to_int"; break;
        case TOKEN_BUILTIN_CHAR_TO_STRING: func_name = "char_to_string"; break;
        case TOKEN_BUILTIN_STRING_CONCAT: func_name = "string_concat"; break;
        // Phase 4: Math Operations
        case TOKEN_BUILTIN_MATH_ABS: func_name = "math_abs"; break;
        case TOKEN_BUILTIN_MATH_MIN: func_name = "math_min"; break;
        case TOKEN_BUILTIN_MATH_MAX: func_name = "math_max"; break;
        case TOKEN_BUILTIN_MATH_POW: func_name = "math_pow"; break;
        // Phase 5.1: Enhanced String Operations
        case TOKEN_BUILTIN_STRING_LENGTH: func_name = "string_length"; break;
        case TOKEN_BUILTIN_STRING_SUBSTRING: func_name = "string_substring"; break;
        case TOKEN_BUILTIN_STRING_INDEX_OF: func_name = "string_index_of"; break;
        case TOKEN_BUILTIN_STRING_LAST_INDEX_OF: func_name = "string_last_index_of"; break;
        // Phase 5.2: Error Handling
        case TOKEN_BUILTIN_EXIT_WITH_CODE: func_name = "exit_with_code"; break;
        case TOKEN_BUILTIN_PANIC: func_name = "panic"; break;
        case TOKEN_BUILTIN_ASSERT: func_name = "mlp_assert"; break;
        case TOKEN_BUILTIN_GET_ERROR_CODE: func_name = "get_error_code"; break;
        case TOKEN_BUILTIN_SET_ERROR_CODE: func_name = "set_error_code"; break;
        // Phase 5.2: Memory Management
        case TOKEN_BUILTIN_MLP_MALLOC: func_name = "mlp_malloc"; break;
        case TOKEN_BUILTIN_MLP_FREE: func_name = "mlp_free"; break;
        case TOKEN_BUILTIN_MLP_REALLOC: func_name = "mlp_realloc"; break;
        case TOKEN_BUILTIN_MLP_CALLOC: func_name = "mlp_calloc"; break;
        case TOKEN_BUILTIN_GET_ALLOCATED_BYTES: func_name = "get_allocated_bytes"; break;
        case TOKEN_BUILTIN_CHECK_MEMORY_LEAKS: func_name = "check_memory_leaks"; break;
        // Phase 5.2: System Utilities
        case TOKEN_BUILTIN_GET_ENV: func_name = "get_env"; break;
        case TOKEN_BUILTIN_CURRENT_TIMESTAMP: func_name = "current_timestamp"; break;
        case TOKEN_BUILTIN_SLEEP_MS: func_name = "sleep_ms"; break;
        // Phase 5.3: Binary File I/O
        case TOKEN_BUILTIN_READ_BINARY: func_name = "read_binary"; break;
        case TOKEN_BUILTIN_WRITE_BINARY: func_name = "write_binary"; break;
        case TOKEN_BUILTIN_GET_FILE_INFO: func_name = "get_file_info"; break;
        case TOKEN_BUILTIN_COPY_FILE: func_name = "copy_file"; break;
        // Phase 5.3: Directory Operations
        case TOKEN_BUILTIN_LIST_DIRECTORY: func_name = "list_directory"; break;
        case TOKEN_BUILTIN_CREATE_DIRECTORY: func_name = "create_directory"; break;
        case TOKEN_BUILTIN_REMOVE_DIRECTORY: func_name = "remove_directory"; break;
        case TOKEN_BUILTIN_DIRECTORY_EXISTS: func_name = "directory_exists"; break;
        case TOKEN_BUILTIN_GET_CURRENT_DIR: func_name = "get_current_dir"; break;
        case TOKEN_BUILTIN_CHANGE_DIRECTORY: func_name = "change_directory"; break;
        default: func_name = "unknown"; break;
    }

    sprintf(buffer, "    ; --- Built-in: %s() ---", func_name);
    asm_append(&text_section, buffer);

    // Evaluate arguments and load into registers (x86-64 calling convention)
    // rdi = arg1, rsi = arg2, rdx = arg3

    if (node->builtin_call_data.arg1) {
        visit(node->builtin_call_data.arg1);  // Result in RAX
        asm_append(&text_section, "    mov rdi, rax  ; arg1");
    }

    if (node->builtin_call_data.arg2) {
        visit(node->builtin_call_data.arg2);  // Result in RAX
        asm_append(&text_section, "    mov rsi, rax  ; arg2");
    }

    if (node->builtin_call_data.arg3) {
        visit(node->builtin_call_data.arg3);  // Result in RAX
        asm_append(&text_section, "    mov rdx, rax  ; arg3");
    }

    // Call the built-in function
    sprintf(buffer, "    call %s", func_name);
    asm_append(&text_section, buffer);

    // Result is in RAX
}

void visit_DonusKomutu(ASTNode* node) {
    asm_append(&text_section, "    ; --- Donus Komutu ---");

    // 1. Döndürülecek ifadeyi hesapla (Sonuç RAX'e yüklenir)
    visit(node->tek_ifade_data.ifade);

    // 2. Fonksiyon Çıkışını (Epilog) üret
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");
}

/**
 * İkili İşlem (a + b) düğümünü ziyaret eder.
 * Sonuç her zaman RAX register'ında kalır.
 */
void visit_IkiliIslem(ASTNode* node) {
    // 1. Sağ tarafı (ifade 2) hesapla ve yığına (stack) it
    visit(node->ikili_islem_data.sag);
    asm_append(&text_section, "    push rax"); // Sağ tarafın sonucunu yığına kaydet

    // 2. Sol tarafı (ifade 1) hesapla (sonuç RAX'te kalır)
    visit(node->ikili_islem_data.sol);

    // 3. Sağ tarafı yığından çek (pop) ve işlemi yap
    asm_append(&text_section, "    pop rbx"); // Sağ tarafın sonucunu RBX'e al

    // Operatör tipine göre Assembly komutunu seç
    switch (node->ikili_islem_data.operator_type) {
        case TOKEN_PLUS:
            asm_append(&text_section, "    add rax, rbx"); // (Sol) + (Sağ)
            break;
        case TOKEN_MINUS:
            asm_append(&text_section, "    sub rax, rbx"); // (Sol) - (Sağ)
            break;
        case TOKEN_MUL:
            asm_append(&text_section, "    imul rax, rbx"); // (Sol) * (Sağ)
            break;
        case TOKEN_DIV:
            asm_append(&text_section, "    cqo"); // RAX'i RDX:RAX'e genişlet
            asm_append(&text_section, "    idiv rbx"); // (Sol) / (Sağ) -> Sonuç RAX'te
            break;
        
        // YENİ: KARŞILAŞTIRMA OPERATÖRLERİ
        case TOKEN_OP_ESIT_KARSILASTIRMA: // ==
            asm_append(&text_section, "    cmp rax, rbx"); // RAX ve RBX'i karşılaştır
            asm_append(&text_section, "    sete al");      // Eğer eşitse (Equal) AL = 1 yap
            asm_append(&text_section, "    movzx rax, al"); // RAX'i 1 veya 0 yap
            break;
        case TOKEN_NOT_ESIT: // !=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setne al");     // Eğer eşit değilse (Not Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_LT: // <
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setl al");      // Eğer küçükse (Less) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_GT: // >
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setg al");      // Eğer büyükse (Greater) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_GTE: // >=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setge al");     // Eğer büyük veya eşitse (Greater or Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;
        case TOKEN_LTE: // <=
            asm_append(&text_section, "    cmp rax, rbx");
            asm_append(&text_section, "    setle al");     // Eğer küçük veya eşitse (Less or Equal) AL = 1
            asm_append(&text_section, "    movzx rax, al");
            break;

        default:
            fprintf(stderr, "HATA [Generator]: Desteklenmeyen ikili operatör tipi: %d\n",
                node->ikili_islem_data.operator_type);
            exit(1);
    }
}

// Ana ziyaretçi fonksiyonu (Tüm düğüm tiplerini yönlendirir)
void visit(ASTNode* node) {
    if (node == NULL) return;
    
    switch (node->type) {
        case AST_BLOK:
            visit_Blok(node);
            break;
        case AST_SAYI:
            visit_Sayi(node);
            break;
        case AST_METIN:
            visit_Metin(node);
            break;
        case AST_YAZDIR_KOMUTU:
            visit_Yazdir(node);
            break;
        
        // YENİ: İkili İşlem
        case AST_IKILI_ISLEM:
            visit_IkiliIslem(node);
            break;
        
        // YENİ: Değişken Tanımlama
        case AST_DEGISKEN_TANIMLAMA:
            visit_DegiskenTanimlama(node);
            break;
            
        // YENİ: Değişken Okuma
        case AST_DEGISKEN:
            visit_Degisken(node);
            break;
            
        // YENİ: Değişken Güncelleme
        case AST_ATAMA_KOMUTU:
            visit_AtamaKomutu(node);
            break;
            
        // YENİ: Koşul (EĞER) Komutu
        case AST_KOSUL_KOMUTU:
            visit_KosulKomutu(node);
            break;
            
        // YENİ: Döngü Komutu (while)
        case AST_DONGU_KOMUTU:
            visit_DonguKomutu(node);
            break;
            
        // YENİ: For Komutu
        case AST_FOR_KOMUTU:
            visit_ForKomutu(node);
            break;
            
        // YENİ: Döngü Bitir Komutu (break)
        case AST_DONGU_BITIR_KOMUTU:
            visit_DonguBitirKomutu(node);
            break;

        // YENİ: Continue Komutu
        case AST_DONGU_DEVAM_KOMUTU:
            // TODO: Implement continue (jump to loop start)
            break;

        // YENİ: İşleç Tanımlama
        case AST_ISLEC_TANIMLAMA:
            visit_IslecTanimlama(node);
            break;

        // YENİ: İşleç Çağırma
        case AST_ISLEC_CAGIRMA:
            visit_IslecCagirma(node);
            break;
            
        // YENİ: Dönüş Komutu
        case AST_DONUS_KOMUTU:
            visit_DonusKomutu(node);
            break;

        // Array Tanımlama
        case AST_ARRAY_TANIMLAMA:
            visit_ArrayTanimlama(node);
            break;

        // Array Erişim
        case AST_ARRAY_ERISIM:
            visit_ArrayErisim(node);
            break;

        // Array Atama
        case AST_ARRAY_ATAMA:
            visit_ArrayAtama(node);
            break;

        // Struct Tanımlama
        case AST_STRUCT_TANIMLAMA:
            visit_StructTanimlama(node);
            break;

        // Struct Field Access
        case AST_STRUCT_FIELD_ACCESS:
            visit_StructFieldAccess(node);
            break;

        // Struct Field Atama
        case AST_STRUCT_FIELD_ATAMA:
            visit_StructFieldAtama(node);
            break;

        // Struct Değişken
        case AST_STRUCT_DEGISKEN:
            visit_StructDegisken(node);
            break;

        // Phase 2: Dynamic Lists
        case AST_LIST_TANIMLAMA:
            visit_ListTanimlama(node);
            break;

        case AST_LIST_ADD:
            visit_ListAdd(node);
            break;

        case AST_LIST_GET:
            visit_ListGet(node);
            break;

        case AST_LIST_SIZE:
            visit_ListSize(node);
            break;

        case AST_LIST_CLEAR:
            visit_ListClear(node);
            break;

        // Phase 4: Hash Map operations
        case AST_MAP_TANIMLAMA:
            visit_MapTanimlama(node);
            break;

        case AST_MAP_SET:
            visit_MapSet(node);
            break;

        case AST_MAP_GET:
            visit_MapGet(node);
            break;

        case AST_MAP_HAS:
            visit_MapHas(node);
            break;

        case AST_MAP_REMOVE:
            visit_MapRemove(node);
            break;

        case AST_MAP_SIZE:
            visit_MapSize(node);
            break;

        case AST_MAP_CLEAR:
            visit_MapClear(node);
            break;

        // Phase 3: Built-in function call
        case AST_BUILTIN_CALL:
            visit_BuiltinCall(node);
            break;

        default:
            fprintf(stderr, "HATA [Generator]: Tanınmayan AST düğüm tipi: %d\n", node->type);
            exit(1);
    }
}

// --- Ana Fonksiyon ---

// AsmCode yapısı için ayrılan belleği serbest bırakır
void free_asm_code(AsmCode* section) {
    free(section->code);
}

char* generate_asm(ASTNode* root) {
    // Kapsamı sıfırla
    kapsam_degisken_sayisi = 0;
    kapsam_yigin_ofseti = 0;
    etiket_sayaci = 0;
    metin_sayaci = 0;
    aktif_dongu_son_etiketi = -1;

    // Initialize sections
    data_section.code = NULL;
    data_section.size = 0;
    data_section.capacity = 0;

    text_section.code = NULL;
    text_section.size = 0;
    text_section.capacity = 0;

    // 1. .data bölümü
    asm_append(&data_section, "extern printf");
    asm_append(&data_section, "extern strlen");      // ✅ Ekle
    asm_append(&data_section, "extern strcmp");      // ✅ Ekle
    asm_append(&data_section, "extern strstr");      // ✅ Ekle
    asm_append(&data_section, "extern tyd_substr");  // ✅ Ekle
    asm_append(&data_section, "extern tyd_strcat");  // ✅ Ekle
    asm_append(&data_section, "extern dosya_ac");
    asm_append(&data_section, "extern dosya_oku");
    asm_append(&data_section, "extern dosya_yaz");
    asm_append(&data_section, "extern dosya_kapat");
    asm_append(&data_section, "extern string_karakter_al");
    asm_append(&data_section, "extern string_alt");
    asm_append(&data_section, "extern karakter_kodu");
    asm_append(&data_section, "extern kodu_karaktere");
    asm_append(&data_section, "extern runtime_dizin_al");
    asm_append(&data_section, "extern runtime_dizin_al"); // Self-host için eklendi
    asm_append(&data_section, "extern tyd_fix_cwd"); // ✅ yeni

    // String fonksiyonları (runtime.c'deki wrapperlar)
    asm_append(&data_section, "extern string_birlestir");
    asm_append(&data_section, "extern string_karsilastir");
    asm_append(&data_section, "extern string_uzunluk");
    asm_append(&data_section, "extern string_esit_mi");
    asm_append(&data_section, "extern string_karakter_al");
    asm_append(&data_section, "extern string_alt");

    // GUI functions (gui_mock.c or real GUI backend)
    asm_append(&data_section, "extern gui_window_create");
    asm_append(&data_section, "extern gui_window_show");
    asm_append(&data_section, "extern gui_window_hide");
    asm_append(&data_section, "extern gui_window_destroy");
    asm_append(&data_section, "extern gui_button_create");
    asm_append(&data_section, "extern gui_label_create");
    asm_append(&data_section, "extern gui_label_set_text");
    asm_append(&data_section, "extern gui_poll_event");
    asm_append(&data_section, "extern gui_get_clicked_button");
    asm_append(&data_section, "extern gui_canvas_create");
    asm_append(&data_section, "extern gui_canvas_clear");
    asm_append(&data_section, "extern gui_canvas_set_color");
    asm_append(&data_section, "extern gui_canvas_draw_rect");
    asm_append(&data_section, "extern gui_canvas_draw_circle");
    asm_append(&data_section, "extern gui_canvas_render");
    asm_append(&data_section, "extern int_to_string");

    // Phase 5.2: Error Handling
    asm_append(&data_section, "extern exit_with_code");
    asm_append(&data_section, "extern panic");
    asm_append(&data_section, "extern mlp_assert");
    asm_append(&data_section, "extern get_error_code");
    asm_append(&data_section, "extern set_error_code");

    // Phase 5.2: Memory Management
    asm_append(&data_section, "extern mlp_malloc");
    asm_append(&data_section, "extern mlp_free");
    asm_append(&data_section, "extern mlp_realloc");
    asm_append(&data_section, "extern mlp_calloc");
    asm_append(&data_section, "extern get_allocated_bytes");
    asm_append(&data_section, "extern check_memory_leaks");

    // Phase 5.2: System Utilities
    asm_append(&data_section, "extern get_env");
    asm_append(&data_section, "extern current_timestamp");
    asm_append(&data_section, "extern sleep_ms");

    // Phase 5.3: Binary File I/O
    asm_append(&data_section, "extern read_binary");
    asm_append(&data_section, "extern write_binary");
    asm_append(&data_section, "extern get_file_info");
    asm_append(&data_section, "extern copy_file");

    // Phase 5.3: Directory Operations
    asm_append(&data_section, "extern list_directory");
    asm_append(&data_section, "extern create_directory");
    asm_append(&data_section, "extern remove_directory");
    asm_append(&data_section, "extern directory_exists");
    asm_append(&data_section, "extern get_current_dir");
    asm_append(&data_section, "extern change_directory");

    asm_append(&data_section, "section .data");
    asm_append(&data_section, "    format_sayi db \"%ld\", 10, 0"); // %d -> %ld
    asm_append(&data_section, "    format_metin db \"%s\", 10, 0");

    // 2. .text bölümü başlangıcı
    asm_append(&text_section, "section .text");
    asm_append(&text_section, "global main");

    // 3. Ana program girişi (Prolog)
    asm_append(&text_section, "main:");
    asm_append(&text_section, "    push rbp");
    asm_append(&text_section, "    mov rbp, rsp");
    asm_append(&text_section, "    sub rsp, 256"); // ✅ Yeterli stack alanı (32 değişkene kadar)
    asm_append(&text_section, "    call tyd_fix_cwd"); // ✅ çalışma dizinini düzelt

    // 4. İki geçişli ziyaret
    ASTNode* blok = root;
    if (blok->type == AST_BLOK) {
        // İlk geçiş: Ana program komutları (fonksiyon tanımları hariç)
        asm_append(&text_section, "    ; --- Ana Program Akışı ---");
        for (int i = 0; i < blok->blok_data.sayisi; i++) {
            ASTNode* node = blok->blok_data.komutlar[i];
            if (node->type != AST_ISLEC_TANIMLAMA) {
                visit(node);
            }
        }
    } else {
        visit(root);
    }

    // 5. Program Çıkışı (Epilog)
    asm_append(&text_section, "    ; --- Program Sonu ---");
    asm_append(&text_section, "    xor rax, rax");
    asm_append(&text_section, "    mov rsp, rbp");
    asm_append(&text_section, "    pop rbp");
    asm_append(&text_section, "    ret");

    // 6. İkinci geçiş: Fonksiyon tanımları
    if (blok->type == AST_BLOK) {
        asm_append(&text_section, "");
        asm_append(&text_section, "; === Fonksiyon Tanımları ===");
        for (int i = 0; i < blok->blok_data.sayisi; i++) {
            ASTNode* node = blok->blok_data.komutlar[i];
            if (node->type == AST_ISLEC_TANIMLAMA) {
                visit(node);
            }
        }
    }

    // 7. Tüm bölümleri birleştir
    size_t stack_section_size = strlen("\nsection .note.GNU-stack noalloc noexec nowrite progbits\n\ndb 0\n");
    size_t total_size = data_section.size + text_section.size + stack_section_size + 2;
    char* final_code = (char*)malloc(total_size);
    if (final_code == NULL) {
        perror("Hafıza ayırma hatası (final assembly)");
        exit(EXIT_FAILURE);
    }

    strcpy(final_code, data_section.code);
    strcat(final_code, "\n");
    strcat(final_code, text_section.code);

    // ✅ NASM uyumlu stack notu + 1 byte içerik ekle (boş bölüm strip olmasın)
    strcat(final_code, "\nsection .note.GNU-stack noalloc noexec nowrite progbits\n");
    strcat(final_code, "\ndb 0\n");

    free_asm_code(&data_section);
    free_asm_code(&text_section);
    kapsam_temizle();

    return final_code;
}

