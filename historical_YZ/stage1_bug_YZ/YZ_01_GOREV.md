# YZ_01: BUG-001 Fix - Type Inference İmplementasyonu

**Tarih:** 20 Aralık 2025  
**Atanan:** YZ_01 (Görevli)  
**Üst Akıl:** YZ_ÜA_04  
**Öncelik:** 🔴 YÜKSEK  
**Önceki:** YZ_00 (Analiz tamamlandı)

---

## 🎯 Görev

`const x = 88` syntax'ını destekle. Tip bilgisi olmadan, expression'dan tip çıkar.

---

## 📋 YZ_00 Bulguları

### Mevcut Infrastructure (HAZIR ✅):
- AST'de `is_const` flag var
- Symbol table'da `is_const + const_value` alanları var
- Stack optimization çalışıyor (const → stack'te yer yok)
- Immediate load çalışıyor (`movq $88, %r8`)

### Sorun:
**Parser type keyword bekliyor:**
```mlp
const int x = 88      # ✅ Çalışıyor
const x = 88          # ❌ Parse edilemiyor (type yok!)
```

---

## 🔧 Yapılacak Değişiklik

### Dosya: `compiler/stage0/modules/variable/variable_parser.c`

**Satır 24-51 arası** (type parsing bölümü):

**Şu anki kod:**
```c
// Type keyword parse et
if (token yok veya identifier) {
    return NULL;  // ❌ Hata! Body parsing duruyor
}
```

**Yeni kod (pseudo-code):**
```c
// Type keyword var mı kontrol et
if (token keyword (int, string, vb.)) {
    // Mevcut davranış
    type = parse_type_keyword();
} else if (token identifier) {
    // YENİ: Type inference
    // identifier = expression parse et
    // Expression'dan tip çıkar
    
    char* var_name = current_token->value;
    advance(); // identifier'ı tüket
    
    if (current_token != '=') {
        error("Expected '=' after variable name");
    }
    advance(); // '=' tüket
    
    // Expression parse et
    Expression* expr = parse_expression();
    
    // Tip çıkar
    if (expr->type == EXPR_LITERAL_INT) {
        type = "int";  // numeric
    } else if (expr->type == EXPR_LITERAL_STRING) {
        type = "string";
    } else {
        error("Cannot infer type from expression");
    }
    
    // Devam et (mevcut kod)
    ...
}
```

---

## 🧪 Test Planı

### Test 1: Basit Const
```mlp
function main() as int {
    const x = 88
    return(x)
}
```

**Beklenen:**
```bash
./test; echo $?
# 88
```

### Test 2: String Const
```mlp
function main() as int {
    const msg = "hello"
    println(msg)
    return(0)
}
```

### Test 3: Expression Const
```mlp
function main() as int {
    const x = 10 + 20
    return(x)
}
```

**Beklenen:** Exit 30

---

## 📁 Değiştirilecek Dosya

**Tek dosya:**
- `compiler/stage0/modules/variable/variable_parser.c` (satır 24-51)

**Diğer dosyalar değişmeyecek** (infrastructure zaten hazır)

---

## ✅ Tamamlanma Kriterleri

- [ ] Type inference implementasyonu eklendi
- [ ] `const x = 88` syntax'ı parse ediliyor
- [ ] Test 1 geçiyor (Exit 88)
- [ ] Test 2 geçiyor (string const)
- [ ] Test 3 geçiyor (expression const)
- [ ] Compiler yeniden derlendi (`make`)
- [ ] Rapor yazıldı (`stage1_bug_YZ/YZ_01.md`)
- [ ] Git commit yapıldı (`bug-001-type-inference_YZ_01`)

---

## 🔗 Referanslar

- `stage1_bug_YZ/YZ_00.md` - Kök sebep analizi
- `compiler/stage0/modules/variable/variable_parser.c` - Değiştirilecek dosya
- `compiler/stage0/modules/variable/variable.h` - AST struct'ları

---

**Üst Akıl:** YZ_ÜA_04  
**Durum:** ⏳ Atandı
