// Widget Sınıfı ve Yönetimi

class Widget {
  constructor(type, properties = {}) {
    this.id = generateId();
    this.type = type;
    this.properties = { ...getDefaultProperties(type), ...properties };
    this.element = null;
  }

  // Widget'ı DOM'a render et
  render() {
    const div = document.createElement('div');
    div.id = this.id;
    div.className = `canvas-widget widget-${this.type}`;
    div.dataset.widgetId = this.id;
    div.dataset.widgetType = this.type;

    // Pozisyon ve boyut
    div.style.left = this.properties.x + 'px';
    div.style.top = this.properties.y + 'px';
    div.style.width = this.properties.width + 'px';
    div.style.height = this.properties.height + 'px';

    // Tip'e göre özel render
    this.renderWidgetContent(div);

    // Resize handle'ları ekle
    this.addResizeHandles(div);

    this.element = div;
    return div;
  }

  // Widget içeriğini render et
  renderWidgetContent(element) {
    switch (this.type) {
      case 'button':
        element.textContent = this.properties.text;
        element.style.backgroundColor = this.properties.backgroundColor;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        break;

      case 'label':
        element.textContent = this.properties.text;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        element.style.backgroundColor = this.properties.backgroundColor;
        break;

      case 'textbox':
        element.style.backgroundColor = this.properties.backgroundColor;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        element.textContent = this.properties.text || this.properties.placeholder;
        break;

      case 'checkbox':
        element.textContent = this.properties.text;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        if (this.properties.checked) {
          element.classList.add('checked');
        }
        break;

      case 'radio':
        element.textContent = this.properties.text;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        if (this.properties.checked) {
          element.classList.add('checked');
        }
        break;

      case 'panel':
        element.style.backgroundColor = this.properties.backgroundColor;
        element.style.borderColor = this.properties.borderColor;
        break;

      case 'groupbox':
        element.style.borderColor = this.properties.borderColor;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        element.dataset.title = this.properties.text;
        break;

      case 'listbox':
        element.style.backgroundColor = this.properties.backgroundColor;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        if (this.properties.items && this.properties.items.length > 0) {
          element.innerHTML = this.properties.items.map(item =>
            `<div style="padding: 4px;">${item}</div>`
          ).join('');
        } else {
          element.innerHTML = '<div style="padding: 4px; color: #666;">(Liste boş)</div>';
        }
        break;

      case 'combobox':
        element.style.backgroundColor = this.properties.backgroundColor;
        element.style.color = this.properties.textColor;
        element.style.fontSize = this.properties.fontSize + 'px';
        const selectedItem = this.properties.items && this.properties.selectedIndex >= 0
          ? this.properties.items[this.properties.selectedIndex]
          : '(Seçiniz)';
        element.textContent = selectedItem;
        break;
    }

    // Görünürlük
    if (!this.properties.visible) {
      element.style.opacity = '0.3';
    }
  }

  // Resize handle'ları ekle
  addResizeHandles(element) {
    const positions = ['nw', 'ne', 'sw', 'se'];
    positions.forEach(pos => {
      const handle = document.createElement('div');
      handle.className = `resize-handle ${pos}`;
      handle.dataset.position = pos;
      element.appendChild(handle);
    });
  }

  // Widget'ı güncelle
  update(newProperties) {
    this.properties = { ...this.properties, ...newProperties };
    if (this.element) {
      // Element'i yeniden render et
      const parent = this.element.parentNode;
      const wasSelected = this.element.classList.contains('selected');
      const newElement = this.render();
      if (wasSelected) {
        newElement.classList.add('selected');
      }
      if (parent) {
        parent.replaceChild(newElement, this.element);
      }
    }
  }

  // Widget verilerini JSON'a dönüştür
  toJSON() {
    return {
      id: this.id,
      type: this.type,
      properties: this.properties
    };
  }

  // JSON'dan widget oluştur
  static fromJSON(data) {
    const widget = new Widget(data.type, data.properties);
    widget.id = data.id;
    // ID counter'ı güncelle
    const idNum = parseInt(data.id.split('_')[1]);
    if (idNum >= widgetIdCounter) {
      widgetIdCounter = idNum + 1;
    }
    return widget;
  }
}
