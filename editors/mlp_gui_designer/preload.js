const { contextBridge, ipcRenderer } = require('electron');

// API'yi renderer process'e güvenli şekilde expose et
contextBridge.exposeInMainWorld('electronAPI', {
  // Proje yönetimi
  saveProject: (projectData) => ipcRenderer.invoke('save-project', projectData),
  openProject: () => ipcRenderer.invoke('open-project'),
  newProject: () => ipcRenderer.invoke('new-project'),

  // MLP kod dışa aktarma
  exportMlpCode: (mlpCode) => ipcRenderer.invoke('export-mlp-code', mlpCode)
});
