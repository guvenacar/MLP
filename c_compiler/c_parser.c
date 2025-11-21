// c_compiler/c_parser.c
// (KILAVUZA UYUMLU SÜRÜM: Tanımlamalar hariç ; YOK)

#include "c_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Parser Durum Yönetimi ---
static Token* current_token = NULL;

// Token tip ismini döndüren helper fonksiyon
static const char* getTokenTypeName(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_SAYI: return "SAYI";
        case TOKEN_METIN: return "METIN";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_TANIMLA_SAYI: return "SAYISAL";
        case TOKEN_TANIMLA_METIN: return "METIN (tip)";
        case TOKEN_TANIMLA_BOOL: return "BOOL";
        case TOKEN_YAPI_YAZDIR: return "YAZDIR";
        case TOKEN_YAPI_KOSUL_EGER: return "EĞER";
        case TOKEN_YAPI_KOSUL_ISE: return "İSE";
        case TOKEN_YAPI_KOSUL_DEGILSE: return "DEĞİLSE";
        case TOKEN_YAPI_ISLEC: return "İŞLEÇ";
        case TOKEN_YAPI_DONUS: return "DÖNÜŞ";
        case TOKEN_YAPI_DONGU: return "DÖNGÜ";
        case TOKEN_YAPI_DO: return "DO";
        case TOKEN_YAPI_FOR: return "FOR";
        case TOKEN_YAPI_TO: return "TO";
        case TOKEN_YAPI_STEP: return "STEP";
        case TOKEN_YAPI_SON: return "SON";
        case TOKEN_LEFT_PAREN: return "(";
        case TOKEN_RIGHT_PAREN: return ")";
        case TOKEN_COMMA: return ",";
        case TOKEN_ASSIGN: return "=";
        case TOKEN_SEMICOLON: return ";";
        case TOKEN_PLUS: return "+";
        case TOKEN_MINUS: return "-";
        case TOKEN_MUL: return "*";
        case TOKEN_DIV: return "/";
        case TOKEN_GT: return ">";
        case TOKEN_LT: return "<";
        case TOKEN_OP_ESIT_KARSILASTIRMA: return "==";
        case TOKEN_NOT_ESIT: return "!=";
        case TOKEN_GTE: return ">=";
        case TOKEN_LTE: return "<=";
        case TOKEN_MOD: return "%";
        case TOKEN_AND: return "and";
        case TOKEN_OR: return "or";
        case TOKEN_NOT: return "not";
        case TOKEN_BITWISE_AND: return "&";
        case TOKEN_BITWISE_OR: return "|";
        case TOKEN_BITWISE_XOR: return "^";
        case TOKEN_BITWISE_NOT: return "~";
        case TOKEN_LSHIFT: return "<<";
        case TOKEN_RSHIFT: return ">>";
        case TOKEN_QUESTION: return "?";
        case TOKEN_BREAK: return "break";
        case TOKEN_CONTINUE: return "continue";
        case TOKEN_STOP: return "stop";
        case TOKEN_GOTO: return "goto";
        default: return "UNKNOWN";
    }
}

// --- Hata Yönetimi ---
void parseError(const char* message, const char* expected) {
    fprintf(stderr, "\n╔════════════════════════════════════════════════════════════╗\n");
    fprintf(stderr, "║ HATA [Parser]: Sözdizimi Hatası!                          ║\n");
    fprintf(stderr, "╚════════════════════════════════════════════════════════════╝\n\n");

    if (current_token != NULL) {
        fprintf(stderr, "📍 Konum: Satır %d, Kolon %d\n\n", current_token->line, current_token->column);
        fprintf(stderr, "❌ Bulunan: ");
        if (current_token->value) {
            fprintf(stderr, "\"%s\" (%s)\n", current_token->value, getTokenTypeName(current_token->type));
        } else {
            fprintf(stderr, "%s\n", getTokenTypeName(current_token->type));
        }
    } else {
        fprintf(stderr, "📍 Konum: Dosya sonu\n\n");
        fprintf(stderr, "❌ Bulunan: Dosya Sonu veya NULL\n");
    }

    fprintf(stderr, "✓ Beklenen: %s\n\n", expected);

    if (message && strlen(message) > 0) {
        fprintf(stderr, "💡 Açıklama: %s\n\n", message);
    }

    exit(EXIT_FAILURE);
}

// Kural Kontrolü (Hata ayıklama için)
void specs_check_no_semicolon(const char* komut_adi) {
    if (current_token && current_token->type == TOKEN_SEMICOLON) {
        fprintf(stderr, "SPECS.md İhlali: %s sonunda ';' kullanılamaz!\n", komut_adi);
        exit(1);
    }
}

// --- Token Yönetimi ---
void consume(TokenType expected_type) {
    if (current_token == NULL) {
        parseError("Dosya beklenenden önce bitti.", "Herhangi bir Token");
    }
    if (current_token->type == expected_type) {
        if (current_token->value != NULL) {
            free(current_token->value);
        }
        free(current_token);
        current_token = getNextToken();
    } else {
        char expected_str[100];
        sprintf(expected_str, "%s (%s)", getTokenTypeName(expected_type),
                expected_type == TOKEN_SEMICOLON ? "SAYISAL, METIN, BOOL değişken tanımlarında gerekli" : "Token");
        parseError("Token tipi uyuşmuyor.", expected_str);
    }
}

// --- AST Düğümü Oluşturucular ---
// (Dosyanızda bulunan tüm createAST_... fonksiyonları buradadır)
// (createAST_Sayi, createAST_Metin, createAST_Degisken, createAST_IslecCagirma, 
//  createAST_DegiskenTanimlama, createAST_KosulKomutu, createAST_IslecTanimlama...)

ASTNode* createAST_Sayi(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_SAYI;
    node->sabit_data.deger = (token->value != NULL) ? strdup(token->value) : NULL;
    return node;
}

ASTNode* createAST_Metin(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_METIN;
    node->sabit_data.deger = (token->value != NULL) ? strdup(token->value) : NULL;
    return node;
}

ASTNode* createAST_Degisken(Token* token) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_DEGISKEN;
    node->degisken_data.ad = (Token*)malloc(sizeof(Token));
    node->degisken_data.ad->type = token->type;
    node->degisken_data.ad->value = strdup(token->value);
    return node;
}

ASTNode* createAST_IslecCagirma(Token* ad_token, ASTNode** argumanlar, int arguman_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ISLEC_CAGIRMA;
    node->islec_cagirma_data.hedef_ad = (Token*)malloc(sizeof(Token));
    node->islec_cagirma_data.hedef_ad->type = ad_token->type;
    node->islec_cagirma_data.hedef_ad->value = strdup(ad_token->value);
    node->islec_cagirma_data.argumanlar = argumanlar;
    node->islec_cagirma_data.arguman_sayisi = arguman_sayisi;
    return node;
}

ASTNode* createAST_DegiskenTanimlama(Token* tip, Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_DEGISKEN_TANIMLAMA;
    node->tanimlama_data.tip = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.tip->type = tip->type;
    node->tanimlama_data.tip->value = strdup(tip->value);
    node->tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->tanimlama_data.ad->type = ad->type;
    node->tanimlama_data.ad->value = strdup(ad->value);
    node->tanimlama_data.ifade = ifade;
    node->tanimlama_data.is_const = false; // Default: not const
    return node;
}

ASTNode* createAST_AtamaKomutu(Token* ad, ASTNode* ifade) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ATAMA_KOMUTU;
    node->atama_data.ad = (Token*)malloc(sizeof(Token));
    node->atama_data.ad->type = ad->type;
    node->atama_data.ad->value = strdup(ad->value);
    node->atama_data.ifade = ifade;
    return node;
}

ASTNode* createAST_ArrayTanimlama(Token* tip, Token* ad, ASTNode* boyut) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_TANIMLAMA;
    node->array_tanimlama_data.tip = (Token*)malloc(sizeof(Token));
    node->array_tanimlama_data.tip->type = tip->type;
    node->array_tanimlama_data.tip->value = strdup(tip->value);
    node->array_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->array_tanimlama_data.ad->type = ad->type;
    node->array_tanimlama_data.ad->value = strdup(ad->value);
    node->array_tanimlama_data.boyut = boyut;
    return node;
}

ASTNode* createAST_ArrayErisim(Token* ad, ASTNode* indeks) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_ERISIM;
    node->array_erisim_data.ad = (Token*)malloc(sizeof(Token));
    node->array_erisim_data.ad->type = ad->type;
    node->array_erisim_data.ad->value = strdup(ad->value);
    node->array_erisim_data.indeks = indeks;
    return node;
}

ASTNode* createAST_ArrayAtama(Token* ad, ASTNode* indeks, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ARRAY_ATAMA;
    node->array_atama_data.ad = (Token*)malloc(sizeof(Token));
    node->array_atama_data.ad->type = ad->type;
    node->array_atama_data.ad->value = strdup(ad->value);
    node->array_atama_data.indeks = indeks;
    node->array_atama_data.deger = deger;
    return node;
}

// Struct Tanımlama (YAPI Nokta İSE SAYISAL x; SAYISAL y; SON)
ASTNode* createAST_StructTanimlama(Token* ad, Token** field_tipleri, Token** field_adlari, int field_sayisi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_TANIMLAMA;
    node->struct_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->struct_tanimlama_data.ad->type = ad->type;
    node->struct_tanimlama_data.ad->value = strdup(ad->value);
    node->struct_tanimlama_data.field_tipleri = field_tipleri;
    node->struct_tanimlama_data.field_adlari = field_adlari;
    node->struct_tanimlama_data.field_sayisi = field_sayisi;
    return node;
}

// Struct Field Access (p.x)
ASTNode* createAST_StructFieldAccess(Token* struct_ad, Token* field_ad) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_FIELD_ACCESS;
    node->struct_field_access_data.struct_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_access_data.struct_ad->type = struct_ad->type;
    node->struct_field_access_data.struct_ad->value = strdup(struct_ad->value);
    node->struct_field_access_data.field_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_access_data.field_ad->type = field_ad->type;
    node->struct_field_access_data.field_ad->value = strdup(field_ad->value);
    return node;
}

// Struct Field Atama (p.x = 10;)
ASTNode* createAST_StructFieldAtama(Token* struct_ad, Token* field_ad, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_FIELD_ATAMA;
    node->struct_field_atama_data.struct_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_atama_data.struct_ad->type = struct_ad->type;
    node->struct_field_atama_data.struct_ad->value = strdup(struct_ad->value);
    node->struct_field_atama_data.field_ad = (Token*)malloc(sizeof(Token));
    node->struct_field_atama_data.field_ad->type = field_ad->type;
    node->struct_field_atama_data.field_ad->value = strdup(field_ad->value);
    node->struct_field_atama_data.deger = deger;
    return node;
}

// Struct Değişken (Nokta p;)
ASTNode* createAST_StructDegisken(Token* struct_tip, Token* ad) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_STRUCT_DEGISKEN;
    node->struct_degisken_data.struct_tip = (Token*)malloc(sizeof(Token));
    node->struct_degisken_data.struct_tip->type = struct_tip->type;
    node->struct_degisken_data.struct_tip->value = strdup(struct_tip->value);
    node->struct_degisken_data.ad = (Token*)malloc(sizeof(Token));
    node->struct_degisken_data.ad->type = ad->type;
    node->struct_degisken_data.ad->value = strdup(ad->value);
    return node;
}

// ===== Phase 2: List AST Creation Helpers =====

ASTNode* createAST_ListTanimlama(Token* element_tipi, Token* degisken_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_TANIMLAMA;
    node->list_tanimlama_data.element_tipi = element_tipi;  // Direct assignment
    node->list_tanimlama_data.degisken_adi = degisken_adi;  // Direct assignment
    return node;
}

ASTNode* createAST_ListAdd(Token* list_adi, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_ADD;
    node->list_add_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_add_data.list_adi->type = list_adi->type;
    node->list_add_data.list_adi->value = strdup(list_adi->value);
    node->list_add_data.deger = deger;
    return node;
}

ASTNode* createAST_ListGet(Token* list_adi, ASTNode* indeks) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_GET;
    node->list_get_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_get_data.list_adi->type = list_adi->type;
    node->list_get_data.list_adi->value = strdup(list_adi->value);
    node->list_get_data.indeks = indeks;
    return node;
}

ASTNode* createAST_ListSize(Token* list_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_SIZE;
    node->list_size_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_size_data.list_adi->type = list_adi->type;
    node->list_size_data.list_adi->value = strdup(list_adi->value);
    return node;
}

ASTNode* createAST_ListClear(Token* list_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_CLEAR;
    node->list_clear_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_clear_data.list_adi->type = list_adi->type;
    node->list_clear_data.list_adi->value = strdup(list_adi->value);
    return node;
}

// Phase 6: New List methods
ASTNode* createAST_ListSet(Token* list_adi, ASTNode* indeks, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_SET;
    node->list_set_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_set_data.list_adi->type = list_adi->type;
    node->list_set_data.list_adi->value = strdup(list_adi->value);
    node->list_set_data.indeks = indeks;
    node->list_set_data.deger = deger;
    return node;
}

ASTNode* createAST_ListRemove(Token* list_adi, ASTNode* indeks) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_REMOVE;
    node->list_remove_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_remove_data.list_adi->type = list_adi->type;
    node->list_remove_data.list_adi->value = strdup(list_adi->value);
    node->list_remove_data.indeks = indeks;
    return node;
}

ASTNode* createAST_ListInsert(Token* list_adi, ASTNode* indeks, ASTNode* deger) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_LIST_INSERT;
    node->list_insert_data.list_adi = (Token*)malloc(sizeof(Token));
    node->list_insert_data.list_adi->type = list_adi->type;
    node->list_insert_data.list_adi->value = strdup(list_adi->value);
    node->list_insert_data.indeks = indeks;
    node->list_insert_data.deger = deger;
    return node;
}

// Phase 3: Built-in function call
ASTNode* createAST_BuiltinCall(TokenType func_type, ASTNode* arg1, ASTNode* arg2, ASTNode* arg3) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_BUILTIN_CALL;
    node->builtin_call_data.function_type = func_type;
    node->builtin_call_data.arg1 = arg1;
    node->builtin_call_data.arg2 = arg2;
    node->builtin_call_data.arg3 = arg3;
    return node;
}

// ===== Phase 4: Hash Map Helper Functions =====

ASTNode* createAST_MapTanimlama(Token* key_tipi, Token* value_tipi, Token* degisken_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_TANIMLAMA;
    node->map_tanimlama_data.key_tipi = (Token*)malloc(sizeof(Token));
    node->map_tanimlama_data.key_tipi->type = key_tipi->type;
    node->map_tanimlama_data.key_tipi->value = strdup(key_tipi->value);
    node->map_tanimlama_data.value_tipi = (Token*)malloc(sizeof(Token));
    node->map_tanimlama_data.value_tipi->type = value_tipi->type;
    node->map_tanimlama_data.value_tipi->value = strdup(value_tipi->value);
    node->map_tanimlama_data.degisken_adi = (Token*)malloc(sizeof(Token));
    node->map_tanimlama_data.degisken_adi->type = degisken_adi->type;
    node->map_tanimlama_data.degisken_adi->value = strdup(degisken_adi->value);
    return node;
}

ASTNode* createAST_MapSet(Token* map_adi, ASTNode* key, ASTNode* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_SET;
    node->map_set_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_set_data.map_adi->type = map_adi->type;
    node->map_set_data.map_adi->value = strdup(map_adi->value);
    node->map_set_data.key = key;
    node->map_set_data.value = value;
    return node;
}

ASTNode* createAST_MapGet(Token* map_adi, ASTNode* key) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_GET;
    node->map_get_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_get_data.map_adi->type = map_adi->type;
    node->map_get_data.map_adi->value = strdup(map_adi->value);
    node->map_get_data.key = key;
    return node;
}

ASTNode* createAST_MapHas(Token* map_adi, ASTNode* key) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_HAS;
    node->map_has_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_has_data.map_adi->type = map_adi->type;
    node->map_has_data.map_adi->value = strdup(map_adi->value);
    node->map_has_data.key = key;
    return node;
}

ASTNode* createAST_MapRemove(Token* map_adi, ASTNode* key) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_REMOVE;
    node->map_remove_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_remove_data.map_adi->type = map_adi->type;
    node->map_remove_data.map_adi->value = strdup(map_adi->value);
    node->map_remove_data.key = key;
    return node;
}

ASTNode* createAST_MapSize(Token* map_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_SIZE;
    node->map_size_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_size_data.map_adi->type = map_adi->type;
    node->map_size_data.map_adi->value = strdup(map_adi->value);
    return node;
}

ASTNode* createAST_MapClear(Token* map_adi) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_MAP_CLEAR;
    node->map_clear_data.map_adi = (Token*)malloc(sizeof(Token));
    node->map_clear_data.map_adi->type = map_adi->type;
    node->map_clear_data.map_adi->value = strdup(map_adi->value);
    return node;
}

ASTNode* createAST_KosulKomutu(ASTNode* kosul, ASTNode* ise_blok, ASTNode* degilse_blok) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_KOSUL_KOMUTU;
    node->kosul_data.kosul = kosul;
    node->kosul_data.ise_blok = ise_blok;
    node->kosul_data.degilse_blok = degilse_blok;
    return node;
}

ASTNode* createAST_IslecTanimlama(Token* ad, Token** parametreler, int parametre_sayisi, ASTNode* govde) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) return NULL;
    node->type = AST_ISLEC_TANIMLAMA;
    node->islec_tanimlama_data.ad = (Token*)malloc(sizeof(Token));
    node->islec_tanimlama_data.ad->type = ad->type;
    node->islec_tanimlama_data.ad->value = strdup(ad->value);
    node->islec_tanimlama_data.parametreler = parametreler;
    node->islec_tanimlama_data.parametre_sayisi = parametre_sayisi;
    node->islec_tanimlama_data.govde = govde;
    return node;
}


// --- Ayrıştırma Fonksiyonları (İleri Bildirimler) ---
ASTNode* ifade();
ASTNode* birincil();
ASTNode* ikili_islem(int onceki_oncelik);
ASTNode* komut();
ASTNode* blok();
ASTNode* kosul_komutu();
ASTNode* dongu_komutu();
ASTNode* for_komutu();
ASTNode* islec_tanimlama();
ASTNode* donus_komutu();
ASTNode* list_tanimlama_parse();  // Phase 6: List<T> tanımlama parser


int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_QUESTION:  // Phase 5.8: Ternary operator (?:)
            return 2;
        case TOKEN_OR:
            return 3;
        case TOKEN_AND:
            return 4;
        case TOKEN_OP_ESIT_KARSILASTIRMA:
        case TOKEN_GT:
        case TOKEN_LT:
        case TOKEN_GTE:
        case TOKEN_LTE:
        case TOKEN_NOT_ESIT:
            return 5;
        case TOKEN_BITWISE_OR:
            return 6;
        case TOKEN_BITWISE_XOR:
            return 7;
        case TOKEN_BITWISE_AND:
            return 8;
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return 10;
        case TOKEN_LSHIFT:
        case TOKEN_RSHIFT:
            return 11;
        case TOKEN_MUL:
        case TOKEN_DIV:
        case TOKEN_MOD:
            return 20;
        default:
            return 0;
    }
}

ASTNode* ifade() {
    return ikili_islem(0);
}

ASTNode* birincil() {
    if (current_token == NULL) {
        parseError("Bir ifade bekleniyordu.", "Sayı, Metin veya Değişken");
    }

    if (current_token->type == TOKEN_SAYI) {
        ASTNode* node = createAST_Sayi(current_token);
        consume(TOKEN_SAYI);
        return node;
    }

    if (current_token->type == TOKEN_METIN) {
        ASTNode* node = createAST_Metin(current_token);
        consume(TOKEN_METIN);
        return node;
    }

    if (current_token->type == TOKEN_LEFT_PAREN) {
        consume(TOKEN_LEFT_PAREN);
        ASTNode* node = ifade();
        if (current_token->type != TOKEN_RIGHT_PAREN) {
            parseError("Kapanış Parantezi bekleniyordu.", ")");
        }
        consume(TOKEN_RIGHT_PAREN);
        return node;
    }

    // Phase 3: Built-in function calls
    if (current_token->type == TOKEN_BUILTIN_READ_FILE ||
        current_token->type == TOKEN_BUILTIN_WRITE_FILE ||
        current_token->type == TOKEN_BUILTIN_APPEND_FILE ||
        current_token->type == TOKEN_BUILTIN_FILE_EXISTS ||
        current_token->type == TOKEN_BUILTIN_FILE_SIZE ||
        current_token->type == TOKEN_BUILTIN_READ_LINES ||
        current_token->type == TOKEN_BUILTIN_STRING_SPLIT ||
        current_token->type == TOKEN_BUILTIN_STRING_JOIN ||
        current_token->type == TOKEN_BUILTIN_STRING_REPLACE ||
        current_token->type == TOKEN_BUILTIN_STRING_TRIM ||
        current_token->type == TOKEN_BUILTIN_STRING_UPPER ||
        current_token->type == TOKEN_BUILTIN_STRING_LOWER ||
        current_token->type == TOKEN_BUILTIN_STRING_FIND ||
        current_token->type == TOKEN_BUILTIN_STRING_STARTS_WITH ||
        current_token->type == TOKEN_BUILTIN_STRING_ENDS_WITH ||
        // Phase 4: Type Conversions
        current_token->type == TOKEN_BUILTIN_INT_TO_STRING ||
        current_token->type == TOKEN_BUILTIN_STRING_TO_INT ||
        current_token->type == TOKEN_BUILTIN_CHAR_TO_STRING ||
        current_token->type == TOKEN_BUILTIN_STRING_CONCAT ||
        // Phase 4: Math Operations
        current_token->type == TOKEN_BUILTIN_MATH_ABS ||
        current_token->type == TOKEN_BUILTIN_MATH_MIN ||
        current_token->type == TOKEN_BUILTIN_MATH_MAX ||
        current_token->type == TOKEN_BUILTIN_MATH_POW ||
        // Phase 5.1: Enhanced String Operations
        current_token->type == TOKEN_BUILTIN_STRING_LENGTH ||
        current_token->type == TOKEN_BUILTIN_STRING_SUBSTRING ||
        current_token->type == TOKEN_BUILTIN_STRING_INDEX_OF ||
        current_token->type == TOKEN_BUILTIN_STRING_LAST_INDEX_OF ||
        // Phase 5.2: Error Handling
        current_token->type == TOKEN_BUILTIN_EXIT_WITH_CODE ||
        current_token->type == TOKEN_BUILTIN_PANIC ||
        current_token->type == TOKEN_BUILTIN_ASSERT ||
        current_token->type == TOKEN_BUILTIN_GET_ERROR_CODE ||
        current_token->type == TOKEN_BUILTIN_SET_ERROR_CODE ||
        // Phase 5.2: Memory Management
        current_token->type == TOKEN_BUILTIN_MLP_MALLOC ||
        current_token->type == TOKEN_BUILTIN_MLP_FREE ||
        current_token->type == TOKEN_BUILTIN_MLP_REALLOC ||
        current_token->type == TOKEN_BUILTIN_MLP_CALLOC ||
        current_token->type == TOKEN_BUILTIN_GET_ALLOCATED_BYTES ||
        current_token->type == TOKEN_BUILTIN_CHECK_MEMORY_LEAKS ||
        // Phase 5.2: System Utilities
        current_token->type == TOKEN_BUILTIN_GET_ENV ||
        current_token->type == TOKEN_BUILTIN_CURRENT_TIMESTAMP ||
        current_token->type == TOKEN_BUILTIN_SLEEP_MS ||
        // Phase 5.3: Binary File I/O
        current_token->type == TOKEN_BUILTIN_READ_BINARY ||
        current_token->type == TOKEN_BUILTIN_WRITE_BINARY ||
        current_token->type == TOKEN_BUILTIN_GET_FILE_INFO ||
        current_token->type == TOKEN_BUILTIN_COPY_FILE ||
        // Phase 5.3: Directory Operations
        current_token->type == TOKEN_BUILTIN_LIST_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_CREATE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_REMOVE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_DIRECTORY_EXISTS ||
        current_token->type == TOKEN_BUILTIN_GET_CURRENT_DIR ||
        current_token->type == TOKEN_BUILTIN_CHANGE_DIRECTORY ||
        // Phase 5.3: Process Control
        current_token->type == TOKEN_BUILTIN_EXECUTE_COMMAND ||
        current_token->type == TOKEN_BUILTIN_GET_COMMAND_OUTPUT ||
        current_token->type == TOKEN_BUILTIN_GET_PROCESS_ID ||
        current_token->type == TOKEN_BUILTIN_GET_PARENT_PROCESS_ID ||
        // Phase 5.3: Time & Date Utilities
        current_token->type == TOKEN_BUILTIN_FORMAT_TIMESTAMP ||
        current_token->type == TOKEN_BUILTIN_PARSE_TIMESTAMP ||
        current_token->type == TOKEN_BUILTIN_GET_MILLISECONDS ||
        current_token->type == TOKEN_BUILTIN_GET_TIME_STRING ||
        // Phase 5.3: Path Utilities
        current_token->type == TOKEN_BUILTIN_JOIN_PATH ||
        current_token->type == TOKEN_BUILTIN_GET_FILE_EXTENSION ||
        current_token->type == TOKEN_BUILTIN_GET_FILE_NAME ||
        current_token->type == TOKEN_BUILTIN_GET_DIRECTORY) {

        TokenType func_type = current_token->type;
        consume(current_token->type);

        consume(TOKEN_LEFT_PAREN);

        // Parse arguments
        ASTNode* arg1 = NULL;
        ASTNode* arg2 = NULL;
        ASTNode* arg3 = NULL;

        if (current_token->type != TOKEN_RIGHT_PAREN) {
            arg1 = ifade();

            if (current_token->type == TOKEN_COMMA) {
                consume(TOKEN_COMMA);
                arg2 = ifade();

                if (current_token->type == TOKEN_COMMA) {
                    consume(TOKEN_COMMA);
                    arg3 = ifade();
                }
            }
        }

        consume(TOKEN_RIGHT_PAREN);

        return createAST_BuiltinCall(func_type, arg1, arg2, arg3);
    }

    if (current_token->type == TOKEN_IDENTIFIER) {
        Token ad_token_kopya;
        ad_token_kopya.type = current_token->type;
        ad_token_kopya.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Fonksiyon çağrısı mı? (func(args))
        if (current_token->type == TOKEN_LEFT_PAREN) {
            consume(TOKEN_LEFT_PAREN);
            ASTNode** arguman_listesi = NULL;
            int a_sayisi = 0;
            if (current_token->type != TOKEN_RIGHT_PAREN) {
                arguman_listesi = (ASTNode**)malloc(sizeof(ASTNode*) * 10);
                do {
                    arguman_listesi[a_sayisi] = ifade();
                    a_sayisi++;
                } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
            }
            consume(TOKEN_RIGHT_PAREN);
            ASTNode* call_node = createAST_IslecCagirma(&ad_token_kopya, arguman_listesi, a_sayisi);
            free(ad_token_kopya.value);
            return call_node;
        }
        // Array erişimi mi? (arr[index])
        else if (current_token->type == TOKEN_LEFT_BRACKET) {
            consume(TOKEN_LEFT_BRACKET);
            ASTNode* indeks = ifade();
            consume(TOKEN_RIGHT_BRACKET);
            ASTNode* array_erisim = createAST_ArrayErisim(&ad_token_kopya, indeks);
            free(ad_token_kopya.value);
            return array_erisim;
        }
        // Struct field access or List method call? (p.x or list.add())
        else if (current_token->type == TOKEN_DOT) {
            consume(TOKEN_DOT);
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Field/method name", "IDENTIFIER");
            }
            Token field_or_method;
            field_or_method.type = current_token->type;
            field_or_method.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);

            // Check if this is a list method call (has parentheses)
            if (current_token->type == TOKEN_LEFT_PAREN) {
                consume(TOKEN_LEFT_PAREN);

                // Phase 6: List methods - add, get, set, remove, insert, length, clear
                if (strcmp(field_or_method.value, "add") == 0) {
                    ASTNode* deger = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* add_node = createAST_ListAdd(&ad_token_kopya, deger);
                    // free(ad_token_kopya.value); // AST owns this
                    free(field_or_method.value);
                    return add_node;
                }
                else if (strcmp(field_or_method.value, "get") == 0) {
                    ASTNode* indeks = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* get_node = createAST_ListGet(&ad_token_kopya, indeks);
                    // free(ad_token_kopya.value); // AST owns this
                    free(field_or_method.value);
                    return get_node;
                }
                else if (strcmp(field_or_method.value, "set") == 0) {
                    // list.set(index, value)
                    ASTNode* indeks = ifade();
                    consume(TOKEN_COMMA);
                    ASTNode* value = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    
                    ASTNode* set_node = createAST_ListSet(&ad_token_kopya, indeks, value);
                    // free(ad_token_kopya.value); // AST owns this
                    free(field_or_method.value);
                    return set_node;
                }
                else if (strcmp(field_or_method.value, "remove") == 0) {
                    // list.remove(index)
                    ASTNode* indeks = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    
                    ASTNode* remove_node = createAST_ListRemove(&ad_token_kopya, indeks);
                    // free(ad_token_kopya.value); // AST owns this
                    free(field_or_method.value);
                    return remove_node;
                }
                else if (strcmp(field_or_method.value, "insert") == 0) {
                    // list.insert(index, value)
                    ASTNode* indeks = ifade();
                    consume(TOKEN_COMMA);
                    ASTNode* value = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    
                    ASTNode* insert_node = createAST_ListInsert(&ad_token_kopya, indeks, value);
                    // free(ad_token_kopya.value); // AST owns this
                    free(field_or_method.value);
                    return insert_node;
                }
                else if (strcmp(field_or_method.value, "length") == 0 || 
                         strcmp(field_or_method.value, "size") == 0) {
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* size_node = createAST_ListSize(&ad_token_kopya);
                    // free(ad_token_kopya.value); // AST owns this
                    free(field_or_method.value);
                    return size_node;
                }
                else if (strcmp(field_or_method.value, "clear") == 0) {
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* clear_node = createAST_ListClear(&ad_token_kopya);
                    // free(ad_token_kopya.value); // AST owns this
                    free(field_or_method.value);
                    return clear_node;
                }
                // ===== Phase 4: Map Methods =====
                else if (strcmp(field_or_method.value, "has") == 0) {
                    // map.has(key)
                    ASTNode* key = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* has_node = createAST_MapHas(&ad_token_kopya, key);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return has_node;
                }
                else {
                    fprintf(stderr, "Unknown list/map method: %s\n", field_or_method.value);
                    parseError("List/Map method", "add/get/set/remove/insert/length/size/clear/has");
                }
            }
            else {
                // Regular struct field access (no parentheses)
                ASTNode* field_access = createAST_StructFieldAccess(&ad_token_kopya, &field_or_method);
                free(ad_token_kopya.value);
                free(field_or_method.value);
                return field_access;
            }
        }
        // Normal değişken
        else {
            ASTNode* var_node = createAST_Degisken(&ad_token_kopya);
            free(ad_token_kopya.value);
            return var_node;
        }
    }

    parseError("Birincil ifade bekleniyordu.", "Sayı, Parantez veya Tanımlayıcı");
    return NULL;
}

// Unary expressions (not, unary minus)
ASTNode* unary_ifade() {
    // Handle 'not' keyword (logical NOT)
    if (current_token->type == TOKEN_NOT) {
        consume(TOKEN_NOT);
        ASTNode* operand = unary_ifade();  // Recursive for multiple nots
        
        // Create a binary operation node: operand == 0
        // This makes 'not x' equivalent to 'x == 0'
        ASTNode* zero_node = (ASTNode*)malloc(sizeof(ASTNode));
        zero_node->type = AST_SAYI;
        zero_node->sabit_data.deger = strdup("0");
        
        ASTNode* comparison = (ASTNode*)malloc(sizeof(ASTNode));
        comparison->type = AST_IKILI_ISLEM;
        comparison->ikili_islem_data.sol = operand;
        comparison->ikili_islem_data.sag = zero_node;
        comparison->ikili_islem_data.operator_type = TOKEN_OP_ESIT_KARSILASTIRMA;
        
        return comparison;
    }
    
    // Phase 5.5: Handle bitwise NOT (~)
    if (current_token->type == TOKEN_BITWISE_NOT) {
        consume(TOKEN_BITWISE_NOT);
        ASTNode* operand = unary_ifade();  // Recursive
        
        // Create unary NOT operation
        // We'll encode it as -1 XOR operand (which is bitwise NOT)
        ASTNode* minus_one = (ASTNode*)malloc(sizeof(ASTNode));
        minus_one->type = AST_SAYI;
        minus_one->sabit_data.deger = strdup("-1");
        
        ASTNode* not_node = (ASTNode*)malloc(sizeof(ASTNode));
        not_node->type = AST_IKILI_ISLEM;
        not_node->ikili_islem_data.sol = operand;
        not_node->ikili_islem_data.sag = minus_one;
        not_node->ikili_islem_data.operator_type = TOKEN_BITWISE_XOR;
        
        return not_node;
    }
    
    // Otherwise, parse primary expression
    return birincil();
}

ASTNode* ikili_islem(int onceki_oncelik) {
    ASTNode* sol = unary_ifade();  // Changed from birincil() to unary_ifade()
    if (sol == NULL) return NULL;

    while (1) {
        TokenType op_type = current_token->type;
        int oncelik = get_precedence(op_type);
        if (oncelik <= onceki_oncelik) {
            break;
        }

        // Phase 5.8: Ternary operator (condition ? true_val : false_val)
        if (op_type == TOKEN_QUESTION) {
            consume(TOKEN_QUESTION);
            ASTNode* dogru_deger = ifade();  // True value
            consume(TOKEN_COLON);            // :
            ASTNode* yanlis_deger = ikili_islem(oncelik);  // False value

            ASTNode* ternary_node = (ASTNode*)malloc(sizeof(ASTNode));
            if (ternary_node == NULL) return NULL;
            ternary_node->type = AST_TERNARY;
            ternary_node->ternary_data.kosul = sol;
            ternary_node->ternary_data.dogru_deger = dogru_deger;
            ternary_node->ternary_data.yanlis_deger = yanlis_deger;
            sol = ternary_node;
            continue;
        }

        consume(op_type);
        ASTNode* sag = ikili_islem(oncelik);
        ASTNode* yeni_node = (ASTNode*)malloc(sizeof(ASTNode));
        if (yeni_node == NULL) return NULL;
        yeni_node->type = AST_IKILI_ISLEM;
        yeni_node->ikili_islem_data.sol = sol;
        yeni_node->ikili_islem_data.sag = sag;
        yeni_node->ikili_islem_data.operator_type = op_type;
        sol = yeni_node;
    }
    return sol;
}

ASTNode* komut() {
    // 0. YAPI - Struct Tanımlama (YAPI Nokta İSE ... SON)
    if (current_token->type == TOKEN_YAPI_STRUCT) {
        consume(TOKEN_YAPI_STRUCT);

        // Struct adı
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Struct adı", "IDENTIFIER");
        }
        Token struct_ad;
        struct_ad.type = current_token->type;
        struct_ad.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // İSE
        consume(TOKEN_YAPI_KOSUL_ISE);

        // Field'ları parse et
        Token** field_tipleri = (Token**)malloc(sizeof(Token*) * 20);  // Max 20 field
        Token** field_adlari = (Token**)malloc(sizeof(Token*) * 20);
        int field_sayisi = 0;

        // SON'a kadar field'ları oku
        while (current_token->type != TOKEN_YAPI_SON) {
            // Field tipi (SAYISAL, METIN, vb.)
            if (current_token->type != TOKEN_TANIMLA_SAYI &&
                current_token->type != TOKEN_TANIMLA_METIN &&
                current_token->type != TOKEN_TANIMLA_BOOL) {
                parseError("Field tipi", "SAYISAL/METIN/BOOL");
            }

            Token* field_tip = (Token*)malloc(sizeof(Token));
            field_tip->type = current_token->type;
            field_tip->value = strdup(current_token->value);
            field_tipleri[field_sayisi] = field_tip;
            consume(current_token->type);

            // Field adı
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Field adı", "IDENTIFIER");
            }
            Token* field_ad = (Token*)malloc(sizeof(Token));
            field_ad->type = current_token->type;
            field_ad->value = strdup(current_token->value);
            field_adlari[field_sayisi] = field_ad;
            consume(TOKEN_IDENTIFIER);

            // Noktalı virgül
            consume(TOKEN_SEMICOLON);

            field_sayisi++;
        }

        // SON
        consume(TOKEN_YAPI_SON);

        ASTNode* struct_node = createAST_StructTanimlama(&struct_ad, field_tipleri, field_adlari, field_sayisi);
        free(struct_ad.value);
        return struct_node;
    }

    // Phase 5.4: Enum Tanımlama (enum Color then RED = 0 GREEN = 1 end)
    if (current_token->type == TOKEN_YAPI_ENUM) {
        consume(TOKEN_YAPI_ENUM);

        // Enum adı
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Enum adı", "IDENTIFIER");
        }
        Token* enum_ad = (Token*)malloc(sizeof(Token));
        enum_ad->type = current_token->type;
        enum_ad->value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // then
        consume(TOKEN_YAPI_KOSUL_ISE);

        // Değerleri parse et
        Token** value_adlari = (Token**)malloc(sizeof(Token*) * 100);  // Max 100 enum value
        int* value_degerleri = (int*)malloc(sizeof(int) * 100);
        int value_sayisi = 0;
        int next_value = 0;

        // end'e kadar değerleri oku
        while (current_token->type != TOKEN_YAPI_SON) {
            // Değer adı
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Enum değer adı", "IDENTIFIER");
            }
            Token* value_ad = (Token*)malloc(sizeof(Token));
            value_ad->type = current_token->type;
            value_ad->value = strdup(current_token->value);
            value_adlari[value_sayisi] = value_ad;
            consume(TOKEN_IDENTIFIER);

            // Opsiyonel: = değer
            if (current_token->type == TOKEN_ASSIGN) {
                consume(TOKEN_ASSIGN);
                if (current_token->type != TOKEN_SAYI) {
                    parseError("Enum değeri", "SAYI");
                }
                next_value = atoi(current_token->value);
                consume(TOKEN_SAYI);
            }
            value_degerleri[value_sayisi] = next_value;
            next_value++;
            value_sayisi++;
        }

        // end
        consume(TOKEN_YAPI_SON);

        // AST node oluştur
        ASTNode* enum_node = (ASTNode*)malloc(sizeof(ASTNode));
        enum_node->type = AST_ENUM_TANIMLAMA;
        enum_node->enum_tanimlama_data.ad = enum_ad;
        enum_node->enum_tanimlama_data.value_adlari = value_adlari;
        enum_node->enum_tanimlama_data.value_degerleri = value_degerleri;
        enum_node->enum_tanimlama_data.value_sayisi = value_sayisi;
        return enum_node;
    }

    // Phase 5.4: Switch Statement (switch expr then case ... end)
    if (current_token->type == TOKEN_YAPI_SWITCH) {
        consume(TOKEN_YAPI_SWITCH);

        // Switch ifadesi
        ASTNode* switch_ifade = ifade();

        // then
        consume(TOKEN_YAPI_KOSUL_ISE);

        // Case'leri parse et
        ASTNode** cases = (ASTNode**)malloc(sizeof(ASTNode*) * 100);  // Max 100 case
        int case_sayisi = 0;
        ASTNode* default_blok = NULL;

        // end'e kadar case'leri oku
        while (current_token->type != TOKEN_YAPI_SON) {
            if (current_token->type == TOKEN_YAPI_CASE) {
                consume(TOKEN_YAPI_CASE);

                // Case değeri
                ASTNode* case_deger = ifade();

                // then
                consume(TOKEN_YAPI_KOSUL_ISE);

                // Case bloğu
                ASTNode** case_komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
                int case_komut_sayisi = 0;

                while (current_token->type != TOKEN_YAPI_SON &&
                       current_token->type != TOKEN_YAPI_CASE &&
                       current_token->type != TOKEN_YAPI_DEFAULT) {
                    case_komutlar[case_komut_sayisi++] = komut();
                }

                // end (case bloğu için)
                if (current_token->type == TOKEN_YAPI_SON) {
                    consume(TOKEN_YAPI_SON);
                }

                // Case node oluştur
                ASTNode* case_blok = (ASTNode*)malloc(sizeof(ASTNode));
                case_blok->type = AST_BLOK;
                case_blok->blok_data.komutlar = case_komutlar;
                case_blok->blok_data.sayisi = case_komut_sayisi;

                ASTNode* case_node = (ASTNode*)malloc(sizeof(ASTNode));
                case_node->type = AST_CASE_KOMUTU;
                case_node->case_data.deger = case_deger;
                case_node->case_data.blok = case_blok;
                cases[case_sayisi++] = case_node;

            } else if (current_token->type == TOKEN_YAPI_DEFAULT) {
                consume(TOKEN_YAPI_DEFAULT);

                // then
                consume(TOKEN_YAPI_KOSUL_ISE);

                // Default bloğu
                ASTNode** default_komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
                int default_komut_sayisi = 0;

                while (current_token->type != TOKEN_YAPI_SON) {
                    default_komutlar[default_komut_sayisi++] = komut();
                }

                // end
                consume(TOKEN_YAPI_SON);

                default_blok = (ASTNode*)malloc(sizeof(ASTNode));
                default_blok->type = AST_BLOK;
                default_blok->blok_data.komutlar = default_komutlar;
                default_blok->blok_data.sayisi = default_komut_sayisi;
            } else {
                parseError("case veya default", "CASE/DEFAULT");
            }
        }

        // end (switch için)
        consume(TOKEN_YAPI_SON);

        // Switch node oluştur
        ASTNode* switch_node = (ASTNode*)malloc(sizeof(ASTNode));
        switch_node->type = AST_SWITCH_KOMUTU;
        switch_node->switch_data.ifade = switch_ifade;
        switch_node->switch_data.cases = cases;
        switch_node->switch_data.case_sayisi = case_sayisi;
        switch_node->switch_data.default_blok = default_blok;
        return switch_node;
    }

    // Struct Değişken Tanımlama (Person p;)
    // Pattern: IDENTIFIER IDENTIFIER SEMICOLON
    if (current_token->type == TOKEN_IDENTIFIER) {
        Token* peeked = peekNextToken();
        if (peeked && peeked->type == TOKEN_IDENTIFIER) {
            // This is: StructName varName;
            // Save struct type name
            Token struct_tip;
            struct_tip.type = current_token->type;
            struct_tip.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);

            // Get variable name
            Token ad;
            ad.type = current_token->type;
            ad.value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);

            // Must be followed by semicolon
            consume(TOKEN_SEMICOLON);

            ASTNode* struct_var_node = createAST_StructDegisken(&struct_tip, &ad);
            free(struct_tip.value);
            free(ad.value);
            return struct_var_node;
        }
    }

    // ===== Phase 4: Map Tanımlama (map[KeyType:ValueType] var = map();) =====
    if (current_token->type == TOKEN_YAPI_MAP) {
        consume(TOKEN_YAPI_MAP);

        // Expect [
        consume(TOKEN_LEFT_BRACKET);

        // Key type
        Token key_tip;
        if (current_token->type == TOKEN_TANIMLA_SAYI ||
            current_token->type == TOKEN_TANIMLA_METIN ||
            current_token->type == TOKEN_IDENTIFIER) {
            key_tip.type = current_token->type;
            key_tip.value = strdup(current_token->value);
            consume(current_token->type);
        } else {
            parseError("Map key type", "int/string");
        }

        // Expect :
        consume(TOKEN_COLON);

        // Value type
        Token value_tip;
        if (current_token->type == TOKEN_TANIMLA_SAYI ||
            current_token->type == TOKEN_TANIMLA_METIN ||
            current_token->type == TOKEN_IDENTIFIER) {
            value_tip.type = current_token->type;
            value_tip.value = strdup(current_token->value);
            consume(current_token->type);
        } else {
            parseError("Map value type", "int/string");
        }

        // Expect ]
        consume(TOKEN_RIGHT_BRACKET);

        // Variable name
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Map variable name", "IDENTIFIER");
        }
        Token degisken_adi;
        degisken_adi.type = current_token->type;
        degisken_adi.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Expect =
        consume(TOKEN_ASSIGN);

        // Expect map()
        if (current_token->type != TOKEN_YAPI_MAP) {
            parseError("map()", "map");
        }
        consume(TOKEN_YAPI_MAP);
        consume(TOKEN_LEFT_PAREN);
        consume(TOKEN_RIGHT_PAREN);

        // Expect ;
        consume(TOKEN_SEMICOLON);

        ASTNode* map_node = createAST_MapTanimlama(&key_tip, &value_tip, &degisken_adi);
        free(key_tip.value);
        free(value_tip.value);
        free(degisken_adi.value);
        return map_node;
    }

    // 1. YAZDIR (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_YAZDIR) {
        consume(TOKEN_YAPI_YAZDIR);
        ASTNode* ifade_dugumu = ifade();
        specs_check_no_semicolon("YAZDIR");
        ASTNode* yazdir_node = (ASTNode*)malloc(sizeof(ASTNode));
        yazdir_node->type = AST_YAZDIR_KOMUTU;
        yazdir_node->tek_ifade_data.ifade = ifade_dugumu;
        return yazdir_node;
    }

    // 3. Const Değişken Tanımlama (Phase 5.6: const int x = 10;)
    if (current_token->type == TOKEN_YAPI_CONST) {
        consume(TOKEN_YAPI_CONST);
        
        if (current_token->type != TOKEN_TANIMLA_SAYI &&
            current_token->type != TOKEN_TANIMLA_METIN &&
            current_token->type != TOKEN_TANIMLA_BOOL) {
            parseError("Tip belirteci (int, string, bool)", current_token->value);
        }
        
        Token tip_token;
        tip_token.type = current_token->type;
        tip_token.value = strdup(current_token->value);
        consume(current_token->type);

        Token ad_token;
        if (current_token->type != TOKEN_IDENTIFIER) parseError("Değişken adı", "IDENTIFIER");
        ad_token.type = current_token->type;
        ad_token.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        consume(TOKEN_ASSIGN);
        ASTNode* ifade_dugumu = ifade();
        consume(TOKEN_SEMICOLON);

        ASTNode* tanimlama_node = createAST_DegiskenTanimlama(&tip_token, &ad_token, ifade_dugumu);
        tanimlama_node->tanimlama_data.is_const = true; // Mark as const
        free(tip_token.value);
        free(ad_token.value);
        return tanimlama_node;
    }

    // 4. Değişken Tanımlama (Noktalı virgüllü)
    if (current_token->type == TOKEN_TANIMLA_SAYI ||
        current_token->type == TOKEN_TANIMLA_METIN ||
        current_token->type == TOKEN_TANIMLA_BOOL ||
        current_token->type == TOKEN_YAPI_LIST)  // Phase 6: List tanımlama
    {
        // Phase 6: List tanımlama
        if (current_token->type == TOKEN_YAPI_LIST) {
            return list_tanimlama_parse();  // Call list_tanimlama_parse() which handles List<T> syntax
        }
        
        Token tip_token;
        tip_token.type = current_token->type;
        tip_token.value = strdup(current_token->value);
        consume(current_token->type);

        Token ad_token;
        if (current_token->type != TOKEN_IDENTIFIER) parseError("Değişken adı", "IDENTIFIER");
        ad_token.type = current_token->type;
        ad_token.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Array tanımlaması mı? (SAYISAL arr[10];)
        if (current_token->type == TOKEN_LEFT_BRACKET) {
            consume(TOKEN_LEFT_BRACKET);
            ASTNode* boyut = ifade();  // Array boyutu
            consume(TOKEN_RIGHT_BRACKET);
            consume(TOKEN_SEMICOLON);

            ASTNode* array_node = createAST_ArrayTanimlama(&tip_token, &ad_token, boyut);
            free(tip_token.value);
            free(ad_token.value);
            return array_node;
        }

        // Normal değişken tanımlaması (SAYISAL x = 5;)
        consume(TOKEN_ASSIGN);
        ASTNode* ifade_dugumu = ifade();

        // ✅ KURAL: Tanımlamalar noktalı virgül ALIR
        consume(TOKEN_SEMICOLON);

        ASTNode* tanimlama_node = createAST_DegiskenTanimlama(&tip_token, &ad_token, ifade_dugumu);
        free(tip_token.value);
        free(ad_token.value);
        return tanimlama_node;
    }

    // 3. EĞER (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_KOSUL_EGER) {
        return kosul_komutu(); // Kendi içinde 'SON'u yönetir
    }

    // 4. FOR Loop (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_FOR) {
        return for_komutu(); // Kendi içinde 'end for'u yönetir
    }

    // 5. DÖNGÜ/WHILE (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_DONGU) {
        return dongu_komutu(); // Kendi içinde 'end while'ı yönetir
    }
    
    // 6. break (Noktalı virgülsüz)
    if (current_token->type == TOKEN_BREAK) {
        consume(TOKEN_BREAK);
        specs_check_no_semicolon("break");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_BREAK;
        return node;
    }

    // 7. continue (Noktalı virgülsüz)
    if (current_token->type == TOKEN_CONTINUE) {
        consume(TOKEN_CONTINUE);
        specs_check_no_semicolon("continue");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_CONTINUE;
        return node;
    }

    // Phase 5.8: stop (debugging breakpoint)
    if (current_token->type == TOKEN_STOP) {
        consume(TOKEN_STOP);
        specs_check_no_semicolon("stop");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_STOP;
        return node;
    }

    // Phase 5.8: goto label_name
    if (current_token->type == TOKEN_GOTO) {
        consume(TOKEN_GOTO);
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("Label name expected after goto", "IDENTIFIER");
        }
        Token* label_adi = (Token*)malloc(sizeof(Token));
        label_adi->type = current_token->type;
        label_adi->value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);
        specs_check_no_semicolon("goto");
        
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_GOTO;
        node->goto_data.label_adi = label_adi;
        return node;
    }
    
    // 8. DÖNÜŞ (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_DONUS) {
        return donus_komutu(); // Kendi içinde yönetir
    }

    // 7. İŞLEÇ (Function Definition) (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_ISLEC) {
        return islec_tanimlama(); // Kendi içinde yönetir
    }

    // 7.5. Built-in Function Call as Statement (Phase 5.2)
    // Handle built-in calls like: set_error_code(42), mlp_free(ptr), assert(1, "msg")
    if (current_token->type == TOKEN_BUILTIN_EXIT_WITH_CODE ||
        current_token->type == TOKEN_BUILTIN_PANIC ||
        current_token->type == TOKEN_BUILTIN_ASSERT ||
        current_token->type == TOKEN_BUILTIN_SET_ERROR_CODE ||
        current_token->type == TOKEN_BUILTIN_MLP_FREE ||
        current_token->type == TOKEN_BUILTIN_CHECK_MEMORY_LEAKS ||
        current_token->type == TOKEN_BUILTIN_SLEEP_MS ||
        // Phase 5.3: Binary File I/O (write_binary can be used as statement)
        current_token->type == TOKEN_BUILTIN_WRITE_BINARY ||
        current_token->type == TOKEN_BUILTIN_COPY_FILE ||
        // Phase 5.3: Directory Operations (can be used as statements)
        current_token->type == TOKEN_BUILTIN_CREATE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_REMOVE_DIRECTORY ||
        current_token->type == TOKEN_BUILTIN_CHANGE_DIRECTORY ||
        // Phase 5.3: Process Control (execute_command can be statement)
        current_token->type == TOKEN_BUILTIN_EXECUTE_COMMAND) {
        // Parse built-in call as expression first
        ASTNode* builtin_node = ifade();
        // Return as statement (no semicolon required)
        specs_check_no_semicolon("Built-in fonksiyon çağrısı");
        return builtin_node;
    }

    // 8. Atama veya İfade Komutu (Noktalı virgülsüz)
    if (current_token->type == TOKEN_IDENTIFIER) {
        // Phase 5.8: Check for label (identifier followed by colon)
        Token* peek_token = peekNextToken();
        if (peek_token && peek_token->type == TOKEN_COLON) {
            Token* label_adi = (Token*)malloc(sizeof(Token));
            label_adi->type = current_token->type;
            label_adi->value = strdup(current_token->value);
            consume(TOKEN_IDENTIFIER);
            consume(TOKEN_COLON);
            specs_check_no_semicolon("label");
            
            ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
            node->type = AST_LABEL;
            node->label_data.label_adi = label_adi;
            return node;
        }

        ASTNode* sol_node = ifade(); // 'birincil()' çağrılır

        // Phase 5.7: Compound Assignment (+=, -=, *=, /=)
        if (sol_node->type == AST_DEGISKEN &&
            (current_token->type == TOKEN_PLUS_ASSIGN || current_token->type == TOKEN_MINUS_ASSIGN ||
             current_token->type == TOKEN_MUL_ASSIGN || current_token->type == TOKEN_DIV_ASSIGN)) {
            TokenType compound_op = current_token->type;
            consume(compound_op);
            ASTNode* sag_ifade = ifade();

            Token* ad_token = sol_node->degisken_data.ad;

            // Create: x = x op value
            ASTNode* var_ref = (ASTNode*)malloc(sizeof(ASTNode));
            var_ref->type = AST_DEGISKEN;
            Token* var_token = (Token*)malloc(sizeof(Token));
            var_token->type = TOKEN_IDENTIFIER;
            var_token->value = strdup(ad_token->value);
            var_token->line = ad_token->line;
            var_token->column = ad_token->column;
            var_ref->degisken_data.ad = var_token;

            TokenType binary_op_type;
            switch (compound_op) {
                case TOKEN_PLUS_ASSIGN: binary_op_type = TOKEN_PLUS; break;
                case TOKEN_MINUS_ASSIGN: binary_op_type = TOKEN_MINUS; break;
                case TOKEN_MUL_ASSIGN: binary_op_type = TOKEN_MUL; break;
                case TOKEN_DIV_ASSIGN: binary_op_type = TOKEN_DIV; break;
                default: binary_op_type = TOKEN_PLUS; break;
            }

            ASTNode* binary_op = (ASTNode*)malloc(sizeof(ASTNode));
            binary_op->type = AST_IKILI_ISLEM;
            binary_op->ikili_islem_data.sol = var_ref;
            binary_op->ikili_islem_data.sag = sag_ifade;
            binary_op->ikili_islem_data.operator_type = binary_op_type;

            ASTNode* atama_node = createAST_AtamaKomutu(ad_token, binary_op);
            specs_check_no_semicolon("Compound assignment");
            free(sol_node);
            return atama_node;
        }

        // DURUM 7.1: ATAMA (örn: x = 5, arr[i] = 5, p.x = 10)
        // 'ifade()' bize bir AST_DEGISKEN (x), AST_ARRAY_ERISIM veya AST_STRUCT_FIELD_ACCESS döndürdüyse
        if ((sol_node->type == AST_DEGISKEN || sol_node->type == AST_ARRAY_ERISIM || sol_node->type == AST_STRUCT_FIELD_ACCESS) &&
            current_token->type == TOKEN_ASSIGN) {
            consume(TOKEN_ASSIGN); // '=' tüket
            ASTNode* sag_ifade = ifade(); // Sağ tarafı (5) ayrıştır

            specs_check_no_semicolon("Atama komutu");

            // Normal değişken ataması
            if (sol_node->type == AST_DEGISKEN) {
                Token* ad_token = sol_node->degisken_data.ad; // Adı al
                ASTNode* atama_node = createAST_AtamaKomutu(ad_token, sag_ifade);

                // sol_node'un belleğini düzgün yönet
                free(ad_token->value);
                free(ad_token);
                free(sol_node);

                return atama_node;
            }
            // Array erişimi ataması (arr[i] = value)
            else if (sol_node->type == AST_ARRAY_ERISIM) {
                specs_check_no_semicolon("Array atama komutu");

                // sol_node AST_ARRAY_ERISIM, ondan ad ve indeks al
                Token* array_ad = sol_node->array_erisim_data.ad;
                ASTNode* array_indeks = sol_node->array_erisim_data.indeks;

                // Yeni AST_ARRAY_ATAMA node oluştur
                ASTNode* array_atama = createAST_ArrayAtama(array_ad, array_indeks, sag_ifade);

                // sol_node'u temizle (ama içindekiler array_atama'da kullanılıyor, dikkat!)
                free(sol_node); // Sadece wrapper'ı free et

                return array_atama;
            }
            // Struct field ataması (p.x = 10)
            else if (sol_node->type == AST_STRUCT_FIELD_ACCESS) {
                specs_check_no_semicolon("Struct field atama komutu");

                // sol_node AST_STRUCT_FIELD_ACCESS, ondan struct_ad ve field_ad al
                Token* struct_ad = sol_node->struct_field_access_data.struct_ad;
                Token* field_ad = sol_node->struct_field_access_data.field_ad;

                // Yeni AST_STRUCT_FIELD_ATAMA node oluştur
                ASTNode* field_atama = createAST_StructFieldAtama(struct_ad, field_ad, sag_ifade);

                // sol_node'u temizle
                free(sol_node);

                return field_atama;
            }
        }
        
        // DURUM 7.2: İFADE KOMUTU (örn: test(), set_error_code(42), mlp_free(ptr))
        // 'ifade()' bize bir AST_ISLEC_CAGIRMA (test()) veya AST_BUILTIN_CALL döndürdüyse
        if (sol_node->type == AST_ISLEC_CAGIRMA || sol_node->type == AST_BUILTIN_CALL) {
            specs_check_no_semicolon("İfade komutu (fonksiyon çağrısı)");
            return sol_node; // Fonksiyon çağrısını komut olarak döndür
        }

        // DURUM 7.3: LIST METHOD CALLS (list.add(), list.clear(), list.set(), etc.)
        // Phase 6: List method calls as statements
        if (sol_node->type == AST_LIST_ADD || 
            sol_node->type == AST_LIST_CLEAR ||
            sol_node->type == AST_LIST_SET ||
            sol_node->type == AST_LIST_REMOVE ||
            sol_node->type == AST_LIST_INSERT) {
            specs_check_no_semicolon("List method call");
            return sol_node; // List method call as statement
        }

        // Hata: 'x' (tek başına) veya 'x + 5' (ifade) bir komut değildir.
        parseError("Geçersiz komut. Atama (=) veya noktalı virgülsüz fonksiyon çağrısı bekleniyordu.", "Geçersiz Komut");
    }

    // Kopyala-yapıştır hatası (ve diğer hatalar) kaldırıldı.
    // Hiçbiri eşleşmezse bu bir hatadır.
    parseError("Komut bekleniyordu.", "YAZDIR, Tip, Tanımlayıcı, EĞER, DÖNGÜ veya İŞLEÇ");
    return NULL;
}

ASTNode* kosul_komutu() {
    consume(TOKEN_YAPI_KOSUL_EGER);
    ASTNode* kosul_ifadesi = ifade(); 
    consume(TOKEN_YAPI_KOSUL_ISE);
    ASTNode* ise_blogu = blok();
    ASTNode* degilse_blogu = NULL;
    if (current_token->type == TOKEN_YAPI_KOSUL_DEGILSE) {
        consume(TOKEN_YAPI_KOSUL_DEGILSE);
        degilse_blogu = blok();
    }
    consume(TOKEN_YAPI_SON);
    specs_check_no_semicolon("EĞER SON");
    return createAST_KosulKomutu(kosul_ifadesi, ise_blogu, degilse_blogu);
}

ASTNode* dongu_komutu() {
    consume(TOKEN_YAPI_DONGU);
    
    // Check if there's a condition or it's an infinite loop
    ASTNode* kosul_ifadesi = NULL;
    
    // If next token is NOT 'do'/'then' or block starter, parse condition
    // Infinite loop: while do ... end or while \n body... end
    // Conditioned: while condition do ... end
    if (current_token->type != TOKEN_YAPI_DO &&
        current_token->type != TOKEN_YAPI_KOSUL_ISE &&
        current_token->type != TOKEN_YAPI_SON) {
        // Parse condition
        kosul_ifadesi = ifade();
    }
    
    // Optional 'do' or 'then' keyword after condition
    if (current_token->type == TOKEN_YAPI_DO || current_token->type == TOKEN_YAPI_KOSUL_ISE) {
        consume(current_token->type);
    }
    
    ASTNode* govde_blogu = blok(); 
    consume(TOKEN_YAPI_SON);
    
    // Optional: check for "end while" or just "end"
    // For now, we accept just "end"
    
    specs_check_no_semicolon("DÖNGÜ SON");
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_DONGU_KOMUTU;
    node->dongu_data.kosul = kosul_ifadesi;  // NULL = infinite loop
    node->dongu_data.govde = govde_blogu;
    return node;
}

ASTNode* for_komutu() {
    consume(TOKEN_YAPI_FOR);
    
    // for i = 0 to 10 [step 2]
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("Loop variable name", "IDENTIFIER");
    }
    Token degisken_token;
    degisken_token.type = current_token->type;
    degisken_token.value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER);
    
    consume(TOKEN_ASSIGN);
    ASTNode* baslangic_ifade = ifade();
    
    consume(TOKEN_YAPI_TO);
    ASTNode* bitis_ifade = ifade();
    
    // Optional: step
    ASTNode* adim_ifade = NULL;
    if (current_token->type == TOKEN_YAPI_STEP) {
        consume(TOKEN_YAPI_STEP);
        adim_ifade = ifade();
    }
    
    // Optional 'do' or 'then' before body
    if (current_token->type == TOKEN_YAPI_DO || current_token->type == TOKEN_YAPI_KOSUL_ISE) {
        consume(current_token->type);
    }
    
    // Parse body
    ASTNode* govde_blogu = blok();
    consume(TOKEN_YAPI_SON);
    
    specs_check_no_semicolon("FOR SON");
    
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_FOR_KOMUTU;
    node->for_data.degisken = (Token*)malloc(sizeof(Token));
    node->for_data.degisken->type = degisken_token.type;
    node->for_data.degisken->value = strdup(degisken_token.value);
    node->for_data.baslangic = baslangic_ifade;
    node->for_data.bitis = bitis_ifade;
    node->for_data.adim = adim_ifade;
    node->for_data.govde = govde_blogu;
    
    free(degisken_token.value);
    return node;
}

ASTNode* islec_tanimlama() {
    consume(TOKEN_YAPI_ISLEC); 
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("İşleç adı bekleniyordu.", "IDENTIFIER");
    }
    Token ad_token;
    ad_token.type = current_token->type;
    ad_token.value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER); 

    consume(TOKEN_LEFT_PAREN);
    Token** parametre_listesi = NULL;
    int p_sayisi = 0;
    if (current_token->type != TOKEN_RIGHT_PAREN) {
        parametre_listesi = (Token**)malloc(sizeof(Token*) * 10);
        do {
            if (current_token->type != TOKEN_IDENTIFIER) {
                parseError("Parametre adı bekleniyordu.", "IDENTIFIER");
            }
            Token* param_token = (Token*)malloc(sizeof(Token));
            param_token->type = current_token->type;
            param_token->value = strdup(current_token->value);
            parametre_listesi[p_sayisi] = param_token;
            p_sayisi++;
            consume(TOKEN_IDENTIFIER);
        } while (current_token->type == TOKEN_COMMA && (consume(TOKEN_COMMA), 1));
    }
    consume(TOKEN_RIGHT_PAREN);

    consume(TOKEN_YAPI_KOSUL_ISE);
    ASTNode* govde_blogu = blok(); 
    consume(TOKEN_YAPI_SON);
    
    // Kılavuza göre SON İŞLEÇ değil, sadece SON
    specs_check_no_semicolon("İŞLEÇ SON");
    
    ASTNode* node = createAST_IslecTanimlama(&ad_token, parametre_listesi, p_sayisi, govde_blogu);
    free(ad_token.value);
    return node;
}

ASTNode* donus_komutu() {
    consume(TOKEN_YAPI_DONUS);
    ASTNode* ifade_dugumu = ifade();
    specs_check_no_semicolon("DÖNÜŞ komutu");
    ASTNode* donus_node = (ASTNode*)malloc(sizeof(ASTNode));
    donus_node->type = AST_DONUS_KOMUTU;
    donus_node->tek_ifade_data.ifade = ifade_dugumu;
    return donus_node;
}

ASTNode* blok() {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (node == NULL) exit(EXIT_FAILURE);
    node->type = AST_BLOK;
    node->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
    node->blok_data.sayisi = 0;

    while (current_token != NULL && // NULL kontrolü eklendi
           current_token->type != TOKEN_EOF &&
           current_token->type != TOKEN_YAPI_SON &&
           current_token->type != TOKEN_YAPI_KOSUL_DEGILSE &&
           current_token->type != TOKEN_YAPI_CASE &&
           current_token->type != TOKEN_YAPI_DEFAULT) 
    {
        // Boş noktalı virgülleri atla (Sadece tanımlamalar için)
        if (current_token->type == TOKEN_SEMICOLON) {
            consume(TOKEN_SEMICOLON);
            continue;
        }

        ASTNode* k = komut();
        if (k != NULL) {
            node->blok_data.komutlar[node->blok_data.sayisi++] = k;
        }
    }
    return node;
}

// Phase 6: List<T> Tanımlama Parser
// Modern syntax: List<int> numbers = List<int>()
ASTNode* list_tanimlama_parse() {
    consume(TOKEN_YAPI_LIST);

    // Expect < for generic type
    if (current_token->type != TOKEN_LT) {
        parseError("List<Type>", "<");
    }
    consume(TOKEN_LT);

    // Element type (int, string, or custom type name)
    Token* element_tip = (Token*)malloc(sizeof(Token));
    if (current_token->type == TOKEN_TANIMLA_SAYI ||
        current_token->type == TOKEN_TANIMLA_METIN ||
        current_token->type == TOKEN_TANIMLA_BOOL ||
        current_token->type == TOKEN_IDENTIFIER) {
        element_tip->type = current_token->type;
        element_tip->value = strdup(current_token->value);
        consume(current_token->type);
    } else {
        parseError("List element type", "int/string/bool/StructName");
    }

    // Expect > to close generic type
    if (current_token->type != TOKEN_GT) {
        parseError("List<Type>", ">");
    }
    consume(TOKEN_GT);

    // Variable name
    if (current_token->type != TOKEN_IDENTIFIER) {
        parseError("List variable name", "IDENTIFIER");
    }
    Token* degisken_adi = (Token*)malloc(sizeof(Token));
    degisken_adi->type = current_token->type;
    degisken_adi->value = strdup(current_token->value);
    consume(TOKEN_IDENTIFIER);

    // Expect =
    consume(TOKEN_ASSIGN);

    // Expect List<Type>()
    if (current_token->type != TOKEN_YAPI_LIST) {
        parseError("List<Type>()", "List");
    }
    consume(TOKEN_YAPI_LIST);
    
    // Expect < again for constructor
    consume(TOKEN_LT);
    
    // Skip element type (already have it)
    if (current_token->type == TOKEN_TANIMLA_SAYI ||
        current_token->type == TOKEN_TANIMLA_METIN ||
        current_token->type == TOKEN_TANIMLA_BOOL ||
        current_token->type == TOKEN_IDENTIFIER) {
        consume(current_token->type);
    }
    
    // Expect >
    consume(TOKEN_GT);
    
    // Expect ()
    consume(TOKEN_LEFT_PAREN);
    consume(TOKEN_RIGHT_PAREN);

    // Expect ;
    consume(TOKEN_SEMICOLON);

    ASTNode* list_node = createAST_ListTanimlama(element_tip, degisken_adi);
    // Tokens are now owned by AST
    return list_node;
}

// --- Ana Ayrıştırma Fonksiyonu ---

ASTNode* parse(const char* source_code) {
    initLexer(source_code);
    current_token = getNextToken();

    ASTNode* root = (ASTNode*)malloc(sizeof(ASTNode));
    root->type = AST_BLOK;
    root->blok_data.komutlar = (ASTNode**)malloc(sizeof(ASTNode*) * 100);
    root->blok_data.sayisi = 0;

    while (current_token != NULL && current_token->type != TOKEN_EOF) {
        // 'blok' fonksiyonu EOF görene kadar ana kod akışını okur
        root->blok_data.komutlar[root->blok_data.sayisi++] = komut();
    }
    return root;
}