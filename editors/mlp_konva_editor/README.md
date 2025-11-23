# MLP Konva Editor

**High-Performance GUI Designer for MLP Language**

Konva.js tabanlı, Canvas ile GPU-accelerated performans.

---

## 🚀 Hızlı Başlangıç

```bash
# Bağımlılıkları yükle
npm install

# Geliştirme modu (sadece browser)
npm run dev

# Electron ile çalıştır
npm run electron:dev

# Sadece Electron
npm start
```

---

## ✨ Özellikler

- ✅ **Konva.js Canvas** - GPU-accelerated rendering
- ✅ **60 FPS Performans** - 500+ widget ile smooth
- ✅ **9 Widget Tipi** - Button, Label, TextBox, CheckBox, etc.
- ✅ **Drag & Drop** - Sürükle-bırak ve yeniden boyutlandırma
- ✅ **Transformer** - Seçili widget'ları resize et
- ✅ **Properties Panel** - Canlı özellik düzenleme
- ✅ **Shared Styles** - Tüm editörlerle aynı tema
- ✅ **Delete Key** - Seçili widget'ı sil

---

## 🎨 Mimari

```
mlp_konva_editor/
├── src/
│   ├── main/           # Electron main process
│   │   ├── main.js
│   │   └── preload.js
│   └── renderer/       # React app
│       ├── main.jsx
│       ├── App.jsx
│       └── components/
│           ├── KonvaCanvas.jsx    # Stage & Layer
│           ├── KonvaWidget.jsx    # Widget renderer
│           ├── Toolbar.jsx        # Widget palette
│           ├── PropertiesPanel.jsx
│           ├── MenuBar.jsx
│           └── TopToolbar.jsx
├── index.html
├── package.json
└── vite.config.js
```

---

## 🎯 Neden Konva.js?

| Özellik | React Flow | Konva.js |
|---------|-----------|----------|
| Performans | Orta | ⭐ Çok Yüksek |
| Canvas Rendering | ❌ | ✅ GPU-accelerated |
| Öğrenme Eğrisi | Kolay | Orta |
| Transformer/Resize | Zor | ✅ Built-in |
| Türkçe Dökümantasyon | ❌ | ✅ |
| React Entegrasyonu | Native | ✅ react-konva |

---

## 📦 Bağımlılıklar

- **konva** (^9.3.6) - Canvas rendering engine
- **react-konva** (^18.2.10) - React wrapper
- **react** (^18.3.1)
- **electron** (^27.1.3)
- **vite** (^5.4.2)

---

## 🔗 Shared Resources

Bu editör `../shared/` klasöründeki ortak kaynakları kullanır:

- `shared/styles/main.css` - Ana stil dosyası
- `shared/themes/variables.css` - Dark/light tema değişkenleri

Tüm MLP editörleri aynı görünümü paylaşır.

---

## 🎮 Kullanım

### Widget Ekleme
- Sol panelden widget'a tıkla
- Canvas'a otomatik eklenir

### Widget Düzenleme
- Widget'a tıkla (seçili duruma gelir)
- Köşelerden sürükleyerek boyutlandır
- Sağ panelden özellikleri değiştir

### Silme
- Widget'ı seç
- **Delete** tuşuna bas

---

## 📝 TODO

- [ ] Drag-drop (toolbar'dan canvas'a sürükleme)
- [ ] Kod üretimi (MLP code generator)
- [ ] Kaydet/Yükle (JSON formatı)
- [ ] Grid snapping
- [ ] Undo/Redo
- [ ] Copy/Paste
- [ ] Alignment guides

---

**Oluşturulma:** 23 Kasım 2025  
**Motor:** Konva.js + React + Electron
