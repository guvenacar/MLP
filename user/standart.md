# Programlama Dili Standartları

## 1. TURING COMPLETE (Olmazsa Olmaz)

Bir dilin Turing Complete olması için gereken **minimum** yapılar:

### 1.1 Değişkenler ve Atama
```
✅ Değişken tanımlama
✅ Değişkene değer atama
✅ Değişken okuma
```

### 1.2 Aritmetik İşlemler
```
✅ Toplama (+)
✅ Çıkarma (-)
✅ Çarpma (*)
✅ Bölme (/)
```

### 1.3 Koşullu Dallanma
```
✅ if-then-else
✅ Karşılaştırma operatörleri (==, !=, <, >, <=, >=)
```

### 1.4 Döngü (en az biri)
```
✅ while döngüsü
   VEYA
✅ goto + label
   VEYA
✅ Özyineleme (recursion)
```

### 1.5 Bellek/Depolama
```
✅ Sınırsız bellek erişimi (dizi veya işaretçi)
```

**Not:** Bu 5 yapı ile teorik olarak HER hesaplama yapılabilir.

---

## 2. STANDART DİL ÖZELLİKLERİ

Pratik kullanım için gereken temel özellikler:

### 2.1 Veri Tipleri
```
✅ Tam sayı (integer/numeric)
✅ Ondalıklı sayı (float/decimal)
✅ Mantıksal (boolean)
✅ Metin (string/text)
✅ Karakter (char) - opsiyonel, string ile çözülebilir
✅ Boş değer (null/nil/none)
```

### 2.2 Operatörler
```
Aritmetik:
✅ +, -, *, /, % (mod)

Karşılaştırma:
✅ ==, !=, <, >, <=, >=

Mantıksal:
✅ and (&&)
✅ or (||)
✅ not (!)

Atama:
✅ =
✅ +=, -=, *=, /= (bileşik atama)
✅ ++, -- (artırma/azaltma) - opsiyonel
```

### 2.3 Kontrol Yapıları
```
Koşullu:
✅ if-then-else
✅ else if / elif
✅ switch-case (veya match)

Döngü:
✅ while
✅ for (sayaçlı)
✅ for-each / for-in (koleksiyon üzerinde)
✅ do-while

Döngü Kontrolü:
✅ break
✅ continue
```

### 2.4 Fonksiyonlar
```
✅ Fonksiyon tanımlama
✅ Parametre alma
✅ Değer döndürme (return)
✅ Birden fazla değer döndürme (tuple/multi-return)
✅ Varsayılan parametre değerleri
✅ Özyineleme (recursion)
```

### 2.5 Veri Yapıları
```
✅ Dizi (array) - sabit boyutlu
✅ Dinamik dizi (list/vector)
✅ Sözlük/Harita (dictionary/map/hashmap)
✅ Yapı (struct/record)
✅ Tuple (sabit uzunluklu heterojen koleksiyon)
```

### 2.6 String İşlemleri
```
✅ Birleştirme (concatenation)
✅ Uzunluk (length)
✅ Alt string (substring)
✅ Arama (indexOf/find)
✅ Bölme (split)
✅ Büyük/küçük harf (toUpper/toLower)
✅ Trim (boşluk temizleme)
✅ String interpolation ("Hello {name}")
```

### 2.7 Giriş/Çıkış (I/O)
```
Konsol:
✅ print/write (çıktı)
✅ read/input (girdi)

Dosya:
✅ Dosya okuma
✅ Dosya yazma
✅ Dosya ekleme (append)
✅ Dosya varlık kontrolü (exists)
```

### 2.8 Modülerlik
```
✅ Modül/dosya import
✅ Export (public/private)
✅ Namespace/module sistemi
```

### 2.9 Hata Yönetimi
```
✅ try-catch (veya eşdeğeri)
✅ throw/raise (hata fırlatma)
✅ finally (temizlik bloğu)
```

### 2.10 Yorum Satırları
```
✅ Tek satır yorum (// veya # veya --)
✅ Çok satır yorum (/* */ veya """ """)
```

---

## 3. MODERN DİL ÖZELLİKLERİ

Günümüz dillerinde beklenen gelişmiş özellikler:

### 3.1 Tip Sistemi
```
✅ Statik tip kontrolü (derleme zamanı)
✅ Tip çıkarımı (type inference)
✅ Generics/Templates (parametrik polimorfizm)
✅ Union types (A | B)
✅ Optional/Nullable types (T?)
✅ Type alias (tip takma adı)
```

### 3.2 Null Güvenliği
```
✅ Nullable tipler (string?)
✅ Null-safe operatörler:
   - ?. (optional chaining)
   - ?? (null coalescing)
   - ?! (force unwrap)
✅ Null kontrolü zorunluluğu
```

### 3.3 Pattern Matching
```
✅ match/switch expression
✅ Destructuring (parçalama)
✅ Guard clauses (koşullu eşleşme)
✅ Wildcard pattern (_)
✅ Type pattern (tip eşleştirme)
```

### 3.4 Fonksiyonel Programlama
```
✅ First-class functions (fonksiyon değişken olarak)
✅ Lambda/Anonymous functions
✅ Closure (kapanış - dış değişkenleri yakalama)
✅ Higher-order functions:
   - map
   - filter
   - reduce/fold
   - forEach
✅ Immutability desteği (const/final/val)
✅ Pure functions teşviki
```

### 3.5 Nesne Yönelimli Programlama
```
✅ Struct/Class tanımlama
✅ Method tanımlama
✅ Constructor/Destructor
✅ Encapsulation (public/private/protected)
✅ Inheritance (kalıtım) - opsiyonel, composition tercih edilebilir
✅ Interface/Protocol/Trait
✅ Polymorphism
✅ Operator overloading
```

### 3.6 Iterator ve Generator
```
✅ Iterator protocol (for-in desteği)
✅ Generator functions (yield)
✅ Lazy evaluation (tembel değerlendirme)
✅ Range expressions (1..10, 1..<10)
```

### 3.7 Asenkron Programlama
```
✅ async/await
✅ Promise/Future
✅ Concurrent execution
✅ Event loop veya runtime desteği
```

### 3.8 Hata Yönetimi (Gelişmiş)
```
✅ Typed exceptions (tipli istisnalar)
✅ Multi-catch (birden fazla tip yakalama)
✅ Result type (Ok/Err pattern)
✅ Stack trace
✅ Custom exception types
```

### 3.9 Bellek Yönetimi
```
✅ Garbage Collection (otomatik)
   VEYA
✅ RAII / Ownership sistemi (Rust tarzı)
   VEYA
✅ Reference counting

Ek:
✅ Pointer desteği (opsiyonel, sistem programlama için)
✅ Manual memory (malloc/free - opsiyonel)
```

### 3.10 Meta-Programlama
```
✅ Reflection (çalışma zamanı tip bilgisi)
✅ Attributes/Annotations/Decorators
✅ Macros (derleme zamanı kod üretimi) - opsiyonel
✅ Code generation
```

### 3.11 Enum (Gelişmiş)
```
✅ Basit enum (sabitler)
✅ Associated values (değerli enum)
✅ Enum methods
✅ Pattern matching ile kullanım
```

### 3.12 State Management
```
✅ State değişkenler (global ama kontrollü)
✅ Reactive/Observable pattern
✅ Immutable state teşviki
```

### 3.13 Bitwise Operatörler
```
✅ & (AND)
✅ | (OR)
✅ ^ (XOR)
✅ ~ (NOT)
✅ << (left shift)
✅ >> (right shift)
```

### 3.14 Debugging & Tooling
```
✅ Debug labels/breakpoints
✅ Assert statements
✅ Logging altyapısı
✅ Source maps
✅ REPL (interaktif ortam)
```

### 3.15 Interoperability
```
✅ FFI (Foreign Function Interface) - C çağırma
✅ Module/Package system
✅ Dependency management
```

---

## 4. MELP DURUM TABLOSU

| Kategori | Özellik | Durum |
|----------|---------|-------|
| **Turing Complete** | | |
| | Değişkenler | ✅ |
| | Aritmetik | ✅ |
| | Koşul (if) | ✅ |
| | Döngü (while/for) | ✅ |
| | Bellek (array) | ✅ |
| **Standart** | | |
| | Veri tipleri (numeric, decimal, boolean, string) | ✅ |
| | Karşılaştırma operatörleri | ✅ |
| | Mantıksal operatörler (and, or, not) | ✅ |
| | else if | ✅ |
| | switch-case | ✅ |
| | for-in | ✅ |
| | do-while | ✅ |
| | break/continue | ✅ |
| | Fonksiyonlar | ✅ |
| | Multi-return | ✅ |
| | Struct | ✅ |
| | Dinamik dizi | ✅ |
| | String işlemleri | ✅ |
| | String interpolation | ✅ |
| | Dosya I/O | ✅ |
| | Import/Export | ✅ |
| | Module sistemi | ✅ |
| | try-catch-finally | ✅ |
| | throw | ✅ |
| **Modern** | | |
| | Type inference | ✅ (TTO) |
| | Generics | ✅ |
| | Nullable types (?) | ✅ |
| | Optional chaining (?.) | ✅ |
| | Null coalescing (??) | ✅ |
| | Pattern matching (match) | ✅ |
| | Destructuring | ✅ |
| | Lambda fonksiyonlar | ✅ |
| | Closure | ✅ |
| | Interface | ✅ |
| | Operator overloading | ✅ |
| | Iterator/for-in | ✅ |
| | Generator (yield) | ✅ |
| | Range (1..10) | ✅ |
| | async/await | ✅ |
| | Typed exceptions | ✅ |
| | Multi-catch | ✅ |
| | Pointer desteği | ✅ |
| | Enum | ✅ |
| | Type alias | ✅ |
| | State management | ✅ |
| | Bitwise operatörler | ✅ |
| | Debug labels | ✅ |
| | CLI args | ✅ |
| **Eksik/Geliştirilecek** | | |
| | Dictionary/Map | ❌ |
| | Set veri yapısı | ❌ |
| | Class (sadece struct var) | ➖ |
| | Inheritance | ❌ |
| | Garbage Collection | ❌ (manuel) |
| | Reflection | ❌ |
| | Macros | ❌ |
| | Package manager | ❌ |
| | REPL | ❌ |

---

## 5. ÖNCELİK SIRASI (Yeni Dil Geliştirme İçin)

### Aşama 1: Minimum Çalışır Dil
1. Değişken + Atama
2. Aritmetik (+, -, *, /)
3. if-else
4. while
5. print

### Aşama 2: Kullanılabilir Dil
6. Fonksiyonlar
7. String tipi
8. for döngüsü
9. Array
10. Temel I/O

### Aşama 3: Pratik Dil
11. Struct
12. Import/Module
13. try-catch
14. switch-case
15. for-in

### Aşama 4: Modern Dil
16. Lambda/Closure
17. Generics
18. Nullable types
19. Pattern matching
20. async/await

### Aşama 5: Olgun Dil
21. Garbage Collection
22. Standard Library
23. Package Manager
24. Tooling (LSP, Formatter, Linter)
25. Documentation Generator

---

Modül Haritası:

Satır	Fonksiyonlar	Modül
1-147	Struct tanımları, #include, #define	codegen.h (types)
148-191	codegen_create	codegen_core.c
192-210	codegen_module_seen, codegen_mark_module_seen	codegen_module.c
212-236	codegen_add/find_operator_overload	codegen_operator.c
237-313	generic funcs (add, find, mangle, instance)	codegen_generic.c
316-497	variable management	codegen_variable.c
499-549	enum_value, type_alias	codegen_type.c
551-564	emit, add_string	codegen_core.c
566-631	function, struct	codegen_function.c, codegen_struct.c
642-668	interface	codegen_interface.c
674-834	prologue, epilogue, is_builtin	codegen_prologue.c
836-1130	declaration	codegen_decl.c
1131-1262	assignment	codegen_assign.c
1264-1386	print + helpers	codegen_print.c
1387-2822	expression_value	codegen_expr.c
2823-2962	comparison	codegen_compare.c
2964-3016	if	codegen_if.c
3018-3274	for, for_in	codegen_for.c
3276-3385	while, do_while	codegen_while.c
3387-3541	switch	codegen_switch.c
3543-3689	func_def	codegen_funcdef.c
3691-3768	return	codegen_return.c
3769-4511	statement	codegen_stmt.c
4512-4735	generate (main)	codegen_main.c
4737-4741	free	codegen_core.c

*Bu doküman MELP dili ve genel programlama dili tasarımı için referans olarak hazırlanmıştır.*
