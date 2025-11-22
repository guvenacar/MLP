# MLP GUI Designer - Değişiklik Günlüğü

## [1.0.2] - 22 Kasım 2024

### Eklenenler ✨
- **Light/Dark Tema Desteği** 🎨:
  - Light ve Dark tema arasında geçiş (Görünüm > Tema: Dark/Light)
  - Otomatik tema kaydetme (localStorage)
  - CSS değişkenleri ile dinamik renkler
  - Tema geçişinde smooth animasyon (0.3s)
  - Grid rengi temaya göre otomatik ayarlanıyor

### İyileştirmeler 🔧
- Tüm hardcoded renkler CSS değişkenlerine dönüştürüldü
- Tema tercihi tarayıcıda saklanıyor
- Light theme için optimize edilmiş renk paleti

---

## [1.0.1] - 22 Kasım 2024

### Eklenenler ✨
- **Düzenle Menüsü**:
  - Geri Al (Ctrl+Z) - Undo özelliği
  - Yinele (Ctrl+Y) - Redo özelliği
  - Kes (Ctrl+X) - Widget kesme
  - Kopyala (Ctrl+C) - Widget kopyalama
  - Yapıştır (Ctrl+V) - Widget yapıştırma
  - Sil (Delete) - Widget silme
  - Tümünü Seç (Ctrl+A) - Yakında

- **Görünüm Menüsü**:
  - Widget Araçlarını Gizle/Göster - Sol panel toggle
  - Özellikleri Gizle/Göster - Sağ panel toggle
  - Yakınlaştır (+) - Zoom in
  - Uzaklaştır (-) - Zoom out
  - Zoom Sıfırla (100%) - Reset zoom
  - Izgara Göster/Gizle - Grid toggle

- **Yardım Menüsü**:
  - Hızlı Başlangıç - İlk adımlar rehberi
  - Klavye Kısayolları - Tüm kısayollar listesi
  - Dokümantasyon - GUI Designer dokümanları
  - MLP Dili Dokümantasyonu - MLP dili hakkında
  - Hakkında - Uygulama bilgileri

### İyileştirmeler 🔧
- Undo/Redo history sistemi (max 50 adım)
- Clipboard desteği (kes/kopyala/yapıştır)
- Zoom özelliği (50% - 200%)
- Panel gizleme/gösterme
- Grid açma/kapama

### Klavye Kısayolları ⌨️
- `Ctrl+Z` - Geri al
- `Ctrl+Y` - Yinele
- `Ctrl+X` - Kes
- `Ctrl+C` - Kopyala
- `Ctrl+V` - Yapıştır

---

## [1.0.0] - 22 Kasım 2024

### İlk Sürüm 🎉

#### Temel Özellikler
- Visual Studio tarzında arayüz
- 9 widget türü (Button, Label, TextBox, CheckBox, RadioButton, Panel, GroupBox, ListBox, ComboBox)
- Sürükle-bırak widget ekleme
- Canlı özellik editörü
- Tasarım/Kod görünüm geçişi
- Otomatik MLP kod üretimi
- Proje kaydetme/yükleme (.mlpgui formatı)
- MLP kod dışa aktarma (.mlp formatı)

#### Widget Özellikleri
- Pozisyon ve boyut düzenleme
- Renk ve font özelleştirme
- Event handler tanımlama
- Görünürlük ve etkinlik kontrolü

#### Klavye Kısayolları
- `Ctrl+N` - Yeni proje
- `Ctrl+O` - Proje aç
- `Ctrl+S` - Kaydet
- `Ctrl+E` - MLP kodu dışa aktar
- `F5` - Kodu göster/çalıştır
- `Delete` - Widget sil

#### Teknik Detaylar
- Electron 28+
- Vanilla JavaScript (framework yok)
- Pure CSS (Dark Theme)
- Context Isolation (güvenlik)
- IPC Communication

#### Dosyalar
- 3,000+ satır kod
- Detaylı dokümantasyon (README, QUICKSTART, ARCHITECTURE)
- Örnek proje dosyası
- Tam kaynak kodu

---

## Gelecek Özellikler (Roadmap)

### v1.1.0 (Planlanan)
- [ ] Çoklu widget seçimi (Ctrl+Click)
- [ ] Hizalama araçları (Align Left/Right/Top/Bottom)
- [ ] Widget gruplama
- [ ] Layout yöneticileri (FlowLayout, GridLayout)
- [ ] Özellik arama/filtreleme
- [ ] Widget şablonları

### v1.2.0 (Planlanan)
- [ ] Tema desteği (Light/Dark)
- [ ] Custom widget oluşturma
- [ ] Widget kütüphanesi
- [ ] Snippet sistemi
- [ ] Kod formatı seçenekleri
- [ ] Çok dilli UI (EN/TR)

### v1.3.0 (Planlanan)
- [ ] Real-time preview (SDL)
- [ ] Breakpoint debug
- [ ] Widget animasyonları
- [ ] Responsive design araçları
- [ ] Version control entegrasyonu
- [ ] Team collaboration

---

## Bilinen Sorunlar

### v1.0.1
- Tümünü Seç özelliği henüz çalışmıyor
- Zoom özelliği bazı durumlarda hizalama sorunlarına yol açabilir
- Çok sayıda widget ile undo/redo yavaşlayabilir

### Geçici Çözümler
- **Zoom sorunları**: Zoom'u sıfırlayın (Ctrl+0)
- **Undo/Redo yavaşlığı**: Proje kaydedin ve yeniden açın

---

## Katkıda Bulunanlar

- MLP Team
- Topluluk geri bildirimleri

## Lisans

MIT License - Detaylar için LICENSE dosyasına bakın.
