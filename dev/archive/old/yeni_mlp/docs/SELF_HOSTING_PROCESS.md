# Self-Hosting Sonrası Süreç - Özet

**Tarih:** 25 Kasım 2025

---

## 🎯 **Ana Soru: Self-Hosting Sonrası Sorunları Nerede Çözeriz?**

### **CEVAP: %95 MLP'de (mlpc.mlp), %5 C'de (mlpc.c)**

---

## ✅ **MLP İçinde Çözülebilir (%95)**

### 1. **Parser Bug'ları**
```
Değişiklik: self_host/compiler_core/parser.mlp
Test: Stage0 (C) ile compile → Stage1 test et
```

### 2. **Code Generator Bug'ları**
```
Değişiklik: self_host/compiler_core/codegen*.mlp
Test: Bootstrap → assembly üret → çalıştır
```

### 3. **Yeni Özellikler**
```
Örnek: Ternary operator eklemek
1. lexer.mlp → TOKEN_QUESTION ekle
2. parser.mlp → ternary parsing ekle
3. codegen.mlp → visitor ekle
4. Stage0 ile compile → Stage1 test
```

### 4. **Optimizations**
```
- Register allocation
- Dead code elimination
- Constant folding
Hepsi MLP dosyalarında yapılabilir
```

---

## ❌ **C Compiler'da Değiştirmek ZORUNLU (%5)**

### 1. **Bootstrapping Bug'ları**
```c
// Eğer mlpc.c'de critical bug varsa:
// Context stack completely broken → Stage1 oluşmuyor

Çözüm:
1. mlpc.c'yi fix et (C'de)
2. mlpc.mlp'yi fix et (MLP'de)
3. Yeni mlpc.c ile mlpc.mlp compile et
4. Stage1 working
```

### 2. **Token/AST Structure Breaking Changes**
```c
// TOKEN enum değişirse:
typedef enum {
    TOKEN_NEW_FEATURE,  // ← YENİ
    TOKEN_NUMBER,
    ...
} TokenType;

// mlpc.mlp eski enum kullanıyor
// → Bootstrap fail
// → mlpc.c + mlpc.mlp birlikte güncelle
```

### 3. **Runtime Library API Changes**
```c
// hashmap.c API değişirse:
// Eski: hashmap_put(map, key, value)
// Yeni: hashmap_put(map, key, value, flags)

// mlpc.mlp eski API kullanıyor
// → mlpc.c + mlpc.mlp + hashmap.c sync gerekli
```

---

## 🔄 **Normal Workflow (Self-Hosting Sonrası)**

```
BUG BULUNDU
    ↓
MLP dosyasını düzelt
(parser.mlp, codegen.mlp, etc.)
    ↓
Stage0 (C) ile compile
gcc -o mlpc mlpc.c ... -lm
./mlpc mlpc.mlp mlpc_new.asm
    ↓
Stage1 oluştur
nasm + gcc → mlpc_stage1
    ↓
Stage1 kendini compile edebiliyor mu?
./mlpc_stage1 mlpc.mlp mlpc_s2.asm
diff mlpc_new.asm mlpc_s2.asm
    ↓
✅ BAŞARILI!
mlpc.c artık güncel değil
mlpc.mlp canonical source
```

---

## ⚠️ **Çözülemeyecek/Zor Durumlar**

### 1. **Circular Dependency**
```
Problem: MLP syntax'ını değiştirmek

Senaryo:
- Yeni syntax: "foreach X in Y"
- mlpc.mlp eski syntax ile yazılmış
- mlpc.c de eski syntax parse ediyor
- Yeni syntax'ı parse edecek compiler yok!

Çözüm:
1. mlpc.c'ye yeni syntax ekle
2. Eski mlpc.mlp'yi compile et
3. mlpc.mlp'yi yeni syntax ile yaz
4. Bootstrap
```

### 2. **Performance-Critical Code**
```
Problem: Lexer çok yavaş (MLP interpreted)

Çözüm:
- Optimize et (register allocation)
- Veya kritik kısımları C'ye taşı (hybrid)
- Veya JIT compiler ekle
```

### 3. **Low-Level System Access**
```
Problem: syscall, threading, signal handling

Çözüm:
- FFI (Foreign Function Interface)
- C runtime library çağır
- Veya inline assembly
```

---

## 📊 **Özet Tablo**

| Sorun Tipi | MLP'de? | C'de Gerekli? |
|------------|---------|---------------|
| Parser bug | ✅ %100 | ❌ |
| Codegen bug | ✅ %100 | ❌ |
| Yeni feature | ✅ %95 | ⚠️ Breaking changes |
| Optimization | ✅ %100 | ❌ |
| Syntax change | ✅ %90 | ⚠️ Bootstrap |
| Runtime lib | ⚠️ %50 | ✅ Performance |
| Token enum | ❌ | ✅ Breaking |
| AST struct | ❌ | ✅ Breaking |
| System calls | ❌ | ✅ Low-level |

---

## 🎯 **İdeal Durum (Tam Self-Hosting)**

```
Stage0 (C) - DEPRECATED
    ↓
Stage1 (MLP) - CANONICAL SOURCE
    ↓
Stage2 (Verification)
    ↓
Stage1 == Stage2 ✅

Tüm değişiklikler mlpc.mlp'de
mlpc.c tarihi artifact
```

---

## 📁 **Yeni Proje Yapısı**

```
yeni_mlp/
├── config/
│   ├── diller.json       # TR→EN mappings
│   └── syntax.json       # Base syntax rules
├── src/
│   ├── normalizer/       # Multi-language support
│   ├── lexer/
│   ├── parser/
│   ├── codegen/
│   └── main/
│       ├── mlpc.c        # Bootstrap
│       └── mlpc.mlp      # Self-hosted
├── runtime/              # Reuse from old MLP
├── tools/
│   └── bootstrap.sh      # Automated bootstrap
└── examples/
    ├── hello_world_tr.mlp
    └── hello_world.mlp
```

**Hazır:**
- ✅ diller.json (TR/EN keywords)
- ✅ syntax.json (MLP base rules)
- ✅ mlp_normalize.py (Python normalizer)
- ✅ bootstrap.sh (3-stage automation)
- ✅ Example programs
- ✅ TODO list (13-week plan)

---

## 🚀 **Sonraki Adımlar**

**Mevcut Proje (tyd-lang/MLP):**
1. Fix son codegen bug (1-2 gün)
2. Complete self-hosting
3. Use as Stage0 for new project

**Yeni Proje (yeni_mlp):**
1. Implement normalizer tests (1 hafta)
2. Port lexer.c (1 hafta)
3. Port parser.c (2-3 hafta)
4. Port codegen.c (3-4 hafta)
5. Self-hosting (2-3 hafta)

**Toplam:** 3 ay (13 hafta)

---

**Sonuç:** Self-hosting sonrası %95 değişiklik MLP'de, sadece breaking changes C'de yapılır. Yeni proje temiz mimari ile baştan başlama şansı veriyor.
