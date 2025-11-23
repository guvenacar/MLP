import React, { useRef, useEffect, useState } from 'react';
import { Stage, Layer } from 'react-konva';
import KonvaWidget from './KonvaWidget';

const KonvaCanvas = ({ 
  widgets, 
  selectedWidget, 
  onWidgetSelect, 
  onUpdateWidget,
  onDeleteWidget,
  onAddWidget,
  onWidgetDoubleClick,
  theme = 'dark'
}) => {
  const stageRef = useRef(null);
  const containerRef = useRef(null);
  const [stageSize, setStageSize] = useState({ width: 800, height: 600 });
  const isLightTheme = theme === 'light';
  const gridColor = isLightTheme ? 'rgba(0,0,0,0.05)' : 'rgba(255,255,255,0.05)';
  const canvasBackground = isLightTheme ? '#f3f3f3' : '#2d2d30';

  // Canvas boyutunu dinamik ayarla
  useEffect(() => {
    const updateSize = () => {
      const container = stageRef.current?.container();
      if (container) {
        const parent = container.parentElement;
        if (parent) {
          setStageSize({
            width: parent.offsetWidth,
            height: parent.offsetHeight
          });
        }
      }
    };

    updateSize();
    window.addEventListener('resize', updateSize);
    return () => window.removeEventListener('resize', updateSize);
  }, []);

  // Boş alana tıklama - seçimi kaldır
  const handleStageClick = (e) => {
    if (e.target === e.target.getStage()) {
      onWidgetSelect(null);
    }
  };

  // Klavye olayları (Delete tuşu)
  useEffect(() => {
    const handleKeyDown = (e) => {
      if (e.key === 'Delete' && selectedWidget) {
        onDeleteWidget(selectedWidget.id);
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [selectedWidget, onDeleteWidget]);

  // Drag-drop desteği
  const handleDrop = (e) => {
    e.preventDefault();
    const widgetType = e.dataTransfer.getData('widgetType');
    if (!widgetType) return;

    const container = containerRef.current;
    if (!container) return;

    const rect = container.getBoundingClientRect();
    const x = e.clientX - rect.left;
    const y = e.clientY - rect.top;

    onAddWidget(widgetType, { x, y });
  };

  const handleDragOver = (e) => {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'copy';
  };

  return (
    <div 
      ref={containerRef}
      onDrop={handleDrop}
      onDragOver={handleDragOver}
      className="konva-canvas-container"
      style={{ 
        width: '100%',
        height: '100%',
        position: 'relative',
        backgroundColor: canvasBackground,
        backgroundImage: `linear-gradient(${gridColor} 1px, transparent 1px), linear-gradient(90deg, ${gridColor} 1px, transparent 1px)` ,
        backgroundSize: '20px 20px'
      }}
    >
      <Stage
      ref={stageRef}
      width={stageSize.width}
      height={stageSize.height}
      onClick={handleStageClick}
      onTap={handleStageClick}
      style={{ backgroundColor: 'transparent' }}
    >
      <Layer>
        {/* Widget'ları render et */}
        {widgets.map((widget) => (
          <KonvaWidget
            key={widget.id}
            widget={widget}
            isSelected={selectedWidget?.id === widget.id}
            onSelect={() => onWidgetSelect(widget)}
            onUpdate={(updates) => onUpdateWidget(widget.id, updates)}
            onDoubleClick={onWidgetDoubleClick}
          />
        ))}
      </Layer>
    </Stage>
    </div>
  );
};

export default KonvaCanvas;
