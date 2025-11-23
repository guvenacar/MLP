#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>    // floor, ceil, sqrt, etc.
#include <unistd.h>  // readlink için
#include <libgen.h>  // dirname için
#include <errno.h>   // errno için
#include <ctype.h>   // toupper, tolower için
#include <time.h>    // time için (Phase 5.2)
#include <sys/stat.h>  // stat, mkdir için (Phase 5.3)
#include <dirent.h>  // opendir, readdir için (Phase 5.3)
#include <sys/wait.h>  // WIFEXITED, WEXITSTATUS için (Phase 5.3)
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

// English alias
char* string_char_at(const char* str, int64_t index) {
    return string_karakter_al(str, index);
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

// English aliases for multi-language support
int64_t char_code(const char* ch) {
    return karakter_kodu(ch);
}

char* code_to_char(int64_t code) {
    return kodu_karaktere(code);
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

// ============================================
// Phase 4: Type Conversion Operations
// ============================================

/**
 * string_to_int - Convert string to integer
 * @param str: String to convert
 * @return: Integer value, or 0 on error
 *
 * MLP Usage: int num = string_to_int("123");
 */
int64_t string_to_int(const char* str) {
    if (str == NULL || *str == '\0') {
        fprintf(stderr, "HATA [string_to_int]: NULL or empty string\n");
        return 0;
    }
    return (int64_t)atoll(str);
}

/**
 * int_to_string - Convert integer to string
 * @param num: Integer to convert
 * @return: String representation (dynamically allocated)
 *
 * MLP Usage: string text = int_to_string(123);
 */
char* int_to_string(int64_t num) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld", num);
    return strdup(buffer);
}

/**
 * string_to_float - Convert string to float
 * @param str: String to convert
 * @return: Float value, or 0.0 on error
 *
 * MLP Usage: float num = string_to_float("123.45");
 */
double string_to_float(const char* str) {
    if (str == NULL || *str == '\0') {
        fprintf(stderr, "HATA [string_to_float]: NULL or empty string\n");
        return 0.0;
    }
    return atof(str);
}

/**
 * float_to_string - Convert float to string
 * @param num: Float to convert
 * @return: String representation (dynamically allocated)
 *
 * MLP Usage: string text = float_to_string(123.45);
 */
char* float_to_string(double num) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.6f", num);
    return strdup(buffer);
}

/**
 * string_to_bool - Convert string to boolean
 * @param str: String to convert ("true"/"false", "1"/"0", "yes"/"no")
 * @return: 1 for true, 0 for false
 *
 * MLP Usage: int flag = string_to_bool("true");
 */
int string_to_bool(const char* str) {
    if (str == NULL) return 0;
    
    if (strcmp(str, "true") == 0 || strcmp(str, "TRUE") == 0 ||
        strcmp(str, "1") == 0 || strcmp(str, "yes") == 0) {
        return 1;
    }
    return 0;
}

/**
 * bool_to_string - Convert boolean to string
 * @param value: Boolean value (0 or 1)
 * @return: "true" or "false" (dynamically allocated)
 *
 * MLP Usage: string text = bool_to_string(1);
 */
char* bool_to_string(int value) {
    return strdup(value ? "true" : "false");
}

/**
 * char_to_string - Convert ASCII code to single-character string
 * @param ch: ASCII/Unicode code point
 * @return: Single-character string (caller must free)
 *
 * MLP Usage: string s = char_to_string(65);  // "A"
 */
char* char_to_string(int ch) {
    char* str = (char*)malloc(2);
    if (!str) {
        fprintf(stderr, "Memory allocation failed in char_to_string\n");
        exit(1);
    }
    str[0] = (char)ch;
    str[1] = '\0';
    return str;
}

/**
 * string_concat - Concatenate two strings
 * @param s1: First string
 * @param s2: Second string
 * @return: Concatenated string (caller must free)
 *
 * MLP Usage: string result = string_concat("Hello", " World");
 */
char* string_concat(const char* s1, const char* s2) {
    if (!s1) s1 = "";
    if (!s2) s2 = "";

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char* result = (char*)malloc(len1 + len2 + 1);

    if (!result) {
        fprintf(stderr, "Memory allocation failed in string_concat\n");
        exit(1);
    }

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// ============================================
// Input Functions (stdin)
// ============================================

/**
 * read_input - Read a line from stdin (without newline)
 * @return: Input string (caller must free)
 *
 * MLP Usage: string input = read_input()
 * Example:
 *   print "Enter your name: "
 *   string name = read_input()
 *   print "Hello, " + name
 */
char* read_input() {
    char* buffer = (char*)malloc(1024);
    if (!buffer) {
        fprintf(stderr, "HATA [read_input]: Memory allocation failed\n");
        exit(1);
    }

    if (fgets(buffer, 1024, stdin) == NULL) {
        free(buffer);
        return strdup(""); // Return empty string on EOF
    }

    // Remove trailing newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    return buffer;
}

/**
 * read_line - Alias for read_input (reads a line from stdin)
 * @return: Input string (caller must free)
 *
 * MLP Usage: string line = read_line()
 */
char* read_line() {
    return read_input();
}

/**
 * read_int - Read an integer from stdin
 * @return: Parsed integer value
 *
 * MLP Usage: numeric num = read_int()
 * Example:
 *   print "Enter a number: "
 *   numeric x = read_int()
 *   print "You entered: " + str(x)
 */
int64_t read_int() {
    char* input = read_input();
    int64_t result = string_to_int(input);
    free(input);
    return result;
}

// ============================================
// Type Casting Helpers (MLP-friendly)
// ============================================

/**
 * num - Convert string to numeric (supports both integer and decimal)
 * @param str: String to convert
 * @return: Numeric value (as double for BigDecimal behavior)
 *
 * MLP Usage: numeric x = num("123.45")
 * Examples:
 *   numeric a = num("42")      // 42.0
 *   numeric b = num("3.14")    // 3.14
 *   numeric c = num("-10")     // -10.0
 */
double num(const char* str) {
    if (!str || str[0] == '\0') {
        fprintf(stderr, "HATA [num]: NULL or empty string\n");
        exit(1);
    }
    
    // Check if string contains decimal point
    if (strchr(str, '.') != NULL) {
        return string_to_float(str);
    } else {
        return (double)string_to_int(str);
    }
}

/**
 * str - Convert numeric to string (smart formatting)
 * @param value: Numeric value to convert
 * @return: String representation (caller must free)
 *
 * MLP Usage: string s = str(42.5)
 * Examples:
 *   string a = str(42.0)    // "42" (removes .0)
 *   string b = str(3.14)    // "3.14"
 *   string c = str(-10)     // "-10"
 */
char* str(double value) {
    // Check if value is effectively an integer
    if (value == floor(value)) {
        // No decimal part, format as integer
        return int_to_string((int64_t)value);
    } else {
        // Has decimal part, format as float
        return float_to_string(value);
    }
}

/**
 * mlp_get_type - Get the type of a value as a string (Phase 5.7)
 * @param value: The value to check (as int64_t, can be number or pointer)
 * @return: Type name string ("numeric", "string", "boolean", "pointer")
 * 
 * Note: This is a simplified implementation. Full type detection would require
 * runtime type metadata. Currently distinguishes between:
 * - Small integers (likely numeric values)
 * - Pointers to strings (checks if valid memory with printable chars)
 * - Boolean values (0 or 1)
 * 
 * MLP Usage: string type_name = typeof(variable);
 */
const char* mlp_get_type(int64_t value) {
    // Check if it's likely a boolean (0 or 1)
    if (value == 0 || value == 1) {
        return "boolean";
    }
    
    // Check if it's a small integer (likely numeric)
    if (value >= -1000000 && value <= 1000000) {
        return "numeric";
    }
    
    // Check if it's a valid pointer (likely string)
    // Pointers are typically in high memory ranges
    if (value > 0x1000) {
        const char* ptr = (const char*)value;
        // Try to check if it's a valid string by checking first char
        // This is unsafe but we have no better way without type metadata
        // In production, we'd use actual type tracking
        if (ptr != NULL) {
            // Assume it's a string pointer
            return "string";
        }
    }
    
    // Default to numeric for other values
    return "numeric";
}

// ============================================
// Phase 4: Math Operations
// ============================================

/**
 * math_sqrt - Square root
 * @param num: Number
 * @return: Square root
 *
 * MLP Usage: float result = math_sqrt(16.0);
 */
double math_sqrt(double num) {
    if (num < 0) {
        fprintf(stderr, "HATA [math_sqrt]: Negative number\n");
        return 0.0;
    }
    return sqrt(num);
}

/**
 * math_pow - Power function
 * @param base: Base number
 * @param exponent: Exponent
 * @return: base^exponent
 *
 * MLP Usage: float result = math_pow(2.0, 3.0);
 */
double math_pow(double base, double exponent) {
    return pow(base, exponent);
}

/**
 * math_abs - Absolute value (integer)
 * @param num: Number
 * @return: Absolute value
 *
 * MLP Usage: int result = math_abs(-5);
 */
int64_t math_abs(int64_t num) {
    return num < 0 ? -num : num;
}

/**
 * math_fabs - Absolute value (float)
 * @param num: Number
 * @return: Absolute value
 *
 * MLP Usage: float result = math_fabs(-5.5);
 */
double math_fabs(double num) {
    return fabs(num);
}

/**
 * math_min - Minimum of two integers
 * @param a: First number
 * @param b: Second number
 * @return: Minimum value
 *
 * MLP Usage: int result = math_min(5, 10);
 */
int64_t math_min(int64_t a, int64_t b) {
    return a < b ? a : b;
}

/**
 * math_max - Maximum of two integers
 * @param a: First number
 * @param b: Second number
 * @return: Maximum value
 *
 * MLP Usage: int result = math_max(5, 10);
 */
int64_t math_max(int64_t a, int64_t b) {
    return a > b ? a : b;
}

/**
 * math_round - Round to nearest integer
 * @param num: Number to round
 * @return: Rounded value
 *
 * MLP Usage: int result = math_round(5.7);
 */
int64_t math_round(double num) {
    return (int64_t)round(num);
}

/**
 * math_floor - Floor function
 * @param num: Number
 * @return: Floor value
 *
 * MLP Usage: int result = math_floor(5.7);
 */
int64_t math_floor(double num) {
    return (int64_t)floor(num);
}

/**
 * math_ceil - Ceiling function
 * @param num: Number
 * @return: Ceiling value
 *
 * MLP Usage: int result = math_ceil(5.3);
 */
int64_t math_ceil(double num) {
    return (int64_t)ceil(num);
}

/**
 * math_sin - Sine function
 * @param angle: Angle in radians
 * @return: Sine value
 *
 * MLP Usage: float result = math_sin(1.57);
 */
double math_sin(double angle) {
    return sin(angle);
}

/**
 * math_cos - Cosine function
 * @param angle: Angle in radians
 * @return: Cosine value
 *
 * MLP Usage: float result = math_cos(0.0);
 */
double math_cos(double angle) {
    return cos(angle);
}

/**
 * math_tan - Tangent function
 * @param angle: Angle in radians
 * @return: Tangent value
 *
 * MLP Usage: float result = math_tan(0.785);
 */
double math_tan(double angle) {
    return tan(angle);
}

// ============================================
// Phase 5.1: Command-Line Arguments
// ============================================

// Global argc/argv storage
static int mlp_argc = 0;
static char** mlp_argv = NULL;

/**
 * mlp_set_args - Store command-line arguments
 * Called by main() before executing MLP code
 * @param argc: Number of arguments
 * @param argv: Array of argument strings
 */
void mlp_set_args(int argc, char** argv) {
    mlp_argc = argc;
    mlp_argv = argv;
}

/**
 * mlp_get_argc - Get argument count
 * @return: Number of command-line arguments
 *
 * MLP Usage: int count = argc;
 */
long mlp_get_argc(void) {
    return mlp_argc;
}

/**
 * mlp_get_argv - Get argument at index
 * @param index: Argument index (0 = program name)
 * @return: Argument string, or empty string if out of bounds
 *
 * MLP Usage: string arg = argv.get(0);
 */
char* mlp_get_argv(long index) {
    if (index < 0 || index >= mlp_argc) {
        return strdup("");
    }
    return strdup(mlp_argv[index]);
}

// ============================================
// Phase 5.1: Enhanced String Operations
// ============================================

/**
 * string_length - Get string length
 * @param str: Input string
 * @return: Length of string
 *
 * MLP Usage: int len = string_length("hello");
 */
long string_length(const char* str) {
    if (!str) return 0;
    return strlen(str);
}

/**
 * string_substring - Extract substring
 * @param str: Input string
 * @param start: Start index (0-based)
 * @param length: Number of characters
 * @return: Substring (caller must free)
 *
 * MLP Usage: string sub = string_substring("hello", 1, 3); // "ell"
 */
char* string_substring(const char* str, long start, long length) {
    if (!str) return strdup("");

    long str_len = strlen(str);
    if (start < 0 || start >= str_len) return strdup("");
    if (length < 0) length = 0;

    // Adjust length if it exceeds string
    if (start + length > str_len) {
        length = str_len - start;
    }

    char* result = (char*)malloc(length + 1);
    if (!result) {
        fprintf(stderr, "Memory allocation failed in string_substring\n");
        exit(1);
    }

    strncpy(result, str + start, length);
    result[length] = '\0';
    return result;
}

/**
 * string_index_of - Find first occurrence of substring
 * @param str: String to search in
 * @param needle: Substring to find
 * @return: Index of first occurrence, or -1 if not found
 *
 * MLP Usage: int pos = string_index_of("hello world", "world"); // 6
 */
long string_index_of(const char* str, const char* needle) {
    if (!str || !needle) return -1;

    char* found = strstr(str, needle);
    if (!found) return -1;

    return found - str;
}

/**
 * string_last_index_of - Find last occurrence of substring
 * @param str: String to search in
 * @param needle: Substring to find
 * @return: Index of last occurrence, or -1 if not found
 *
 * MLP Usage: int pos = string_last_index_of("hello hello", "hello"); // 6
 */
long string_last_index_of(const char* str, const char* needle) {
    if (!str || !needle) return -1;

    char* last_found = NULL;
    char* current = (char*)str;

    while ((current = strstr(current, needle)) != NULL) {
        last_found = current;
        current++;
    }

    if (!last_found) return -1;
    return last_found - str;
}

/**
 * char_code - Get ASCII/UTF-8 code of character at index
 * @param str: Input string
 * @param index: Character index (0-based)
 * @return: ASCII code of character, or 0 if index out of bounds
 *
 * MLP Usage: numeric code = char_code_at("A", 0); // 65
 */
long char_code_at(const char* str, long index) {
    if (!str) return 0;
    
    long len = strlen(str);
    if (index < 0 || index >= len) return 0;
    
    return (unsigned char)str[index];
}

/**
 * char_at - Get character at index as single-char string
 * @param str: Input string
 * @param index: Character index (0-based)
 * @return: Single character string (caller must free), or empty string if out of bounds
 *
 * MLP Usage: string ch = char_at("hello", 1); // "e"
 */
char* char_at(const char* str, long index) {
    if (!str) return strdup("");
    
    long len = strlen(str);
    if (index < 0 || index >= len) return strdup("");
    
    char* result = (char*)malloc(2);
    if (!result) {
        fprintf(stderr, "Memory allocation failed in char_at\n");
        exit(1);
    }
    
    result[0] = str[index];
    result[1] = '\0';
    return result;
}

// ========================================================================
// MODERN WRAPPERS - Python/VB.NET Style API
// ========================================================================

/**
 * len - Get length of string (Python-style)
 * @param str: Input string
 * @return: Length of string
 *
 * MLP Usage: numeric length = len("hello")  // 5
 */
long len(const char* str) {
    return string_length(str);
}

/**
 * ord - Get ASCII/Unicode code of first character (Python-style)
 * @param ch: Single character string
 * @return: ASCII code, or 0 if empty
 *
 * MLP Usage: numeric code = ord("A")  // 65
 */
long ord(const char* ch) {
    if (!ch || ch[0] == '\0') return 0;
    return (unsigned char)ch[0];
}

/**
 * chr - Convert ASCII code to single-char string (Python-style)
 * @param code: ASCII code
 * @return: Single character string (caller must free)
 *
 * MLP Usage: string ch = chr(65)  // "A"
 */
char* chr(long code) {
    if (code < 0 || code > 255) return strdup("");
    
    char* result = (char*)malloc(2);
    if (!result) {
        fprintf(stderr, "Memory allocation failed in chr\n");
        exit(1);
    }
    
    result[0] = (char)code;
    result[1] = '\0';
    return result;
}

/**
 * substring - Extract substring (modern style, simpler params)
 * @param str: Input string
 * @param start: Start index
 * @param length: Number of characters
 * @return: Substring (caller must free)
 *
 * MLP Usage: string sub = substring("hello", 1, 3)  // "ell"
 */
char* substring(const char* str, long start, long length) {
    return string_substring(str, start, length);
}

/**
 * mid - VB.NET style substring (alternative name)
 * @param str: Input string
 * @param start: Start index (1-based for VB compatibility, converted to 0-based)
 * @param length: Number of characters
 * @return: Substring (caller must free)
 *
 * MLP Usage: string sub = mid("hello", 2, 3)  // "ell" (1-based)
 */
char* mid(const char* str, long start, long length) {
    // VB.NET uses 1-based indexing, convert to 0-based
    if (start > 0) start--;
    return string_substring(str, start, length);
}

/**
 * left - Get leftmost N characters (VB.NET style)
 * @param str: Input string
 * @param length: Number of characters
 * @return: Substring (caller must free)
 *
 * MLP Usage: string sub = left("hello", 3)  // "hel"
 */
char* left(const char* str, long length) {
    return string_substring(str, 0, length);
}

/**
 * right - Get rightmost N characters (VB.NET style)
 * @param str: Input string
 * @param length: Number of characters
 * @return: Substring (caller must free)
 *
 * MLP Usage: string sub = right("hello", 3)  // "llo"
 */
char* right(const char* str, long length) {
    if (!str) return strdup("");
    long str_len = strlen(str);
    if (length >= str_len) return strdup(str);
    return string_substring(str, str_len - length, length);
}

/**
 * contains - Check if string contains substring (modern style)
 * @param str: String to search in
 * @param needle: Substring to find
 * @return: 1 if found, 0 if not
 *
 * MLP Usage: if contains("hello world", "world") then
 */
long contains(const char* str, const char* needle) {
    return string_index_of(str, needle) >= 0 ? 1 : 0;
}

/**
 * startswith - Check if string starts with prefix (Python-style)
 * @param str: String to check
 * @param prefix: Prefix to look for
 * @return: 1 if starts with prefix, 0 if not
 *
 * MLP Usage: if startswith("hello", "he") then
 */
long startswith(const char* str, const char* prefix) {
    if (!str || !prefix) return 0;
    size_t prefix_len = strlen(prefix);
    if (strlen(str) < prefix_len) return 0;
    return strncmp(str, prefix, prefix_len) == 0 ? 1 : 0;
}

/**
 * endswith - Check if string ends with suffix (Python-style)
 * @param str: String to check
 * @param suffix: Suffix to look for
 * @return: 1 if ends with suffix, 0 if not
 *
 * MLP Usage: if endswith("hello", "lo") then
 */
long endswith(const char* str, const char* suffix) {
    if (!str || !suffix) return 0;
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (str_len < suffix_len) return 0;
    return strcmp(str + str_len - suffix_len, suffix) == 0 ? 1 : 0;
}

// ========================================================================
// PHASE 5.2: ERROR HANDLING & MEMORY MANAGEMENT
// ========================================================================

// -------------------- ERROR HANDLING (5 functions) --------------------

// Global error code storage
static int mlp_last_error_code = 0;

/**
 * exit_with_code - Exit program with specific return code
 * @param code: Exit code (0 = success, non-zero = error)
 *
 * MLP Usage: exit_with_code(1);
 */
void exit_with_code(long code) {
    exit((int)code);
}

/**
 * panic - Print error message to stderr and exit with code 1
 * @param message: Error message to print
 *
 * MLP Usage: panic("Fatal error: division by zero");
 */
void panic(const char* message) {
    if (message) {
        fprintf(stderr, "PANIC: %s\n", message);
    } else {
        fprintf(stderr, "PANIC: Unknown error\n");
    }
    exit(1);
}

/**
 * mlp_assert - Check condition, panic if false
 * @param condition: Condition to check (0 = false, non-zero = true)
 * @param message: Error message if condition fails
 *
 * MLP Usage: mlp_assert(x > 0, "x must be positive");
 */
void mlp_assert(long condition, const char* message) {
    if (!condition) {
        if (message) {
            fprintf(stderr, "ASSERTION FAILED: %s\n", message);
        } else {
            fprintf(stderr, "ASSERTION FAILED\n");
        }
        exit(1);
    }
}

/**
 * get_error_code - Get last error code
 * @return: Last error code set
 *
 * MLP Usage: int code = get_error_code();
 */
long get_error_code(void) {
    return mlp_last_error_code;
}

/**
 * set_error_code - Set error code
 * @param code: Error code to set
 *
 * MLP Usage: set_error_code(42);
 */
void set_error_code(long code) {
    mlp_last_error_code = code;
}

// -------------------- MEMORY MANAGEMENT (6 functions) --------------------

// Memory tracking structure
typedef struct MemoryBlock {
    void* ptr;
    size_t size;
    struct MemoryBlock* next;
} MemoryBlock;

static MemoryBlock* memory_list_head = NULL;
static size_t total_allocated = 0;
static size_t total_freed = 0;

/**
 * track_allocation - Add allocation to tracking list
 */
static void track_allocation(void* ptr, size_t size) {
    if (!ptr) return;

    MemoryBlock* block = (MemoryBlock*)malloc(sizeof(MemoryBlock));
    if (!block) return;  // Tracking failure shouldn't crash program

    block->ptr = ptr;
    block->size = size;
    block->next = memory_list_head;
    memory_list_head = block;
    total_allocated += size;
}

/**
 * untrack_allocation - Remove allocation from tracking list
 */
static void untrack_allocation(void* ptr) {
    if (!ptr) return;

    MemoryBlock** current = &memory_list_head;
    while (*current) {
        if ((*current)->ptr == ptr) {
            MemoryBlock* to_free = *current;
            total_freed += to_free->size;
            *current = to_free->next;
            free(to_free);
            return;
        }
        current = &(*current)->next;
    }
}

/**
 * mlp_malloc - Allocate memory with tracking
 * @param size: Number of bytes to allocate
 * @return: Pointer to allocated memory, NULL on failure
 *
 * MLP Usage: string buffer = mlp_malloc(256);
 */
void* mlp_malloc(long size) {
    if (size <= 0) return NULL;

    void* ptr = malloc((size_t)size);
    if (ptr) {
        track_allocation(ptr, (size_t)size);
    }
    return ptr;
}

/**
 * mlp_free - Free memory with tracking
 * @param ptr: Pointer to memory to free
 *
 * MLP Usage: mlp_free(buffer);
 */
void mlp_free(void* ptr) {
    if (!ptr) return;

    untrack_allocation(ptr);
    free(ptr);
}

/**
 * mlp_realloc - Reallocate memory with tracking
 * @param ptr: Pointer to existing memory (or NULL)
 * @param size: New size in bytes
 * @return: Pointer to reallocated memory
 *
 * MLP Usage: buffer = mlp_realloc(buffer, 512);
 */
void* mlp_realloc(void* ptr, long size) {
    if (size <= 0) {
        mlp_free(ptr);
        return NULL;
    }

    if (!ptr) {
        return mlp_malloc(size);
    }

    // Find old size for tracking
    size_t old_size = 0;
    MemoryBlock* current = memory_list_head;
    while (current) {
        if (current->ptr == ptr) {
            old_size = current->size;
            break;
        }
        current = current->next;
    }

    void* new_ptr = realloc(ptr, (size_t)size);
    if (new_ptr) {
        untrack_allocation(ptr);
        track_allocation(new_ptr, (size_t)size);
    }
    return new_ptr;
}

/**
 * mlp_calloc - Allocate zeroed memory with tracking
 * @param count: Number of elements
 * @param size: Size of each element
 * @return: Pointer to allocated zeroed memory
 *
 * MLP Usage: int array = mlp_calloc(10, 8);  // 10 x 8 bytes
 */
void* mlp_calloc(long count, long size) {
    if (count <= 0 || size <= 0) return NULL;

    void* ptr = calloc((size_t)count, (size_t)size);
    if (ptr) {
        track_allocation(ptr, (size_t)(count * size));
    }
    return ptr;
}

/**
 * get_allocated_bytes - Get total allocated memory
 * @return: Total bytes currently allocated
 *
 * MLP Usage: int total = get_allocated_bytes();
 */
long get_allocated_bytes(void) {
    return (long)(total_allocated - total_freed);
}

/**
 * check_memory_leaks - Check for memory leaks and report
 * @return: Number of leaked blocks
 *
 * MLP Usage: int leaks = check_memory_leaks();
 */
long check_memory_leaks(void) {
    long leak_count = 0;
    size_t leaked_bytes = 0;

    MemoryBlock* current = memory_list_head;
    while (current) {
        leak_count++;
        leaked_bytes += current->size;
        fprintf(stderr, "MEMORY LEAK: %zu bytes at %p\n",
                current->size, current->ptr);
        current = current->next;
    }

    if (leak_count > 0) {
        fprintf(stderr, "TOTAL LEAKS: %ld blocks, %zu bytes\n",
                leak_count, leaked_bytes);
    }

    return leak_count;
}

// -------------------- SYSTEM UTILITIES (3 functions) --------------------

/**
 * get_env - Get environment variable value
 * @param name: Environment variable name
 * @return: Value string, or empty string if not found
 *
 * MLP Usage: string path = get_env("PATH");
 */
char* get_env(const char* name) {
    if (!name) return strdup("");

    char* value = getenv(name);
    if (!value) return strdup("");

    return strdup(value);
}

/**
 * current_timestamp - Get current Unix timestamp
 * @return: Current time in seconds since epoch
 *
 * MLP Usage: int now = current_timestamp();
 */
long current_timestamp(void) {
    return (long)time(NULL);
}

/**
 * sleep_ms - Sleep for milliseconds
 * @param milliseconds: Time to sleep in milliseconds
 *
 * MLP Usage: sleep_ms(1000);  // Sleep 1 second
 */
void sleep_ms(long milliseconds) {
    if (milliseconds <= 0) return;

    #ifdef _WIN32
        Sleep((DWORD)milliseconds);
    #else
        usleep((useconds_t)(milliseconds * 1000));
    #endif
}

// ==================== PHASE 5.3: BINARY FILE I/O (4 functions) ====================

/**
 * read_binary - Read entire file as binary data
 * @param path: File path (absolute or relative)
 * @return: Binary data buffer, or NULL on error
 *
 * Error codes:
 *   ERR_FILE_NOT_FOUND (101): File doesn't exist
 *   ERR_PERMISSION_DENIED (102): No read permission
 *   ERR_OUT_OF_MEMORY (104): File too large
 *   ERR_IO_ERROR (103): Read failed
 *
 * MLP Usage: 
 *   data := read_binary("output.bin")
 *   if data == null { print "Failed to read file" }
 */
void* read_binary(const char* path) {
    if (!path) {
        set_error_code(101);  // ERR_FILE_NOT_FOUND
        return NULL;
    }

    FILE* file = fopen(path, "rb");
    if (!file) {
        set_error_code(101);  // ERR_FILE_NOT_FOUND
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size < 0) {
        fclose(file);
        set_error_code(103);  // ERR_IO_ERROR
        return NULL;
    }
    
    // Allocate buffer
    void* buffer = mlp_malloc(size + 1);  // +1 for potential null terminator
    if (!buffer) {
        fclose(file);
        set_error_code(104);  // ERR_OUT_OF_MEMORY
        return NULL;
    }
    
    // Read data
    size_t bytes_read = fread(buffer, 1, size, file);
    fclose(file);
    
    if (bytes_read != (size_t)size) {
        mlp_free(buffer);
        set_error_code(103);  // ERR_IO_ERROR
        return NULL;
    }
    
    // Null-terminate for safety
    ((char*)buffer)[size] = '\0';
    
    return buffer;
}

/**
 * write_binary - Write binary data to file
 * @param path: File path (absolute or relative)
 * @param data: Binary data buffer
 * @param size: Number of bytes to write
 * @return: 1 on success, 0 on failure
 *
 * Error codes:
 *   ERR_PERMISSION_DENIED (102): Cannot create/write file
 *   ERR_IO_ERROR (103): Write failed
 *
 * MLP Usage:
 *   success := write_binary("output.bin", data, 1024)
 *   if !success { print "Write failed" }
 */
int write_binary(const char* path, const void* data, size_t size) {
    if (!path || !data) {
        set_error_code(102);  // ERR_PERMISSION_DENIED
        return 0;
    }

    FILE* file = fopen(path, "wb");
    if (!file) {
        set_error_code(102);  // ERR_PERMISSION_DENIED
        return 0;
    }
    
    size_t bytes_written = fwrite(data, 1, size, file);
    fclose(file);
    
    if (bytes_written != size) {
        set_error_code(103);  // ERR_IO_ERROR
        return 0;
    }
    
    return 1;  // Success
}

/**
 * get_file_info - Get file metadata
 * @param path: File path
 * @return: FileInfo string (format: "size:modified:is_dir:readable:writable"), or NULL on error
 *
 * Error codes:
 *   ERR_FILE_NOT_FOUND (101): File doesn't exist
 *
 * MLP Usage:
 *   info := get_file_info("test.txt")
 *   if info != null { print info }
 *
 * Returns string format: "1024:1700000000:0:1:1"
 *   - size (bytes)
 *   - modified_time (Unix timestamp)
 *   - is_directory (0/1)
 *   - is_readable (0/1)
 *   - is_writable (0/1)
 */
char* get_file_info(const char* path) {
    if (!path) {
        set_error_code(101);  // ERR_FILE_NOT_FOUND
        return NULL;
    }

    struct stat st;
    if (stat(path, &st) != 0) {
        set_error_code(101);  // ERR_FILE_NOT_FOUND
        return NULL;
    }
    
    // Format: "size:modified:is_dir:readable:writable"
    char* info = mlp_malloc(256);
    snprintf(info, 256, "%ld:%ld:%d:%d:%d",
        (long)st.st_size,
        (long)st.st_mtime,
        S_ISDIR(st.st_mode) ? 1 : 0,
        (st.st_mode & S_IRUSR) ? 1 : 0,
        (st.st_mode & S_IWUSR) ? 1 : 0
    );
    
    return info;
}

/**
 * copy_file - Copy file from source to destination
 * @param source: Source file path
 * @param dest: Destination file path
 * @return: 1 on success, 0 on failure
 *
 * Error codes:
 *   ERR_FILE_NOT_FOUND (101): Source file doesn't exist
 *   ERR_PERMISSION_DENIED (102): Cannot read source or write destination
 *   ERR_IO_ERROR (103): Copy operation failed
 *
 * MLP Usage:
 *   if copy_file("source.txt", "dest.txt") {
 *       print "Copy successful"
 *   }
 */
int copy_file(const char* source, const char* dest) {
    if (!source || !dest) {
        set_error_code(101);  // ERR_FILE_NOT_FOUND
        return 0;
    }

    FILE* src = fopen(source, "rb");
    if (!src) {
        set_error_code(101);  // ERR_FILE_NOT_FOUND
        return 0;
    }
    
    FILE* dst = fopen(dest, "wb");
    if (!dst) {
        fclose(src);
        set_error_code(102);  // ERR_PERMISSION_DENIED
        return 0;
    }
    
    // Copy in 4KB chunks
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            set_error_code(103);  // ERR_IO_ERROR
            return 0;
        }
    }
    
    fclose(src);
    fclose(dst);
    return 1;  // Success
}

// ==================== PHASE 5.3: DIRECTORY OPERATIONS (6 functions) ====================

/**
 * list_directory - List all files in a directory
 * @param path: Directory path
 * @param count: Output parameter for number of entries (pass pointer)
 * @return: Array of filenames (char**), or NULL on error
 *
 * Error codes:
 *   ERR_FILE_NOT_FOUND (101): Directory doesn't exist
 *   ERR_PERMISSION_DENIED (102): No read permission
 *
 * MLP Usage:
 *   files := list_directory("src")
 *   for file in files { print file }
 *
 * Note: Skips "." and ".." entries. Caller must free array and strings.
 */
char** list_directory(const char* path, int* count) {
    if (!path || !count) {
        set_error_code(101);
        if (count) *count = 0;
        return NULL;
    }

    DIR* dir = opendir(path);
    if (!dir) {
        set_error_code(101);  // ERR_FILE_NOT_FOUND
        *count = 0;
        return NULL;
    }
    
    // Count entries first (excluding . and ..)
    int entry_count = 0;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            entry_count++;
        }
    }
    
    if (entry_count == 0) {
        closedir(dir);
        *count = 0;
        return NULL;  // Empty directory
    }
    
    // Allocate array for filenames
    char** files = mlp_malloc(sizeof(char*) * entry_count);
    if (!files) {
        closedir(dir);
        *count = 0;
        return NULL;
    }
    
    // Read entries again
    rewinddir(dir);
    int i = 0;
    while ((entry = readdir(dir)) != NULL && i < entry_count) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            size_t name_len = strlen(entry->d_name);
            files[i] = mlp_malloc(name_len + 1);
            if (files[i]) {
                strcpy(files[i], entry->d_name);
                i++;
            }
        }
    }
    
    closedir(dir);
    *count = i;
    return files;
}

/**
 * create_directory - Create a new directory
 * @param path: Directory path to create
 * @return: 1 on success, 0 on failure
 *
 * Error codes:
 *   ERR_FILE_EXISTS (108): Directory already exists
 *   ERR_PERMISSION_DENIED (102): No permission to create
 *
 * MLP Usage:
 *   if create_directory("build") {
 *       print "Directory created"
 *   }
 */
int create_directory(const char* path) {
    if (!path) {
        set_error_code(102);
        return 0;
    }

    if (mkdir(path, 0755) != 0) {
        if (errno == EEXIST) {
            set_error_code(108);  // ERR_FILE_EXISTS
        } else {
            set_error_code(102);  // ERR_PERMISSION_DENIED
        }
        return 0;
    }
    return 1;
}

/**
 * remove_directory - Remove an empty directory
 * @param path: Directory path to remove
 * @return: 1 on success, 0 on failure
 *
 * Error codes:
 *   ERR_DIRECTORY_NOT_EMPTY (107): Directory contains files
 *   ERR_PERMISSION_DENIED (102): No permission to remove
 *
 * MLP Usage:
 *   if remove_directory("temp") {
 *       print "Directory removed"
 *   }
 */
int remove_directory(const char* path) {
    if (!path) {
        set_error_code(102);
        return 0;
    }

    if (rmdir(path) != 0) {
        if (errno == ENOTEMPTY || errno == EEXIST) {
            set_error_code(107);  // ERR_DIRECTORY_NOT_EMPTY
        } else {
            set_error_code(102);  // ERR_PERMISSION_DENIED
        }
        return 0;
    }
    return 1;
}

/**
 * directory_exists - Check if directory exists
 * @param path: Directory path to check
 * @return: 1 if exists and is directory, 0 otherwise
 *
 * MLP Usage:
 *   if directory_exists("src") {
 *       print "Source directory found"
 *   }
 */
int directory_exists(const char* path) {
    if (!path) return 0;

    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;  // Doesn't exist
    }
    
    return S_ISDIR(st.st_mode) ? 1 : 0;
}

/**
 * get_current_dir - Get current working directory
 * @return: Current directory path string, or NULL on error
 *
 * Error codes:
 *   ERR_IO_ERROR (103): Failed to get current directory
 *
 * MLP Usage:
 *   cwd := get_current_dir()
 *   print "Working in: " + cwd
 */
char* get_current_dir(void) {
    char* buffer = mlp_malloc(4096);  // PATH_MAX
    if (!buffer) {
        set_error_code(104);  // ERR_OUT_OF_MEMORY
        return NULL;
    }

    if (getcwd(buffer, 4096) == NULL) {
        mlp_free(buffer);
        set_error_code(103);  // ERR_IO_ERROR
        return NULL;
    }
    
    return buffer;
}

/**
 * change_directory - Change current working directory
 * @param path: New directory path
 * @return: 1 on success, 0 on failure
 *
 * Error codes:
 *   ERR_FILE_NOT_FOUND (101): Directory doesn't exist
 *   ERR_PERMISSION_DENIED (102): No permission to access
 *
 * MLP Usage:
 *   if change_directory("src") {
 *       print "Changed to src directory"
 *   }
 */
int change_directory(const char* path) {
    if (!path) {
        set_error_code(101);
        return 0;
    }

    if (chdir(path) != 0) {
        if (errno == ENOENT) {
            set_error_code(101);  // ERR_FILE_NOT_FOUND
        } else {
            set_error_code(102);  // ERR_PERMISSION_DENIED
        }
        return 0;
    }
    
    return 1;
}

// ==================== PHASE 5.3: PROCESS CONTROL (4 functions) ====================

/**
 * execute_command - Execute shell command and return exit code
 * @param cmd: Shell command to execute
 * @return: Exit code of command, or -1 on error
 *
 * Error codes:
 *   ERR_COMMAND_FAILED (106): Command execution failed
 *
 * MLP Usage:
 *   exit_code := execute_command("ls -la")
 *   if exit_code == 0 { print "Success" }
 *
 * Security Warning: Never pass user input directly to this function!
 * Validate and sanitize all inputs to prevent command injection.
 */
int execute_command(const char* cmd) {
    if (!cmd) {
        set_error_code(106);  // ERR_COMMAND_FAILED
        return -1;
    }

    int exit_code = system(cmd);
    if (exit_code == -1) {
        set_error_code(106);  // ERR_COMMAND_FAILED
        return -1;
    }
    
    // Extract actual exit code from system() return value
    #ifdef _WIN32
        return exit_code;
    #else
        if (WIFEXITED(exit_code)) {
            return WEXITSTATUS(exit_code);
        }
        return -1;
    #endif
}

/**
 * get_command_output - Execute command and capture output
 * @param cmd: Shell command to execute
 * @return: Command output as string, or NULL on error
 *
 * Error codes:
 *   ERR_COMMAND_FAILED (106): Command execution failed
 *   ERR_OUT_OF_MEMORY (104): Failed to allocate memory for output
 *
 * MLP Usage:
 *   output := get_command_output("ls src")
 *   print output
 *
 * Security Warning: Never pass user input directly to this function!
 */
char* get_command_output(const char* cmd) {
    if (!cmd) {
        set_error_code(106);  // ERR_COMMAND_FAILED
        return NULL;
    }

    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        set_error_code(106);  // ERR_COMMAND_FAILED
        return NULL;
    }
    
    // Read output in chunks
    size_t buffer_size = 1024;
    size_t output_size = 0;
    char* output = mlp_malloc(buffer_size);
    if (!output) {
        pclose(pipe);
        set_error_code(104);  // ERR_OUT_OF_MEMORY
        return NULL;
    }
    
    output[0] = '\0';
    char chunk[256];
    while (fgets(chunk, sizeof(chunk), pipe) != NULL) {
        size_t chunk_len = strlen(chunk);
        
        // Resize buffer if needed
        while (output_size + chunk_len + 1 > buffer_size) {
            buffer_size *= 2;
            char* new_output = mlp_realloc(output, buffer_size);
            if (!new_output) {
                mlp_free(output);
                pclose(pipe);
                set_error_code(104);  // ERR_OUT_OF_MEMORY
                return NULL;
            }
            output = new_output;
        }
        
        strcat(output, chunk);
        output_size += chunk_len;
    }
    
    pclose(pipe);
    return output;
}

/**
 * get_process_id - Get current process ID
 * @return: Current process ID (PID)
 *
 * MLP Usage:
 *   pid := get_process_id()
 *   print "My PID: " + string(pid)
 */
int get_process_id(void) {
    return getpid();
}

/**
 * get_parent_process_id - Get parent process ID
 * @return: Parent process ID (PPID)
 *
 * MLP Usage:
 *   ppid := get_parent_process_id()
 *   print "Parent PID: " + string(ppid)
 */
int get_parent_process_id(void) {
    return getppid();
}

// ==================== PHASE 5.3: TIME & DATE UTILITIES (4 functions) ====================

/**
 * format_timestamp - Format Unix timestamp to string
 * @param timestamp: Unix timestamp (seconds since epoch)
 * @param format: strftime format string (e.g., "%Y-%m-%d %H:%M:%S")
 * @return: Formatted date/time string, or NULL on error
 *
 * Error codes:
 *   ERR_INVALID_FORMAT (105): Invalid format string
 *   ERR_OUT_OF_MEMORY (104): Failed to allocate memory
 *
 * MLP Usage:
 *   now := current_timestamp()
 *   formatted := format_timestamp(now, "%Y-%m-%d %H:%M:%S")
 *   print formatted  // "2025-11-21 14:30:00"
 *
 * Common format specifiers:
 *   %Y - Year (4 digits)
 *   %m - Month (01-12)
 *   %d - Day (01-31)
 *   %H - Hour (00-23)
 *   %M - Minute (00-59)
 *   %S - Second (00-59)
 */
char* format_timestamp(long timestamp, const char* format) {
    if (!format) {
        set_error_code(105);  // ERR_INVALID_FORMAT
        return NULL;
    }

    char* buffer = mlp_malloc(256);
    if (!buffer) {
        set_error_code(104);  // ERR_OUT_OF_MEMORY
        return NULL;
    }
    
    time_t t = (time_t)timestamp;
    struct tm* tm_info = localtime(&t);
    
    if (strftime(buffer, 256, format, tm_info) == 0) {
        mlp_free(buffer);
        set_error_code(105);  // ERR_INVALID_FORMAT
        return NULL;
    }
    
    return buffer;
}

/**
 * parse_timestamp - Parse date string to Unix timestamp
 * @param date_string: Date string in ISO 8601 format
 * @return: Unix timestamp, or -1 on error
 *
 * Error codes:
 *   ERR_INVALID_FORMAT (105): Invalid date string format
 *
 * MLP Usage:
 *   timestamp := parse_timestamp("2025-11-21 14:30:00")
 *   print timestamp
 *
 * Supported formats:
 *   "YYYY-MM-DD HH:MM:SS" - Full date and time
 *   "YYYY-MM-DD" - Date only (time defaults to 00:00:00)
 */
long parse_timestamp(const char* date_string) {
    if (!date_string) {
        set_error_code(105);  // ERR_INVALID_FORMAT
        return -1;
    }

    struct tm tm = {0};
    
    // Try ISO 8601 format: YYYY-MM-DD HH:MM:SS
    if (strptime(date_string, "%Y-%m-%d %H:%M:%S", &tm) != NULL) {
        return (long)mktime(&tm);
    }
    
    // Try date only: YYYY-MM-DD
    if (strptime(date_string, "%Y-%m-%d", &tm) != NULL) {
        return (long)mktime(&tm);
    }
    
    set_error_code(105);  // ERR_INVALID_FORMAT
    return -1;
}

/**
 * get_milliseconds - Get high-precision time in milliseconds
 * @return: Current time in milliseconds since boot
 *
 * MLP Usage:
 *   start := get_milliseconds()
 *   // ... do work ...
 *   elapsed := get_milliseconds() - start
 *   print "Elapsed: " + string(elapsed) + "ms"
 *
 * Note: Uses CLOCK_MONOTONIC for accurate timing
 */
long long get_milliseconds(void) {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

/**
 * get_time_string - Get current time as formatted string
 * @return: Current time in format "YYYY-MM-DD HH:MM:SS"
 *
 * MLP Usage:
 *   time_str := get_time_string()
 *   print "Current time: " + time_str
 */
char* get_time_string(void) {
    time_t now = time(NULL);
    return format_timestamp((long)now, "%Y-%m-%d %H:%M:%S");
}

// ==================== PHASE 5.3: PATH UTILITIES (4 functions) ====================

/**
 * join_path - Join path components with forward slash
 * @param parts: Array of path components
 * @param count: Number of components
 * @return: Joined path string
 *
 * MLP Usage:
 *   path := join_path(["home", "user", "file.txt"], 3)
 *   print path  // "home/user/file.txt"
 */
char* join_path(char** parts, int count) {
    if (!parts || count == 0) {
        char* empty = mlp_malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }
    
    // Calculate total length
    size_t total_len = 0;
    for (int i = 0; i < count; i++) {
        if (parts[i]) {
            total_len += strlen(parts[i]);
        }
        if (i < count - 1) total_len++;  // For '/' separator
    }
    
    char* result = mlp_malloc(total_len + 1);
    if (!result) return NULL;
    
    result[0] = '\0';
    for (int i = 0; i < count; i++) {
        if (parts[i]) {
            strcat(result, parts[i]);
        }
        if (i < count - 1) strcat(result, "/");
    }
    
    return result;
}

/**
 * get_file_extension - Extract file extension from path
 * @param path: File path
 * @return: Extension string (without dot), or empty string if no extension
 *
 * MLP Usage:
 *   ext := get_file_extension("test.mlp")
 *   print ext  // "mlp"
 */
char* get_file_extension(const char* path) {
    if (!path) {
        char* empty = mlp_malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }

    const char* dot = strrchr(path, '.');
    const char* slash = strrchr(path, '/');
    
    // No extension or dot is in directory name
    if (!dot || (slash && dot < slash)) {
        char* empty = mlp_malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }
    
    // Copy extension (without dot)
    size_t ext_len = strlen(dot + 1);
    char* ext = mlp_malloc(ext_len + 1);
    if (ext) {
        strcpy(ext, dot + 1);
    }
    return ext;
}

/**
 * get_file_name - Extract filename from path
 * @param path: File path
 * @return: Filename string
 *
 * MLP Usage:
 *   name := get_file_name("/path/to/file.txt")
 *   print name  // "file.txt"
 */
char* get_file_name(const char* path) {
    if (!path) {
        char* empty = mlp_malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }

    const char* slash = strrchr(path, '/');
    const char* filename = slash ? slash + 1 : path;
    
    size_t name_len = strlen(filename);
    char* result = mlp_malloc(name_len + 1);
    if (result) {
        strcpy(result, filename);
    }
    return result;
}

/**
 * get_directory - Extract directory path from file path
 * @param path: File path
 * @return: Directory path, or "." if no directory
 *
 * MLP Usage:
 *   dir := get_directory("/path/to/file.txt")
 *   print dir  // "/path/to"
 */
char* get_directory(const char* path) {
    if (!path) {
        char* dot = mlp_malloc(2);
        if (dot) strcpy(dot, ".");
        return dot;
    }

    const char* slash = strrchr(path, '/');
    
    if (!slash) {
        // No directory, return "."
        char* dot = mlp_malloc(2);
        if (dot) strcpy(dot, ".");
        return dot;
    }
    
    size_t dir_len = slash - path;
    if (dir_len == 0) {
        // Root directory "/"
        char* root = mlp_malloc(2);
        if (root) strcpy(root, "/");
        return root;
    }
    
    char* result = mlp_malloc(dir_len + 1);
    if (result) {
        strncpy(result, path, dir_len);
        result[dir_len] = '\0';
    }
    return result;
}

// ============================================================================
// Phase 6: Dynamic List Implementation
// ============================================================================

// List struct - Generic dynamic array
typedef struct {
    void* data;           // Generic data pointer
    int64_t length;       // Current number of elements
    int64_t capacity;     // Allocated capacity
    int64_t element_size; // Size of each element in bytes
} MLP_List;

// Create a new list with given element size
MLP_List* mlp_list_create(int64_t element_size) {
    if (element_size <= 0) {
        fprintf(stderr, "Error: list_create - element_size must be positive\n");
        return NULL;
    }
    
    MLP_List* list = (MLP_List*)mlp_malloc(sizeof(MLP_List));
    if (!list) {
        fprintf(stderr, "Error: list_create - failed to allocate list structure\n");
        return NULL;
    }
    
    list->element_size = element_size;
    list->length = 0;
    list->capacity = 8; // Initial capacity
    list->data = mlp_malloc(list->capacity * element_size);
    
    if (!list->data) {
        mlp_free(list);
        fprintf(stderr, "Error: list_create - failed to allocate data array\n");
        return NULL;
    }
    
    return list;
}

// Resize list capacity (internal helper)
static int mlp_list_resize(MLP_List* list, int64_t new_capacity) {
    if (!list || new_capacity < list->length) {
        return 0;
    }
    
    void* new_data = mlp_realloc(list->data, new_capacity * list->element_size);
    if (!new_data) {
        fprintf(stderr, "Error: list_resize - realloc failed\n");
        return 0;
    }
    
    list->data = new_data;
    list->capacity = new_capacity;
    return 1;
}

// Add element to end of list
void mlp_list_add(MLP_List* list, void* element) {
    if (!list || !element) {
        fprintf(stderr, "Error: list_add - null pointer\n");
        return;
    }
    
    // Grow if needed
    if (list->length >= list->capacity) {
        int64_t new_capacity = list->capacity * 2;
        if (!mlp_list_resize(list, new_capacity)) {
            fprintf(stderr, "Error: list_add - resize failed\n");
            return;
        }
    }
    
    // Copy element to end
    char* dest = (char*)list->data + (list->length * list->element_size);
    memcpy(dest, element, list->element_size);
    list->length++;
}

// Get element at index (returns pointer to element)
void* mlp_list_get(MLP_List* list, int64_t index) {
    if (!list) {
        fprintf(stderr, "Error: list_get - null list\n");
        return NULL;
    }
    
    if (index < 0 || index >= list->length) {
        fprintf(stderr, "Error: list_get - index %ld out of bounds (0-%ld)\n", 
                index, list->length - 1);
        return NULL;
    }
    
    char* ptr = (char*)list->data + (index * list->element_size);
    return ptr;
}

// Set element at index
void mlp_list_set(MLP_List* list, int64_t index, void* element) {
    if (!list || !element) {
        fprintf(stderr, "Error: list_set - null pointer\n");
        return;
    }
    
    if (index < 0 || index >= list->length) {
        fprintf(stderr, "Error: list_set - index %ld out of bounds (0-%ld)\n", 
                index, list->length - 1);
        return;
    }
    
    char* dest = (char*)list->data + (index * list->element_size);
    memcpy(dest, element, list->element_size);
}

// Remove element at index (shifts remaining elements)
void mlp_list_remove(MLP_List* list, int64_t index) {
    if (!list) {
        fprintf(stderr, "Error: list_remove - null list\n");
        return;
    }
    
    if (index < 0 || index >= list->length) {
        fprintf(stderr, "Error: list_remove - index %ld out of bounds (0-%ld)\n", 
                index, list->length - 1);
        return;
    }
    
    // Shift elements after index
    if (index < list->length - 1) {
        char* dest = (char*)list->data + (index * list->element_size);
        char* src = dest + list->element_size;
        size_t bytes_to_move = (list->length - index - 1) * list->element_size;
        memmove(dest, src, bytes_to_move);
    }
    
    list->length--;
    
    // Shrink if needed (when usage drops below 25%)
    if (list->capacity > 8 && list->length < list->capacity / 4) {
        mlp_list_resize(list, list->capacity / 2);
    }
}

// Insert element at index (shifts existing elements right)
void mlp_list_insert(MLP_List* list, int64_t index, void* element) {
    if (!list || !element) {
        fprintf(stderr, "Error: list_insert - null pointer\n");
        return;
    }
    
    if (index < 0 || index > list->length) {
        fprintf(stderr, "Error: list_insert - index %ld out of bounds (0-%ld)\n", 
                index, list->length);
        return;
    }
    
    // Grow if needed
    if (list->length >= list->capacity) {
        if (!mlp_list_resize(list, list->capacity * 2)) {
            fprintf(stderr, "Error: list_insert - resize failed\n");
            return;
        }
    }
    
    // Shift elements from index onwards to the right
    if (index < list->length) {
        char* dest = (char*)list->data + ((index + 1) * list->element_size);
        char* src = (char*)list->data + (index * list->element_size);
        size_t bytes_to_move = (list->length - index) * list->element_size;
        memmove(dest, src, bytes_to_move);
    }
    
    // Insert new element
    char* insert_pos = (char*)list->data + (index * list->element_size);
    memcpy(insert_pos, element, list->element_size);
    list->length++;
}

// Clear all elements (keeps capacity)
void mlp_list_clear(MLP_List* list) {
    if (!list) {
        fprintf(stderr, "Error: list_clear - null list\n");
        return;
    }
    
    list->length = 0;
}

// Get current length
int64_t mlp_list_length(MLP_List* list) {
    if (!list) return 0;
    return list->length;
}

// Get current capacity
int64_t mlp_list_capacity(MLP_List* list) {
    if (!list) return 0;
    return list->capacity;
}

// Phase 11.2: Build a List<string> snapshot of command-line arguments
MLP_List* mlp_get_args_list(void) {
    // Each element stores a pointer (char*)
    MLP_List* list = mlp_list_create(sizeof(char*));
    if (!list) {
        return NULL;
    }

    for (int i = 0; i < mlp_argc; i++) {
        char* arg_value = mlp_get_argv(i);
        if (!arg_value) {
            arg_value = strdup("");
        }
        mlp_list_add(list, &arg_value);
    }

    return list;
}

// Check if list is empty
int64_t mlp_list_is_empty(MLP_List* list) {
    if (!list) return 1;
    return (list->length == 0) ? 1 : 0;
}

// Free list and its data
void mlp_list_free(MLP_List* list) {
    if (!list) return;
    
    if (list->data) {
        mlp_free(list->data);
    }
    mlp_free(list);
}

// Get pointer to underlying array (for direct access)
void* mlp_list_data(MLP_List* list) {
    if (!list) return NULL;
    return list->data;
}

// ===== Phase 6.1: C Helper Functions for MLP Runtime Layer =====
// These are low-level primitives that MLP runtime can use
// Eventually these will be replaced with inline assembly or syscalls

// Memory allocation wrapper (for MLP code)
void* c_malloc(int64_t size) {
    return malloc((size_t)size);
}

// Memory free wrapper (for MLP code)
void c_free(void* ptr) {
    free(ptr);
}

// Get string length (for MLP code)
int64_t c_string_length(const char* str) {
    if (!str) return 0;
    return (int64_t)strlen(str);
}

// Get character at index (for MLP code)
int64_t c_char_at(const char* str, int64_t index) {
    if (!str) return 0;
    int64_t len = strlen(str);
    if (index < 0 || index >= len) return 0;
    return (int64_t)(unsigned char)str[index];
}

// Set character at index (for MLP code)
void c_set_char_at(char* str, int64_t index, int64_t ch) {
    if (!str) return;
    str[index] = (char)ch;
}

// Memory copy (for MLP code)
void c_memcpy(void* dest, const void* src, int64_t n) {
    memcpy(dest, src, (size_t)n);
}

// Memory set (for MLP code)
void c_memset(void* dest, int64_t value, int64_t n) {
    memset(dest, (int)value, (size_t)n);
}

