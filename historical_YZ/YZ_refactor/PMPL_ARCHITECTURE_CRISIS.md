# 🚨 PMPL Architecture Crisis - Major Refactor Gerekli

**Tarih:** 14 Aralık 2025  
**Tespit Eden:** YZ_75 Investigation  
**Durum:** 🔴 **LEXER PMPL FELSEFESİNE AYKIRI ÇALIŞIYOR**  
**Etki:** Self-hosting ve multi-syntax desteği **IMKANSıZ**

---

## 🎯 PMPL'NİN VARLIK NEDENİ

### Kurallar Kitabı'ndan (Line 17):

> **MELP'in felsefesi:** MLP yani Multi Language Programming çok dilli çok sözdizimli bir programlama dilidir.
> 
> Kullanıcı istediği dilde (Türkçe, İngilizce, Rusça, Arapça) ve istediği stilde (C-style, Python-style, MLP-style) kodlama yapabilir.
> 
> **MLP'de iki dil paralel gitmektedir:**
> 1. Kullanıcıların göreceği syntax.json'da yer alan MLP dili
> 2. Kullanıcıların asla görmeyeceği arka planda çalışan **pragmatik kodlama dili "PMPL"**
> 
> **Örneğin:**
> - MLP'de if kod bloğu "end if" ile bitmektedir
> - C'de "}" ile bitmektedir
> - **Lexer ve parserde karışıklık çıkmaması için "end if"'i tek tokene indirgemek amacıyla PMPL'de "end_if" kullanılmaktadır.**

### Mimari Katmanlar:

```
[1] Kullanıcı Kodu (Türkçe/İngilizce/Rusça, C/Python/MLP style)
    ↓
[2] diller.json + syntax.json (Normalize Edici)
    ↓
[3] PMPL - English + Pragmatic MLP Base Syntax ← TEK STANDART!
    ↓
[4] Lexer (PMPL okur, TOKEN üretir)
    ↓
[5] Parser (TOKEN'ları parse eder)
    ↓
[6] Codegen
    ↓
[7] Assembly/LLVM
```

### PMPL'nin Amacı:

**"MELP derleyicisi sadece PMPL'yi görür. Bu da dil tasarımında tutarlılık sağlamaktadır."**

---

## 🚨 MEVCUT DURUM - FELSEFİYE AYKIRI

### Normalize Edici Ne Üretmeli:

**Kullanıcı Kodu (MLP Style):**
```mlp
if x > 5 then
    print("Büyük")
end if
```

**Kullanıcı Kodu (C Style):**
```c
if (x > 5) {
    print("Büyük");
}
```

**Kullanıcı Kodu (Python Style):**
```python
if x > 5:
    print("Büyük")
# end_if (implicit veya comment)
```

**Normalize Edici Çıktısı (PMPL - TEK STANDART):**
```pmpl
if x > 5 then
    print("Büyük")
end_if    ← TEK KELİME! (Underscore ile)
```

### Lexer Ne Görüyor Olmalı:

```
Token Stream:
TOKEN_IF
TOKEN_IDENTIFIER("x")
TOKEN_GT
TOKEN_NUMBER(5)
TOKEN_THEN
TOKEN_PRINT
TOKEN_STRING("Büyük")
TOKEN_END_IF    ← TEK TOKEN!
```

### Lexer Gerçekte Ne Görüyor:

```
Token Stream:
TOKEN_IF
TOKEN_IDENTIFIER("x")
TOKEN_GT
TOKEN_NUMBER(5)
TOKEN_THEN
TOKEN_PRINT
TOKEN_STRING("Büyük")
TOKEN_END       ← İKİ AYRI TOKEN! ❌
TOKEN_IF        ← PARSER KARISIR! ❌
```

---

## 💥 NEDEN SORUN?

### 1. Self-Hosting İmkansız

**PMPL'de yazılmış compiler:**
```pmpl
# Normalize edici PMPL üretiyor:
if token_type == TOKEN_IF then
    parse_if_statement()
end_if    ← PMPL syntax

# Ama lexer "end if" görünce:
# TOKEN_END + TOKEN_IF → PARSER KARISIR!
# Self-hosting çöker! 💥
```

### 2. Multi-Syntax Desteği Çöküyor

**Normalize edici çıktısı (hepsi için aynı):**
```pmpl
# C'den geldi:
while count < 10 do
    count = count + 1
end_while    ← PMPL standardı

# Python'dan geldi:
while count < 10 do
    count = count + 1
end_while    ← PMPL standardı

# Go'dan geldi:
while count < 10 do
    count = count + 1
end_while    ← PMPL standardı
```

**Ama lexer:**
```
TOKEN_END + TOKEN_WHILE ← İKİ TOKEN! ❌
```

**Parser her syntax için özel kod yazmalı!** ❌

```c
// statement_parser.c - HACK!
if (tok->type == TOKEN_END) {
    Token* next = lexer_next_token(lexer);
    if (next && next->type == TOKEN_IF) {
        // "end if" pattern
    }
    else if (next && next->type == TOKEN_WHILE) {
        // "end while" pattern
    }
    else if (next && next->type == TOKEN_FOR) {
        // "end for" pattern
    }
    // ... SONSUZ PATTERN MATCHING! ❌
}
```

### 3. Normalize Edici Sorumluluğu Karıştı

**Normalize edici ne yapmalı:**
```
User: "}" (C style)     → PMPL: "end_if"
User: "end if" (MLP)    → PMPL: "end_if"
User: "fi" (Bash)       → PMPL: "end_if"
User: "endif" (Vim)     → PMPL: "end_if"
```

**Şu an ne yapıyor:**
```
User: "}" (C style)     → PMPL: "end if" (İKİ KELİME!) ❌
User: "end if" (MLP)    → PMPL: "end if" (İKİ KELİME!) ❌
```

**Lexer bunu düzeltmeye çalışıyor** → **MİMARİ HATASI!**

---

## 📋 KURALLAR KİTABI'NIN DİKTELERİ

### Lexer Token Birleştirme Bölümü (Line 850-910):

```markdown
## 7. Lexer Token Birleştirme

### Problem
Kullanıcı "end if" (2 kelime) yazar → Lexer 2 token üretir → Parser karışır
C'de `}` 1 token, MLP'de "end if" 2 token → Parser karmaşıklaşır

### Çözüm: Akıllı Token Birleştirme
Lexer peek-ahead ile çok-kelimeli keywordleri tek token'a birleştirir:

"end if"      → END_IF (1 token)
"end while"   → END_WHILE (1 token)
"end for"     → END_FOR (1 token)
"exit for"    → EXIT_FOR (1 token)
"exit while"  → EXIT_WHILE (1 token)
```

**YANİ:** Kurallar kitabı lexer'ın token birleştirme yapmasını diyor!

**AĞIR HATA:** Lexer'da token birleştirme yok, parser'da manuel hack'ler var!

### Birleştirilecek Keyword Listesi:

```markdown
- **end:** if, while, for, function, struct, enum, switch
- **exit:** if, for, while, function, switch
- **continue:** for, while
```

**Toplam:** ~20 kombinasyon

---

## 🎯 NEDEN NORMALIZE EDİCİ YAPMAMALI?

### Yanlış Yaklaşım:

```
[Normalize Edici]
  "}"       → "end if"    ← İKİ KELİME üretiyor
  "end if"  → "end if"    ← İKİ KELİME geçiriyor
  
[Lexer]
  "end if"  → TOKEN_END + TOKEN_IF  ← İki token
  
[Parser]
  if (TOKEN_END && peek() == TOKEN_IF) {  ← Manuel birleştirme! ❌
    // HACK!
  }
```

**Sorun:** Parser her yerde pattern matching yapmalı!

### Doğru Yaklaşım:

```
[Normalize Edici]
  "}"       → "end_if"    ← TEK KELİME! (PMPL standardı)
  "end if"  → "end_if"    ← TEK KELİME! (normalize)
  "fi"      → "end_if"    ← TEK KELİME! (normalize)
  
[Lexer]
  "end_if"  → TOKEN_END_IF  ← TEK TOKEN!
  
[Parser]
  switch (tok->type) {
    case TOKEN_END_IF:   ← BASIT! ✅
      // ...
  }
```

**Avantaj:** Parser basit, tüm syntax'lar aynı!

---

## 🔍 MEVCUT NORMALIZE EDİCİ NE YAPIYOR?

Kontrol edelim:

### diller.json:

```json
{
  "tr": {
    "if": "if",
    "end if": "end if"    ← İKİ KELİME! ❌
  },
  "en": {
    "if": "if",
    "end if": "end if"
  }
}
```

### syntax.json:

**Beklenen (PMPL):**
```json
{
  "mlp": {
    "if_start": "if {condition} then",
    "if_end": "end_if"    ← TEK KELİME! ✅
  },
  "c_style": {
    "if_start": "if ({condition}) {",
    "if_end": "}"         → "end_if" (normalize)
  }
}
```

**Gerçek:** Normalize edici muhtemelen eksik!

---

## 💣 SONUÇ - MİMARİ FELSEFESİNE AYKIRI

### PMPL Felsefesi Der Ki:

> "MELP derleyicisi sadece PMPL'yi görür."

### Gerçek Durum:

❌ **Lexer PMPL görmüyor, kullanıcı syntax'ını görüyor!**  
❌ **Normalize edici PMPL üretmiyor!**  
❌ **Parser manual pattern matching yapıyor!**  
❌ **Self-hosting imkansız!**  
❌ **Multi-syntax desteği çöküyor!**

---

## 📋 MAJOR REFACTOR PLANI (YZ_77)

### Hedef: PMPL Felsefesini Restore Et

**Süre:** 6-8 saat (büyük mimari değişiklik)

### Adım 1: Normalize Edici Refactor (2 saat)

**Görev:** Tüm syntax'ları PMPL'ye çevir (underscore ile)

```python
# normalize.py (veya C)
def normalize_syntax(user_code, syntax_type):
    if syntax_type == "c_style":
        code = user_code.replace("}", "end_if")   # C brace
        code = code.replace("if (", "if ")
        # ...
    elif syntax_type == "python_style":
        # Python indentation → PMPL keywords
        code = convert_indentation_to_keywords(user_code)
        # ...
    elif syntax_type == "mlp_style":
        code = user_code.replace("end if", "end_if")  # Space → underscore
        code = code.replace("end while", "end_while")
        # ...
    
    return code  # PMPL standardında!
```

**Çıktı:**
```pmpl
if x > 5 then
    print("Test")
end_if    ← TEK KELİME! (Her syntax'tan gelirse)
```

### Adım 2: Lexer Token Definitions (1 saat)

**lexer.h:**
```c
typedef enum {
    // ... existing ...
    
    // Block terminators (PMPL standardı)
    TOKEN_END_IF,        // "end_if"
    TOKEN_END_WHILE,     // "end_while"
    TOKEN_END_FOR,       // "end_for"
    TOKEN_END_FUNCTION,  // "end_function"
    TOKEN_END_STRUCT,    // "end_struct"
    TOKEN_END_ENUM,      // "end_enum"
    TOKEN_END_SWITCH,    // "end_switch"
    
    // Loop control
    TOKEN_EXIT_FOR,      // "exit_for"
    TOKEN_EXIT_WHILE,    // "exit_while"
    TOKEN_EXIT_IF,       // "exit_if"
    TOKEN_CONTINUE_FOR,  // "continue_for"
    TOKEN_CONTINUE_WHILE,// "continue_while"
    
    // Legacy (deprecated)
    TOKEN_END,           // "end" (tek başına - eski kod uyumluluk)
    
} TokenType;
```

### Adım 3: Lexer Keyword Recognition (1 saat)

**lexer.c:**
```c
static TokenType get_keyword_type(const char* value) {
    // PMPL keywords (underscore ile)
    if (strcmp(value, "end_if") == 0) return TOKEN_END_IF;
    if (strcmp(value, "end_while") == 0) return TOKEN_END_WHILE;
    if (strcmp(value, "end_for") == 0) return TOKEN_END_FOR;
    if (strcmp(value, "end_function") == 0) return TOKEN_END_FUNCTION;
    if (strcmp(value, "end_struct") == 0) return TOKEN_END_STRUCT;
    
    if (strcmp(value, "exit_for") == 0) return TOKEN_EXIT_FOR;
    if (strcmp(value, "exit_while") == 0) return TOKEN_EXIT_WHILE;
    if (strcmp(value, "continue_for") == 0) return TOKEN_CONTINUE_FOR;
    
    // Basic keywords
    if (strcmp(value, "if") == 0) return TOKEN_IF;
    if (strcmp(value, "while") == 0) return TOKEN_WHILE;
    // ...
    
    return TOKEN_IDENTIFIER;
}
```

**ARTIK:** Lexer PMPL okur, tek token üretir! ✅

### Adım 4: Parser Simplification (2 saat)

**statement_parser.c - ÖNCESİ (HACK):**
```c
if (tok->type == TOKEN_END) {
    Token* next = lexer_next_token(lexer);
    if (next && next->type == TOKEN_IF) {
        token_free(next);  // MANUEL CONSUME! ❌
        // ...
    }
    else if (next && next->type == TOKEN_WHILE) {
        // PATTERN MATCHING! ❌
    }
    // ... 20 kombinasyon! ❌
}
```

**statement_parser.c - SONRASI (TEMİZ):**
```c
// If statement body parsing
while (1) {
    Statement* stmt = statement_parse(parser);
    if (!stmt) break;
    
    // Check for end of if block
    Token* tok = parser->current_token;
    if (tok && tok->type == TOKEN_END_IF) {  // ✅ TEK TOKEN!
        token_free(tok);
        parser->current_token = NULL;
        break;
    }
    
    // Append to body
    append_statement(body, stmt);
}
```

**SONUÇ:** Parser basit, pattern matching yok! ✅

### Adım 5: Control Flow Parser Update (1 saat)

**control_flow_parser.c:**
```c
WhileStatement* control_flow_parse_while(Lexer* lexer, Token* while_token) {
    // ...
    
    // Parse body until TOKEN_END_WHILE
    while (1) {
        Token* tok = lexer_next_token(lexer);
        if (!tok) break;
        
        if (tok->type == TOKEN_END_WHILE) {  // ✅ TEK TOKEN!
            token_free(tok);
            break;
        }
        
        lexer_unget_token(lexer, tok);
        Statement* stmt = statement_parse(parser);
        append_statement(body, stmt);
    }
    
    return stmt;
}
```

### Adım 6: Testing & Validation (1 saat)

**Test Suite:**
```bash
# Test 1: MLP style (normalize: "end if" → "end_if")
echo 'if x > 5 then
    print("Yes")
end if' | ./normalize | ./lexer | ./parser
# Beklenen: TOKEN_END_IF (tek token)

# Test 2: C style (normalize: "}" → "end_if")
echo 'if (x > 5) {
    print("Yes");
}' | ./normalize | ./lexer | ./parser
# Beklenen: TOKEN_END_IF (tek token)

# Test 3: Python style (normalize: indentation → "end_if")
echo 'if x > 5:
    print("Yes")
# end' | ./normalize | ./lexer | ./parser
# Beklenen: TOKEN_END_IF (tek token)
```

---

## ✅ REFACTOR SONRASI DURUM

### Mimari Katmanlar (DOĞRU):

```
[1] Kullanıcı Kodu (C/Python/MLP style)
    ↓
[2] Normalize Edici
    ↓ "}" → "end_if", "end if" → "end_if"
    ↓
[3] PMPL (TEK STANDART!)
    if x > 5 then
        print("Yes")
    end_if    ← Underscore!
    ↓
[4] Lexer
    ↓ "end_if" → TOKEN_END_IF (TEK TOKEN!)
    ↓
[5] Parser
    ↓ switch (TOKEN_END_IF) { ... } (BASIT!)
    ↓
[6] Codegen
```

### Avantajlar:

✅ **Self-hosting mümkün** - PMPL compiler PMPL okur  
✅ **Multi-syntax desteği** - Tüm syntax'lar → PMPL → aynı lexer/parser  
✅ **Parser basitleşir** - Pattern matching yok  
✅ **Normalize edici tek sorumluluk** - Syntax dönüşümü sadece orada  
✅ **PMPL felsefesine uygun** - "Compiler sadece PMPL görür"  
✅ **Yeni syntax kolay** - Normalize edici + diller.json güncelle, lexer/parser değişmez  

---

## 🎯 YZ_77 ACİL REFACTOR TAKVİMİ

### Gün 1 (4 saat):
- [ ] Normalize edici analizi (mevcut kod var mı?)
- [ ] Normalize edici refactor (syntax → PMPL)
- [ ] Lexer token definitions (TOKEN_END_IF vs.)
- [ ] Lexer keyword recognition update

### Gün 2 (4 saat):
- [ ] Parser simplification (pattern matching kaldır)
- [ ] Control flow parser update
- [ ] Functions standalone update
- [ ] Testing & validation

### Başarı Kriterleri:

```bash
# Test 1: While loop (MLP style)
cat > test.mlp << 'EOF'
while count < 10 do
    count = count + 1
end while
EOF

./normalize test.mlp | ./lexer | grep TOKEN_END_WHILE
# Beklenen: TOKEN_END_WHILE (tek token)

# Test 2: Self-hosting test
# PMPL compiler PMPL okuyabilmeli
./mlpc_pmpl compiler_pmpl.mlp test.s
# Beklenen: SUCCESS (end_if/end_while tek token olarak parse edilir)
```

---

## 📚 İLGİLİ DOSYALAR

**Normalize Edici:**
- `compiler/normalize/` (var mı araştır)
- `syntax.json` - Syntax kuralları
- `diller.json` - Keyword çevirileri

**Lexer:**
- `compiler/stage0/modules/lexer/lexer.h` - Token definitions
- `compiler/stage0/modules/lexer/lexer.c` - Keyword recognition

**Parser:**
- `compiler/stage0/modules/statement/statement_parser.c` - Pattern matching kaldırılacak
- `compiler/stage0/modules/control_flow/control_flow_parser.c` - Simplify
- `compiler/stage0/modules/functions/functions_standalone.c` - First pass fix

**Docs:**
- `kurallar_kitabı.md` - PMPL felsefesi, token birleştirme kuralları
- `NEXT_AI_START_HERE.md` - PMPL architecture bölümü güncelle

---

## 🎓 ÖĞRENİLEN DERSLER

### 1. Mimari Felsefe Takip Edilmeli

❌ **Yanlış:** "Parser'da hallederiz" mentalitesi  
✅ **Doğru:** Her katman kendi sorumluluğunu yerine getirmeli

### 2. Normalize Edici Kritik

PMPL'nin varlık nedeni: **Tüm syntax'ları tek standarda indirgemek**

Normalize edici zayıfsa → Lexer/Parser karmaşıklaşır

### 3. Self-Hosting Test Önemli

Eğer compiler kendi syntax'ını parse edemiyorsa → Mimari hatalı!

### 4. Documentation Takip Et

Kurallar kitabı **açık açık** "lexer token birleştirmesi" diyor.  
Biz **parser'da manuel hack** yaptık.  
**Dokümantasyonu ciddiye al!**

---

## 🚀 SONUÇ

**YZ_77: PMPL Architecture Restoration**

**Öncelik:** 🔴 **EN YÜKSEK** (self-hosting blocker)  
**Süre:** 6-8 saat (major refactor)  
**Etki:** Tüm lexer, parser, normalize edici

**Hedef:** 
- PMPL felsefesini restore et
- Lexer PMPL görsün (underscore keywords)
- Parser basitleşsin (pattern matching yok)
- Self-hosting mümkün olsun

**Alternatif:** 
- Geçici hack'ler ile devam et ❌
- Her yeni syntax için parser update ❌
- Self-hosting hiç çalışmaz ❌

**Karar:** **MAJOR REFACTOR ŞART!** ✅

---

**Rapor:** YZ_75 (PMPL Architecture Crisis Discovery)  
**Sonraki:** YZ_77 (PMPL Normalize + Lexer Refactor)  
**Önce:** YZ_76 (Print fix) - ERTELE, mimari önce!
