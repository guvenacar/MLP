# MLP Proje Yapısı - Detaylı Rehber

## 📋 Genel Bakış

```
move/
├── compiler/          # MLP Derleyici Pipeline
│   ├── stage0/       # ✅ Minimal (yazdir() only) - TAMAMLANDI
│   ├── stage1/       # 🚧 Değişkenler + Aritmetik
│   ├── stage2/       # ⏳ Kontrol Akışı + Fonksiyonlar
│   └── docs/         # Derleyici dokümantasyonu
│
├── runtime/          # Runtime Kütüphaneleri
│   ├── tto/         # ✅ TTO Runtime (overflow, BigDecimal, SSO)
│   ├── memory/      # 🚧 Bellek yönetimi
│   └── stdlib/      # ⏳ Standart kütüphane
│
├── modules/         # Modül Sistemi (47 modül)
│   ├── core/       # ✅ 8/8 Temel modüller
│   ├── advanced/   # 🚧 14/22 İleri özellikler
│   └── experimental/ # ⏳ 0/17 Deneysel
│
├── examples/        # Örnek Programlar
│   ├── hello/      # ✅ Hello World
│   ├── basics/     # 🚧 Temel örnekler
│   └── advanced/   # ⏳ İleri örnekler
│
├── tests/          # Test Suite
│   ├── unit/       # Birim testler
│   ├── integration/ # Entegrasyon testleri
│   └── e2e/        # Uçtan uca testler
│
└── docs/           # Dokümantasyon
    ├── language/   # Dil spesifikasyonu
    ├── api/        # API referansı
    └── tutorials/  # Öğreticiler
```

## 🎯 Stage Planı

### Stage 0 - TAMAMLANDI ✅
**Tarih**: 6 Aralık 2025  
**Durum**: %100 Çalışıyor

**Özellikler**:
- Lexer (UTF-8, Türkçe)
- Parser (yazdir() only)
- Code Generator (x86-64 NASM)
- 26 modül entegrasyonu
- 384KB binary

**Test Sonuçları**:
- ✅ hello_world.mlp (3 yazdir)
- ✅ test_print.mlp (12 yazdir, UTF-8)
- ✅ 15/15 yazdir() başarılı

**Dosya Konumları**:
```
Kaynak:    /melp/C/stage0/
Taşınacak: /move/compiler/stage0/
```

### Stage 1 - SONRAKİ ADIM 🚧
**Hedef**: Değişkenler ve Aritmetik

**Eklenecekler**:
```mlp
-- Değişken tanımlama
sayi x = 42
sayi y = 58

-- Aritmetik işlemler
sayi toplam = x + y
sayi fark = x - y
sayi carpim = x * y
sayi bolum = x / y

-- Sayıları yazdırma
yazdir(toplam)  -- "100"
```

**Gerekli Değişiklikler**:
1. Parser: `sayi identifier = expression` parse et
2. Symbol table: Değişken kaydet
3. Expression evaluator: Aritmetik ifadeler
4. Codegen: Stack frame, değişken adresleme
5. Print: Sayıyı string'e çevir

**Tahmin**: ~1-2 hafta

### Stage 2 - GELECEK ⏳
**Hedef**: Kontrol Akışı ve Fonksiyonlar

**Eklenecekler**:
```mlp
-- If/else
eger x > 0 ise
    yazdir("Pozitif")
değilse
    yazdir("Negatif")
son

-- While döngüsü
sayi i = 0
iken i < 10
    yazdir(i)
    i = i + 1
son

-- Fonksiyonlar
fonksiyon topla(a, b)
    dön a + b
son

sayi sonuc = topla(5, 3)
```

**Tahmin**: ~2-3 hafta

## 📦 Modül Sistemi

### Kategori 1: Core (8 modül)
**Durum**: ✅ 100% Tamamlandı

| Modül | Açıklama | Dosyalar |
|-------|----------|----------|
| lexer | Tokenization | .c, .h |
| parser | Syntax parsing | .c, .h |
| ast | AST oluşturma | .c, .h |
| semantic | Semantic analiz | .c, .h |
| codegen_context | Codegen context | .c, .h |
| error_handling | Hata yönetimi | .c, .h |
| type_system | Tip sistemi | .c, .h, _parser, _codegen |
| print | Print fonksiyonu | .c, .h, _parser, _codegen |

### Kategori 2: Advanced (22 modül)
**Durum**: 🚧 14/22 Hazır, 8 Kısmi

**Tam Çalışan (14)**:
- async, concurrency, memory, null_safety
- generic, trait, enum, struct
- macro, decorator, ffi, regex
- pattern_matching, debug

**Kısmi Çalışan (8)**:
- array_operations, collections
- generator, iterator_system
- module_system, operator_overloading
- result_option, smart_pointers

### Kategori 3: Experimental (17 modül)
**Durum**: ⏳ 0/17 Henüz başlanmadı

Planlananlar:
- dependent_types, linear_types
- type_classes, const_generics
- reflection, unsafe_blocks
- ownership_system, linking
- ... (11 more)

## 🔧 Runtime Bileşenleri

### TTO Runtime ✅
**Dosya**: `tto_runtime.c` (260 satır)

**Özellikler**:
1. **Overflow Detection**
   - `tto_would_overflow_add(a, b)`
   - Compile-time checking
   
2. **BigDecimal Library**
   - Arbitrary precision
   - Heap-allocated
   - Reference counted

3. **SSO (Small String Optimization)**
   - ≤23 byte → stack
   - >23 byte → heap
   - Automatic promotion

**Test**: `test_tto_runtime.c` - 6 test senaryosu

### Memory Management 🚧
**Planlanan**:
- Smart pointers (Box, Rc, Arc)
- Ownership tracking
- Borrow checker integration
- Memory pools

### Standard Library ⏳
**Planlanan**:
- String utilities
- Collections (Vector, HashMap)
- File I/O
- Network I/O

## 📊 İstatistikler

### Kod Satırları
```
Stage 0 Compiler:
  main.c:              140 satır
  tto_runtime.c:       260 satır
  Makefile:            50 satır
  
Modüller (26 x avg 200):  ~5,200 satır
Test programları:         ~50 satır
Dokümantasyon:            ~2,000 satır
─────────────────────────────────
TOPLAM:                   ~7,700 satır
```

### Binary Boyutları
```
melpc_26:           384 KB (derleyici)
hello_world:        8 KB (örnek program)
test_print:         9 KB (test programı)
```

### Test Kapsamı
```
Unit Tests:         0/100 (Yazılacak)
Integration Tests:  2/10  (hello, test_print)
E2E Tests:          0/20  (Yazılacak)
─────────────────────────────────
TOPLAM:             2/130 (%1.5)
```

## 🚀 Geliştirme İş Akışı

### 1. Yeni Özellik Ekleme
```bash
# 1. Branch oluştur
git checkout -b feature/yeni-ozellik

# 2. Kodu yaz
vim move/compiler/stage1/...

# 3. Test yaz
vim move/tests/unit/test_yeni_ozellik.c

# 4. Derle ve test et
make test

# 5. Commit et
git commit -m "feat: Yeni özellik eklendi"

# 6. Push et
git push origin feature/yeni-ozellik
```

### 2. Bug Düzeltme
```bash
# 1. Issue oluştur (GitHub)
# 2. Bug'ı reproduce et
# 3. Fix yaz
# 4. Test ekle
# 5. PR aç
```

### 3. Dokümantasyon
```bash
# Her değişiklikle birlikte:
- README.md güncelle
- API docs güncelle
- Changelog güncelle
```

## 📝 Öncelikli Görevler

### Kısa Vadeli (1-2 hafta)
1. [ ] Stage 0'ı move/compiler/stage0/ altına taşı
2. [ ] Runtime'ı move/runtime/ altına taşı
3. [ ] Temel örnekleri kopyala
4. [ ] Build sistem düzenle (Makefile)

### Orta Vadeli (1 ay)
5. [ ] Stage 1 implementasyonu (değişkenler)
6. [ ] Test framework kur
7. [ ] CI/CD pipeline
8. [ ] Modül dokümantasyonu

### Uzun Vadeli (3 ay)
9. [ ] Stage 2 (kontrol akışı)
10. [ ] Tüm 22 modülü aktif et
11. [ ] Standard library
12. [ ] Package manager

## 🎓 Öğrenme Kaynakları

### Dokümantasyon Konumları
```
Dil Spec:       /move/docs/language/
API Referans:   /move/docs/api/
Tutorial'ler:   /move/docs/tutorials/
Örnekler:       /move/examples/
```

### Harici Kaynaklar
- [Compiler Design Patterns]
- [LLVM Backend Tutorial]
- [Type System Theory]
- [Runtime Optimization]

## ⚡ Hızlı Komutlar

```bash
# Derleyiciyi derle
cd move/compiler/stage0 && make

# Test çalıştır
cd move && make test

# Örnek derle
./move/compiler/stage0/melpc_26 \
  move/examples/hello/hello_world.mlp \
  output.s

# Assembly → Executable
nasm -f elf64 output.s -o output.o
ld output.o -o hello
./hello

# Tüm projeyi temizle
make clean
```

## 🐛 Bilinen Sorunlar

1. **Stage 0 Limitations**:
   - Sadece yazdir() destekleniyor
   - Değişken yok
   - İfade değerlendirme yok

2. **Runtime Issues**:
   - BigDecimal implementation basitleştirilmiş
   - Memory pooling yok
   - GC yok (manual memory management)

3. **Test Coverage**:
   - Unit test framework kurulmadı
   - Coverage %1.5 (çok düşük)

## 📞 İletişim

**Proje Sahibi**: [Your Name]  
**Repo**: https://github.com/guvenacar/MLP  
**Branch**: mlp-true-syntax  
**Son Güncelleme**: 6 Aralık 2025

---

## 🎉 Milestone: Stage 0 Complete!

**Tarih**: 6 Aralık 2025  
**Commit**: `6a612d1`  
**Durum**: ✅ Tam Çalışır

**Başarılar**:
- 26 modül derlenmiş
- İlk MLP programı çalıştı
- UTF-8 Türkçe desteği
- Temiz assembly üretimi
- Tam pipeline (mlp→s→o→exe)

**Sıradaki**: Stage 1 - Variables & Arithmetic

---

*Bu dosya sürekli güncellenir. Son versiyon için git history'ye bakın.*
