# MLP Dili - Özellik Durum ve TODO Listesi

## 🏆 Self-Hosting Durumu

**MLP ARTIK TAM SELF-HOSTING'TİR!** ✅ (Tamamlandı: 22 Kasım 2024)

MLP derleyicisi kendi kendini derleyebilir durumda:
- **Lexer:** `self_host/lexer.mlp` - MLP ile yazılmış tokenizer
- **Parser:** `self_host/parser.mlp` - MLP ile yazılmış AST oluşturucu
- **Generator:** `self_host/generator.mlp` - MLP ile yazılmış assembly üretici
- **Ana Derleyici:** `self_host/mlpc.mlp` - Tam MLP derleyicisi MLP'de

### 🔀 Hibrit Mimari (İki Derleme Yolu)

MLP, iki farklı derleme yolu sunar:

#### Yol 1: MLP → Assembly (Direkt)
```
kaynak.mlp → self_host/mlpc.mlp → x86-64 Assembly
```
- MLP ile yazılmış derleyici bileşenleri kullanır
- `generator.mlp` ile direkt assembly üretimi
- Daha hızlı derleme süresi
- Saf MLP implementasyonu

#### Yol 2: MLP → C → Assembly (Varsayılan) ⭐
```
kaynak.mlp → c_compiler/mlpc → C Ara Kodu → x86-64 Assembly
```
- C tabanlı bootstrap derleyici kullanır
- Önce optimize C kodu üretir
- Daha iyi performans optimizasyonları
- **Production kullanımı için önerilen**

**Her iki yol da tamamen çalışır durumda!**

---

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

### ✅ Self-Hosting İçin HAZIR Özellikler:

1. **✅ ARRAY/DİZİ DESTEĞİ** - TAMAMLANDI! (22 Kasım 2024)
   - Array literal syntax: `numeric arr[] = [1, 2, 3]`
   - Array erişimi: `arr[0]`, `arr[i]`
   - Global ve local scope'ta çalışıyor
   - **Durum: TAMAMLANDI** ✅

2. **✅ LIST/DİNAMİK DİZİ DESTEĞİ** - TAMAMLANDI! (22 Kasım 2024)
   - List tanımlama: `numeric items()`, `numeric items(10)`
   - List literal: `numeric items() = (1,2,3)`
   - Metodlar: add, get, set, remove, insert, size, clear
   - **Durum: TAMAMLANDI** ✅

3. **✅ MAP/DICTIONARY DESTEĞİ** - TAMAMLANDI! (22 Kasım 2024)
   - Map tanımlama: `map[string:numeric] ages = map()`
   - Metodlar: set, get, has, remove, size, clear
   - String ve numeric key desteği
   - **Durum: TAMAMLANDI** ✅

### ⚠️ Self-Hosting İçin KALAN Kritik Özellikler:

1. **🟡 Input Fonksiyonu** (İsteğe bağlı)
   - Komut satırı argümanları için
   - **Öncelik: ORTA** - Dosyadan okuma ile halledilebilir

2. **🟢 Module/Import Sistemi** (Nice-to-have)
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
- [x] **Arrays** - Sabit boyutlu diziler `int arr[10]`, array literal `[1,2,3]` ⭐ YENİ (22 Kas 2024)
- [x] **Lists** - Dinamik listeler `numeric items()` + 7 metod (add/get/set/remove/insert/size/clear) ⭐ YENİ (22 Kas 2024)
- [x] **Maps/Dictionaries** - Hash map `map[K:V]` + 6 metod (set/get/has/remove/size/clear) ⭐ YENİ (22 Kas 2024)

### String İşlemleri (18+ fonksiyon) ✅ TAM
- [x] `string_length()`, `string_concat()`, `string_compare()`, `string_equals()`
- [x] `string_at()`, `string_substring()`, `string_char_at()`
- [x] `string_split()`, `string_join()`, `string_replace()`
- [x] `string_trim()`, `string_upper()`, `string_lower()`
- [x] `string_find()`, `string_starts_with()`, `string_ends_with()`
- [x] `string_to_int()`, `int_to_string()`, `string_to_float()`, `float_to_string()`
- [x] `string_to_bool()`, `bool_to_string()`, `char_to_string()`
- [x] `string_index_of()`, `string_last_index_of()`

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

## 🟢 DÜŞÜK ÖNCELİKLİ - TAMAMLANMIŞ ÖZELLĶLER (Devamı)

### 10. ✅ Phase 6.2: Generic Type System - TAMAMLANDI! (22 Kasım 2025)
**Öncelik: ORTA - TAMAMLANDI**

```mlp
-- Generic optional type
optional<numeric> maybe_num
optional<string> maybe_text = null
optional<numeric> x = optional<numeric>()
```

**Durum: TAMAMLANDI** ✅
- Parser: Full generic syntax support (`optional<T>`)
- Code Generator: Complete visitor implementation
- Tests: `test/test_optional_complete.mlp` ✅
- Documentation: Updated
- Status: Phase 6.2 100% Complete

**Implementation:**
- `visit_OptionalTanimlama`: Stack allocation for optional structs ✅
- `visit_OptionalHasValue`: Check has_value field ✅
- `visit_OptionalValue`: Extract value field ✅
- `visit_OptionalValueOr`: Conditional value extraction ✅

---

## 🟡 ORTA ÖNCELİKLİ EKSİKLER

### 2. ✅ Input/Stdin Okuma - TAMAMLANDI! (22 Kasım 2025)
**Öncelik: ORTA - TAMAMLANDI**

```mlp
string input = read_input()
int number = read_int()
string line = read_line()
```

**Kullanım:**
- Komut satırı argümanları
- REPL (Read-Eval-Print Loop)
- Interactive program'lar

**Durum: TAMAMLANDI** ✅
- `read_input()` - Stdin'den satır okur
- `read_line()` - Alias for read_input
- `read_int()` - Integer okur ve parse eder

### 3. ✅ Type Casting/Dönüşüm - TAMAMLANDI! (22 Kasım 2025)
**Öncelik: ORTA - TAMAMLANDI**

```mlp
string s = str(42)         -- "42"
numeric n = num("123")     -- 123
```

**Durum: TAMAMLANDI** ✅
- `num(string)` - String'i numeric'e çevirir
- `str(numeric)` - Numeric'i string'e çevirir (akıllı formatlama)
- `int_to_string()` - Mevcut ✅
- `string_to_int()` - Mevcut ✅

### 3. ✅ Default Parameters - TAMAMLANDI! (22 Kasım 2025)
**Öncelik: DÜŞÜK - TAMAMLANDI**

```mlp
function add(a = 10, b = 20)
    print a + b
end function

add()      -- Output: 30 (uses both defaults)
add(5)     -- Output: 25 (5 + 20)
add(5, 7)  -- Output: 12 (5 + 7)
```

**Durum: TAMAMLANDI** ✅
- ✅ Syntax parsing: `param = value` destekleniyor
- ✅ AST'de default değerler saklanıyor
- ✅ Function registry: İki geçişli (two-pass) derleme sistemi
- ✅ Runtime: Eksik parametreler otomatik olarak default değerlerle doldurulur
- ✅ Test: `test/test_default_params_extended.mlp` ✅

**İmplementasyon:**
- **Two-pass compilation**: Pre-scan → Register functions → Generate code
- **Function registry**: HashMap ile tüm fonksiyon imzaları saklanır
- **Default handling**: Eksik parametreler için default AST node'ları kullanılır
- **Forward reference**: Pre-scan sayesinde fonksiyon tanımları çağrılardan önce kaydedilir

**Test Sonuçları:**
```
Test 1: add() -> should be 30
30
Test 2: add(5) -> should be 25
25
Test 3: add(5, 7) -> should be 12
12
```

---

## 🟢 DÜŞÜK ÖNCELİKLİ EKSİKLER

### 5. ✅ typeof Operatörü - TAMAMLANDI! (22 Kasım 2025)
**Öncelik: DÜŞÜK - TAMAMLANDI**

```mlp
numeric x = 10
string type_name = typeof(x)  -- "numeric"
```

**Durum: TAMAMLANDI** ✅
- Compile-time type detection (literals)
- Runtime type detection (variables)
- Returns: "numeric", "string", "boolean", "pointer"
- Test: `tests/test_typeof.mlp` ✅

### 6. ✅ Range Syntax - TAMAMLANDI! (22 Kasım 2025)
**Öncelik: DÜŞÜK - TAMAMLANDI**

```mlp
-- Single argument: range(end) - 0 to end-1
for i in range(10)
    print i
end for

-- Two arguments: range(start, end)
for i in range(5, 15)
    print i
end for

-- Three arguments: range(start, end, step)
for i in range(0, 20, 2)
    print i
end for
```

**Durum: TAMAMLANDI** ✅
- `for X in range(...)` syntax implemented
- Supports 1, 2, or 3 arguments
- Exclusive end (Python-style): range(5) → 0,1,2,3,4
- Works with expressions: range(start, end)
- Nested loops supported
- Break/continue work correctly
- Test: `tests/test_range.mlp` ✅

### 7. ✅ Module/Import System - TAMAMLANDI! (22 Kasım 2025)
**Öncelik: DÜŞÜK - TAMAMLANDI**

```mlp
import "utils.mlp"
import "math_helpers.mlp"
```

**Durum: TAMAMLANDI** ✅
- Syntax: `import "file.mlp"` - Explicit file path
- Circular import protection (automatic skip on re-import)
- Lexer: TOKEN_IMPORT keyword recognition
- Parser: AST_IMPORT node with file path
- Generator: File reading, parsing, and code integration
- Global scope sharing: Imported variables accessible in main file
- Test: `examples/test_import.mlp` + `examples/math_utils.mlp` ✅
- Circular test: `examples/test_circular.mlp` ✅

**Örnek Kullanım:**
```mlp
// math_utils.mlp
numeric PI = 314
string MATH_VERSION = "1.0"

// main.mlp
import "math_utils.mlp"
print "PI = {PI}"  // Output: PI = 314
```

**Circular Import Koruması:**
```
A imports B → B imports A (circular detected, skipped)
```

### 8. ✅ String Interpolation - TAMAMLANDI (22 Kasım 2025)
**Öncelik: DÜŞÜK - TAMAMLANDI**

```mlp
string name = "Alice"
numeric age = 25
print "Name: {name}, Age: {age}"
```

**Durum: TAMAMLANDI** ✅
- Syntax: `"Hello {variable}!"` pattern support
- Works with string and numeric variables
- Automatic type conversion for numeric values
- Multiple variables in single string
- Lexer: TOKEN_INTERPOLATED_STRING detection
- Parser: AST_INTERPOLATED_STRING node
- Generator: String concatenation with int_to_string
- Test: `examples/test_string_interpolation.mlp` ✅

**Örnek Çıktı:**
```
Hello Alice!
Name: Alice, Age: 25
Count is 42
User Alice is 25 years old and has 42 items
```

### 9. ✅ Multi-line String - TAMAMLANDI! (Phase 5.8)
**Öncelik: DÜŞÜK - TAMAMLANDI**

```mlp
string text = """
Bu bir
çok satırlı
string'tir
"""
```

**Durum: TAMAMLANDI** ✅
- Syntax: `"""..."""` triple-quote syntax
- Newline karakterleri korunur
- String interpolation ile uyumlu
- Test: `examples/test_multiline_string.mlp` ✅

**Not:** Phase 5.8'de eklenmiş, zaten çalışıyor durumda.

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
- [x] **Generic types/templates** - Phase 6.2 Complete (`optional<T>`) ✅
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
- ✅ Tamamlandı: ~80 özellik (%80) - **Phase 6.2 Generic Types eklendi!**
- 🔴 Kritik eksik: 0 özellik - **Tüm kritik özellikler tamamlandı!** ✅
- 🟡 Orta önemli: 0 özellik - **Tamamlandı!**
- 🟢 Düşük önemli: 0 özellik - **Tamamlandı!**
- ⛔ Nice-to-have: 15+ özellik (OOP, async/await, macros, etc.)

**MLP artık modern, production-ready bir programlama dili!** 🚀
**Generic types, arrays, lists, maps, string interpolation - hepsi çalışıyor!**
