# 📋 TODO: STAGE1-MLP PMLP MIGRATION

**Numara:** 2  
**Tarih:** 26 Aralık 2025  
**Öncelik:** 🔴 KRİTİK  
**Tahmini Süre:** 2-3 saat  
**Bağımlılık:** ✅ TODO_STAGE0_PMLP_SYNTAX (tamamlandı)  
**Durum:** ⏳ Bekliyor

---

## 🎯 AMAÇ

Stage1-MLP compiler modüllerini (`compiler/stage1-mlp/compiler_core/*.mlp`) yeni PMLP syntax'a migrate etmek.

**SORUN:** Stage0-C compiler artık PMLP syntax kullanıyor (semicolon separator), ama Stage1-MLP modülleri hala eski syntax (virgül separator) kullanıyor. Bu durum **self-hosting**'i engelliyor.

```bash
# MEVCUT HATA:
./build/bin/mlpc compiler/stage1-mlp/compiler_core/codegen.mlp /tmp/test.asm
# HATA: Line 47 - "," beklenmeyen token!
```

---

## 📊 MEVCUT DURUM ANALİZİ

### Etkilenen Dosyalar

```
compiler/stage1-mlp/compiler_core/
├── codegen.mlp               (809 satır)
├── codegen_functions.mlp     
├── codegen_control.mlp       
├── codegen_structures.mlp    
├── codegen_async.mlp         
├── codegen_lambda.mlp        
├── lexer.mlp                 
├── parser.mlp                
├── parser_advanced.mlp       
├── parser_functions.mlp      
├── parser_structures.mlp     
├── scope_manager.mlp         
├── function_registry.mlp     
├── gc_integration.mlp        
├── import_handler.mlp        
├── token_types.mlp           
├── mlp_compiler_main.mlp     
└── ast/
    └── ast_nodes.mlp
```

**Toplam:** ~17 dosya, ~5000+ satır

### Değiştirilecek Pattern'ler

| Eski Syntax | Yeni PMLP Syntax | Örnek |
|-------------|------------------|-------|
| `func(a, b)` | `func(a; b)` | Fonksiyon çağrısı |
| `function f(a, b)` | `function f(a; b)` | Fonksiyon tanımı |
| `[1, 2, 3]` | `[1; 2; 3;]` | Liste/array literal |

**⚠️ DİKKAT:** Virgül artık SADECE ondalık ayırıcı:
- `3,14` = 3.14 (Türk formatı) - **DEĞİŞMEMELİ**

---

## 📋 PHASE'LER

### Phase 1: Analiz ve Planlama (30 dk) ✅ TAMAMLANDI

**Task 1.1:** Tüm .mlp dosyalarını tara
- [x] `grep -rn "function.*(.*, " compiler/stage1-mlp/` ile fonksiyon parametrelerini bul → 137 adet
- [x] `grep -rn "\[.*,.*\]" compiler/stage1-mlp/` ile array literal'ları bul → 3 adet (yorumlarda)
- [x] Ondalık sayı kullanımlarını tespit et (bunlar korunmalı) → 0 adet ✅

**Task 1.2:** Migration script hazırla (opsiyonel)
- [x] Regex pattern'leri belirle
- [x] Test için küçük script yaz → `tools/migrate_stage1_to_pmlp.py`
- [x] False positive'leri kontrol et → Dry-run test başarılı

**📊 Phase 1 Test Sonuçları:**
```bash
# Script test (token_types.mlp)
python3 tools/migrate_stage1_to_pmlp.py compiler/stage1-mlp/compiler_core/token_types.mlp --dry-run
Sonuç: ✅ BAŞARILI - 6 satır değişiklik tespit edildi
```

**📝 STAGE1_YZ_01 Raporu:** `TODO_STAGE1_MIGRATION/STAGE1_YZ/completed/STAGE1_YZ_01_TAMAMLANDI.md`

### Phase 2: Dosya Migration (1.5-2 saat)

**Task 2.1:** Core modüller
- [ ] `token_types.mlp` - en basit, önce bu
- [ ] `scope_manager.mlp`
- [ ] `function_registry.mlp`

**Task 2.2:** Lexer/Parser
- [ ] `lexer.mlp`
- [ ] `parser.mlp`
- [ ] `parser_advanced.mlp`
- [ ] `parser_functions.mlp`
- [ ] `parser_structures.mlp`

**Task 2.3:** Codegen
- [ ] `codegen.mlp` (en büyük, 809 satır)
- [ ] `codegen_functions.mlp`
- [ ] `codegen_control.mlp`
- [ ] `codegen_structures.mlp`
- [ ] `codegen_async.mlp`
- [ ] `codegen_lambda.mlp`

**Task 2.4:** Diğerleri
- [ ] `gc_integration.mlp`
- [ ] `import_handler.mlp`
- [ ] `mlp_compiler_main.mlp`
- [ ] `ast/ast_nodes.mlp`

### Phase 3: Doğrulama (30 dk)

**Task 3.1:** Syntax doğrulama
- [ ] Her dosyayı `./build/bin/mlpc` ile parse et
- [ ] Hata olmadığını doğrula

**Task 3.2:** Test
- [ ] Mevcut PMLP test suite'i çalıştır (13 test)
- [ ] Yeni migration testleri ekle

---

## ✅ BAŞARI KRİTERLERİ

1. **Tüm Stage1-MLP dosyaları PMLP syntax'a uyumlu**
   ```bash
   ./build/bin/mlpc compiler/stage1-mlp/compiler_core/codegen.mlp /tmp/test.asm
   # HATA YOK
   ```

2. **Mevcut testler geçiyor**
   ```bash
   cd tests/pmlp && ./run_all.sh
   # 13/13 PASSED
   ```

3. **Ondalık sayılar korunmuş**
   - `3,14` formatı bozulmamış

---

## 🔧 MIGRATION STRATEJİSİ

### Önerilen Yaklaşım: Manuel + Script Hibrit

**Adım 1:** Basit sed script ile toplu değişiklik
```bash
# DİKKAT: Bu sadece örnek, dikkatli kullanılmalı!
# Fonksiyon tanımlarında virgül → semicolon
sed -i 's/function \([^(]*\)(\([^)]*\), /function \1(\2; /g' *.mlp
```

**Adım 2:** Manuel kontrol
- Her dosyayı aç
- String içindeki virgüllerin bozulmadığını kontrol et
- Ondalık sayıların korunduğunu kontrol et

**Adım 3:** Compiler ile doğrula
```bash
for f in *.mlp; do
    ./build/bin/mlpc "$f" /tmp/test.asm 2>&1 | head -5
done
```

---

## ⚠️ RİSKLER

| Risk | Etki | Önlem |
|------|------|-------|
| String içi virgüller değişir | Yüksek | Regex'te string literal hariç tut |
| Ondalık sayılar bozulur | Orta | `\d,\d` pattern'i koru |
| Nested fonksiyon çağrıları | Orta | Manuel kontrol |

---

## 📚 REFERANSLAR

- [1-TODO_STAGE0_PMLP_SYNTAX.md](1-TODO_STAGE0_PMLP_SYNTAX.md) - PMLP syntax detayları
- [docs/specs/pmlp_kesin_sozdizimi.md](docs/specs/pmlp_kesin_sozdizimi.md) - PMLP referans
- [tests/pmlp/](tests/pmlp/) - PMLP test suite

---

## 🔗 BAĞIMLILIKLAR

**Önkoşul:**
- ✅ TODO_STAGE0_PMLP_SYNTAX tamamlanmış olmalı

**Sonraki:**
- → 3-TODO_SELFHOST_VERIFY (self-hosting doğrulama)

---

**Mastermind Onayı:** ⏳ Bekliyor
