#ifndef GENERATOR_H
#define GENERATOR_H

#include <stddef.h>
#include "../../lexer.h"

// Generator durumu
typedef enum GenState {
    GEN_CREATED,        // Oluşturuldu, henüz başlamadı
    GEN_RUNNING,        // Çalışıyor
    GEN_SUSPENDED,      // Durduruldu (yield)
    GEN_FINISHED        // Tamamlandı
} GenState;

// Generator context (state machine)
typedef struct GenContext {
    GenState state;         // Mevcut durum
    int resume_point;       // Devam noktası (hangi yield'den)
    void* local_vars;       // Yerel değişkenler
    size_t locals_size;     // Yerel değişken boyutu
    void* current_value;    // Son yield edilen değer
} GenContext;

// Generator tanımı
typedef struct Generator {
    char* name;             // Generator adı
    void* body;             // Generator gövdesi
    GenContext* context;    // Execution context
    int yield_count;        // Kaç yield var
} Generator;

// Yield ifadesi
typedef struct Yield {
    void* value;            // Yield edilecek değer
    int yield_id;           // Yield noktası ID'si
} Yield;

// Generator çağrısı
typedef struct GenCall {
    Generator* generator;   // Çağrılacak generator
    int next_call;          // next() çağrısı mı?
} GenCall;

// Iterator protokolü (generator için)
typedef struct Iterator {
    Generator* generator;   // Bağlı generator
    int has_next;           // Daha eleman var mı?
    void* current;          // Mevcut değer
} Iterator;

// API fonksiyonları
Generator* generator_create(const char* name);
GenContext* gen_context_create(void);
Yield* yield_create(void* value, int yield_id);
GenCall* gen_call_create(Generator* gen, int next_call);
Iterator* iterator_create(Generator* gen);

void generator_free(Generator* gen);
void gen_context_free(GenContext* ctx);
void yield_free(Yield* yld);
void gen_call_free(GenCall* call);
void iterator_free(Iterator* iter);

// Generator operasyonları
void generator_start(Generator* gen);
void generator_resume(Generator* gen);
void generator_yield(Generator* gen, void* value);
int generator_has_next(Generator* gen);

#endif // GENERATOR_H
