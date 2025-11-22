# MLP Temizlik ve Yeniden Yapılanma
**Tarih:** 22 Kasım 2025

## 🎯 Hedef
MLP'yi **tamamen MLP ile** yeniden inşa etmek. Python ve C bağımlılıklarından kurtulmak.

## 📦 Arşivlenen Dosyalar

### Python Preprocessor (stage0/)
- ✅ `mlp_preprocessor.py` → `tmp/old_version/stage0_python/`
- ✅ `syntax_preprocessor.py` → `tmp/old_version/stage0_python/`
- **Sebep:** Preprocessor'ı MLP ile yeniden yazacağız

### C Compiler Backup (c_compiler/)
- ⏳ `c_lexer.c/h`, `c_parser.c/h`, `c_generator.c/h` → Yedeklenecek
- ✅ `c_backend.c/h`, `backend_common.h` → Tutulacak (C backend için)
- **Sebep:** Bootstrap için gerekli, ama aktif kullanım yok

## 🚀 Yeni Mimari

```
┌─────────────────────────────────────────────────┐
│  USER SOURCE CODE (.mlp)                        │
│  - Turkish, English, Russian, etc.              │
│  - Any syntax style (C-like, Python-like, etc.) │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  PREPROCESSOR.MLP (MLP ile yazılmış!)           │
│  - diller_comprehensive.json okur               │
│  - syntax_comprehensive.json okur               │
│  - Multi-language → Base English IR çevirir    │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  BASE ENGLISH IR (.ir.mlp)                      │
│  - Normalized keywords (int, if, then, etc.)    │
│  - No semicolons (Python style)                 │
│  - Self-documenting ends (end if, end while)    │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  LEXER.MLP → PARSER.MLP → GENERATOR.MLP         │
│  (Self-host compiler - MLP ile yazılmış!)       │
└─────────────────────────────────────────────────┘
                    ↓
┌─────────────────────────────────────────────────┐
│  OUTPUT (ASM / C)                               │
│  - ASM Backend: NASM → Binary                   │
│  - C Backend: GCC → Binary                      │
└─────────────────────────────────────────────────┘
```

## 📋 Yapılacaklar (Öncelik Sırasına Göre)

### 1️⃣ PREPROCESSOR.MLP Yaz (İlk Öncelik)
- [ ] JSON parser (mlp_json/ kullan veya yeniden yaz)
- [ ] `diller_comprehensive.json` okuma
- [ ] `syntax_comprehensive.json` okuma
- [ ] Keyword mapping engine
- [ ] Syntax transformation
- [ ] Test suite

### 2️⃣ LEXER.MLP Düzelt (Mevcut hardcoded olanı yeniden yaz)
- [ ] Base English keywords only
- [ ] Dynamic token creation
- [ ] UTF-8 desteği
- [ ] String/number parsing
- [ ] Comment handling

### 3️⃣ PARSER.MLP Düzelt
- [ ] Recursive descent parsing
- [ ] AST construction
- [ ] Error handling
- [ ] Expression parsing

### 4️⃣ GENERATOR.MLP Düzelt
- [ ] ASM generation (mevcut logic'i koru)
- [ ] C generation (hybrid backend)
- [ ] Scope-based memory management

### 5️⃣ Bootstrap Süreci
- [ ] C compiler ile preprocessor.mlp'yi derle
- [ ] C compiler ile lexer.mlp, parser.mlp, generator.mlp'yi derle
- [ ] Self-host compiler ile kendini derle (Double compilation test)
- [ ] C compiler'ı arşivle

## ⚠️ Geçici Olarak Tutulacaklar

### C Compiler (Bootstrap için)
```
c_compiler/
├── main.c              ← Bootstrap için gerekli
├── c_lexer.c/h         ← Geçici (preprocessor.mlp hazır olana kadar)
├── c_parser.c/h        ← Geçici (lexer.mlp hazır olana kadar)
├── c_generator.c/h     ← Geçici (generator.mlp hazır olana kadar)
├── c_backend.c/h       ← Kalacak (C backend option)
└── backend_common.h    ← Kalacak (backend interface)
```

**Silme koşulu:** 
- ✅ preprocessor.mlp çalışıyor
- ✅ lexer.mlp + parser.mlp + generator.mlp çalışıyor
- ✅ Self-host başarılı
- ✅ Tüm testler geçiyor

## 🎯 Başarı Kriterleri

1. **%100 MLP ile yazılmış compiler**
2. **Python bağımlılığı yok**
3. **C kodu sadece runtime.c** (sistem çağrıları için)
4. **Çoklu dil/syntax desteği** (JSON tabanlı)
5. **Self-hosting kanıtlandı** (kendini derleyebiliyor)

## 📅 Tahmini Süre
- Preprocessor.mlp: 3-4 gün
- Lexer.mlp düzeltme: 2-3 gün
- Parser.mlp düzeltme: 2-3 gün
- Generator.mlp düzeltme: 2-3 gün
- Bootstrap & Test: 1-2 gün
- **TOPLAM: ~2 hafta**

---

**Motto:** *"No more Python! No more C! Only MLP!"* 🚀
