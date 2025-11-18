# MLP Preprocessor Design Specification

## Overview

The MLP preprocessor translates multi-language source code into standardized English keywords before compilation. This enables developers to write code in their native language.

## Architecture

```
Source Code (any language)
    ↓
Preprocessor (dil_cevirici.mlp)
    ↓
English Keywords (standardized)
    ↓
Lexer → Parser → Generator
```

## Language Selection

### Method 1: File Header Directive
```mlp
-- lang: tr-TR
SAYISAL x = 5;
EĞER x > 0 İSE
    YAZDIR "Pozitif"
SON
```

### Method 2: File Extension
```
program.tr.mlp  → Turkish
program.ru.mlp  → Russian
program.zh.mlp  → Chinese
program.mlp     → Default (English)
```

### Method 3: Global Config
```json
// .mlp-config.json
{
  "default_language": "tr-TR",
  "strict_mode": true
}
```

## Translation Rules

### 1. Keyword Translation
```
Input:  EĞER x > 0 İSE
Output: if x > 0 then
```

### 2. String Literal Protection
Strings are NOT translated:
```
Input:  YAZDIR "EĞER bu değişmez"
Output: print "EĞER bu değişmez"
         ^     ^^^^^^^^^^^^^^^^^
         ✓     Protected (no change)
```

### 3. Comment Preservation
Comments are preserved as-is:
```
Input:  -- YAZDIR "test"  ← Yorum satırı
Output: -- YAZDIR "test"  ← Yorum satırı
```

### 4. Identifier Protection
Variable/function names are NOT translated:
```
Input:  YAZDIR_debug()  ← Function name
Output: YAZDIR_debug()  ← NOT translated
```

## Implementation Strategy

### Phase 1: Simple Token Replacement (Current Goal)
- Read diller.json
- Tokenize with state machine (CODE/STRING/COMMENT states)
- Replace only CODE-state keywords
- Write preprocessed output

### Phase 2: Advanced Features
- Multi-language mixing in single file
- Custom user dictionaries
- Syntax sugar extensions
- Macro support

## Preprocessor Workflow

```python
def preprocess(source_code, language_id):
    # 1. Load language dictionary
    lang = load_language(language_id)

    # 2. State machine tokenization
    state = STATE_CODE
    output = []

    for token in tokenize(source_code):
        if state == STATE_STRING:
            output.append(token)  # Keep as-is
        elif state == STATE_COMMENT:
            output.append(token)  # Keep as-is
        elif state == STATE_CODE:
            # Translate keyword if found
            translated = lang.translate(token)
            output.append(translated)

    return "".join(output)
```

## Example Translation

### Input (Turkish):
```mlp
-- lang: tr-TR

SAYISAL toplam = 0;
SAYISAL i = 1;

DÖNGÜ
    EĞER i > 10 İSE
        DÖNGÜ_BITIR
    SON

    toplam = toplam + i
    i = i + 1
SON

YAZDIR toplam
```

### Output (Preprocessed English):
```mlp
-- lang: tr-TR

int toplam = 0;
int i = 1;

while
    if i > 10 then
        break
    end

    toplam = toplam + i
    i = i + 1
end

print toplam
```

## File Structure

```
/home/user/MLP/
├── diller.json              # Language definitions
├── dil_cevirici.mlp         # Preprocessor implementation
├── preprocessor/
│   ├── languages/           # Additional language files
│   │   ├── tr-TR.json
│   │   ├── ru-RU.json
│   │   └── custom/
│   └── tests/
│       ├── test_turkish.mlp
│       └── test_russian.mlp
└── docs/
    └── PREPROCESSOR_DESIGN.md
```

## Usage

### Command Line:
```bash
# Auto-detect language from header
./mlp compile program.mlp

# Explicit language specification
./mlp compile program.mlp --lang=tr-TR

# Preprocess only (debug)
./mlp preprocess program.mlp --output=preprocessed.mlp
```

### Programmatic:
```mlp
-- In dil_cevirici.mlp
METIN kaynak = DOSYA_OKU("program.tr.mlp");
METIN dil = detect_language(kaynak);
METIN cevrilmis = translate(kaynak, dil);
DOSYA_YAZ("program.preprocessed.mlp", cevrilmis);
```

## Security Considerations

1. **No Code Injection**: Preprocessor only translates keywords
2. **String Safety**: Literals remain unchanged
3. **Comment Preservation**: Documentation maintained
4. **Identifier Safety**: Variable names not modified

## Future Extensions

1. **Localized Error Messages**
   ```
   Error (tr-TR): "Değişken 'x' tanımlanmamış"
   Error (en-US): "Variable 'x' is not defined"
   ```

2. **Mixed Language Support**
   ```mlp
   -- lang: multi
   -- Turkish + English mixed
   SAYISAL count = 0;
   for i in range(10) {
       YAZDIR i
   }
   ```

3. **Custom Syntax Sugar**
   ```json
   {
     "id": "custom",
     "syntax_sugar": {
       "foreach": "for each",
       "unless": "if not"
     }
   }
   ```

## Testing Strategy

- Unit tests for each language
- String protection tests
- Comment preservation tests
- Identifier safety tests
- Round-trip testing (preprocess → compile → verify)

---

**Status**: Design Phase Complete ✓
**Next**: Implement dil_cevirici.mlp
