import React from 'react';

function TopToolbar({ projectName }) {
  return (
    <div className="toolbar">
      <button className="toolbar-btn" title="Yeni Proje">
        <span>📄</span>
      </button>
      <button className="toolbar-btn" title="Aç">
        <span>📁</span>
      </button>
      <button className="toolbar-btn" title="Kaydet">
        <span>💾</span>
      </button>
      
      <div className="toolbar-separator"></div>
      
      <button className="toolbar-btn" title="Çalıştır">
        <span>▶️</span>
      </button>
      <button className="toolbar-btn" title="Dışa Aktar">
        <span>📤</span>
      </button>
      
      <div className="toolbar-separator"></div>
      
      <label htmlFor="language-selector" style={{ color: '#ccc', fontSize: '12px', marginRight: '5px' }}>
        Dil:
      </label>
      <select id="language-selector" className="toolbar-select" title="Kod Dili">
        <option value="tr-TR">Türkçe</option>
        <option value="en-US">English</option>
      </select>
      
      <label htmlFor="syntax-selector" style={{ color: '#ccc', fontSize: '12px', marginLeft: '15px', marginRight: '5px' }}>
        Syntax:
      </label>
      <select id="syntax-selector" className="toolbar-select" title="Syntax Stili">
        <option value="mlp-default">MLP Default</option>
        <option value="c-style">C-Style</option>
        <option value="python-style">Python-Style</option>
      </select>
    </div>
  );
}

export default TopToolbar;
