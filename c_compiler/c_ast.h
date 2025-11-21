// c_compiler/c_ast.h

#ifndef C_AST_H
#define C_AST_H

#include "c_lexer.h" // Token yapısını kullanmak için

// --- 1. Düğüm Tiplerini Tanımla (Enum) ---
typedef enum {
    AST_BLOK,
    AST_SAYI,
    AST_METIN,
    AST_DEGISKEN,
    AST_IKILI_ISLEM,
    AST_DEGISKEN_TANIMLAMA,
    AST_ATAMA_KOMUTU,
    AST_YAZDIR_KOMUTU,
    AST_KOSUL_KOMUTU, // <-- YENİ (EĞER)
    AST_DONGU_KOMUTU, // <-- YENİ (DÖNGÜ - while)
    AST_FOR_KOMUTU, // <-- YENİ (FOR - for loop)
    AST_DONGU_BITIR_KOMUTU, // <-- YENİ (DÖNGÜ_BITIR - break)
    AST_DONGU_DEVAM_KOMUTU, // <-- YENİ (continue)
    AST_ISLEC_TANIMLAMA,
    AST_DONUS_KOMUTU,
    AST_ISLEC_CAGIRMA,
    AST_ARRAY_TANIMLAMA,    // Array bildirimi: SAYISAL arr[10];
    AST_ARRAY_ERISIM,       // Array erişim: arr[5]
    AST_ARRAY_ATAMA,        // Array atama: arr[5] = 10;
    AST_STRUCT_TANIMLAMA,   // Struct tanımlama: YAPI Nokta İSE ... SON
    AST_STRUCT_FIELD_ACCESS,// Struct field erişim: p.x
    AST_STRUCT_FIELD_ATAMA, // Struct field atama: p.x = 10;
    AST_STRUCT_DEGISKEN,    // Struct değişken: Nokta p;
    // Phase 2: Dynamic Lists
    AST_LIST_TANIMLAMA,     // List tanımlama: list[int] numbers = list();
    AST_LIST_ADD,           // List add: numbers.add(10)
    AST_LIST_GET,           // List get: numbers.get(0)
    AST_LIST_SIZE,          // List size: numbers.size()
    AST_LIST_CLEAR,         // List clear: numbers.clear()

    // Phase 3: Built-in Functions
    AST_BUILTIN_CALL,       // Built-in function call: read_file("path")

    // Phase 4: Hash Maps
    AST_MAP_TANIMLAMA,      // Map declaration: map[string:int] ages = map();
    AST_MAP_SET,            // Map set: ages.set("Alice", 25)
    AST_MAP_GET,            // Map get: ages.get("Alice")
    AST_MAP_HAS,            // Map has: ages.has("Bob")
    AST_MAP_REMOVE,         // Map remove: ages.remove("Charlie")
    AST_MAP_SIZE,           // Map size: ages.size()
    AST_MAP_CLEAR,          // Map clear: ages.clear()

    // Phase 5.4: Language Features
    AST_ENUM_TANIMLAMA,     // Enum definition: enum Color then RED = 0 ... end
    AST_ENUM_VALUE,         // Enum value reference: Color.RED or just RED
    AST_SWITCH_KOMUTU,      // Switch statement
    AST_CASE_KOMUTU,        // Case clause
} ASTNodeType;

// İleri Bildirimler (C'de iç içe struct'lar için gerekli)
typedef struct ASTNode ASTNode;
typedef struct Blok ASTBlok;

// --- 2. Temel AST Düğümü Yapısı ---
// Tüm düğümlerin ortak özelliklerini tutar
struct ASTNode {
    ASTNodeType type;
    
    // YENİ: Düğümlere Özgü Veri
    // Bu, C'deki "Union" yapısının kullanımını gerektirir (ileri aşamada)
    // Şimdilik sadece ana struct'ları tanımlayalım:
    union {
        // Blok yapısı (Blok, Döngü, İşleç Gövdesi)
        struct {
            ASTNode** komutlar; // Komut dizisi (Heap'ten ayrılacak)
            int sayisi;         // Komut sayısı
        } blok_data;
        
        // Sayi/Metin yapısı
        struct {
            char* deger; // Token'dan gelen değer
        } sabit_data;

        // İkili İşlem yapısı (a + b)
        struct {
            ASTNode* sol;
            ASTNode* sag;
            TokenType operator_type;
        } ikili_islem_data;
        
        // YENİ: Değişken/Tanımlayıcı
        struct {
            Token* ad;
        } degisken_data;

        // Değişken Tanımlama (SAYISAL a = 5;)
        struct {
            Token* tip;
            Token* ad;
            ASTNode* ifade; // Atanan değer
        } tanimlama_data;
        
        // YENİ: Atama Komutu (sayac = 10;)
        struct {
            Token* ad;
            ASTNode* ifade; // Atanan değer
        } atama_data;
        
        // YENİ: Koşul Komutu (EĞER ifade İSE blok [DEĞİLSE blok])
        struct {
            ASTNode* kosul;       // (sayac == 5)
            ASTNode* ise_blok;    // EĞER doğruysa çalışacak blok
            ASTNode* degilse_blok; // (Opsiyonel) EĞER yanlışsa çalışacak blok
        } kosul_data;
        
        // YENİ: Döngü Komutu (DÖNGÜ blok SON - while loop)
        struct {
            ASTNode* kosul;     // While condition (NULL = infinite loop)
            ASTNode* govde;     // Döngü gövdesi
        } dongu_data;

        // YENİ: For Döngüsü (for i = 0 to 10 [step 2])
        struct {
            Token* degisken;    // Loop variable (i)
            ASTNode* baslangic; // Start value (0)
            ASTNode* bitis;     // End value (10)
            ASTNode* adim;      // Step value (optional, default 1)
            ASTNode* govde;     // Loop body
        } for_data;

        // İşleç Tanımlama (Fonksiyon)
        struct {
            Token* ad;
            Token** parametreler; // YENİ: Parametre adları (Token listesi)
            int parametre_sayisi; // YENİ
            ASTNode* govde;
        } islec_tanimlama_data;
        
        // İşleç Çağırma (topla(5, 7))
        struct {
            Token* hedef_ad;
            ASTNode** argumanlar;
            int arguman_sayisi;
        } islec_cagirma_data;
        
        // Diğer komutlar (YAZDIR, DÖNÜŞ, ATAMA)
        struct {
            ASTNode* ifade;
        } tek_ifade_data;

        // Array Tanımlama (SAYISAL arr[10];)
        struct {
            Token* tip;        // SAYISAL, METIN vb.
            Token* ad;         // Array ismi
            ASTNode* boyut;    // Array boyutu (sabit ifade)
        } array_tanimlama_data;

        // Array Erişim (arr[5], arr[i+1])
        struct {
            Token* ad;         // Array ismi
            ASTNode* indeks;   // İndeks ifadesi
        } array_erisim_data;

        // Array Atama (arr[5] = 10;)
        struct {
            Token* ad;         // Array ismi
            ASTNode* indeks;   // İndeks ifadesi
            ASTNode* deger;    // Atanacak değer
        } array_atama_data;

        // Struct Tanımlama (YAPI Nokta İSE SAYISAL x; SAYISAL y; SON)
        struct {
            Token* ad;              // Struct ismi (Nokta)
            Token** field_tipleri;  // Field tipleri (SAYISAL, METIN, vb.)
            Token** field_adlari;   // Field isimleri (x, y, vb.)
            int field_sayisi;       // Field sayısı
        } struct_tanimlama_data;

        // Struct Field Access (p.x)
        struct {
            Token* struct_ad;  // Struct değişken ismi (p)
            Token* field_ad;   // Field ismi (x)
        } struct_field_access_data;

        // Struct Field Atama (p.x = 10;)
        struct {
            Token* struct_ad;  // Struct değişken ismi (p)
            Token* field_ad;   // Field ismi (x)
            ASTNode* deger;    // Atanacak değer
        } struct_field_atama_data;

        // Struct Değişken (Nokta p;)
        struct {
            Token* struct_tip; // Struct tipi (Nokta)
            Token* ad;         // Değişken ismi (p)
        } struct_degisken_data;

        // ===== Phase 2: Dynamic Lists =====

        // List Tanımlama (list[int] numbers = list();)
        struct {
            Token* element_tipi;  // Element tipi (int, string, struct name)
            Token* degisken_adi;  // List değişken ismi (numbers)
        } list_tanimlama_data;

        // List Add (numbers.add(10))
        struct {
            Token* list_adi;      // List ismi (numbers)
            ASTNode* deger;       // Eklenecek değer
        } list_add_data;

        // List Get (numbers.get(0))
        struct {
            Token* list_adi;      // List ismi (numbers)
            ASTNode* indeks;      // İndeks ifadesi
        } list_get_data;

        // List Size (numbers.size())
        struct {
            Token* list_adi;      // List ismi (numbers)
        } list_size_data;

        // List Clear (numbers.clear())
        struct {
            Token* list_adi;      // List ismi (numbers)
        } list_clear_data;

        // Phase 3: Built-in Function Call
        struct {
            TokenType function_type;  // Which built-in function
            ASTNode* arg1;           // First argument
            ASTNode* arg2;           // Second argument (optional)
            ASTNode* arg3;           // Third argument (optional, for join)
        } builtin_call_data;

        // ===== Phase 4: Hash Maps =====

        // Map Tanımlama (map[string:int] ages = map();)
        struct {
            Token* key_tipi;      // Key type (string, int)
            Token* value_tipi;    // Value type (string, int)
            Token* degisken_adi;  // Map variable name (ages)
        } map_tanimlama_data;

        // Map Set (ages.set("Alice", 25))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
            ASTNode* value;       // Value expression
        } map_set_data;

        // Map Get (ages.get("Alice"))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
        } map_get_data;

        // Map Has (ages.has("Bob"))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
        } map_has_data;

        // Map Remove (ages.remove("Charlie"))
        struct {
            Token* map_adi;       // Map name (ages)
            ASTNode* key;         // Key expression
        } map_remove_data;

        // Map Size (ages.size())
        struct {
            Token* map_adi;       // Map name (ages)
        } map_size_data;

        // Map Clear (ages.clear())
        struct {
            Token* map_adi;       // Map name (ages)
        } map_clear_data;

        // ===== Phase 5.4: Language Features =====

        // Enum Tanımlama (enum Color then RED = 0 GREEN = 1 end)
        struct {
            Token* ad;              // Enum ismi (Color)
            Token** value_adlari;   // Değer isimleri (RED, GREEN, BLUE)
            int* value_degerleri;   // Değerler (0, 1, 2) - NULL ise otomatik
            int value_sayisi;       // Değer sayısı
        } enum_tanimlama_data;

        // Enum Value (Color.RED veya sadece RED)
        struct {
            Token* enum_ad;         // Enum ismi (opsiyonel, NULL olabilir)
            Token* value_ad;        // Değer ismi (RED)
        } enum_value_data;

        // Switch Statement
        struct {
            ASTNode* ifade;         // Switch edilecek ifade
            ASTNode** cases;        // Case listesi
            int case_sayisi;        // Case sayısı
            ASTNode* default_blok;  // Default bloğu (opsiyonel)
        } switch_data;

        // Case Clause
        struct {
            ASTNode* deger;         // Case değeri (sabit)
            ASTNode* blok;          // Case bloğu
        } case_data;
    };
};

// Fonksiyon Prototipleri
ASTNode* parse(const char* source_code); // Ana ayrıştırma fonksiyonu
void freeAST(ASTNode* node);             // AST temizleme (Bellek yönetimi)

// Phase 3: Built-in function helper
ASTNode* createAST_BuiltinCall(TokenType func_type, ASTNode* arg1, ASTNode* arg2, ASTNode* arg3);

// Phase 4: Hash map helpers
ASTNode* createAST_MapTanimlama(Token* key_tipi, Token* value_tipi, Token* degisken_adi);
ASTNode* createAST_MapSet(Token* map_adi, ASTNode* key, ASTNode* value);
ASTNode* createAST_MapGet(Token* map_adi, ASTNode* key);
ASTNode* createAST_MapHas(Token* map_adi, ASTNode* key);
ASTNode* createAST_MapRemove(Token* map_adi, ASTNode* key);
ASTNode* createAST_MapSize(Token* map_adi);
ASTNode* createAST_MapClear(Token* map_adi);

#endif // C_AST_H