# 🎯 MLP Self-Hosting Pipeline

> **Amaç**: MLP compiler'ın kendini compile etmesi (Ring 3 MLP → Ring 0 C → gcc → binary)

**Son Güncelleme**: 2025-11-15
**Mevcut Aşama**: ✅ Stage 0 TAMAMLANDI - Runtime Library'ye geçiş (Stage 1)

---

## 📊 DURUM ÖZETİ

### ✅ TAMAMLANAN ADIMLAR

#### 1. Temel Altyapı (Tamamlandı)
- ✅ Runtime library (runtime.c, runtime.h) - C fonksiyonları
- ✅ Seed compiler (stage0/seed-compiler.py) - Python bootstrap compiler
- ✅ Build system (Makefile)
- ✅ Örnek programlar (test-simple.mlp, calculator.mlp)

#### 2. MLP Kaynak Kodu (Tamamlandı)
- ✅ **6 core module MLP'ye transpile edildi**:
  - src/lexer/tokens.mlp
  - src/lexer/lexer.mlp
  - src/parser/parser.mlp
  - src/ast/ast_nodes.mlp
  - src/codegen/generator.mlp
  - src/compiler/compiler.mlp
- ✅ **English → Turkish syntax dönüşümü** (commit 98d8caf)
  - CLASS → SINIF
  - METHOD → IŞLEÇ
  - IF/ELSE → EĞER/DEĞILSE/İSE
  - RETURN → DÖNÜŞ
  - END → SON
  - vb.

#### 3. Dil Tasarım Değişiklikleri (Tamamlandı)
- ✅ **VAR keyword KALDIRILDI** (commit 8ea3e2b)
- ✅ **Type inference eklendi** - Parser'da `tip_cikart()` fonksiyonu
- ✅ **Symbol table tracking** - `tanimli_degiskenler` dictionary
- ✅ **Scope management** - `scope_push()` / `scope_pop()`
- ✅ **Yeni semicolon kuralları**:
  - İlk tanımlama: `a = 5;` (semicolon ZORUNLU)
  - Atama: `a = 10` (semicolon YASAK)
  - Function call: `YAZDIR(5)` (semicolon OPSİYONEL)

#### 4. Seed Compiler Güncellemeleri (✅ TAMAMLANDI - commit 1fcdb53)
- ✅ Turkish keywords lexer mapping
- ✅ Basic parser (class, method, if/else, while, return)
- ✅ Basic code generator (C struct/function generation)
- ✅ **VAR/DEĞIŞKEN token types KALDIRILDI**
- ✅ **Symbol table tracking eklendi** - `is_variable_defined()`, `define_variable()`
- ✅ **Type inference eklendi** - `infer_type()` metodu
- ✅ **Scope management** - `scope_push()`, `scope_pop()`
- ✅ **Yeni semicolon kuralları**:
  - İlk tanımlama: `a = 5;` ✅ ZORUNLU
  - Reassignment: `a = 10` ✅ YASAK
  - YAZDIR/DÖNÜŞ: ✅ OPSİYONEL
- ✅ **Test edildi ve çalışıyor**:
  - test-simple.mlp → ✅ BAŞARILI
  - examples/04-calculator.mlp → ✅ BAŞARILI

---

## 🔄 YAPILACAK ADIMLAR

### ~~**STAGE 0: Seed Compiler'ı MLP Syntax'ına Uyarla**~~ ✅ TAMAMLANDI (commit 1fcdb53)

**Sonuç**: Seed compiler artık yeni MLP syntax'ını tam destekliyor!
- ✅ VAR keyword yok
- ✅ Type inference çalışıyor
- ✅ Symbol table tracking aktif
- ✅ Semicolon kuralları uygulanıyor
- ✅ Test dosyaları başarıyla compile oluyor

---

### **STAGE 1: Runtime Library'yi Tamamla** 🔴 ŞİMDİ ÖNCELİKLİ

**Amaç**: MLP kaynak kodunun kullandığı tüm built-in fonksiyonlar C'de mevcut olmalı

#### ~~0.1. Seed Compiler'da VAR Keyword'ü Kaldır~~ ✅ TAMAMLANDI
**Dosya**: `stage0/seed-compiler.py`

**Değişiklikler**:
```python
# ✗ KALDIRILIYOR
TokenType.VAR = auto()
TokenType.DEĞIŞKEN = auto()

# Parser'da:
def parse_var_decl(self):  # ✗ SİLİNECEK
    # VAR name = value; parsing'i kaldırılacak
```

**Yeni logic**:
```python
def parse_statement(self):
    # IDENTIFIER ile başlayan ifadeler:
    if token.type == TokenType.IDENTIFIER:
        # Symbol table'a bak
        if is_first_declaration:
            # name = value; → DegiskenTanimlama
            expect_semicolon()
        else:
            # name = value → AtamaKomutu
            forbid_semicolon()
```

**Engeller**:
- Symbol table tracking Python'da implement edilmeli
- Scope management eklenmeli

---

#### 0.2. Seed Compiler'a Type Inference Ekle
**Dosya**: `stage0/seed-compiler.py`

**Eklenecekler**:
```python
class Parser:
    def __init__(self):
        self.symbol_table = {}  # {variable_name: type}
        self.scope_stack = []   # Nested scope tracking

    def infer_type(self, expr_node):
        """İfadeden tip çıkarımı yap"""
        if isinstance(expr_node, IntegerLiteral):
            return "SAYISAL"
        elif isinstance(expr_node, StringLiteral):
            return "METIN"
        # ... vb.
```

**Engeller**:
- MLP AST node'larını Python'da temsil etme

---

#### 0.3. Semicolon Kurallarını Güncelle
**Dosya**: `stage0/seed-compiler.py`

**Değişiklikler**:
```python
# YAZDIR statement
def parse_print(self):
    # ... parse expression
    if current_token == SEMICOLON:
        consume(SEMICOLON)  # Optional!
    return PrintNode(expr)

# DÖNÜŞ statement
def parse_return(self):
    # ... parse expression
    if current_token == SEMICOLON:
        consume(SEMICOLON)  # Optional!
    return ReturnNode(expr)
```

**Engeller**: Yok - basit değişiklik

---

### **STAGE 1: Runtime Library'yi Tamamla** 🔴 KRİTİK

**Amaç**: MLP kaynak kodunun kullandığı tüm built-in fonksiyonlar C'de mevcut olmalı

#### 1.1. Eksik Fonksiyonları Implement Et
**Dosya**: `runtime/runtime.c` ve `runtime/runtime.h`

**Eklenecek fonksiyonlar**:

```c
// Tip işlemleri
char* mlp_type_of(void* obj);           // TYPE_OF()
size_t mlp_length(void* obj);           // UZUNLUK()
char* mlp_to_string(void* obj);         // METIN()

// String işlemleri
char* mlp_substring(const char* str, size_t start, size_t len);  // ALT_METIN()
bool mlp_is_digit(char c);              // IS_DIGIT()
bool mlp_is_alpha(char c);              // IS_ALPHA()
bool mlp_is_whitespace(char c);         // IS_WHITESPACE()

// Parse işlemleri
// ✅ MEVCUT: mlp_parse_int, mlp_parse_float (header'da var)
// ❌ EKSİK: mlp_json_parse implementation

// Dosya işlemleri
// ✅ MEVCUT: mlp_file_read (header'da var)
// ❌ EKSİK: runtime.c'de implementasyon

// Hata işleme
void mlp_throw_error(const char* msg);  // HATA_FIRLAT()
```

**Engeller**:
- JSON parsing karmaşık (basit JSON subset desteklenebilir)
- TYPE_OF için runtime type info gerekli

**Tahmini Süre**: 4-6 saat

---

#### 1.2. Runtime Fonksiyonlarını Test Et
**Dosya**: Yeni test dosyası `tests/runtime_test.c`

**Test edilecekler**:
- String operations (substring, is_digit, vb.)
- Type operations (TYPE_OF, UZUNLUK)
- File operations (DOSYA_OKU)
- JSON parsing (basit durumlar için)

**Engeller**: Yok

**Tahmini Süre**: 1-2 saat

---

### **STAGE 2: İlk Compile Denemesi** 🟡 ORTA ÖNCELİK

**Amaç**: MLP kaynak kodunu seed compiler ile compile etmeyi dene

#### 2.1. Basit Bir MLP Modülü Compile Et
**Hedef**: `src/lexer/tokens.mlp` gibi basit bir dosyayı compile et

**Komut**:
```bash
python3 stage0/seed-compiler.py src/lexer/tokens.mlp -o build/tokens
```

**Beklenen Sonuç**:
- ✅ C kodu üretilmeli (build/tokens.c)
- ✅ gcc ile compile olmalı
- ✅ Binary çalışmalı

**Engeller**:
- ❌ Seed compiler eksik syntax desteklemiyor
- ❌ Runtime fonksiyonları eksik
- ❌ Module system yok (KULLAN statement)

**BAĞIMLILIKLAR**: Stage 0 ve Stage 1 tamamlanmalı

---

#### 2.2. Tüm Compiler Kaynak Kodunu Compile Et
**Hedef**: Full MLP compiler'ı compile et

**Komut**:
```bash
python3 stage0/seed-compiler.py src/compiler/main.mlp -o build/mlp-v1
```

**Beklenen Sonuç**:
- ✅ mlp-v1 binary oluşmalı
- ✅ mlp-v1 çalıştırılabilir olmalı

**Engeller**:
- ❌ Module system (KULLAN) yok
- ❌ Struct literals (Config{...}) yok
- ❌ Generic types (DİZİ<METIN>) yok
- ❌ Array indexing (args[1]) yok
- ❌ For-each loops (HER ... İÇİNDE) yok
- ❌ Pattern matching (EŞLEŞTIR) yok

**BAĞIMLILIKLAR**: Stage 2.1 başarılı olmalı + ekstra özellikler eklenmeli

---

### **STAGE 3: Self-Compile** 🟢 SON HEDEF

**Amaç**: mlp-v1 kendini compile etsin ve ikili eşitliği sağlansın

#### 3.1. İlk Self-Compile
**Komut**:
```bash
# mlp-v1 ile mlp-v2 üret
./build/mlp-v1 src/compiler/main.mlp -o build/mlp-v2
```

**Beklenen Sonuç**:
- ✅ mlp-v2 oluşmalı
- ✅ mlp-v2 çalışmalı

**Engeller**: Stage 2.2'ye bağımlı

---

#### 3.2. Binary Eşitliği Kontrolü
**Komut**:
```bash
# mlp-v2 ile mlp-v3 üret
./build/mlp-v2 src/compiler/main.mlp -o build/mlp-v3

# Binary karşılaştırması
cmp build/mlp-v2 build/mlp-v3
# Çıktı: Dosyalar özdeş olmalı!
```

**Beklenen Sonuç**:
- ✅ mlp-v2 == mlp-v3 (binary eşitlik)
- ✅ **SELF-HOSTING TAMAMLANDI!** 🎉

**Engeller**: Stage 3.1'e bağımlı

---

## 🚧 ENGELLER ve BAĞIMLILIKLAR

### Critical Path (Kritik Yol)

```
Stage 0 (Seed Compiler Syntax Update)
    ↓
Stage 1 (Runtime Library Complete)
    ↓
Stage 2.1 (Simple Module Compile)
    ↓
Stage 2.2 (Full Compiler Compile)
    ↓
Stage 3.1 (Self-Compile)
    ↓
Stage 3.2 (Binary Equality)
    ↓
🎉 SELF-HOSTING COMPLETE
```

### Teknik Engeller

| Özellik | Seed Compiler'da Var mı? | Runtime'da Var mı? | Gerekli mi? |
|---------|-------------------------|-------------------|------------|
| VAR keyword kaldırma | ❌ YOK | N/A | 🔴 KRİTİK |
| Type inference | ❌ YOK | N/A | 🔴 KRİTİK |
| Symbol table | ❌ YOK | N/A | 🔴 KRİTİK |
| Semicolon kuralları | ❌ YOK | N/A | 🔴 KRİTİK |
| TYPE_OF() | N/A | ❌ YOK | 🔴 KRİTİK |
| UZUNLUK() | N/A | ❌ YOK | 🔴 KRİTİK |
| ALT_METIN() | N/A | ❌ YOK | 🔴 KRİTİK |
| IS_DIGIT/ALPHA/... | N/A | ❌ YOK | 🔴 KRİTİK |
| JSON_PARSE() | N/A | ❌ YOK | 🟡 ORTA |
| DOSYA_OKU() impl | N/A | ❌ YOK | 🔴 KRİTİK |
| KULLAN (import) | ❌ YOK | N/A | 🟡 ORTA |
| YAPI (struct) | ❌ YOK | N/A | 🟡 ORTA |
| Generic types | ❌ YOK | N/A | 🟡 ORTA |
| Array indexing | ❌ YOK | N/A | 🟡 ORTA |
| HER (for-each) | ❌ YOK | N/A | 🟡 ORTA |
| EŞLEŞTIR (match) | ❌ YOK | N/A | 🟢 DÜŞÜK |

---

## 📋 ŞU AN YAPILACAK İŞ LİSTESİ

### ⏰ Hemen Yapılacaklar (1-2 gün)

1. **Seed Compiler: VAR Keyword Kaldır**
   - Dosya: `stage0/seed-compiler.py`
   - Token type'dan VAR/DEĞIŞKEN kaldır
   - Parser'da parse_var_decl() sil
   - Symbol table tracking ekle
   - İlk tanımlama vs atama ayrımı yap
   - **Tahmini Süre**: 2-3 saat

2. **Runtime: Kritik Fonksiyonları Ekle**
   - Dosya: `runtime/runtime.c`
   - TYPE_OF(), UZUNLUK(), METIN() ekle
   - ALT_METIN(), IS_DIGIT(), IS_ALPHA(), IS_WHITESPACE() ekle
   - DOSYA_OKU() implement et
   - **Tahmini Süre**: 4-6 saat

3. **İlk Compile Testi**
   - `src/lexer/tokens.mlp` compile et
   - Hataları düzelt
   - **Tahmini Süre**: 2-3 saat

### 📅 Kısa Vadeli (1 hafta)

4. **Seed Compiler: Temel Özellikler**
   - KULLAN (import) ekle
   - YAPI (struct) ekle
   - Array indexing (arr[i]) ekle
   - **Tahmini Süre**: 1-2 gün

5. **Runtime: JSON Parser**
   - Basit JSON subset (string, number, array, object)
   - **Tahmini Süre**: 4-6 saat

6. **Full Compiler Compile**
   - `src/compiler/main.mlp` compile et
   - mlp-v1 binary üret
   - **Tahmini Süre**: 1 gün (debugging dahil)

### 🎯 Orta Vadeli (2 hafta)

7. **Self-Compile Test**
   - mlp-v1 ile mlp-v2 üret
   - Binary karşılaştırması
   - **Tahmini Süre**: 2-3 gün (debugging dahil)

8. **Binary Equality**
   - mlp-v2 == mlp-v3 eşitliği sağla
   - **SELF-HOSTING TAMAMLANDI!**

---

## 🎯 KARAR NOKTASI

**ŞU AN HANGİ ADIMLA BAŞLAMALIYIZ?**

**Seçenek A**: Seed Compiler'ı Güncelle (Stage 0)
- ✅ Direk sonuç alınabilir
- ✅ MLP syntax'ına uyumlu hale gelir
- ⚠️ Python kod yazımı gerekli

**Seçenek B**: Runtime Library'yi Tamamla (Stage 1)
- ✅ C kodu yazımı (daha basit)
- ✅ MLP kaynak kodu için gerekli
- ⚠️ Seed compiler hazır olmadan test edilemez

**Seçenek C**: Paralel İlerleme (Önerilen!)
- ✅ Hem seed compiler hem runtime paralel
- ✅ En hızlı ilerleme
- 1. Runtime'a kritik fonksiyonları ekle (TYPE_OF, UZUNLUK, vb.)
- 2. Seed compiler'da VAR kaldır + type inference ekle
- 3. İlk compile testi yap

---

## ✅ TAMAMLANMA KRİTERLERİ

Self-hosting'in BAŞARILI olduğunu nasıl anlarız?

1. ✅ `mlp-v1` binary üretildi (seed compiler tarafından)
2. ✅ `mlp-v1 src/compiler/main.mlp` → `mlp-v2` üretildi
3. ✅ `mlp-v2 src/compiler/main.mlp` → `mlp-v3` üretildi
4. ✅ `cmp mlp-v2 mlp-v3` → Dosyalar özdeş
5. ✅ Python'a SIFIR bağımlılık (stage0 artık gereksiz)

**O zaman MLP SELF-HOSTING'dir!** 🎉

---

## 📝 NOTLAR

- BOOTSTRAP_STATUS.md bu dosyaya göre güncellenecek (eski bilgiler var)
- Her adım tamamlandığında bu dosya güncellenecek
- Git commit'lerde bu dosyaya referans verilecek
