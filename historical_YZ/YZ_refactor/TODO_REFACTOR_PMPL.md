# 🔧 PMPL ARCHITECTURE REFACTOR - TODO

**Tarih:** 14 Aralık 2025  
**Durum:** ACTIVE  
**Süre Tahmini:** 7-10 saat (3 YZ)

---

## 🎯 REFACTOR AMACI

MELP compiler'ın PMPL (Pragmatic MLP) mimarisini restore etmek.

**PMPL NEDİR?**
PMPL, MELP compiler'ının gördüğü **TEK VE BİRİCİK** syntax'tır.

### PMPL Kuralları (EZBERLE!)

#### 1. Block Terminators (Underscore ile birleşik keyword)
```pmpl
end_if          ← Tek keyword (TOKEN_END_IF)
end_while       ← Tek keyword (TOKEN_END_WHILE)
end_for         ← Tek keyword (TOKEN_END_FOR)
end_function    ← Tek keyword (TOKEN_END_FUNCTION)
end_struct      ← Tek keyword (TOKEN_END_STRUCT)
end_switch      ← Tek keyword (TOKEN_END_SWITCH)
end_match       ← Tek keyword (TOKEN_END_MATCH)
end_operator    ← Tek keyword (TOKEN_END_OPERATOR)
end_try         ← Tek keyword (TOKEN_END_TRY)
```

#### 2. Control Flow Keywords
```pmpl
else_if         ← Tek keyword (TOKEN_ELSE_IF)
```

#### 3. Loop Control Keywords
```pmpl
exit_if         ← Tek keyword (TOKEN_EXIT_IF)
exit_for        ← Tek keyword (TOKEN_EXIT_FOR)
exit_while      ← Tek keyword (TOKEN_EXIT_WHILE)
exit_function   ← Tek keyword (TOKEN_EXIT_FUNCTION)
exit_switch     ← Tek keyword (TOKEN_EXIT_SWITCH)

continue_for    ← Tek keyword (TOKEN_CONTINUE_FOR)
continue_while  ← Tek keyword (TOKEN_CONTINUE_WHILE)
```

#### 4. Debug Keywords
```pmpl
debug_goto      ← Tek keyword (TOKEN_DEBUG_GOTO)
debug_pause     ← Tek keyword (TOKEN_DEBUG_PAUSE)
debug_label     ← Tek keyword (TOKEN_DEBUG_LABEL)
debug_print     ← Tek keyword (TOKEN_DEBUG_PRINT)
```

#### 5. State Keywords
```pmpl
shared_state    ← Tek keyword (TOKEN_SHARED_STATE)
```

---

## 🚨 KRİTİK UYARI: BU REFACTOR'DA ÇALIŞAN YZ İÇİN

### ⛔ YASAKLAR
- **"end if" gibi iki kelimelik syntax kullanma!** PMPL'de böyle bir şey yok!
- **"MLP user syntax" kelimesini kullanma!** Sadece PMPL var!
- **Normalization'dan bahsetme!** Sen compiler'sın, normalize edilmiş PMPL görüyorsun!
- **kurallar_kitabı.md okuma!** O kullanıcı için, sen sadece bu TODO'yu oku!

### ✅ KURALLAR
- **PMPL = Compiler'ın gördüğü TEK syntax**
- **Her keyword underscore ile birleşik:** `end_if`, `else_if`, `exit_for`
- **Lexer bunları TEK TOKEN olarak üretir:** TOKEN_END_IF (tek token!)
- **Parser sadece tek token görür:** `case TOKEN_END_IF:` (basit!)

### 📚 TEK KAYNAK
Bu TODO dosyası senin **TEK** referansın. Başka belgeye bakma!

---

## 📋 3-FAZLI REFACTOR PLANI

### **YZ_76: Normalize Layer** ⏳ (2-3 saat)
**Durum:** 🔴 NOT STARTED

**Görev:** User input → PMPL dönüştürücü katmanı

**Dosyalar:**
```
compiler/stage0/normalize/
├── normalize.c          # Ana dönüştürücü
├── normalize.h          # Header
├── syntax_rules.h       # Dönüşüm kuralları
└── test_normalize.c     # Unit testler
```

**Implementasyon:**
```c
// normalize.c - Ana fonksiyon
char* normalize_to_pmpl(const char* user_input) {
    // User input'taki iki kelimelik yapıları underscore ile birleştir
    // Örnek: "end if" → "end_if"
    // Örnek: "else if" → "else_if"
    // Örnek: "exit for" → "exit_for"
    
    // NOT: User input'un nasıl göründüğünü bilmene gerek yok!
    // Sen sadece normalize ediyorsun!
}
```

**Dönüşüm Kuralları (syntax_rules.h):**
```c
// Block terminators
"end if"       → "end_if"
"end while"    → "end_while"
"end for"      → "end_for"
"end function" → "end_function"
"end struct"   → "end_struct"
"end switch"   → "end_switch"
"end match"    → "end_match"
"end operator" → "end_operator"
"end try"      → "end_try"

// Control flow
"else if"      → "else_if"

// Loop control
"exit if"      → "exit_if"
"exit for"     → "exit_for"
"exit while"   → "exit_while"
"exit function"→ "exit_function"
"exit switch"  → "exit_switch"

"continue for" → "continue_for"
"continue while"→"continue_while"

// Debug
"debug goto"   → "debug_goto"
"debug pause"  → "debug_pause"
"debug label"  → "debug_label"
"debug print"  → "debug_print"

// State
"shared state" → "shared_state"
```

**Test Kriteri:**
```bash
cd compiler/stage0/normalize
gcc -o test_normalize test_normalize.c normalize.c -I..
./test_normalize

# Expected output:
✓ "end if" → "end_if"
✓ "end while" → "end_while"
✓ "else if" → "else_if"
✓ "exit for" → "exit_for"
... (tüm kurallar)
✓ All tests passed!
```

**Başarı Kriteri:**
- [ ] normalize.c implementasyonu complete
- [ ] syntax_rules.h tüm kuralları içeriyor
- [ ] test_normalize.c tüm kombinasyonları test ediyor
- [ ] `./test_normalize` tüm testleri geçiyor ✅

**Commit:**
```bash
git add compiler/stage0/normalize/
git commit -m "YZ_76: Normalize layer - User input to PMPL converter"
```

---

### **YZ_77: Lexer Refactor** ⏳ (2-3 saat)
**Durum:** 🔴 NOT STARTED (YZ_76 tamamlanınca başla!)

**Görev:** PMPL underscore keyword'lerini tokenize et

**Dosyalar:**
```
compiler/stage0/modules/lexer/
├── lexer.h              # Token enum'larına yenileri ekle
├── lexer.c              # Keyword recognition ekle
└── test_lexer.c         # Unit testler (yoksa oluştur)
```

**Token Tanımları (lexer.h):**
```c
typedef enum {
    // Existing tokens...
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    // ... existing ...
    
    // ⭐ YENİ: PMPL underscore tokens
    TOKEN_END_IF,
    TOKEN_END_WHILE,
    TOKEN_END_FOR,
    TOKEN_END_FUNCTION,
    TOKEN_END_STRUCT,
    TOKEN_END_SWITCH,
    TOKEN_END_MATCH,
    TOKEN_END_OPERATOR,
    TOKEN_END_TRY,
    
    TOKEN_ELSE_IF,
    
    TOKEN_EXIT_IF,
    TOKEN_EXIT_FOR,
    TOKEN_EXIT_WHILE,
    TOKEN_EXIT_FUNCTION,
    TOKEN_EXIT_SWITCH,
    
    TOKEN_CONTINUE_FOR,
    TOKEN_CONTINUE_WHILE,
    
    TOKEN_DEBUG_GOTO,
    TOKEN_DEBUG_PAUSE,
    TOKEN_DEBUG_LABEL,
    TOKEN_DEBUG_PRINT,
    
    TOKEN_SHARED_STATE,
    
    // ... rest
} TokenType;
```

**Lexer Keyword Recognition (lexer.c):**
```c
// ⭐ ESKİ KOD (YANLIŞ - KALDIRILACAK!):
// if (strcmp(value, "end") == 0) return TOKEN_END;
// if (strcmp(value, "if") == 0) return TOKEN_IF;

// ⭐ YENİ KOD (DOĞRU):
if (strcmp(value, "end_if") == 0) return TOKEN_END_IF;
if (strcmp(value, "end_while") == 0) return TOKEN_END_WHILE;
if (strcmp(value, "end_for") == 0) return TOKEN_END_FOR;
if (strcmp(value, "end_function") == 0) return TOKEN_END_FUNCTION;
if (strcmp(value, "end_struct") == 0) return TOKEN_END_STRUCT;
if (strcmp(value, "end_switch") == 0) return TOKEN_END_SWITCH;
if (strcmp(value, "end_match") == 0) return TOKEN_END_MATCH;
if (strcmp(value, "end_operator") == 0) return TOKEN_END_OPERATOR;
if (strcmp(value, "end_try") == 0) return TOKEN_END_TRY;

if (strcmp(value, "else_if") == 0) return TOKEN_ELSE_IF;

if (strcmp(value, "exit_if") == 0) return TOKEN_EXIT_IF;
if (strcmp(value, "exit_for") == 0) return TOKEN_EXIT_FOR;
if (strcmp(value, "exit_while") == 0) return TOKEN_EXIT_WHILE;
if (strcmp(value, "exit_function") == 0) return TOKEN_EXIT_FUNCTION;
if (strcmp(value, "exit_switch") == 0) return TOKEN_EXIT_SWITCH;

if (strcmp(value, "continue_for") == 0) return TOKEN_CONTINUE_FOR;
if (strcmp(value, "continue_while") == 0) return TOKEN_CONTINUE_WHILE;

if (strcmp(value, "debug_goto") == 0) return TOKEN_DEBUG_GOTO;
if (strcmp(value, "debug_pause") == 0) return TOKEN_DEBUG_PAUSE;
if (strcmp(value, "debug_label") == 0) return TOKEN_DEBUG_LABEL;
if (strcmp(value, "debug_print") == 0) return TOKEN_DEBUG_PRINT;

if (strcmp(value, "shared_state") == 0) return TOKEN_SHARED_STATE;

// Existing single keywords stay:
if (strcmp(value, "if") == 0) return TOKEN_IF;
if (strcmp(value, "then") == 0) return TOKEN_THEN;
if (strcmp(value, "else") == 0) return TOKEN_ELSE;
// ... etc
```

**Test Kriteri:**
```c
// test_lexer.c
void test_pmpl_keywords() {
    assert(tokenize("end_if") == TOKEN_END_IF);
    assert(tokenize("end_while") == TOKEN_END_WHILE);
    assert(tokenize("else_if") == TOKEN_ELSE_IF);
    assert(tokenize("exit_for") == TOKEN_EXIT_FOR);
    // ... tüm keyword'ler
    
    printf("✓ All PMPL keywords tokenized correctly!\n");
}
```

**Başarı Kriteri:**
- [ ] TOKEN_END_IF, TOKEN_END_WHILE vb. tanımlandı
- [ ] Lexer tüm underscore keyword'leri tanıyor
- [ ] ESKİ iki-kelimelik pattern matching kodu KALDIRILDI
- [ ] Test suite tüm token'ları doğruluyor ✅

**Commit:**
```bash
git add compiler/stage0/modules/lexer/
git commit -m "YZ_77: Lexer PMPL keywords - Underscore tokens added"
```

---

### **YZ_78: Parser Simplification** ⏳ (3-4 saat)
**Durum:** 🔴 NOT STARTED (YZ_77 tamamlanınca başla!)

**Görev:** Pattern matching hack'lerini kaldır, parser'ı basitleştir

**Dosyalar:**
```
compiler/stage0/modules/statement/
├── statement_parser.c   # Ana temizlik
└── control_flow.c       # If/while/for parsing

compiler/stage0/modules/functions/
└── functions_standalone.c  # Function parsing temizlik
```

**ESKİ KOD (KALDIRILACAK!):**
```c
// statement_parser.c line 43-67 (YANLIŞ KOD!)
if (tok->type == TOKEN_END) {
    Token* next = lexer_next_token(lexer);
    if (next && next->type == TOKEN_IF) {
        token_free(next);  // ❌ HACK! Manuel token tüketimi
        return;
    } else if (next && next->type == TOKEN_WHILE) {
        token_free(next);  // ❌ HACK!
        return;
    } else if (next && next->type == TOKEN_FOR) {
        token_free(next);  // ❌ HACK!
        return;
    }
    // ... daha fazla hack
    lexer_unget_token(lexer, next);  // ❌ HACK!
}
```

**YENİ KOD (DOĞRU!):**
```c
// statement_parser.c - Basit switch case!
switch (tok->type) {
    case TOKEN_END_IF:     // ✅ Tek token!
        return;
        
    case TOKEN_END_WHILE:  // ✅ Tek token!
        return;
        
    case TOKEN_END_FOR:    // ✅ Tek token!
        return;
        
    case TOKEN_END_FUNCTION:  // ✅ Tek token!
        return;
        
    // ... diğer end_X token'ları
    
    case TOKEN_ELSE_IF:    // ✅ Tek token!
        // else if parsing
        break;
        
    case TOKEN_EXIT_FOR:   // ✅ Tek token!
        // exit for codegen
        break;
        
    case TOKEN_EXIT_WHILE: // ✅ Tek token!
        // exit while codegen
        break;
        
    // ... diğer exit/continue token'ları
        
    default:
        // Normal statement parsing
        break;
}
```

**Temizlenecek Dosyalar:**

1. **statement_parser.c:**
   - Line 43-67: TOKEN_END pattern matching → Kaldır
   - TOKEN_END_IF, TOKEN_END_WHILE vb. case'ler ekle

2. **functions_standalone.c:**
   - Token_END + peek ahead hack → Kaldır
   - TOKEN_END_FUNCTION case ekle

3. **control_flow.c:**
   - else if parsing: TOKEN_ELSE + peek → Kaldır
   - TOKEN_ELSE_IF case ekle

**Başarı Kriteri:**
- [ ] Tüm TOKEN_END pattern matching kodu kaldırıldı
- [ ] Tüm lexer_unget_token() hack'leri kaldırıldı
- [ ] Her underscore token'ı için basit case var
- [ ] Mevcut test suite çalışıyor ✅
- [ ] Kod %40-50 daha kısa ✅

**Test:**
```bash
# Existing tests should still pass:
cd /home/pardus/projeler/MLP/MLP
./melp test_adv.mlp test_adv.s
nasm -f elf64 test_adv.s -o test_adv.o
gcc test_adv.o runtime/sto/*.o -o test_adv
./test_adv
# Should work without any changes to test files!
```

**Commit:**
```bash
git add compiler/stage0/modules/statement/
git add compiler/stage0/modules/functions/
git commit -m "YZ_78: Parser simplification - Pattern matching hacks removed"
```

---

## 🎯 FINAL INTEGRATION

**Durum:** 🔴 NOT STARTED (Tüm 3 YZ tamamlanınca)

### Pipeline Test
```bash
# 1. User input → Normalize → PMPL
echo "if x > 5 then
    print(x)
end if" > test_input.txt

./normalize test_input.txt test_normalized.pmpl

# Expected output (test_normalized.pmpl):
# if x > 5 then
#     print(x)
# end_if    ← Underscore!

# 2. PMPL → Lexer → Tokens
./lexer test_normalized.pmpl tokens.txt

# Expected: TOKEN_IF, TOKEN_IDENTIFIER, ..., TOKEN_END_IF

# 3. Tokens → Parser → AST → Assembly
./melp test_normalized.pmpl test.s
nasm -f elf64 test.s -o test.o
gcc test.o runtime/sto/*.o -o test
./test
# Should execute!
```

### Self-Hosting Test
```bash
# PMPL compiler'ı kendi PMPL syntax'ını parse edebilmeli!
# (Bu YZ_79 için - şimdilik sadece planlama)
```

---

## 📊 İLERLEME TABLOSU

| YZ | Görev | Süre | Durum | Test |
|----|-------|------|-------|------|
| YZ_76 | Normalize Layer | 2-3h | 🔴 NOT STARTED | ❌ |
| YZ_77 | Lexer Refactor | 2-3h | 🔴 NOT STARTED | ❌ |
| YZ_78 | Parser Simplification | 3-4h | 🔴 NOT STARTED | ❌ |
| **TOTAL** | **Full Refactor** | **7-10h** | **0% Complete** | **❌** |

---

## 🚦 YZ GEÇİŞ PROTOKOLÜ

### YZ_76 → YZ_77 Geçiş
```bash
# YZ_76 tamamlandığında:
cd compiler/stage0/normalize
./test_normalize

# Tüm testler geçerse:
git add .
git commit -m "YZ_76 complete: Normalize layer"
git push

# YZ_77 başlayabilir
```

### YZ_77 → YZ_78 Geçiş
```bash
# YZ_77 tamamlandığında:
cd compiler/stage0/modules/lexer
./test_lexer

# Tüm testler geçerse:
git add .
git commit -m "YZ_77 complete: Lexer PMPL keywords"
git push

# YZ_78 başlayabilir
```

### YZ_78 Tamamlanma
```bash
# YZ_78 tamamlandığında:
cd /home/pardus/projeler/MLP/MLP
./run_all_tests.sh

# Tüm testler geçerse:
git add .
git commit -m "YZ_78 complete: Parser simplification - PMPL refactor DONE!"
git push

# TODO.md'ye geri dön
```

---

## 📝 NOTLAR

### Refactor Motivasyonu
1. **Self-hosting blocker:** PMPL compiler kendi syntax'ını parse edemiyor
2. **Multi-syntax impossible:** Normalize layer yok
3. **Code debt:** Pattern matching hack'leri kod kalitesini düşürüyor
4. **Spec violation:** kurallar_kitabı.md'de PMPL spec var ama implement edilmemiş

### Refactor Sonrası
1. ✅ PMPL spec'e uygun mimari
2. ✅ Self-hosting mümkün
3. ✅ Multi-syntax support hazır (Python/C/Go syntax normalize edilebilir)
4. ✅ Daha temiz, bakımı kolay kod

### Risk Mitigation
- **3 küçük YZ:** Her biri geri alınabilir
- **Test-driven:** Her YZ kendi testlerini geçmeli
- **Incremental:** Bir önceki YZ bitmeden sonraki başlamaz

---

## 🎓 YZ Eğitim Materyali

### PMPL Örnekleri

**If-Else:**
```pmpl
if x > 5 then
    print(x)
else_if x > 3 then    ← Tek keyword!
    print("medium")
else
    print("small")
end_if    ← Tek keyword!
```

**While Loop:**
```pmpl
while count < 10 do
    count = count + 1
    if count == 5 then
        exit_while    ← Tek keyword!
    end_if
end_while    ← Tek keyword!
```

**For Loop:**
```pmpl
for i = 0 to 10 do
    if i == 7 then
        continue_for    ← Tek keyword!
    end_if
    print(i)
end_for    ← Tek keyword!
```

**Function:**
```pmpl
function calculate(numeric x, numeric y)
    if x < 0 then
        exit_function    ← Tek keyword!
    end_if
    return x + y
end_function    ← Tek keyword!
```

**Struct:**
```pmpl
struct Point
    numeric x
    numeric y
end_struct    ← Tek keyword!
```

---

**SON GÜNCELLEME:** 14 Aralık 2025  
**SONRAKI ADIM:** YZ_76 başlasın!
