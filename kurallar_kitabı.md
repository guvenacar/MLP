# MLP Kurallar Kitabı (AI Agent Referansı)

**Son Güncelleme:** 28 Kasım 2025  
**Versiyon:** 3.0  
**Milestone:** Phase 3 Complete - MELP is Turing-complete! 🎉  
**Amaç:** Tüm kritik proje bilgisini tek merkezi dokümanda toplamak

---

## 📑 İçindekiler

1. [Kritik Kurallar](#kritik-kurallar)
2. [Proje Durumu](#proje-durumu)
3. [MLP Mimarisi](#mlp-mimarisi)
4. [Söz Dizimi Referansı](#söz-dizimi-referansı)
5. [Veri Tipleri](#veri-tipleri)
6. [Kontrol Akışı](#kontrol-akışı)
7. [Lexer Token Birleştirme](#lexer-token-birleştirme)
8. [Bootstrap Süreci](#bootstrap-süreci)
9. [Aktif Görevler](#aktif-görevler)
10. [Sonraki AI'ye Notlar](#sonraki-aiye-notlar)
11. [Hızlı Referans](#hızlı-referans)

---

## 1. Kritik Kurallar

### ⛔ YASAKLAR
- **Repoya PUSH YOK**: Tüm çalışma lokal, `git push` kesinlikle yasak
- **Mevcut Kodu Bozma**: `self_host/` ve `runtime/` klasörleri çalışıyor, elleme
- **Yeni Belgeler Oluşturma**: Tüm bilgi bu dosyaya yazılır, yeni `.md` dosyası oluşturma
- **Belge Okumadan Kod Yazma**: Görev başlamadan önce bu belgeyi tamamen oku

### ✅ ZORUNLULAR
- **Bu Belge Tek Kaynak**: Yeni bilgi eklemek için önce index'e ekle, sonra ilgili bölüme yaz
- **Git Temiz Tut**: Çalışma başında `git status` kontrol et, temiz olmalı
- **Test Et**: Kod değişikliği yaptıktan sonra mutlaka test et
- **Self-Hosting İlkesi**: Compiler MLP'de yazılacak (runtime C'de kalacak)

---

## 2. Proje Durumu

### Genel Durum
- **MELP (Minimal):** Phase 3 tamamlandı - Turing Complete! 🎉
- **Dosya Uzantısı:** `.mlp` (tüm MLP/MELP dosyaları için standart)
- **Bootstrap Compiler:** C ile yazıldı, x86-64 assembly üretiyor
- **Milestone:** Variables, conditionals, loops, arithmetic, I/O çalışıyor

### Dosya Yapısı
```
MLP/
├── kurallar_kitabı.md           # Bu dosya (tek kaynak)
├── todo_user.md                 # Keyword roadmap (Phase 0-12)
├── melp/                        # Yeni minimal MELP projesi
│   ├── bootstrap/               # C'de bootstrap compiler
│   │   ├── lexer.c             # Phase 1: Comments, strings, numbers
│   │   ├── parser.c            # Phase 0: Sadece deklarasyon
│   │   ├── codegen.c           # x86-64 assembly üretir
│   │   └── main.c
│   ├── compiler/                # MELP'te yazılmış compiler
│   │   └── compiler_state.mlp  # Compiler state definitions
│   ├── runtime/                 # Minimal C runtime
│   │   ├── runtime.c           # malloc/free only
│   │   └── runtime.h
│   ├── tests/
│   │   ├── test_simple.mlp     # Phase 0 test
│   │   └── test_phase1.mlp     # Phase 1 test (yorumlar)
│   ├── syntax.json              # Minimal rules
│   ├── diller.json              # tr/en keywords
│   ├── Makefile
│   └── README.md
└── ornekler/                    # Eski dosyalar (arşiv)
    ├── self_host/
    ├── runtime/
    └── ...
```

### Aktif Durum

**Phase 3 Complete! (28 Kasım 2025)** ✅

Tamamlanan Özellikler:
- ✅ Phase 0: Deklarasyonlar (numeric, decimal, boolean)
- ✅ Phase 1: Assignment, print, expressions
- ✅ Phase 2: If/else, comparison operators (==, !=, <, <=, >, >=)
- ✅ Phase 3: For loops (with step), while loops, exit, continue
- ✅ Arithmetic operators: +, -, *, /
- ✅ Nested loops with context tracking

**MELP is now Turing-complete!** 🚀

Sırada:
- ⏳ Debug features (debug goto, debug pause, debug label)
- ⏳ Phase 4: Functions (func/return)
- ⏳ Phase 5+: Structs, arrays, advanced features
- Bootstrap compiler hazır
- İlk self-hosting: `compiler_state.mlp` derlendi

**MELP Phase 1:** 🔄 Devam Ediyor  
- Lexer: ✅ Comments, strings, numbers, operators eklendi
- Parser: ⏳ Assignment desteği eklenmeli
- Codegen: ⏳ print() ve değişken init eklenmeli

---

## 3. MLP Mimarisi

### 3-Aşamalı Pipeline

```
Kullanıcı Kodu (Herhangi Syntax + Herhangi Dil)
    ↓
[Stage 1: Syntax Normalization]
→ syntax_preprocessor.py + syntax.json
→ C {}, Python :, Go → MLP base syntax
    ↓
Normalize MLP (MLP syntax + Herhangi Dil)
    ↓
[Stage 2: Language Translation]
→ mlp_preprocessor.c + diller.json  
→ Turkish/Russian/Hindi → English
    ↓
Pragmatik MLP (MLP syntax + English keywords)
    ↓
[Stage 3: Compilation]
→ Lexer → Parser → Codegen
→ English keywords only
    ↓
x86-64 Assembly (NASM)
```

### Önemli Detaylar
- **Stage 1-2:** Kullanıcının görmesi gerekmiyor, arka planda çalışır
- **Stage 3:** Compiler (lexer/parser/codegen) sadece Pragmatik MLP görür
- **Lexer Girdisi:** "end if" (2 kelime) → Lexer çıktısı: END_IF (1 token)
- **Runtime:** C kütüphaneleri (performance için), değişmeyecek

---

## 4. Söz Dizimi Referansı

### Pragmatik MLP Base Syntax
(Stage 1-2'den sonra compiler'ın gördüğü)

#### Değişken Tanımlama
```mlp
numeric x = 10
string name = "Ali"
boolean flag = true
```

#### Fonksiyon Tanımı
```mlp
function add(numeric a, numeric b) then
    return a + b
end function
```

#### Struct Tanımı
```mlp
struct Person then
    string name
    numeric age
end struct
```

#### Kontrol Akışı
```mlp
if x > 5 then
    print x
end if

while x < 10
    x = x + 1
end while

for i = 0 to 10
    print i
end for
```

#### Liste İşlemleri
```mlp
numeric list() numbers = list()
numbers.add(10)
numbers.add(20)
numeric val = numbers.get(0)
```

### VB.NET-Style Keywords (Çıkış Komutları)
```mlp
exit if         -- if bloğundan çık
exit for        -- for döngüsünden çık
exit while      -- while döngüsünden çık
exit function   -- fonksiyondan çık
exit switch     -- switch bloğundan çık
goto @label     -- etikete atla
stop            -- programı durdur
```

**Not:** `break` ve `continue` YOK. VB.NET felsefesi: herhangi bir bloktan çıkabilirsin.

---

## 5. Veri Tipleri

### BigDecimal-Based Type System
- **numeric:** Tüm sayılar (int/float ayrımı YOK), BigDecimal tabanlı
- **string:** UTF-8 string
- **boolean:** true/false
- **list:** Typeless liste `list()`, generic DEĞİL `list[T]` değil

### Örnek
```mlp
numeric x = 10        -- integer gibi
numeric y = 3.14      -- float gibi
numeric z = x + y     -- Hepsi numeric, sorunsuz topla

string list() names = list()    -- String listesi
```

---

## 6. Kontrol Akışı

### If-Then-Else
```mlp
if condition then
    -- kod
else
    -- kod
end if
```

### While
```mlp
while condition
    -- kod
    if break_condition then
        exit while
    end if
end while
```

### For
```mlp
for i = 0 to 10
    if skip_condition then
        exit for
    end if
    print i
end for
```

### Switch-Case
```mlp
switch value
    case 1
        print "bir"
    case 2
        print "iki"
    default
        print "diğer"
end switch
```

### Goto ve Labels
```mlp
goto @cleanup

@retry
    -- kod
    if error then
        goto @cleanup
    end if

@cleanup
    -- temizlik kodu
```

---

## 7. Lexer Token Birleştirme

### Problem
Kullanıcı "end if" (2 kelime) yazar → Lexer 2 token üretir → Parser karışır

C'de `}` 1 token, MLP'de "end if" 2 token → Parser karmaşıklaşır

### Çözüm: Akıllı Token Birleştirme
Lexer peek-ahead ile çok-kelimeli keywordleri tek token'a birleştirir:

```
"end if"      → END_IF (1 token)
"end while"   → END_WHILE (1 token)
"end for"     → END_FOR (1 token)
"exit for"    → EXIT_FOR (1 token)
"exit while"  → EXIT_WHILE (1 token)
```

### Lexer Algoritması
```mlp
function lex_identifier_or_keyword(string source, numeric pos) then
    -- Kelimeyi oku
    string word = read_word(source, pos)
    
    -- "end" ise sonraki kelimeye bak
    if word == "end" then
        string next = peek_next_word(source, pos)
        if next == "if" then
            return Token("KEYWORD", "END_IF")
        end if
        if next == "while" then
            return Token("KEYWORD", "END_WHILE")
        end if
        -- vs...
    end if
    
    -- "exit" ise sonraki kelimeye bak
    if word == "exit" then
        string next = peek_next_word(source, pos)
        if next == "for" then
            return Token("KEYWORD", "EXIT_FOR")
        end if
        -- vs...
    end if
    
    -- Tek kelime keyword mi?
    if is_single_keyword(word) then
        return Token("KEYWORD", word)
    end if
    
    return Token("IDENTIFIER", word)
end function
```

### Birleştirilecek Keyword Listesi
- **end:** if, while, for, function, struct, enum, switch
- **exit:** if, for, while, function, switch
- **continue:** for, while

---

## 8. Bootstrap Süreci

### Hedef
`self_host/mlpc.mlp` çalışmıyor (codegen hatası), yeni minimal compiler yaz.

### Strateji: Option 2 (Minimal Compiler)
1. **bootstrap_compiler/** klasöründe minimal compiler yaz
2. Mevcut C `mlpc` ile derle
3. Bootstrap test yap (Stage 1 → Stage 2)
4. Çalıştıktan sonra `self_host/mlpc.mlp` düzelt

### Adımlar
```bash
# 1. Lexer yaz (keyword merging ile)
vim bootstrap_compiler/lexer.mlp

# 2. C compiler ile derle
./mlpc bootstrap_compiler/lexer.mlp lexer.asm
nasm -f elf64 lexer.asm -o lexer.o
gcc lexer.o runtime/*.o -o lexer_test

# 3. Test et
./lexer_test < test_input.mlp

# 4. Parser ekle
vim bootstrap_compiler/parser.mlp

# 5. Codegen ekle
vim bootstrap_compiler/codegen.mlp

# 6. Full bootstrap
./mlpc bootstrap_compiler/compiler.mlp mlpc_stage1.asm
nasm -f elf64 mlpc_stage1.asm -o mlpc_stage1.o
gcc mlpc_stage1.o runtime/*.o -o mlpc_stage1

./mlpc_stage1 bootstrap_compiler/compiler.mlp mlpc_stage2.asm
diff mlpc_stage1.asm mlpc_stage2.asm  # Aynı olmalı
```

---

## 9. Aktif Görevler

### 🔴 P0: Bootstrap Compiler Lexer
**Durum:** Yapım aşamasında  
**Dosya:** `bootstrap_compiler/lexer.mlp`  
**Görev:** Keyword merging mantığını implement et

**Checklist:**
- [ ] `peek_next_word()` helper fonksiyonu
- [ ] `lex_identifier_or_keyword()` içinde token birleştirme
- [ ] Tüm "end X", "exit X", "continue X" kombinasyonları
- [ ] Test: "end if" → END_IF (1 token)
- [ ] C compiler ile derleme testi

**Keyword Combinations to Merge:**
```
end + if       → END_IF
end + while    → END_WHILE
end + for      → END_FOR
end + function → END_FUNCTION
end + struct   → END_STRUCT
end + enum     → END_ENUM
end + switch   → END_SWITCH

exit + if       → EXIT_IF
exit + for      → EXIT_FOR
exit + while    → EXIT_WHILE
exit + function → EXIT_FUNCTION
exit + switch   → EXIT_SWITCH

continue + for   → CONTINUE_FOR
continue + while → CONTINUE_WHILE
```

### 🟡 P1: Bootstrap Compiler Parser
**Durum:** Bekliyor (lexer bittikten sonra)  
**Dosya:** `bootstrap_compiler/parser.mlp`  
**Görev:** Token stream → AST

**Design Notes:**
- Tek token görecek: END_IF, END_WHILE, etc. (lexer merge etti)
- Recursive descent parser
- Minimal AST nodes: Function, Statement, Expression
- İlk hedef: Basit fonksiyon/if/while parse et

### 🟡 P2: Bootstrap Compiler Codegen
**Durum:** Bekliyor (parser bittikten sonra)  
**Dosya:** `bootstrap_compiler/codegen.mlp`  
**Görev:** AST → x86-64 assembly

**Design Notes:**
- NASM format assembly
- C runtime fonksiyonlarını çağır (print, list_add, etc.)
- Stack-based expression evaluation
- Function prologue/epilogue (rbp, rsp)

### 🟢 P3: TODO-001 Fix
**Durum:** Bootstrap tamamlandıktan sonra  
**Görev:** `self_host/mlpc.mlp` codegen hatasını çöz

**Known Issues:**
- Assembly'de "Struct 'Numeric' tanımlı değil" hatası
- HashMap/List function call karışıklığı
- Import parsing sorunu

---

## 10. Sonraki AI'ye Notlar

### 🎯 Ana Hedef
%100 self-hosting: MLP compiler tamamen MLP'de yazılacak, C runtime kalacak.

### ⚠️ Dikkat Edilecekler
1. **Token Birleştirme Kritik:** "end if" → END_IF mantığı lexer'da olmalı
2. **VB.NET Felsefesi:** `break`/`continue` değil, `exit for`/`exit while` kullan
3. **BigDecimal:** `int`/`float` ayrımı yok, hepsi `numeric`
4. **Typeless Lists:** `list[T]` değil, `T list()` kullan
5. **C Runtime Kalacak:** gc.c, simple_runtime.c, thread_pool.c değişmeyecek

### ⚠️ Dikkat Edilecekler

1. **Token Birleştirme Kritik:** "end if" → END_IF mantığı lexer'da olmalı
2. **VB.NET Felsefesi:** `break`/`continue` değil, `exit for`/`exit while` kullan
3. **BigDecimal:** `int`/`float` ayrımı yok, hepsi `numeric`
4. **Typeless Lists:** `list[T]` değil, `T list()` kullan
5. **C Runtime Kalacak:** gc.c, simple_runtime.c, thread_pool.c değişmeyecek
6. **Ana Dizin Temiz:** Tüm eski belgeler `archive/` klasöründe, gereksiz dosya ekleme

### 📚 Diğer Önemli Belgeler

Ana dizinde bulunan kritik belgeler (kurallar_kitabı.md dışında):

- **AI_RULES.md** - Türkçe AI kuralları (eski, temel bilgi için bak)
- **AI_GUIDELINES.md** - İngilizce teknik kurallar (eski, temel bilgi için bak)
- **SPECS.md** - Dil spesifikasyonu (referans için, değiştirme)
- **ROADMAP.md** - Proje yol haritası (eski phase bilgileri)
- **TODO_CURRENT.md** - Aktif görevler (eski, TODO-001 bilgisi)
- **README.md** - Proje tanıtımı
- **API_REFERENCE.md** - Runtime fonksiyon referansı
- **mlp_soz_dizimi.md** - Kullanıcı syntax dökümanı

**Not:** Bu belgeler bilgi için referans edilebilir ama `kurallar_kitabı.md` önceliklidir.

### 📊 Progress Tracking
Eğer karmaşık multi-step görev yapıyorsan:
- `manage_todo_list` tool'u kullan
- Her adımı işaretle (not-started → in-progress → completed)
- Unutma: Bir adım bitmeden diğerine geçme

### 🚫 Yapılmaması Gerekenler
- Yeni `.md` dokümantasyon dosyası oluşturma (bu dosya yeterli)
- Repoya push
- `self_host/` klasörünü silme/bozma
- Syntax'ı değiştirme (SPECS.md'de belirtilmedikçe)

### 📝 Bilgi Ekleme Prosedürü
1. Bu dosyayı aç
2. İçindekiler'e yeni başlık ekle
3. İlgili bölümü oluştur/güncelle
4. Değişiklikleri kaydet

**Örnek:**
```markdown
## 📑 İçindekiler
...
11. [Yeni Konu](#yeni-konu)  <-- Buraya ekle
...

## 11. Yeni Konu                <-- Sonra buraya içerik yaz

İçerik...
```

---

## Versiyon Geçmişi

- **v1.0** (28 Kasım 2025): İlk sürüm, tüm kritik bilgi konsolide edildi
- **v1.1** (28 Kasım 2025): Ana dizin temizliği - 31 gereksiz belge `archive/` klasörüne taşındı

---

## 11. Hızlı Referans

### Komutlar (Cheat Sheet)

```bash
# Compiler test
./mlpc test.mlp test.asm

# Bootstrap compile
./mlpc bootstrap_compiler/lexer.mlp lexer.asm
nasm -f elf64 lexer.asm -o lexer.o
gcc lexer.o runtime/*.o -o lexer_test -lm

# Git durumu
git status
git diff

# Dosya arama
find . -name "*.mlp" | grep -v test
grep -r "function.*then" bootstrap_compiler/

# Runtime object'leri
ls -lh runtime/*.o
```

### Token Type Listesi

| Type | Examples |
|------|----------|
| KEYWORD | if, then, end, while, for, function, struct |
| MERGED_KEYWORD | END_IF, END_WHILE, EXIT_FOR, CONTINUE_WHILE |
| IDENTIFIER | myVar, calculate, Person |
| NUMBER | 42, 3.14, 0.001 |
| STRING | "hello", "world" |
| OPERATOR | +, -, *, /, ==, !=, <=, >= |
| EOF | (end of file) |

### C Runtime Functions (Referans)

```c
// gc.h
void* gc_malloc(size_t size);
void gc_free(void* ptr);
void gc_collect();

// simple_runtime.c
void mlp_print(const char* str);
void mlp_print_num(double num);
void mlp_print_bool(int val);

// List operations
void* mlp_list_create();
void mlp_list_add(void* list, void* item);
void* mlp_list_get(void* list, int index);
int mlp_list_size(void* list);

// String operations
char* mlp_string_concat(const char* a, const char* b);
int mlp_string_length(const char* str);
char* mlp_string_substr(const char* str, int start, int len);
```

### Assembly Template (Minimal)

```nasm
section .data
    ; String literals here

section .text
    global main
    extern mlp_print
    extern mlp_print_num
    extern gc_malloc

main:
    push rbp
    mov rbp, rsp
    sub rsp, 16        ; Local variables
    
    ; Your code here
    
    mov rsp, rbp
    pop rbp
    xor rax, rax       ; return 0
    ret
```

---

**NOT:** Bu belge dışında dokümantasyon oluşturma. Tüm güncellemeler buraya.

---

## 📖 Hızlı Başlangıç (Yeni AI Agent İçin)

### İlk 5 Dakika
1. `cat kurallar_kitabı.md` - Bu dosyayı oku (5 dk)
2. `git status` - Repo temiz mi kontrol et
3. `ls bootstrap_compiler/` - Aktif çalışma alanını gör
4. Kullanıcıya sor: "Hangi görevi yapmamı istersiniz?"

### Test Komutları
```bash
# C compiler çalışıyor mu?
./mlpc --version

# Basit test derle
echo 'function main() then print "test" end function' > /tmp/test.mlp
./mlpc /tmp/test.mlp /tmp/test.asm
cat /tmp/test.asm | head -20

# Runtime nesneleri var mı?
ls -lh runtime/*.o
```

### Dosya Ağacı (Özet)
```
MLP/
├── kurallar_kitabı.md          ← SEN BURADASIN
├── mlpc                         ← C compiler (çalışıyor)
├── bootstrap_compiler/          ← Yeni compiler (yapılıyor)
│   └── lexer.mlp               ← Keyword merger ekle
├── self_host/                   ← Eski MLP compiler (bozuk)
│   ├── mlpc.mlp                ← Codegen hatası var
│   └── compiler_core/*.mlp     ← 17 modül
└── runtime/                     ← C runtime (elleme)
    ├── gc.c
    ├── simple_runtime.c
    └── thread_pool.c
```
