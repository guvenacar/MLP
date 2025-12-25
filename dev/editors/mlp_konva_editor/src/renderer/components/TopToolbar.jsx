import React from 'react';

function TopToolbar({ projectName, onNewProject, onSaveProject, onExportCode }) {
  return (
    <div className="toolbar">
      <div className="toolbar-section">
        <button className="toolbar-button" onClick={onNewProject} title="Yeni Proje">
          <span>📄</span>
        </button>
        <button className="toolbar-button" title="Proje Aç">
          <span>📂</span>
        </button>
        <button className="toolbar-button" onClick={onSaveProject} title="Kaydet">
          <span>💾</span>
        </button>
        <button className="toolbar-button" onClick={onExportCode} title="Kodu Dışa Aktar">
          <span>📥</span>
        </button>
      </div>
      
      <div className="toolbar-section">
        <button className="toolbar-button" title="Geri Al">
          <span>↶</span>
        </button>
        <button className="toolbar-button" title="İleri Al">
          <span>↷</span>
        </button>
      </div>
      
      <div className="project-name">{projectName}</div>
    </div>
  );
}

export default TopToolbar;
