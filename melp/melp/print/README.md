# Print Module

## Açıklama
Print statement'ları için kod üretimi modülü. MLP'de `print` statement'larını x86-64 assembly'ye çevirir.

## Dosyalar

### C Versiyonu (Bootstrap)
- `print.h` - Print AST tipleri
- `print_parser.h/c` - Print parsing (statement_parser.c'de implementasyonu var)
- `print_codegen.h/c` - Print code generation

### MLP Versiyonu (Self-hosting)
- `print.mlp` - MLP ile yazılmış print code generator

## Fonksiyonlar

### `codegen_print_statement(file, print_stmt)`
Print statement için assembly kodu üretir. Farklı expression tiplerini handle eder:
- String literal → doğrudan sys_write syscall
- Numeric/Boolean değişken → print_int helper çağrısı
- String değişken → print_string helper çağrısı
- Function call → return type'a göre uygun helper çağrısı

### `collect_print_strings(file, stmts, count, str_counter)`
Print statement'larındaki string literallerini toplar ve .data section'a yazar.
Recursively tüm control flow yapılarını (if/while/for/function) tarar.

## Örnek Kullanım

```mlp
-- MLP kaynak kod
numeric x = 42
print(x)
print("Hello, World!")
```

```asm
; Üretilen assembly
; print(x)
    ; print numeric variable
    mov rax, [var_x]
    call print_int

; print("Hello, World!")
    ; print string literal
    mov rax, 1          ; sys_write
    mov rdi, 1          ; stdout
    mov rsi, print_str_0
    mov rdx, print_str_0_len
    syscall

section .data
    print_str_0 db "Hello, World!", 10, 0
    print_str_0_len equ $ - print_str_0
```

## Bağımlılıklar
- `arithmetic` modülü - codegen_expression() için
- `variable` modülü - get_var_type() için
- `cli_io` modülü - print_int/print_string helper fonksiyonları için

## Notlar
- Print string literal'leri .data section'da saklanır
- Newline (10) otomatik eklenir
- String değişkenler runtime'da print_string helper ile yazdırılır
- Numeric değişkenler print_int helper ile integer'a çevrilip yazdırılır
