# MELP Stage 0 - C Bootstrap Compiler

## ⚠️ DURUMU

**ÖNCE OKU:** `/home/pardus/projeler/MLP/MLP/AI_HANDOFF_CRITICAL.md`

Bu dizin çalışan bir compiler içeriyor AMA yapısal sorunları var:
- `codegen.c` 874 satır ve MONOLİTİK
- `modules/` dizini var ama çoğu kullanılmıyor
- Refactoring gerekiyor

## Derleme

```bash
make clean && make
```

## Test

```bash
./melpc test_string_all.mlp output.s
nasm -f elf64 output.s -o output.o
ld output.o -o test
./test
```

## Dosya Yapısı

```
stage0/
├── main.c              # Giriş noktası
├── lexer.c/h           # Tokenizer
├── parser.c/h          # AST üretici
├── statement_parser.c/h # Statement parser
├── codegen.c/h         # KOD ÜRETİCİ (874 satır - SORUN!)
└── modules/            # Modüller (çoğu kullanılmıyor)
```

## Çalışan Özellikler

- ✅ Variable declarations (numeric, string, boolean)
- ✅ Arithmetic operations (+, -, *, /, %)
- ✅ Comparison operators (==, !=, <, >, <=, >=)
- ✅ Logical operators (AND, OR)
- ✅ Control flow (if/else, for, while)
- ✅ Functions (definition, call, return, recursive)
- ✅ String functions (length, concat, substr, char_at)
- ✅ Print statement

## Öncelik

**Refactoring:** codegen.c'yi parçala, modüllere taşı.

Detaylar için: `AI_HANDOFF_CRITICAL.md`
