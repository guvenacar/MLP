# C FFI Status - mlp-original

**Tarih:** 24 Aralık 2025  
**Durum:** 🟡 Partial / Planned

---

## 📊 MEVCUT DURUM

### ✅ Var Olanlar

1. **Runtime C Fonksiyonları (Hardcoded)**
   - Location: `self_host/mlp_compiler.c` lines 9349-9366
   - Hardcoded `extern` declarations in assembly output:
     ```asm
     extern printf
     extern malloc
     extern free
     extern strlen, strcmp, strstr
     extern tyd_substr, tyd_strcat
     extern dosya_ac, dosya_oku, dosya_yaz, dosya_kapat
     extern string_karakter_al, string_alt
     extern karakter_kodu, kodu_karaktere
     extern runtime_dizin_al
     ```

2. **Extern Syntax Örnekleri (Yorum Satırlarında)**
   - Files: `self_host/runtime/*.mlp`
   - Examples:
     ```mlp
     -- extern hashmap_create(numeric capacity) -> pointer
     -- extern hashmap_set(pointer map, pointer key, pointer value)
     -- extern async_create_promise() -> pointer
     -- extern GC_malloc(numeric size) -> pointer
     ```

### ❌ Eksikler

1. **Lexer/Parser Desteği**
   - `extern` keyword lexer'da tanımlı değil
   - `TOKEN_EXTERN` yok
   - `AST_EXTERN_FUNCTION` node type yok

2. **Codegen Desteği**
   - Dynamic C function binding yok
   - User-defined extern declarations desteklenmiyor
   - Sadece hardcoded runtime functions var

3. **Type System**
   - C types → MLP types mapping eksik
   - Pointer safety checks yok
   - ABI compatibility layer yok

---

## 🎯 FFI EKLEMEK İÇİN YAPILMASI GEREKENLER

### Phase 1: Lexer/Parser (2-3 saat)
```c
// TOKEN_EXTERN ekle
if (strcmp(keyword, "extern") == 0) return TOKEN_EXTERN;

// AST node type
typedef struct {
    char* function_name;
    char* return_type;
    ASTNode** params;
    int param_count;
} ExternFunctionData;
```

### Phase 2: Codegen (3-4 saat)
```c
void visit_ExternFunction(ASTNode* node) {
    char buffer[256];
    sprintf(buffer, "extern %s", node->extern_data.function_name);
    asm_append(&data_section, buffer);
    
    // Register in symbol table
    register_extern_function(node->extern_data.function_name);
}
```

### Phase 3: Runtime Linking (1-2 saat)
- Dynamic library loading (.so/.dylib)
- Symbol resolution
- dlopen/dlsym integration

---

## 💡 WORKAROUND: Şu An Nasıl C Fonksiyonu Çağrılır?

### Yöntem 1: Hardcoded Runtime Functions
```mlp
-- mlp_compiler.c'de tanımlı olanları direkt çağır
sonuc = malloc(100)
metin = strlen("hello")
```

### Yöntem 2: Inline Assembly (Eğer varsa)
```mlp
-- MLP'de inline asm desteği yoksa bu çalışmaz
assembly "call my_c_function"
```

### Yöntem 3: Wrapper C File
```c
// custom_runtime.c
void my_custom_function() {
    // C code here
}

// Compile & link:
// gcc -c custom_runtime.c
// gcc mlp_output.asm custom_runtime.o -o program
```

---

## 📈 ROADMAP'TE FFI VAR MI?

❌ **Hayır** - ROADMAP.md'de C FFI feature planı yok  
✅ **Ama:** Async/GC/HashMap için C runtime fonksiyonları var

**Not:** Self-hosting öncelikli, FFI Phase 10+ olabilir

---

## 🔍 ÖNERİ

mlp-original için FFI **şu an kritik değil** çünkü:
1. ✅ Self-hosting için yeterli runtime var
2. ✅ Hardcoded C functions ile async/GC çalışıyor
3. ❌ Generic FFI eklemek 6-10 saat iş

**Öncelik:** Self-hosting'i bitir, sonra FFI ekle

---

## 📝 KARŞILAŞTIRMA: Diğer Dillerde FFI

### Python
```python
from ctypes import *
libc = CDLL("libc.so.6")
libc.printf(b"Hello %s\n", b"world")
```

### Lua
```lua
local ffi = require("ffi")
ffi.cdef[[
    int printf(const char *fmt, ...);
]]
ffi.C.printf("Hello %s\n", "world")
```

### MLP (Planlanan Syntax)
```mlp
extern printf(metin format, ...) -> numeric

function main()
    printf("Hello %s\n", "world")
end_function
```

---

**Sonuç:** C FFI **syntax planned ✅**, **implementation missing ❌**
