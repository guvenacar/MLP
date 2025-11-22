# MLP Dil Evrim Rehberi (Language Evolution Guide)

**Tarih:** 21 Kasım 2025  
**Versiyon:** 1.0  
**Durum:** 🎯 Aktif Geliştirme

---

## 📋 İçindekiler

1. [Genel Bakış](#genel-bakış)
2. [Majör Değişiklik Nasıl Yapılır](#majör-değişiklik-nasıl-yapılır)
3. [Örnek: int → decimal Değişikliği](#örnek-int--decimal-değişikliği)
4. [Değişiklik Kontrol Listesi](#değişiklik-kontrol-listesi)
5. [Test Stratejisi](#test-stratejisi)
6. [Geriye Uyumluluk](#geriye-uyumluluk)
7. [Yaygın Senaryolar](#yaygın-senaryolar)

---

## 🎯 Genel Bakış

Self-hosting sayesinde artık MLP derleyicisinin tam kontrolü elimizde. Ancak her değişiklik **4 farklı yerde** güncellenmelidir:

```
┌─────────────────────────────────────────────────────────┐
│  1. SPECS.md         → Dil tanımı (dokümantasyon)       │
│  2. self_host/       → Self-hosting derleyici (MLP)     │
│  3. c_compiler/      → Bootstrap derleyici (C)          │
│  4. diller.json      → Çok dilli destek                 │
└─────────────────────────────────────────────────────────┘
```

### Self-Hosting Avantajları

✅ **Tek bir dilde çalışma:** Artık sadece MLP kodu yazıyoruz  
✅ **Hızlı iterasyon:** Değişiklikleri hemen test edebiliriz  
✅ **Tutarlılık:** Aynı parser kuralları hem derleyicide hem kullanıcı kodunda  

### Self-Hosting Zorlukları

⚠️ **Bootstrap bağımlılığı:** C derleyici hala gerekli (ilk derleme için)  
⚠️ **Dikkatli test:** Derleyici kendini derlerken hata çıkarsa sorun!  
⚠️ **Geriye uyumluluk:** Eski kodlar bozulmamalı  

---

## 🔧 Majör Değişiklik Nasıl Yapılır

### Adım 1: Planlama

**Hangi değişiklikler majör sayılır?**
- ✅ Yeni keyword ekleme/çıkarma (`int` → `decimal`)
- ✅ Syntax değişikliği (`;` kuralı, `end` kullanımı)
- ✅ Yeni data type
- ✅ Operatör önceliği değişikliği
- ✅ Control flow değişikliği

**Hazırlık Soruları:**
1. Bu değişiklik geriye uyumlu mu?
2. Hangi dosyalar etkilenecek?
3. Var olan kodlar bozulur mu?
4. Test senaryoları neler?

### Adım 2: Branch Oluşturma

```bash
# Yedek branch oluştur
git checkout -b feature-decimal-type

# Veya majör versiyon için
git checkout -b v4.0-major-syntax-update
```

### Adım 3: SPECS.md Güncelleme

**Önce dokümantasyonu güncelleyin!** Bu sizin blueprint'iniz.

```markdown
## Data Types

### Primitive Types

| Type | Keyword | Size | Default | Example |
|------|---------|------|---------|---------|
| ~~Integer~~ | ~~`int`~~ | ~~64-bit~~ | ~~0~~ | ~~`int x = 42;`~~ |
| **Decimal** | `decimal` | 64-bit float | 0.0 | `decimal x = 3.14;` |
| String | `string` | Pointer | "" | `string name = "Bob";` |
```

**Versiyonlama ekleyin:**
```markdown
> **⚠️ Breaking Change in v4.0:**  
> `int` keyword deprecated. Use `decimal` instead.  
> Migration: Find/replace `int` → `decimal`
```

### Adım 4: Lexer Güncelleme

**Dosya:** `self_host/lexer.mlp`

```mlp
-- Eski keyword kaldır (veya deprecate et)
-- if keyword_str == "int" then
--     return Token_INT
-- end

-- Yeni keyword ekle
if keyword_str == "decimal" then
    return Token_DECIMAL
end func
```

**Token enum'u güncelle:**
```mlp
-- Token tipleri
Token_INT = 1
Token_DECIMAL = 2  -- YENİ
Token_STRING = 3
-- ...
```

### Adım 5: Parser Güncelleme

**Dosya:** `self_host/parser.mlp`

```mlp
-- Değişken tanımlama parsing
func parse_declaration()
    -- Eski: Token_INT kontrolü
    -- if token.type == Token_INT then
    
    -- Yeni: Token_DECIMAL kontrolü
    if token.type == Token_DECIMAL then
        consume_token()  -- 'decimal' tüket
        
        -- Değişken adı al
        string var_name = consume(Token_IDENTIFIER);
        
        -- '=' bekle
        consume(Token_EQUALS)
        
        -- Expression parse et (3.14 gibi)
        -- ...
    end
end func
```

### Adım 6: Code Generator Güncelleme

**Dosya:** `self_host/generator.mlp`

```mlp
func generate_declaration(node)
    -- Eski: int için stack allocation
    -- if node.type == Node_INT_DECL then
    --     emit("    sub rsp, 8")  -- 64-bit int
    -- end
    
    -- Yeni: decimal için stack allocation
    if node.type == Node_DECIMAL_DECL then
        emit("    sub rsp, 8")  -- 64-bit float
        
        -- Floating point register kullan
        if node.has_initializer then
            string value = node.init_value
            emit("    mov rax, __float64__(" + value + ")")
            emit("    movq xmm0, rax")  -- XMM register
            emit("    movq [rbp-" + node.offset + "], xmm0")
        end
    end
end func
```

### Adım 7: Runtime Güncelleme (gerekirse)

**Dosya:** `runtime/runtime.c`

```c
// Yeni decimal print fonksiyonu
void mlp_print_decimal(double value) {
    printf("%.2f\n", value);
}
```

**Assembly export:**
```nasm
global mlp_print_decimal
```

### Adım 8: C Compiler Güncelleme (Bootstrap için)

**Dosya:** `c_compiler/c_lexer.c`

```c
// Keyword tablosuna ekle
if (strcmp(buffer, "decimal") == 0) {
    return TOKEN_DECIMAL;
}
```

**Dosya:** `c_compiler/c_parser.c`, `c_compiler/c_generator.c`

Self-host ile aynı mantığı C'de uygula.

### Adım 9: Multi-Language Support

**Dosya:** `diller.json`

```json
{
  "languages": [
    {
      "id": "tr-TR",
      "keywords": {
        "decimal": ["ondalik", "ONDALIK", "kesir"]
      }
    },
    {
      "id": "ru-RU",
      "keywords": {
        "decimal": ["десятичное"]
      }
    }
  ]
}
```

### Adım 10: Test Yazma

**Dosya:** `test/test_decimal.mlp`

```mlp
-- lang: en-US
-- Test: Decimal basic usage

decimal x = 3.14;
decimal y = 2.71;

print x
print y

decimal sum = x + y;
print sum  -- Beklenen: 5.85
```

**Dosya:** `test/test_decimal_turkish.mlp`

```mlp
-- lang: tr-TR
-- Test: Ondalık sayı desteği

ondalik x = 3.14;
yazdir x
```

### Adım 11: Derleme ve Test

```bash
# 1. C compiler ile self-host derleyiciyi derle
cd c_compiler
./build.sh

# 2. Self-host derleyiciyi test et
cd ..
./self_host/mlpc test/test_decimal.mlp -o test_decimal
./test_decimal

# 3. Self-host ile self-host'u derle (kendini derleme)
./self_host/mlpc self_host/mlpc.mlp -o mlpc_new
./mlpc_new --version  # Çalışıyor mu?

# 4. Çapraz test: Yeni derleyici eski kodları derleyebiliyor mu?
./mlpc_new examples/01-hello-world.mlp -o hello
./hello
```

### Adım 12: Geriye Uyumluluk (isteğe bağlı)

**Deprecation warning ekle:**

```mlp
-- lexer.mlp içinde
if keyword_str == "int" then
    print_warning("'int' deprecated. Use 'decimal' instead.")
    return Token_DECIMAL  -- Hala çalış ama uyar
end func
```

**Veya migration tool:**

```bash
# Otomatik migration
./tools/migrate_to_v4.sh old_code.mlp
```

### Adım 13: Dokümantasyon

**Dosya:** `CHANGELOG.md`

```markdown
## [4.0.0] - 2025-11-21

### Breaking Changes
- 🔴 `int` keyword removed
- ✅ `decimal` keyword added for floating-point numbers

### Migration Guide
1. Replace all `int` → `decimal` in your code
2. Update numeric literals: `42` → `42.0`
3. Recompile with new compiler

### Examples
**Before (v3.x):**
```mlp
int x = 42;
```

**After (v4.0):**
```mlp
decimal x = 42.0;
```
```

**Dosya:** `docs/MIGRATION_V3_TO_V4.md`

Detaylı migration guide oluştur.

---

## 📝 Örnek: int → decimal Değişikliği

### Tam Uygulama

#### 1. SPECS.md
```markdown
## Data Types (v4.0)

- ~~`int`: Integer (DEPRECATED)~~
- `decimal`: 64-bit floating point number
```

#### 2. self_host/lexer.mlp
```mlp
-- Keyword tanıma
if keyword_str == "decimal" then
    return create_token(Token_DECIMAL, "decimal", line, col)
end func
```

#### 3. self_host/parser.mlp
```mlp
func parse_var_declaration()
    if current_token.type == Token_DECIMAL then
        advance_token()
        
        string name = expect(Token_IDENTIFIER);
        expect(Token_EQUALS)
        
        decimal value = parse_decimal_literal();
        
        return DeclarationNode(name, value)
    end
end func
```

#### 4. self_host/generator.mlp
```mlp
func gen_decimal_decl(node)
    emit("    ; decimal " + node.name)
    emit("    sub rsp, 8")
    
    -- Float literal → XMM register
    emit("    mov rax, " + float_to_hex(node.value))
    emit("    movq xmm0, rax")
    emit("    movq [rbp-" + node.offset + "], xmm0")
end func
```

#### 5. runtime/runtime.c
```c
void mlp_print_decimal(double value) {
    printf("%.6f\n", value);
}

// Export
asm("    .global mlp_print_decimal\n");
```

#### 6. Test
```mlp
-- test/test_decimal.mlp
decimal pi = 3.14159;
decimal e = 2.71828;

print pi  -- 3.141590
print e   -- 2.718280

decimal sum = pi + e;
print sum  -- 5.859870
```

---

## ✅ Değişiklik Kontrol Listesi

Her majör değişiklik için bu listeyi takip edin:

### Planlama
- [ ] Değişiklik belgelendi (SPECS.md)
- [ ] Breaking change mi? (Versiyonlama)
- [ ] Backward compatibility planı var mı?
- [ ] Test senaryoları yazıldı mı?

### Kod Değişiklikleri
- [ ] `self_host/lexer.mlp` güncellendi
- [ ] `self_host/parser.mlp` güncellendi
- [ ] `self_host/generator.mlp` güncellendi
- [ ] `runtime/runtime.c` güncellendi (gerekirse)
- [ ] `c_compiler/*` güncellendi (bootstrap için)

### Multi-Language
- [ ] `diller.json` güncellendi
- [ ] Tüm diller için keyword mapping yapıldı

### Test
- [ ] Unit testler yazıldı
- [ ] Integration testler çalıştırıldı
- [ ] Self-host test (kendini derleme) başarılı
- [ ] Geriye dönük testler geçti

### Dokümantasyon
- [ ] SPECS.md güncellendi
- [ ] CHANGELOG.md entry eklendi
- [ ] Migration guide yazıldı (breaking change ise)
- [ ] Örnekler güncellendi

### Sürüm
- [ ] Branch oluşturuldu
- [ ] Commit message açıklayıcı
- [ ] Tag oluşturuldu (majör versiyon için)
- [ ] Release notes hazırlandı

---

## 🧪 Test Stratejisi

### 1. Unit Tests

Her bileşen için ayrı test:

```mlp
-- test/lexer_test_decimal.mlp
-- Lexer: "decimal" token tanıyabilmeli

-- test/parser_test_decimal.mlp
-- Parser: "decimal x = 3.14;" parse edebilmeli

-- test/codegen_test_decimal.mlp
-- Codegen: Doğru assembly üretmeli
```

### 2. Integration Tests

Uçtan uca test:

```mlp
-- test/integration_decimal.mlp
decimal price = 19.99;
decimal tax = 1.60;
decimal total = price + tax;

print "Total:"
print total  -- Beklenen: 21.59
```

### 3. Self-Host Test

**En kritik test!**

```bash
# Adım 1: C compiler ile self-host'u derle
./c_compiler/mlpc self_host/mlpc.mlp -o mlpc_stage1

# Adım 2: mlpc_stage1 ile self-host'u derle
./mlpc_stage1 self_host/mlpc.mlp -o mlpc_stage2

# Adım 3: Binary karşılaştırma
diff mlpc_stage1 mlpc_stage2
# Eğer aynıysa: Self-hosting başarılı! ✅
```

### 4. Regression Tests

Eski kodlar hala çalışmalı:

```bash
# Tüm örnekleri derle
for file in examples/*.mlp; do
    echo "Testing $file..."
    ./mlpc $file -o test_out
    ./test_out
done
```

### 5. Stress Tests

```mlp
-- test/stress_decimal.mlp
-- 1000 tane decimal değişken tanımla
decimal var0 = 0.0;
decimal var1 = 1.0;
decimal var2 = 2.0;
-- ...
decimal var999 = 999.0;
```

---

## 🔄 Geriye Uyumluluk

### Strateji 1: Deprecation Period

```mlp
-- Version 4.0: int deprecated ama çalışıyor
if keyword_str == "int" then
    emit_warning("'int' is deprecated. Use 'decimal' in v5.0")
    return Token_DECIMAL  -- Hala çalışıyor
end func

-- Version 5.0: int tamamen kaldırılacak
```

### Strateji 2: Alias Support

```mlp
-- Her iki keyword de aynı şeyi yapıyor
if keyword_str == "int" or keyword_str == "decimal" then
    return Token_DECIMAL
end func
```

### Strateji 3: Migration Tool

```python
# tools/migrate_int_to_decimal.py
import re
import sys

def migrate(source_code):
    # 'int' → 'decimal' değiştir (keyword olarak)
    code = re.sub(r'\bint\b', 'decimal', source_code)
    
    # Integer literalleri float'a çevir
    code = re.sub(r'= (\d+);', r'= \1.0;', code)
    
    return code

if __name__ == '__main__':
    with open(sys.argv[1], 'r') as f:
        old_code = f.read()
    
    new_code = migrate(old_code)
    
    with open(sys.argv[1], 'w') as f:
        f.write(new_code)
    
    print(f"✅ Migrated {sys.argv[1]}")
```

**Kullanım:**
```bash
python3 tools/migrate_int_to_decimal.py my_old_program.mlp
```

---

## 🎯 Yaygın Senaryolar

### Senaryo 1: Yeni Keyword Ekleme

**Örnek:** `const` keyword eklemek

**Adımlar:**
1. Token ekle: `Token_CONST`
2. Lexer: `"const"` tanı
3. Parser: `parse_const_declaration()`
4. Codegen: Read-only memory section
5. Test: Const değişken değiştirilmeye çalışıldığında hata

### Senaryo 2: Syntax Değişikliği

**Örnek:** `;` zorunlu hale getirmek

**Adımlar:**
1. Parser: Her statement sonunda `expect(Token_SEMICOLON)`
2. Error messages: "Missing semicolon" hatası
3. Migration tool: Otomatik `;` ekleme
4. SPECS.md: Syntax kurallarını güncelle

### Senaryo 3: Operatör Ekleme

**Örnek:** `**` (power) operatörü

**Adımlar:**
1. Lexer: `**` token'ı tanı
2. Parser: Operator precedence tablosuna ekle
3. Codegen: `pow()` fonksiyonu çağır veya inline assembly
4. Test: `2 ** 3 == 8`

### Senaryo 4: Control Flow Değişikliği

**Örnek:** `for` loop eklemek

**Adımlar:**
1. Keyword: `for`, `in`, `range`
2. Parser: `parse_for_loop()`
3. Codegen: While loop'a desugar et
4. Test: `for i in range(10) then`

### Senaryo 5: Data Type Ekleme

**Örnek:** `bool` type

**Adımlar:**
1. Token: `Token_BOOL`, `Token_TRUE`, `Token_FALSE`
2. Parser: Boolean expressions
3. Codegen: 1 byte (0 = false, 1 = true)
4. Test: `bool flag = true;`

---

## 🚀 Hızlı Başvuru

### Değişiklik Yapılacak Dosyalar

```
Dokümantasyon:
├── SPECS.md              → Dil tanımı
├── CHANGELOG.md          → Versiyon notları
└── docs/MIGRATION_*.md   → Migration rehberleri

Self-Host (MLP):
├── self_host/lexer.mlp    → Token tanıma
├── self_host/parser.mlp   → Syntax parsing
├── self_host/generator.mlp → Assembly üretimi
└── self_host/mlpc.mlp     → Main

Bootstrap (C):
├── c_compiler/c_lexer.c
├── c_compiler/c_parser.c
└── c_compiler/c_generator.c

Runtime:
└── runtime/runtime.c      → Built-in fonksiyonlar

Multi-Language:
└── diller.json            → Keyword mappings

Test:
└── test/*.mlp             → Test dosyaları
```

### Komutlar

```bash
# Self-host derleme
./c_compiler/build.sh
./c_compiler/mlpc self_host/mlpc.mlp -o mlpc

# Test çalıştırma
./mlpc test/test_decimal.mlp -o test && ./test

# Self-host test
./mlpc self_host/mlpc.mlp -o mlpc_new
./mlpc_new --version

# Migration
python3 tools/migrate.py old_code.mlp
```

### Versiyonlama

```
Breaking change:    v3.0 → v4.0 (majör)
New feature:        v3.0 → v3.1 (minör)
Bug fix:            v3.0 → v3.0.1 (patch)
```

---

## 💡 Best Practices

### 1. Dokümantasyon Öncelikli
Kod yazmadan önce SPECS.md'yi güncelleyin. Bu sizin blueprint'iniz.

### 2. Küçük Adımlar
Her değişikliği ayrı commit'lerde yapın:
```
feat(lexer): Add decimal keyword support
feat(parser): Implement decimal parsing
feat(codegen): Generate decimal assembly
test: Add decimal integration tests
```

### 3. Test, Test, Test
- Unit test
- Integration test
- **Self-host test** (en önemlisi!)
- Regression test

### 4. Backward Compatibility
Breaking change yapıyorsanız:
- Migration tool sağlayın
- Deprecation warning ekleyin
- Major version bump yapın

### 5. Peer Review
Majör değişiklikler için:
- Branch oluşturun
- Pull request açın
- Review isteyin
- Tartışın

---

## 🎓 Sonuç

**Self-hosting = Tam Kontrol**

Artık MLP'yi MLP ile geliştiriyoruz. Bu:
- ✅ Tutarlılık sağlıyor
- ✅ Hızlı iterasyon imkanı veriyor
- ✅ Dilin güçünü test ediyor

**Ancak dikkatli olun:**
- ⚠️ Derleyici hata verirse kendini derleyemiyor
- ⚠️ Breaking change'ler planla
- ⚠️ Test etmeyi unutma

**Önemli:** Her majör değişiklik için bu rehberi takip edin. Specs'e uyun. Test edin. Sonra deploy edin.

---

**© 2025 MLP Project**  
**Version:** 1.0  
**License:** MIT
