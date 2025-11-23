const { app, BrowserWindow } = require('electron');
const path = require('path');

console.log('[PixiDesigner] Electron main process starting. CWD:', process.cwd());
console.log('[PixiDesigner] __dirname:', __dirname);

function createWindow() {
  const win = new BrowserWindow({
    width: 1000,
    height: 700,
    webPreferences: {
      contextIsolation: true,
      nodeIntegration: false,
      preload: path.join(__dirname, 'preload.js')
    }
  });
  console.log('[PixiDesigner] Loading index.html from:', path.join(__dirname, 'index.html'));
  win.loadFile('index.html');
  win.setTitle('MLP Pixi Designer');
}

app.whenReady().then(() => {
  createWindow();
  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit();
});
