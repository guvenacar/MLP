# MLP Flow Editor

**High-Performance GUI Designer for MLP Language**

Built with React + React Flow + Electron for Figma-like smoothness.

---

## 🚀 Quick Start

```bash
# Install dependencies
npm install

# Development mode
npm run dev

# Build for production
npm run build

# Run Electron app
npm start
```

---

## 📋 Project Status

**Phase:** Setup & Planning
**Progress:** 5%
**See:** [TODO.md](./TODO.md) for detailed roadmap

---

## 🎯 Goals

- ✅ Figma-like performance (60 FPS)
- ✅ Smooth zoom/pan
- ✅ Handle 500+ widgets
- ✅ MLP code generation
- ✅ Multi-language support

---

## 📂 Structure

```
mlp_flow_editor/
├── src/
│   ├── main/          # Electron main process
│   ├── renderer/      # React app
│   │   ├── components/
│   │   ├── adapters/
│   │   └── utils/
│   └── styles/
├── public/
├── TODO.md           # Development roadmap
└── README.md         # This file
```

---

## 🏗️ Architecture

**Tech Stack:**
- React 18 + React Flow 11
- Electron 27
- Vite 5 (Build tool)

**Migration from:** `mlp_gui_designer/` (DOM-based)

---

## 📚 Documentation

- [TODO.md](./TODO.md) - Complete development plan
- [React Flow Docs](https://reactflow.dev/)
- [MLP Specs](../SPECS.md)
- [AI Rules](../AI_RULES.md)

---

## 🎨 Features

### Current (Old Designer)
- 9 widget types
- Drag & drop
- Properties panel
- Code generation

### New (React Flow)
- All above + performance boost
- Virtualization
- Smooth animations
- Mini-map
- Better zoom/pan

---

## 🐛 Issues?

Check [TODO.md](./TODO.md) "Known Issues" section.

---

**Created:** November 23, 2025
**License:** MIT
