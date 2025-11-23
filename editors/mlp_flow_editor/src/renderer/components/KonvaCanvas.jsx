import React, { useRef, useEffect, useState } from 'react';
import { Stage, Layer } from 'react-konva';
import KonvaWidget from './KonvaWidget';

const KonvaCanvas = ({ 
  widgets, 
  selectedWidget, 
  onWidgetSelect, 
  onUpdateWidget,
  onDeleteWidget 
}) => {
  const stageRef = useRef(null);
  const [stageSize, setStageSize] = useState({ width: 800, height: 600 });

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

  return (
    <Stage
      ref={stageRef}
      width={stageSize.width}
      height={stageSize.height}
      onClick={handleStageClick}
      onTap={handleStageClick}
      style={{ backgroundColor: '#1e1e1e' }}
    >
      <Layer>
        {/* Grid arka plan (opsiyonel) */}
        {/* Buraya grid çizimleri eklenebilir */}
        
        {/* Widget'ları render et */}
        {widgets.map((widget) => (
          <KonvaWidget
            key={widget.id}
            widget={widget}
            isSelected={selectedWidget?.id === widget.id}
            onSelect={() => onWidgetSelect(widget)}
            onUpdate={(updates) => onUpdateWidget(widget.id, updates)}
          />
        ))}
      </Layer>
    </Stage>
  );
};

export default KonvaCanvas;
