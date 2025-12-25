import React, { useState, useCallback, useRef } from 'react';
import KonvaCanvas from './components/KonvaCanvas';
import Toolbar from './components/Toolbar';
import PropertiesPanel from './components/PropertiesPanel';
import MenuBar from './components/MenuBar';
import TopToolbar from './components/TopToolbar';
import './styles/main.css';

function App() {
  const [widgets, setWidgets] = useState([]);
  const [selectedWidget, setSelectedWidget] = useState(null);
  const [showToolbox, setShowToolbox] = useState(true);
  const [showProperties, setShowProperties] = useState(true);
  const [projectName, setProjectName] = useState('Yeni Proje');
  const canvasRef = useRef(null);

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
      x: position?.x || 100,
      y: position?.y || 100,
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
  }, []);

  const onDeleteWidget = useCallback((widgetId) => {
    setWidgets((widgets) => widgets.filter(w => w.id !== widgetId));
    if (selectedWidget?.id === widgetId) {
      setSelectedWidget(null);
    }
  }, [selectedWidget]);

  return (
    <div className="app">
      <div className="top-bar">
        <MenuBar 
          projectName={projectName}
          onToggleToolbox={() => setShowToolbox(!showToolbox)}
          onToggleProperties={() => setShowProperties(!showProperties)}
        />
        <TopToolbar projectName={projectName} />
      </div>
      
      <div className="main-content">
        {showToolbox && (
          <div className="left-panel">
            <div className="panel-header">Widget Araçları</div>
            <Toolbar onAddWidget={onAddWidget} />
          </div>
        )}
        
        <div className="center-panel" ref={canvasRef}>
          <KonvaCanvas
            widgets={widgets}
            selectedWidget={selectedWidget}
            onWidgetSelect={onWidgetSelect}
            onUpdateWidget={onUpdateWidget}
            onDeleteWidget={onDeleteWidget}
          />
        </div>
        
        {showProperties && (
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
