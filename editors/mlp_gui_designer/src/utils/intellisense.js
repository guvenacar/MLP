// IntelliSense Autocomplete System

class IntelliSense {
  constructor() {
    this.popup = null;
    this.items = [];
    this.selectedIndex = 0;
    this.isVisible = false;
    this.currentEditor = null;
    this.triggerPosition = { line: 0, column: 0 };
    this.filterText = '';

    // MLP functions database
    this.functions = [];
    this.keywords = [];

    this.createPopup();
    this.loadFunctions();
  }

  // Popup HTML oluştur
  createPopup() {
    this.popup = document.createElement('div');
    this.popup.className = 'intellisense-popup';
    this.popup.style.display = 'none';
    document.body.appendChild(this.popup);
  }

  // MLP fonksiyonlarını yükle
  async loadFunctions() {
    try {
      const response = await fetch('./mlp_functions.json');
      const data = await response.json();

      // Fonksiyonları düzleştir
      this.functions = [];
      for (const category of Object.values(data.categories)) {
        this.functions.push(...category.functions);
      }

      // Sabitleri ekle
      if (data.constants) {
        this.functions.push(...data.constants.map(c => ({
          name: c.name,
          signature: `${c.name} = ${c.value}`,
          description: c.description,
          category: 'constant'
        })));
      }

      console.log(`IntelliSense: ${this.functions.length} fonksiyon yüklendi`);
    } catch (error) {
      console.error('IntelliSense functions loading error:', error);
    }
  }

  // MLP keyword'lerini yükle
  loadKeywords(keywords) {
    this.keywords = keywords.map(kw => ({
      name: kw,
      signature: kw,
      description: 'MLP anahtar kelimesi',
      category: 'keyword'
    }));
  }

  // IntelliSense'i göster
  show(editor, position) {
    this.currentEditor = editor;
    this.triggerPosition = position;

    // Filtreleme için metin al
    const line = this.getLineText(position.line);
    this.filterText = this.getWordBeforeCursor(line, position.column);

    // Önerileri filtrele
    this.updateSuggestions();

    if (this.items.length === 0) {
      this.hide();
      return;
    }

    // Popup'ı konumlandır
    this.positionPopup(position);

    // Göster
    this.popup.style.display = 'block';
    this.isVisible = true;
    this.selectedIndex = 0;
    this.renderItems();
  }

  // Önerileri güncelle
  updateSuggestions() {
    const filter = this.filterText.toLowerCase();

    // Tüm öğeleri birleştir
    const allItems = [...this.functions, ...this.keywords];

    // Filtrele
    if (filter.length === 0) {
      this.items = allItems.slice(0, 20); // İlk 20'yi göster
    } else {
      this.items = allItems.filter(item =>
        item.name.toLowerCase().startsWith(filter)
      ).slice(0, 20);
    }

    // İsme göre sırala
    this.items.sort((a, b) => a.name.localeCompare(b.name));
  }

  // Popup konumlandır
  positionPopup(position) {
    // Editor'ün pozisyonunu al
    const editorRect = this.currentEditor.getBoundingClientRect();

    // Cursor pozisyonunu tahmin et (yaklaşık)
    const lineHeight = 18;
    const charWidth = 8;

    const left = editorRect.left + (position.column * charWidth);
    const top = editorRect.top + ((position.line + 1) * lineHeight);

    this.popup.style.left = `${left}px`;
    this.popup.style.top = `${top}px`;
  }

  // Öğeleri render et
  renderItems() {
    this.popup.innerHTML = '';

    this.items.forEach((item, index) => {
      const itemDiv = document.createElement('div');
      itemDiv.className = 'intellisense-item';
      if (index === this.selectedIndex) {
        itemDiv.classList.add('selected');
      }

      // Icon
      const icon = this.getIconForCategory(item.category);
      const iconSpan = document.createElement('span');
      iconSpan.className = 'intellisense-icon';
      iconSpan.textContent = icon;

      // Name
      const nameSpan = document.createElement('span');
      nameSpan.className = 'intellisense-name';
      nameSpan.textContent = item.name;

      // Type/Category
      const typeSpan = document.createElement('span');
      typeSpan.className = 'intellisense-type';
      typeSpan.textContent = item.category || 'function';

      itemDiv.appendChild(iconSpan);
      itemDiv.appendChild(nameSpan);
      itemDiv.appendChild(typeSpan);

      // Click event
      itemDiv.addEventListener('click', () => {
        this.selectItem(index);
      });

      // Hover event
      itemDiv.addEventListener('mouseenter', () => {
        this.selectedIndex = index;
        this.renderItems();
      });

      this.popup.appendChild(itemDiv);
    });
  }

  // Kategori icon'u
  getIconForCategory(category) {
    const icons = {
      'function': '𝑓',
      'keyword': '🔑',
      'constant': '📌',
      'variable': '📦',
      'reduce': '∑',
      'map': '→',
      'utility': '🔧'
    };
    return icons[category] || '📄';
  }

  // Öğe seç ve ekle
  selectItem(index) {
    if (index < 0 || index >= this.items.length) return;

    const item = this.items[index];
    this.insertCompletion(item);
    this.hide();
  }

  // Tamamlamayı ekle
  insertCompletion(item) {
    // Basit implementasyon - sadece fonksiyon adını ekle
    const completion = item.name;

    // Editor'e yazdırma işlemi
    // Not: Bu kısım editor implementasyonuna bağlı
    if (this.currentEditor && this.currentEditor.textContent !== undefined) {
      const lines = this.currentEditor.textContent.split('\n');
      const line = lines[this.triggerPosition.line] || '';

      // Kelimeyi değiştir
      const before = line.substring(0, this.triggerPosition.column - this.filterText.length);
      const after = line.substring(this.triggerPosition.column);

      lines[this.triggerPosition.line] = before + completion + after;
      this.currentEditor.textContent = lines.join('\n');

      // Event tetikle (kod güncelleme için)
      this.currentEditor.dispatchEvent(new Event('input', { bubbles: true }));
    }
  }

  // Gizle
  hide() {
    this.popup.style.display = 'none';
    this.isVisible = false;
    this.items = [];
    this.selectedIndex = 0;
  }

  // Yukarı ok
  selectPrevious() {
    if (this.selectedIndex > 0) {
      this.selectedIndex--;
      this.renderItems();
      this.scrollToSelected();
    }
  }

  // Aşağı ok
  selectNext() {
    if (this.selectedIndex < this.items.length - 1) {
      this.selectedIndex++;
      this.renderItems();
      this.scrollToSelected();
    }
  }

  // Seçili öğeye scroll
  scrollToSelected() {
    const items = this.popup.querySelectorAll('.intellisense-item');
    if (items[this.selectedIndex]) {
      items[this.selectedIndex].scrollIntoView({ block: 'nearest' });
    }
  }

  // Enter - seçili öğeyi ekle
  insertSelected() {
    if (this.items.length > 0) {
      this.selectItem(this.selectedIndex);
    }
  }

  // Escape - gizle
  cancel() {
    this.hide();
  }

  // Yardımcı: Satır metnini al
  getLineText(lineNum) {
    if (!this.currentEditor) return '';
    const lines = this.currentEditor.textContent.split('\n');
    return lines[lineNum] || '';
  }

  // Yardımcı: Cursor'dan önceki kelimeyi al
  getWordBeforeCursor(line, column) {
    const textBefore = line.substring(0, column);
    const match = textBefore.match(/[a-zA-Z_][a-zA-Z0-9_]*$/);
    return match ? match[0] : '';
  }

  // Yardımcı: Cursor pozisyonunu al (editor içinde)
  getCursorPosition(editor) {
    const selection = window.getSelection();
    if (!selection.rangeCount) return { line: 0, column: 0 };

    const range = selection.getRangeAt(0);
    const textBefore = editor.textContent.substring(0, range.startOffset);
    const lines = textBefore.split('\n');

    return {
      line: lines.length - 1,
      column: lines[lines.length - 1].length
    };
  }
}

// Global instance
const intelliSense = new IntelliSense();
