# Arşivlenmiş Merkezi Parser ve Codegen Dosyaları

Bu dizin, modüler yapıya geçiş öncesi kullanılan merkezi parser ve codegen dosyalarını içerir.

## Dosyalar

### C Implementasyonu
- **parser_C.c** (602 satır) - Merkezi parser dosyası
  - `parser_create()`, `parser_free()`, `advance()`
  - `parse_declaration()` - Değişken tanımlamaları
  - Modül bazlı parserleri koordine eder
  - Kaynak: `melp/C/stage0/parser.c`

- **codegen_C.c** (131 satır) - Merkezi codegen dosyası  
  - Ana code generation koordinatörü
  - Modül bazlı codegen'leri yönetir
  - Kaynak: `melp/C/stage0/codegen.c`

### MLP Self-hosting
- **lexer.mlp** (272 satır) - Lexer implementasyonu
  - Token üretimi
  - Kaynak: `melp/melp/lexer.mlp`

## Modüler Mimari Geçişi

**Tarih**: 2 Aralık 2025

**Neden Arşivlendi:**
- Tüm özellikler artık modül bazlı parser/codegen dosyalarında
- Her modül kendi `[module]_parser.c/.mlp` ve `[module]_codegen.c/.mlp` dosyalarına sahip
- 33 aktif modül, tamamen modüler yapı
- Merkezi dosyalar artık koordinasyon için minimal rol oynuyor

**Modüler Yapı:**
```
melp/C/stage0/modules/[module]/
    ├── [module]_parser.c/.h
    ├── [module]_codegen.c/.h
    └── [module].h

melp/melp/[module]/
    ├── [module]_parser.mlp
    ├── [module]_codegen.mlp
    └── [module].mlp (opsiyonel helper)
```

**Aktif Modüller (33):**
- arithmetic, array, async, cli_io, collections, comments, comparison
- concurrency, control_flow, debug_features, enum, exception_handling
- file_io, function, generator, generic_types, interface_trait, lambda
- logical, memory, module_system, null_safety, operator_overloading
- pattern_matching, pointer, preprocessor, print, reflection
- state_management, string_ops, struct, switch_match, variable

## Referanslar

Bu dosyalar tarihsel referans ve debug amaçlı saklanmaktadır.
Aktif geliştirme için modül bazlı dosyaları kullanın.

**Daha fazla bilgi:**
- `user/modüller.md` - 253 özellik detayları (158 ✅, %63)
- `user/özellik_listesi.md` - 33 bileşen listesi
- `MODULAR_STRUCTURE.md` - Modüler mimari dokümantasyonu
