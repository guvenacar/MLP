# MLP GUI Editors

Bu klasör, MLP dili için geliştirilmiş tüm GUI editörlerini içerir.

## 📁 Yapı

```
editors/
├── shared/              # Tüm editörler için ortak kaynaklar
│   ├── styles/          # Ortak CSS dosyaları
│   ├── themes/          # Tema tanımları (dark/light)
│   └── assets/          # Ortak görseller, ikonlar
│
├── mlp_gui_designer/    # 🎯 ANA EDITÖR - Electron tabanlı (%95 hazır)
│   ├── Özellikler: Sürükle-bırak, 9 widget, kod üretimi
│   ├── Status: Production-ready, IntelliSense entegrasyonu devam ediyor
│   └── Teknoloji: Electron 28, Vanilla JS, GPU-accelerated
│
├── mlp_flow_editor/     # React Flow tabanlı modern editör (WIP)
│   ├── Özellikler: Node-based editor, React Flow
│   ├── Status: Development, %30 tamamlandı
│   └── Teknoloji: React 18, React Flow 11, Vite 5
│
└── gui_editor_legacy/   # İlk prototip (HTML+JS)
    ├── Özellikler: Basit sürükle-bırak
    ├── Status: Archived, referans için saklandı
    └── Teknoloji: Pure HTML/CSS/JS
```

## 🎨 Shared Resources

### Styles (`shared/styles/`)
- **main.css** - Ana stil dosyası (VS Code dark theme)
- **themes.css** - Tema değişkenleri (dark/light)
- **widgets.css** - Widget stilleri

### Themes (`shared/themes/`)
Tüm editörler aynı tema sistemini kullanır:
- Dark theme (default)
- Light theme
- CSS custom properties ile kolay geçiş

## 🚀 Öncelikli Editör: mlp_gui_designer

### Neden bu editör?
- %95 tamamlanmış, production-ready
- Tüm temel özellikler çalışıyor
- Sadece IntelliSense ve syntax validation entegrasyonu gerekiyor
- 60 FPS performans, GPU-accelerated

### Eksik Özellikler
1. IntelliSense aktifleştirilmesi
2. Syntax validation görsel feedback
3. Error panel integration
4. Çift tıklama ile kod editörüne geçiş

### Roadmap
1. ✅ mlp_gui_designer'ı %100'e çıkar
2. ⏳ mlp_flow_editor'ü tamamla (zaman kalırsa)
3. 🗂️ gui_editor_legacy - Arşiv

## 📝 Kullanım

### mlp_gui_designer'ı çalıştır:
```bash
cd editors/mlp_gui_designer
npm start
```

### mlp_flow_editor'ü çalıştır:
```bash
cd editors/mlp_flow_editor
npm run dev
```

## 🎯 Ortak Hedefler

Tüm editörler şu özellikleri desteklemeli:
- ✅ Sürükle-bırak widget yerleştirme
- ✅ Canlı özellik düzenleme
- ✅ Otomatik MLP kod üretimi
- ✅ Dark/Light tema
- ✅ Proje kaydetme/yükleme
- ⏳ IntelliSense (MLP fonksiyonları)
- ⏳ Syntax validation
- ⏳ Error highlighting

## 🔧 Geliştirme

### Yeni editör eklerken:
1. `editors/` altında yeni klasör oluştur
2. `shared/styles/` ve `shared/themes/` kullan
3. Bu README'yi güncelle

### Stil değişiklikleri:
- `shared/styles/main.css` içinde yapılmalı
- Tüm editörler otomatik güncellenir
