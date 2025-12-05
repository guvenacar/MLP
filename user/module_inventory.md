# MELP MODÜL ENVANTERİ

**Tarih:** 2 Aralık 2025
**Amaç:** Stage 0 → Stage 1 geçişi için tam modül listesi

---

## 📦 MEVCUT MODÜLLER (41 klasör)

### ✅ Tam Modüller (Her biri 6 dosyalı)
1. **arithmetic** - +, -, *, / (%, **, bitwise eksik)
2. **array** - Array temel yapısı
3. **array_operations** - Array işlemleri (index, length, resize)
4. **async** - async/await
5. **cli_io** - input, get_argc, get_argv
6. **codegen_context** - Codegen yardımcı yapılar
7. **collections** - Tuple, List
8. **comments** - --, ---, --#, ---#
9. **comparison** - ==, !=, <, <=, >, >=
10. **concurrency** - Thread, Mutex, Channel, Atomic
11. **control_flow** - if/else, ternary
12. **debug_features** - goto, @label, debug print
13. **enum** - Enum tanımlama
14. **exception_handling** - try-catch-finally, throw
15. **expression** - AST Expression node
16. **file_io** - read_file, write_file, append
17. **function** - ??? (functions ile duplicate?)
18. **functions** - Function tanımlama, çağırma, return
19. **generator** - yield, for-in
20. **generic_types** - Generic functions/structs
21. **interface_trait** - Interface/Trait
22. **lambda** - Lambda, closure
23. **logical** - AND, OR, NOT
24. **memory** - malloc, free, GC, defer
25. **module_system** - import, export, namespace
26. **null_safety** - nullable types, ??, ?.
27. **operator_overloading** - Operator overloading
28. **parser_core** - Parser temel yapıları
29. **pattern_matching** - Pattern matching
30. **pointer** - Address-of, dereference, arithmetic
31. **preprocessor** - Macro, conditional compilation
32. **print** - print statement
33. **program** - Program AST root
34. **reflection** - Type inspection, metadata
35. **state_management** - state, shared state
36. **statement** - AST Statement node
37. **string_operations** - ??? (string_ops ile duplicate?)
38. **string_ops** - String fonksiyonları (concat, substr, etc.)
39. **struct** - Struct tanımlama, field erişimi
40. **switch_match** - switch-case
41. **variable** - Değişken tanımlama, atama

---

## 🔍 DUPLICATE KONTROL

**Birleştirilmesi gerekenler:**
- `function` ↔ `functions` (hangisi kullanılıyor?)
- `string_operations` ↔ `string_ops` (hangisi kullanılıyor?)

---

## ❌ EKSİK MODÜLLER

### 1. ARİTMETİK GELİŞMELERİ (arithmetic içinde eksik)
- **modulo** - % operatörü
- **power** - ** operatörü  
- **bitwise** - &, |, ^ operatörleri

### 2. LOOP İYİLEŞTİRMELERİ (control_flow içinde eksik?)
- **for_loop** - for i = start to end
- **while_loop** - while condition
- **do_while** - do-while

### 3. HATA YÖNETİMİ ALTERNATİFLERİ
- **result_type** - Result<T, E>
- **option_type** - Option<T>
- **error_propagation** - ? operator

### 4. ATTRIBUTES/ANNOTATIONS
- **attributes** - @decorator, annotations

### 5. FFI (Foreign Function Interface)
- **ffi** - extern "C", dynamic library loading

### 6. BUILD SYSTEM
- **build_system** - Package management, dependency resolution

### 7. DOCUMENTATION
- **documentation** - Doc comments, doc generation

### 8. TEST FRAMEWORK
- **test_framework** - Unit tests, assert, test runner

### 9. PERFORMANCE
- **performance** - inline, constexpr, SIMD, optimization hints

### 10. ADVANCED NUMERIC TYPES
- **numeric_advanced** - unsigned, fixed-point, complex, rational

### 11. REGEX
- **regex** - Regular expressions

### 12. NETWORK I/O
- **network_io** - Socket, HTTP, WebSocket

---

## 🎯 ÖNCELİK SIRASI

### P0 - KRİTİK (Stage 0 completion için)
1. ✅ Duplicate'leri temizle (function/functions, string_operations/string_ops)
2. ✅ arithmetic modülünü tamamla (mod, power, bitwise)
3. ✅ control_flow'a eksik döngüleri ekle (for, while, do-while)

### P1 - ÖNEMLİ (Stage 1 başlamadan önce)
4. ❌ result_type, option_type, error_propagation modülleri
5. ❌ attributes modülü (decorators için)

### P2 - ORTA (Stage 1 sırasında)
6. ❌ test_framework modülü (self-test için)
7. ❌ documentation modülü
8. ❌ performance modülü

### P3 - DÜŞÜK (Stage 2+)
9. ❌ ffi modülü
10. ❌ build_system modülü
11. ❌ numeric_advanced modülü
12. ❌ regex modülü
13. ❌ network_io modülü

---

## 📋 MODÜL YAPISI STANDARDI

Her modül **6 dosyadan** oluşur:

### C Implementation (Stage 0)
```
modules/module_name/
  ├── module_name.h              # Veri yapıları, API
  ├── module_name.c              # Core implementasyon
  ├── module_name_parser.h       # Parser API
  ├── module_name_parser.c       # Parser implementasyon
  ├── module_name_codegen.h      # Codegen API
  └── module_name_codegen.c      # Codegen implementasyon
```

### MLP Implementation (Stage 1)
```
melp/melp/modules/module_name/
  ├── module_name.mlp            # Core implementasyon
  ├── module_name_parser.mlp     # Parser implementasyon
  └── module_name_codegen.mlp    # Codegen implementasyon
```

---

## ✅ AKSIYONLAR

1. **Duplicate kontrolü yap**
   - [ ] function vs functions → Hangisi kullanılıyor?
   - [ ] string_operations vs string_ops → Hangisi kullanılıyor?
   - [ ] Kullanılmayanı sil veya birleştir

2. **Eksik C modüllerini oluştur (P0)**
   - [ ] arithmetic içine: modulo, power, bitwise ekle
   - [ ] control_flow kontrol et: for, while, do-while var mı?

3. **Yeni modüller oluştur (P1)**
   - [ ] result_type modülü
   - [ ] option_type modülü  
   - [ ] error_propagation modülü
   - [ ] attributes modülü

4. **Stage 1 MLP dosyaları oluştur**
   - [ ] Her modül için .mlp dosyaları oluştur
   - [ ] Modül sırasını belirle (bağımlılık grafiği)

5. **Makefile güncelle**
   - [ ] Tüm yeni modülleri ekle
   - [ ] Derleme sırasını optimize et

---

**Sonraki Adım:** Duplicate kontrolü ile başla
