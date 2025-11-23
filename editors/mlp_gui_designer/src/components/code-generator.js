// MLP Kod Üreteci

class CodeGenerator {
  constructor() {
    this.language = 'tr-TR'; // Varsayılan dil
    this.syntax = 'mlp-default'; // Varsayılan syntax
  }

  // Widget'lardan MLP kodu üret
  generate(widgets, windowTitle = 'MLP GUI Uygulaması', windowWidth = 800, windowHeight = 600) {
    // Seçili dil ve syntax'ı al
    const currentLang = configLoader.getCurrentLanguage();
    const currentSyntax = configLoader.getCurrentSyntax();

    if (currentLang) {
      this.language = currentLang.id;
    }
    if (currentSyntax) {
      this.syntax = currentSyntax.id;
    }

    let code = `-- lang: ${this.language}\n`;
    code += `-- MLP GUI Uygulaması\n`;
    code += `-- Bu kod MLP GUI Designer ile otomatik oluşturulmuştur\n\n`;

    // Değişken tanımlamaları
    code += `-- Widget ID tanımlamaları\n`;
    widgets.forEach(widget => {
      code += `numeric ${widget.id}\n`;
    });
    code += '\n';

    // Event handler fonksiyonları
    const eventHandlers = this.collectEventHandlers(widgets);
    if (eventHandlers.length > 0) {
      code += `-- Event handler fonksiyonları\n`;
      eventHandlers.forEach(handler => {
        code += this.generateEventHandler(handler);
      });
      code += '\n';
    }

    // Ana program
    code += `-- Ana program başlangıcı\n`;
    code += `YAZDIR("GUI uygulaması başlatılıyor...")\n\n`;

    // Pencere oluştur
    code += `-- Pencere oluştur\n`;
    code += `GUI_PENCERE_AC(${windowWidth}, ${windowHeight}, "${windowTitle}")\n`;
    code += `YAZDIR("Pencere oluşturuldu: ${windowWidth}x${windowHeight}")\n\n`;

    // Widget'ları oluştur
    code += `-- Widget'ları oluştur\n`;
    widgets.forEach(widget => {
      code += this.generateWidgetCode(widget);
      code += '\n';
    });

    // Event loop
    const markers = this.getBlockMarkers();
    const syntax = configLoader.getCurrentSyntax();
    const useBraces = syntax?.block_style === 'braces';
    const useIndent = syntax?.block_style === 'indentation';

    code += `-- Ana event loop\n`;
    code += `numeric running\n`;
    code += `running = 1\n\n`;

    // While loop başlangıcı
    if (useBraces) {
      code += `while running == 1 ${markers.while_start}\n`;
    } else if (useIndent) {
      code += `while running == 1${markers.while_start}\n`;
    } else {
      code += `while running == 1\n`;
    }

    code += `    numeric event\n`;
    code += `    event = GUI_POLL_EVENT()\n\n`;

    // IF event == QUIT
    if (useBraces) {
      code += `    if event == GUI_EVENT_QUIT ${markers.if_start}\n`;
      code += `        running = 0\n`;
      code += `    ${markers.if_end}\n\n`;
    } else if (useIndent) {
      code += `    if event == GUI_EVENT_QUIT${markers.if_start}\n`;
      code += `        running = 0\n\n`;
    } else {
      code += `    if event == GUI_EVENT_QUIT\n`;
      code += `        running = 0\n`;
      code += `    end if\n\n`;
    }

    // Button click event'leri
    const buttonWidgets = widgets.filter(w => w.type === 'button' && w.properties.onClick);
    if (buttonWidgets.length > 0) {
      if (useBraces) {
        code += `    if event == GUI_EVENT_BUTTON_CLICK ${markers.if_start}\n`;
      } else if (useIndent) {
        code += `    if event == GUI_EVENT_BUTTON_CLICK${markers.if_start}\n`;
      } else {
        code += `    if event == GUI_EVENT_BUTTON_CLICK\n`;
      }

      code += `        numeric clicked_button\n`;
      code += `        clicked_button = GUI_GET_CLICKED_BUTTON()\n\n`;

      buttonWidgets.forEach((widget, index) => {
        const ifKeyword = index === 0 ? 'if' : 'else if';
        if (useBraces) {
          code += `        ${ifKeyword} clicked_button == ${widget.id} ${markers.if_start}\n`;
          code += `            ${widget.properties.onClick}()\n`;
          code += `        ${markers.if_end}\n`;
        } else if (useIndent) {
          code += `        ${ifKeyword} clicked_button == ${widget.id}${markers.if_start}\n`;
          code += `            ${widget.properties.onClick}()\n`;
        } else {
          code += `        ${ifKeyword} clicked_button == ${widget.id}\n`;
          code += `            ${widget.properties.onClick}()\n`;
        }
      });

      if (!useBraces && !useIndent) {
        code += `        end if\n`;
      }
      if (useBraces) {
        code += `    ${markers.if_end}\n\n`;
      } else {
        code += `\n`;
      }
    }

    code += `    GUI_GUNCELLE()\n`;

    // While loop kapanışı
    if (markers.while_end) {
      code += `${markers.while_end}\n\n`;
    } else {
      code += `\n`;
    }

    // Temizlik
    code += `-- Temizlik\n`;
    code += `GUI_KAPAT()\n`;
    code += `YAZDIR("GUI kapatıldı")\n`;

    return code;
  }

  // Event handler'ları topla
  collectEventHandlers(widgets) {
    const handlers = new Set();
    widgets.forEach(widget => {
      if (widget.properties.onClick) handlers.add(widget.properties.onClick);
      if (widget.properties.onChange) handlers.add(widget.properties.onChange);
      if (widget.properties.onSelect) handlers.add(widget.properties.onSelect);
    });
    return Array.from(handlers);
  }

  // Event handler fonksiyonu oluştur
  generateEventHandler(handlerName) {
    if (!handlerName) return '';

    const markers = this.getBlockMarkers();
    const syntax = configLoader.getCurrentSyntax();
    const useBraces = syntax?.block_style === 'braces';
    const useIndent = syntax?.block_style === 'indentation';

    let code = '';

    // Fonksiyon tanımı
    if (useBraces) {
      code += `func ${handlerName}() -> numeric ${markers.func_start}\n`;
    } else if (useIndent) {
      code += `func ${handlerName}() -> numeric${markers.func_start}\n`;
    } else {
      code += `func ${handlerName}() -> numeric\n`;
    }

    code += `    -- TODO: ${handlerName} fonksiyonu implementasyonu\n`;
    code += `    YAZDIR("${handlerName} çağrıldı")\n`;
    code += `    return 0\n`;

    // Kapanış
    if (markers.func_end) {
      code += `${markers.func_end}\n\n`;
    } else {
      code += `\n`;
    }

    return code;
  }

  // Tek bir widget için kod üret
  generateWidgetCode(widget) {
    const props = widget.properties;
    let code = `-- ${getWidgetTypeName(widget.type)}: ${widget.id}\n`;

    switch (widget.type) {
      case 'button':
        code += `${widget.id} = GUI_BUTTON_CREATE(1, "${props.text}", ${props.x}, ${props.y}, ${props.width}, ${props.height})\n`;
        if (props.onClick) {
          code += `-- onClick: ${props.onClick}()\n`;
        }
        break;

      case 'label':
        code += `${widget.id} = GUI_LABEL_CREATE(1, "${props.text}", ${props.x}, ${props.y})\n`;
        break;

      case 'textbox':
        code += `${widget.id} = GUI_TEXTBOX_CREATE(1, "${props.placeholder || ''}", ${props.x}, ${props.y}, ${props.width}, ${props.height})\n`;
        if (props.text) {
          code += `GUI_TEXTBOX_SET_TEXT(${widget.id}, "${props.text}")\n`;
        }
        break;

      case 'checkbox':
        code += `${widget.id} = GUI_CHECKBOX_CREATE(1, "${props.text}", ${props.x}, ${props.y})\n`;
        if (props.checked) {
          code += `GUI_CHECKBOX_SET_CHECKED(${widget.id}, 1)\n`;
        }
        break;

      case 'radio':
        code += `${widget.id} = GUI_RADIO_CREATE(1, "${props.text}", "${props.group}", ${props.x}, ${props.y})\n`;
        if (props.checked) {
          code += `GUI_RADIO_SET_CHECKED(${widget.id}, 1)\n`;
        }
        break;

      case 'panel':
        code += `${widget.id} = GUI_PANEL_CREATE(1, ${props.x}, ${props.y}, ${props.width}, ${props.height})\n`;
        break;

      case 'groupbox':
        code += `${widget.id} = GUI_GROUPBOX_CREATE(1, "${props.text}", ${props.x}, ${props.y}, ${props.width}, ${props.height})\n`;
        break;

      case 'listbox':
        code += `${widget.id} = GUI_LISTBOX_CREATE(1, ${props.x}, ${props.y}, ${props.width}, ${props.height})\n`;
        if (props.items && props.items.length > 0) {
          props.items.forEach(item => {
            code += `GUI_LISTBOX_ADD_ITEM(${widget.id}, "${item}")\n`;
          });
        }
        break;

      case 'combobox':
        code += `${widget.id} = GUI_COMBOBOX_CREATE(1, ${props.x}, ${props.y}, ${props.width}, ${props.height})\n`;
        if (props.items && props.items.length > 0) {
          props.items.forEach(item => {
            code += `GUI_COMBOBOX_ADD_ITEM(${widget.id}, "${item}")\n`;
          });
        }
        if (props.selectedIndex >= 0) {
          code += `GUI_COMBOBOX_SET_SELECTED(${widget.id}, ${props.selectedIndex})\n`;
        }
        break;
    }

    // Görünürlük
    if (!props.visible) {
      code += `GUI_WIDGET_SET_VISIBLE(${widget.id}, 0)\n`;
    }

    // Etkinlik
    if (props.hasOwnProperty('enabled') && !props.enabled) {
      code += `GUI_WIDGET_SET_ENABLED(${widget.id}, 0)\n`;
    }

    return code;
  }

  // Dil ayarla
  setLanguage(lang) {
    this.language = lang;
  }

  // Syntax'a göre block markers'ı al
  getBlockMarkers() {
    const syntax = configLoader.getCurrentSyntax();
    if (!syntax) {
      return {
        if_start: 'if',
        if_end: 'end if',
        while_start: 'while',
        while_end: 'end while',
        func_start: 'func',
        func_end: 'end func'
      };
    }

    const markers = syntax.markers || {};
    const blockStyle = syntax.block_style || 'keyword';

    // C-style için
    if (blockStyle === 'braces') {
      return {
        if_start: '{',
        if_end: '}',
        while_start: '{',
        while_end: '}',
        func_start: '{',
        func_end: '}'
      };
    }

    // Python-style için
    if (blockStyle === 'indentation') {
      return {
        if_start: ':',
        if_end: '',  // Dedent ile hallolur
        while_start: ':',
        while_end: '',
        func_start: ':',
        func_end: ''
      };
    }

    // MLP-default
    return {
      if_start: 'if',
      if_end: 'end if',
      while_start: 'while',
      while_end: 'end while',
      func_start: 'func',
      func_end: 'end func'
    };
  }

  // Dile göre keyword translate et
  translateKeyword(keyword) {
    return configLoader.translateKeyword(keyword) || keyword;
  }
}
