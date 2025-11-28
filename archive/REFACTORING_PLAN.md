# Self-Hosting Sonrası Temiz Refactoring Planı

**Tarih:** 25 Kasım 2025  
**Amaç:** Mevcut sistemi self-hosting sonrası sıfırdan yeniden düzenlemek

---

## ✅ **CEVAP: EVET, Sistem Tamamen Normale Döndürülebilir!**

Self-hosting tamamlandıktan sonra, **mlpc.c artık sadece bootstrap aracı** olur. Bu noktada:

1. ✅ C compiler'ı istediğin gibi yeniden yaz
2. ✅ Token enum'ları değiştir
3. ✅ Runtime API'yi refactor et
4. ✅ Tüm mimariyi baştan kur

**Tek şart:** Yeni mlpc.c, mevcut mlpc.mlp'yi compile edebilmeli (Stage1 oluşturabilmeli).

---

## 🎯 **Refactoring Stratejisi**

### **Senaryo: Self-Hosting Başarılı (Stage1 = Stage2)**

```
Mevcut Durum:
✅ mlpc.mlp var (self-hosted compiler)
✅ mlpc.c var (bootstrap compiler - DEPRECATED)
✅ Stage1 = Stage2 (verification passed)

Hedef:
🎯 Temiz C bootstrap compiler (mlpc_clean.c)
🎯 Yeni token system
🎯 Yeni runtime API
🎯 Thread-safe architecture
```

---

## 📋 **Adım Adım Plan**

### **Phase 1: Backup & Analysis (1 gün)**

```bash
# 1. Mevcut çalışan sistemi yedekle
cd /home/pardus/projeler/tyd-lang/MLP
mkdir backup_self_hosted
cp mlpc mlpc_stage1 self_host/mlpc.mlp backup_self_hosted/
cp -r runtime/ backup_self_hosted/runtime_old/

# 2. Çalışan Stage1'i koru
mv mlpc_stage1 mlpc_stage1_WORKING
```

**Sonuç:** ✅ Çalışan sisteme her zaman dönebiliriz

---

### **Phase 2: Design New C Bootstrap (2-3 gün)**

#### 2.1 Yeni Token System

**Eski (Karışık):**
```c
// mlpc.c
typedef enum {
    TOKEN_FUNCTION,  // "işlev" hardcoded
    TOKEN_IF,        // "eğer" hardcoded
    ...
} TokenType;
```

**Yeni (Temiz):**
```c
// mlpc_clean.c
typedef enum {
    // Keywords (normalized)
    TOKEN_FUNCTION,     // "function" only
    TOKEN_IF,           // "if" only
    TOKEN_WHILE,        // "while" only
    TOKEN_RETURN,       // "return" only
    
    // Types
    TOKEN_NUMERIC,      // "numeric"
    TOKEN_STRING,       // "string"
    
    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    
    // Literals
    TOKEN_NUMBER,       // 123, 45.67
    TOKEN_STRING_LIT,   // "hello"
    TOKEN_IDENTIFIER,   // variable names
    
    // Structural
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_COMMA,        // ,
    TOKEN_SEMICOLON,    // ;
    TOKEN_COLON,        // :
    TOKEN_DOT,          // .
    TOKEN_ARROW,        // ->
    
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

// NO TURKISH KEYWORDS IN LEXER!
```

#### 2.2 Yeni Lexer Architecture

```c
// Thread-safe state
typedef struct {
    const char* source;
    size_t length;
    size_t position;
    int line;
    int column;
    Token current;
} LexerState;

// Initialize lexer
LexerState* lexer_new(const char* source) {
    LexerState* state = malloc(sizeof(LexerState));
    state->source = strdup(source);
    state->length = strlen(source);
    state->position = 0;
    state->line = 1;
    state->column = 1;
    return state;
}

// Get next token
Token lexer_next(LexerState* state) {
    skip_whitespace(state);
    
    if (state->position >= state->length) {
        return make_token(TOKEN_EOF, state);
    }
    
    char ch = state->source[state->position];
    
    // Operators
    if (ch == '+') return make_simple_token(TOKEN_PLUS, state);
    if (ch == '-') return make_simple_token(TOKEN_MINUS, state);
    
    // Numbers
    if (isdigit(ch)) return lex_number(state);
    
    // Strings
    if (ch == '"') return lex_string(state);
    
    // Keywords and identifiers
    if (isalpha(ch) || ch == '_') return lex_identifier(state);
    
    return make_error_token("Unexpected character", state);
}

// Keyword check (ONLY ENGLISH)
TokenType check_keyword(const char* word) {
    if (strcmp(word, "function") == 0) return TOKEN_FUNCTION;
    if (strcmp(word, "if") == 0) return TOKEN_IF;
    if (strcmp(word, "while") == 0) return TOKEN_WHILE;
    if (strcmp(word, "return") == 0) return TOKEN_RETURN;
    if (strcmp(word, "numeric") == 0) return TOKEN_NUMERIC;
    if (strcmp(word, "string") == 0) return TOKEN_STRING;
    
    return TOKEN_IDENTIFIER;
}

// Clean up
void lexer_free(LexerState* state) {
    free((char*)state->source);
    free(state);
}
```

#### 2.3 Yeni AST Structures

```c
// Clean AST node types
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION_DEF,
    AST_VARIABLE_DECL,
    AST_IF_STATEMENT,
    AST_WHILE_LOOP,
    AST_FOR_LOOP,
    AST_RETURN_STATEMENT,
    AST_EXPRESSION_STATEMENT,
    AST_BLOCK,
    
    // Expressions
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_CALL,
    AST_MEMBER_ACCESS,
    AST_ARRAY_ACCESS,
    AST_LITERAL,
    AST_IDENTIFIER,
} ASTNodeType;

// Polymorphic AST node
typedef struct ASTNode {
    ASTNodeType type;
    int line;
    int column;
    
    union {
        struct {
            char* name;
            struct ASTNode** params;
            int param_count;
            struct ASTNode* body;
            char* return_type;
        } function_def;
        
        struct {
            char* name;
            char* type;
            struct ASTNode* initializer;
        } variable_decl;
        
        struct {
            struct ASTNode* condition;
            struct ASTNode* then_branch;
            struct ASTNode* else_branch;
        } if_statement;
        
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
            TokenType operator;
        } binary_op;
        
        struct {
            char* value;
        } literal;
    } as;
} ASTNode;
```

#### 2.4 Yeni Runtime API

**Eski (Karışık):**
```c
// runtime/hashmap.c
void* mlp_map_get(HashMap* map, const char* key);
void* mlp_list_get(List* list, int index);  // FARKLI İSİM STILI
```

**Yeni (Tutarlı):**
```c
// runtime_clean/collections.h

// HashMap API
typedef struct HashMap HashMap;

HashMap* hashmap_new(void);
void hashmap_put(HashMap* map, const char* key, void* value);
void* hashmap_get(HashMap* map, const char* key);
bool hashmap_has(HashMap* map, const char* key);
void hashmap_remove(HashMap* map, const char* key);
void hashmap_free(HashMap* map);

// List API
typedef struct List List;

List* list_new(void);
void list_append(List* list, void* value);
void* list_get(List* list, int index);
void list_set(List* list, int index, void* value);
int list_size(List* list);
void list_free(List* list);

// String API
typedef struct String String;

String* string_new(const char* cstr);
String* string_concat(String* a, String* b);
int string_length(String* s);
const char* string_cstr(String* s);
void string_free(String* s);
```

---

### **Phase 3: Implement Clean Bootstrap (1 hafta)**

```bash
# 1. Yeni dosyalar oluştur
cd /home/pardus/projeler/tyd-lang/MLP

mkdir clean_bootstrap/
cd clean_bootstrap/

# 2. Clean C compiler
cat > mlpc_clean.c
# (Yukarıdaki temiz mimariyi uygula)

# 3. Clean runtime
mkdir runtime_clean/
cat > runtime_clean/collections.h
cat > runtime_clean/collections.c
cat > runtime_clean/string.c
cat > runtime_clean/gc.c
```

**Hedef:** Tek dosya, temiz API, thread-safe

---

### **Phase 4: Integrate with MLP (1 hafta)**

#### 4.1 MLP Compiler'ı Güncellemek GEREKMEZ!

**Önemli:** `mlpc.mlp` **DEĞİŞMEZ**!

```
mlpc_clean.c sadece şunu yapabilmeli:
- mlpc.mlp'yi OKU
- Mevcut MLP syntax'ını PARSE ET
- Assembly ÜRET
```

**Neden?**
- mlpc.mlp zaten çalışıyor (self-hosted)
- mlpc_clean.c bir INTERPRETER, compiler değil
- mlpc.mlp'nin ürettiği Stage1 zaten doğru

#### 4.2 Test Strategy

```bash
# 1. Eski bootstrap
gcc -o mlpc_old mlpc.c runtime/hashmap.c -lm
./mlpc_old self_host/mlpc.mlp stage1_old.asm

# 2. Yeni bootstrap
gcc -o mlpc_clean clean_bootstrap/mlpc_clean.c \
    runtime_clean/collections.c -lm
./mlpc_clean self_host/mlpc.mlp stage1_new.asm

# 3. Compare
diff stage1_old.asm stage1_new.asm

# ✅ Eğer fark yoksa: SUCCESS!
```

---

### **Phase 5: Switch to Clean System (2-3 gün)**

```bash
# 1. Yeni bootstrap başarılı oldu
cd /home/pardus/projeler/tyd-lang/MLP

# 2. Eski sistemi arşivle
mkdir archive_old_system/
mv mlpc.c archive_old_system/
mv runtime/ archive_old_system/

# 3. Yeni sistemi aktif et
mv clean_bootstrap/mlpc_clean.c ./mlpc.c
mv runtime_clean/ ./runtime/

# 4. Yeni compiler derle
gcc -o mlpc mlpc.c runtime/collections.c -lm

# 5. Stage1 oluştur
./mlpc self_host/mlpc.mlp mlpc_stage1.asm
nasm -f elf64 mlpc_stage1.asm
gcc -o mlpc_stage1 mlpc_stage1.o runtime/collections.c -lm

# 6. Verify
./mlpc_stage1 self_host/mlpc.mlp mlpc_stage2.asm
diff mlpc_stage1.asm mlpc_stage2.asm

# ✅ Stage1 = Stage2 → SUCCESS!
```

---

### **Phase 6: Refactor MLP Code (Opsiyonel) (2-4 hafta)**

Şimdi artık **mlpc.mlp'yi de refactor edebilirsin**:

#### 6.1 Normalizer Ekle

```bash
# Yeni workflow:
python mlp_normalize.py mycode_tr.mlp mycode_norm.mlp
./mlpc mycode_norm.mlp mycode.asm
```

#### 6.2 MLP Compiler'ı Güncelle

```mlp
# self_host/mlpc_v2.mlp

# Yeni runtime API kullan
function example() : numeric
    let map = hashmap_new()      # ← YENİ API
    hashmap_put(map, "key", 42)
    let val = hashmap_get(map, "key")
    return val
end_function
```

#### 6.3 Bootstrap v2

```bash
# Stage0: mlpc_clean.c (temiz)
./mlpc self_host/mlpc_v2.mlp mlpc_v2_stage1.asm

# Stage1: mlpc_v2_stage1 (MLP'den)
./mlpc_v2_stage1 self_host/mlpc_v2.mlp mlpc_v2_stage2.asm

# Verify
diff mlpc_v2_stage1.asm mlpc_v2_stage2.asm
```

---

## 🎯 **Özet**

### **Şu Anda (Self-Hosting Öncesi):**
```
mlpc.c (karmaşık) → mlpc.mlp compile ediyor → Stage1 üretiyor
├─ Token enum: Türkçe hardcoded
├─ Global state: Thread-unsafe
├─ Runtime API: İsimlendirme karışık
└─ Parser: Birçok workaround
```

### **Self-Hosting Sonrası (1-3 gün):**
```
mlpc.c (karmaşık) → mlpc.mlp compile edebiliyor
mlpc_stage1 (temiz MLP) → kendini compile edebiliyor
                        → Stage1 = Stage2 ✅
```

### **Refactoring Sonrası (2-3 hafta):**
```
mlpc_clean.c (TEMIZ) → mlpc.mlp compile edebiliyor
├─ Token enum: Sadece İngilizce
├─ Thread-safe: LexerState struct
├─ Runtime API: Tutarlı isimlendirme
└─ Parser: Temiz mimari

mlpc_stage1 (MLP) → kendini compile edebiliyor
                  → Stage1 = Stage2 ✅
```

### **Final Durum (1-2 ay):**
```
mlpc_clean.c (minimal bootstrap) → sadece Stage1 üretmek için
mlpc_stage1 (production compiler) → asıl geliştirme burada
├─ Normalizer: Türkçe/İngilizce/İspanyolca destekli
├─ Thread-safe: Paralel compilation
├─ Modern runtime: GC, async/await, FFI
└─ Self-hosting: %100 MLP ile yazılmış
```

---

## ✅ **SONUÇ**

### **Sorunuzun Cevabı:**

> **"Self-hosting tamamlandığında sistem normale döndürülebilir mi?"**

**EVET!** ✅

1. ✅ Token enum'ları tamamen yeniden tasarlayabilirsin
2. ✅ Runtime API'yi baştan yazabilirsin
3. ✅ C compiler'ı thread-safe yapabilirsin
4. ✅ Tüm mimariyi temiz şekilde kurabilirsin

**Tek Şart:**
- Yeni mlpc_clean.c, mevcut mlpc.mlp'yi compile edebilmeli
- Çünkü mlpc.mlp zaten self-hosted ve çalışıyor

**Süreç:**
```
1. Self-hosting başarılı (1-3 gün)
   → mlpc.mlp çalışıyor ✅

2. mlpc_clean.c yaz (1 hafta)
   → Temiz mimari
   → mlpc.mlp'yi compile edebilir

3. Switch (1 gün)
   → mlpc.c → mlpc_clean.c
   → Stage1 = Stage2 verify

4. Refactor mlpc.mlp (Opsiyonel, 2-4 hafta)
   → Yeni API kullan
   → Normalizer ekle
```

**Sonuç:** Sistem baştan hiç hata yapılmamış gibi temiz olabilir! 🎉

---

## 📋 **Timeline**

| Faz | Süre | Çıktı |
|-----|------|-------|
| Self-hosting complete | 1-3 gün | Stage1 = Stage2 ✅ |
| Design clean bootstrap | 2-3 gün | mlpc_clean.c tasarımı |
| Implement mlpc_clean.c | 1 hafta | Temiz C compiler |
| Test & switch | 2-3 gün | mlpc.c → mlpc_clean.c |
| Refactor mlpc.mlp (optional) | 2-4 hafta | Yeni API, normalizer |

**TOPLAM:** 2-6 hafta (self-hosting sonrası)

---

## 🚀 **Şimdi Ne Yapmalı?**

1. **Önce self-hosting'i bitir** (1-3 gün)
   ```bash
   ./mlpc self_host/mlpc.mlp stage1.asm
   # Stage1 = Stage2 ✅
   ```

2. **Sonra refactoring planını uygula** (2-6 hafta)
   - mlpc_clean.c yaz
   - Runtime temizle
   - Test et
   - Switch

3. **Son olarak MLP code refactor** (opsiyonel)
   - Normalizer
   - Yeni API
   - Thread-safe

**Sonuç:** Sistem baştan doğru kurulmuş gibi temiz! 🎉
