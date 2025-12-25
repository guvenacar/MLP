# Syntax Karşılaştırma: mlp-original vs pmlp_kesin_sozdizimi.md

**Tarih:** 24 Aralık 2025  
**Hedef:** pmlp_kesin_sozdizimi.md'yi mlp-original'e entegre etmek

---

## 📊 MEVCUT DURUM ANALİZİ

### ✅ UYUMLU OLANLAR

#### 1. Blok Sonlandırıcılar
**pmlp_kesin_sozdizimi.md:**
```mlp
if condition then
    code
end_if
```

**mlp-original:**
```mlp
if condition then
    code
end_if
```
✅ **TAM UYUMLU** - Alt çizgili syntax her ikisinde de var

#### 2. Function Syntax
**pmlp_kesin_sozdizimi.md:**
```mlp
function func_name(param1; param2) -> return_type
    code
end_function
```

**mlp-original:**
```mlp
function func_name(param1, param2)
    code
end_function
```
⚠️ **FARKLILIK:**
- PMPL: Parametre ayırıcı `;` (noktalı virgül)
- mlp-original: Virgül `,` kullanılmış (ama belki lexer'da parse ediliyor?)

---

## 🔍 KRİTİK FARKLAR

### 1. Parametre Ayırıcı

**pmlp_kesin_sozdizimi.md:** `;` (Her yerde zorunlu)
```mlp
func(345,95; true; "OK")  -- 345,95 = ondalık sayı, parametre ayırıcı ;
list.add(10; 20; 30)
map.set("key"; "value")
```

**mlp-original Kaynak Kodu:**
```mlp
asm_emit_text(state.code, "    ; Comment")  -- Virgül kullanılmış
numeric_to_metin(param_count)
scope_allocate_variable(state, "__promise", "SAYISAL")
```

**Analiz:**
- mlp-original kaynak kodunda `,` kullanılmış
- Ama bu C compiler tarafından PMPL'ye normalize edilmiş olabilir
- Kontrol: Lexer/Parser'da `,` → `;` dönüşümü var mı?

### 2. Token İsimleri

**pmlp_kesin_sozdizimi.md:**
- `TOKEN_EXIT` (break karşılığı)
- `TOKEN_CONTINUE` (continue karşılığı)
- `TOKEN_END_IF`, `TOKEN_END_WHILE`, vb. (alt çizgili)

**mlp-original token_types.mlp:**
```mlp
TOKEN_WHILE_BITIR  -- "break" karşılığı (Türkçe)
```

⚠️ **FARKLILIK:** Token isimleri farklı

---

## 🎯 ENTEGRASYON ZORLUĞUseverity: LOW-MEDIUM

### Kolay Olanlar (1-2 saat)

1. **Blok Sonlandırıcılar:** ✅ Zaten uyumlu
2. **Struct/Enum Syntax:** ✅ Muhtemelen uyumlu (kontrol lazım)
3. **Control Flow (if/while/for):** ✅ Zaten uyumlu

### Orta Zorlukta Olanlar (3-5 saat)

1. **Parametre Ayırıcı `;` Enforcement**
   - Lexer'da virgül `,` parsing kontrolü
   - Parser'da parametre ayırıcı validation
   - Error mesajları: "Use `;` not `,` for parameters"
   
2. **Token İsim Harmonizasyonu**
   - `TOKEN_WHILE_BITIR` → `TOKEN_EXIT` alias ekle
   - `TOKEN_DEVAM` → `TOKEN_CONTINUE` alias ekle
   - Backward compatibility için her ikisini destekle

### Zor Olanlar (5-10 saat)

1. **Ondalık Sayı vs Parametre Ayırıcı**
   - Lexer'da `345,95` → `TOKEN_NUMBER(345.95)` parse etme
   - `func(10,5; 20,3)` → İki parametre: `10.5` ve `20.3`
   - Mevcut kod tabanı virgüllü sayıları destekliyor mu?

2. **Normalizer Layer**
   - pmlp_kesin_sozdizimi.md diyor ki:
     ```
     [Kullanıcı Kodu] → [Normalizer] → [PMPL] → [Lexer]
     ```
   - mlp-original'de normalizer yok!
   - Multi-language syntax support için normalizer layer eklemek gerekir

---

## 💡 ÖNERİLEN YAKLAŞIM

### Aşama 1: Minimal Changes (2-3 saat)
1. Token aliases ekle:
   ```c
   #define TOKEN_EXIT TOKEN_WHILE_BITIR
   #define TOKEN_CONTINUE TOKEN_DEVAM
   ```
2. Dokümantasyonu güncelle
3. Test: Mevcut .mlp dosyaları compile edebiliyor mu?

### Aşama 2: Parametre Syntax (3-5 saat)
1. Lexer'da `;` parametre ayırıcı desteği ekle
2. Parser'da hem `,` hem `;` destekle (backward compat)
3. Warning: "Deprecated: Use `;` instead of `,` for params"

### Aşama 3: Full PMPL Compliance (10-15 saat)
1. Normalizer layer ekle
2. Ondalık sayı parsing (Türk formatı: `345,95`)
3. Multi-language syntax → PMPL mapping
4. Full regression test suite

---

## 📈 ZORLseverity: LOW ⭐⭐☆☆☆

**Neden?**
1. ✅ mlp-original zaten alt çizgili syntax kullanıyor (`end_if`, `end_while`)
2. ✅ Token yapısı benzer
3. ⚠️ Parametre ayırıcı farklı ama backward compatible yapılabilir
4. ⚠️ Normalizer layer yok ama self-hosting için şart değil

**Self-hosting için kritik mi?**
- ❌ **Hayır** - mlp-original kendi syntax'ını compile edebiliyor
- ✅ **Ama:** Multi-language support için PMPL standardizasyonu gerekli

---

## 🎯 ÖNERİ: AŞAMALI ENTEGRASYON

```
Priority 1 (ŞİMDİ): Self-hosting'i bitir
├─ TOKEN_BREAK sorunu çöz
├─ Gen1 assembly üret
└─ Binary convergence test et

Priority 2 (SONRA): Token harmonizasyonu
├─ TOKEN_EXIT/CONTINUE aliases
├─ Dokümantasyon güncellemesi
└─ Backward compatibility test

Priority 3 (GELECEKgelecek): Full PMPL compliance
├─ Normalizer layer
├─ Ondalık sayı parsing
└─ Multi-language syntax support
```

---

**SONUÇ:**  
Entegrasyon **kolay değil** (10-15 saat) ama **şu an gerekli değil**.  
Self-hosting önce, PMPL standardizasyonu sonra! 🚀
