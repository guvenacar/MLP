# Program Module

**Durum:** ✅ Aktif  
**Oluşturulma:** 2 Aralık 2025  

## Amaç

Program modülü, MELP derleyicisinin ana kod üretim yapısını organize eder. Assembly çıktısının `.data`, `.bss`, ve `.text` bölümlerini yönetir ve tüm bileşenleri bir araya getirir.

## Özellikler

### 1. Data Section Generation (`codegen_data_section`)
- String sabitleri (declarations'dan)
- Print string literals
- Expression string literals

### 2. BSS Section Generation (`codegen_bss_section`)
- Global değişken tanımları
- For döngü değişkenleri
- Function parametre storage
- Statement body declarations

### 3. Text Section Generation (`codegen_text_section`)
- CLI/IO helper fonksiyonları
- String helper fonksiyonları
- Kullanıcı tanımlı fonksiyonlar

### 4. _start Entry Point
- Global değişken başlatma
- Ana program statements
- Program exit kodu

## Modülerleşme

Bu modül sayesinde `codegen.c`:
- **528 satır** → **109 satır** (%79 azalma)
- Sadece router ve global state yönetimi
- Tüm implementasyon detayları modüllere taşındı

## Kullanım

```c
#include "modules/program/program_codegen.h"

// Main code generation
void codegen_generate(Program* program, const char* output_file) {
    // ... setup ...
    
    // Delegate to program module
    codegen_data_section(f, program);
    codegen_bss_section(f, program);
    codegen_text_section(f, program);
    
    // Generate _start
    fprintf(f, "\n_start:\n");
    codegen_initialize_declarations(f, program);
    codegen_main_statements(f, program);
    codegen_program_exit(f);
}
```

## Bağımlılıklar

- `variable_codegen` - Variable declarations
- `string_ops_codegen` - String literal collection
- `print_codegen` - Print string collection
- `cli_io_codegen` - CLI/IO helpers
- `function_codegen` - Function definitions
- `arithmetic_codegen` - Expression code generation

## Bileşenler

| Dosya | Satır | Açıklama |
|-------|-------|----------|
| `program.h` | 7 | Minimal header |
| `program.c` | 3 | Placeholder |
| `program_parser.h` | 8 | Parser interface |
| `program_parser.c` | 4 | Minimal stub |
| `program_codegen.h` | 28 | Code generation interface |
| `program_codegen.c` | 195 | Implementation |

**Toplam:** ~245 satır (codegen.c'den taşınan mantık)

## Etki

Bu modül ile:
- ✅ codegen.c minimal router haline geldi
- ✅ Assembly section generation mantığı ayrıştı
- ✅ Her section bağımsız test edilebilir
- ✅ Kod okunabilirliği arttı
- ✅ Modüler mimari tamamlandı
