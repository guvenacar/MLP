# 📋 MLP Derleyici TODO Listesi
**Oyuncaktan Gerçek Derleyiciye Dönüşüm Planı**

Tarih: 5 Aralık 2025  
Durum: Stage 0 Prototype → Stage 1 Functional Compiler

---

## 🎯 ÖNCELİK 1: Print Modülünü Çalıştır (KRİTİK)

### ❌ TODO 1.1: Print Parser'ı İmplement Et
**Dosya:** `melp/C/stage0/modules/print/print_parser.c`

**Şu an:** Sadece operator sayıyor
```c
// Mevcut: Sadece +, -, *, / sayıyor
if(t.type==TK_OP) { printf("✓ Op: %s\n", t.val); }
```

**Olması gereken:**
```c
// Parse: print("Merhaba Dünya!")
// 1. "print" keyword'ünü yakala
// 2. "(" bul
// 3. String literal'i parse et: "Merhaba Dünya!"
// 4. ")" bul
// 5. PrintStatement struct'ı döndür
```

**Gerekli struct:**
```c
typedef struct {
    char string_content[256];  // "Merhaba Dünya!"
    int line_number;
} PrintStatement;

PrintStatement* parse_print_statement();
```

**Test:**
```bash
./print_compiler test.mlp out.s
# İçinde: print("Test")
# Beklenen: PrintStatement{content="Test"}
```

---

### ❌ TODO 1.2: Print Codegen'i İmplement Et
**Dosya:** `melp/C/stage0/modules/print/print_codegen.c`

**Şu an:** Sabit "Print OK!" yazıyor
```c
fprintf(out,"section .data\n  msg: db 'Print OK!',10,0\n...");
```

**Olması gereken:**
```c
void generate_print_statement(FILE* out, PrintStatement* stmt) {
    // String'i .data section'a yaz
    fprintf(out, "section .data\n");
    fprintf(out, "    str_%d: db \"%s\", 10, 0\n", 
            stmt->line_number, stmt->string_content);
    fprintf(out, "    str_%d_len: equ $ - str_%d\n", 
            stmt->line_number, stmt->line_number);
    
    // sys_write assembly kodu
    fprintf(out, "\nsection .text\n");
    fprintf(out, "    mov rax, 1\n");              // sys_write
    fprintf(out, "    mov rdi, 1\n");              // stdout
    fprintf(out, "    lea rsi, [str_%d]\n", stmt->line_number);
    fprintf(out, "    mov rdx, str_%d_len\n", stmt->line_number);
    fprintf(out, "    syscall\n");
}
```

**Test:**
```bash
./print_compiler "print(\"Hello\")" out.s
nasm -f elf64 out.s -o out.o
ld out.o -o prog
./prog
# Çıktı: Hello
```

---

### ❌ TODO 1.3: Print Modülü Entegre Test
**Test dosyası oluştur:** `test_print_real.mlp`
```mlp
print("Merhaba Dünya!")
print("İkinci satır")
print("Türkçe: ğüşıöç")
```

**Beklenen çıktı:**
```
Merhaba Dünya!
İkinci satır
Türkçe: ğüşıöç
```

**Komut:**
```bash
./melp/C/stage0/modules/print/print_compiler test_print_real.mlp test.s
nasm -f elf64 test.s -o test.o
ld test.o -o test_prog
./test_prog
```

---

## 🎯 ÖNCELİK 2: Modül Entegrasyonu

### ❌ TODO 2.1: mlp_compile.sh'yi Düzelt
**Dosya:** `mlp_compile.sh`

**Sorun:** Son aşamada sabit kodlu assembly yazıyor
```bash
# ŞU AN (YANLIŞ):
cat > "$TEMP_DIR/main.s" << 'EOF'
section .data
    msg: db "MLP Program Running", 10, 0  # SABİT!
```

**Çözüm:** Modül çıktılarını birleştir
```bash
# OLMASI GEREKEN:
# 1. Tüm modüllerin .s dosyalarını topla
# 2. .data section'ları birleştir
# 3. .text section'ları birleştir
# 4. _start noktasını ayarla

cat "$TEMP_DIR/variables.s" >> "$TEMP_DIR/final.s"
cat "$TEMP_DIR/print.s" >> "$TEMP_DIR/final.s"
# ... diğer modüller
```

---

### ❌ TODO 2.2: Section Merging İmplement Et
**Yeni script:** `merge_assembly.sh`

```bash
#!/bin/bash
# Assembly dosyalarını birleştir

OUTPUT="$1"
shift
FILES="$@"

echo "section .data" > "$OUTPUT"
for f in $FILES; do
    # Her dosyadan .data section'ı çek
    sed -n '/section \.data/,/section \.text/p' "$f" | 
    grep -v 'section' >> "$OUTPUT"
done

echo "section .text" >> "$OUTPUT"
echo "global _start" >> "$OUTPUT"
echo "_start:" >> "$OUTPUT"
for f in $FILES; do
    # Her dosyadan .text section'ı çek
    sed -n '/section \.text/,$p' "$f" | 
    grep -v 'section\|global\|_start:' >> "$OUTPUT"
done

# Exit syscall ekle
echo "    mov rax, 60" >> "$OUTPUT"
echo "    xor rdi, rdi" >> "$OUTPUT"
echo "    syscall" >> "$OUTPUT"
```

---

## 🎯 ÖNCELİK 3: Diğer Temel Modüller

### ❌ TODO 3.1: Variable Modülünü Tamamla
**Durum:** TTO var ama tam çalışmıyor

**Yapılacaklar:**
- [ ] Gerçek değişken tanımlamalarını assembly'ye çevir
- [ ] TTO optimizasyonunu test et
- [ ] Stack allocation implement et

**Test:**
```mlp
numeric x = 42
numeric y = 3.14
text name = "Ali"
```

---

### ❌ TODO 3.2: Arithmetic Modülünü Tamamla
**Yapılacaklar:**
- [ ] +, -, *, /, % operatörlerini parse et
- [ ] Assembly instruction'ları üret (add, sub, mul, div)
- [ ] Register allocation yap

**Test:**
```mlp
numeric a = 10
numeric b = 20
numeric c = a + b
```

---

### ❌ TODO 3.3: Comments Modülünü Tamamla
**Yapılacaklar:**
- [ ] `---` yorumlarını kaldır
- [ ] `--` satır yorumlarını kaldır
- [ ] Temiz kod çıktısı ver

---

## 🎯 ÖNCELİK 4: Control Flow

### ❌ TODO 4.1: If-Else İmplement Et
**Parser:**
```c
// if x > 10 then ... end if
typedef struct {
    Condition* cond;       // x > 10
    Statement* body;       // ...
    Statement* else_body;  // optional
} IfStatement;
```

**Codegen:**
```asm
    ; if condition
    cmp rax, 10
    jle .else_label
    ; then body
    jmp .endif_label
.else_label:
    ; else body
.endif_label:
```

---

### ❌ TODO 4.2: While Loop İmplement Et
**Parser:**
```c
// while x < 100 then ... end while
typedef struct {
    Condition* cond;
    Statement* body;
} WhileStatement;
```

**Codegen:**
```asm
.loop_start:
    ; check condition
    cmp rax, 100
    jge .loop_end
    ; body
    jmp .loop_start
.loop_end:
```

---

## 🎯 ÖNCELİK 5: Functions

### ❌ TODO 5.1: Function Definition
**Parser:**
```c
// function add(numeric a, numeric b) numeric
typedef struct {
    char name[64];
    Parameter params[10];
    int param_count;
    Type return_type;
    Statement* body;
} FunctionDef;
```

**Codegen:**
```asm
add:
    push rbp
    mov rbp, rsp
    ; function body
    pop rbp
    ret
```

---

### ❌ TODO 5.2: Function Call
**Parser:**
```c
// result = add(5, 3)
typedef struct {
    char func_name[64];
    Expression* args[10];
    int arg_count;
} FunctionCall;
```

**Codegen:**
```asm
    ; push arguments
    mov rdi, 5
    mov rsi, 3
    call add
    ; result in rax
```

---

## 🎯 ÖNCELİK 6: Advanced Features

### ❌ TODO 6.1: Arrays
- [ ] Array declaration: `numeric arr[10]`
- [ ] Array access: `arr[5]`
- [ ] Bounds checking

### ❌ TODO 6.2: Structs
- [ ] Struct definition
- [ ] Member access
- [ ] Memory layout

### ❌ TODO 6.3: Pointers
- [ ] Pointer syntax
- [ ] Dereference
- [ ] Address-of operator

---

## 🛠️ ARAÇLAR VE TESTLER

### ❌ TODO 7.1: Test Framework
**Dosya:** `test_runner.sh`

```bash
#!/bin/bash
# Tüm testleri çalıştır

TESTS_DIR="tests"
PASSED=0
FAILED=0

for test in $TESTS_DIR/*.mlp; do
    echo "Testing: $test"
    ./mlp_compile.sh "$test" /tmp/test_out
    /tmp/test_out > /tmp/actual.txt
    
    expected="${test%.mlp}.expected"
    if diff /tmp/actual.txt "$expected"; then
        echo "✅ PASS"
        ((PASSED++))
    else
        echo "❌ FAIL"
        ((FAILED++))
    fi
done

echo ""
echo "Results: $PASSED passed, $FAILED failed"
```

---

### ❌ TODO 7.2: Test Cases Oluştur
**Dizin:** `tests/`

```
tests/
├── 01_print_simple.mlp
├── 01_print_simple.expected
├── 02_variables.mlp
├── 02_variables.expected
├── 03_arithmetic.mlp
├── 03_arithmetic.expected
├── 04_if_else.mlp
├── 05_while_loop.mlp
├── 06_functions.mlp
└── README.md
```

---

### ❌ TODO 7.3: CI/CD Setup
**Dosya:** `.github/workflows/test.yml`

```yaml
name: MLP Compiler Tests

on: [push, pull_request]

jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y nasm gcc
      - name: Build modules
        run: make all
      - name: Run tests
        run: ./test_runner.sh
```

---

## 📚 DOKÜMANTASYON

### ❌ TODO 8.1: Modül Dokümantasyonu
Her modül için README.md:

```markdown
# Print Module

## Sorumluluk
MLP print() statement'larını x86-64 assembly'e çevirir.

## Input Format
```mlp
print("Hello World")
```

## Output Format
```asm
section .data
    str_1: db "Hello World", 10, 0
section .text
    mov rax, 1
    ...
```

## API
- `parse_print()` - Print statement'ı parse et
- `generate_print()` - Assembly kodu üret

## Tests
./test.sh
```

---

### ❌ TODO 8.2: Kullanım Kılavuzu
**Dosya:** `USAGE.md`

```markdown
# MLP Derleyici Kullanım Kılavuzu

## Kurulum
git clone ...
cd MLP
make all

## Kullanım
./mlp_compile.sh program.mlp output_binary
./output_binary

## Örnekler
Bkz: examples/ dizini
```

---

## 🎯 MİLESTONE'LAR

### Milestone 1: "Hello World" Çalışsın (1 hafta)
- [x] 63 modül altyapısı ✅
- [ ] Print modülü çalışır halde
- [ ] "Hello World" başarıyla çalışıyor
- [ ] Test framework hazır

### Milestone 2: Temel Özellikler (2 hafta)
- [ ] Variables çalışıyor
- [ ] Arithmetic çalışıyor
- [ ] If-else çalışıyor
- [ ] While loops çalışıyor

### Milestone 3: Functions (2 hafta)
- [ ] Function definition
- [ ] Function calls
- [ ] Return values
- [ ] Recursion

### Milestone 4: Advanced (4 hafta)
- [ ] Arrays
- [ ] Structs
- [ ] Pointers
- [ ] Standard library başlangıcı

### Milestone 5: Self-Hosting (8 hafta)
- [ ] MLP ile MLP derleyicisi yazmak
- [ ] Bootstrap süreci

---

## 📊 İLERLEME TAKIBI

```
Modül               Durum    Parser  Codegen  Tests  Entegre
─────────────────────────────────────────────────────────────
print               🔴 0%    ❌      ❌       ❌     ❌
variable            🟡 30%   ✅      🟡       ❌     ❌
arithmetic          🔴 0%    ❌      ❌       ❌     ❌
comments            🔴 0%    ❌      ❌       ❌     ❌
functions           🔴 0%    ❌      ❌       ❌     ❌
control_flow        🔴 0%    ❌      ❌       ❌     ❌
comparison          🔴 0%    ❌      ❌       ❌     ❌
─────────────────────────────────────────────────────────────
TOPLAM              🔴 5%
```

**Renk Kodu:**
- 🔴 0-25%: Başlanmadı/Minimal
- 🟡 26-75%: Devam ediyor
- 🟢 76-100%: Tamamlandı

---

## 🚀 BAŞLARKEN

### Yeni bir AI geliyorsa ilk yapılacak:

1. **Bu dosyayı oku**: `TODO.md`
2. **Mevcut durumu öğren**: 
   ```bash
   ./mlp_compile.sh test_simple_45.mlp test
   ./test  # Ne çıktı görüyorsun?
   ```
3. **İlk görev**: TODO 1.1 - Print Parser
4. **Test et**: Her değişiklikten sonra test çalıştır
5. **Commit et**: Küçük, anlamlı commit'ler

### Geliştirme Döngüsü:
```
1. TODO'dan bir görev seç
2. Kodu yaz
3. Test et
4. Commit et
5. TODO'yu güncelle
6. Repeat
```

---

## 📞 YARDIM

Takıldığın yer:
1. İlgili modülün test dosyasına bak
2. Çalışan assembly örneklerine bak
3. Diğer modüllerin kodunu incele
4. TODO'daki örnekleri kullan

---

**NOT:** Bu TODO sürekli güncellenir. Her tamamlanan görev için:
- [ ] → [x] değiştir
- İlerleme yüzdesini güncelle
- Yeni sorunlar/görevler ekle

**Son güncelleme:** 5 Aralık 2025  
**Günceleyen:** AI Assistant (Claude Sonnet 4.5)  
**Sonraki hedef:** Print modülünü çalıştırmak! 🎯
