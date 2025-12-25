// Ana Designer Sınıfı - Drag & Drop ve Widget Yönetimi

class Designer {
  constructor() {
    this.widgets = [];
    this.selectedWidget = null;
    this.canvas = document.getElementById('canvas');
    this.propertiesPanel = new PropertiesPanel(
      document.getElementById('properties'),
      this.onPropertyChange.bind(this)
    );
    this.codeGenerator = new CodeGenerator();
    this.isDragging = false;
    this.isResizing = false;
    this.dragStartX = 0;
    this.dragStartY = 0;
    this.resizeHandle = null;
    this.rafId = null; // RequestAnimationFrame ID

    // GPU-accelerated drag için geçici değerler
    this.currentMouseX = 0;
    this.currentMouseY = 0;
    this.initialWidgetX = 0;
    this.initialWidgetY = 0;

    this.initializeEventListeners();
  }

  // Event listener'ları başlat
  initializeEventListeners() {
    // Toolbox'tan sürükleme
    const toolboxItems = document.querySelectorAll('.widget-item');
    toolboxItems.forEach(item => {
      item.addEventListener('dragstart', this.onToolboxDragStart.bind(this));
    });

    // Canvas'a bırakma
    this.canvas.addEventListener('dragover', this.onCanvasDragOver.bind(this));
    this.canvas.addEventListener('drop', this.onCanvasDrop.bind(this));

    // Canvas'ta widget seçme ve sürükleme
    this.canvas.addEventListener('mousedown', this.onCanvasMouseDown.bind(this));
    document.addEventListener('mousemove', this.onDocumentMouseMove.bind(this));
    document.addEventListener('mouseup', this.onDocumentMouseUp.bind(this));

    // Widget'a çift tıklama (kod görünümüne geç)
    this.canvas.addEventListener('dblclick', this.onCanvasDoubleClick.bind(this));

    // Delete tuşu ile silme
    document.addEventListener('keydown', this.onKeyDown.bind(this));
  }

  // Toolbox'tan sürükleme başladı
  onToolboxDragStart(e) {
    const widgetType = e.target.dataset.widgetType;
    e.dataTransfer.setData('widgetType', widgetType);
    e.dataTransfer.effectAllowed = 'copy';
  }

  // Canvas üzerinde sürükleme
  onCanvasDragOver(e) {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'copy';
  }

  // Canvas'a widget bırakıldı
  onCanvasDrop(e) {
    e.preventDefault();
    const widgetType = e.dataTransfer.getData('widgetType');
    if (!widgetType) return;

    // Canvas'a göre pozisyon hesapla
    const rect = this.canvas.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    // Yeni widget oluştur
    const widget = new Widget(widgetType, { x, y });
    this.addWidget(widget);

    showStatus(`${getWidgetTypeName(widgetType)} eklendi`);
  }

  // Widget ekle
  addWidget(widget) {
    this.widgets.push(widget);
    const element = widget.render();
    this.canvas.appendChild(element);

    // Canvas info'yu gizle
    const canvasInfo = this.canvas.querySelector('.canvas-info');
    if (canvasInfo && this.widgets.length > 0) {
      canvasInfo.style.display = 'none';
    }

    this.selectWidget(widget);
    updateWidgetCount(this.widgets.length);

    // Undo stack'e kaydet
    if (typeof saveToUndoStack === 'function') {
      saveToUndoStack();
    }
  }

  // Canvas'ta mouse basıldı
  onCanvasMouseDown(e) {
    // Resize handle'a mı tıklandı?
    if (e.target.classList.contains('resize-handle')) {
      this.isResizing = true;
      this.resizeHandle = e.target.dataset.position;
      this.dragStartX = e.clientX;
      this.dragStartY = e.clientY;
      e.preventDefault();
      return;
    }

    // Widget'a mı tıklandı?
    const widgetElement = e.target.closest('.canvas-widget');
    if (widgetElement) {
      const widgetId = widgetElement.dataset.widgetId;
      const widget = this.widgets.find(w => w.id === widgetId);
      if (widget) {
        this.selectWidget(widget);
        this.isDragging = true;
        this.dragStartX = e.clientX - widget.properties.x;
        this.dragStartY = e.clientY - widget.properties.y;

        // GPU-accelerated drag başlangıç değerleri
        this.initialWidgetX = widget.properties.x;
        this.initialWidgetY = widget.properties.y;
        this.currentMouseX = e.clientX;
        this.currentMouseY = e.clientY;

        // will-change CSS hint ekle (GPU layer oluşturur)
        widget.element.style.willChange = 'transform';

        e.preventDefault();
      }
    } else {
      // Boş alana tıklandı, seçimi kaldır
      this.selectWidget(null);
    }
  }

  // Mouse hareket etti
  onDocumentMouseMove(e) {
    if (!this.isDragging && !this.isResizing) return;

    // Mouse pozisyonunu güncelle (RAF dışında, lightweight)
    this.currentMouseX = e.clientX;
    this.currentMouseY = e.clientY;

    // requestAnimationFrame ile throttle (60 FPS'e sınırla)
    if (this.rafId) return;

    this.rafId = requestAnimationFrame(() => {
      this.rafId = null;

      if (this.isDragging && this.selectedWidget) {
        // ⚡ GPU-ACCELERATED DRAG: transform3d kullan (top/left yerine)
        const rect = this.canvas.getBoundingClientRect();
        let targetX = this.currentMouseX - this.dragStartX;
        let targetY = this.currentMouseY - this.dragStartY;

        // Canvas sınırları içinde tut
        targetX = Math.max(0, Math.min(targetX, rect.width - this.selectedWidget.properties.width));
        targetY = Math.max(0, Math.min(targetY, rect.height - this.selectedWidget.properties.height));

        // Transform ile GPU'da render (Layout Reflow YOK!)
        const deltaX = targetX - this.initialWidgetX;
        const deltaY = targetY - this.initialWidgetY;

        this.selectedWidget.element.style.transform = `translate3d(${deltaX}px, ${deltaY}px, 0)`;

        // Properties'i sadece memory'de güncelle (DOM güncelleme yok)
        this.selectedWidget.properties.x = Math.round(targetX);
        this.selectedWidget.properties.y = Math.round(targetY);

      } else if (this.isResizing && this.selectedWidget) {
        // Widget'ı yeniden boyutlandır
        const deltaX = this.currentMouseX - this.dragStartX;
        const deltaY = this.currentMouseY - this.dragStartY;

        const props = this.selectedWidget.properties;
        let newWidth = props.width;
        let newHeight = props.height;
        let newX = props.x;
        let newY = props.y;

        switch (this.resizeHandle) {
          case 'se': // Güneydoğu (sağ alt)
            newWidth = Math.max(50, props.width + deltaX);
            newHeight = Math.max(30, props.height + deltaY);
            break;
          case 'sw': // Güneybatı (sol alt)
            newWidth = Math.max(50, props.width - deltaX);
            newHeight = Math.max(30, props.height + deltaY);
            newX = props.x + (props.width - newWidth);
            break;
          case 'ne': // Kuzeydoğu (sağ üst)
            newWidth = Math.max(50, props.width + deltaX);
            newHeight = Math.max(30, props.height - deltaY);
            newY = props.y + (props.height - newHeight);
            break;
          case 'nw': // Kuzeybatı (sol üst)
            newWidth = Math.max(50, props.width - deltaX);
            newHeight = Math.max(30, props.height - deltaY);
            newX = props.x + (props.width - newWidth);
            newY = props.y + (props.height - newHeight);
            break;
        }

        this.selectedWidget.update({ x: newX, y: newY, width: newWidth, height: newHeight });
        this.dragStartX = this.currentMouseX;
        this.dragStartY = this.currentMouseY;
        this.propertiesPanel.showProperties(this.selectedWidget);
      }
    });
  }

  // Mouse bırakıldı
  onDocumentMouseUp(e) {
    // Drag bittiğinde transform'u temizle ve gerçek pozisyonu kaydet
    if (this.isDragging && this.selectedWidget) {
      // Transform'u kaldır
      this.selectedWidget.element.style.transform = '';
      this.selectedWidget.element.style.willChange = 'auto';

      // Gerçek pozisyonu top/left ile kaydet (tek sefer, drag bittiğinde)
      const finalX = this.selectedWidget.properties.x;
      const finalY = this.selectedWidget.properties.y;

      this.selectedWidget.element.style.left = finalX + 'px';
      this.selectedWidget.element.style.top = finalY + 'px';

      // Properties panel'i güncelle
      if (this.selectedWidget === this.selectedWidget) {
        this.propertiesPanel.showProperties(this.selectedWidget);
      }
    }

    this.isDragging = false;
    this.isResizing = false;
    this.resizeHandle = null;
  }

  // Çift tıklama (kod görünümüne geç)
  onCanvasDoubleClick(e) {
    const widgetElement = e.target.closest('.canvas-widget');
    if (widgetElement) {
      const codeTab = document.getElementById('tab-code');
      if (codeTab) {
        codeTab.click();
        showStatus('Kod görünümüne geçildi');
      }
    }
  }

  // Klavye olayları
  onKeyDown(e) {
    // Delete tuşu ile widget sil
    if (e.key === 'Delete' && this.selectedWidget) {
      this.deleteWidget(this.selectedWidget);
      e.preventDefault();
    }
  }

  // Widget seç
  selectWidget(widget) {
    // Önceki seçimi kaldır
    if (this.selectedWidget && this.selectedWidget.element) {
      this.selectedWidget.element.classList.remove('selected');
    }

    this.selectedWidget = widget;

    if (widget && widget.element) {
      widget.element.classList.add('selected');
      this.propertiesPanel.showProperties(widget);
    } else {
      this.propertiesPanel.clear();
    }
  }

  // Widget özelliği değişti
  onPropertyChange(widget, property, value) {
    this.updateWidgetProperty(widget, property, value);
  }

  // Widget özelliğini güncelle
  updateWidgetProperty(widget, property, value) {
    const updates = { [property]: value };
    widget.update(updates);

    // Properties panel'i güncelle (pozisyon değişirse sayılar güncellensin)
    if (this.selectedWidget === widget) {
      this.propertiesPanel.showProperties(widget);
    }
  }

  // Widget sil
  deleteWidget(widget) {
    const index = this.widgets.indexOf(widget);
    if (index > -1) {
      this.widgets.splice(index, 1);
      if (widget.element && widget.element.parentNode) {
        widget.element.parentNode.removeChild(widget.element);
      }
      if (this.selectedWidget === widget) {
        this.selectWidget(null);
      }
      updateWidgetCount(this.widgets.length);
      showStatus(`${getWidgetTypeName(widget.type)} silindi`);

      // Eğer widget kalmadıysa info'yu göster
      if (this.widgets.length === 0) {
        const canvasInfo = this.canvas.querySelector('.canvas-info');
        if (canvasInfo) {
          canvasInfo.style.display = 'block';
        }
      }
    }
  }

  // MLP kodu üret
  generateCode() {
    return this.codeGenerator.generate(this.widgets);
  }

  // Projeyi JSON'a dönüştür
  toJSON() {
    return {
      version: '1.0',
      widgets: this.widgets.map(w => w.toJSON())
    };
  }

  // JSON'dan proje yükle
  loadFromJSON(data) {
    // Mevcut widget'ları temizle
    this.clear();

    // Yeni widget'ları yükle
    if (data.widgets) {
      data.widgets.forEach(widgetData => {
        const widget = Widget.fromJSON(widgetData);
        this.addWidget(widget);
      });
    }

    showStatus('Proje yüklendi');
  }

  // Tümünü temizle
  clear() {
    this.widgets = [];
    this.selectedWidget = null;
    clearElement(this.canvas);

    // Canvas info'yu ekle
    const canvasInfo = document.createElement('div');
    canvasInfo.className = 'canvas-info';
    canvasInfo.textContent = "Widget'ları buraya sürükleyin";
    this.canvas.appendChild(canvasInfo);

    this.propertiesPanel.clear();
    updateWidgetCount(0);
    widgetIdCounter = 1;
  }
}
