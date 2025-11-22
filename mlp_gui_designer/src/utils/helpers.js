// Yardımcı Fonksiyonlar

// Benzersiz ID oluştur
let widgetIdCounter = 1;
function generateId() {
  return `widget_${widgetIdCounter++}`;
}

// Widget türüne göre varsayılan özellikler
function getDefaultProperties(type) {
  const defaults = {
    button: {
      x: 100,
      y: 100,
      width: 120,
      height: 35,
      text: 'Button',
      backgroundColor: '#0e639c',
      textColor: '#ffffff',
      fontSize: 14,
      enabled: true,
      visible: true,
      onClick: ''
    },
    label: {
      x: 100,
      y: 100,
      width: 100,
      height: 25,
      text: 'Label',
      textColor: '#d4d4d4',
      fontSize: 14,
      visible: true,
      backgroundColor: 'transparent'
    },
    textbox: {
      x: 100,
      y: 100,
      width: 200,
      height: 30,
      text: '',
      placeholder: 'Metin girin...',
      backgroundColor: '#3c3c3c',
      textColor: '#d4d4d4',
      fontSize: 14,
      enabled: true,
      visible: true,
      onChange: ''
    },
    checkbox: {
      x: 100,
      y: 100,
      width: 150,
      height: 25,
      text: 'CheckBox',
      checked: false,
      textColor: '#d4d4d4',
      fontSize: 14,
      enabled: true,
      visible: true,
      onChange: ''
    },
    radio: {
      x: 100,
      y: 100,
      width: 150,
      height: 25,
      text: 'RadioButton',
      checked: false,
      group: 'group1',
      textColor: '#d4d4d4',
      fontSize: 14,
      enabled: true,
      visible: true,
      onChange: ''
    },
    panel: {
      x: 100,
      y: 100,
      width: 200,
      height: 150,
      backgroundColor: '#2d2d30',
      borderColor: '#3e3e42',
      visible: true
    },
    groupbox: {
      x: 100,
      y: 100,
      width: 200,
      height: 150,
      text: 'GroupBox',
      borderColor: '#3e3e42',
      textColor: '#d4d4d4',
      fontSize: 12,
      visible: true
    },
    listbox: {
      x: 100,
      y: 100,
      width: 200,
      height: 150,
      items: [],
      backgroundColor: '#3c3c3c',
      textColor: '#d4d4d4',
      fontSize: 14,
      enabled: true,
      visible: true,
      onSelect: ''
    },
    combobox: {
      x: 100,
      y: 100,
      width: 200,
      height: 30,
      items: [],
      selectedIndex: -1,
      backgroundColor: '#3c3c3c',
      textColor: '#d4d4d4',
      fontSize: 14,
      enabled: true,
      visible: true,
      onChange: ''
    }
  };

  return { ...defaults[type] };
}

// Widget türü adını Türkçe'ye çevir
function getWidgetTypeName(type) {
  const names = {
    button: 'Buton',
    label: 'Etiket',
    textbox: 'Metin Kutusu',
    checkbox: 'Onay Kutusu',
    radio: 'Radyo Düğmesi',
    panel: 'Panel',
    groupbox: 'Grup Kutusu',
    listbox: 'Liste Kutusu',
    combobox: 'Açılır Liste'
  };
  return names[type] || type;
}

// Renk formatını dönüştür
function rgbToHex(rgb) {
  if (rgb.startsWith('#')) return rgb;
  const match = rgb.match(/\d+/g);
  if (!match) return '#000000';
  return '#' + match.slice(0, 3).map(x => {
    const hex = parseInt(x).toString(16);
    return hex.length === 1 ? '0' + hex : hex;
  }).join('');
}

function hexToRgb(hex) {
  const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
}

// DOM elementini temizle
function clearElement(element) {
  while (element.firstChild) {
    element.removeChild(element.firstChild);
  }
}

// Durum mesajı göster
function showStatus(message, duration = 3000) {
  const statusText = document.getElementById('status-text');
  if (statusText) {
    statusText.textContent = message;
    if (duration > 0) {
      setTimeout(() => {
        statusText.textContent = 'Hazır';
      }, duration);
    }
  }
}

// Widget sayısını güncelle
function updateWidgetCount(count) {
  const widgetCount = document.getElementById('widget-count');
  if (widgetCount) {
    widgetCount.textContent = `Widget Sayısı: ${count}`;
  }
}
