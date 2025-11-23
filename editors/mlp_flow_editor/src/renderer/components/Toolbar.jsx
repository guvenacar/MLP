import React from 'react';

function Toolbar() {
  const widgetTypes = [
    { type: 'button', icon: '🔘', label: 'Button', category: 'Temel Bileşenler' },
    { type: 'label', icon: '🏷️', label: 'Label', category: 'Temel Bileşenler' },
    { type: 'textbox', icon: '📝', label: 'TextBox', category: 'Temel Bileşenler' },
    { type: 'checkbox', icon: '☑️', label: 'CheckBox', category: 'Temel Bileşenler' },
    { type: 'radio', icon: '🔘', label: 'RadioButton', category: 'Temel Bileşenler' },
    { type: 'panel', icon: '📦', label: 'Panel', category: 'Konteynerler' },
    { type: 'groupbox', icon: '🗂️', label: 'GroupBox', category: 'Konteynerler' },
    { type: 'listbox', icon: '📋', label: 'ListBox', category: 'Listeler' },
    { type: 'combobox', icon: '🔽', label: 'ComboBox', category: 'Listeler' },
  ];

  const categories = [...new Set(widgetTypes.map(w => w.category))];

  const onDragStart = (event, nodeType) => {
    event.dataTransfer.setData('application/reactflow', nodeType);
    event.dataTransfer.effectAllowed = 'move';
  };

  return (
    <div className="toolbox">
      {categories.map(category => (
        <div key={category} className="toolbox-category">
          <div className="category-title">{category}</div>
          {widgetTypes
            .filter(w => w.category === category)
            .map(widget => (
              <div
                key={widget.type}
                className="widget-item"
                draggable
                onDragStart={(e) => onDragStart(e, widget.type)}
              >
                <span className="widget-icon">{widget.icon}</span>
                <span className="widget-name">{widget.label}</span>
              </div>
            ))
          }
        </div>
      ))}
    </div>
  );
}

export default Toolbar;
