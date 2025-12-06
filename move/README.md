# MLP - Yeni Proje Yapısı

Bu klasör, MLP derleyicisinin temiz ve organize edilmiş yeni yapısını içerir.

## 📁 Dizin Yapısı

```
move/
├── compiler/           # MLP Derleyici (Stage 0, 1, 2...)
│   ├── stage0/        # Minimal çalışan derleyici (26 modül)
│   ├── stage1/        # Değişkenler ve aritmetik
│   ├── stage2/        # Kontrol akışı ve fonksiyonlar
│   └── docs/          # Derleyici dokümantasyonu
│
├── runtime/           # Runtime kütüphaneleri
│   ├── tto/          # Transparent Type Optimization runtime
│   ├── memory/       # Bellek yönetimi
│   └── stdlib/       # Standart kütüphane
│
├── modules/          # Gelecek modüller (future_modules'den)
│   ├── core/         # Temel modüller
│   ├── advanced/     # İleri özellikler
│   └── experimental/ # Deneysel özellikler
│
├── examples/         # Örnek MLP programları
│   ├── hello/       # Hello World varyasyonları
│   ├── basics/      # Temel örnekler
│   └── advanced/    # İleri düzey örnekler
│
├── tests/           # Test dosyaları
│   ├── unit/        # Birim testler
│   ├── integration/ # Entegrasyon testler
│   └── e2e/         # Uçtan uca testler
│
└── docs/            # Dokümantasyon
    ├── language/    # Dil spesifikasyonu
    ├── api/         # API referansı
    └── tutorials/   # Öğreticiler
```

## 🎯 Hedef

1. **Temiz organizasyon**: Her şey mantıklı konumda
2. **Kolay geliştirme**: Modüler yapı
3. **Bakım kolaylığı**: Anlaşılır hiyerarşi
4. **Ölçeklenebilirlik**: Büyümeye hazır

## 📝 Geçiş Planı

### Aşama 1: Temel Yapı
- [x] move/ klasörü oluşturuldu
- [ ] compiler/stage0/ yapısı kopyalanacak
- [ ] runtime/ yapısı oluşturulacak
- [ ] examples/ temel örnekler eklenecek

### Aşama 2: Modül Taşıma
- [ ] future_modules → modules/ taşınacak
- [ ] Modüller kategorize edilecek (core/advanced/experimental)
- [ ] Her modül README'si güncellenecek

### Aşama 3: Dokümantasyon
- [ ] Tüm dokümantasyon docs/ altına taşınacak
- [ ] API referansı oluşturulacak
- [ ] Tutorial'ler yazılacak

### Aşama 4: Test Yapısı
- [ ] Mevcut testler organize edilecek
- [ ] Test framework kurulacak
- [ ] CI/CD pipeline hazırlanacak

## 🚀 Kullanım

```bash
# Derleyiciyi derle
cd move/compiler/stage0
make

# Hello World çalıştır
./melpc ../../examples/hello/hello_world.mlp output.s
nasm -f elf64 output.s -o output.o
ld output.o -o hello
./hello
```

## 📊 İlerleme

- **Stage 0**: ✅ %100 Tamamlandı (26 modül, çalışan derleyici)
- **Yeni yapı**: 🚧 Hazırlanıyor
- **Modül taşıma**: ⏳ Beklemede
- **Dokümantasyon**: ⏳ Beklemede

---

*Son güncelleme: 6 Aralık 2025*
*Durum: Stage 0 Complete - Yeni yapı hazırlanıyor*
