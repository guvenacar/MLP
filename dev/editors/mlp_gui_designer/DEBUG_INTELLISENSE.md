# 🔍 IntelliSense Debug - Detaylı Log Eklendi

## 📊 Mevcut Durum
```
✅ IntelliSense yüklendi (41 fonksiyon + 36 keyword)
✅ getCursorPosition() çalışıyor
❌ show() hiç çağrılmıyor
```

## 🐛 Olası Sorun
`wordBefore.length >= 2` koşulu sağlanmıyor olabilir.

## ✅ Eklenen Debug Logları

### app.js (keyup handler)
```javascript
codeEditor.addEventListener('keyup', (e) => {
  console.log("[keyup] key:", e.key);  // ← YENİ
  
  if (e.key.match(/^[a-zA-Z_]$/)) {
    setTimeout(() => {
      const pos = intelliSense.getCursorPosition(codeEditor);
      const line = intelliSense.getLineText(pos.line);
      const wordBefore = intelliSense.getWordBeforeCursor(line, pos.column);
      
      console.log("[IntelliSense] pos:", pos, "line:", line, "wordBefore:", wordBefore);  // ← YENİ
      console.log("[IntelliSense] wordBefore.length:", wordBefore.length, ">=2?", wordBefore.length >= 2);  // ← YENİ
      
      if (wordBefore.length >= 2) {
        intelliSense.show(codeEditor, pos);
      }
    }, 300);
  }
});
```

## 🧪 Test Et

1. **Uygulamayı kapat** (Ctrl+C terminalde)
2. **Yeniden başlat:**
   ```bash
   cd /home/pardus/projeler/tyd-lang/MLP/editors/mlp_gui_designer
   npm start
   ```
3. **DevTools aç** (Ctrl+Shift+I)
4. **"Kod" sekmesine geç**
5. **`GU` yaz**

## 📋 Beklenen Console Çıktısı

### ✅ DOĞRU Çıktı (çalışıyor):
```
[keyup] key: G
IntelliSense cursor position: {line: 0, column: 1}
[keyup] key: U
IntelliSense cursor position: {line: 0, column: 2}
[IntelliSense] pos: {line: 0, column: 2} line: "GU" wordBefore: "GU"
[IntelliSense] wordBefore.length: 2 >=2? true
IntelliSense.show() called {line: 0, column: 2}
IntelliSense suggestions: 8 items
IntelliSense: Showing popup
```

### ❌ SORUN Varsa Göreceğin:
```
[keyup] key: G
IntelliSense cursor position: {line: 0, column: 1}
[keyup] key: U
IntelliSense cursor position: {line: 0, column: 2}
[IntelliSense] pos: {line: 0, column: 2} line: "GU" wordBefore: ""  ← BOŞ!
[IntelliSense] wordBefore.length: 0 >=2? false  ← SORUN BURADA!
```

## 🔧 Sorun Senaryoları

| Durum | Anlamı | Çözüm |
|-------|--------|-------|
| `[keyup]` yok | keyup olayı çalışmıyor | Event listener bağlanmamış |
| `wordBefore: ""` | getWordBeforeCursor() boş dönüyor | Fonksiyon düzeltmesi gerekli |
| `wordBefore.length: 0` | Kelime tespit edilemiyor | Regex veya cursor pozisyonu sorunu |
| `show() called` yok | if koşulu geçilmiyor | wordBefore < 2 karakter |

## 📝 Backups
- `src/app.js.backup2` (debug öncesi)
- `src/utils/intellisense.js.backup` (orijinal)

**Sonuçları raporla!** ��
