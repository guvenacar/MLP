# MLP Modülleri

MLP'nin gelecekteki özelliklerini içeren modül koleksiyonu.

## 📁 Kategorizasyon

### Core (Temel)
Dilin temel yapı taşları - **her zaman gerekli**
- `lexer`, `parser`, `ast`, `semantic`
- `codegen_context`, `error_handling`
- `type_system`, `print`

### Advanced (İleri)
Güçlü özellikler - **sık kullanılan**
- `async`, `concurrency`, `memory`
- `generic`, `trait`, `enum`, `struct`
- `macro`, `decorator`, `pattern_matching`

### Experimental (Deneysel)
Yenilikçi özellikler - **geliştirilmekte**
- `dependent_types`, `linear_types`
- `type_classes`, `const_generics`
- `reflection`, `unsafe_blocks`

## 🔧 Modül Durumu

| Kategori | Hazır | Kısmi | Planlı | Toplam |
|----------|-------|-------|--------|--------|
| Core | 8 | 0 | 0 | 8 |
| Advanced | 14 | 8 | 0 | 22 |
| Experimental | 0 | 5 | 12 | 17 |
| **Toplam** | **22** | **13** | **12** | **47** |

## 📊 Detay

### Core Modüller (8/8 ✅)
1. ✅ `lexer` - Tokenization
2. ✅ `parser` - Syntax parsing
3. ✅ `ast` - Abstract Syntax Tree
4. ✅ `semantic` - Semantic analysis
5. ✅ `codegen_context` - Code generation context
6. ✅ `error_handling` - Error handling
7. ✅ `type_system` - Type system
8. ✅ `print` - Print functionality

### Advanced Modüller (14/22 ✅)
**Tam Çalışan (14):**
- ✅ `async` - Async/await
- ✅ `concurrency` - Concurrency primitives
- ✅ `memory` - Memory management
- ✅ `null_safety` - Null safety
- ✅ `generic` - Generic types
- ✅ `trait` - Traits
- ✅ `enum` - Enumerations
- ✅ `struct` - Structures
- ✅ `macro` - Macro system
- ✅ `decorator` - Decorators
- ✅ `ffi` - Foreign Function Interface
- ✅ `regex` - Regex patterns
- ✅ `pattern_matching` - Pattern matching
- ✅ `debug` - Debug features

**Kısmi Uygulanan (8):**
- 🚧 `array_operations` - Array ops
- 🚧 `collections` - Collections
- 🚧 `generator` - Generators
- 🚧 `iterator_system` - Iterators
- 🚧 `module_system` - Module system
- 🚧 `operator_overloading` - Operator overloading
- 🚧 `result_option` - Result/Option types
- 🚧 `smart_pointers` - Smart pointers

### Experimental (0/17 ⏳)
**Planlanan:**
- ⏳ `dependent_types` - Dependent types
- ⏳ `linear_types` - Linear types
- ⏳ `type_classes` - Type classes
- ⏳ `const_generics` - Const generics
- ⏳ `reflection` - Reflection
- ⏳ `unsafe_blocks` - Unsafe blocks
- ⏳ ... (11 more)

## 🚀 Kullanım

Her modül kendi dizininde şunları içerir:
- `module.h` - Interface
- `module.c` - Implementation
- `module_parser.c` - Parser extension
- `module_codegen.c` - Code generation
- `README.md` - Dokümantasyon

## 📝 Yeni Modül Ekleme

1. Uygun kategoriyi seç (core/advanced/experimental)
2. Modül dizini oluştur
3. Template dosyaları kopyala
4. Interface ve implementasyonu yaz
5. Parser ve codegen ekle
6. Test yaz
7. README güncelle

---

*Toplam: 47 modül (22 hazır, 13 kısmi, 12 planlı)*
