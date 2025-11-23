const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electron', {
  // İleride dosya işlemleri için IPC metodları eklenecek
  platform: process.platform,
  
  saveFile: (data) => ipcRenderer.invoke('save-file', data),
  openFile: () => ipcRenderer.invoke('open-file'),
  exportCode: (code) => ipcRenderer.invoke('export-code', code)
});
