// Ana Uygulama

// Global designer instance
let designer;

// Uygulama başlangıcı
document.addEventListener('DOMContentLoaded', () => {
  designer = new Designer();
  initializeUI();
  loadSavedTheme(); // Kaydedilmiş temayı yükle
  showStatus('MLP GUI Designer hazır');
});

// UI event listener'larını başlat
function initializeUI() {
  // Tab geçişleri
  const tabDesign = document.getElementById('tab-design');
  const tabCode = document.getElementById('tab-code');
  const viewDesign = document.getElementById('view-design');
  const viewCode = document.getElementById('view-code');
  const codeEditor = document.getElementById('code-editor');

  tabDesign.addEventListener('click', () => {
    tabDesign.classList.add('active');
    tabCode.classList.remove('active');
    viewDesign.classList.remove('hidden');
    viewCode.classList.add('hidden');
    showStatus('Tasarım görünümü');
  });

  tabCode.addEventListener('click', () => {
    tabCode.classList.add('active');
    tabDesign.classList.remove('active');
    viewCode.classList.remove('hidden');
    viewDesign.classList.add('hidden');

    // Kodu güncelle
    updateCodeView();
    showStatus('Kod görünümü');
  });

  // Dosya menüsü
  document.getElementById('menu-new').addEventListener('click', handleNewProject);
  document.getElementById('menu-open').addEventListener('click', handleOpenProject);
  document.getElementById('menu-save').addEventListener('click', handleSaveProject);
  document.getElementById('menu-export').addEventListener('click', handleExportCode);

  // Düzenle menüsü
  document.getElementById('menu-undo').addEventListener('click', handleUndo);
  document.getElementById('menu-redo').addEventListener('click', handleRedo);
  document.getElementById('menu-cut').addEventListener('click', handleCut);
  document.getElementById('menu-copy').addEventListener('click', handleCopy);
  document.getElementById('menu-paste').addEventListener('click', handlePaste);
  document.getElementById('menu-delete').addEventListener('click', handleDelete);
  document.getElementById('menu-select-all').addEventListener('click', handleSelectAll);

  // Görünüm menüsü
  document.getElementById('menu-toggle-theme').addEventListener('click', handleToggleTheme);
  document.getElementById('menu-toggle-toolbox').addEventListener('click', handleToggleToolbox);
  document.getElementById('menu-toggle-properties').addEventListener('click', handleToggleProperties);
  document.getElementById('menu-zoom-in').addEventListener('click', handleZoomIn);
  document.getElementById('menu-zoom-out').addEventListener('click', handleZoomOut);
  document.getElementById('menu-zoom-reset').addEventListener('click', handleZoomReset);
  document.getElementById('menu-toggle-grid').addEventListener('click', handleToggleGrid);

  // Yardım menüsü
  document.getElementById('menu-quickstart').addEventListener('click', handleQuickstart);
  document.getElementById('menu-keyboard-shortcuts').addEventListener('click', handleKeyboardShortcuts);
  document.getElementById('menu-documentation').addEventListener('click', handleDocumentation);
  document.getElementById('menu-mlp-docs').addEventListener('click', handleMlpDocs);
  document.getElementById('menu-about').addEventListener('click', handleAbout);

  // Toolbar butonları
  document.getElementById('btn-new').addEventListener('click', handleNewProject);
  document.getElementById('btn-open').addEventListener('click', handleOpenProject);
  document.getElementById('btn-save').addEventListener('click', handleSaveProject);
  document.getElementById('btn-export').addEventListener('click', handleExportCode);
  document.getElementById('btn-run').addEventListener('click', handleRun);
}

// Kod görünümünü güncelle
function updateCodeView() {
  const codeEditor = document.getElementById('code-editor');
  const code = designer.generateCode();
  codeEditor.textContent = code;
}

// Yeni proje
async function handleNewProject() {
  if (designer.widgets.length > 0) {
    const result = await window.electronAPI.newProject();
    if (!result.confirmed) return;
  }

  designer.clear();
  showStatus('Yeni proje oluşturuldu');
}

// Proje aç
async function handleOpenProject() {
  try {
    const result = await window.electronAPI.openProject();

    if (result.success && result.data) {
      designer.loadFromJSON(result.data);
      showStatus('Proje açıldı: ' + result.filePath);
    } else if (result.error) {
      alert('Proje açılırken hata: ' + result.error);
    }
  } catch (error) {
    console.error('Proje açma hatası:', error);
    alert('Proje açılırken bir hata oluştu');
  }
}

// Proje kaydet
async function handleSaveProject() {
  try {
    const projectData = designer.toJSON();
    const result = await window.electronAPI.saveProject(projectData);

    if (result.success) {
      showStatus('Proje kaydedildi: ' + result.filePath);
    } else if (result.error) {
      alert('Proje kaydedilirken hata: ' + result.error);
    }
  } catch (error) {
    console.error('Proje kaydetme hatası:', error);
    alert('Proje kaydedilirken bir hata oluştu');
  }
}

// MLP kodunu dışa aktar
async function handleExportCode() {
  try {
    const mlpCode = designer.generateCode();
    const result = await window.electronAPI.exportMlpCode(mlpCode);

    if (result.success) {
      showStatus('MLP kodu dışa aktarıldı: ' + result.filePath);
    } else if (result.error) {
      alert('Kod dışa aktarılırken hata: ' + result.error);
    }
  } catch (error) {
    console.error('Kod dışa aktarma hatası:', error);
    alert('Kod dışa aktarılırken bir hata oluştu');
  }
}

// Çalıştır (önizleme)
function handleRun() {
  // Kod görünümüne geç ve kodu göster
  const tabCode = document.getElementById('tab-code');
  tabCode.click();
  showStatus('Kod oluşturuldu - MLP derleyicisi ile çalıştırabilirsiniz');

  // Bilgilendirme mesajı
  setTimeout(() => {
    alert(
      'MLP GUI Kodu Oluşturuldu!\n\n' +
      'Kodu çalıştırmak için:\n' +
      '1. "Dosya > MLP Kodu Dışa Aktar" ile .mlp dosyası olarak kaydedin\n' +
      '2. MLP derleyicisi ile derleyin: mlpc gui_program.mlp\n' +
      '3. Çalıştırın: ./gui_program\n\n' +
      'Not: MLP GUI runtime fonksiyonlarının yüklü olması gerekir.'
    );
  }, 500);
}

// Klavye kısayolları
document.addEventListener('keydown', (e) => {
  // Ctrl+S: Kaydet
  if (e.ctrlKey && e.key === 's') {
    e.preventDefault();
    handleSaveProject();
  }

  // Ctrl+O: Aç
  if (e.ctrlKey && e.key === 'o') {
    e.preventDefault();
    handleOpenProject();
  }

  // Ctrl+N: Yeni
  if (e.ctrlKey && e.key === 'n') {
    e.preventDefault();
    handleNewProject();
  }

  // Ctrl+E: Dışa aktar
  if (e.ctrlKey && e.key === 'e') {
    e.preventDefault();
    handleExportCode();
  }

  // F5: Çalıştır
  if (e.key === 'F5') {
    e.preventDefault();
    handleRun();
  }

  // Ctrl+Z: Geri al
  if (e.ctrlKey && e.key === 'z') {
    e.preventDefault();
    handleUndo();
  }

  // Ctrl+Y: Yinele
  if (e.ctrlKey && e.key === 'y') {
    e.preventDefault();
    handleRedo();
  }

  // Ctrl+X: Kes
  if (e.ctrlKey && e.key === 'x') {
    e.preventDefault();
    handleCut();
  }

  // Ctrl+C: Kopyala
  if (e.ctrlKey && e.key === 'c') {
    e.preventDefault();
    handleCopy();
  }

  // Ctrl+V: Yapıştır
  if (e.ctrlKey && e.key === 'v') {
    e.preventDefault();
    handlePaste();
  }
});

// Menü dropdown'ları
document.querySelectorAll('.menu-item').forEach(item => {
  let timeout;

  item.addEventListener('mouseenter', () => {
    clearTimeout(timeout);
    const dropdown = item.querySelector('.menu-dropdown');
    if (dropdown) {
      dropdown.style.display = 'block';
    }
  });

  item.addEventListener('mouseleave', () => {
    const dropdown = item.querySelector('.menu-dropdown');
    if (dropdown) {
      timeout = setTimeout(() => {
        dropdown.style.display = 'none';
      }, 200);
    }
  });
});

// ==================== DÜZENLE MENÜSÜ ====================

// Undo/Redo için basit history sistemi
let undoStack = [];
let redoStack = [];

function saveToUndoStack() {
  if (designer && designer.widgets) {
    undoStack.push(JSON.stringify(designer.toJSON()));
    redoStack = []; // Yeni değişiklik yapıldığında redo stack'i temizle
    if (undoStack.length > 50) { // Max 50 undo
      undoStack.shift();
    }
  }
}

function handleUndo() {
  if (undoStack.length === 0) {
    showStatus('Geri alınacak işlem yok');
    return;
  }

  // Mevcut durumu redo stack'e kaydet
  redoStack.push(JSON.stringify(designer.toJSON()));

  // Son durumu geri yükle
  const prevState = undoStack.pop();
  designer.loadFromJSON(JSON.parse(prevState));
  showStatus('Geri alındı');
}

function handleRedo() {
  if (redoStack.length === 0) {
    showStatus('Yinelenecek işlem yok');
    return;
  }

  // Mevcut durumu undo stack'e kaydet
  undoStack.push(JSON.stringify(designer.toJSON()));

  // Redo durumunu geri yükle
  const nextState = redoStack.pop();
  designer.loadFromJSON(JSON.parse(nextState));
  showStatus('Yinelendi');
}

let clipboard = null;

function handleCut() {
  if (!designer.selectedWidget) {
    showStatus('Kesilecek widget seçilmedi');
    return;
  }
  clipboard = JSON.stringify(designer.selectedWidget.toJSON());
  designer.deleteWidget(designer.selectedWidget);
  showStatus('Widget kesildi');
}

function handleCopy() {
  if (!designer.selectedWidget) {
    showStatus('Kopyalanacak widget seçilmedi');
    return;
  }
  clipboard = JSON.stringify(designer.selectedWidget.toJSON());
  showStatus('Widget kopyalandı');
}

function handlePaste() {
  if (!clipboard) {
    showStatus('Yapıştırılacak widget yok');
    return;
  }
  try {
    const widgetData = JSON.parse(clipboard);
    // Yeni pozisyonda yapıştır (biraz kaydır)
    widgetData.properties.x += 20;
    widgetData.properties.y += 20;
    const widget = Widget.fromJSON(widgetData);
    designer.addWidget(widget);
    showStatus('Widget yapıştırıldı');
  } catch (error) {
    showStatus('Yapıştırma hatası');
  }
}

function handleDelete() {
  if (!designer.selectedWidget) {
    showStatus('Silinecek widget seçilmedi');
    return;
  }
  designer.deleteWidget(designer.selectedWidget);
}

function handleSelectAll() {
  showStatus('Tümünü seç özelliği yakında eklenecek');
}

// ==================== GÖRÜNÜM MENÜSÜ ====================

let currentTheme = 'dark'; // Varsayılan: dark

function handleToggleTheme() {
  const body = document.body;

  if (currentTheme === 'dark') {
    body.classList.add('light-theme');
    currentTheme = 'light';
    showStatus('Light tema aktif');

    // Grid rengini güncelle
    if (gridVisible) {
      const canvas = document.getElementById('canvas');
      canvas.style.backgroundImage =
        'linear-gradient(rgba(0,0,0,.05) 1px, transparent 1px), ' +
        'linear-gradient(90deg, rgba(0,0,0,.05) 1px, transparent 1px)';
    }
  } else {
    body.classList.remove('light-theme');
    currentTheme = 'dark';
    showStatus('Dark tema aktif');

    // Grid rengini güncelle
    if (gridVisible) {
      const canvas = document.getElementById('canvas');
      canvas.style.backgroundImage =
        'linear-gradient(rgba(255,255,255,.05) 1px, transparent 1px), ' +
        'linear-gradient(90deg, rgba(255,255,255,.05) 1px, transparent 1px)';
    }
  }

  // Local storage'a kaydet
  try {
    localStorage.setItem('mlp-gui-designer-theme', currentTheme);
  } catch (e) {
    // Ignore localStorage errors
  }
}

// Sayfa yüklendiğinde kaydedilmiş temayı yükle
function loadSavedTheme() {
  try {
    const savedTheme = localStorage.getItem('mlp-gui-designer-theme');
    if (savedTheme === 'light') {
      handleToggleTheme(); // Light tema geç
    }
  } catch (e) {
    // Ignore localStorage errors
  }
}

function handleToggleToolbox() {
  const leftPanel = document.querySelector('.left-panel');
  if (leftPanel.style.display === 'none') {
    leftPanel.style.display = 'flex';
    showStatus('Widget araçları gösteriliyor');
  } else {
    leftPanel.style.display = 'none';
    showStatus('Widget araçları gizlendi');
  }
}

function handleToggleProperties() {
  const rightPanel = document.querySelector('.right-panel');
  if (rightPanel.style.display === 'none') {
    rightPanel.style.display = 'flex';
    showStatus('Özellikler gösteriliyor');
  } else {
    rightPanel.style.display = 'none';
    showStatus('Özellikler gizlendi');
  }
}

let currentZoom = 1.0;

function handleZoomIn() {
  currentZoom = Math.min(currentZoom + 0.1, 2.0);
  applyZoom();
  showStatus(`Zoom: ${Math.round(currentZoom * 100)}%`);
}

function handleZoomOut() {
  currentZoom = Math.max(currentZoom - 0.1, 0.5);
  applyZoom();
  showStatus(`Zoom: ${Math.round(currentZoom * 100)}%`);
}

function handleZoomReset() {
  currentZoom = 1.0;
  applyZoom();
  showStatus('Zoom sıfırlandı (100%)');
}

function applyZoom() {
  const canvas = document.getElementById('canvas');
  canvas.style.transform = `scale(${currentZoom})`;
  canvas.style.transformOrigin = 'top left';
}

let gridVisible = true;

function handleToggleGrid() {
  const canvas = document.getElementById('canvas');
  gridVisible = !gridVisible;

  if (gridVisible) {
    canvas.style.backgroundImage =
      'linear-gradient(rgba(255,255,255,.05) 1px, transparent 1px), ' +
      'linear-gradient(90deg, rgba(255,255,255,.05) 1px, transparent 1px)';
    showStatus('Izgara gösteriliyor');
  } else {
    canvas.style.backgroundImage = 'none';
    showStatus('Izgara gizlendi');
  }
}

// ==================== YARDIM MENÜSÜ ====================

function handleQuickstart() {
  const message = `
🚀 MLP GUI Designer - Hızlı Başlangıç

1️⃣ Widget Ekleme:
   - Sol panelden widget seçin
   - Çalışma alanına sürükleyin

2️⃣ Widget Düzenleme:
   - Taşımak için sürükleyin
   - Köşelerden boyutlandırın
   - Sağ panelde özellikleri düzenleyin

3️⃣ Event Handler:
   - Widget seçin
   - Sağ panel > Olaylar
   - Fonksiyon adı girin

4️⃣ Kod Üretme:
   - Ctrl+E veya Dosya > Dışa Aktar
   - .mlp dosyası kaydedin
   - mlpc ile derleyin

5️⃣ Klavye Kısayolları:
   - Ctrl+S: Kaydet
   - Ctrl+N: Yeni
   - Ctrl+O: Aç
   - Delete: Sil

Detaylı bilgi için Yardım > Dokümantasyon
  `;
  alert(message);
}

function handleKeyboardShortcuts() {
  const message = `
⌨️ Klavye Kısayolları

📁 Dosya:
  Ctrl+N  - Yeni proje
  Ctrl+O  - Proje aç
  Ctrl+S  - Kaydet
  Ctrl+E  - MLP kodu dışa aktar
  F5      - Kodu göster/çalıştır

✂️ Düzenle:
  Ctrl+Z  - Geri al
  Ctrl+Y  - Yinele
  Ctrl+X  - Kes
  Ctrl+C  - Kopyala
  Ctrl+V  - Yapıştır
  Delete  - Sil

👁️ Görünüm:
  +       - Yakınlaştır
  -       - Uzaklaştır

🖱️ Widget İşlemleri:
  Sürükle      - Taşı
  Köşe Sürükle - Boyutlandır
  Çift Tık     - Kod görünümüne geç
  Delete       - Sil
  `;
  alert(message);
}

function handleDocumentation() {
  const message = `
📚 MLP GUI Designer Dokümantasyonu

Dosya Konumu:
  mlp_gui_designer/README.md
  mlp_gui_designer/QUICKSTART.md
  mlp_gui_designer/ARCHITECTURE.md

Online Dokümantasyon:
  https://github.com/your-repo/MLP

İçerik:
  • Widget türleri ve özellikleri
  • Kod üretimi
  • Proje yönetimi
  • İleri seviye özellikler

Dokümantasyon dosyalarını bir metin editörü
veya markdown viewer ile açabilirsiniz.
  `;
  alert(message);
}

function handleMlpDocs() {
  const message = `
📖 MLP Dili Dokümantasyonu

Dosya Konumu:
  ../README.md
  ../SPECS.md
  ../docs/

MLP Dili Özellikleri:
  • Çok dilli syntax (TR, EN, RU, ZH, HI)
  • Self-hosting compiler
  • x86-64 assembly output
  • Struct, enum, list desteği
  • GUI runtime

Örnekler:
  ../examples/
  ../tests/

MLP derleyicisi ile ilgili detaylı bilgi
için ana README dosyasına bakın.
  `;
  alert(message);
}

function handleAbout() {
  const message = `
🎨 MLP GUI Designer
Versiyon 1.0.0

Visual Studio tarzında profesyonel
GUI tasarım aracı - MLP Dili için

Özellikler:
  ✅ 9 widget türü
  ✅ Sürükle-bırak interface
  ✅ Canlı özellik editörü
  ✅ Otomatik kod üretimi
  ✅ Proje yönetimi
  ✅ Dark theme

Teknoloji:
  • Electron 28+
  • Vanilla JavaScript
  • Pure CSS

Geliştirici: MLP Team
Lisans: MIT
Tarih: 22 Kasım 2024

MLP programlama dili hakkında:
  https://github.com/your-repo/MLP
  `;
  alert(message);
};
