# MLP Dili - Özellik Durum ve TODO Listesi

## 📌 Self-Hosting İçin GEREKLİ Minimum Özellikler

Self-hosting (MLP ile MLP derleyicisini yazmak) için **TÜM özelliklerin** tamamlanmasına gerek **YOK**. Sadece şunlar kritik:

### ✅ Zaten Mevcut ve Yeterli:
- ✅ Temel veri tipleri (int, string, bool)
- ✅ Değişkenler ve const
- ✅ Fonksiyonlar (parametreler, return)
- ✅ Kontrol yapıları (if/else, while, for, switch)
- ✅ String işlemleri (concat, compare, substring, split, etc.)
- ✅ Dosya okuma/yazma
- ✅ struct (veri yapıları için)
- ✅ enum (token tipleri için)

### ⚠️ Self-Hosting İçin EKSİK Kritik Özellikler:

1. **🔴 ARRAY/DİZİ DESTEĞİ** (EN ÖNEMLİ!)
   - Token listesi için: `Token[] tokens`
   - AST node listesi için: `ASTNode[] nodes`
   - String array için: `string[] keywords`
   - **Öncelik: YÜKSEK** - Bu olmadan derleyici yazılamaz

2. **🟡 Dynamic Memory/Array Büyütme**
   - malloc/realloc zaten var ✅
   - Array resize fonksiyonları gerekli
   - **Öncelik: ORTA** - Workaround'lar mümkün

3. **🟡 Input Fonksiyonu** (İsteğe bağlı)
   - Komut satırı argümanları için
   - **Öncelik: DÜŞÜK** - Dosyadan okuma ile halledilebilir

4. **🟢 Module/Import Sistemi** (Nice-to-have)
   - Lexer, Parser, Generator'ı ayrı dosyalarda tutmak için
   - **Öncelik: DÜŞÜK** - Tek dosyada da yazılabilir

---

## ✅ TAMAMLANMIŞ ÖZELLİKLER

### Temel Dil Özellikleri
- [x] `int`, `string`, `bool` tipleri
- [x] `char` literals ('A', '\n', '\t')
- [x] `null` keyword
- [x] Değişken tanımlama ve atama
- [x] Global ve local değişkenler
- [x] **const değişkenler** (Phase 5.6) ⭐ YENİ
- [x] Compound assignment (+=, -=, *=, /=)

### Operatörler
**Aritmetik:**
- [x] `+` `-` `*` `/` (dört işlem)
- [x] `%` (modulo)
- [x] `+=` `-=` `*=` `/=`

**Karşılaştırma:**
- [x] `==` `!=` `<` `>` `<=` `>=`

**Mantıksal:**
- [x] `and` `or` `not`

**Bitwise:**
- [x] `&` `|` `^` `~` `<<` `>>`

### Kontrol Yapıları
- [x] `if` / `else if` / `else`
- [x] Ternary operator (`x > 0 ? "pos" : "neg"`)
- [x] `while` döngüsü
- [x] `for` döngüsü
- [x] İçiçe döngüler
- [x] `break`, `continue`, `stop`
- [x] `goto` ve label'lar
- [x] `switch` / `case` / `default`

### Fonksiyonlar
- [x] Fonksiyon tanımlama
- [x] Parametreler ve return değerleri
- [x] Recursive fonksiyonlar
- [x] Fonksiyon çağrıları

### Veri Yapıları
- [x] `enum` (numaralandırma)
- [x] `struct` (yapı tanımlama)
- [x] Struct field erişimi (`.` operatörü)

### String İşlemleri (15 fonksiyon)
- [x] `string_length()`, `string_concat()`, `string_compare()`
- [x] `string_at()`, `string_substring()`, `string_equals()`
- [x] `string_split()`, `string_join()`, `string_replace()`
- [x] `string_trim()`, `string_upper()`, `string_lower()`
- [x] `string_find()`, `string_starts_with()`, `string_ends_with()`

### Dosya İşlemleri (8 fonksiyon)
- [x] `open_file()`, `read_file()`, `write_file()`, `close_file()`
- [x] `read_binary()`, `write_binary()`
- [x] `get_file_info()`, `copy_file()`

### Dizin İşlemleri (6 fonksiyon)
- [x] `list_directory()`, `create_directory()`, `remove_directory()`
- [x] `directory_exists()`, `get_current_dir()`, `change_directory()`

### Sistem/Process (5 fonksiyon)
- [x] `execute_command()`, `get_command_output()`
- [x] `get_process_id()`, `get_parent_process_id()`
- [x] `get_env()`

### Zaman İşlemleri (6 fonksiyon)
- [x] `current_timestamp()`, `sleep_ms()`
- [x] `format_timestamp()`, `parse_timestamp()`
- [x] `get_milliseconds()`, `get_time_string()`

### Path İşlemleri (4 fonksiyon)
- [x] `join_path()`, `get_file_extension()`
- [x] `get_file_name()`, `get_directory()`

### Bellek Yönetimi (6 fonksiyon)
- [x] `malloc()`, `free()`, `realloc()`, `calloc()`
- [x] `get_allocated_bytes()`, `check_memory_leaks()`

### I/O ve Hata Yönetimi
- [x] `print` (ekrana yazdırma)
- [x] `panic()`, `assert()`
- [x] `get_error_code()`, `set_error_code()`, `exit_with_code()`

---

## 🔴 KRİTİK EKSİKLER (Self-Hosting İçin Gerekli)

### 1. Array/Dizi Desteği ⚠️ EN ÖNEMLİ
**Öncelik: YÜKSEK - Self-hosting için kritik!**

```mlp
-- Statik diziler
int numbers[10]
string keywords[5] = ["if", "else", "while", "for", "func"]

-- Array erişimi
int x = numbers[0]
numbers[5] = 42

-- Array operasyonları
int length = array_length(numbers)
array_push(numbers, 100)
int value = array_pop(numbers)
array_insert(numbers, 2, 50)
array_remove(numbers, 3)
```

**Neden gerekli:**
- Token listesi tutmak için: `Token[] tokens`
- AST node listesi için: `ASTNode[] nodes`
- Symbol table için: `Degisken[] kapsam_haritasi`
- String array: `string[] keywords = ["if", "else", "while"]`

**Implementasyon:**
- Lexer: `[` `]` token'ları ekle
- Parser: Array tanımlama ve erişim syntax'ı
- Generator: Stack'te array allocation
- Runtime: Array helper fonksiyonları

---

## 🟡 ORTA ÖNCELİKLİ EKSİKLER

### 2. Input/Stdin Okuma
**Öncelik: ORTA - Alternatif: dosyadan okuma**

```mlp
string input = read_input()
int number = read_int()
string line = read_line()
```

**Kullanım:**
- Komut satırı argümanları
- REPL (Read-Eval-Print Loop)
- Interactive program'lar

**Workaround:** Dosyadan okuma zaten var ✅

### 3. Type Casting/Dönüşüm
**Öncelik: ORTA**

```mlp
int x = 65
char ch = (char)x  -- 'A'
string s = int_to_string(x)  -- Zaten var ✅
int y = string_to_int("123")
```

**Durum:**
- `int_to_string()` zaten var ✅
- Eksik: `string_to_int()`, explicit casting

### 4. Default Parametreler
**Öncelik: DÜŞÜK - Claude #2 başlamış**

```mlp
func greet(string name = "World") -> string {
    return "Hello, " + name
}

print greet()           -- "Hello, World"
print greet("Alice")    -- "Hello, Alice"
```

---

## 🟢 DÜŞÜK ÖNCELİKLİ EKSİKLER

### 5. typeof Operatörü
**Öncelik: DÜŞÜK - Claude #2 başlamış**

```mlp
int x = 10
string type_name = typeof(x)  -- "int"
```

### 6. Range Syntax
**Öncelik: DÜŞÜK - Claude #2 başlamış**

```mlp
for i in range(0, 10) {
    print i
}
```

### 7. Module/Import Sistemi
**Öncelik: DÜŞÜK**

```mlp
import lexer
import parser
import generator

-- veya
import "lexer.mlp"
import "parser.mlp"
```

**Workaround:** Tüm kodu tek dosyada yazabiliriz

### 8. String Interpolation
**Öncelik: DÜŞÜK**

```mlp
string name = "Alice"
int age = 25
print "Name: {name}, Age: {age}"
```

**Workaround:** String concat zaten var ✅

### 9. Multi-line String
**Öncelik: DÜŞÜK**

```mlp
string text = """
Bu bir
çok satırlı
string'tir
"""
```

---

## ⛔ SELF-HOSTING İÇİN GEREKSİZ (Şimdilik Yapılmayabilir)

### OOP Özellikleri
- [ ] Class tanımlama
- [ ] Inheritance (kalıtım)
- [ ] Method'lar
- [ ] Constructor/Destructor
- [ ] Public/Private modifiers
- [ ] Interface/Trait

**Not:** Struct + fonksiyonlarla halledilebilir

### İleri Seviye Özellikler
- [ ] Generic types/templates
- [ ] Lambda/anonim fonksiyonlar
- [ ] First-class fonksiyonlar
- [ ] Pointer'lar (explicit)
- [ ] Reference types (`&x`)
- [ ] Optional types (`int?`)
- [ ] Pattern matching
- [ ] Destructuring
- [ ] Async/await
- [ ] Coroutines
- [ ] Macro sistemi

**Not:** Bunlar dil maturity için gerekli ama self-hosting için değil

### Diğer
- [ ] Dictionary/Map veri yapısı
- [ ] Tuple
- [ ] Union
- [ ] Dynamic array (resize)
- [ ] Variable arguments (varargs)
- [ ] Try/catch exception handling
- [ ] Custom exception türleri

---

## 📋 SELF-HOSTING ROADMAP

### Phase 6: Array Desteği (KRİTİK!)
**Hedef: Array tanımlama, erişim ve temel operasyonlar**

1. **Array Syntax**
   - Lexer: `[` `]` token'ları
   - Parser: `int arr[10]`, `arr[i]` syntax
   - Generator: Stack allocation

2. **Array Fonksiyonları**
   ```mlp
   int array_length(int[] arr)
   void array_push(int[] arr, int value)
   int array_pop(int[] arr)
   int array_get(int[] arr, int index)
   void array_set(int[] arr, int index, int value)
   ```

3. **Dynamic Arrays** (İsteğe bağlı)
   - `malloc`/`realloc` zaten var ✅
   - Array resize wrapper'ları

### Phase 7: Input ve Type Utilities
**Hedef: Kullanıcı input ve tip dönüşümleri**

1. `read_input()`, `read_line()`, `read_int()`
2. `string_to_int()`, `string_to_bool()`
3. Explicit casting (opsiyonel)

### Phase 8: Self-Hosting Denemeleri
**Hedef: MLP ile basit lexer yazmak**

1. Mini lexer (sadece token'ize et)
2. Mini parser (basit expression'lar)
3. Mini generator (basit ASM üret)

### Phase 9: Full Self-Hosting
**Hedef: Tam özellikli MLP derleyicisi MLP'de**

1. Tüm lexer fonksiyonları
2. Tüm parser fonksiyonları  
3. Tüm generator fonksiyonları
4. Bootstrap süreci

---

## 🎯 ÖZET

**Self-Hosting için:**
- ✅ Temel özellikler: %95 hazır
- 🔴 **Kritik eksik: ARRAY desteği**
- 🟡 Orta önemli: Input, type casting
- 🟢 Nice-to-have: Module, interpolation

**Array implementasyonu tamamlandıktan sonra self-hosting denemeye başlanabilir!**

Array olmadan derleyici yazmak neredeyse imkansız çünkü:
- Token listesi gerekli
- AST node listesi gerekli  
- Symbol table gerekli
- String keyword listesi gerekli

**Tahmini süre:**
- Array implementasyonu: 2-3 gün
- Input fonksiyonları: 1 gün
- İlk self-hosting denemesi: 1-2 hafta
- Tam self-hosting: 1-2 ay

---

## 📊 İstatistikler

**Toplam 100+ özellik:**
- ✅ Tamamlandı: ~75 özellik (%75)
- 🔴 Kritik eksik: 1 özellik (Array)
- 🟡 Orta önemli: 3 özellik
- 🟢 Düşük önemli: 5 özellik
- ⛔ Nice-to-have: 15+ özellik

**MLP şu anda prosedürel programlama için yeterli güçte bir dil!**
