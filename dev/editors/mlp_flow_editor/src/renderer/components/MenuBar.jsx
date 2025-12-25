import React, { useState, useEffect } from 'react';

function MenuBar({ projectName, onToggleToolbox, onToggleProperties }) {
  const [activeMenu, setActiveMenu] = useState(null);
  const [theme, setTheme] = useState('dark');

  useEffect(() => {
    document.documentElement.setAttribute('data-theme', theme);
  }, [theme]);

  const toggleTheme = () => {
    setTheme(theme === 'dark' ? 'light' : 'dark');
    setActiveMenu(null);
  };

  const menuItems = {
    file: {
      label: 'Dosya',
      items: [
        { id: 'new', label: 'Yeni Proje', shortcut: 'Ctrl+N' },
        { id: 'open', label: 'Proje Aç...', shortcut: 'Ctrl+O' },
        { id: 'save', label: 'Kaydet', shortcut: 'Ctrl+S' },
        { type: 'divider' },
        { id: 'export', label: 'MLP Kodu Dışa Aktar...', shortcut: 'Ctrl+E' },
      ]
    },
    edit: {
      label: 'Düzenle',
      items: [
        { id: 'undo', label: 'Geri Al', shortcut: 'Ctrl+Z' },
        { id: 'redo', label: 'Yinele', shortcut: 'Ctrl+Y' },
        { type: 'divider' },
        { id: 'cut', label: 'Kes', shortcut: 'Ctrl+X' },
        { id: 'copy', label: 'Kopyala', shortcut: 'Ctrl+C' },
        { id: 'paste', label: 'Yapıştır', shortcut: 'Ctrl+V' },
        { id: 'delete', label: 'Sil', shortcut: 'Delete' },
        { type: 'divider' },
        { id: 'select-all', label: 'Tümünü Seç', shortcut: 'Ctrl+A' },
      ]
    },
    view: {
      label: 'Görünüm',
      items: [
        { id: 'toggle-theme', label: `Tema: ${theme === 'dark' ? 'Dark' : 'Light'}`, action: toggleTheme },
        { type: 'divider' },
        { id: 'toggle-toolbox', label: 'Widget Araçlarını Gizle/Göster', action: onToggleToolbox },
        { id: 'toggle-properties', label: 'Özellikleri Gizle/Göster', action: onToggleProperties },
        { type: 'divider' },
        { id: 'zoom-in', label: 'Yakınlaştır', shortcut: '+' },
        { id: 'zoom-out', label: 'Uzaklaştır', shortcut: '-' },
        { id: 'zoom-reset', label: 'Zoom Sıfırla (100%)', shortcut: '0' },
        { type: 'divider' },
        { id: 'toggle-grid', label: 'Izgara Göster/Gizle' },
      ]
    },
    help: {
      label: 'Yardım',
      items: [
        { id: 'quickstart', label: 'Hızlı Başlangıç' },
        { id: 'keyboard-shortcuts', label: 'Klavye Kısayolları' },
        { id: 'mlp-functions', label: 'MLP Fonksiyonları' },
        { type: 'divider' },
        { id: 'documentation', label: 'Dokümantasyon' },
        { id: 'mlp-docs', label: 'MLP Dili Dokümantasyonu' },
        { type: 'divider' },
        { id: 'about', label: 'Hakkında' },
      ]
    }
  };

  const handleMenuClick = (menuKey) => {
    setActiveMenu(activeMenu === menuKey ? null : menuKey);
  };

  const handleItemClick = (item) => {
    if (item.action) {
      item.action();
    }
    setActiveMenu(null);
  };

  return (
    <div className="menu-bar">
      {Object.entries(menuItems).map(([key, menu]) => (
        <div key={key} className="menu-item">
          <span onClick={() => handleMenuClick(key)}>{menu.label}</span>
          {activeMenu === key && (
            <div className="menu-dropdown">
              {menu.items.map((item, idx) => 
                item.type === 'divider' ? (
                  <div key={idx} className="menu-divider"></div>
                ) : (
                  <div 
                    key={item.id} 
                    className="menu-option"
                    onClick={() => handleItemClick(item)}
                  >
                    {item.label}
                    {item.shortcut && <span className="menu-shortcut">{item.shortcut}</span>}
                  </div>
                )
              )}
            </div>
          )}
        </div>
      ))}
    </div>
  );
}

export default MenuBar;
