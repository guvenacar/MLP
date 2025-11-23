# 🚀 MLP GUI Designer - Hızlı Başlangıç

## 1️⃣ Kurulum (İlk Defa)

```bash
cd mlp_gui_designer
npm install
```

## 2️⃣ Çalıştırma

### Normal Mod
```bash
npm start
```

### Geliştirme Modu (DevTools açık)
```bash
npm run dev
```

## 3️⃣ İlk GUI Uygulamanız

1. **Widget ekleyin:**
   - Sol panelden `Button` widget'ını çalışma alanına sürükleyin
   - Sağ panelde "Metin" özelliğini değiştirin

2. **Özellikleri düzenleyin:**
   - X, Y pozisyonunu ayarlayın
   - Genişlik ve yüksekliği değiştirin
   - Renkleri özelleştirin

3. **Event handler ekleyin:**
   - Sağ panelde "Olaylar" bölümünden `onClick` alanına fonksiyon adı yazın
   - Örnek: `butona_tikla`

4. **Kodu görün:**
   - Alttaki "Kod" sekmesine tıklayın
   - Otomatik oluşturulan MLP kodunu görürsünüz

5. **Kaydedin:**
   - `Ctrl+S` ile projeyi kaydedin (`.mlpgui` formatı)
   - `Ctrl+E` ile MLP kodunu dışa aktarın (`.mlp` formatı)

## 4️⃣ Widget Türleri

### Temel Bileşenler
- 🔘 **Button** - Tıklanabilir düğme
- 🏷️ **Label** - Metin etiketi
- 📝 **TextBox** - Metin giriş kutusu
- ☑️ **CheckBox** - Onay kutusu
- 🔘 **RadioButton** - Radyo düğmesi

### Konteynerler
- 📦 **Panel** - Genel konteyner
- 🗂️ **GroupBox** - Başlıklı grup kutusu

### Listeler
- 📋 **ListBox** - Liste kutusu
- 🔽 **ComboBox** - Açılır liste

## 5️⃣ Klavye Kısayolları

| Kısayol | Açıklama |
|---------|----------|
| `Ctrl+N` | Yeni proje |
| `Ctrl+O` | Proje aç |
| `Ctrl+S` | Kaydet |
| `Ctrl+E` | MLP kodu dışa aktar |
| `F5` | Kodu göster |
| `Delete` | Seçili widget'ı sil |

## 6️⃣ Örnek Akış

```
1. Widget ekle (sürükle-bırak)
   ↓
2. Pozisyon ayarla (fare ile taşı)
   ↓
3. Boyut ayarla (köşelerden sürükle)
   ↓
4. Özellikleri düzenle (sağ panel)
   ↓
5. Event handler ekle (onClick, vb.)
   ↓
6. Projeyi kaydet (Ctrl+S)
   ↓
7. MLP kodu dışa aktar (Ctrl+E)
   ↓
8. MLP derleyicisi ile derle
   ↓
9. Çalıştır!
```

## 7️⃣ İpuçları

### Widget Taşıma
- Widget'a tıklayın ve sürükleyin
- Klavye ok tuşları ile hassas pozisyonlama (yakında)

### Widget Boyutlandırma
- Seçili widget'ın köşelerindeki mavi noktaları sürükleyin
- Minimum boyutlar korunur

### Çoklu Seçim
- Yakında eklenecek!

### Hizalama Araçları
- Yakında eklenecek!

### Izgara (Grid) Sistemi
- Yakında eklenecek!

## 8️⃣ Çalıştırma

Oluşturduğunuz MLP kodunu çalıştırmak için:

```bash
# Kodu dışa aktarın (Ctrl+E)
# Örnek: gui_app.mlp

# MLP derleyicisi ile derleyin
mlpc gui_app.mlp

# Çalıştırın
./gui_app
```

**Not:** MLP runtime'ında GUI fonksiyonlarının implementasyonu gereklidir. Mock veya SDL implementasyonunu kullanabilirsiniz.

## 9️⃣ Sorun Giderme

### "Electron bulunamadı" hatası
```bash
npm install electron --save-dev
```

### "npm: komut bulunamadı"
Node.js ve npm'i yükleyin: https://nodejs.org/

### Widget sürüklenmiyor
- Tarayıcınızı güncelleyin
- DevTools'ta console'u kontrol edin (`npm run dev`)

### Kod oluşturulmuyor
- En az bir widget ekleyin
- "Kod" sekmesine tıklayın

## 🔟 Daha Fazla Bilgi

- **Detaylı Dokümantasyon:** [README.md](README.md)
- **MLP Dili:** [../README.md](../README.md)
- **GUI Runtime:** [../runtime/gui_sdl.c](../runtime/gui_sdl.c)

---

Keyifli kodlamalar! 🎉
