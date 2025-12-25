// Ana Uygulama

// Global designer instance
let designer;

// Uygulama başlangıcı
document.addEventListener('DOMContentLoaded', async () => {
  designer = new Designer();

  // Config dosyalarını yükle
  showStatus('Dil ve syntax yapılandırmaları yükleniyor...');
  const configLoaded = await configLoader.loadConfigs();

  if (configLoaded) {
    populateLanguageSelector();
    populateSyntaxSelector();

    // MLP keyword'lerini IntelliSense'e yükle (setTimeout ile gecikme)
    setTimeout(() => {
      try {
        if (typeof intelliSense !== 'undefined' && intelliSense.loadKeywords) {
          const mlpKeywords = [
            'numeric', 'string', 'boolean', 'pointer',
            'function', 'end', 'return',
            'if', 'else', 'then', 'elif',
            'while', 'for', 'in', 'break', 'continue', 'stop',
            'struct', 'enum', 'const',
            'lambda', 'map', 'list', 'optional',
            'import', 'export', 'module',
            'true', 'false', 'null',
            'and', 'or', 'not',
            'print', 'typeof', 'range'
          ];
          intelliSense.loadKeywords(mlpKeywords);
          console.log('IntelliSense keywords loaded:', mlpKeywords.length);
        } else {
          console.error('IntelliSense object not available!');
        }
      } catch (error) {
        console.error('Error loading IntelliSense keywords:', error);
      }
    }, 500);
  }

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
  document.getElementById('menu-mlp-functions').addEventListener('click', handleMlpFunctions);
  document.getElementById('menu-documentation').addEventListener('click', handleDocumentation);
  document.getElementById('menu-mlp-docs').addEventListener('click', handleMlpDocs);
  document.getElementById('menu-about').addEventListener('click', handleAbout);

  // Toolbar butonları
  document.getElementById('btn-new').addEventListener('click', handleNewProject);
  document.getElementById('btn-open').addEventListener('click', handleOpenProject);
  document.getElementById('btn-save').addEventListener('click', handleSaveProject);
  document.getElementById('btn-export').addEventListener('click', handleExportCode);
  document.getElementById('btn-run').addEventListener('click', handleRun);

  // Dil ve Syntax seçiciler
  document.getElementById('language-selector').addEventListener('change', handleLanguageChange);
  document.getElementById('syntax-selector').addEventListener('change', handleSyntaxChange);

  // Code editor'da yazı yazılınca validation yap
  codeEditor.addEventListener('input', () => {
    // Satır numaralarını hemen güncelle
    updateLineNumbers();

    // Kısa bir gecikme ile validate et (her tuş vuruşunda değil)
    clearTimeout(window.validationTimeout);
    window.validationTimeout = setTimeout(() => {
      validateAndHighlightCode();
    }, 500); // 500ms bekle
  });

  // Code editor'da IntelliSense için klavye event'leri
  codeEditor.addEventListener('keydown', (e) => {
    // Ctrl+Space - IntelliSense'i manuel olarak aç
    if (e.ctrlKey && e.code === 'Space') {
      e.preventDefault();
      const pos = intelliSense.getCursorPosition(codeEditor);
      intelliSense.show(codeEditor, pos);
      return;
    }

    // IntelliSense açıksa
    if (intelliSense.isVisible) {
      switch (e.key) {
        case 'ArrowDown':
          e.preventDefault();
          intelliSense.selectNext();
          break;
        case 'ArrowUp':
          e.preventDefault();
          intelliSense.selectPrevious();
          break;
        case 'Enter':
        case 'Tab':
          e.preventDefault();
          intelliSense.insertSelected();
          break;
        case 'Escape':
          e.preventDefault();
          intelliSense.cancel();
          break;
      }
    }
  });

  // Code editor'da yazarken otomatik IntelliSense
  codeEditor.addEventListener('keyup', (e) => {
    // Özel tuşları atla
    if (['ArrowDown', 'ArrowUp', 'Enter', 'Tab', 'Escape', 'Shift', 'Control', 'Alt'].includes(e.key)) {
      return;
    }

    // Harf veya _ yazıldıysa IntelliSense'i göster
    if (e.key.match(/^[a-zA-Z_]$/)) {
      clearTimeout(window.intellisenseTimeout);
      window.intellisenseTimeout = setTimeout(() => {
        const pos = intelliSense.getCursorPosition(codeEditor);
        const line = intelliSense.getLineText(pos.line);
        const wordBefore = intelliSense.getWordBeforeCursor(line, pos.column);

        // En az 2 karakter yazılmışsa göster
        if (wordBefore.length >= 2) {
          intelliSense.show(codeEditor, pos);
        }
      }, 300);
    } else {
      // Boşluk, noktalama vs - IntelliSense'i kapat
      intelliSense.hide();
    }
  });

  // Editör dışına tıklanınca IntelliSense'i kapat
  document.addEventListener('click', (e) => {
    if (!intelliSense.popup.contains(e.target) && e.target !== codeEditor) {
      intelliSense.hide();
    }
  });
}

// Dil seçiciyi doldur
function populateLanguageSelector() {
  const selector = document.getElementById('language-selector');
  const languages = configLoader.getLanguages();

  // Clear existing options
  selector.innerHTML = '';

  languages.forEach(lang => {
    const option = document.createElement('option');
    option.value = lang.id;
    option.textContent = lang.name;
    if (lang.id === configLoader.currentLanguage) {
      option.selected = true;
    }
    selector.appendChild(option);
  });
}

// Syntax seçiciyi doldur
function populateSyntaxSelector() {
  const selector = document.getElementById('syntax-selector');
  const syntaxes = configLoader.getSyntaxes();

  // Clear existing options
  selector.innerHTML = '';

  Object.values(syntaxes).forEach(syntax => {
    const option = document.createElement('option');
    option.value = syntax.id;
    option.textContent = syntax.name;
    if (syntax.id === configLoader.currentSyntax) {
      option.selected = true;
    }
    selector.appendChild(option);
  });
}

// Dil değişti
function handleLanguageChange(e) {
  const langId = e.target.value;
  configLoader.setLanguage(langId);
  showStatus(`Dil değiştirildi: ${configLoader.getCurrentLanguage().name}`);

  // Kod görünümü açıksa güncelle
  if (!document.getElementById('view-code').classList.contains('hidden')) {
    updateCodeView();
  }
}

// Syntax değişti
function handleSyntaxChange(e) {
  const syntaxId = e.target.value;
  configLoader.setSyntax(syntaxId);
  showStatus(`Syntax değiştirildi: ${configLoader.getCurrentSyntax().name}`);

  // Kod görünümü açıksa güncelle
  if (!document.getElementById('view-code').classList.contains('hidden')) {
    updateCodeView();
  }
}

// Kod görünümünü güncelle
function updateCodeView() {
  const codeEditor = document.getElementById('code-editor');
  const code = designer.generateCode();
  codeEditor.textContent = code;

  // Satır numaralarını güncelle
  updateLineNumbers();

  // Syntax validation yap
  validateAndHighlightCode();
}

// Satır numaralarını güncelle
function updateLineNumbers() {
  const codeEditor = document.getElementById('code-editor');
  const lineNumbersDiv = document.getElementById('line-numbers');

  if (!lineNumbersDiv) return;

  const code = codeEditor.textContent;
  const lines = code.split('\n');

  // Satır numaralarını oluştur
  let lineNumbersHTML = '';
  for (let i = 1; i <= lines.length; i++) {
    lineNumbersHTML += `<div class="line-number">${i}</div>`;
  }

  lineNumbersDiv.innerHTML = lineNumbersHTML;

  // Hatalı satırları işaretle
  const errors = syntaxValidator.getAllErrors();
  errors.forEach(err => {
    const lineNum = err.line;
    const lineElement = lineNumbersDiv.children[lineNum - 1];
    if (lineElement && err.severity === 'error') {
      lineElement.classList.add('error-line');
    }
  });
}

// Kodu validate et ve hataları göster
function validateAndHighlightCode() {
  const codeEditor = document.getElementById('code-editor');
  const code = codeEditor.textContent;

  // Validate
  const errors = syntaxValidator.validate(code);

  // Error panel'i güncelle
  displayErrors(errors);

  // Hataları kod içinde highlight et
  if (errors.length > 0) {
    highlightErrors(code, errors);
  }

  // Satır numaralarını güncelle
  updateLineNumbers();
}

// Hataları error panel'de göster
function displayErrors(errors) {
  const viewCode = document.getElementById('view-code');
  let errorPanel = viewCode.querySelector('.error-panel');

  // Error panel yoksa oluştur
  if (!errorPanel) {
    errorPanel = document.createElement('div');
    errorPanel.className = 'error-panel';
    viewCode.appendChild(errorPanel);
  }

  // Temizle
  errorPanel.innerHTML = '';

  if (errors.length === 0) {
    errorPanel.style.display = 'none';
    return;
  }

  errorPanel.style.display = 'block';

  // Hataları listele
  errors.forEach(err => {
    const errorItem = document.createElement('div');
    errorItem.className = `error-item error-${err.severity}`;
    errorItem.innerHTML = `
      <span class="error-line">Satır ${err.line}:</span>
      <span class="error-message">${err.message}</span>
    `;

    // Tıklanınca o satıra git
    errorItem.addEventListener('click', () => {
      scrollToLine(err.line);
    });

    errorPanel.appendChild(errorItem);
  });
}

// Hataları kod içinde highlight et
function highlightErrors(code, errors) {
  const codeEditor = document.getElementById('code-editor');
  const lines = code.split('\n');

  // HTML ile satır satır render et
  let html = '';

  lines.forEach((line, index) => {
    const lineNum = index + 1;
    const lineErrors = errors.filter(err => err.line === lineNum);

    if (lineErrors.length > 0) {
      // Hatalı satır
      const severity = lineErrors[0].severity;
      html += `<div class="code-line error-line-${severity}" title="${lineErrors.map(e => e.message).join(', ')}">${escapeHtml(line)}</div>`;
    } else {
      html += `<div class="code-line">${escapeHtml(line)}</div>`;
    }
  });

  codeEditor.innerHTML = html;
}

// Belirli satıra scroll et
function scrollToLine(lineNum) {
  const codeEditor = document.getElementById('code-editor');
  const lines = codeEditor.querySelectorAll('.code-line');

  if (lines[lineNum - 1]) {
    lines[lineNum - 1].scrollIntoView({ behavior: 'smooth', block: 'center' });
    lines[lineNum - 1].classList.add('highlight-flash');

    setTimeout(() => {
      lines[lineNum - 1].classList.remove('highlight-flash');
    }, 2000);
  }
}

// HTML escape
function escapeHtml(text) {
  const div = document.createElement('div');
  div.textContent = text;
  return div.innerHTML;
}

// Fonksiyonu highlight et ve scroll et
function highlightFunction(functionName) {
  const codeEditor = document.getElementById('code-editor');
  const code = codeEditor.textContent;

  // Fonksiyon tanımını bul (örn: "func butona_tikla() -> numeric")
  const functionPattern = new RegExp(`func\\s+${functionName}\\s*\\(`, 'i');
  const match = code.match(functionPattern);

  if (!match) {
    console.warn(`Function "${functionName}" not found in code`);
    return;
  }

  // Fonksiyonun başladığı pozisyonu bul
  const functionStartIndex = match.index;

  // Fonksiyon satırını bul (önceki satır sayısını hesapla)
  const codeBeforeFunction = code.substring(0, functionStartIndex);
  const lineNumber = codeBeforeFunction.split('\n').length;

  // Kod editor'deki satırlar
  const lines = code.split('\n');

  // Highlight için geçici marker ekle
  const viewCode = document.getElementById('view-code');
  const codeContainer = viewCode.querySelector('.code-container') || viewCode;

  // Satır yüksekliğini hesapla (yaklaşık 20px per line)
  const lineHeight = 20;
  const scrollPosition = (lineNumber - 3) * lineHeight; // 3 satır padding

  // Scroll to position
  codeContainer.scrollTop = Math.max(0, scrollPosition);

  // Visual feedback: Highlight the function temporarily
  const originalHTML = codeEditor.innerHTML;
  const highlightedCode = code.replace(
    functionPattern,
    '<mark style="background-color: #4a90e2; color: white; padding: 2px 4px; border-radius: 3px;">$&</mark>'
  );

  codeEditor.innerHTML = highlightedCode;

  // Remove highlight after 2 seconds
  setTimeout(() => {
    codeEditor.textContent = code;
  }, 2000);

  showStatus(`Fonksiyon bulundu: ${functionName}() - Satır ${lineNumber}`);
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

async function handleMlpFunctions() {
  try {
    // mlp_functions.json'dan fonksiyonları yükle
    const response = await fetch('./mlp_functions.json');
    const data = await response.json();

    // Modal dialog oluştur
    const modal = document.createElement('div');
    modal.className = 'modal-overlay';
    modal.innerHTML = `
      <div class="modal-dialog" style="max-width: 900px; max-height: 80vh;">
        <div class="modal-header">
          <h2>🔧 MLP Fonksiyonları</h2>
          <button class="modal-close" onclick="this.closest('.modal-overlay').remove()">✖</button>
        </div>
        <div class="modal-body" style="overflow-y: auto; max-height: 60vh;">
          <div class="functions-search">
            <input type="text" id="function-search" placeholder="🔍 Fonksiyon ara..." style="width: 100%; padding: 8px; margin-bottom: 12px; font-size: 14px;">
          </div>
          <div id="functions-list"></div>
        </div>
      </div>
    `;
    document.body.appendChild(modal);

    // Fonksiyonları listele
    const listContainer = modal.querySelector('#functions-list');
    renderFunctions(data, listContainer);

    // Arama fonksiyonu
    const searchInput = modal.querySelector('#function-search');
    searchInput.addEventListener('input', (e) => {
      const query = e.target.value.toLowerCase();
      renderFunctions(data, listContainer, query);
    });

    // Modal dışına tıklanınca kapat
    modal.addEventListener('click', (e) => {
      if (e.target === modal) {
        modal.remove();
      }
    });

  } catch (error) {
    console.error('MLP Functions loading error:', error);
    alert('MLP fonksiyonları yüklenemedi. Dosya: mlp_functions.json');
  }
}

function renderFunctions(data, container, searchQuery = '') {
  container.innerHTML = '';

  // Kategorilere göre grupla
  for (const [categoryName, categoryData] of Object.entries(data.categories)) {
    const functions = categoryData.functions.filter(fn => {
      if (!searchQuery) return true;
      return fn.name.toLowerCase().includes(searchQuery) ||
             fn.description.toLowerCase().includes(searchQuery);
    });

    if (functions.length === 0) continue;

    // Kategori başlığı
    const categoryDiv = document.createElement('div');
    categoryDiv.style.marginBottom = '24px';
    categoryDiv.innerHTML = `
      <h3 style="color: var(--accent-color); border-bottom: 2px solid var(--accent-color); padding-bottom: 6px; margin-bottom: 12px;">
        ${categoryData.icon} ${categoryData.name}
      </h3>
    `;

    // Fonksiyonlar
    functions.forEach(fn => {
      const fnDiv = document.createElement('div');
      fnDiv.style.marginBottom = '16px';
      fnDiv.style.padding = '12px';
      fnDiv.style.backgroundColor = 'var(--bg-secondary)';
      fnDiv.style.borderRadius = '6px';
      fnDiv.style.borderLeft = '4px solid var(--accent-color)';

      fnDiv.innerHTML = `
        <div style="font-family: 'Courier New', monospace; font-weight: bold; color: var(--accent-color); margin-bottom: 6px;">
          ${fn.signature}
        </div>
        <div style="color: var(--text-secondary); margin-bottom: 8px; font-size: 13px;">
          ${fn.description}
        </div>
        ${fn.params && fn.params.length > 0 ? `
          <div style="font-size: 12px; margin-top: 8px;">
            <strong>Parametreler:</strong>
            <ul style="margin: 4px 0 0 20px; padding: 0;">
              ${fn.params.map(p => `
                <li><code>${p.name}</code> (${p.type}): ${p.description}</li>
              `).join('')}
            </ul>
          </div>
        ` : ''}
        ${fn.returns ? `
          <div style="font-size: 12px; margin-top: 6px;">
            <strong>Döndürür:</strong> (${fn.returns.type}) ${fn.returns.description}
          </div>
        ` : ''}
        ${fn.example ? `
          <details style="margin-top: 8px; font-size: 12px;">
            <summary style="cursor: pointer; color: var(--accent-color);">📝 Örnek Kod</summary>
            <pre style="background: var(--bg-tertiary); padding: 8px; border-radius: 4px; margin-top: 6px; overflow-x: auto;"><code>${fn.example}</code></pre>
          </details>
        ` : ''}
      `;

      categoryDiv.appendChild(fnDiv);
    });

    container.appendChild(categoryDiv);
  }

  // Sabitler varsa ekle
  if (data.constants && data.constants.length > 0) {
    const constantsDiv = document.createElement('div');
    constantsDiv.style.marginBottom = '24px';
    constantsDiv.innerHTML = `
      <h3 style="color: var(--accent-color); border-bottom: 2px solid var(--accent-color); padding-bottom: 6px; margin-bottom: 12px;">
        📌 Sabitler
      </h3>
    `;

    data.constants.forEach(c => {
      const constDiv = document.createElement('div');
      constDiv.style.marginBottom = '8px';
      constDiv.style.padding = '8px';
      constDiv.style.backgroundColor = 'var(--bg-secondary)';
      constDiv.style.borderRadius = '4px';

      constDiv.innerHTML = `
        <code style="color: var(--accent-color);">${c.name} = ${c.value}</code>
        <span style="color: var(--text-secondary); margin-left: 12px; font-size: 12px;">- ${c.description}</span>
      `;

      constantsDiv.appendChild(constDiv);
    });

    container.appendChild(constantsDiv);
  }

  if (container.children.length === 0) {
    container.innerHTML = '<p style="text-align: center; color: var(--text-tertiary); padding: 20px;">Sonuç bulunamadı.</p>';
  }
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
Versiyon 1.0.3 ⚡ GPU-Accelerated

Visual Studio tarzında profesyonel
GUI tasarım aracı - MLP Dili için

Özellikler:
  ✅ 9 widget türü
  ✅ Sürükle-bırak interface (Figma-level smooth!)
  ✅ Canlı özellik editörü
  ✅ Otomatik kod üretimi
  ✅ Proje yönetimi
  ✅ Light/Dark theme
  ✅ GPU-accelerated drag (60 FPS)

Teknoloji:
  • Electron 28+
  • Vanilla JavaScript
  • Pure CSS
  • GPU Compositing (transform3d)

Performans:
  • 60 FPS drag & drop
  • <16ms input lag
  • Layout Reflow eliminated

Geliştirici: MLP Team
Lisans: MIT
Tarih: 23 Kasım 2024

MLP programlama dili hakkında:
  https://github.com/your-repo/MLP
  `;
  alert(message);
};
