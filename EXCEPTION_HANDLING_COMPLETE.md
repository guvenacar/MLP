# İstisna Yönetimi (Exception Handling) - Tamamlandı ✅

**Tamamlanma Tarihi:** 29 Kasım 2025  
**Versiyon:** 1.0  
**Durum:** Tam fonksiyonel, tüm testler geçiyor

---

## 📋 Özet

MELP'e tam özellikli istisna yönetimi sistemi eklendi. Sistem setjmp/longjmp tabanlı çalışıyor ve modern dillerdeki try-catch-finally yapısını destekliyor.

## ✅ Eklenen Özellikler

### 1. Sözdizimi
```mlp
try
    -- Hata fırlatılabilecek kod
    throw RuntimeError("Bir şeyler yanlış gitti!")
catch RuntimeError e
    -- RuntimeError yakalanır
    print("Runtime hatası yakalandı")
catch ValueError e
    -- ValueError yakalanır
    print("Değer hatası yakalandı")
catch e
    -- Tüm diğer hatalar (catch-all)
    print("Bilinmeyen hata")
finally
    -- Her durumda çalışır
    print("Temizlik işlemleri")
end try
```

### 2. Özellik Listesi
- ✅ **Çoklu catch blokları**: Tip bazlı istisna yakalama
- ✅ **Finally blokları**: Her durumda çalışan kod
- ✅ **Tip kontrolü**: RuntimeError, ValueError, NetworkError vb.
- ✅ **Catch-all**: Tüm istisnaları yakalayan genel catch
- ✅ **Yakalanmamış istisnalar**: Hata mesajı yazdırıp düzgün çıkış
- ✅ **İç içe try-catch**: Parent handler'a re-throw desteği
- ✅ **Custom mesajlar**: `throw Type("mesaj")` formatı

## 🏗️ Mimari

### Lexer Değişiklikleri (`bootstrap/lexer.c`)
**Eklenen tokenlar:**
- `TOKEN_TRY` (124)
- `TOKEN_CATCH` (125)
- `TOKEN_THROW` (126)
- `TOKEN_FINALLY` (127)

**Lokasyon:** Line 60-70 civarı, keyword tanımlamaları

### Parser Değişiklikleri (`bootstrap/parser.c`)

**Yeni yapılar:**
```c
// CatchBlock structure (lines ~234-239)
typedef struct {
    char* exception_type;    // "RuntimeError", "ValueError", NULL (catch-all)
    char* exception_var;     // İstisna değişken adı
    Statement** body;
    int body_count;
} CatchBlock;

// Try-catch statement (STMT_TRY_CATCH)
struct {
    Statement** try_body;
    int try_count;
    CatchBlock* catch_blocks;
    int catch_count;
    Statement** finally_body;
    int finally_count;
} try_catch;

// Throw statement (STMT_THROW)
struct {
    char* exception_type;    // "RuntimeError" vb.
    char* message;           // Hata mesajı
    int code;                // Hata kodu (opsiyonel)
} throw_stmt;
```

**Eklenen fonksiyonlar:**
- `parser_parse_try_catch()` - try...catch...finally...end try parsing
- `parser_parse_throw()` - throw Type("message") parsing

**Lokasyon:** Lines 1566-1650 civarı

### Runtime Değişiklikleri (`runtime/runtime.c`)

**Yeni yapılar:**
```c
typedef struct MlpException {
    char* type;
    char* message;
    int code;
} MlpException;

typedef struct ExceptionHandler {
    jmp_buf jump_buffer;
    MlpException exception;
    struct ExceptionHandler* prev;
} ExceptionHandler;

static ExceptionHandler* mlp_exception_stack = NULL;
```

**Eklenen fonksiyonlar:**
```c
// Handler yönetimi
ExceptionHandler* mlp_exception_push(void);                    // Line 539
void mlp_exception_pop(void);                                  // Line 553
void mlp_throw(const char* type, const char* msg, int code);   // Line 571

// İstisna bilgisi
const char* mlp_exception_type(void);                          // Line 619
const char* mlp_exception_message(void);                       // Line 624
int mlp_exception_code(void);                                  // Line 629

// Handler kontrolleri
int mlp_exception_has_handler(void);                           // Line 635
int mlp_exception_has_parent_handler(void);                    // Line 641
```

**Lokasyon:** Lines 530-650 civarı

### Codegen Değişiklikleri (`bootstrap/codegen.c`)

**Eklenen extern bildirimleri (lines ~345-354):**
```c
extern mlp_exception_push
extern mlp_exception_pop
extern mlp_throw
extern mlp_exception_type
extern mlp_exception_message
extern mlp_exception_code
extern mlp_exception_has_handler
extern mlp_exception_has_parent_handler
```

**Try-catch-finally codegen (lines 2058-2230):**
1. Handler push: `call mlp_exception_push`
2. setjmp kurulumu: `if (setjmp(...) != 0) goto catch`
3. Try body çalıştırma
4. Catch blokları: `strcmp` ile tip kontrolü
5. Finally bloğu: Her path'te çalışır
6. Re-throw/uncaught handling

**Kritik detaylar:**
- Parent handler kontrolü mlp_exception_pop ÖNCESİNDE yapılmalı
- Exception data pop sonrası freed olur, önce kaydedilmeli
- `mlp_exception_has_parent_handler()` API kullanarak kontrol yap
- Uncaught exception: mesajı yazdır, exit(1) yap

**Lokasyon:** Lines 2058-2230 civarı

## 🧪 Test Dosyaları

### test_exception_simple.mlp ✅
Temel try-catch testi:
```mlp
try
    numeric x = 10
    numeric y = 0
    throw y
catch e
    print(999)
end try
print(42)
```
**Çıktı:** `999\n42\n`

### test_exception_full.mlp ✅
Finally bloğu ile kapsamlı test:
```mlp
try
    print(1)
    throw RuntimeError("Test error")
catch RuntimeError e
    print(100)
finally
    print(42)
end try
print(888)
```
**Çıktı:** `1\n100\n42\n888\n`

### test_exception_multi_catch.mlp ✅
Çoklu catch blokları:
```mlp
try
    print(1)
    throw ValueError("Wrong value")
catch RuntimeError e
    print(100)
catch ValueError e
    print(200)
finally
    print(42)
end try
print(888)
```
**Çıktı:** `1\n200\n42\n888\n`

### test_exception_uncaught.mlp ✅
Yakalanmamış istisna:
```mlp
try
    print(1)
    throw NetworkError("Connection failed!")
catch RuntimeError e
    print(100)
end try
print(888)
```
**Çıktı:** `1\nUncaught exception: NetworkError - Connection failed!\n` (exit code 1)

## 🐛 Çözülen Sorunlar

### Sorun 1: Throw longjmp yapmıyordu
**Belirti:** Exception fırlatıldığında catch bloğuna gitmiyordu  
**Neden:** throw codegen syscall ile exit yapıyordu  
**Çözüm:** mlp_throw() fonksiyonu ile longjmp yapıldı

### Sorun 2: Uncaught exception infinite loop
**Belirti:** Yakalanmamış istisna sonsuz döngüye giriyordu  
**Neden:** No-catch durumunda re-throw yapılıyordu  
**Çözüm:** Parent handler kontrolü eklendi, yoksa exit

### Sorun 3: Exception data corruption
**Belirti:** Hata mesajları garbled/boş çıkıyordu  
**Neden:** mlp_exception_pop freed data, sonra erişmeye çalışıyorduk  
**Çözüm:** Pop öncesi exception data kaydedildi

### Sorun 4: Undefined reference to mlp_exception_stack
**Belirti:** Direct assembly memory access derlenmiyordu  
**Neden:** mlp_exception_stack static, linker'a görünmüyordu  
**Çözüm:** mlp_exception_has_parent_handler() API eklendi

## 🔧 Derleme ve Test

```bash
# Bootstrap compiler rebuild
cd melp
make clean && make

# Test compilation
./melp-bootstrap test_exception_full.mlp test_exception_full.s
nasm -f elf64 test_exception_full.s -o test_exception_full.o
ld test_exception_full.o runtime/runtime.o -o test_exception_full \
   -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2

# Run test
./test_exception_full
```

## 📝 Sonraki AI'ye Notlar

### Exception Handling Tamamlandı
- Tüm temel özellikler çalışıyor
- Bellek yönetimi düzgün (leak yok)
- Test coverage yeterli

### Geliştirilecek Özellikler (Opsiyonel)
1. **Exception variable usage**: Catch bloğunda `e.message` erişimi
2. **Stack trace**: Exception oluştuğunda call stack yazdırma
3. **Custom exception types**: Kullanıcı tanımlı exception yapıları
4. **Exception chaining**: Cause/previous exception desteği

### Bilinen Sınırlamalar
- Exception variable şu an kullanılamıyor (sadece tip kontrolü için)
- Stack trace yok
- Exception code parameter kullanılmıyor (gelecek için hazır)

### Mimari Notlar
- setjmp/longjmp kullanımı performanslı ve güvenilir
- Exception handler linked list yeterince hızlı
- Runtime'da static global yeterli (thread-safe değil ama şu an gerek yok)

## 🎯 Sıradaki Özellikler

1. **Modül Sistemi** - import/export/module
2. **Generics** - Tip parametreleri
3. **Pattern Matching** - match/case
4. **Operator Overloading** - Custom operatörler

---

**Durum:** Bu dokümantasyon tamamlandı. Exception handling MELP'in bir parçası artık! 🎉
