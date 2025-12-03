# MELP Modüler Yapı Güncellemesi

## Tarih: 30 Kasım 2024

## Özet

Bu çalışmada MELP derleyicisi hem C bootstrap hem de MLP self-host için modüler hale getirildi.

## Yapılan Değişiklikler

### 1. PMLP Standardizasyonu: `text` → `string`

**Etkilenen dosyalar:**
- `MLP_LANGUAGE_SPEC.md` - Spec güncellendi
- `bootstrap/lexer.c` - `TOKEN_TEXT` → `TOKEN_STRING_TYPE`, keyword `"text"` → `"string"`
- `bootstrap/parser.c` - Tüm `TOKEN_TEXT` referansları güncellendi
- `bootstrap/codegen.c` - Tip ismi ve fonksiyon mapping'leri güncellendi
- `runtime/runtime.c` - `mlp_to_text()` → `mlp_to_string()`, `mlp_decimal_to_text()` → `mlp_decimal_to_string()`
- `selfhost/*.mlp` dosyaları - `text` → `string` değiştirildi

### 2. C Bootstrap Modüler Include Dosyaları

`bootstrap/codegen_inc/` klasörü oluşturuldu:

| Dosya | Açıklama |
|-------|----------|
| `codegen_variable.inc` | Variable management fonksiyonları |
| `codegen_function.inc` | Function table yönetimi |
| `codegen_struct.inc` | Struct ve interface yönetimi |
| `codegen_if.inc` | If/else kod üretimi |
| `codegen_for.inc` | For ve for-in döngü üretimi |
| `codegen_while.inc` | While ve do-while üretimi |
| `codegen_switch.inc` | Switch/case üretimi |

Bu dosyalar:
- C bootstrap için referans görevi görür
- MLP self-host çevirisi için şablon olarak kullanılabilir
- Monolitik `codegen.c` hala çalışır durumda (değiştirilmedi)

### 3. MLP Self-Host Modüler Codegen Dosyaları

`selfhost/codegen/` klasörü:

| Dosya | Açıklama |
|-------|----------|
| `if.mlp` | If statement codegen |
| `for.mlp` | For ve for-in loop codegen |
| `while.mlp` | While ve do-while codegen |
| `switch.mlp` | Switch/case codegen |

Her dosya:
- Karşılık gelen C .inc dosyasından MLP'ye çevrilmiş
- State değişkenler ile global erişim sağlar
- Helper fonksiyonlar ile AST erişimi modüler

## Test Sonuçları

```bash
# Test: string keyword
echo 'string name = "Test"
print(name)' > test.mlp
./melp-bootstrap test.mlp test.s
# Output: Test
```

✅ Bootstrap derleyici çalışıyor
✅ `string` keyword tanınıyor
✅ Kod üretimi doğru

## Sonraki Adımlar

1. **Modüler codegen entegrasyonu**: `codegen.c`'yi .inc dosyalarını include edecek şekilde refactor et
2. **Selfhost tamamlama**: Eksik codegen modüllerini ekle (expr, decl, print, etc.)
3. **Test suite**: Modüler dosyalar için birim testleri
4. **Self-hosting milestone**: MLP ile yazılmış derleyicinin ilk çalışan versiyonu

## Dosya Yapısı

```
melp/
├── bootstrap/
│   ├── lexer.c          # Lexer (TOKEN_STRING_TYPE)
│   ├── parser.c         # Parser
│   ├── parser.h         # Parser header (tip tanımları)
│   ├── codegen.c        # Monolitik codegen (aktif)
│   ├── codegen_backup.c # Yedek
│   ├── codegen_inc/     # Modüler include dosyaları
│   │   ├── codegen_variable.inc
│   │   ├── codegen_function.inc
│   │   ├── codegen_struct.inc
│   │   ├── codegen_if.inc
│   │   ├── codegen_for.inc
│   │   ├── codegen_while.inc
│   │   └── codegen_switch.inc
│   └── main.c           # Ana dosya
├── selfhost/
│   ├── lexer.mlp        # MLP ile yazılmış lexer
│   ├── parser/
│   │   ├── types.mlp    # AST tip tanımları
│   │   ├── expr.mlp     # Expression parser
│   │   └── if.mlp       # If parser
│   └── codegen/
│       ├── if.mlp       # If codegen
│       ├── for.mlp      # For codegen
│       ├── while.mlp    # While codegen
│       └── switch.mlp   # Switch codegen
└── runtime/
    └── runtime.c        # C runtime (mlp_to_string, etc.)
```
