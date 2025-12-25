import React, { useState, useCallback, useRef, useEffect, useMemo } from 'react';
import KonvaCanvas from './components/KonvaCanvas';
import Toolbar from './components/Toolbar';
import PropertiesPanel from './components/PropertiesPanel';
import MenuBar from './components/MenuBar';
import TopToolbar from './components/TopToolbar';

function App() {
  const [widgets, setWidgets] = useState([]);
  const [selectedWidget, setSelectedWidget] = useState(null);
  const [showToolbox, setShowToolbox] = useState(true);
  const [showProperties, setShowProperties] = useState(true);
  const [projectName, setProjectName] = useState('Yeni Proje');
  const [currentView, setCurrentView] = useState('design'); // 'design' veya 'code'
  const [generatedCode, setGeneratedCode] = useState('');
  const [theme, setTheme] = useState('dark');
  const canvasRef = useRef(null);

  // Persist tema seçimi (shared styles body.light-theme kullanıyor)
  useEffect(() => {
    try {
      const savedTheme = localStorage.getItem('mlp-konva-theme');
      if (savedTheme === 'light') {
        setTheme('light');
      }
    } catch (error) {
      console.warn('Tema yüklenemedi', error);
    }
  }, []);

  useEffect(() => {
    if (theme === 'light') {
      document.body.classList.add('light-theme');
    } else {
      document.body.classList.remove('light-theme');
    }

    try {
      localStorage.setItem('mlp-konva-theme', theme);
    } catch (error) {
      console.warn('Tema kaydedilemedi', error);
    }
  }, [theme]);

  const onWidgetSelect = useCallback((widget) => {
    setSelectedWidget(widget);
  }, []);

  const onAddWidget = useCallback((widgetType, position) => {
    const defaultSizes = {
      button: { width: 120, height: 40 },
      label: { width: 100, height: 30 },
      textbox: { width: 200, height: 30 },
      checkbox: { width: 150, height: 25 },
      radio: { width: 150, height: 25 },
      panel: { width: 300, height: 200 },
      groupbox: { width: 300, height: 200 },
      listbox: { width: 200, height: 150 },
      combobox: { width: 200, height: 30 }
    };

    const size = defaultSizes[widgetType] || { width: 100, height: 30 };
    
    const newWidget = {
      id: `widget-${Date.now()}`,
      type: widgetType,
      x: position?.x || Math.random() * 400 + 50,
      y: position?.y || Math.random() * 300 + 50,
      width: size.width,
      height: size.height,
      label: `Yeni ${widgetType}`,
      backgroundColor: '#007bff',
      textColor: '#ffffff',
      fontSize: 14,
      fontFamily: 'Arial',
      visible: true
    };
    
    setWidgets((widgets) => [...widgets, newWidget]);
  }, []);

  const onUpdateWidget = useCallback((widgetId, updates) => {
    setWidgets((widgets) =>
      widgets.map((widget) =>
        widget.id === widgetId
          ? { ...widget, ...updates }
          : widget
      )
    );

    setSelectedWidget((current) =>
      current?.id === widgetId ? { ...current, ...updates } : current
    );
  }, []);

  const onDeleteWidget = useCallback((widgetId) => {
    setWidgets((widgets) => widgets.filter(w => w.id !== widgetId));
    if (selectedWidget?.id === widgetId) {
      setSelectedWidget(null);
    }
  }, [selectedWidget]);

  const editorTheme = useMemo(() => (theme === 'light' ? 'light' : 'dark'), [theme]);

  const handleToggleTheme = useCallback(() => {
    setTheme((current) => (current === 'dark' ? 'light' : 'dark'));
  }, []);

  const generateCode = useCallback((highlightWidgetId = null) => {
    let code = '';
    code += '-- MLP GUI Uygulamas\u0131\n';
    code += '-- Bu kod MLP GUI Designer ile otomatik olu\u015fturulmu\u015ftur\n\n';
    
    // Deği\u015fken tan\u0131mlamalar\u0131
    code += '-- Widget ID tan\u0131mlamalar\u0131\n';
    widgets.forEach((widget, index) => {
      code += `numeric ${widget.type}${index + 1}\n`;
    });
    code += '\n';
    
    // Ana program
    code += '-- GUI uygulamas\u0131 ba\u015flat\u0131l\u0131yor\n';
    code += 'YAZDIR("GUI uygulamas\u0131 ba\u015flat\u0131l\u0131yor...")\n\n';
    
    // Pencere olu\u015ftur
    code += '-- Pencere olu\u015ftur\n';
    code += 'GUI_PENCERE_AC(800, 600, "MLP GUI Uygulamas\u0131")\n\n';
    
    // Widget'lar\u0131 olu\u015ftur
    code += '-- Widget\'lar\u0131 olu\u015ftur\n';
    widgets.forEach((widget, index) => {
      const varName = `${widget.type}${index + 1}`;
      const isHighlighted = widget.id === highlightWidgetId;
      const prefix = isHighlighted ? '>>> ' : '';
      
      code += `${prefix}-- ${widget.label}\n`;
      
      switch (widget.type) {
        case 'button':
          code += `${prefix}${varName} = GUI_BUTTON_CREATE(1, "${widget.label}", ${Math.round(widget.x)}, ${Math.round(widget.y)}, ${Math.round(widget.width)}, ${Math.round(widget.height)})\n`;
          break;
        case 'label':
          code += `${prefix}${varName} = GUI_LABEL_CREATE(1, "${widget.label}", ${Math.round(widget.x)}, ${Math.round(widget.y)})\n`;
          break;
        case 'textbox':
          code += `${prefix}${varName} = GUI_TEXTBOX_CREATE(1, "${widget.label}", ${Math.round(widget.x)}, ${Math.round(widget.y)}, ${Math.round(widget.width)}, ${Math.round(widget.height)})\n`;
          break;
        case 'checkbox':
          code += `${prefix}${varName} = GUI_CHECKBOX_CREATE(1, "${widget.label}", ${Math.round(widget.x)}, ${Math.round(widget.y)})\n`;
          break;
        case 'radio':
          code += `${prefix}${varName} = GUI_RADIO_CREATE(1, "${widget.label}", "grup1", ${Math.round(widget.x)}, ${Math.round(widget.y)})\n`;
          break;
        case 'panel':
          code += `${prefix}${varName} = GUI_PANEL_CREATE(1, ${Math.round(widget.x)}, ${Math.round(widget.y)}, ${Math.round(widget.width)}, ${Math.round(widget.height)})\n`;
          break;
        case 'groupbox':
          code += `${prefix}${varName} = GUI_GROUPBOX_CREATE(1, "${widget.label}", ${Math.round(widget.x)}, ${Math.round(widget.y)}, ${Math.round(widget.width)}, ${Math.round(widget.height)})\n`;
          break;
        case 'listbox':
          code += `${prefix}${varName} = GUI_LISTBOX_CREATE(1, ${Math.round(widget.x)}, ${Math.round(widget.y)}, ${Math.round(widget.width)}, ${Math.round(widget.height)})\n`;
          break;
        case 'combobox':
          code += `${prefix}${varName} = GUI_COMBOBOX_CREATE(1, ${Math.round(widget.x)}, ${Math.round(widget.y)}, ${Math.round(widget.width)}, ${Math.round(widget.height)})\n`;
          break;
      }
      code += '\n';
    });
    
    // Event loop
    code += '-- Ana event loop\n';
    code += 'numeric running\n';
    code += 'running = 1\n\n';
    code += 'while running == 1\n';
    code += '    numeric event\n';
    code += '    event = GUI_POLL_EVENT()\n\n';
    code += '    if event == GUI_EVENT_QUIT\n';
    code += '        running = 0\n';
    code += '    end if\n\n';
    code += '    GUI_GUNCELLE()\n';
    code += 'end while\n\n';
    
    // Temizlik
    code += '-- Temizlik\n';
    code += 'GUI_KAPAT()\n';
    code += 'YAZDIR("GUI kapat\u0131ld\u0131")\n';
    
    return code;
  }, [widgets]);

  const handleViewChange = useCallback((view) => {
    setCurrentView(view);
    if (view === 'code') {
      setGeneratedCode(generateCode());
    }
  }, [generateCode]);

  const handleWidgetDoubleClick = useCallback((widget) => {
    // Widget'a çift tıklayınca kod görünümüne geç ve ilgili satırı highlight et
    setCurrentView('code');
    setGeneratedCode(generateCode(widget.id));
    setSelectedWidget(widget);
  }, [generateCode]);

  const handleNewProject = () => {
    if (widgets.length > 0 && !confirm('Mevcut proje kaybolacak. Devam edilsin mi?')) {
      return;
    }
    setWidgets([]);
    setSelectedWidget(null);
    setProjectName('Yeni Proje');
  };

  const handleSaveProject = () => {
    const project = {
      name: projectName,
      widgets: widgets,
      version: '1.0'
    };
    const json = JSON.stringify(project, null, 2);
    const blob = new Blob([json], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${projectName}.mlpgui`;
    a.click();
    URL.revokeObjectURL(url);
  };

  const handleExportCode = () => {
    const code = generateCode();
    const blob = new Blob([code], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${projectName}.mlp`;
    a.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div id="app" className="app">
      <div className="top-bar">
        <MenuBar 
          projectName={projectName}
          onToggleToolbox={() => setShowToolbox(!showToolbox)}
          onToggleProperties={() => setShowProperties(!showProperties)}
          onNewProject={handleNewProject}
          onSaveProject={handleSaveProject}
          onExportCode={handleExportCode}
          onToggleTheme={handleToggleTheme}
          currentTheme={theme}
        />
        <TopToolbar 
          projectName={projectName}
          onNewProject={handleNewProject}
          onSaveProject={handleSaveProject}
          onExportCode={handleExportCode}
        />
      </div>
      
      <div className="tabs">
        <button 
          className={`tab ${currentView === 'design' ? 'active' : ''}`}
          onClick={() => handleViewChange('design')}
        >
          Tasarım
        </button>
        <button 
          className={`tab ${currentView === 'code' ? 'active' : ''}`}
          onClick={() => handleViewChange('code')}
        >
          Kod
        </button>
      </div>
      
      <div className="main-content">
        {currentView === 'design' && showToolbox && (
          <div className="left-panel">
            <div className="panel-header">Widget Araçları</div>
            <Toolbar onAddWidget={onAddWidget} />
          </div>
        )}
        
        {currentView === 'design' && (
          <div className="center-panel" ref={canvasRef}>
            <KonvaCanvas
              widgets={widgets}
              selectedWidget={selectedWidget}
              onWidgetSelect={onWidgetSelect}
              onUpdateWidget={onUpdateWidget}
              onDeleteWidget={onDeleteWidget}
              onAddWidget={onAddWidget}
              onWidgetDoubleClick={handleWidgetDoubleClick}
              theme={editorTheme}
            />
          </div>
        )}
        
        {currentView === 'code' && (
          <div className="center-panel" style={{ padding: '20px', height: '100%' }}>
            <textarea
              value={generatedCode}
              onChange={(e) => setGeneratedCode(e.target.value)}
              style={{ 
                width: '100%',
                height: '100%',
                backgroundColor: '#1e1e1e', 
                color: '#d4d4d4', 
                padding: '20px', 
                borderRadius: '5px',
                fontFamily: 'Consolas, monospace',
                fontSize: '14px',
                lineHeight: '1.5',
                border: '1px solid #3e3e42',
                resize: 'none',
                outline: 'none'
              }}
            />
          </div>
        )}
        
        {currentView === 'design' && showProperties && (
          <div className="right-panel">
            <div className="panel-header">Özellikler</div>
            <PropertiesPanel 
              selectedWidget={selectedWidget}
              onUpdateWidget={onUpdateWidget}
            />
          </div>
        )}
      </div>
      
      <div className="status-bar">
        <span id="status-text">Hazır | Konva.js Canvas</span>
        <span id="widget-count">Widget Sayısı: {widgets.length}</span>
      </div>
    </div>
  );
}

export default App;
