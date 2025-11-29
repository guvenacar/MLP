# MELP Bootstrap Compiler - Güncel Durum (29 Kasım 2025)

## ✅ Tamamlanan Özellikler

### Phase 0-12: Turing Complete + Ekstralar
- Variables (numeric, decimal, boolean, text)
- Assignment ve aritmetik
- Karşılaştırma operatörleri (>, <, >=, <=, ==, !=)
- **AND/OR/NOT operatörleri** ✨
- **ELSE-IF chains** - YENİ EKLENDİ ✨ (Phase 10)
- **SWITCH/CASE statements** - YENİ EKLENDİ ✨ (Phase 10)
- **DO-WHILE loops** - YENİ EKLENDİ ✨ (Phase 10)
- IF/ELSE (tek seviye)
- WHILE ve FOR döngüleri
- Fonksiyonlar (parametreler, return)
- Diziler (dinamik, resize, length)
- String operasyonları (concat, substring, indexOf, charAt)
- File I/O (read, write, append, exists)
- CLI arguments (argc, argv)
- Pointer desteği (başlangıç seviyesi)

### Phase 10 Yeni Özellikler (29 Kasım 2025)

#### 1. Else-If Chains ✅
```mlp
if x < 5 then
    print(1)
else if x < 10 then
    print(2)
else if x < 15 then
    print(3)
else
    print(4)
end if
```

**Test:**
```bash
./melp-bootstrap test_elseif_multi.mlp out.s
nasm -f elf64 out.s -o out.o
ld out.o runtime/runtime.o -o test -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2
./test  # Output: 2, 30, 400, 999
```

#### 2. Switch/Case Statements ✅
```mlp
switch x
    case 1
        print(10)
    case 2
        print(20)
    case 3
        print(30)
    default
        print(999)
```

**Özellikler:**
- Multiple case support
- Default case
- Automatic break (no fall-through)

**Test:**
```bash
./melp-bootstrap test_switch.mlp out.s
# x=2 → 20, y=5 → 999 (default)
```

#### 3. Do-While Loops ✅
```mlp
numeric i = 0
do
    print(i)
    i = i + 1
while i < 3
# Output: 0, 1, 2 (en az 1 kez çalışır)
```

**Özellikler:**
- Post-condition check (body en az 1 kez çalışır)
- Nested do-while support
- Same syntax as traditional do-while

**Test:**
```bash
./melp-bootstrap test_dowhile_comprehensive.mlp out.s
# Tests: basic, single execution, nested, sum
```

### Logical Operatörler Detayı
```mlp
# AND çalışıyor ✅
if x > 3 and x < 10 then
    print(1)
end if

# OR çalışıyor ✅
if x < 0 or x > 100 then
    print(1)
end if

# NOT çalışıyor ✅
if not flag then
    print(1)
end if
```

## 🚧 Eksik/Yapılacak Özellikler

### Yüksek Öncelikli
1. **Nullable Types (?)** - Optional values with null safety
2. **Enums** - Named constant sets  
3. **Type aliases** - Custom type names

### Orta Öncelikli
4. **Multiple return values** - Tuple-like returns
5. **Short-circuit evaluation** - AND/OR için (performans optimizasyonu)
6. **Debug features** - debug goto, debug pause, debug label

### Düşük Öncelikli (Defer)
7. **Lambda** - Anonymous functions (complex implementation)
8. **Try/catch** - Exception handling (requires runtime support)
9. **Modules** - Code organization (large architectural change)
10. **Async/await** - Concurrency (requires runtime scheduler)

## 📁 Proje Yapısı

```
melp/
├── melp-bootstrap          # Ana executable (C bootstrap compiler)
├── bootstrap/
│   ├── lexer.c            # Tokenizer - AND/OR/NOT/ELSE_IF tokenları var ✅
│   ├── parser.c           # Parser - Logical expr parse ediliyor ✅, ELSE IF yok ❌
│   ├── codegen.c          # Assembly generator - Logical ops codegen var ✅
│   └── bootstrap.c        # Main entry point
├── runtime/
│   ├── runtime.c          # Runtime fonksiyonları (print, file I/O, arrays)
│   └── runtime.o          # Compiled runtime
└── tests/                 # Test dosyaları
```

## 🔧 Build Sistemi

**Bootstrap compiler'ı rebuild etmek:**
```bash
cd /home/pardus/projeler/MLP/MLP/melp
make
```

**Test programı derlemek:**
```bash
./melp-bootstrap input.mlp output.s
nasm -f elf64 output.s -o output.o
gcc output.o runtime/runtime.o -o program -nostartfiles
./program
```

**ÖNEMLİ:** GCC ile linklerken `-nostartfiles` bayrağı gerekli (assembly'de `_start` var).

## 🎯 Bir Sonraki Adım

**Öneri:** ELSE IF ekle, sonra self-hosting'e geç.

**Neden bu sırayla?**
1. ELSE IF basit bir eklenti (parser seviyesi)
2. Test etmesi kolay
3. Self-hosting için core features tamamlanmış olur

**Self-hosting için hazırlık:**
- C bootstrap compiler artık robust
- Tüm temel özellikler çalışıyor
- Test suite mevcut
- `compiler/mlp_compiler.mlp` dosyasını güncelleyerek başlanabilir

## 📝 Notlar

- Token limit nedeniyle YZ değişimi yapıldı
- Kodun güncel hali stable ve çalışıyor
- Test dosyaları: `test_*.mlp` pattern'i kullan
- Assembly output: `test_*.s`
- Executable: `test_*_exec` veya `test_*` (link sonrası)

## 🐛 Bilinen Sorunlar

- Yok! AND/OR/NOT düzgün çalışıyor ✅
- ELSE IF beklenen özellik, bug değil

---

**Tarih:** 29 Kasım 2025  
**Branch:** melp20251128  
**Son Test:** test_simple_and.mlp - BAŞARILI ✅
