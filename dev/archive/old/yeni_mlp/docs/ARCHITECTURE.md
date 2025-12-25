# MLP Dil Dönüşüm Mimarisi - Doğru Yol Haritası

## 📋 Mimari Özeti

```
Kullanıcı Kodu (Türkçe)
    ↓
diller.json + syntax.json (Dil Tanımları)
    ↓
Dönüştürücü (Normalizer)
    ↓
İngilizce + MLP Base Syntax
    ↓
Lexer (Token Üretimi)
    ↓
Parser (AST Oluşturma)
    ↓
Code Generator (Assembly/C)
```

---

## 🎯 Doğru Adımlar (Sıralı)

### **Faz 1: Dil Tanımları ve Normalizer (Temel Altyapı)**

#### 1.1 Dil Tanım Dosyalarını Hazırla
- [ ] `diller.json`: Türkçe → İngilizce keyword mapping
  ```json
  {
    "tr": {
      "function": "işlev",
      "if": "eğer",
      "then": "ise",
      "while": "döngü",
      "return": "dön",
      "numeric": "sayısal",
      "string": "metin"
    }
  }
  ```

- [ ] `syntax.json`: MLP base syntax kuralları
  ```json
  {
    "keywords": ["function", "if", "while", "struct", ...],
    "operators": ["+", "-", "*", "/", "==", "!=", ...],
    "block_terminators": {
      "function": "end_function",
      "if": "end_if",
      "while": "end_while",
      "struct": "end_struct"
    }
  }
  ```

#### 1.2 Normalizer (Dönüştürücü) Oluştur
- [ ] `normalize.py` veya `mlp_normalize` binary
- [ ] **Görevleri:**
  1. Kullanıcı kodunu oku (Türkçe keyword'lerle)
  2. `diller.json` kullanarak Türkçe → İngilizce dönüştür
  3. `syntax.json` kurallarına uygun format'a çevir
  4. Normalize edilmiş kodu çıktı olarak ver
  
- [ ] **Önemli:** 
  - Tek kelime keyword'ler kullan: `end_function`, `end_if`, `end_while`
  - Boşluklardan kaçın: `end function` ❌ → `end_function` ✅
  - Tutarlı syntax: Tüm yapılar aynı pattern'i takip etsin

#### 1.3 Test Senaryoları
- [ ] Basit fonksiyon tanımı (Türkçe → normalize)
- [ ] If-else yapısı
- [ ] While/for döngüleri
- [ ] Struct tanımları
- [ ] Import statements

---

### **Faz 2: Lexer (Token Üretimi)**

#### 2.1 Lexer Tasarımı
- [ ] **Input:** Normalize edilmiş MLP kodu (İngilizce + base syntax)
- [ ] **Output:** Token stream

#### 2.2 Token Tipleri Tanımla
```c
typedef enum {
    // Keywords
    TOKEN_FUNCTION,
    TOKEN_IF, TOKEN_THEN, TOKEN_ELSE,
    TOKEN_WHILE, TOKEN_DO,
    TOKEN_RETURN,
    TOKEN_STRUCT,
    
    // End Terminators (TEK KELİME!)
    TOKEN_END_FUNCTION,
    TOKEN_END_IF,
    TOKEN_END_WHILE,
    TOKEN_END_STRUCT,
    TOKEN_END,  // Generic end
    
    // Types
    TOKEN_TYPE_NUMERIC,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_BOOLEAN,
    
    // Operators
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MUL, TOKEN_DIV,
    TOKEN_ASSIGN, TOKEN_EQ, TOKEN_NEQ,
    
    // Identifiers & Literals
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING_LITERAL,
    
    TOKEN_EOF
} TokenType;
```

#### 2.3 Keyword Recognition
- [ ] `check_keyword()` fonksiyonu: string → TokenType
- [ ] **KRİTİK:** Keyword'ler tek kelime olmalı:
  ```c
  if (strcmp(word, "end_function") == 0) return TOKEN_END_FUNCTION;
  if (strcmp(word, "end_if") == 0) return TOKEN_END_IF;
  if (strcmp(word, "end_while") == 0) return TOKEN_END_WHILE;
  ```

#### 2.4 Lexer Testi
- [ ] Her token tipi için unit test
- [ ] Edge case'ler: whitespace, comments, string literals
- [ ] Keyword vs identifier ayrımı

---

### **Faz 3: Parser (AST Oluşturma)**

#### 3.1 AST Node Tipleri
```c
typedef enum {
    AST_FUNCTION_DECLARATION,
    AST_IF_STATEMENT,
    AST_WHILE_LOOP,
    AST_RETURN_STATEMENT,
    AST_STRUCT_DECLARATION,
    AST_VARIABLE_DECLARATION,
    AST_ASSIGNMENT,
    AST_BINARY_OPERATION,
    AST_FUNCTION_CALL,
    AST_BLOCK,
    ...
} ASTNodeType;
```

#### 3.2 Parsing Fonksiyonları
- [ ] `parse_function()`: function ... end_function
- [ ] `parse_if()`: if ... then ... end_if
- [ ] `parse_while()`: while ... do ... end_while
- [ ] `parse_struct()`: struct ... end_struct
- [ ] `parse_block()`: Statement'ları context-aware parse et

#### 3.3 Context Stack (Önemli!)
- [ ] **Sorun Kaynağı:** Nested yapılarda end token'ı hangi bloka ait?
- [ ] **Çözüm:** Context stack kullan
  ```c
  typedef enum {
      CTX_FUNCTION,
      CTX_IF,
      CTX_WHILE,
      CTX_STRUCT
  } BlockContext;
  
  BlockContext context_stack[100];
  int context_depth = 0;
  
  void push_context(BlockContext ctx);
  void pop_context();
  ```

- [ ] Her yapı parse edilirken:
  1. `push_context(CTX_XXX)`
  2. Blok parse et
  3. `pop_context()`
  4. End token consume et

#### 3.4 Block Parsing
- [ ] `parse_block()` context-aware olmalı:
  ```c
  while (current_token != TOKEN_EOF) {
      if (context_depth > 0) {
          BlockContext ctx = context_stack[context_depth - 1];
          
          // Context'e uygun end token'da dur
          if (ctx == CTX_FUNCTION && token == TOKEN_END_FUNCTION) break;
          if (ctx == CTX_IF && token == TOKEN_END_IF) break;
          if (ctx == CTX_WHILE && token == TOKEN_END_WHILE) break;
          
          // Generic end her context için geçerli
          if (token == TOKEN_END) break;
      }
      
      // Statement parse et
      ASTNode* stmt = parse_statement();
      if (stmt) add_to_block(stmt);
  }
  ```

#### 3.5 Import System (Dikkatli!)
- [ ] Circular import detection: imported_files[] array
- [ ] Nested import: Lexer state save/restore
- [ ] **HATA YAPMA:** Nested import'ta token pointer'ı KAYDETME, restore sonrası yeni token al:
  ```c
  // ❌ YANLIŞ:
  Token* saved_token = current_token;
  // ... import parse ...
  current_token = saved_token;  // Stale pointer!
  
  // ✅ DOĞRU:
  restore_lexer_state();
  current_token = getNextToken();  // Fresh token!
  ```

#### 3.6 Parser Testi
- [ ] Her AST node tipi için test
- [ ] Nested yapılar: if içinde while, function içinde if, vb.
- [ ] Import: basit, nested, circular

---

### **Faz 4: Code Generator (Assembly/C Üretimi)**

#### 4.1 Generator Tasarımı
- [ ] **Input:** AST tree
- [ ] **Output:** Assembly veya C kodu

#### 4.2 Visitor Pattern
- [ ] Her AST node tipi için `visit_XXX()` fonksiyonu
- [ ] Recursive traversal: Parent → Children

#### 4.3 Code Generation Strategies
- [ ] **Function:** Label tanımı, prologue, body, epilogue
- [ ] **Variables:** Stack allocation, scope tracking
- [ ] **Control Flow:** Jump instructions (if/while)
- [ ] **Expressions:** Register allocation, operator precedence

#### 4.4 Runtime Library
- [ ] Built-in fonksiyonlar: print, memory allocation, file I/O
- [ ] Garbage collection (optional)
- [ ] String/array operations

---

## 🚨 Kritik Hatalar ve Önleme

### **Hata 1: End Keyword Tutarsızlığı**
❌ **Sorun:** Bazı dosyalarda `end function`, bazılarında `end_function`

✅ **Çözüm:**
- Normalizer'da TEK format kullan: `end_function`, `end_if`, `end_while`
- Lexer'da TEK keyword tanımla: `"end_function"` → `TOKEN_END_FUNCTION`
- User code dilinde (Türkçe) ne olursa olsun, normalize sonrası hep aynı format

### **Hata 2: Context Stack Dengesizliği**
❌ **Sorun:** `push_context()` yapıldı ama `pop_context()` unutuldu (özellikle else-if'te)

✅ **Çözüm:**
- **KURAL:** Her `push_context()` için MUTLAKA `pop_context()` olmalı
- else-if recursive call yapıyorsa, outer if'in context'i doğru yönetilmeli
- Veya daha iyi: else-if yerine separate if'ler kullan (context karışıklığını önler)

### **Hata 3: Import Token Corruption**
❌ **Sorun:** Nested import sonrası stale token pointer

✅ **Çözüm:**
- Save: Lexer state (source_code, position, line, column)
- **Token'ı KAYDETME!**
- Restore: Lexer state
- **Yeni token al:** `current_token = getNextToken()`

### **Hata 4: Block Parsing vs Import Parsing**
❌ **Sorun:** Import parsing'de end token'ları statement olarak parse edilmeye çalışılıyor

✅ **Çözüm:**
- Import parsing için ayrı loop kullan
- Loop başında END token check yap, görünce break
- Veya: `komut()` NULL döndürebilir, caller handle eder

---

## 📝 Doğru Implementation Order

### **Adım 1-3: Temel Pipeline (1-2 hafta)**
1. Normalizer + dil tanımları → Test edilebilir, bağımsız
2. Lexer → Normalize edilmiş kod üzerinde test
3. Parser (basit) → AST üretimi, nested yapılar YOK henüz

### **Adım 4-6: Advanced Features (2-3 hafta)**
4. Context stack + nested yapılar → Parser'ı güçlendir
5. Import system → Circular detection, nested import
6. Code generator (temel) → Basit assembly/C üret

### **Adım 7-9: Self-Hosting (2-4 hafta)**
7. Compiler kendi kendini compile edebilmeli
8. Runtime library → Tüm built-in fonksiyonlar
9. Optimization → Performance, memory

---

## 🎯 Başarı Kriterleri

### **Milestone 1: Normalizer + Lexer ✅**
- Türkçe kod → İngilizce normalize
- Normalize kod → Token stream
- Test coverage: %90+

### **Milestone 2: Parser ✅**
- Token stream → AST
- Nested yapılar doğru parse
- Import system çalışıyor

### **Milestone 3: Code Gen ✅**
- AST → Assembly/C
- Basit programlar çalışıyor
- Runtime library entegrasyonu

### **Milestone 4: Self-Hosting 🎯**
- Compiler kendi kodunu compile ediyor
- Bootstrap process otomatik
- Full test suite passing

---

## 🔧 Tooling ve Testing

### **Test Stratejisi**
```
tests/
├── normalizer/
│   ├── basic_keywords.mlp
│   ├── nested_structures.mlp
│   └── edge_cases.mlp
├── lexer/
│   ├── tokens.test.c
│   └── keywords.test.c
├── parser/
│   ├── ast_nodes.test.c
│   └── nested_blocks.test.c
└── codegen/
    ├── simple_function.test.c
    └── control_flow.test.c
```

### **Debug Araçları**
- [ ] AST visualizer: Tree'yi göster
- [ ] Token dumper: Lexer output'u göster
- [ ] Step-by-step parser: Her adımı trace et

---

## 📚 Referanslar

### **Benzer Diller**
- **Lua:** Basit syntax, powerful VM
- **Python:** Dynamic typing, readable
- **Ruby:** Blocks, metaprogramming

### **Compiler Kaynakları**
- Dragon Book (Compilers: Principles, Techniques, and Tools)
- Crafting Interpreters (online, ücretsiz)
- LLVM Tutorial

---

## 🚀 Son Notlar

### **En Önemli İlkeler**
1. **Tutarlılık:** Syntax kuralları değişmez, her yerde aynı
2. **Basitlik:** Karmaşık çözümler yerine basit, anlaşılır kod
3. **Test:** Her adımda test, entegrasyon sonunda değil
4. **Dokümantasyon:** Her kararın nedeni yazılmalı

### **Önümüzdeki Engeller**
- ✅ Parser phase başarılı
- ⏳ Codegen import parsing - syntax tutarlılığı sağlanmalı
- ❌ Assembly generation - henüz test edilmedi
- ❌ Self-hosting - final milestone

### **Bir Sonraki Adım**
1. Kalan syntax hatalarını fix et (end keywords)
2. Codegen phase'i tamamla
3. İlk assembly çıktısını al
4. Bootstrap süreci test et

---

**Tarih:** 25 Kasım 2025
**Durum:** Parser ✅ | Codegen ⏳ | Self-hosting ❌
**Sonraki Hedef:** Assembly generation başarılı
