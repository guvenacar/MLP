#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>  // readlink için
#include <libgen.h>  // dirname için
#include <errno.h>   // errno için
#include <ctype.h>   // toupper, tolower için
#include "json_parser.h"  // MLP Language definitions parser

// Forward declarations
char* runtime_dizin_al();

// Çalıştırılabilir dosyanın tam yolunu alır
char* get_executable_path() {
    char* path = (char*)malloc(1024);
    if (path == NULL) return NULL;

    ssize_t len = readlink("/proc/self/exe", path, 1023);
    if (len != -1) {
        path[len] = '\0';
        return path;
    }

    free(path);
    return NULL;
}

void yazdir_sayi(int64_t sayi) {
    printf("%ld\n", sayi);
}

void yazdir_metin(const char* metin) {
    printf("%s\n", metin);
}

typedef struct {
    char* anahtar;
    int64_t deger;
    int kullanilmis;
} SozlukGirdisi;

SozlukGirdisi* sozluk_tablosu = NULL;
int sozluk_boyutu = 0;

unsigned long hash_fonksiyonu(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void sozluk_yok_et(void) {
    if (sozluk_tablosu == NULL) return;
    for (int i = 0; i < sozluk_boyutu; i++) {
        if (sozluk_tablosu[i].kullanilmis && sozluk_tablosu[i].anahtar != NULL) {
            free(sozluk_tablosu[i].anahtar);
        }
    }
    free(sozluk_tablosu);
    sozluk_tablosu = NULL;
    sozluk_boyutu = 0;
}

void sozluk_olustur(int boyut) {
    if (sozluk_tablosu != NULL) {
        sozluk_yok_et();
    }
    sozluk_boyutu = boyut;
    sozluk_tablosu = (SozlukGirdisi*)calloc(sozluk_boyutu, sizeof(SozlukGirdisi));
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük için hafıza ayrılamadı.\n");
        exit(1);
    }
}

void sozluk_ekle(const char* anahtar, int64_t deger) {
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük başlatılmamış.\n");
        return;
    }
    unsigned long hash = hash_fonksiyonu(anahtar);
    int indeks = hash % sozluk_boyutu;
    int baslangic_indeksi = indeks;
    while (sozluk_tablosu[indeks].kullanilmis) {
        if (strcmp(sozluk_tablosu[indeks].anahtar, anahtar) == 0) {
            sozluk_tablosu[indeks].deger = deger;
            return;
        }
        indeks = (indeks + 1) % sozluk_boyutu;
        if (indeks == baslangic_indeksi) {
            fprintf(stderr, "Hata: Sözlük dolu!\n");
            return;
        }
    }
    sozluk_tablosu[indeks].anahtar = strdup(anahtar);
    sozluk_tablosu[indeks].deger = deger;
    sozluk_tablosu[indeks].kullanilmis = 1;
}

int64_t sozluk_getir(const char* anahtar) {
    if (sozluk_tablosu == NULL) {
        fprintf(stderr, "Hata: Sözlük başlatılmamış.\n");
        return -1;
    }
    unsigned long hash = hash_fonksiyonu(anahtar);
    int indeks = hash % sozluk_boyutu;
    int baslangic_indeksi = indeks;
    while (sozluk_tablosu[indeks].kullanilmis) {
        if (strcmp(sozluk_tablosu[indeks].anahtar, anahtar) == 0) {
            return sozluk_tablosu[indeks].deger;
        }
        indeks = (indeks + 1) % sozluk_boyutu;
        if (indeks == baslangic_indeksi) {
            break;
        }
    }
    fprintf(stderr, "Hata: Anahtar bulunamadı: %s\n", anahtar);
    return -1;
}

char* string_birlestir(const char* str1, const char* str2) {
    
    // ⚠️ KORUMA: Gelen string'lerden herhangi biri NULL ise
    // (Bellek hatası/Segfault riskini engellemek için)
    if (str1 == NULL || str2 == NULL) {
        // Hata ayıklama için terminale yazdıralım
        fprintf(stderr, "Hata: Birleştirme için NULL string argümanı alındı.\n");
        return NULL; // Bu, Assembly'ye de NULL dönecektir.
    }
    
    // 1. Yeni string'in toplam uzunluğunu hesapla (+1, null terminator için)
    size_t uzunluk = strlen(str1) + strlen(str2) + 1;

    // 2. Yeni hafıza bloğu ayır (Dinamik hafıza yönetimi)
    char* yeni_str = (char*)malloc(uzunluk);
    if (yeni_str == NULL) {
        fprintf(stderr, "Hata: Birleştirme için hafıza ayrılamadı.\n");
        return NULL; 
    }

    // 3. İlk string'i yeni bloğa kopyala
    strcpy(yeni_str, str1);

    // 4. İkinci string'i ilkinin sonuna ekle (birleştir)
    strcat(yeni_str, str2);

    // 5. Birleştirilmiş yeni string'in adresini döndür
    return yeni_str;
}

// NOT: Bu fonksiyonun döndürdüğü string'in hafızasını 
// SIL komutu veya SOZLUK_YOK_ET gibi fonksiyonlarla serbest bırakmayı unutmamalıyız.

int string_karsilastir(const char* str1, const char* str2) {
    // strcmp her iki string eşitse 0 döndürür.
    // Assembly'de bu sonucu doğrudan kullanacağız.
    return strcmp(str1, str2);
}

/**
 * STRING_ESIT_MI - İki string'in eşit olup olmadığını kontrol eder
 * @param str1: İlk string
 * @param str2: İkinci string
 * @return: Eşitse 1, değilse 0 (Boolean)
 */
int64_t string_esit_mi(const char* str1, const char* str2) {
    if (str1 == NULL || str2 == NULL) {
        return 0;  // NULL stringler eşit değildir
    }
    return strcmp(str1, str2) == 0 ? 1 : 0;
}

/**
 * STRING_UZUNLUK - String'in uzunluğunu döndürür
 * @param str: String
 * @return: String uzunluğu (karakter sayısı)
 */
int64_t string_uzunluk(const char* str) {
    if (str == NULL) {
        fprintf(stderr, "HATA [STRING_UZUNLUK]: NULL string\n");
        return 0;
    }
    return (int64_t)strlen(str);
}

// =============================================================================
// FILE I/O FONKSİYONLARI
// =============================================================================

/**
 * DOSYA_AC - Dosya açar ve dosya tanıtıcısını (FILE*) döndürür
 * @param yol: Dosya yolu (string) - relative veya absolute
 * @param mod: Açma modu ("r", "w", "a", vb.)
 * @return: FILE* pointer (int64_t olarak cast edilmiş)
 *
 * NOT: Eğer yol relative ise (/ ile başlamıyorsa), programın bulunduğu
 * dizin ile birleştirilerek absolute path oluşturulur. Bu sayede
 * program farklı dizinlerden çalıştırılsa bile dosyaları bulabilir.
 */
int64_t dosya_ac(const char* yol, const char* mod) {
    if (yol == NULL || mod == NULL) {
        fprintf(stderr, "HATA [DOSYA_AC]: NULL argüman alındı\n");
        return 0; // NULL pointer
    }

    char* kullanilacak_yol = NULL;
    int path_allocated = 0; // Flag to track if we allocated memory

    // Path absolute mi kontrol et (/ ile başlıyorsa absolute)
    if (yol[0] == '/') {
        // Absolute path - olduğu gibi kullan
        kullanilacak_yol = (char*)yol;
    } else {
        // Relative path - executable'ın dizini ile birleştir
        char* exe_dir = runtime_dizin_al();
        if (exe_dir == NULL) {
            fprintf(stderr, "HATA [DOSYA_AC]: Executable dizini alınamadı\n");
            return 0;
        }

        // Birleştir: exe_dir + "/" + yol
        size_t uzunluk = strlen(exe_dir) + 1 + strlen(yol) + 1;
        kullanilacak_yol = (char*)malloc(uzunluk);
        if (kullanilacak_yol == NULL) {
            free(exe_dir);
            fprintf(stderr, "HATA [DOSYA_AC]: Hafıza ayırma hatası\n");
            return 0;
        }

        snprintf(kullanilacak_yol, uzunluk, "%s/%s", exe_dir, yol);
        free(exe_dir);
        path_allocated = 1;
    }

    FILE* dosya = fopen(kullanilacak_yol, mod);

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_AC]: Dosya açılamadı: %s\n", kullanilacak_yol);
        if (path_allocated) {
            free(kullanilacak_yol);
        }
        return 0;
    }

    // Eğer yeni path oluşturduysak, onu serbest bırak
    if (path_allocated) {
        free(kullanilacak_yol);
    }

    // FILE* pointer'ı int64_t olarak döndür
    return (int64_t)dosya;
}

/**
 * DOSYA_OKU - Dosyadan tüm içeriği okur ve string olarak döndürür
 * @param dosya_ptr: FILE* pointer (int64_t olarak)
 * @return: Dosya içeriği (dynamically allocated string)
 */
char* dosya_oku(int64_t dosya_ptr) {
    FILE* dosya = (FILE*)dosya_ptr;

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_OKU]: Geçersiz dosya pointer\n");
        return NULL;
    }

    // Dosya boyutunu öğren
    fseek(dosya, 0, SEEK_END);
    long dosya_boyutu = ftell(dosya);
    fseek(dosya, 0, SEEK_SET);

    if (dosya_boyutu < 0) {
        fprintf(stderr, "HATA [DOSYA_OKU]: Dosya boyutu okunamadı\n");
        return NULL;
    }

    // Bellek ayır
    char* icerik = (char*)malloc(dosya_boyutu + 1);
    if (icerik == NULL) {
        fprintf(stderr, "HATA [DOSYA_OKU]: Hafıza ayırma hatası\n");
        return NULL;
    }

    // Dosyayı oku
    size_t okunan = fread(icerik, 1, dosya_boyutu, dosya);
    icerik[okunan] = '\0'; // Null terminator

    return icerik;
}

/**
 * DOSYA_YAZ - Dosyaya string yazar
 * @param dosya_ptr: FILE* pointer (int64_t olarak)
 * @param veri: Yazılacak string
 * @return: Yazılan byte sayısı
 */
int64_t dosya_yaz(int64_t dosya_ptr, const char* veri) {
    FILE* dosya = (FILE*)dosya_ptr;

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_YAZ]: Geçersiz dosya pointer\n");
        return -1;
    }

    if (veri == NULL) {
        fprintf(stderr, "HATA [DOSYA_YAZ]: NULL veri\n");
        return -1;
    }

    size_t yazilan = fwrite(veri, 1, strlen(veri), dosya);
    return (int64_t)yazilan;
}

/**
 * DOSYA_KAPAT - Dosyayı kapatır
 * @param dosya_ptr: FILE* pointer (int64_t olarak)
 * @return: 0 başarılı, -1 hata
 */
int64_t dosya_kapat(int64_t dosya_ptr) {
    FILE* dosya = (FILE*)dosya_ptr;

    if (dosya == NULL) {
        fprintf(stderr, "HATA [DOSYA_KAPAT]: Geçersiz dosya pointer (NULL)\n");
        return -1;
    }

    int sonuc = fclose(dosya);
    if (sonuc != 0) {
        fprintf(stderr, "HATA [DOSYA_KAPAT]: Dosya kapatılamadı (errno=%d)\n", errno);
        return -1;
    }

    return 0;
}

// =============================================================================
// YOL (PATH) YÖNETİMİ
// =============================================================================

/**
 * DIZIN_AL - Çalışan programın bulunduğu dizini döndürür.
 * (Self-hosting için dosya yolu sorununu çözer)
 * * @return: Programın dizin yolu (char*). Bu hafıza 'free' edilmeli.
 */
char* tyd_fix_cwd() {
    // 1. Programın tam yolunu al (zaten var olan fonksiyon)
    char* exe_path = get_executable_path();
    if (exe_path == NULL) {
        fprintf(stderr, "HATA [DIZIN_AL]: Program yolu alınamadı.\n");
        return NULL;
    }

    // 2. 'dirname' girdisini değiştirebilir, bu yüzden bir kopya üzerinde çalış
    // (Aksi takdirde 'exe_path' belleği bozulabilir)
    char* path_copy = strdup(exe_path);
    if (path_copy == NULL) {
        free(exe_path);
        fprintf(stderr, "HATA [DIZIN_AL]: Hafıza kopyalama hatası.\n");
        return NULL;
    }

    // 3. Dizin adını al (örn: /home/pardus/proje/c_compiler)
    char* dir = dirname(path_copy);

    // 4. 'dir' şu anda path_copy'nin içini işaret ediyor.
    // Bellek sızıntısını önlemek için 'dir'in de bir kopyasını oluşturup 
    // onu döndürmeliyiz.
    char* result = strdup(dir);

    // 5. Ara bellekleri temizle
    free(exe_path);   // Orijinal path'i serbest bırak
    free(path_copy);  // Kopyayı serbest bırak

    // 6. Yeni, bağımsız dizin kopyasını döndür
    return result;
}

char* runtime_dizin_al() {
    // 1. Programın tam yolunu al (zaten var olan fonksiyon)
    char* exe_path = get_executable_path();
    if (exe_path == NULL) {
        fprintf(stderr, "HATA [DIZIN_AL]: Program yolu alınamadı.\n");
        return NULL;
    }

    // 2. 'dirname' girdisini değiştirebilir, bu yüzden bir kopya üzerinde çalış
    // (Aksi takdirde 'exe_path' belleği bozulabilir)
    char* path_copy = strdup(exe_path);
    if (path_copy == NULL) {
        free(exe_path);
        fprintf(stderr, "HATA [DIZIN_AL]: Hafıza kopyalama hatası.\n");
        return NULL;
    }

    // 3. Dizin adını al (örn: /home/pardus/proje/c_compiler)
    char* dir = dirname(path_copy);

    // 4. 'dir' şu anda path_copy'nin içini işaret ediyor.
    // Bellek sızıntısını önlemek için 'dir'in de bir kopyasını oluşturup
    // onu döndürmeliyiz.
    char* result = strdup(dir);

    // 5. Ara bellekleri temizle
    free(exe_path);   // Orijinal path'i serbest bırak
    free(path_copy);  // Kopyayı serbest bırak

    // 6. Yeni, bağımsız dizin kopyasını döndür
    return result;
}

// =============================================================================
// STRING İŞLEMLERİ (BOOTSTRAP FONKSİYONLARI)
// =============================================================================
// NOT: Bu fonksiyonlar sadece TYD compiler'ını TYD'de yazmak için gerekli.
// Self-hosting tamamlandıktan sonra bu işlevler TYD'de yeniden yazılacak.

/**
 * STRING_KARAKTER_AL - String'in belirtilen indeksindeki karakteri döndürür
 * @param str: Kaynak string
 * @param indeks: Karakter indeksi (0-tabanlı)
 * @return: Tek karakterlik string (dynamically allocated)
 *
 * Örnek: STRING_KARAKTER_AL("Merhaba", 0) -> "M"
 */
char* string_karakter_al(const char* str, int64_t indeks) {
    if (str == NULL) {
        fprintf(stderr, "HATA [STRING_KARAKTER_AL]: NULL string\n");
        return NULL;
    }

    int64_t uzunluk = strlen(str);
    if (indeks < 0 || indeks >= uzunluk) {
        fprintf(stderr, "HATA [STRING_KARAKTER_AL]: İndeks sınır dışı (indeks=%ld, uzunluk=%ld)\n",
                indeks, uzunluk);
        return NULL;
    }

    // Tek karakterlik string oluştur
    char* sonuc = (char*)malloc(2); // 1 karakter + null terminator
    if (sonuc == NULL) {
        fprintf(stderr, "HATA [STRING_KARAKTER_AL]: Hafıza ayırma hatası\n");
        return NULL;
    }

    sonuc[0] = str[indeks];
    sonuc[1] = '\0';

    return sonuc;
}

/**
 * STRING_ALT - String'in bir kısmını (substring) döndürür
 * @param str: Kaynak string
 * @param baslangic: Başlangıç indeksi (0-tabanlı, dahil)
 * @param uzunluk: Alınacak karakter sayısı
 * @return: Substring (dynamically allocated)
 *
 * Örnek: STRING_ALT("Merhaba", 3, 2) -> "ha"
 */
char* string_alt(const char* str, int64_t baslangic, int64_t uzunluk) {
    if (str == NULL) {
        fprintf(stderr, "HATA [STRING_ALT]: NULL string\n");
        return NULL;
    }

    int64_t str_uzunluk = strlen(str);

    if (baslangic < 0 || baslangic >= str_uzunluk) {
        fprintf(stderr, "HATA [STRING_ALT]: Başlangıç indeksi sınır dışı\n");
        return NULL;
    }

    if (uzunluk < 0) {
        fprintf(stderr, "HATA [STRING_ALT]: Negatif uzunluk\n");
        return NULL;
    }

    // Gerçek alınabilecek uzunluğu hesapla (string sonunu aşmamalı)
    int64_t gercek_uzunluk = uzunluk;
    if (baslangic + uzunluk > str_uzunluk) {
        gercek_uzunluk = str_uzunluk - baslangic;
    }

    // Yeni string için bellek ayır
    char* sonuc = (char*)malloc(gercek_uzunluk + 1);
    if (sonuc == NULL) {
        fprintf(stderr, "HATA [STRING_ALT]: Hafıza ayırma hatası\n");
        return NULL;
    }

    // Karakterleri kopyala
    strncpy(sonuc, str + baslangic, gercek_uzunluk);
    sonuc[gercek_uzunluk] = '\0';

    return sonuc;
}

/**
 * KARAKTER_KODU - Bir karakterin ASCII kodunu döndürür
 * @param karakter_str: Tek karakterlik string
 * @return: ASCII kodu (int64_t)
 *
 * Örnek: KARAKTER_KODU("A") -> 65
 */
int64_t karakter_kodu(const char* karakter_str) {
    if (karakter_str == NULL || karakter_str[0] == '\0') {
        fprintf(stderr, "HATA [KARAKTER_KODU]: NULL veya boş string\n");
        return -1;
    }

    return (int64_t)karakter_str[0];
}

/**
 * KODU_KARAKTERE - ASCII kodunu karaktere dönüştürür
 * @param kod: ASCII kodu
 * @return: Tek karakterlik string (dynamically allocated)
 *
 * Örnek: KODU_KARAKTERE(65) -> "A"
 */
char* kodu_karaktere(int64_t kod) {
    if (kod < 0 || kod > 127) {
        fprintf(stderr, "HATA [KODU_KARAKTERE]: Geçersiz ASCII kodu: %ld\n", kod);
        return NULL;
    }

    char* sonuc = (char*)malloc(2);
    if (sonuc == NULL) {
        fprintf(stderr, "HATA [KODU_KARAKTERE]: Hafıza ayırma hatası\n");
        return NULL;
    }

    sonuc[0] = (char)kod;
    sonuc[1] = '\0';

    return sonuc;
}

// ============================================================================
// DYNAMIC LIST IMPLEMENTATION - Phase 2
// ============================================================================

/**
 * List - Dynamic, growable array structure
 *
 * Memory layout:
 *   - data: Array of pointers (grows with realloc)
 *   - size: Current number of elements
 *   - capacity: Allocated capacity
 *   - element_size: Size of each element (always 8 bytes for pointers)
 */
typedef struct {
    void** data;        // Generic pointer array
    int size;           // Current number of elements
    int capacity;       // Allocated capacity
    int element_size;   // Size of each element (8 bytes)
} List;

#define LIST_INITIAL_CAPACITY 8
#define LIST_GROWTH_FACTOR 2

/**
 * list_create - Creates a new empty list
 * @return: Pointer to new List, or NULL on error
 *
 * MLP Usage: list[int] numbers = list();
 */
List* list_create() {
    List* list = (List*)malloc(sizeof(List));
    if (list == NULL) {
        fprintf(stderr, "HATA [list_create]: Hafıza ayırma hatası\n");
        return NULL;
    }

    list->data = (void**)malloc(LIST_INITIAL_CAPACITY * sizeof(void*));
    if (list->data == NULL) {
        fprintf(stderr, "HATA [list_create]: Data array hafıza hatası\n");
        free(list);
        return NULL;
    }

    list->size = 0;
    list->capacity = LIST_INITIAL_CAPACITY;
    list->element_size = 8;  // 64-bit pointers

    return list;
}

/**
 * list_destroy - Frees list memory
 * @param list: List to destroy
 *
 * NOTE: Does NOT free the elements themselves (user's responsibility)
 */
void list_destroy(List* list) {
    if (list == NULL) return;

    if (list->data != NULL) {
        free(list->data);
    }
    free(list);
}

/**
 * list_grow - Internal function to grow list capacity
 * @param list: List to grow
 * @return: 0 on success, -1 on error
 */
static int list_grow(List* list) {
    int new_capacity = list->capacity * LIST_GROWTH_FACTOR;
    void** new_data = (void**)realloc(list->data, new_capacity * sizeof(void*));

    if (new_data == NULL) {
        fprintf(stderr, "HATA [list_grow]: realloc hatası (capacity: %d -> %d)\n",
                list->capacity, new_capacity);
        return -1;
    }

    list->data = new_data;
    list->capacity = new_capacity;
    return 0;
}

/**
 * list_add - Appends element to end of list
 * @param list: Target list
 * @param element: Element to add (pointer/integer as void*)
 * @return: 0 on success, -1 on error
 *
 * MLP Usage: numbers.add(10)
 */
int list_add(List* list, void* element) {
    if (list == NULL) {
        fprintf(stderr, "HATA [list_add]: NULL list\n");
        return -1;
    }

    // Grow if needed
    if (list->size >= list->capacity) {
        if (list_grow(list) != 0) {
            return -1;
        }
    }

    list->data[list->size] = element;
    list->size++;
    return 0;
}

/**
 * list_get - Gets element at index
 * @param list: Source list
 * @param index: Index to access
 * @return: Element at index, or NULL on error
 *
 * MLP Usage: int x = numbers.get(0)
 */
void* list_get(List* list, int index) {
    if (list == NULL) {
        fprintf(stderr, "HATA [list_get]: NULL list\n");
        return NULL;
    }

    if (index < 0 || index >= list->size) {
        fprintf(stderr, "HATA [list_get]: Index out of bounds (index: %d, size: %d)\n",
                index, list->size);
        return NULL;
    }

    return list->data[index];
}

/**
 * list_size - Gets number of elements in list
 * @param list: Source list
 * @return: Number of elements, or -1 on error
 *
 * MLP Usage: int count = numbers.size()
 */
int list_size(List* list) {
    if (list == NULL) {
        fprintf(stderr, "HATA [list_size]: NULL list\n");
        return -1;
    }

    return list->size;
}

/**
 * list_clear - Removes all elements from list
 * @param list: List to clear
 *
 * NOTE: Keeps allocated capacity (doesn't shrink)
 * MLP Usage: numbers.clear()
 */
void list_clear(List* list) {
    if (list == NULL) {
        fprintf(stderr, "HATA [list_clear]: NULL list\n");
        return;
    }

    list->size = 0;
    // Keep capacity and data array allocated
}

/**
 * list_remove - Removes element at index (shifts remaining elements)
 * @param list: Target list
 * @param index: Index to remove
 * @return: 0 on success, -1 on error
 *
 * MLP Usage: numbers.remove(2)  // Future feature
 */
int list_remove(List* list, int index) {
    if (list == NULL) {
        fprintf(stderr, "HATA [list_remove]: NULL list\n");
        return -1;
    }

    if (index < 0 || index >= list->size) {
        fprintf(stderr, "HATA [list_remove]: Index out of bounds (index: %d, size: %d)\n",
                index, list->size);
        return -1;
    }

    // Shift elements left
    for (int i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }

    list->size--;
    return 0;
}

// ============================================
// Phase 3: File I/O Operations
// ============================================

/**
 * read_file - Read entire file into string
 * @param path: File path
 * @return: File contents or NULL on error
 *
 * MLP Usage: string content = read_file("program.mlp");
 */
char* read_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "HATA [read_file]: Cannot open file: %s\n", path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer
    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        fprintf(stderr, "HATA [read_file]: Memory allocation failed\n");
        return NULL;
    }

    // Read content
    size_t read = fread(buffer, 1, size, file);
    buffer[read] = '\0';

    fclose(file);
    return buffer;
}

/**
 * write_file - Write string to file
 * @param path: File path
 * @param content: Content to write
 * @return: 0 on success, -1 on error
 *
 * MLP Usage: int result = write_file("output.txt", content);
 */
int write_file(const char* path, const char* content) {
    FILE* file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "HATA [write_file]: Cannot open file: %s\n", path);
        return -1;
    }

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);

    fclose(file);
    return (written == len) ? 0 : -1;
}

/**
 * append_file - Append string to file
 * @param path: File path
 * @param content: Content to append
 * @return: 0 on success, -1 on error
 *
 * MLP Usage: int result = append_file("log.txt", "New entry\n");
 */
int append_file(const char* path, const char* content) {
    FILE* file = fopen(path, "a");
    if (!file) {
        fprintf(stderr, "HATA [append_file]: Cannot open file: %s\n", path);
        return -1;
    }

    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);

    fclose(file);
    return (written == len) ? 0 : -1;
}

/**
 * file_exists - Check if file exists
 * @param path: File path
 * @return: 1 if exists, 0 if not
 *
 * MLP Usage: int exists = file_exists("config.txt");
 */
int file_exists(const char* path) {
    FILE* file = fopen(path, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

/**
 * file_size - Get file size in bytes
 * @param path: File path
 * @return: File size or -1 on error
 *
 * MLP Usage: int size = file_size("data.bin");
 */
long file_size(const char* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);

    return size;
}

/**
 * read_lines - Read file as array of lines
 * @param path: File path
 * @param line_count: Output parameter for line count
 * @return: Array of strings (lines) or NULL on error
 *
 * MLP Usage: list[string] lines = read_lines("file.txt");
 * Note: This will be integrated with list[string] in future
 */
char** read_lines(const char* path, int* line_count) {
    char* content = read_file(path);
    if (!content) {
        *line_count = 0;
        return NULL;
    }

    // Count lines
    int count = 1;
    for (char* p = content; *p; p++) {
        if (*p == '\n') count++;
    }

    // Allocate line array
    char** lines = malloc(sizeof(char*) * count);
    int line_idx = 0;
    char* line_start = content;

    // Split by newlines
    for (char* p = content; *p; p++) {
        if (*p == '\n') {
            *p = '\0';
            lines[line_idx++] = strdup(line_start);
            line_start = p + 1;
        }
    }

    // Last line (if no trailing newline)
    if (*line_start) {
        lines[line_idx++] = strdup(line_start);
    }

    free(content);
    *line_count = line_idx;
    return lines;
}

// ============================================
// Phase 3: String Operations
// ============================================

/**
 * string_split - Split string by delimiter
 * @param str: String to split
 * @param delim: Delimiter
 * @param count: Output parameter for part count
 * @return: Array of strings
 *
 * MLP Usage: list[string] parts = string_split("a,b,c", ",");
 */
char** string_split(const char* str, const char* delim, int* count) {
    char* copy = strdup(str);
    int capacity = 10;
    char** parts = malloc(sizeof(char*) * capacity);
    *count = 0;

    char* token = strtok(copy, delim);
    while (token) {
        if (*count >= capacity) {
            capacity *= 2;
            parts = realloc(parts, sizeof(char*) * capacity);
        }
        parts[(*count)++] = strdup(token);
        token = strtok(NULL, delim);
    }

    free(copy);
    return parts;
}

/**
 * string_join - Join strings with delimiter
 * @param parts: Array of strings
 * @param count: Number of strings
 * @param delim: Delimiter
 * @return: Joined string
 *
 * MLP Usage: string result = string_join(parts, "-");
 */
char* string_join(char** parts, int count, const char* delim) {
    if (count == 0) return strdup("");

    // Calculate total length
    size_t total_len = 0;
    size_t delim_len = strlen(delim);

    for (int i = 0; i < count; i++) {
        total_len += strlen(parts[i]);
        if (i < count - 1) total_len += delim_len;
    }

    // Allocate buffer
    char* result = malloc(total_len + 1);
    result[0] = '\0';

    // Join parts
    for (int i = 0; i < count; i++) {
        strcat(result, parts[i]);
        if (i < count - 1) strcat(result, delim);
    }

    return result;
}

/**
 * string_replace - Replace all occurrences of substring
 * @param str: Original string
 * @param old: Substring to replace
 * @param new: Replacement string
 * @return: New string with replacements
 *
 * MLP Usage: string result = string_replace("hello world", "world", "MLP");
 */
char* string_replace(const char* str, const char* old, const char* new) {
    char* result;
    int i, count = 0;
    size_t new_len = strlen(new);
    size_t old_len = strlen(old);

    // Count occurrences
    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], old) == &str[i]) {
            count++;
            i += old_len - 1;
        }
    }

    // Allocate result
    result = malloc(i + count * (new_len - old_len) + 1);

    // Replace
    i = 0;
    while (*str) {
        if (strstr(str, old) == str) {
            strcpy(&result[i], new);
            i += new_len;
            str += old_len;
        } else {
            result[i++] = *str++;
        }
    }

    result[i] = '\0';
    return result;
}

/**
 * string_trim - Remove leading and trailing whitespace
 * @param str: String to trim
 * @return: Trimmed string
 *
 * MLP Usage: string trimmed = string_trim("  hello  ");
 */
char* string_trim(const char* str) {
    while (isspace(*str)) str++;
    if (*str == '\0') return strdup("");

    const char* end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;

    size_t len = end - str + 1;
    char* result = malloc(len + 1);
    memcpy(result, str, len);
    result[len] = '\0';
    return result;
}

/**
 * string_upper - Convert string to uppercase
 * @param str: String to convert
 * @return: Uppercase string
 *
 * MLP Usage: string upper = string_upper("hello");
 */
char* string_upper(const char* str) {
    char* result = strdup(str);
    for (char* p = result; *p; p++) {
        *p = toupper((unsigned char)*p);
    }
    return result;
}

/**
 * string_lower - Convert string to lowercase
 * @param str: String to convert
 * @return: Lowercase string
 *
 * MLP Usage: string lower = string_lower("HELLO");
 */
char* string_lower(const char* str) {
    char* result = strdup(str);
    for (char* p = result; *p; p++) {
        *p = tolower((unsigned char)*p);
    }
    return result;
}

/**
 * string_find - Find first occurrence of substring
 * @param str: String to search in
 * @param needle: Substring to find
 * @return: Index of first occurrence, or -1 if not found
 *
 * MLP Usage: int pos = string_find("hello world", "world");
 */
int string_find(const char* str, const char* needle) {
    const char* pos = strstr(str, needle);
    return pos ? (pos - str) : -1;
}

/**
 * string_starts_with - Check if string starts with prefix
 * @param str: String to check
 * @param prefix: Prefix to look for
 * @return: 1 if starts with prefix, 0 otherwise
 *
 * MLP Usage: int starts = string_starts_with("hello world", "hello");
 */
int string_starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0 ? 1 : 0;
}

/**
 * string_ends_with - Check if string ends with suffix
 * @param str: String to check
 * @param suffix: Suffix to look for
 * @return: 1 if ends with suffix, 0 otherwise
 *
 * MLP Usage: int ends = string_ends_with("hello world", "world");
 */
int string_ends_with(const char* str, const char* suffix) {
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (suffix_len > str_len) return 0;
    return strcmp(str + str_len - suffix_len, suffix) == 0 ? 1 : 0;
}

