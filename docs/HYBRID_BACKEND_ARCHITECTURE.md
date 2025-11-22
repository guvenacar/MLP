# MLP Hibrit Backend Mimarisi

**Tarih:** 22 Kasım 2025  
**Versiyon:** 1.0  
**Durum:** 🎯 Uygulama Planı

---

## 🎯 Strateji: Her İki Dünyadan da Faydalan

### Vizyon

```
                    MLP Kaynak Kodu
                           ↓
                    Preprocessor
                           ↓
                    Lexer + Parser
                           ↓
                       AST (Shared)
                           ↓
              ┌────────────┴────────────┐
              ↓                         ↓
         C Backend                  ASM Backend
         (Default)                  (Legacy/Pro)
              ↓                         ↓
         C Code                    NASM Assembly
              ↓                         ↓
        GCC/Clang                      NASM
              ↓                         ↓
              └────────────┬────────────┘
                           ↓
                    Native Binary
```

### Felsefi Yaklaşım

**C Backend:** "Demokratik" - Herkes katkıda bulunabilir, kolay öğrenilir  
**ASM Backend:** "Profesyonel" - Performans odaklı, deneyimli katkıcılar için

---

## 📋 İki Backend'in Rolleri

### C Backend (Varsayılan) - "Community Edition"

**Hedef Kitle:**
- Yeni başlayanlar
- Hızlı prototipleme
- Cross-platform geliştirme
- Bellek yönetimi öğrenenler

**Avantajları:**
- ✅ Kolay katkı (sadece C bilgisi yeterli)
- ✅ Otomatik bellek yönetimi (scope-based free)
- ✅ GCC optimizasyonu
- ✅ Tüm platformlar (x86, ARM, RISC-V, Windows, macOS, Linux)
- ✅ Debugging kolay (GDB, Valgrind)
- ✅ 500 satır kod (maintainable)

**Kullanım Senaryoları:**
```bash
# Günlük geliştirme
mlpc myapp.mlp -o myapp

# Cross-compile (ARM için)
mlpc myapp.mlp -o myapp --target=arm64

# Debug build
mlpc myapp.mlp -o myapp --debug

# C kodunu göster
mlpc myapp.mlp --emit-c -o myapp.c
```

**Katkı Süreci:**
```
1. Issue aç: "list<string> desteği eksik"
2. c_backend.c'yi fork et
3. generate_list_string() fonksiyonu ekle
4. Test yaz
5. Pull request
6. Review (kolay, C kodu okunabilir)
7. Merge
```

### ASM Backend (Advanced) - "Professional Edition"

**Hedef Kitle:**
- Assembly bilgisi olanlar
- Performans kritik uygulamalar
- Sistem programlama
- Öğretim (derleyici + assembly eğitimi)

**Avantajları:**
- ✅ Tam kontrol (her instruction kontrol altında)
- ✅ Minimum overhead
- ✅ Öğretici (assembly öğrenme)
- ✅ Benchmark reference (C backend'i test etmek için)

**Dezavantajları:**
- ⚠️ Sadece x86-64 Linux (şimdilik)
- ⚠️ 2500+ satır kod (complex)
- ⚠️ Bellek yönetimi manuel
- ⚠️ Katkı zorluğu (assembly bilgisi şart)

**Kullanım Senaryoları:**
```bash
# Maximum performance
mlpc myapp.mlp -o myapp --backend=asm

# Assembly göster
mlpc myapp.mlp --backend=asm --emit-asm -o myapp.asm

# Benchmark
mlpc myapp.mlp -o myapp_c --backend=c
mlpc myapp.mlp -o myapp_asm --backend=asm
time ./myapp_c
time ./myapp_asm
```

**Katkı Süreci:**
```
1. Issue aç: "SIMD optimization for arrays"
2. c_generator.c'yi fork et (dikkat: assembly!)
3. generate_array_add_simd() fonksiyonu ekle
4. Test yaz (assembly output doğrulaması)
5. Pull request
6. Review (zor, assembly expertise gerekiyor)
7. Merge (dikkatli)
```

---

## 🏗️ Mimari Tasarım

### Dosya Yapısı

```
c_compiler/
├── main.c                    # Backend routing
├── c_lexer.c                 # Shared
├── c_parser.c                # Shared
├── c_ast.h                   # Shared AST definitions
│
├── c_backend.c               # ⭐ YENİ: C code generator
├── c_backend.h               # ⭐ YENİ: C backend API
│
├── c_generator.c             # MEVCUT: Assembly generator (rename edilebilir)
├── c_generator.h             # MEVCUT: Assembly backend API
│
└── backend_common.h          # ⭐ YENİ: Common interface
```

### Common Interface (backend_common.h)

```c
// backend_common.h
#ifndef BACKEND_COMMON_H
#define BACKEND_COMMON_H

#include "c_ast.h"

// Backend interface - Her backend bunu implement eder
typedef struct {
    const char* name;           // "C" veya "Assembly"
    const char* file_extension; // ".c" veya ".asm"
    
    // Backend fonksiyonları
    void (*initialize)(void);
    void (*generate_program)(ASTNode* root, const char* output_file);
    void (*cleanup)(void);
    
    // Backend özellikleri
    bool supports_auto_free;      // Otomatik free desteği var mı?
    bool supports_cross_platform; // Cross-platform derlenebilir mi?
    bool supports_debugging;      // Debug symbols üretir mi?
} Backend;

// Backend registry
Backend* get_c_backend(void);
Backend* get_asm_backend(void);

#endif
```

### Backend Selection Logic (main.c)

```c
// main.c
#include "backend_common.h"

typedef enum {
    BACKEND_C,      // Varsayılan
    BACKEND_ASM,    // Legacy/Professional
    BACKEND_AUTO    // Otomatik seçim
} BackendType;

int main(int argc, char* argv[]) {
    // 1. Parse arguments
    BackendType backend_type = BACKEND_C; // Varsayılan
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--backend=asm") == 0) {
            backend_type = BACKEND_ASM;
        } else if (strcmp(argv[i], "--backend=c") == 0) {
            backend_type = BACKEND_C;
        } else if (strcmp(argv[i], "--backend=auto") == 0) {
            backend_type = BACKEND_AUTO;
        }
    }
    
    // 2. Auto-selection logic
    if (backend_type == BACKEND_AUTO) {
        // Platform kontrolü
        #ifdef _WIN32
            backend_type = BACKEND_C; // Windows'da sadece C
        #elif defined(__APPLE__)
            backend_type = BACKEND_C; // macOS'ta sadece C
        #elif defined(__linux__) && defined(__x86_64__)
            backend_type = BACKEND_ASM; // Linux x86-64'te ASM
        #else
            backend_type = BACKEND_C; // Diğer her şey C
        #endif
    }
    
    // 3. Backend seç ve başlat
    Backend* backend = NULL;
    
    switch (backend_type) {
        case BACKEND_C:
            backend = get_c_backend();
            printf("🔵 Using C Backend (Community Edition)\n");
            break;
        case BACKEND_ASM:
            backend = get_asm_backend();
            printf("⚡ Using Assembly Backend (Professional Edition)\n");
            break;
        default:
            fprintf(stderr, "Unknown backend type\n");
            return 1;
    }
    
    // 4. Lexer + Parser (Shared)
    Token* tokens = lexer_tokenize(source_code);
    ASTNode* ast = parser_parse(tokens);
    
    // 5. Backend'e yönlendir
    backend->initialize();
    backend->generate_program(ast, output_file);
    backend->cleanup();
    
    // 6. Post-processing
    if (backend_type == BACKEND_C) {
        // C dosyasını derle
        printf("🔨 Compiling C code with GCC...\n");
        compile_c_to_binary(output_file);
    } else {
        // Assembly'yi derle
        printf("🔨 Assembling with NASM...\n");
        assemble_to_binary(output_file);
    }
    
    return 0;
}
```

### C Backend Implementation (c_backend.c)

```c
// c_backend.c
#include "c_backend.h"
#include "backend_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Scope tracking (otomatik free için)
typedef struct {
    char* var_name;
    char* var_type;
    bool needs_free; // list<T> gibi heap allocation'lar için
} ScopeVariable;

static ScopeVariable scope_stack[100];
static int scope_depth = 0;

// C backend initialization
static void c_backend_init(void) {
    scope_depth = 0;
    printf("C Backend initialized\n");
}

// Main generation function
static void c_backend_generate(ASTNode* root, const char* output_file) {
    FILE* f = fopen(output_file, "w");
    if (!f) {
        fprintf(stderr, "Error: Cannot create output file\n");
        return;
    }
    
    // 1. Header
    fprintf(f, "// Generated by MLP Compiler (C Backend)\n");
    fprintf(f, "// Date: %s\n\n", __DATE__);
    fprintf(f, "#include <stdio.h>\n");
    fprintf(f, "#include <stdlib.h>\n");
    fprintf(f, "#include \"runtime.h\"\n\n");
    
    // 2. Generate functions
    generate_ast_node(f, root);
    
    fclose(f);
}

// Generate individual AST nodes
static void generate_ast_node(FILE* f, ASTNode* node) {
    switch (node->type) {
        case AST_PROGRAM:
            // Main program
            fprintf(f, "int main(void) {\n");
            for (int i = 0; i < node->child_count; i++) {
                generate_ast_node(f, node->children[i]);
            }
            
            // Cleanup scope
            generate_scope_cleanup(f);
            
            fprintf(f, "    return 0;\n");
            fprintf(f, "}\n");
            break;
            
        case AST_VAR_DECL:
            generate_var_declaration(f, node);
            break;
            
        case AST_LIST_DECL:
            generate_list_declaration(f, node);
            break;
            
        case AST_PRINT:
            generate_print(f, node);
            break;
            
        case AST_FUNCTION:
            generate_function(f, node);
            break;
            
        // ... diğer node tipleri
    }
}

// List declaration with auto-cleanup tracking
static void generate_list_declaration(FILE* f, ASTNode* node) {
    char* var_name = node->var_name;
    char* element_type = node->element_type;
    
    // Generate creation
    fprintf(f, "    MLPList* %s = mlp_list_create(sizeof(%s));\n",
            var_name, element_type);
    
    // Track for cleanup (ÖNEMLİ!)
    scope_stack[scope_depth].var_name = strdup(var_name);
    scope_stack[scope_depth].var_type = strdup("list");
    scope_stack[scope_depth].needs_free = true;
    scope_depth++;
}

// Scope cleanup (scope sonunda otomatik çağrılır)
static void generate_scope_cleanup(FILE* f) {
    fprintf(f, "\n    // Auto-generated cleanup\n");
    
    for (int i = scope_depth - 1; i >= 0; i--) {
        if (scope_stack[i].needs_free) {
            if (strcmp(scope_stack[i].var_type, "list") == 0) {
                fprintf(f, "    mlp_list_free(%s);\n",
                        scope_stack[i].var_name);
            }
            // Gelecek: string, hashmap, vs.
        }
    }
}

// Backend cleanup
static void c_backend_cleanup(void) {
    // Scope stack'i temizle
    for (int i = 0; i < scope_depth; i++) {
        free(scope_stack[i].var_name);
        free(scope_stack[i].var_type);
    }
    scope_depth = 0;
}

// Backend interface implementation
static Backend c_backend_interface = {
    .name = "C",
    .file_extension = ".c",
    .initialize = c_backend_init,
    .generate_program = c_backend_generate,
    .cleanup = c_backend_cleanup,
    .supports_auto_free = true,
    .supports_cross_platform = true,
    .supports_debugging = true
};

Backend* get_c_backend(void) {
    return &c_backend_interface;
}
```

### ASM Backend Wrapper (c_generator.c'ye eklenecek)

```c
// c_generator.c (mevcut assembly generator'a ekleme)

// En üste:
#include "backend_common.h"

// En altta:
static Backend asm_backend_interface = {
    .name = "Assembly",
    .file_extension = ".asm",
    .initialize = generator_init,  // Mevcut init fonksiyonu
    .generate_program = generate_program,  // Mevcut generate fonksiyonu
    .cleanup = generator_cleanup,  // Mevcut cleanup fonksiyonu
    .supports_auto_free = false,   // Manuel bellek yönetimi
    .supports_cross_platform = false, // Sadece x86-64 Linux
    .supports_debugging = false    // Assembly debugging zor
};

Backend* get_asm_backend(void) {
    return &asm_backend_interface;
}
```

---

## 🚀 Uygulama Planı (Fazlar)

### Faz 1: Temel Hibrit Mimari (2-3 gün)

**Görevler:**
1. ✅ `backend_common.h` oluştur
2. ✅ `c_backend.c` ve `c_backend.h` oluştur
3. ✅ `main.c`'de backend selection ekle
4. ✅ Basit test: `int x = 5; print x;`

**Test:**
```bash
# C backend
./mlpc test.mlp -o test --backend=c
./test  # Çalışmalı

# ASM backend (mevcut)
./mlpc test.mlp -o test --backend=asm
./test  # Çalışmalı
```

### Faz 2: C Backend Temel Özellikler (3-4 gün)

**Eklenecekler:**
- ✅ Variable declarations (int, string)
- ✅ Expressions (arithmetic, comparison)
- ✅ Control flow (if/else, while)
- ✅ Functions (definition, call)
- ✅ Print statement

**Test:**
```mlp
function add(a, b) then
    return a + b
end

int x = 5;
int y = 10;
int result = add(x, y);
print result  # 15
```

### Faz 3: Scope-based Free Injection (3-4 gün)

**Özellik:** Otomatik bellek yönetimi

**MLP Kodu:**
```mlp
function test() then
    list<int> nums = list<int>();
    nums.add(42);
    nums.add(100);
    
    if true then
        list<int> temp = list<int>();
        temp.add(1);
        -- temp burada free edilmeli
    end
    
    -- nums burada free edilmeli
end
```

**Üretilen C:**
```c
void test() {
    MLPList* nums = mlp_list_create(sizeof(int));
    int temp_42 = 42;
    mlp_list_add(nums, &temp_42);
    int temp_100 = 100;
    mlp_list_add(nums, &temp_100);
    
    if (1) {
        MLPList* temp = mlp_list_create(sizeof(int));
        int temp_1 = 1;
        mlp_list_add(temp, &temp_1);
        
        // ✨ Otomatik cleanup (inner scope)
        mlp_list_free(temp);
    }
    
    // ✨ Otomatik cleanup (function scope)
    mlp_list_free(nums);
}
```

### Faz 4: Control Flow Free (2-3 gün)

**Zorluk:** Early return/break durumları

**MLP:**
```mlp
function search(target) then
    list<int> data = list<int>();
    data.add(1);
    data.add(2);
    
    if target == 1 then
        return 0  -- ⚠️ data leak!
    end
    
    return -1
end
```

**Üretilen C (Cleanup Label Pattern):**
```c
int search(int target) {
    MLPList* data = mlp_list_create(sizeof(int));
    int temp_1 = 1;
    mlp_list_add(data, &temp_1);
    int temp_2 = 2;
    mlp_list_add(data, &temp_2);
    
    int __return_value;
    
    if (target == 1) {
        __return_value = 0;
        goto __cleanup;  // ✅ Jump to cleanup
    }
    
    __return_value = -1;
    
__cleanup:
    mlp_list_free(data);  // ✅ Always executed
    return __return_value;
}
```

### Faz 5: Test Suite ve Benchmark (2-3 gün)

**Test Kategorileri:**

1. **Basic Tests:**
   - Variables (int, string)
   - Arithmetic (+, -, *, /)
   - Comparisons (==, !=, <, >)
   - Print

2. **Control Flow Tests:**
   - if/else
   - while loops
   - Nested blocks

3. **Function Tests:**
   - Simple functions
   - Recursion (fibonacci)
   - Multiple parameters

4. **Memory Tests:**
   - list<T> creation/destruction
   - Nested scopes
   - Early returns

5. **Benchmark:**
   ```bash
   # Fibonacci(35) - Recursive
   time ./test_fib_c      # C backend
   time ./test_fib_asm    # ASM backend
   
   # List operations (10000 elements)
   time ./test_list_c     # C backend
   time ./test_list_asm   # ASM backend
   ```

### Faz 6: Dokümantasyon (1-2 gün)

**Güncellenecek Dokümanlar:**

1. **README.md:**
   ```markdown
   ## Backend Options
   
   MLP supports two backends:
   
   - **C Backend** (Default): Cross-platform, auto memory management
   - **Assembly Backend**: x86-64 Linux, manual memory management
   
   Usage:
   ```bash
   mlpc program.mlp -o output              # C backend (default)
   mlpc program.mlp -o output --backend=c  # Explicit C
   mlpc program.mlp -o output --backend=asm # Assembly
   ```
   ```

2. **SPECS.md:**
   ```markdown
   ## Compiler Backends
   
   ### C Backend (Recommended)
   - Target: All platforms with GCC/Clang
   - Memory: Automatic scope-based free
   - Debug: Full GDB support
   
   ### Assembly Backend (Advanced)
   - Target: x86-64 Linux only
   - Memory: Manual management
   - Debug: Limited
   ```

3. **BACKEND_GUIDE.md** (YENİ):
   - Hangi durumda hangi backend?
   - Performance comparison
   - Katkıda bulunma rehberi

---

## 📊 Backend Seçim Rehberi

### C Backend'i Ne Zaman Kullanmalı?

✅ **Geliştirme aşamasında** - Hızlı iterasyon  
✅ **Cross-platform** - Windows, macOS, Linux, ARM  
✅ **Öğrenme** - Derleyici nasıl çalışır?  
✅ **Bellek yönetimi öğrenme** - Scope-based free  
✅ **Katkıda bulunma** - Kolay C kodu  

### ASM Backend'i Ne Zaman Kullanmalı?

✅ **Maximum performance** - Her mikrosaniye önemli  
✅ **x86-64 Linux** - Hedef platform belli  
✅ **Assembly öğrenme** - Eğitim amaçlı  
✅ **Benchmark reference** - C backend'i test etmek için  
✅ **Uzman katkıcılar** - Assembly bilgisi var  

### Otomatik Seçim (--backend=auto)

```bash
mlpc program.mlp --backend=auto
```

**Algoritma:**
```c
if (platform == WINDOWS || platform == MACOS) {
    use C_BACKEND;
} else if (platform == LINUX && arch == X86_64) {
    use ASM_BACKEND;  // Native platform
} else {
    use C_BACKEND;    // Fallback
}
```

---

## 🎯 Başarı Metrikleri

### Faz 1-2 Sonunda (Temel Hibrit)

- [ ] Her iki backend de basit programları derleyebiliyor
- [ ] `--backend=c` ve `--backend=asm` bayrakları çalışıyor
- [ ] Fibonacci test geçiyor

### Faz 3-4 Sonunda (Otomatik Free)

- [ ] C backend list<T> için otomatik free yapıyor
- [ ] Early return durumları doğru handle ediliyor
- [ ] Memory leak yok (Valgrind temiz)

### Faz 5 Sonunda (Test & Benchmark)

- [ ] 50+ test case her iki backend'de de geçiyor
- [ ] Performance karşılaştırması var
- [ ] Hangi durumda hangisi hızlı, biliniyor

### Faz 6 Sonunda (Dokümantasyon)

- [ ] Kullanıcılar hangi backend'i seçeceklerini biliyorlar
- [ ] Katkıda bulunma rehberi hazır
- [ ] Örnekler her iki backend için de var

---

## 🤝 Topluluk Stratejisi

### C Backend → Açık Katkı

**Hedef:** Herkesin katkıda bulunabileceği, demokratik geliştirme

**Katkı Süreci:**
1. Issue aç: "Feature X eksik"
2. Fork: `c_backend.c`
3. Implement: C kodu yaz (kolay)
4. Test: `make test-c-backend`
5. PR: Pull request aç
6. Review: Maintainer'lar kolayca anlıyor (C okunabilir)
7. Merge: Hızlı kabul

**Örnekler:**
- "string concatenation desteği"
- "for loop syntactic sugar"
- "array type ekleme"

### ASM Backend → Uzman Katkı

**Hedef:** Performans odaklı, deneyimli katkıcılar

**Katkı Süreci:**
1. Issue aç: "SIMD optimization for arrays"
2. Fork: `c_generator.c`
3. Implement: Assembly kodu yaz (zor)
4. Test: `make test-asm-backend` + manual verification
5. PR: Pull request aç
6. Review: Assembly expertise gerekiyor
7. Merge: Dikkatli kabul (breaking riski yüksek)

**Örnekler:**
- "AVX2 vectorization"
- "Cache-friendly struct layout"
- "Custom calling convention"

### Dokümantasyon Stratejisi

**Yeni Başlayanlar İçin:**
```markdown
# MLP'ye Katkıda Bulunma

## C Backend (Kolay)
Eğer C biliyorsanız, hemen başlayabilirsiniz!

1. `c_backend.c` dosyasına bakın
2. Eksik bir feature bulun
3. C kodu yazın
4. Test edin
5. PR gönderin

## Assembly Backend (İleri Seviye)
x86-64 assembly bilgisi gerekiyor.

1. `c_generator.c` dosyasına bakın
2. Performans optimizasyonu yapın
3. NASM assembly yazın
4. Benchmark edin
5. PR gönderin
```

---

## 💡 İleri Seviye Özellikler (Gelecek)

### Faz 7: LLVM Backend (3-6 ay sonra)

```
MLP → AST → LLVM IR → Binary
```

**Avantajlar:**
- World-class optimizasyon
- WebAssembly desteği
- GPU compilation (CUDA, ROCm)

**Kullanım:**
```bash
mlpc program.mlp --backend=llvm -O3
```

### Faz 8: JIT Compilation (6-12 ay sonra)

```
MLP → Bytecode → JIT → Native
```

**Kullanım:**
```bash
mlp-jit script.mlp  # Anında çalıştır
```

### Faz 9: Multiple C Standards (3-6 ay sonra)

```bash
mlpc program.mlp --backend=c --std=c99   # Eski sistemler
mlpc program.mlp --backend=c --std=c11   # Modern C
mlpc program.mlp --backend=c --std=c++17 # C++ backend
```

---

## 🎓 Sonuç

**Hibrit Sistem = Best of Both Worlds**

| Özellik | C Backend | ASM Backend |
|---------|-----------|-------------|
| **Kullanım** | Günlük geliştirme | Performans kritik |
| **Katkı** | Herkes | Uzmanlar |
| **Platform** | Hepsi | x86-64 Linux |
| **Bellek** | Otomatik | Manuel |
| **Öğrenme** | Kolay | Zor |
| **Gelecek** | LLVM'e geçiş | Legacy/Educational |

**Strateji:**
1. **Şimdi:** C backend'i varsayılan yap
2. **6 ay:** ASM backend legacy olsun ama korunsun
3. **1 yıl:** LLVM backend ekle
4. **2 yıl:** Kullanıcılar seçsin: c, llvm, asm

**Topluluk:**
- C backend → Açık katkı, hızlı büyüme
- ASM backend → Özel ilgi, performans takımı
- LLVM backend → Profesyonel proje, production-ready

---

**© 2025 MLP Project**  
**Version:** 1.0  
**Status:** 🚀 Implementation Ready  
**License:** MIT
