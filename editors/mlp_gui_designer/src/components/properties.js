// Özellikler Paneli Yönetimi

class PropertiesPanel {
  constructor(container, onPropertyChange) {
    this.container = container;
    this.onPropertyChange = onPropertyChange;
    this.currentWidget = null;
  }

  // Widget seçildiğinde özellikleri göster
  showProperties(widget) {
    this.currentWidget = widget;
    clearElement(this.container);

    if (!widget) {
      const noSelection = document.createElement('div');
      noSelection.className = 'no-selection';
      noSelection.textContent = 'Bir widget seçin';
      this.container.appendChild(noSelection);
      return;
    }

    // Widget bilgi başlığı
    const header = document.createElement('div');
    header.style.cssText = 'padding: 8px; background-color: #2d2d30; margin-bottom: 12px; border-radius: 3px;';
    header.innerHTML = `
      <strong style="color: #cccccc;">${getWidgetTypeName(widget.type)}</strong>
      <div style="font-size: 11px; color: #888; margin-top: 4px;">ID: ${widget.id}</div>
    `;
    this.container.appendChild(header);

    // Özellikleri grupla
    this.renderPropertyGroup('Pozisyon ve Boyut', [
      { key: 'x', label: 'X', type: 'number' },
      { key: 'y', label: 'Y', type: 'number' },
      { key: 'width', label: 'Genişlik', type: 'number' },
      { key: 'height', label: 'Yükseklik', type: 'number' }
    ], widget);

    // Görünüm özellikleri
    const appearanceProps = [];
    if (widget.properties.hasOwnProperty('text')) {
      appearanceProps.push({ key: 'text', label: 'Metin', type: 'text' });
    }
    if (widget.properties.hasOwnProperty('placeholder')) {
      appearanceProps.push({ key: 'placeholder', label: 'Placeholder', type: 'text' });
    }
    if (widget.properties.hasOwnProperty('backgroundColor')) {
      appearanceProps.push({ key: 'backgroundColor', label: 'Arka Plan Rengi', type: 'color' });
    }
    if (widget.properties.hasOwnProperty('textColor')) {
      appearanceProps.push({ key: 'textColor', label: 'Metin Rengi', type: 'color' });
    }
    if (widget.properties.hasOwnProperty('borderColor')) {
      appearanceProps.push({ key: 'borderColor', label: 'Kenar Rengi', type: 'color' });
    }
    if (widget.properties.hasOwnProperty('fontSize')) {
      appearanceProps.push({ key: 'fontSize', label: 'Font Boyutu', type: 'number' });
    }

    if (appearanceProps.length > 0) {
      this.renderPropertyGroup('Görünüm', appearanceProps, widget);
    }

    // Davranış özellikleri
    const behaviorProps = [];
    if (widget.properties.hasOwnProperty('enabled')) {
      behaviorProps.push({ key: 'enabled', label: 'Etkin', type: 'checkbox' });
    }
    if (widget.properties.hasOwnProperty('visible')) {
      behaviorProps.push({ key: 'visible', label: 'Görünür', type: 'checkbox' });
    }
    if (widget.properties.hasOwnProperty('checked')) {
      behaviorProps.push({ key: 'checked', label: 'İşaretli', type: 'checkbox' });
    }

    if (behaviorProps.length > 0) {
      this.renderPropertyGroup('Davranış', behaviorProps, widget);
    }

    // Özel özellikler
    if (widget.type === 'radio' && widget.properties.hasOwnProperty('group')) {
      this.renderPropertyGroup('Özel', [
        { key: 'group', label: 'Grup', type: 'text' }
      ], widget);
    }

    if ((widget.type === 'listbox' || widget.type === 'combobox') && widget.properties.hasOwnProperty('items')) {
      this.renderItemsEditor(widget);
    }

    // Event handler'lar
    this.renderEventHandlers(widget);
  }

  // Özellik grubunu render et
  renderPropertyGroup(title, properties, widget) {
    const group = document.createElement('div');
    group.className = 'property-group';

    const groupTitle = document.createElement('div');
    groupTitle.className = 'property-group-title';
    groupTitle.textContent = title;
    group.appendChild(groupTitle);

    properties.forEach(prop => {
      const row = document.createElement('div');
      row.className = 'property-row';

      const label = document.createElement('label');
      label.className = 'property-label';
      label.textContent = prop.label;
      row.appendChild(label);

      const input = this.createPropertyInput(prop, widget);
      row.appendChild(input);

      group.appendChild(row);
    });

    this.container.appendChild(group);
  }

  // Özellik input'u oluştur
  createPropertyInput(prop, widget) {
    let input;

    if (prop.type === 'checkbox') {
      input = document.createElement('input');
      input.type = 'checkbox';
      input.className = 'property-input';
      input.checked = widget.properties[prop.key];
    } else if (prop.type === 'color') {
      input = document.createElement('input');
      input.type = 'color';
      input.className = 'property-input';
      input.value = widget.properties[prop.key];
    } else if (prop.type === 'number') {
      input = document.createElement('input');
      input.type = 'number';
      input.className = 'property-input';
      input.value = widget.properties[prop.key];
    } else if (prop.type === 'select') {
      input = document.createElement('select');
      input.className = 'property-select';
      prop.options.forEach(opt => {
        const option = document.createElement('option');
        option.value = opt.value;
        option.textContent = opt.label;
        if (widget.properties[prop.key] === opt.value) {
          option.selected = true;
        }
        input.appendChild(option);
      });
    } else {
      input = document.createElement('input');
      input.type = 'text';
      input.className = 'property-input';
      input.value = widget.properties[prop.key] || '';
    }

    input.addEventListener('change', (e) => {
      let value = prop.type === 'checkbox' ? e.target.checked :
                  prop.type === 'number' ? parseFloat(e.target.value) || 0 :
                  e.target.value;

      if (this.onPropertyChange) {
        this.onPropertyChange(widget, prop.key, value);
      }
    });

    return input;
  }

  // Liste öğelerini düzenle
  renderItemsEditor(widget) {
    const group = document.createElement('div');
    group.className = 'property-group';

    const groupTitle = document.createElement('div');
    groupTitle.className = 'property-group-title';
    groupTitle.textContent = 'Liste Öğeleri';
    group.appendChild(groupTitle);

    const itemsText = document.createElement('textarea');
    itemsText.className = 'property-input';
    itemsText.style.cssText = 'width: 100%; min-height: 100px; resize: vertical; font-family: monospace;';
    itemsText.placeholder = 'Her satıra bir öğe yazın...';
    itemsText.value = (widget.properties.items || []).join('\n');

    itemsText.addEventListener('change', () => {
      const items = itemsText.value.split('\n').filter(line => line.trim());
      if (this.onPropertyChange) {
        this.onPropertyChange(widget, 'items', items);
      }
    });

    group.appendChild(itemsText);
    this.container.appendChild(group);
  }

  // Event handler'ları render et
  renderEventHandlers(widget) {
    const events = [];

    if (widget.properties.hasOwnProperty('onClick')) {
      events.push({ key: 'onClick', label: 'onClick' });
    }
    if (widget.properties.hasOwnProperty('onChange')) {
      events.push({ key: 'onChange', label: 'onChange' });
    }
    if (widget.properties.hasOwnProperty('onSelect')) {
      events.push({ key: 'onSelect', label: 'onSelect' });
    }

    if (events.length === 0) return;

    const group = document.createElement('div');
    group.className = 'property-group';

    const groupTitle = document.createElement('div');
    groupTitle.className = 'property-group-title';
    groupTitle.textContent = 'Olaylar';
    group.appendChild(groupTitle);

    events.forEach(event => {
      const row = document.createElement('div');
      row.className = 'property-row';

      const label = document.createElement('label');
      label.className = 'property-label';
      label.textContent = event.label;
      row.appendChild(label);

      const eventHandler = document.createElement('div');
      eventHandler.className = 'event-handler';

      const input = document.createElement('input');
      input.type = 'text';
      input.className = 'property-input';
      input.style.flex = '1';
      input.placeholder = 'Fonksiyon adı...';
      input.value = widget.properties[event.key] || '';

      input.addEventListener('change', (e) => {
        if (this.onPropertyChange) {
          this.onPropertyChange(widget, event.key, e.target.value);
        }
      });

      const editBtn = document.createElement('button');
      editBtn.textContent = '...';
      editBtn.title = 'Kod düzenle';
      editBtn.addEventListener('click', () => {
        // Kod sekmesine geç ve ilgili fonksiyonu göster
        const codeTab = document.getElementById('tab-code');
        if (codeTab) {
          codeTab.click();

          // Fonksiyon adını al
          const functionName = widget.properties[event.key];
          if (functionName) {
            // Kod editor'ü bul ve fonksiyona scroll et
            setTimeout(() => {
              highlightFunction(functionName);
            }, 100); // Tab değişimi animasyonundan sonra
          }
        }
      });

      eventHandler.appendChild(input);
      eventHandler.appendChild(editBtn);
      row.appendChild(eventHandler);

      group.appendChild(row);
    });

    this.container.appendChild(group);
  }

  // Özellikleri temizle
  clear() {
    this.currentWidget = null;
    clearElement(this.container);
    const noSelection = document.createElement('div');
    noSelection.className = 'no-selection';
    noSelection.textContent = 'Bir widget seçin';
    this.container.appendChild(noSelection);
  }
}
