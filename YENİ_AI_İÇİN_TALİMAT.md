# YENİ AI İÇİN TALİMAT

**Tarih:** 4 Aralık 2025  
**Branch:** mlp-true-syntax  
**Durum:** Bootstrap compiler'a if/while/for desteği eklenecek

---

## 🎯 ŞU ANDA NEREDEYIZ?

### ✅ TAMAMLANAN
1. **Bootstrap melpc (C derleyici)** - %90 çalışıyor
   - Lokasyon: `melp/bootstrap/`
   - Derleme: `cd melp/bootstrap && make`
   - Binary: `melp/bootstrap/melpc`
   - **ÖNEMLİ BUG FİX:** Variable name bug düzeltildi (expect yerine match kullan)

2. **Doğru MLP Syntax** öğrenildi
   - Comments: `--` (single line), `---` multi-line `---`
   - Functions: `function name() numeric ... end function`
   - Variables: `numeric x = 42`
   - NO: `//`, `fn`, `let`, `const`, `{}`, `->`

3. **MLP Skeleton Modülleri** oluşturuldu
   - `melp/bootstrap/lexer.mlp` - Token types, lexer fonksiyonları
   - `melp/bootstrap/parser.mlp` - AST node types, parser fonksiyonları
   - `melp/bootstrap/codegen.mlp` - Assembly generation
   - `melp/bootstrap/main.mlp` - Main pipeline
   - **Hepsi bootstrap melpc ile derlenebiliyor ✅**

### ❌ EKSİK OLAN
**Bootstrap melpc if/while/for desteklemiyor!**

---

## 🚀 YAPILACAK İŞ: IF/WHILE/FOR DESTEĞI EKLE

### Süre Tahmini: 1-2 saat

### Adım 1: parser.h'ye Node Types Ekle (5 dakika)

**Dosya:** `melp/bootstrap/parser.h`

```c
typedef enum {
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_VAR_DECL,
    NODE_RETURN,
    NODE_STRUCT,
    NODE_IDENTIFIER,
    NODE_LITERAL,
    NODE_TYPE,
    NODE_IF,        // ← EKLE
    NODE_WHILE,     // ← EKLE
    NODE_FOR        // ← EKLE
} NodeType;
```

### Adım 2: parser.c'ye Parse Fonksiyonları Ekle (45 dakika)

**Dosya:** `melp/bootstrap/parser.c`

**NOT:** lexer.c zaten if/while/for keyword'lerini tanıyor (satır 93-107), sadece parser eksik!

#### A) parse_if() fonksiyonu ekle:

```c
static ASTNode* parse_if() {
    if (!match(TK_KEYWORD, "if")) return NULL;
    
    ASTNode *node = create_node(NODE_IF);
    advance();
    
    // Parse condition (basit: sadece identifier veya number)
    if (current_token.type == TK_IDENTIFIER || current_token.type == TK_NUMBER) {
        node->left = create_node(NODE_LITERAL);
        strcpy(node->left->value, current_token.value);
        advance();
    }
    
    // Expect "then"
    expect(TK_KEYWORD, "then");
    
    // Parse body statements (şimdilik atla, end if'e kadar)
    while (!match(TK_KEYWORD, "end")) {
        parse_statement(); // Body'deki statement'ları parse et
    }
    
    // Expect "end if"
    expect(TK_KEYWORD, "end");
    expect(TK_KEYWORD, "if");
    
    return node;
}
```

#### B) parse_while() fonksiyonu ekle:

```c
static ASTNode* parse_while() {
    if (!match(TK_KEYWORD, "while")) return NULL;
    
    ASTNode *node = create_node(NODE_WHILE);
    advance();
    
    // Parse condition
    if (current_token.type == TK_IDENTIFIER || current_token.type == TK_NUMBER) {
        node->left = create_node(NODE_LITERAL);
        strcpy(node->left->value, current_token.value);
        advance();
    }
    
    // Parse body until "end while"
    while (!match(TK_KEYWORD, "end")) {
        parse_statement();
    }
    
    expect(TK_KEYWORD, "end");
    expect(TK_KEYWORD, "while");
    
    return node;
}
```

#### C) parse_statement() içine ekle:

```c
static ASTNode* parse_statement() {
    // Mevcut kod...
    
    // IF statement
    if (match(TK_KEYWORD, "if")) {
        return parse_if();
    }
    
    // WHILE statement
    if (match(TK_KEYWORD, "while")) {
        return parse_while();
    }
    
    // FOR statement (isteğe bağlı)
    if (match(TK_KEYWORD, "for")) {
        return parse_for();
    }
    
    // Mevcut kod devam...
}
```

### Adım 3: codegen.c'ye Assembly Generation Ekle (30 dakika)

**Dosya:** `melp/bootstrap/codegen.c`

#### A) gen_if() fonksiyonu:

```c
static int label_counter = 0;

static void gen_if(ASTNode *node) {
    int label_id = label_counter++;
    
    fprintf(output, "    ; if statement\n");
    
    // Condition evaluation (basit: sadece compare with 0)
    if (node->left) {
        fprintf(output, "    cmp qword [%s], 0\n", node->left->value);
    }
    
    // Jump if false
    fprintf(output, "    je .else_%d\n", label_id);
    
    // Then body (şimdilik boş)
    fprintf(output, "    ; then body\n");
    
    fprintf(output, "    jmp .end_if_%d\n", label_id);
    fprintf(output, ".else_%d:\n", label_id);
    
    // Else body (şimdilik boş)
    fprintf(output, "    ; else body\n");
    
    fprintf(output, ".end_if_%d:\n", label_id);
}
```

#### B) gen_while() fonksiyonu:

```c
static void gen_while(ASTNode *node) {
    int label_id = label_counter++;
    
    fprintf(output, ".while_start_%d:\n", label_id);
    fprintf(output, "    ; while condition\n");
    
    if (node->left) {
        fprintf(output, "    cmp qword [%s], 0\n", node->left->value);
    }
    
    fprintf(output, "    je .while_end_%d\n", label_id);
    
    // While body
    fprintf(output, "    ; while body\n");
    
    fprintf(output, "    jmp .while_start_%d\n", label_id);
    fprintf(output, ".while_end_%d:\n", label_id);
}
```

#### C) generate() fonksiyonuna ekle:

```c
void generate(ASTNode *root, FILE *out) {
    // Mevcut kod...
    
    if (node->type == NODE_IF) {
        gen_if(node);
    } else if (node->type == NODE_WHILE) {
        gen_while(node);
    }
    
    // Mevcut kod devam...
}
```

### Adım 4: Test Et (15 dakika)

#### Test 1: If statement
```mlp
function test_if() numeric
    numeric x = 5
    if x then
        numeric y = 10
    end if
    return 0
end function
```

```bash
./melp/bootstrap/melpc test_if.mlp test_if.s
```

#### Test 2: While loop
```mlp
function test_while() numeric
    numeric x = 0
    while x then
        numeric y = 1
    end while
    return 0
end function
```

```bash
./melp/bootstrap/melpc test_while.mlp test_while.s
```

---

## 📝 NOTLAR

### Basitleştirmeler
- Condition parsing çok basit (sadece identifier veya number)
- Body parsing şimdilik boş (statement'ları parse et ama ignore et)
- Nested if/while desteği basit (label counter ile)

### Zaten Çalışan Kısımlar
- ✅ Lexer if/while/for keyword'lerini tanıyor
- ✅ Token system çalışıyor
- ✅ AST node creation çalışıyor
- ✅ Variable declaration çalışıyor
- ✅ Function declaration çalışıyor
- ✅ Return statement çalışıyor
- ✅ Struct declaration çalışıyor

### Önemli Dosyalar
- `melp/bootstrap/parser.c` - Ana düzenleme burada
- `melp/bootstrap/parser.h` - Node type enum'ları
- `melp/bootstrap/codegen.c` - Assembly generation
- `melp/bootstrap/Makefile` - Derleme için

### Derleme Komutları
```bash
cd melp/bootstrap
make clean
make
./melpc test.mlp test.s
```

---

## 🎯 SONUÇ

If/while/for desteği eklendikten sonra:
1. MLP modüllerini (lexer.mlp, parser.mlp, codegen.mlp, main.mlp) gerçek implementasyonla doldur
2. Bootstrap melpc ile tüm modülleri derle
3. Linkleme yap, melpc binary'si oluştur
4. **SELF-HOSTING TEST:** `./melpc melpc_source.mlp melpc_v2.s`

**Hedef:** MLP derleyicisi MLP'de yazılmış, kendi kodunu derleyebiliyor! 🚀

---

**Bu işi sen yap, sonra commit/push et. Başarılar! 💪**
