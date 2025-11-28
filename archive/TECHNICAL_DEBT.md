# MLP Compiler - Teknik Borç Raporu

**Tarih:** 25 Kasım 2025  
**Durum:** Parser ✅ | Codegen ⏳ | Self-Hosting %95

---

## 🚨 KRİTİK SORUNLAR (Self-Hosting'i Engelleyebilir)

### ❌ **1. Global Mutable State (Thread-Unsafe)**

**Konum:** `self_host/mlp_compiler.c` lines 200-250

```c
// Global lexer state
static char* source_code;
static int current_position;
static int current_line;
static int current_column;
static Token* current_token;
```

**Sorun:**
- Re-entrant değil (aynı anda birden fazla dosya parse edilemez)
- Nested import'larda state corruption riski
- Thread-unsafe (paralel derleme yapılamaz)

**Risk Seviyesi:** 🟡 ORTA
- Self-hosting için problem değil (tek thread)
- İleride paralel derleme eklenirse BÜY ÜK SORUN

**Çözüm:**
```c
typedef struct {
    const char* source_code;
    int position;
    int line;
    int column;
    Token* current_token;
} LexerState;

// Her parse işlemi kendi state'ini tutar
Token* lex_next(LexerState* state);
```

---

### ⚠️ **2. Hardcoded Turkish Keywords in Lexer**

**Konum:** `self_host/mlp_compiler.c` lines 1800-2000 (check_keyword)

```c
if (strcmp(word, "işlev") == 0) return TOKEN_FUNCTION;
if (strcmp(word, "eğer") == 0) return TOKEN_IF;
```

**Sorun:**
- Türkçe keyword'ler lexer'da hardcode
- Normalizer olmadan dil değişikliği imkansız
- Multi-language support için C kodunu değiştirmek gerekir

**Risk Seviyesi:** 🟢 DÜŞÜK
- Mevcut Türkçe kod tabanı için çalışıyor
- Self-hosting'e engel değil
- Ama scalability sorunu

**Çözüm:**
- Normalizer ekle: Türkçe → İngilizce base syntax
- Lexer sadece base syntax'ı parse etsin
- `diller.json` dosyasından keyword mapping okusun

---

### ⚠️ **3. Import System: Stale Token Pointer Bug (FİXLENDİ)**

**Konum:** `self_host/mlp_compiler.c` lines 6000-6090

```c
// ✅ FİXED VERSION:
// Don't restore current_token directly - it may be stale from nested imports
// Instead, get the current token at the restored position
current_token = getNextToken();
```

**Sorun (ESKİ):**
```c
// ❌ YANLIŞ (ESKI):
Token* saved_token = current_token;
parse_imported_file();
current_token = saved_token;  // Stale pointer!
```

**Durum:** ✅ FİXLENDİ (24 Kasım 2025)

**Risk Seviyesi:** 🟢 YOK (çözüldü)

---

### ⚠️ **4. Context Stack: Else-If Recursive Call (FİXLENDİ)**

**Konum:** `self_host/compiler_core/import_handler.mlp` lines 191-207

**Sorun (ESKİ):**
```mlp
if stmt.type == AST_FUNCTION then
    collect_function(ctx, stmt)
else if stmt.type == AST_ENUM then  -- Recursive kosul_komutu()
    collect_enum(ctx, stmt)
else if stmt.type == AST_STRUCT then  -- Another recursive call
    collect_struct(ctx, stmt)
end_if  -- Only ONE pop_context!
```

**Durum:** ✅ FİXLENDİ (24 Kasım 2025)
- Converted to separate if statements
- Each if has own push/pop pair

**Risk Seviyesi:** 🟢 YOK (çözüldü)

---

### ⚠️ **5. Syntax Inconsistency: "end X" vs "end_X" (FİXLENDİ)**

**Konum:** Tüm `self_host/**/*.mlp` dosyaları

**Sorun (ESKİ):**
- Bazı dosyalar: `end function` (iki kelime)
- Lexer expects: `end_function` (tek keyword)
- Parser error: "Found: end, Expected: end_function"

**Durum:** ✅ FİXLENDİ (24 Kasım 2025)
- Global replacement: 370+ değişiklik
- All files now use underscore format: `end_function`, `end_if`, etc.

**Risk Seviyesi:** 🟢 YOK (çözüldü)

---

## 🟡 ORTA SEVİYE SORUNLAR (İleride Problem Çıkarabilir)

### 1. **HashMap Method Confusion: list vs map**

**Konum:** Birçok `*.mlp` dosyası

```mlp
-- YANLIŞ (scope_manager.mlp line 123):
value = mlp_list_get(manager.variable_map, key)  -- ❌ list değil MAP!

-- DOĞRU:
value = mlp_map_get(manager.variable_map, key)   -- ✅
```

**Risk:**
- Runtime'da crash (wrong function call)
- Hafıza corruption riski

**Durum:** ⏳ KISMEN FİXLENDİ
- `scope_manager.mlp` düzeltildi
- Diğer dosyalar kontrol edilmedi

**Çözüm:**
```bash
# Tüm dosyalarda ara ve düzelt:
grep -r "mlp_list_get.*_map" self_host/compiler_core/*.mlp
grep -r "mlp_list_" self_host/compiler_core/*.mlp | grep -E "map|table|registry"
```

---

### 2. **TODO Comments: Incomplete Features**

**Konum:** Birçok dosya

```mlp
-- TODO: Track loop end labels in state (codegen_control.mlp:117)
-- TODO: Track loop start labels in state (codegen_control.mlp:131)
-- TODO: Track deferred statements (codegen_control.mlp:206)
-- TODO: Track imports properly (codegen_control.mlp:356)
-- TODO: Implement actual file reading (mlp_compiler_main.mlp:51)
```

**Risk:**
- Features eksik kalabilir
- Runtime'da unexpected behavior

**Durum:** 🟡 BİLİNMİYOR
- Hangi TODO'lar critical?
- Hangileri optional?

**Öneri:**
- Her TODO için issue aç
- Priority: CRITICAL / HIGH / LOW
- Self-hosting için gerekli mi?

---

### 3. **Naming Inconsistency: Turkish vs English**

**Konum:** AST node field names

```c
// Karışık naming:
node->fonksiyon_tanimlama_data.ad         // Türkçe
node->function_data.name                  // İngilizce
node->tanimlama_data.tip                  // Türkçe
node->struct_tanimlama_data.field_sayisi  // Karışık!
```

**Risk:**
- Code readability düşük
- Yeni geliştirici onboarding zor

**Durum:** 🟡 YAYGIN
- 50+ AST node tipi var
- Çoğunda Türkçe field names

**Çözüm:**
- Global refactor: Tüm field names → İngilizce
- Breaking change: 2-3 gün sürer

---

### 4. **Error Messages: Mixed Languages**

**Konum:** `mlp_compiler.c` parseError calls

```c
parseError("Bir ifade expected.", "Sayı, Metin or Değişken");  // Karışık!
parseError("Field tipi", "SAYISAL/METIN/BOOL/IDENTIFIER");     // Karışık!
```

**Risk:**
- User experience kötü
- Debug zorlaşır

**Durum:** 🟡 YAYGIN
- 100+ error message var
- Çoğu Türkçe-İngilizce karışık

**Çözüm:**
- i18n system: Error codes + translation table
- `ERROR_001 = "Type mismatch"` → `diller.json` → "Tip uyuşmazlığı"`

---

## 🟢 DÜŞÜK SEVİYE SORUNLAR (Cosmetic)

### 1. **Debug Output Left in Code**

**Konum:** `mlp_compiler.c` lines 6000-6020

```c
fprintf(stderr, "DEBUG: Before komut() - token type=%d...\n");
fprintf(stderr, "DEBUG: After komut(AST_IMPORT) - token type=%d...\n");
fprintf(stderr, "DEBUG: Restored lexer state - token type=%d...\n");
```

**Risk:** 🟢 YOK
- Sadece stderr'a yazıyor
- Performance impact minimal

**Çözüm:**
```c
#ifdef DEBUG_MODE
    fprintf(stderr, "DEBUG: ...\n");
#endif
```

---

### 2. **Magic Numbers**

**Konum:** Birçok yerde

```c
context_stack[100];  // Neden 100?
malloc(sizeof(Token*) * 10);  // Neden 10?
malloc(sizeof(ASTNode*) * 500);  // Neden 500?
```

**Risk:** 🟢 DÜŞÜK
- Bellek yönetimi suboptimal
- Overflow riski (çok büyük dosyalarda)

**Çözüm:**
```c
#define MAX_CONTEXT_DEPTH 100
#define INITIAL_PARAM_CAPACITY 10
#define INITIAL_BLOCK_CAPACITY 500
```

---

### 3. **Memory Leaks (Possible)**

**Konum:** AST node allocation

```c
ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
// ... kullan ...
// ❓ free() nerede?
```

**Risk:** 🟢 DÜŞÜK
- Compiler kısa ömürlü process
- OS cleanup yapar

**Durum:** 🤷 BİLİNMİYOR
- Sistematik free() yok
- Valgrind ile test edilmedi

**Öneri:**
- Arena allocator kullan
- Compile bitince toplu free

---

## 📊 ÖZET: Self-Hosting Risk Analizi

### ✅ **Self-Hosting İçin Hazır**

1. Parser phase COMPLETE ✅
2. Context stack working ✅
3. Import system functional ✅
4. Token corruption FİXED ✅
5. Syntax consistency FİXED ✅

### ⚠️ **Potansiyel Blocker'lar**

1. **Codegen import parsing error** (ŞU AN AKTIF)
   - Son kalan syntax hatası
   - 1-2 gün içinde fix edilebilir
   - Assembly generation'ı engelliyor

2. **HashMap method confusion**
   - Runtime crash riski
   - Tüm `mlp_list_get(X_map, ...)` çağrıları kontrol edilmeli

### 🎯 **Self-Hosting Sonrası Zorunlu İyileştirmeler**

1. **Normalizer ekle** (2-3 hafta)
   - Multi-language support
   - Türkçe keyword'leri lexer'dan çıkar

2. **Global state refactor** (1 hafta)
   - LexerState struct
   - Thread-safe parsing

3. **Naming standardization** (3-5 gün)
   - Tüm field names → İngilizce
   - Error messages → i18n

4. **TODO cleanup** (1-2 hafta)
   - Her TODO için issue
   - Critical olanlar implement et

---

## 🔮 **Gelecek Projeler İçin Öneriler**

### ✅ **Kesinlikle Kullan:**
- HashMap implementation (runtime/hashmap.c)
- Context stack architecture
- Token type system
- AST structure (field naming düzeltilince)
- Import pre-scan strategy

### ⚠️ **Düzelt ve Kullan:**
- Lexer (Türkçe keyword'leri çıkar)
- Parser (global state → local state)
- Error handling (i18n ekle)

### ❌ **Kullanma:**
- Hardcoded Turkish keywords
- Global mutable state pattern
- Mixed language naming

---

## 📝 **Action Items (Priority Order)**

### 🔴 **URGENT (Self-Hosting İçin Gerekli)**

- [ ] Fix remaining codegen import parsing error
- [ ] Test assembly generation
- [ ] Verify bootstrap process

### 🟡 **HIGH (Self-Hosting Sonrası İlk Hafta)**

- [ ] HashMap method audit: Fix all `mlp_list_get(X_map, ...)`
- [ ] Remove debug fprintf statements
- [ ] Add compile-time flags: `-DDEBUG_MODE`

### 🟢 **MEDIUM (1-2 Ay İçinde)**

- [ ] Implement normalizer
- [ ] Refactor global lexer state
- [ ] Standardize naming (English only)
- [ ] i18n for error messages

### 🔵 **LOW (Zaman Kalırsa)**

- [ ] Arena allocator for memory management
- [ ] Valgrind memory leak analysis
- [ ] Replace magic numbers with constants
- [ ] Parallel compilation support (thread-safe refactor)

---

**Sonuç:** Mevcut sistem %95 hazır. Son 1-2 bug fix ile self-hosting tamamlanır. Ancak production-ready olması için 4-6 haftalık refactor gerekir.
