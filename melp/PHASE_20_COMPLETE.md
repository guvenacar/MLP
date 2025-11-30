# PHASE 20 COMPLETE - Built-in Functions

**Tarih:** 30 Kasım 2025  
**Durum:** ✅ TAMAMLANDI

## Eklenen Özellikler

### 1. Type Conversion Functions ✅
- `to_numeric(text)` - String'i sayıya çevirir: `to_numeric("42")` → 42
- `to_text(numeric)` - Sayıyı string'e çevirir: `to_text(42)` → "42"
- `to_decimal(text)` - String'i ondalıklı sayıya çevirir
- `parse_int(text)` - Integer parse (hata kontrolü ile)
- `parse_float(text)` - Float parse (hata kontrolü ile)

### 2. Input Functions ✅
- `input()` - stdin'den satır okur
- `input(prompt)` - Prompt ile satır okur

### 3. Math Functions ✅
- `sin(x)`, `cos(x)`, `tan(x)` - Trigonometrik
- `sqrt(x)` - Karekök
- `pow(base, exp)` - Üs alma
- `abs(x)` - Mutlak değer
- `floor(x)`, `ceil(x)`, `round(x)` - Yuvarlama
- `log(x)`, `log10(x)`, `exp(x)` - Logaritma/üstel
- `fmod(x, y)` - Mod (ondalıklı)
- `pi()`, `e()` - Sabitler

### 4. String Functions ✅
- `trim(str)` - Boşlukları kırp
- `replace(str, old, new)` - Değiştir
- `to_upper(str)`, `to_lower(str)` - Büyük/küçük harf
- `starts_with(str, prefix)` - Başlangıç kontrolü
- `ends_with(str, suffix)` - Bitiş kontrolü
- `contains(str, substr)` - İçerme kontrolü
- `split(str, delim)` - Böl (array döner)

### 5. Bitwise Operations ✅
- `band(a, b)` - Bitwise AND
- `bor(a, b)` - Bitwise OR
- `bxor(a, b)` - Bitwise XOR
- `bnot(a)` - Bitwise NOT
- `shl(a, b)` - Shift left
- `shr(a, b)` - Shift right (signed)
- `ushr(a, b)` - Shift right (unsigned)

### 6. Assert Function ✅
- `assert(condition, message)` - Assertion

## Değiştirilen Dosyalar

1. `runtime/runtime.h` - Yeni fonksiyon bildirimleri
2. `runtime/runtime.c` - Yeni fonksiyon implementasyonları
3. `bootstrap/codegen.c` - Extern declarations, builtins list, function mappings

## Test Dosyaları

- `test_phase20.mlp` - Type conversion, math, string, assert testleri
- `test_bitwise.mlp` - Bitwise operatör testleri

## Sonraki Adımlar

- [ ] TTO Numeric (int64/double/BigDecimal)
- [ ] TTO Text (SSO)
- [ ] Union Types

## Derleme Komutu

```bash
cd melp
gcc -o melp-bootstrap bootstrap/main.c runtime/runtime.c -lm
```

## Test Komutu

```bash
./melp-bootstrap test_phase20.mlp test.s && \
nasm -f elf64 test.s -o test.o && \
ld -o test test.o runtime/runtime.o -lc -lm --dynamic-linker /lib64/ld-linux-x86-64.so.2 && \
./test
```
