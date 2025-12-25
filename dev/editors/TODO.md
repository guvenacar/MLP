# MLP Editors TODO

## 🎯 ÖNCELİK 1: mlp_gui_designer'ı Bitir

### Kritik (Bugün yapılacak)
- [ ] IntelliSense aktifleştirme testi
  - [ ] Kod editöründe `Ctrl+Space` çalışıyor mu?
  - [ ] `YA` yazdığında YAZDIR önerisi geliyor mu?
  - [ ] Console'da hata var mı kontrol et
  
- [ ] Syntax Validation görsel feedback
  - [ ] Error panel gösterme/gizleme
  - [ ] Satır numaralarında hata işaretleri
  - [ ] Hover ile hata mesajı gösterme

- [ ] Widget'a çift tıklama ile kod editörüne geçiş
  - [ ] dblclick event listener ekle
  - [ ] İlgili kod satırını highlight et
  - [ ] Kod tab'ına otomatik geçiş

### Önemli (Bu hafta)
- [ ] Kod editöründe satır numaraları
- [ ] Error panel styling iyileştirme
- [ ] IntelliSense popup styling düzeltme
- [ ] mlp_functions.json kontrolü
- [ ] Tüm widget türlerini test et

### İyileştirmeler
- [ ] Drag performance optimization
- [ ] Widget selection multiple
- [ ] Undo/Redo stack implementation
- [ ] Copy/Paste widget
- [ ] Keyboard shortcuts testi

## 📋 Özellik Testi Checklist

### Widget İşlemleri
- [ ] Button ekle
- [ ] Label ekle
- [ ] TextBox ekle
- [ ] CheckBox ekle
- [ ] RadioButton ekle
- [ ] Panel ekle
- [ ] GroupBox ekle
- [ ] ListBox ekle
- [ ] ComboBox ekle

### Sürükle-Bırak
- [ ] Toolbox'tan canvas'a sürükleme
- [ ] Widget taşıma
- [ ] Widget boyutlandırma
- [ ] Grid snap çalışması

### Properties Panel
- [ ] Label değiştirme
- [ ] Width/Height değiştirme
- [ ] X/Y pozisyon değiştirme
- [ ] Background color
- [ ] Text color
- [ ] Font özellikleri

### Kod Üretimi
- [ ] MLP kod üretimi test
- [ ] Türkçe syntax
- [ ] İngilizce syntax
- [ ] Event handler'lar

### Menü İşlemleri
- [ ] Dosya > Yeni Proje
- [ ] Dosya > Proje Aç
- [ ] Dosya > Kaydet
- [ ] Dosya > Dışa Aktar
- [ ] Düzenle > Geri Al
- [ ] Düzenle > Kes/Kopyala/Yapıştır
- [ ] Görünüm > Tema Değiştir
- [ ] Görünüm > Panel Gizle/Göster
- [ ] Yardım > MLP Fonksiyonları

## 🔄 ÖNCELİK 2: mlp_flow_editor (Zaman kalırsa)

### Temel Özellikler
- [ ] Drag-drop düzeltme (hala büyük geliyor)
- [ ] Property panel input düzeltme
- [ ] Tüm node türlerini ekle
- [ ] Tema değiştirme test
- [ ] Electron entegrasyonu

### İleri Özellikler
- [ ] Node connections (handle'lar)
- [ ] Auto-layout
- [ ] Export/Import
- [ ] Code generation

## 🗂️ Shared Resources

### Yapılacaklar
- [x] shared/styles/ oluştur
- [x] shared/themes/ oluştur
- [x] Electron CSS'i kopyala
- [ ] Theme switcher utility
- [ ] Widget components library
- [ ] Icon set standardization

### Dokümantasyon
- [x] README.md
- [x] TODO.md
- [ ] SPECS.md (widget specs)
- [ ] CONTRIBUTING.md

## 📊 İlerleme

### mlp_gui_designer: %95
- ✅ UI Layout
- ✅ Widget Toolbox
- ✅ Drag & Drop
- ✅ Properties Panel
- ✅ Code Generation
- ✅ Project Save/Load
- ✅ Menu System
- ⏳ IntelliSense (kodu var, test gerek)
- ⏳ Syntax Validation (kodu var, test gerek)
- ❌ Double-click to code

### mlp_flow_editor: %30
- ✅ Temel yapı
- ✅ React Flow entegrasyonu
- ✅ Drag-drop (çalışıyor ama hatalı)
- ⏳ Node türleri (2/9)
- ⏳ Properties panel (basit)
- ❌ Code generation
- ❌ Save/Load

### gui_editor_legacy: Archived
- Sadece referans için saklanıyor

## 🎯 Bugünkü Hedef

1. mlp_gui_designer Electron penceresini aç
2. IntelliSense test et
3. Syntax validation test et
4. Hataları düzelt
5. %100'e çıkar
6. Git commit

## 📝 Notlar

- mlp_gui_designer neredeyse hazır, sadece son testler gerekiyor
- IntelliSense ve SyntaxValidator kodları zaten var
- mlp_functions.json mevcut
- Electron penceresi açılıyor, GTK hataları önemsiz
- Shared resources tüm editörlerde kullanılacak
