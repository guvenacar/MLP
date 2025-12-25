# MLP Self-Hosting Compiler Kullanım Kılavuzu

## Gereksinimler
- GCC
- NASM
- MLP C Compiler (c_compiler/mlpc)

## Derleme Adımları

```bash
cd /home/pardus/projeler/tyd-lang/MLP

# 1. MLP compiler'ı derle
./c_compiler/mlpc self_host/mlpc.mlp /tmp/mlpc.asm

# 2. Assembly'yi object dosyasına çevir
nasm -f elf64 /tmp/mlpc.asm -o /tmp/mlpc.o

# 3. Çalıştırılabilir dosya oluştur
gcc -no-pie /tmp/mlpc.o runtime/runtime.o -lm -o /tmp/mlpc

# 4. Çalıştır (assembly kodunu stdout'a yazar)
/tmp/mlpc
```

## Tek Satırda Derleme

```bash
./c_compiler/mlpc self_host/mlpc.mlp /tmp/mlpc.asm && nasm -f elf64 /tmp/mlpc.asm -o /tmp/mlpc.o && gcc -no-pie /tmp/mlpc.o runtime/runtime.o -lm -o /tmp/mlpc && /tmp/mlpc
```

## Üretilen Kodu Çalıştırma

```bash
# Assembly çıktısını dosyaya kaydet
/tmp/mlpc > /tmp/output.asm

# Derle ve çalıştır
nasm -f elf64 /tmp/output.asm -o /tmp/output.o
gcc -no-pie /tmp/output.o -o /tmp/output
/tmp/output  # Sonuç: 15
```

## Mevcut Özellikler
- Lexer: Kaynak kodu tokenize eder
- Parser: Tokenları AST'ye çevirir
- Generator: AST'den x86-64 assembly üretir

## Desteklenen Syntax
- `int x = 10;` - Değişken tanımlama
- `print x + 5` - Print ve toplama

## Dosyalar
- `self_host/mlpc.mlp` - Ana compiler
- `self_host/lexer.mlp` - Lexer modülü
- `self_host/parser.mlp` - Parser modülü
- `self_host/generator.mlp` - Generator modülü
