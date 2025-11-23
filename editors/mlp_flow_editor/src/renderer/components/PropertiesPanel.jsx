import React from 'react';

function PropertiesPanel({ selectedWidget, onUpdateWidget }) {
  if (!selectedWidget) {
    return (
      <div className="properties-container">
        <div className="no-selection">Bir widget seçin</div>
      </div>
    );
  }

  const handleChange = (field, value) => {
    onUpdateWidget(selectedWidget.id, { [field]: value });
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
            value={selectedWidget.id}
            disabled
          />
        </div>

        <div className="property-row">
          <label className="property-label">Tip</label>
          <input
            type="text"
            className="property-input"
            value={selectedWidget.type}
            disabled
          />
        </div>

        <div className="property-row">
          <label className="property-label">Label</label>
          <input
            type="text"
            className="property-input"
            value={selectedWidget.label || ''}
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
            value={Math.round(selectedWidget.x || 0)}
            onChange={(e) => handleChange('x', parseInt(e.target.value))}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Y</label>
          <input
            type="number"
            className="property-input"
            value={Math.round(selectedWidget.y || 0)}
            onChange={(e) => handleChange('y', parseInt(e.target.value))}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Width</label>
          <input
            type="number"
            className="property-input"
            value={selectedWidget.width || 120}
            onChange={(e) => handleChange('width', parseInt(e.target.value))}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Height</label>
          <input
            type="number"
            className="property-input"
            value={selectedWidget.height || 40}
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
            value={selectedWidget.backgroundColor || '#007bff'}
            onChange={(e) => handleChange('backgroundColor', e.target.value)}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Text Color</label>
          <input
            type="color"
            className="property-input"
            value={selectedWidget.textColor || '#ffffff'}
            onChange={(e) => handleChange('textColor', e.target.value)}
          />
        </div>

        <div className="property-row">
          <label className="property-label">Font Size</label>
          <input
            type="number"
            className="property-input"
            value={selectedWidget.fontSize || 14}
            onChange={(e) => handleChange('fontSize', parseInt(e.target.value))}
            min="8"
            max="72"
          />
        </div>

        <div className="property-row">
          <label className="property-label">Font Family</label>
          <select
            className="property-input"
            value={selectedWidget.fontFamily || 'Arial'}
            onChange={(e) => handleChange('fontFamily', e.target.value)}
          >
            <option value="Arial">Arial</option>
            <option value="Helvetica">Helvetica</option>
            <option value="Times New Roman">Times New Roman</option>
            <option value="Courier New">Courier New</option>
            <option value="Verdana">Verdana</option>
            <option value="Georgia">Georgia</option>
          </select>
        </div>
      </div>
    </div>
  );
}

export default PropertiesPanel;
