import React from 'react';

function PropertiesPanel({ selectedNode, onUpdateNode }) {
  if (!selectedNode) {
    return (
      <div className="properties-container">
        <div className="no-selection">Bir widget seçin</div>
      </div>
    );
  }

  const handleChange = (field, value) => {
    onUpdateNode(selectedNode.id, { [field]: value });
  };

  return (
    <div className="properties-container">
      <div className="property-group">
        <div className="property-group-title">Genel</div>
        
        <div className="property-row">
          <label className="property-label">ID</label>
          <input
            type="text"
            className="property-input"
            value={selectedNode.id}
            disabled
          />
        </div>

        <div className="property-row">
          <label className="property-label">Tip</label>
          <input
            type="text"
            className="property-input"
            value={selectedNode.type}
            disabled
          />
        </div>

        <div className="property-row">
          <label className="property-label">Label</label>
          <input
            type="text"
            className="property-input"
            value={selectedNode.data.label || ''}
            onChange={(e) => handleChange('label', e.target.value)}
            placeholder="Widget Label"
          />
        </div>
      </div>

      <div className="property-group">
        <div className="property-group-title">Boyut & Konum</div>
        
        <div className="property-row">
          <label className="property-label">X</label>
          <input
            type="number"
            className="property-input"
            value={Math.round(selectedNode.position?.x || 0)}
            onChange={(e) => {
              const newNodes = selectedNode;
              newNodes.position = { ...newNodes.position, x: parseInt(e.target.value) };
              onUpdateNode(selectedNode.id, {});
            }}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Y</label>
          <input
            type="number"
            className="property-input"
            value={Math.round(selectedNode.position?.y || 0)}
            onChange={(e) => {
              const newNodes = selectedNode;
              newNodes.position = { ...newNodes.position, y: parseInt(e.target.value) };
              onUpdateNode(selectedNode.id, {});
            }}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Width</label>
          <input
            type="number"
            className="property-input"
            value={selectedNode.data.width || 120}
            onChange={(e) => handleChange('width', parseInt(e.target.value))}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Height</label>
          <input
            type="number"
            className="property-input"
            value={selectedNode.data.height || 40}
            onChange={(e) => handleChange('height', parseInt(e.target.value))}
          />
        </div>
      </div>

      <div className="property-group">
        <div className="property-group-title">Stil</div>
        
        <div className="property-row">
          <label className="property-label">Background Color</label>
          <input
            type="color"
            className="property-input"
            value={selectedNode.data.style?.backgroundColor || '#3c3c3c'}
            onChange={(e) => handleChange('style', { 
              ...selectedNode.data.style, 
              backgroundColor: e.target.value 
            })}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Text Color</label>
          <input
            type="color"
            className="property-input"
            value={selectedNode.data.style?.color || '#cccccc'}
            onChange={(e) => handleChange('style', { 
              ...selectedNode.data.style, 
              color: e.target.value 
            })}
          />
        </div>
      </div>
    </div>
  );
}

export default PropertiesPanel;
