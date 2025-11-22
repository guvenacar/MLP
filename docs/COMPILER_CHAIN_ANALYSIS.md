# MLP Derleyici Zinciri: Backend Değişiklik Analizi

**Tarih:** 22 Kasım 2025  
**Soru:** "C backend yaparsak hangi halka değişecek?"  
**Cevap:** **Sadece Generator!** 🎯

---

## 📊 Derleyici Zinciri Genel Görünüm

```
┌─────────────────────────────────────────────────────────────┐
│  1. PREPROCESSOR (dil_cevirici.py)                          │
│     Çok dilli destek → İngilizce MLP                        │
│     DEĞİŞMEYECEK ✅                                         │
└─────────────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────────┐
│  2. LEXER (c_lexer.c)                                       │
│     Kaynak kod → Token'lar                                  │
│     DEĞİŞMEYECEK ✅                                         │
└─────────────────────────────────────────────────────────────┘
                         ↓
┌─────────────────────────────────────────────────────────────┐
│  3. PARSER (c_parser.c)                                     │
│     Token'lar → AST (Abstract Syntax Tree)                  │
│     DEĞİŞMEYECEK ✅                                         │
└─────────────────────────────────────────────────────────────┘
                         ↓
                    AST (Shared)
                         ↓
        ┌────────────────┴────────────────┐
        ↓                                 ↓
┌──────────────────┐            ┌──────────────────┐
│  C BACKEND       │            │  ASM BACKEND     │
│  (c_backend.c)   │            │  (c_generator.c) │
│  YENİ! ⭐        │            │  MEVCUT ✅        │
│  AST → C Kodu    │            │  AST → Assembly  │
└──────────────────┘            └──────────────────┘
        ↓                                 ↓
    GCC/Clang                           NASM
        ↓                                 ↓
        └────────────────┬────────────────┘
                         ↓
                  Native Binary
```

---

## 🔍 Her Aşamanın Sorumluluğu

### 1. Preprocessor (dil_cevirici.py) - DEĞİŞMEYECEK ✅

**Görev:** Çok dilli keyword'leri İngilizce'ye çevir

**Girdi:**
```mlp
-- lang: tr-TR
SAYISAL x = 42;
YAZDIR x
```

**Çıktı:**
```mlp
-- lang: en-US (preprocessed)
int x = 42;
print x
```

**Neden değişmiyor?**
- Backend'den bağımsız
- Sadece keyword translation
- Her backend için aynı intermediate format

---

### 2. Lexer (c_lexer.c) - DEĞİŞMEYECEK ✅

**Görev:** Kaynak kodu Token'lara ayır

**Girdi:**
```mlp
int x = 42;
print x
```

**Çıktı (Token Stream):**
```
Token { type: TOKEN_TANIMLA_SAYI, value: "int" }
Token { type: TOKEN_IDENTIFIER, value: "x" }
Token { type: TOKEN_ASSIGN, value: "=" }
Token { type: TOKEN_SAYI, value: "42" }
Token { type: TOKEN_SEMICOLON, value: ";" }
Token { type: TOKEN_YAPI_YAZDIR, value: "print" }
Token { type: TOKEN_IDENTIFIER, value: "x" }
Token { type: TOKEN_EOF, value: NULL }
```

**Neden değişmiyor?**
- Token'lar backend'den bağımsız
- Lexical analysis aynı kalıyor
- Her backend aynı token'ları kullanır

**Örnek Lexer Kodu:**
```c
// c_lexer.c (mevcut, değişmeyecek)
Token* getNextToken(void) {
    skipWhitespace();
    
    // Keyword tanıma
    if (isalpha(current_char())) {
        char* word = readWord();
        
        if (strcmp(word, "int") == 0) {
            return createToken(TOKEN_TANIMLA_SAYI, "int");
        } else if (strcmp(word, "print") == 0) {
            return createToken(TOKEN_YAPI_YAZDIR, "print");
        }
        // ...
    }
    
    // Number tanıma
    if (isdigit(current_char())) {
        char* number = readNumber();
        return createToken(TOKEN_SAYI, number);
    }
    
    // Operator tanıma
    if (current_char() == '=') {
        return createToken(TOKEN_ASSIGN, "=");
    }
    
    // ...
}
```

---

### 3. Parser (c_parser.c) - DEĞİŞMEYECEK ✅

**Görev:** Token'ları AST'ye (Abstract Syntax Tree) dönüştür

**Girdi (Token Stream):**
```
TOKEN_TANIMLA_SAYI("int") → TOKEN_IDENTIFIER("x") → TOKEN_ASSIGN → TOKEN_SAYI("42") → TOKEN_SEMICOLON
TOKEN_YAPI_YAZDIR("print") → TOKEN_IDENTIFIER("x")
```

**Çıktı (AST):**
```
ProgramNode
├── VarDeclNode
│   ├── type: "int"
│   ├── name: "x"
│   └── initializer: LiteralNode(42)
└── PrintNode
    └── argument: IdentifierNode("x")
```

**Neden değişmiyor?**
- AST backend'den bağımsız
- Syntax analysis aynı kalıyor
- Her backend aynı AST'yi kullanır

**Örnek Parser Kodu:**
```c
// c_parser.c (mevcut, değişmeyecek)
ASTNode* parse_var_declaration(void) {
    // "int" token'ını consume et
    Token* type_token = consume(TOKEN_TANIMLA_SAYI);
    
    // Değişken adını al
    Token* name_token = consume(TOKEN_IDENTIFIER);
    
    // "=" bekle
    consume(TOKEN_ASSIGN);
    
    // Expression parse et
    ASTNode* initializer = parse_expression();
    
    // ";" bekle
    consume(TOKEN_SEMICOLON);
    
    // AST node oluştur
    ASTNode* node = createASTNode(AST_VAR_DECL);
    node->var_name = strdup(name_token->value);
    node->var_type = strdup(type_token->value);
    node->initializer = initializer;
    
    return node;
}

ASTNode* parse_print_statement(void) {
    // "print" token'ını consume et
    consume(TOKEN_YAPI_YAZDIR);
    
    // Expression parse et
    ASTNode* argument = parse_expression();
    
    // AST node oluştur
    ASTNode* node = createASTNode(AST_PRINT);
    node->print_arg = argument;
    
    return node;
}
```

**AST Yapısı (c_ast.h):**
```c
// c_ast.h (mevcut, değişmeyecek)
typedef enum {
    AST_PROGRAM,
    AST_VAR_DECL,
    AST_PRINT,
    AST_FUNCTION,
    AST_IF_STATEMENT,
    AST_WHILE_LOOP,
    AST_EXPRESSION,
    // ...
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    
    // Variable declaration
    char* var_name;
    char* var_type;
    struct ASTNode* initializer;
    
    // Print statement
    struct ASTNode* print_arg;
    
    // Function
    char* function_name;
    struct ASTNode** parameters;
    struct ASTNode* body;
    
    // If statement
    struct ASTNode* condition;
    struct ASTNode* then_branch;
    struct ASTNode* else_branch;
    
    // Children (generic)
    struct ASTNode** children;
    int child_count;
} ASTNode;
```

---

### 4A. C Backend (c_backend.c) - YENİ! ⭐

**Görev:** AST'yi C koduna çevir

**Girdi (AST):**
```
ProgramNode
├── VarDeclNode { type: "int", name: "x", value: 42 }
└── PrintNode { arg: IdentifierNode("x") }
```

**Çıktı (C Kodu):**
```c
#include <stdio.h>
#include "runtime.h"

int main(void) {
    int x = 42;
    mlp_print_int(x);
    return 0;
}
```

**Neden yeni?**
- Tamamen yeni kod generation logic
- AST → C code transformation
- Scope-based free injection
- C syntax rules

**Örnek C Backend Kodu:**
```c
// c_backend.c (YENİ!)
void generate_c_program(ASTNode* ast, FILE* output) {
    // Header
    fprintf(output, "#include <stdio.h>\n");
    fprintf(output, "#include <stdlib.h>\n");
    fprintf(output, "#include \"runtime.h\"\n\n");
    
    // Main function
    fprintf(output, "int main(void) {\n");
    
    // Body
    for (int i = 0; i < ast->child_count; i++) {
        generate_c_statement(ast->children[i], output);
    }
    
    // Cleanup (scope-based free)
    generate_scope_cleanup(output);
    
    fprintf(output, "    return 0;\n");
    fprintf(output, "}\n");
}

void generate_c_statement(ASTNode* node, FILE* output) {
    switch (node->type) {
        case AST_VAR_DECL:
            // int x = 42;
            fprintf(output, "    %s %s = ", 
                    node->var_type, node->var_name);
            generate_c_expression(node->initializer, output);
            fprintf(output, ";\n");
            break;
            
        case AST_PRINT:
            // mlp_print_int(x);
            fprintf(output, "    mlp_print_int(");
            generate_c_expression(node->print_arg, output);
            fprintf(output, ");\n");
            break;
            
        case AST_LIST_DECL:
            // MLPList* nums = mlp_list_create(sizeof(int));
            fprintf(output, "    MLPList* %s = mlp_list_create(sizeof(%s));\n",
                    node->var_name, node->element_type);
            
            // Track for cleanup
            track_heap_variable(node->var_name);
            break;
            
        // ... diğer statement'lar
    }
}

void generate_scope_cleanup(FILE* output) {
    fprintf(output, "\n    // Auto-generated cleanup\n");
    
    for (int i = 0; i < heap_var_count; i++) {
        fprintf(output, "    mlp_list_free(%s);\n", heap_vars[i]);
    }
}
```

---

### 4B. ASM Backend (c_generator.c) - MEVCUT ✅

**Görev:** AST'yi Assembly'ye çevir

**Girdi (AST):**
```
ProgramNode
├── VarDeclNode { type: "int", name: "x", value: 42 }
└── PrintNode { arg: IdentifierNode("x") }
```

**Çıktı (NASM Assembly):**
```nasm
section .data
section .bss
section .text
    global main
    extern printf

main:
    push rbp
    mov rbp, rsp
    
    ; int x = 42
    sub rsp, 8
    mov qword [rbp-8], 42
    
    ; print x
    mov rdi, [rbp-8]
    call yazdir_sayi
    
    mov rsp, rbp
    pop rbp
    mov rax, 0
    ret
```

**Neden değişmiyor?**
- Mevcut kod çalışıyor
- Legacy/Professional backend olarak kalacak
- Performans referansı
- Assembly öğrenme amaçlı

**Örnek ASM Backend Kodu (mevcut):**
```c
// c_generator.c (MEVCUT, değişmeyecek)
void generate_asm_program(ASTNode* ast, FILE* output) {
    // Section headers
    fprintf(output, "section .data\n");
    // ... data section
    
    fprintf(output, "section .text\n");
    fprintf(output, "    global main\n");
    fprintf(output, "    extern printf\n\n");
    
    fprintf(output, "main:\n");
    fprintf(output, "    push rbp\n");
    fprintf(output, "    mov rbp, rsp\n");
    
    // Body
    for (int i = 0; i < ast->child_count; i++) {
        generate_asm_statement(ast->children[i], output);
    }
    
    fprintf(output, "    mov rsp, rbp\n");
    fprintf(output, "    pop rbp\n");
    fprintf(output, "    mov rax, 0\n");
    fprintf(output, "    ret\n");
}

void generate_asm_statement(ASTNode* node, FILE* output) {
    switch (node->type) {
        case AST_VAR_DECL:
            // int x = 42
            fprintf(output, "    ; int %s = %s\n", 
                    node->var_name, node->initializer->value);
            fprintf(output, "    sub rsp, 8\n");
            fprintf(output, "    mov qword [rbp-%d], %s\n",
                    get_stack_offset(node->var_name),
                    node->initializer->value);
            break;
            
        case AST_PRINT:
            // print x
            fprintf(output, "    ; print %s\n", node->print_arg->var_name);
            fprintf(output, "    mov rdi, [rbp-%d]\n",
                    get_stack_offset(node->print_arg->var_name));
            fprintf(output, "    call yazdir_sayi\n");
            break;
            
        // ... diğer statement'lar
    }
}
```

---

## 🎯 Özet: Ne Değişiyor, Ne Değişmiyor?

### DEĞİŞMEYEN Bileşenler ✅

| Bileşen | Dosya | Sebep |
|---------|-------|-------|
| **Preprocessor** | `dil_cevirici.py` | Backend'den bağımsız keyword translation |
| **Lexer** | `c_lexer.c`, `c_lexer.h` | Backend'den bağımsız tokenization |
| **Parser** | `c_parser.c`, `c_parser.h` | Backend'den bağımsız AST construction |
| **AST Definition** | `c_ast.h` | Shared AST structure |
| **ASM Backend** | `c_generator.c`, `c_generator.h` | Legacy/Professional olarak korunuyor |

### YENİ Bileşenler ⭐

| Bileşen | Dosya | Görev |
|---------|-------|-------|
| **C Backend** | `c_backend.c`, `c_backend.h` | AST → C code generation |
| **Backend Interface** | `backend_common.h` | Common interface for backends |
| **Backend Selection** | `main.c` (güncelleme) | `--backend=c/asm` flag handling |

### GÜNCELLENEN Bileşenler 🔄

| Bileşen | Dosya | Değişiklik |
|---------|-------|------------|
| **Main** | `main.c` | Backend selection logic eklenecek |
| **Makefile** | `Makefile` | C backend build rules eklenecek |

---

## 📁 Dosya Yapısı Karşılaştırması

### Mevcut Yapı (Assembly Only)

```
c_compiler/
├── main.c                  # Entry point
├── c_lexer.c              # Tokenization
├── c_lexer.h
├── c_parser.c             # AST construction
├── c_parser.h
├── c_ast.h                # AST definitions
├── c_generator.c          # Assembly generation ⚡
├── c_generator.h
└── Makefile
```

### Yeni Yapı (Hybrid Backends)

```
c_compiler/
├── main.c                  # Entry point + backend selection 🔄
├── c_lexer.c              # Tokenization (unchanged) ✅
├── c_lexer.h
├── c_parser.c             # AST construction (unchanged) ✅
├── c_parser.h
├── c_ast.h                # AST definitions (unchanged) ✅
│
├── backend_common.h       # Backend interface ⭐ NEW
│
├── c_backend.c            # C code generation ⭐ NEW
├── c_backend.h            # C backend API ⭐ NEW
│
├── c_generator.c          # Assembly generation (unchanged) ✅
├── c_generator.h
│
└── Makefile               # Updated for C backend 🔄
```

---

## 🔨 main.c'deki Değişiklikler

### Mevcut main.c (Simplified)

```c
// main.c (MEVCUT)
int main(int argc, char* argv[]) {
    // 1. Read source file
    char* source = read_file(argv[1]);
    
    // 2. Lexer
    initLexer(source);
    Token** tokens = getAllTokens();
    
    // 3. Parser
    ASTNode* ast = parse_program(tokens);
    
    // 4. Generator (ASM only)
    FILE* output = fopen("output.asm", "w");
    generate_asm_program(ast, output);  // ⚡ Hardcoded ASM
    fclose(output);
    
    // 5. Assemble
    system("nasm -f elf64 output.asm -o output.o");
    system("gcc output.o runtime.o -o output -no-pie");
    
    return 0;
}
```

### Yeni main.c (Hybrid)

```c
// main.c (YENİ)
#include "backend_common.h"

int main(int argc, char* argv[]) {
    // 0. Parse arguments
    BackendType backend_type = BACKEND_C; // Default
    char* input_file = NULL;
    char* output_file = "output";
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--backend=asm") == 0) {
            backend_type = BACKEND_ASM;
        } else if (strcmp(argv[i], "--backend=c") == 0) {
            backend_type = BACKEND_C;
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
        }
    }
    
    // 1. Read source file
    char* source = read_file(input_file);
    
    // 2. Lexer (UNCHANGED)
    initLexer(source);
    Token** tokens = getAllTokens();
    
    // 3. Parser (UNCHANGED)
    ASTNode* ast = parse_program(tokens);
    
    // 4. Backend selection ⭐ NEW
    Backend* backend = NULL;
    
    if (backend_type == BACKEND_C) {
        backend = get_c_backend();
        printf("🔵 Using C Backend\n");
    } else {
        backend = get_asm_backend();
        printf("⚡ Using Assembly Backend\n");
    }
    
    // 5. Code generation
    backend->initialize();
    
    char output_path[256];
    sprintf(output_path, "%s%s", output_file, backend->file_extension);
    
    FILE* output = fopen(output_path, "w");
    backend->generate_program(ast, output);
    fclose(output);
    
    backend->cleanup();
    
    // 6. Compilation
    if (backend_type == BACKEND_C) {
        // Compile C code
        char cmd[512];
        sprintf(cmd, "gcc %s runtime/runtime.c -o %s -no-pie", 
                output_path, output_file);
        system(cmd);
    } else {
        // Assemble ASM code
        char cmd[512];
        sprintf(cmd, "nasm -f elf64 %s -o temp.o && gcc temp.o runtime.o -o %s -no-pie",
                output_path, output_file);
        system(cmd);
    }
    
    printf("✅ Compilation successful: %s\n", output_file);
    
    return 0;
}
```

---

## 💡 Neden Sadece Generator Değişiyor?

### 1. Frontend vs Backend Separation

**Frontend (Değişmez):**
- Lexer: Dil tokenization → Backend'den bağımsız
- Parser: Syntax analysis → Backend'den bağımsız
- AST: Abstract representation → Backend'den bağımsız

**Backend (Değişir):**
- Code generator: AST → Target code (C veya Assembly)
- Her backend farklı target language
- Ama aynı AST'yi kullanıyorlar

### 2. AST = Universal Interface

```
      ┌─────────────────────┐
      │   AST (Shared)      │
      │  ProgramNode        │
      │  ├─ VarDeclNode     │
      │  ├─ FunctionNode    │
      │  └─ PrintNode       │
      └─────────────────────┘
              ↓     ↓
       ┌──────┘     └──────┐
       ↓                   ↓
   C Backend          ASM Backend
   AST → C            AST → Assembly
```

AST her backend için aynı çünkü:
- Dil semantiği aynı
- Sadece target representation farklı
- `VarDeclNode` her zaman bir değişken tanımı (C'de `int x`, ASM'de `sub rsp, 8`)

### 3. Separation of Concerns

```
Lexer:     "MLP syntax'ını tokenlarla temsil et"
Parser:    "Token'ları anlamlı AST'ye dönüştür"
Generator: "AST'yi target dile çevir" ← Burada backend farklılaşıyor
```

Her aşama sadece kendi işini yapıyor:
- Lexer C/ASM farkını bilmez
- Parser C/ASM farkını bilmez
- Generator ise target language'e özel

---

## 🎯 Pratik Örnek: Fibonacci

### MLP Kaynak Kodu

```mlp
func fibonacci(n)
    if n <= 1 then
        return n
    end
    return fibonacci(n - 1) + fibonacci(n - 2)
end func

int result = fibonacci(10);
print result
```

### Lexer Çıktısı (Her Backend İçin Aynı)

```
TOKEN_YAPI_ISLEC, "function"
TOKEN_IDENTIFIER, "fibonacci"
TOKEN_LEFT_PAREN, "("
TOKEN_IDENTIFIER, "n"
TOKEN_RIGHT_PAREN, ")"
TOKEN_YAPI_ISE, "then"
TOKEN_YAPI_KOSUL_EGER, "if"
TOKEN_IDENTIFIER, "n"
TOKEN_LTE, "<="
TOKEN_SAYI, "1"
TOKEN_YAPI_ISE, "then"
TOKEN_YAPI_DONUS, "return"
TOKEN_IDENTIFIER, "n"
TOKEN_YAPI_SON, "end"
// ... (devamı)
```

### Parser Çıktısı (Her Backend İçin Aynı AST)

```
ProgramNode
├── FunctionNode
│   ├── name: "fibonacci"
│   ├── params: ["n"]
│   └── body:
│       └── IfNode
│           ├── condition: BinaryOp(n, "<=", 1)
│           ├── then_branch: ReturnNode(n)
│           └── else_branch: ReturnNode(
│               BinaryOp(
│                   FunctionCall("fibonacci", [n-1]),
│                   "+",
│                   FunctionCall("fibonacci", [n-2])
│               ))
├── VarDeclNode
│   ├── type: "int"
│   ├── name: "result"
│   └── initializer: FunctionCall("fibonacci", [10])
└── PrintNode
    └── argument: Identifier("result")
```

### C Backend Çıktısı

```c
#include <stdio.h>
#include "runtime.h"

int64_t fibonacci(int64_t n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(void) {
    int64_t result = fibonacci(10);
    mlp_print_int(result);
    return 0;
}
```

### ASM Backend Çıktısı

```nasm
section .text
    global main
    extern yazdir_sayi

fibonacci:
    push rbp
    mov rbp, rsp
    
    ; if n <= 1
    mov rax, [rbp+16]
    cmp rax, 1
    jg .L1
    
    ; return n
    mov rax, [rbp+16]
    jmp .L2
    
.L1:
    ; fibonacci(n-1)
    mov rax, [rbp+16]
    sub rax, 1
    push rax
    call fibonacci
    add rsp, 8
    push rax
    
    ; fibonacci(n-2)
    mov rax, [rbp+16]
    sub rax, 2
    push rax
    call fibonacci
    add rsp, 8
    
    ; add
    pop rbx
    add rax, rbx
    
.L2:
    mov rsp, rbp
    pop rbp
    ret

main:
    push rbp
    mov rbp, rsp
    
    ; int result = fibonacci(10)
    mov rdi, 10
    call fibonacci
    mov [rbp-8], rax
    
    ; print result
    mov rdi, [rbp-8]
    call yazdir_sayi
    
    mov rsp, rbp
    pop rbp
    mov rax, 0
    ret
```

**Görüldüğü gibi:**
- Lexer ve Parser çıktısı aynı
- Sadece code generation farklı
- AST her iki backend için de aynı

---

## 🎓 Sonuç

### Kısa Cevap

**Sadece Generator değişecek!** 🎯

### Detaylı Cevap

```
✅ DEĞIŞMEYECEK:
├── Preprocessor (dil_cevirici.py)
├── Lexer (c_lexer.c)
├── Parser (c_parser.c)
└── AST Definition (c_ast.h)

⭐ YENİ EKLENECEK:
├── C Backend (c_backend.c)
├── Backend Interface (backend_common.h)
└── Backend Selection (main.c'de routing)

✅ KORUNACAK:
└── ASM Backend (c_generator.c)
```

### Neden Bu Kadar Temiz?

**Derleyici mimarisi iyi tasarlanmış:**
1. Frontend (Lexer+Parser) backend'den bağımsız
2. AST universal interface rolünde
3. Backend'ler sadece code generation'dan sorumlu

**Bu sayede:**
- Lexer/Parser koduna dokunmuyoruz
- AST yapısı değişmiyor
- Sadece yeni bir backend ekliyoruz
- Mevcut ASM backend korunuyor

### Avantajlar

✅ Minimal code change  
✅ Risk düşük (mevcut kod bozulmuyor)  
✅ Kolay test (her backend ayrı test edilir)  
✅ Maintainability (her backend bağımsız)  
✅ Extensibility (gelecekte LLVM backend de eklenebilir)  

---

**© 2025 MLP Project**  
**Version:** 1.0  
**Status:** 📚 Technical Documentation  
**License:** MIT
