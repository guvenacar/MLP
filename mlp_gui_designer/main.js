const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const path = require('path');
const fs = require('fs');

let mainWindow;

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 1400,
    height: 900,
    minWidth: 1000,
    minHeight: 700,
    title: 'MLP GUI Designer',
    backgroundColor: '#1e1e1e',
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      nodeIntegration: false,
      contextIsolation: true,
      sandbox: false
    }
  });

  mainWindow.loadFile('index.html');

  // Geliştirme modunda DevTools'u aç
  if (process.argv.includes('--dev')) {
    mainWindow.webContents.openDevTools();
  }

  mainWindow.on('closed', () => {
    mainWindow = null;
  });
}

app.whenReady().then(() => {
  createWindow();

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });
});

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

// IPC Handlers

// Proje kaydetme
ipcMain.handle('save-project', async (event, projectData) => {
  try {
    const { filePath } = await dialog.showSaveDialog(mainWindow, {
      title: 'Projeyi Kaydet',
      defaultPath: 'untitled.mlpgui',
      filters: [
        { name: 'MLP GUI Project', extensions: ['mlpgui'] },
        { name: 'All Files', extensions: ['*'] }
      ]
    });

    if (filePath) {
      fs.writeFileSync(filePath, JSON.stringify(projectData, null, 2), 'utf-8');
      return { success: true, filePath };
    }
    return { success: false, cancelled: true };
  } catch (error) {
    return { success: false, error: error.message };
  }
});

// Proje açma
ipcMain.handle('open-project', async () => {
  try {
    const { filePaths } = await dialog.showOpenDialog(mainWindow, {
      title: 'Proje Aç',
      filters: [
        { name: 'MLP GUI Project', extensions: ['mlpgui'] },
        { name: 'All Files', extensions: ['*'] }
      ],
      properties: ['openFile']
    });

    if (filePaths && filePaths.length > 0) {
      const data = fs.readFileSync(filePaths[0], 'utf-8');
      return { success: true, data: JSON.parse(data), filePath: filePaths[0] };
    }
    return { success: false, cancelled: true };
  } catch (error) {
    return { success: false, error: error.message };
  }
});

// MLP kod dosyasını kaydetme
ipcMain.handle('export-mlp-code', async (event, mlpCode) => {
  try {
    const { filePath } = await dialog.showSaveDialog(mainWindow, {
      title: 'MLP Kodunu Dışa Aktar',
      defaultPath: 'gui_program.mlp',
      filters: [
        { name: 'MLP Source File', extensions: ['mlp'] },
        { name: 'All Files', extensions: ['*'] }
      ]
    });

    if (filePath) {
      fs.writeFileSync(filePath, mlpCode, 'utf-8');
      return { success: true, filePath };
    }
    return { success: false, cancelled: true };
  } catch (error) {
    return { success: false, error: error.message };
  }
});

// Yeni proje
ipcMain.handle('new-project', async () => {
  const { response } = await dialog.showMessageBox(mainWindow, {
    type: 'question',
    buttons: ['Evet', 'Hayır'],
    defaultId: 1,
    title: 'Yeni Proje',
    message: 'Kaydedilmemiş değişiklikler kaybolacak. Devam etmek istiyor musunuz?'
  });
  return { confirmed: response === 0 };
});
