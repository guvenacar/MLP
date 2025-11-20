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
        case TOKEN_YAPI_FOR: return "FOR";
        case TOKEN_YAPI_TO: return "TO";
        case TOKEN_YAPI_STEP: return "STEP";
        case TOKEN_YAPI_DONGU_BITIR: return "DÖNGÜ_BITIR";
        case TOKEN_YAPI_DONGU_DEVAM: return "continue";
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
    node->list_tanimlama_data.element_tipi = (Token*)malloc(sizeof(Token));
    node->list_tanimlama_data.element_tipi->type = element_tipi->type;
    node->list_tanimlama_data.element_tipi->value = strdup(element_tipi->value);
    node->list_tanimlama_data.degisken_adi = (Token*)malloc(sizeof(Token));
    node->list_tanimlama_data.degisken_adi->type = degisken_adi->type;
    node->list_tanimlama_data.degisken_adi->value = strdup(degisken_adi->value);
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


int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return 10;
        case TOKEN_MUL:
        case TOKEN_DIV:
            return 20;
        case TOKEN_OP_ESIT_KARSILASTIRMA:
        case TOKEN_GT:
        case TOKEN_LT:
        case TOKEN_GTE:
        case TOKEN_LTE:
        case TOKEN_NOT_ESIT:
            return 5;
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
        current_token->type == TOKEN_BUILTIN_MATH_POW) {

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

                // List method: add, get, size, clear
                if (strcmp(field_or_method.value, "add") == 0) {
                    ASTNode* deger = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* add_node = createAST_ListAdd(&ad_token_kopya, deger);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return add_node;
                }
                else if (strcmp(field_or_method.value, "get") == 0) {
                    ASTNode* indeks = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* get_node = createAST_ListGet(&ad_token_kopya, indeks);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return get_node;
                }
                else if (strcmp(field_or_method.value, "size") == 0) {
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* size_node = createAST_ListSize(&ad_token_kopya);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return size_node;
                }
                else if (strcmp(field_or_method.value, "clear") == 0) {
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* clear_node = createAST_ListClear(&ad_token_kopya);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return clear_node;
                }
                // ===== Phase 4: Map Methods =====
                else if (strcmp(field_or_method.value, "set") == 0) {
                    // map.set(key, value)
                    ASTNode* key = ifade();
                    consume(TOKEN_COMMA);
                    ASTNode* value = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* set_node = createAST_MapSet(&ad_token_kopya, key, value);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return set_node;
                }
                else if (strcmp(field_or_method.value, "has") == 0) {
                    // map.has(key)
                    ASTNode* key = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* has_node = createAST_MapHas(&ad_token_kopya, key);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return has_node;
                }
                else if (strcmp(field_or_method.value, "remove") == 0) {
                    // map.remove(key)
                    ASTNode* key = ifade();
                    consume(TOKEN_RIGHT_PAREN);
                    ASTNode* remove_node = createAST_MapRemove(&ad_token_kopya, key);
                    free(ad_token_kopya.value);
                    free(field_or_method.value);
                    return remove_node;
                }
                else {
                    fprintf(stderr, "Unknown list/map method: %s\n", field_or_method.value);
                    parseError("List/Map method", "add/get/size/clear/set/has/remove");
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

ASTNode* ikili_islem(int onceki_oncelik) {
    ASTNode* sol = birincil();
    if (sol == NULL) return NULL;

    while (1) {
        TokenType op_type = current_token->type;
        int oncelik = get_precedence(op_type);
        if (oncelik <= onceki_oncelik) {
            break;
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

    // ===== Phase 2: List Tanımlama (list[Type] var = list();) =====
    if (current_token->type == TOKEN_YAPI_LIST) {
        consume(TOKEN_YAPI_LIST);

        // Expect [
        consume(TOKEN_LEFT_BRACKET);

        // Element type (int, string, or custom type name)
        Token element_tip;
        if (current_token->type == TOKEN_TANIMLA_SAYI ||
            current_token->type == TOKEN_TANIMLA_METIN ||
            current_token->type == TOKEN_TANIMLA_BOOL ||
            current_token->type == TOKEN_IDENTIFIER) {
            element_tip.type = current_token->type;
            element_tip.value = strdup(current_token->value);
            consume(current_token->type);
        } else {
            parseError("List element type", "int/string/bool/StructName");
        }

        // Expect ]
        consume(TOKEN_RIGHT_BRACKET);

        // Variable name
        if (current_token->type != TOKEN_IDENTIFIER) {
            parseError("List variable name", "IDENTIFIER");
        }
        Token degisken_adi;
        degisken_adi.type = current_token->type;
        degisken_adi.value = strdup(current_token->value);
        consume(TOKEN_IDENTIFIER);

        // Expect =
        consume(TOKEN_ASSIGN);

        // Expect list()
        if (current_token->type != TOKEN_YAPI_LIST) {
            parseError("list()", "list");
        }
        consume(TOKEN_YAPI_LIST);
        consume(TOKEN_LEFT_PAREN);
        consume(TOKEN_RIGHT_PAREN);

        // Expect ;
        consume(TOKEN_SEMICOLON);

        ASTNode* list_node = createAST_ListTanimlama(&element_tip, &degisken_adi);
        free(element_tip.value);
        free(degisken_adi.value);
        return list_node;
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

    // 3. Değişken Tanımlama (Noktalı virgüllü)
    if (current_token->type == TOKEN_TANIMLA_SAYI ||
        current_token->type == TOKEN_TANIMLA_METIN ||
        current_token->type == TOKEN_TANIMLA_BOOL)
    {
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
    
    // 6. DÖNGÜ_BITIR/break (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_DONGU_BITIR) {
        consume(TOKEN_YAPI_DONGU_BITIR);
        specs_check_no_semicolon("DÖNGÜ_BITIR");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_DONGU_BITIR_KOMUTU;
        return node;
    }

    // 7. continue (Noktalı virgülsüz)
    if (current_token->type == TOKEN_YAPI_DONGU_DEVAM) {
        consume(TOKEN_YAPI_DONGU_DEVAM);
        specs_check_no_semicolon("continue");
        ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
        node->type = AST_DONGU_DEVAM_KOMUTU;
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

    // 8. Atama veya İfade Komutu (Noktalı virgülsüz)
    if (current_token->type == TOKEN_IDENTIFIER) {
        ASTNode* sol_node = ifade(); // 'birincil()' çağrılır

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
        
        // DURUM 7.2: İFADE KOMUTU (örn: test())
        // 'ifade()' bize bir AST_ISLEC_CAGIRMA (test()) döndürdüyse
        if (sol_node->type == AST_ISLEC_CAGIRMA) {
            specs_check_no_semicolon("İfade komutu (fonksiyon çağrısı)");
            return sol_node; // 'test()' çağrısını komut olarak döndür
        }

        // DURUM 7.3: LIST METHOD CALLS (list.add(), list.clear())
        // Phase 2: List method calls as statements
        if (sol_node->type == AST_LIST_ADD || sol_node->type == AST_LIST_CLEAR) {
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
    
    // If next token is not a block start, it's a conditioned while
    // while condition \n body... end while
    if (current_token->type != TOKEN_YAPI_SON && 
        current_token->type != TOKEN_TANIMLA_SAYI &&
        current_token->type != TOKEN_TANIMLA_METIN &&
        current_token->type != TOKEN_IDENTIFIER &&
        current_token->type != TOKEN_YAPI_YAZDIR &&
        current_token->type != TOKEN_YAPI_KOSUL_EGER &&
        current_token->type != TOKEN_YAPI_DONGU &&
        current_token->type != TOKEN_YAPI_FOR) {
        // Parse condition
        kosul_ifadesi = ifade();
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
           current_token->type != TOKEN_YAPI_KOSUL_DEGILSE) 
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