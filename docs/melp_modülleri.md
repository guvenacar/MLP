# MLP Modül ve Kütüphane Referansı

**Son Güncelleme:** 24 Aralık 2025  
**Durum:** Aktif Geliştirme

Bu dokümantasyon, MLP dilinde mevcut olan tüm modülleri, kütüphaneleri ve fonksiyonları içerir.

---

## 📚 İçindekiler

1. [Kütüphane Yapısı](#kütüphane-yapısı)
2. [Core Kütüphaneleri](#core-kütüphaneleri)
3. [Runtime Fonksiyonları](#runtime-fonksiyonları)
4. [JSON Kütüphanesi](#json-kütüphanesi)
5. [Standart Kütüphane](#standart-kütüphane)

---

## Kütüphane Yapısı

```
lib/
├── core/              # Temel kütüphaneler (Pure MLP)
│   ├── functional.mlp     # Fonksiyonel programlama (map, filter, reduce)
│   ├── optional.mlp       # Optional<T> generic tip
│   └── string_ops.mlp     # String işlemleri
│
├── json/              # JSON işleme
│   ├── json_parser.mlp    # JSON parser
│   ├── json_lexer_*.mlp   # Çeşitli lexer versiyonları
│   └── DESIGN.md          # Tasarım dokümantasyonu
│
└── std/               # Standart kütüphane
    ├── mlp.mlp            # Ana compiler entry point
    └── mylib*.mlp         # Utility kütüphaneleri
```

---

## Core Kütüphaneleri

### 📦 lib/core/functional.mlp

**Fonksiyonel Programlama - Map/Filter/Reduce**

Higher-order array fonksiyonları ile fonksiyonel programlama desteği.

#### Map Fonksiyonları

| Fonksiyon | Açıklama | Parametreler | Döndürür |
|-----------|----------|--------------|----------|
| `map_double()` | Her elemanı 2 ile çarpar | `arr[]`, `size` | `numeric[]` |
| `map_square()` | Her elemanın karesini alır | `arr[]`, `size` | `numeric[]` |
| `map_increment()` | Her elemana 1 ekler | `arr[]`, `size` | `numeric[]` |

**Örnek:**
```mlp
numeric data[] = [1, 2, 3, 4, 5]
numeric result[] = map_double(data, 5)
-- Result: [2, 4, 6, 8, 10]
```

#### Filter Fonksiyonları

| Fonksiyon | Açıklama | Parametreler | Döndürür |
|-----------|----------|--------------|----------|
| `filter_positive()` | Pozitif elemanları filtreler | `arr[]`, `size`, `out_size` | `numeric[]` |
| `filter_even()` | Çift sayıları filtreler | `arr[]`, `size`, `out_size` | `numeric[]` |
| `filter_greater_than_10()` | 10'dan büyükleri filtreler | `arr[]`, `size` | `numeric[]` |

**Örnek:**
```mlp
numeric data[] = [-2, -1, 0, 1, 2, 3]
numeric out_size = 0
numeric result[] = filter_positive(data, 6, out_size)
-- Result: [1, 2, 3], out_size = 3
```

#### Reduce Fonksiyonları

| Fonksiyon | Açıklama | Parametreler | Döndürür |
|-----------|----------|--------------|----------|
| `reduce_sum()` | Tüm elemanları toplar | `arr[]`, `size` | `numeric` |
| `reduce_product()` | Tüm elemanları çarpar | `arr[]`, `size` | `numeric` |
| `reduce_max()` | En büyük elemanı bulur | `arr[]`, `size` | `numeric` |
| `reduce_min()` | En küçük elemanı bulur | `arr[]`, `size` | `numeric` |

**Örnek:**
```mlp
numeric data[] = [1, 2, 3, 4, 5]
numeric sum = reduce_sum(data, 5)
-- Result: 15
```

#### Yardımcı Fonksiyonlar

| Fonksiyon | Açıklama | Parametreler | Döndürür |
|-----------|----------|--------------|----------|
| `count_if_positive()` | Pozitif eleman sayısı | `arr[]`, `size` | `numeric` |
| `any_positive()` | Herhangi biri pozitif mi? | `arr[]`, `size` | `boolean` |
| `all_positive()` | Tümü pozitif mi? | `arr[]`, `size` | `boolean` |
| `print_array()` | Array'i yazdırır | `arr[]`, `size` | `void` |

---

### 📦 lib/core/optional.mlp

**Generic Optional<T> Tip İmplementasyonu**

Null-safety için Optional tipi (Phase 6.2).

#### OptionalNumeric

```mlp
struct OptionalNumeric then
    numeric value
    boolean has_value
end
```

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `optional_numeric_none()` | Boş Optional oluşturur | `OptionalNumeric` |
| `optional_numeric_some(val)` | Değerli Optional oluşturur | `OptionalNumeric` |
| `optional_numeric_has_value(opt)` | Değer var mı kontrol eder | `boolean` |
| `optional_numeric_value(opt)` | Değeri döndürür | `numeric` |
| `optional_numeric_value_or(opt, default)` | Değer veya default döndürür | `numeric` |

**Örnek:**
```mlp
OptionalNumeric opt = optional_numeric_some(42)
if optional_numeric_has_value(opt) then
    print(optional_numeric_value(opt))  -- 42
end
```

#### OptionalString

```mlp
struct OptionalString then
    string value
    boolean has_value
end
```

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `optional_string_none()` | Boş Optional oluşturur | `OptionalString` |
| `optional_string_some(val)` | Değerli Optional oluşturur | `OptionalString` |
| `optional_string_has_value(opt)` | Değer var mı kontrol eder | `boolean` |
| `optional_string_value(opt)` | Değeri döndürür | `string` |
| `optional_string_value_or(opt, default)` | Değer veya default döndürür | `string` |

---

### 📦 lib/core/string_ops.mlp

**String İşlemleri Kütüphanesi**

Pure MLP ile yazılmış string fonksiyonları (Phase 6.1).

| Fonksiyon | Açıklama | Parametreler | Döndürür |
|-----------|----------|--------------|----------|
| `string_to_upper(input)` | Büyük harfe çevirir | `string` | `string` |
| `string_to_lower(input)` | Küçük harfe çevirir | `string` | `string` |
| `string_trim(input)` | Baştaki/sondaki boşlukları siler | `string` | `string` |

**Örnek:**
```mlp
string text = "  Hello World  "
string upper = string_to_upper(text)    -- "  HELLO WORLD  "
string trimmed = string_trim(text)      -- "Hello World"
string lower = string_to_lower(upper)   -- "  hello world  "
```

**Not:** Bu fonksiyonlar C runtime fonksiyonlarını kullanır:
- `c_string_length()` - String uzunluğu
- `c_char_at()` - Karakter al
- `c_set_char_at()` - Karakter set et
- `c_malloc()` - Bellek ayır

---

## Runtime Fonksiyonları

MLP Runtime (C ile yazılmış) geniş bir API sunar. Tüm fonksiyonlar `runtime/runtime.c` içinde tanımlıdır.

### 🔢 Temel I/O

| Fonksiyon | Açıklama | Parametreler | Döndürür |
|-----------|----------|--------------|----------|
| `yazdir_sayi()` | Sayı yazdır | `int64_t` | `void` |
| `yazdir_metin()` | Metin yazdır | `const char*` | `void` |
| `read_input()` | Kullanıcı girdisi al | - | `char*` |
| `read_line()` | Satır oku | - | `char*` |

### 📝 String İşlemleri

#### Temel String Fonksiyonları

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `string_birlestir(str1, str2)` | İki string birleştir | `char*` |
| `string_karsilastir(str1, str2)` | String karşılaştır | `int` |
| `string_concat(s1, s2)` | String birleştir (concat) | `char*` |
| `string_length(str)` | String uzunluğu | `long` |
| `len(str)` | String uzunluğu (kısa) | `long` |

#### String Manipülasyonu

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `string_substring(str, start, length)` | Alt string | `char*` |
| `substring(str, start, length)` | Alt string (alias) | `char*` |
| `mid(str, start, length)` | Ortadan al | `char*` |
| `left(str, length)` | Soldan al | `char*` |
| `right(str, length)` | Sağdan al | `char*` |
| `string_alt(str, baslangic, uzunluk)` | Alt string (Türkçe) | `char*` |

#### String Arama

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `string_find(str, needle)` | String içinde ara | `int` |
| `string_index_of(str, needle)` | İlk indeks | `long` |
| `string_last_index_of(str, needle)` | Son indeks | `long` |
| `contains(str, needle)` | İçeriyor mu? | `long` |
| `string_starts_with(str, prefix)` | Başlıyor mu? | `int` |
| `string_ends_with(str, suffix)` | Bitiyor mu? | `int` |
| `startswith(str, prefix)` | Başlıyor mu? (alias) | `long` |
| `endswith(str, suffix)` | Bitiyor mu? (alias) | `long` |

#### String Dönüşüm

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `string_join(parts, count, delim)` | String array'i birleştir | `char*` |
| `string_replace(str, old, new)` | String değiştir | `char*` |
| `string_trim(str)` | Boşlukları sil | `char*` |
| `string_upper(str)` | Büyük harf | `char*` |
| `string_lower(str)` | Küçük harf | `char*` |

#### Karakter İşlemleri

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `string_karakter_al(str, indeks)` | Karakter al | `char*` |
| `string_char_at(str, index)` | Karakter al | `char*` |
| `char_at(str, index)` | Karakter al (alias) | `char*` |
| `char_code_at(str, index)` | ASCII kodu al | `long` |
| `ord(ch)` | Karakter → ASCII | `long` |
| `chr(code)` | ASCII → Karakter | `char*` |
| `kodu_karaktere(kod)` | ASCII → Karakter (Türkçe) | `char*` |
| `code_to_char(code)` | ASCII → Karakter | `char*` |
| `char_to_string(ch)` | Karakter → String | `char*` |

### 🔢 Tip Dönüşümleri

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `int_to_string(num)` | Int → String | `char*` |
| `string_to_float(str)` | String → Float | `double` |
| `float_to_string(num)` | Float → String | `char*` |
| `string_to_bool(str)` | String → Bool | `int` |
| `bool_to_string(value)` | Bool → String | `char*` |
| `num(str)` | String → Numeric | `double` |
| `str(value)` | Numeric → String | `char*` |
| `mlp_get_type(value)` | Tipi al | `const char*` |

### 📊 Matematik Fonksiyonları

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `math_sqrt(num)` | Karekök | `double` |
| `math_pow(base, exponent)` | Üs alma | `double` |
| `math_fabs(num)` | Mutlak değer (float) | `double` |
| `math_sin(angle)` | Sinüs | `double` |
| `math_cos(angle)` | Kosinüs | `double` |
| `math_tan(angle)` | Tanjant | `double` |

### 📁 Dosya İşlemleri

#### Temel Dosya İO

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `read_file(path)` | Dosya oku | `char*` |
| `write_file(path, content)` | Dosyaya yaz | `int` |
| `append_file(path, content)` | Dosyaya ekle | `int` |
| `file_exists(path)` | Dosya var mı? | `int` |
| `file_size(path)` | Dosya boyutu | `long` |
| `dosya_oku(dosya_ptr)` | Dosya oku (Türkçe) | `char*` |

#### Binary I/O

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `write_binary(path, data, size)` | Binary yaz | `int` |

#### Dosya Bilgileri

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `get_file_info(path)` | Dosya bilgisi al | `char*` |
| `copy_file(source, dest)` | Dosya kopyala | `int` |
| `get_file_extension(path)` | Dosya uzantısı | `char*` |
| `get_file_name(path)` | Dosya adı | `char*` |

### 📂 Dizin İşlemleri

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `create_directory(path)` | Dizin oluştur | `int` |
| `remove_directory(path)` | Dizin sil | `int` |
| `directory_exists(path)` | Dizin var mı? | `int` |
| `get_current_dir()` | Mevcut dizin | `char*` |
| `change_directory(path)` | Dizin değiştir | `int` |
| `runtime_dizin_al()` | Runtime dizini al | `char*` |
| `get_executable_path()` | Çalıştırılabilir path | `char*` |
| `tyd_fix_cwd()` | CWD düzelt | `char*` |

### 🛤️ Path Utilities

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `join_path(parts, count)` | Path birleştir | `char*` |
| `get_directory(path)` | Dizin yolu al | `char*` |

### ⚙️ Process Control

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `execute_command(cmd)` | Komut çalıştır | `int` |
| `get_command_output(cmd)` | Komut çıktısı al | `char*` |
| `get_process_id()` | Process ID | `int` |
| `get_parent_process_id()` | Parent process ID | `int` |

### 🕒 Zaman İşlemleri

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `current_timestamp()` | Unix timestamp | `long` |
| `sleep_ms(milliseconds)` | Bekleme (ms) | `void` |
| `format_timestamp(timestamp, format)` | Timestamp format | `char*` |
| `parse_timestamp(date_string)` | String → Timestamp | `long` |
| `get_time_string()` | Zaman string | `char*` |

### 🔐 Hata Yönetimi

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `exit_with_code(code)` | Çıkış yap | `void` |
| `panic(message)` | Panic (hata fırlat) | `void` |
| `mlp_assert(condition, message)` | Assert kontrolü | `void` |
| `get_error_code()` | Hata kodu al | `long` |
| `set_error_code(code)` | Hata kodu set et | `void` |

### 🧠 Bellek Yönetimi

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `mlp_free(ptr)` | Bellek serbest bırak | `void` |
| `c_free(ptr)` | C free | `void` |
| `get_allocated_bytes()` | Ayrılan byte sayısı | `long` |
| `check_memory_leaks()` | Bellek sızıntısı kontrolü | `long` |
| `c_memcpy(dest, src, n)` | Bellek kopyala | `void` |
| `c_memset(dest, value, n)` | Bellek set et | `void` |

### 🌍 Çevre Değişkenleri

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `get_env(name)` | Çevre değişkeni al | `char*` |

### 📋 Komut Satırı Argümanları

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `mlp_set_args(argc, argv)` | Argümanları set et | `void` |
| `mlp_get_argc()` | Argüman sayısı | `long` |
| `mlp_get_argv(index)` | Argüman al | `char*` |

### 📚 Liste (List) İşlemleri

**C List API:**

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `list_destroy(list)` | Liste yok et | `void` |
| `list_add(list, element)` | Eleman ekle | `int` |
| `list_size(list)` | Liste boyutu | `int` |
| `list_clear(list)` | Listeyi temizle | `void` |
| `list_remove(list, index)` | Eleman sil | `int` |

**MLP List API:**

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `mlp_list_add(list, element)` | Eleman ekle | `void` |
| `mlp_list_set(list, index, element)` | Eleman değiştir | `void` |
| `mlp_list_remove(list, index)` | Eleman sil | `void` |
| `mlp_list_insert(list, index, element)` | Eleman ekle (belirli indeks) | `void` |
| `mlp_list_clear(list)` | Listeyi temizle | `void` |
| `mlp_list_free(list)` | Liste serbest bırak | `void` |

### 🗂️ Sözlük (Dictionary) İşlemleri

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `sozluk_olustur(boyut)` | Sözlük oluştur | `void` |
| `sozluk_ekle(anahtar, deger)` | Anahtar-değer ekle | `void` |
| `sozluk_yok_et()` | Sözlük yok et | `void` |

### 🔧 Low-level C Fonksiyonları

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `c_set_char_at(str, index, ch)` | Karakter set et | `void` |
| `c_memcpy(dest, src, n)` | Bellek kopyala | `void` |
| `c_memset(dest, value, n)` | Bellek set et | `void` |

---

## JSON Kütüphanesi

### 📦 lib/json/

JSON parse etme ve işleme kütüphanesi.

#### Mevcut Dosyalar

- `json_parser.mlp` - Ana JSON parser
- `json_lexer_final.mlp` - Final lexer versiyonu
- `json_lexer_v3.mlp` - Lexer v3
- `json_lexer_with_lists.mlp` - Liste destekli lexer
- `lexer_minimal.mlp` - Minimal lexer
- `DESIGN.md` - Tasarım dokümantasyonu

#### Token Tipleri

```mlp
int TOKEN_LBRACE = 1      -- {
int TOKEN_RBRACE = 2      -- }
int TOKEN_LBRACKET = 3    -- [
int TOKEN_RBRACKET = 4    -- ]
int TOKEN_COLON = 5       -- :
int TOKEN_COMMA = 6       -- ,
int TOKEN_STRING = 7      -- "text"
int TOKEN_NUMBER = 8      -- 123
int TOKEN_TRUE = 9        -- true
int TOKEN_FALSE = 10      -- false
int TOKEN_NULL = 11       -- null
int TOKEN_EOF = 12        -- End of input
```

**Not:** JSON kütüphanesi aktif geliştirme aşamasında. Farklı versiyonlar denenmiş.

---

## Standart Kütüphane

### 📦 lib/std/mlp.mlp

**MLP Self-Hosting Compiler - Ana Entry Point**

Compiler'ın kendisi MLP ile yazılmış (self-hosting).

#### Compiler Modülleri

```mlp
import "compiler_core/token_types.mlp"
import "compiler_core/ast_nodes.mlp"
import "compiler_core/function_registry.mlp"
import "compiler_core/lexer.mlp"
import "compiler_core/parser.mlp"
import "compiler_core/parser_functions.mlp"
import "compiler_core/parser_structures.mlp"
import "compiler_core/parser_advanced.mlp"
import "compiler_core/scope_manager.mlp"
import "compiler_core/codegen.mlp"
import "compiler_core/codegen_functions.mlp"
import "compiler_core/codegen_structures.mlp"
import "compiler_core/codegen_control.mlp"
import "runtime/file_io.mlp"
```

#### CompilerOptions Struct

```mlp
struct CompilerOptions then
    metin input_file
    metin output_file
    sayisal verbose
    sayisal debug_tokens
    sayisal debug_ast
    sayisal debug_scope
    sayisal optimize_level
    sayisal show_help
    sayisal show_version
end struct
```

---

## 🎨 Preprocessor Modülleri

### Çokdilli Destek

MLP, Türkçe, İngilizce, Rusça, Çince, Hintçe gibi dillerde keyword desteği sunar.

**Config Dosyaları:**
- `diller.json` - Temel dil tanımları
- `diller_comprehensive.json` - Kapsamlı dil desteği
- `syntax.json` - Syntax tanımları
- `syntax_comprehensive.json` - Kapsamlı syntax

**Preprocessor:**
- `compiler/preprocessors/mlp_preprocessor.py` - Ana preprocessor
- `compiler/preprocessors/syntax_preprocessor.py` - Syntax processor

---

## 🔌 GUI Modülleri

### runtime/gui_sdl.c

SDL tabanlı GUI fonksiyonları.

#### Window Management

| Fonksiyon | Açıklama |
|-----------|----------|
| `gui_window_open(width, height, title)` | Pencere aç |
| `gui_window_create(title, width, height)` | Pencere oluştur |
| `gui_window_show(id)` | Pencere göster |
| `gui_window_hide(id)` | Pencere gizle |
| `gui_window_destroy(id)` | Pencere yok et |
| `gui_close()` | GUI kapat |

#### Drawing

| Fonksiyon | Açıklama |
|-----------|----------|
| `gui_set_color(r, g, b)` | Renk ayarla |
| `gui_draw_rect(x, y, w, h)` | Dikdörtgen çiz |
| `gui_update()` | Ekranı güncelle |
| `gui_wait(milliseconds)` | Bekle |

#### Widgets

| Fonksiyon | Açıklama |
|-----------|----------|
| `gui_button_create(window_id, text, x, y, w, h)` | Buton oluştur |
| `gui_label_create(window_id, text, x, y)` | Label oluştur |
| `gui_label_set_text(label_id, text)` | Label text değiştir |
| `gui_canvas_create(window_id, x, y, w, h)` | Canvas oluştur |
| `gui_canvas_clear(canvas_id, r, g, b)` | Canvas temizle |

---

## 🗂️ HashMap Modülü

### runtime/hashmap.c

C ile yazılmış HashMap implementasyonu.

| Fonksiyon | Açıklama | Döndürür |
|-----------|----------|----------|
| `hashmap_put(map, key, value)` | Anahtar-değer ekle | `int` |
| `hashmap_remove(map, key)` | Eleman sil | `int` |
| `hashmap_contains(map, key)` | İçeriyor mu? | `int` |
| `hashmap_size(map)` | HashMap boyutu | `int` |
| `hashmap_clear(map)` | HashMap temizle | `void` |
| `hashmap_destroy(map)` | HashMap yok et | `void` |

---

## 📝 Notlar

### Dil Desteği

MLP aşağıdaki keyword çevirilerini destekler:

| İngilizce | Türkçe | Rusça | Çince | Hintçe |
|-----------|--------|-------|-------|--------|
| `function` | `fonksiyon` | `функция` | `函数` | `फ़ंक्शन` |
| `if` | `eger` | `если` | `如果` | `अगर` |
| `while` | `dongu` | `пока` | `当` | `जबकि` |
| `return` | `dondur` | `вернуть` | `返回` | `वापसी` |
| `numeric` | `sayisal` | `число` | `数字` | `संख्या` |
| `string` | `metin` | `строка` | `字符串` | `तार` |

### Tip Sistemi

MLP'de temel tipler:
- `numeric` - Sayısal değerler (int/float)
- `string` - Metin (char*)
- `boolean` - Boolean değer (true/false)

**ÖNEMLİ:** `int`, `float`, `double` gibi C tipleri **kullanılmamalı**. Bunlar yerine `numeric` kullanılmalı.

### Build Pipeline

```
Source.mlp → Preprocessor → Normalized.mlp → Compiler → Assembly → Binary
```

1. **Preprocessor** - Çokdilli keyword'leri İngilizce'ye çevirir
2. **Compiler** - MLP → Assembly
3. **Assembler** - Assembly → Object
4. **Linker** - Object + Runtime → Executable

---

## 🚀 Kullanım Örnekleri

### Örnek 1: Functional Programming

```mlp
-- Map/Filter/Reduce kullanımı
import "lib/core/functional.mlp"

numeric data[] = [1, 2, 3, 4, 5, -1, -2]

-- Map: Her elemanı 2 ile çarp
numeric doubled[] = map_double(data, 7)

-- Filter: Sadece pozitif olanlar
numeric out_size = 0
numeric positive[] = filter_positive(data, 7, out_size)

-- Reduce: Toplamı al
numeric sum = reduce_sum(positive, out_size)

print_array(positive, out_size)
print(sum)
```

### Örnek 2: Optional Kullanımı

```mlp
import "lib/core/optional.mlp"

function divide(numeric a, numeric b) returns OptionalNumeric then
    if b == 0 then
        return optional_numeric_none()
    end
    return optional_numeric_some(a / b)
end

OptionalNumeric result = divide(10, 2)
if optional_numeric_has_value(result) then
    print("Result: " + str(optional_numeric_value(result)))
else
    print("Division by zero!")
end
```

### Örnek 3: String İşlemleri

```mlp
import "lib/core/string_ops.mlp"

string name = "  John Doe  "
string clean = string_trim(name)           -- "John Doe"
string upper = string_to_upper(clean)      -- "JOHN DOE"
string lower = string_to_lower(upper)      -- "john doe"

print(upper)
```

### Örnek 4: Dosya İşlemleri

```mlp
-- Dosya oku
string content = read_file("data.txt")

-- Dosyaya yaz
write_file("output.txt", "Hello World")

-- Dosyaya ekle
append_file("output.txt", "\nNew line")

-- Dosya var mı kontrol et
if file_exists("data.txt") then
    print("File exists!")
    print("Size: " + str(file_size("data.txt")) + " bytes")
end
```

---

## 🔮 Gelecek Geliştirmeler

### Planlanan Özellikler

- [ ] Lambda ve Closure desteği
- [ ] Generic tipler (Optional dışında)
- [ ] Gelişmiş error handling
- [ ] Async/await desteği
- [ ] Package manager
- [ ] Standard library expansion

### Bilinen Eksikler

- Lambda/closure yok (13+ test bekliyor)
- `const` keyword desteği yok
- `++`, `--` operatörleri yok
- Ternary operator (`? :`) yok
- Line continuation (`\`) yok
- String interpolation eksik

---

## 📖 Referanslar

- [ARCHITECTURE_AND_MEMORY_STRATEGY.md](ARCHITECTURE_AND_MEMORY_STRATEGY.md)
- [LANGUAGE_EVOLUTION_GUIDE.md](LANGUAGE_EVOLUTION_GUIDE.md)
- [PHASE5_4_LANGUAGE_FEATURES.md](PHASE5_4_LANGUAGE_FEATURES.md)
- [TEST_FIX_GUIDE.md](status/TEST_FIX_GUIDE.md)
- [HANDOFF_TO_NEXT_AI.md](../HANDOFF_TO_NEXT_AI.md)

---

**Son Güncelleme:** 24 Aralık 2025  
**Katkıda Bulunanlar:** MLP Development Team  
**Lisans:** Proje LICENSE dosyasına bakın
