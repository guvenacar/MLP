# MLP Self-Hosting Handoff - 30 Kasım 2025

## Proje Durumu

MLP (MELP) kendi kendini derleyen bir programlama dili. Bootstrap derleyici C'de yazılmış ve çalışıyor.

## Önemli Dosyalar

### Dökümantasyon
- `MLP_LANGUAGE_SPEC.md` - Tam dil spesifikasyonu (~1500 satır)
- `SELF_HOSTING_PLAN.md` - Self-hosting stratejisi ve yol haritası

### Bootstrap Derleyici (C)
- `melp/bootstrap/lexer.c` - Lexer
- `melp/bootstrap/parser.c` - Parser  
- `melp/bootstrap/codegen.c` - Code generator (4670 satır, çalışıyor)
- `melp/bootstrap/codegen/` - Modüler yapı (24 dosya, referans için)
- `melp/runtime/runtime.c` - C runtime

### Self-Hosting (MLP)
- `melp/selfhost/lexer.mlp` - MLP'de yazılmış lexer ✅ Çalışıyor
- `melp/selfhost/parser.mlp` - Henüz yok ⏳
- `melp/selfhost/codegen.mlp` - Henüz yok ⏳

## Derleme Komutları

```bash
# MLP programı derle
cd melp
./melp-bootstrap program.mlp output.s
nasm -f elf64 output.s -o output.o
ld output.o runtime/runtime.o -o program -lc -lm -dynamic-linker /lib64/ld-linux-x86-64.so.2

# Self-hosting lexer test
echo "numeric x = 5" | ./selfhost/lexer_test
```

## Sonraki Adımlar

1. **Parser.mlp** - MLP'de recursive descent parser yaz
2. **Codegen.mlp** - MLP'de x86-64 assembly üretici yaz
3. **Bootstrap** - MLP derleyicisini kendi kendini derleyecek hale getir

## Teknik Notlar

- TTO (Transparent Type Optimization): int64/double/BigDecimal otomatik seçim
- State variables: Global değişkenler `state` keyword ile
- Modül sistemi: `module X ... end module`, `import X`
- Exception handling: `try/catch/finally`, `throw`
- Generics: `function<T> swap(T a, T b)`

## Git Branch
`melp20241229` - Ana geliştirme branch'i
