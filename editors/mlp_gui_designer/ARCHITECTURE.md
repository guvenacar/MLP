# MLP GUI Designer - Mimari Dokümantasyonu

## 🏗️ Genel Mimari

MLP GUI Designer, Electron framework'ü üzerine inşa edilmiş, modern bir desktop uygulamasıdır.

```
┌─────────────────────────────────────────────────────────┐
│                    Electron App                          │
├─────────────────────────────────────────────────────────┤
│  Main Process (Node.js)     │  Renderer Process (Web)   │
│  ├─ main.js                 │  ├─ index.html            │
│  ├─ preload.js              │  ├─ src/app.js            │
│  └─ IPC Handlers            │  ├─ src/components/       │
│     ├─ save-project         │  │  ├─ designer.js        │
│     ├─ open-project         │  │  ├─ widget.js          │
│     ├─ export-mlp-code      │  │  ├─ properties.js      │
│     └─ new-project          │  │  └─ code-generator.js  │
│                             │  └─ src/utils/helpers.js  │
└─────────────────────────────────────────────────────────┘
```

## 📦 Modül Yapısı

### 1. Main Process (main.js)
**Sorumluluklar:**
- Electron pencere yönetimi
- Dosya sistemi işlemleri (save/load)
- IPC (Inter-Process Communication) handler'ları
- Native dialog'lar (Open/Save)

**API:**
```javascript
// Proje kaydetme
ipcMain.handle('save-project', async (event, projectData) => {...})

// Proje açma
ipcMain.handle('open-project', async () => {...})

// MLP kod dışa aktarma
ipcMain.handle('export-mlp-code', async (event, mlpCode) => {...})
```

### 2. Preload Script (preload.js)
**Sorumluluklar:**
- Güvenli API köprüsü (contextBridge)
- Main ve Renderer process arasında güvenli iletişim

**Exposed API:**
```javascript
window.electronAPI = {
  saveProject: (projectData) => ipcRenderer.invoke('save-project', projectData),
  openProject: () => ipcRenderer.invoke('open-project'),
  exportMlpCode: (mlpCode) => ipcRenderer.invoke('export-mlp-code', mlpCode)
}
```

### 3. Renderer Process

#### a. App Controller (src/app.js)
**Sorumluluklar:**
- Ana uygulama mantığı
- UI event handling
- Menü ve toolbar işlemleri
- Klavye kısayolları
- Tab yönetimi (Design/Code)

**Önemli Fonksiyonlar:**
```javascript
handleNewProject()      // Yeni proje oluştur
handleOpenProject()     // Proje aç
handleSaveProject()     // Projeyi kaydet
handleExportCode()      // MLP kodu dışa aktar
updateCodeView()        // Kod görünümünü güncelle
```

#### b. Designer (src/components/designer.js)
**Sorumluluklar:**
- Widget yönetimi (CRUD)
- Drag & Drop sistemi
- Widget seçimi
- Resize/Move işlemleri
- Canvas yönetimi

**Önemli Özellikler:**
```javascript
class Designer {
  widgets[]               // Tüm widget'lar
  selectedWidget          // Seçili widget
  propertiesPanel         // Özellik paneli referansı
  codeGenerator           // Kod üreteci referansı

  addWidget()             // Widget ekle
  selectWidget()          // Widget seç
  deleteWidget()          // Widget sil
  updateWidgetProperty()  // Widget özelliği güncelle
  generateCode()          // MLP kodu üret
  toJSON()                // Projeyi JSON'a çevir
  loadFromJSON()          // JSON'dan proje yükle
}
```

**Event Handling:**
- `onToolboxDragStart()` - Toolbox'tan sürükleme başlangıcı
- `onCanvasDrop()` - Canvas'a widget bırakma
- `onCanvasMouseDown()` - Widget seçme/taşıma başlangıcı
- `onDocumentMouseMove()` - Widget taşıma/resize
- `onDocumentMouseUp()` - Taşıma/resize bitişi
- `onCanvasDoubleClick()` - Kod görünümüne geç
- `onKeyDown()` - Klavye olayları (Delete)

#### c. Widget (src/components/widget.js)
**Sorumluluklar:**
- Widget veri modeli
- DOM rendering
- Widget özellikleri yönetimi

**Widget Sınıfı:**
```javascript
class Widget {
  id                      // Benzersiz ID
  type                    // Widget tipi (button, label, etc.)
  properties              // Özellikler (x, y, width, height, text, etc.)
  element                 // DOM elementi

  render()                // DOM'a render et
  renderWidgetContent()   // Widget'a özel içerik
  addResizeHandles()      // Resize handle'ları ekle
  update()                // Özellikleri güncelle
  toJSON()                // JSON'a dönüştür
  static fromJSON()       // JSON'dan oluştur
}
```

**Desteklenen Widget Tipleri:**
- `button` - Tıklanabilir düğme
- `label` - Metin etiketi
- `textbox` - Metin giriş kutusu
- `checkbox` - Onay kutusu
- `radio` - Radyo düğmesi
- `panel` - Genel konteyner
- `groupbox` - Başlıklı grup kutusu
- `listbox` - Liste kutusu
- `combobox` - Açılır liste

#### d. Properties Panel (src/components/properties.js)
**Sorumluluklar:**
- Özellik paneli UI'ı
- Özellik editörleri (input, color, checkbox, etc.)
- Event handler editörleri

**PropertiesPanel Sınıfı:**
```javascript
class PropertiesPanel {
  container               // DOM container
  currentWidget           // Şu anki widget
  onPropertyChange        // Callback fonksiyon

  showProperties()        // Widget özelliklerini göster
  renderPropertyGroup()   // Özellik grubu render et
  createPropertyInput()   // Özellik input'u oluştur
  renderItemsEditor()     // Liste öğeleri editörü
  renderEventHandlers()   // Event handler'ları göster
  clear()                 // Paneli temizle
}
```

**Özellik Grupları:**
1. **Pozisyon ve Boyut**: x, y, width, height
2. **Görünüm**: text, backgroundColor, textColor, fontSize
3. **Davranış**: enabled, visible, checked
4. **Özel**: Widget'a özel özellikler
5. **Olaylar**: onClick, onChange, onSelect

#### e. Code Generator (src/components/code-generator.js)
**Sorumluluklar:**
- Widget'lardan MLP kodu üretme
- Event handler fonksiyon şablonları
- GUI başlatma kodu
- Event loop kodu

**CodeGenerator Sınıfı:**
```javascript
class CodeGenerator {
  language                // Dil (tr-TR, en-US, etc.)

  generate()              // Ana kod üretim fonksiyonu
  collectEventHandlers()  // Event handler'ları topla
  generateEventHandler()  // Handler fonksiyonu üret
  generateWidgetCode()    // Widget kodu üret
}
```

**Üretilen Kod Yapısı:**
```mlp
-- Header (lang, comments)
-- Widget ID tanımlamaları
-- Event handler fonksiyonları
-- Ana program
  -- Pencere oluştur
  -- Widget'ları oluştur
  -- Event loop
    -- Olayları işle
    -- Ekranı güncelle
  -- Temizlik
```

#### f. Helpers (src/utils/helpers.js)
**Yardımcı Fonksiyonlar:**
```javascript
generateId()                  // Benzersiz ID üret
getDefaultProperties(type)    // Varsayılan özellikler
getWidgetTypeName(type)       // Widget tipi adı (Türkçe)
rgbToHex(rgb)                // RGB → Hex dönüşümü
hexToRgb(hex)                // Hex → RGB dönüşümü
clearElement(element)         // DOM element temizle
showStatus(message)           // Durum mesajı göster
updateWidgetCount(count)      // Widget sayısını güncelle
```

## 🎨 UI Bileşenleri

### 1. Top Bar
- **Menu Bar**: Dosya, Düzenle, Görünüm, Yardım
- **Toolbar**: Hızlı erişim butonları (New, Open, Save, Run, Export)

### 2. Left Panel
- **Widget Toolbox**: Kategorize widget'lar
  - Temel Bileşenler
  - Konteynerler
  - Listeler

### 3. Center Panel
- **Tabs**: Tasarım / Kod
- **Design Canvas**: Sürükle-bırak çalışma alanı
- **Code Editor**: Otomatik oluşturulan MLP kodu

### 4. Right Panel
- **Properties Container**: Seçili widget özellikleri
  - Pozisyon ve Boyut
  - Görünüm
  - Davranış
  - Olaylar

### 5. Status Bar
- **Status Text**: Durum mesajları
- **Widget Count**: Toplam widget sayısı

## 🔄 Veri Akışı

### Widget Ekleme
```
1. Toolbox'tan widget sürüklenir (dragstart)
   ↓
2. Canvas'a bırakılır (drop)
   ↓
3. Designer.addWidget() çağrılır
   ↓
4. Widget instance oluşturulur
   ↓
5. Widget.render() ile DOM'a eklenir
   ↓
6. Widget otomatik seçilir
   ↓
7. PropertiesPanel.showProperties() çağrılır
```

### Özellik Değiştirme
```
1. Properties panel'de input değiştirilir (change event)
   ↓
2. onPropertyChange callback çağrılır
   ↓
3. Designer.updateWidgetProperty() çağrılır
   ↓
4. Widget.update() ile widget güncellenir
   ↓
5. DOM yeniden render edilir
```

### Proje Kaydetme
```
1. Ctrl+S veya Save butonu
   ↓
2. Designer.toJSON() ile proje serileştirilir
   ↓
3. window.electronAPI.saveProject() çağrılır
   ↓
4. IPC üzerinden main process'e gönderilir
   ↓
5. Native dialog gösterilir
   ↓
6. JSON dosyası kaydedilir (.mlpgui)
```

### Kod Üretme
```
1. Ctrl+E veya Export butonu
   ↓
2. Designer.generateCode() çağrılır
   ↓
3. CodeGenerator.generate() widget listesini işler
   ↓
4. MLP kodu string olarak oluşturulur
   ↓
5. window.electronAPI.exportMlpCode() çağrılır
   ↓
6. Native dialog gösterilir
   ↓
7. .mlp dosyası kaydedilir
```

## 🎯 Tasarım Prensipleri

### 1. Separation of Concerns
- Her modül tek bir sorumluluğa sahip
- Widget (model), Designer (controller), PropertiesPanel (view)

### 2. Event-Driven Architecture
- Tüm etkileşimler event'ler üzerinden
- Callback'ler ile loose coupling

### 3. Immutability
- Widget güncellemeleri yeni özellik objesi ile
- State mutation minimize edilmiş

### 4. Visual Studio Style
- Tanıdık arayüz (toolbox, properties, canvas)
- Klavye kısayolları
- Context menu'ler (yakında)

### 5. Extensibility
- Yeni widget tipleri kolayca eklenebilir
- Özellik grupları genişletilebilir
- Kod generator özelleştirilebilir

## 🔒 Güvenlik

### Context Isolation
- Renderer process'te `nodeIntegration: false`
- `contextIsolation: true`
- Preload script ile güvenli API

### IPC Security
- Sadece belirli IPC channel'ları expose edilmiş
- Dosya yolu validation
- JSON serileştirme

## 🚀 Performans

### Optimizasyonlar
- Event delegation kullanımı
- DOM güncellemelerini batch'leme
- Gereksiz re-render'ları önleme
- Resize handle'lar sadece seçili widget'ta görünür

### Gelecek İyileştirmeler
- Virtual scrolling (çok sayıda widget için)
- Undo/Redo stack
- Widget caching
- Code generation memoization

## 📝 Dosya Formatları

### .mlpgui (Project File)
```json
{
  "version": "1.0",
  "widgets": [
    {
      "id": "widget_1",
      "type": "button",
      "properties": {
        "x": 100,
        "y": 100,
        "width": 120,
        "height": 35,
        "text": "Button",
        ...
      }
    }
  ]
}
```

### .mlp (Generated Code)
```mlp
-- lang: tr-TR
-- MLP GUI Kodu

numeric widget_1

func handler_name() -> numeric
    YAZDIR "Event handled"
    return 0
end func

GUI_PENCERE_AC 800 600 "MLP App"
widget_1 = GUI_BUTTON_CREATE(1, "Button", 100, 100, 120, 35)

numeric running = 1
while running == 1
    numeric event = GUI_POLL_EVENT()
    ...
end while

GUI_KAPAT()
```

## 🧪 Test Stratejisi

### Manuel Test
1. Widget ekleme/silme
2. Sürükle-bırak
3. Özellik değiştirme
4. Proje kaydetme/yükleme
5. Kod üretimi

### Gelecek Test Planları
- Unit tests (Jest)
- Integration tests
- E2E tests (Spectron)
- Visual regression tests

## 🔧 Geliştirme

### Yeni Widget Ekleme
1. `helpers.js`'te `getDefaultProperties()` fonksiyonuna ekle
2. `widget.js`'te `renderWidgetContent()` fonksiyonuna case ekle
3. `code-generator.js`'te `generateWidgetCode()` fonksiyonuna case ekle
4. `main.css`'te `.widget-{type}` stili ekle
5. `index.html`'de toolbox'a ekle

### Yeni Özellik Ekleme
1. Widget default properties'e ekle
2. `properties.js`'te ilgili grup altına ekle
3. `createPropertyInput()` ile input oluştur
4. Code generator'da kullan

## 📚 Referanslar

- [Electron Documentation](https://www.electronjs.org/docs)
- [MLP Language Docs](../README.md)
- [GUI Runtime API](../runtime/gui_sdl.c)

---

**Son Güncelleme:** 22 Kasım 2024
