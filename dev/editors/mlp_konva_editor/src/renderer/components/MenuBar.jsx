import React, { useState } from 'react';

function MenuBar({ projectName, onToggleToolbox, onToggleProperties, onNewProject, onSaveProject, onExportCode, onToggleTheme, currentTheme }) {
  const [openMenu, setOpenMenu] = useState(null);

  const toggleMenu = (menu) => {
    setOpenMenu(openMenu === menu ? null : menu);
  };

  const handleMenuClick = (action) => {
    action();
    setOpenMenu(null);
  };

  return (
    <div className="menu-bar">
      <div className="menu-item" onClick={() => toggleMenu('file')}>
        Dosya
        {openMenu === 'file' && (
          <div className="menu-dropdown">
            <div className="menu-option" onClick={() => handleMenuClick(onNewProject)}>
              Yeni Proje
            </div>
            <div className="menu-option" onClick={() => handleMenuClick(onSaveProject)}>
              Kaydet
            </div>
            <div className="menu-option" onClick={() => handleMenuClick(onExportCode)}>
              Kodu Dışa Aktar
            </div>
          </div>
        )}
      </div>
      
      <div className="menu-item" onClick={() => toggleMenu('view')}>
        Görünüm
        {openMenu === 'view' && (
          <div className="menu-dropdown">
            <div className="menu-option" onClick={() => handleMenuClick(onToggleTheme)}>
              {currentTheme === 'dark' ? 'Light Tema' : 'Dark Tema'}
            </div>
            <div className="menu-option" onClick={() => handleMenuClick(onToggleToolbox)}>
              Widget Araçları
            </div>
            <div className="menu-option" onClick={() => handleMenuClick(onToggleProperties)}>
              Özellikler Paneli
            </div>
          </div>
        )}
      </div>
      
      <div className="menu-item">Yardım</div>
    </div>
  );
}

export default MenuBar;
