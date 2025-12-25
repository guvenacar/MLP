# MLP Flow Editor - Development TODO

**Project:** High-Performance GUI Designer for MLP Language
**Tech Stack:** React + React Flow + Electron
**Goal:** Figma-like smoothness and performance
**Started:** November 23, 2025

---

## 📋 Project Status

**Current Phase:** Phase 1 - Setup & Architecture
**Progress:** 0/100 (Just started)
**Next Milestone:** Basic React Flow canvas rendering

---

## 🎯 Development Phases

### ✅ Phase 0: Project Initialization (CURRENT)
- [x] Create project directory
- [x] Initialize npm project
- [x] Create TODO.md documentation
- [ ] Install dependencies
- [ ] Setup Vite + React
- [ ] Setup Electron
- [ ] Configure build system

**Timeline:** Day 1 (Today)

---

### 📦 Phase 1: Core Setup (Next)
**Goal:** Get React Flow running in Electron

#### 1.1 Dependencies Installation
- [ ] Install React 18.x
- [ ] Install React DOM 18.x
- [ ] Install React Flow 11.x
- [ ] Install Electron 27.x
- [ ] Install Vite 5.x
- [ ] Install dev dependencies

**Commands to run:**
```bash
npm install react react-dom reactflow
npm install electron --save-dev
npm install vite @vitejs/plugin-react --save-dev
```

#### 1.2 Project Structure
```
mlp_flow_editor/
├── src/
│   ├── main/
│   │   └── main.js              # Electron main process
│   ├── renderer/
│   │   ├── main.jsx             # React entry point
│   │   ├── App.jsx              # Main app component
│   │   ├── components/
│   │   │   ├── FlowCanvas.jsx   # React Flow wrapper
│   │   │   ├── Toolbar.jsx      # Widget toolbox
│   │   │   ├── PropertiesPanel.jsx
│   │   │   └── nodes/           # Custom node types
│   │   │       ├── ButtonNode.jsx
│   │   │       ├── LabelNode.jsx
│   │   │       └── ...
│   │   ├── adapters/
│   │   │   └── widgetAdapter.js # Widget data converter
│   │   └── styles/
│   │       └── main.css
├── public/
│   └── index.html
├── package.json
├── vite.config.js
├── electron-builder.yml
└── TODO.md (this file)
```

#### 1.3 Configuration Files
- [ ] Create vite.config.js
- [ ] Create electron main process
- [ ] Create preload.js
- [ ] Setup hot reload
- [ ] Configure build scripts

**Estimated Time:** 2-3 hours

---

### 🎨 Phase 2: React Flow Integration
**Goal:** Render basic canvas with zoom/pan

#### 2.1 Basic Canvas Setup
- [ ] Create FlowCanvas component
- [ ] Initialize React Flow
- [ ] Setup default zoom/pan controls
- [ ] Add mini-map (optional)
- [ ] Add background pattern

#### 2.2 Node System Foundation
- [ ] Create base node component
- [ ] Define node types (button, label, textbox, etc.)
- [ ] Implement node selection
- [ ] Add node deletion

#### 2.3 Performance Configuration
- [ ] Enable node virtualization
- [ ] Configure viewport settings
- [ ] Optimize re-render logic
- [ ] Setup RAF updates

**Estimated Time:** 4-5 hours

---

### 🔌 Phase 3: Widget Adapter Layer
**Goal:** Convert old widget format to React Flow nodes

#### 3.1 Adapter Implementation
**File:** `src/renderer/adapters/widgetAdapter.js`

```javascript
// Convert old widget to React Flow node
export function widgetToNode(widget) {
  return {
    id: widget.id,
    type: `${widget.type}Node`,
    position: { x: widget.properties.x, y: widget.properties.y },
    data: {
      label: widget.properties.text,
      width: widget.properties.width,
      height: widget.properties.height,
      style: { ...widget.properties },
      originalWidget: widget
    }
  };
}

// Convert React Flow node back to widget
export function nodeToWidget(node) {
  return {
    id: node.id,
    type: node.type.replace('Node', ''),
    properties: {
      x: node.position.x,
      y: node.position.y,
      ...node.data.style,
      text: node.data.label
    }
  };
}
```

#### 3.2 Migration Strategy
- [ ] Read existing widget.js structure
- [ ] Map all widget types to node types
- [ ] Preserve all properties
- [ ] Test backward compatibility

**Estimated Time:** 3-4 hours

---

### 🎭 Phase 4: Custom Node Components
**Goal:** Create MLP-specific node renderers

#### 4.1 Node Types to Implement
- [ ] ButtonNode (priority: HIGH)
- [ ] LabelNode (priority: HIGH)
- [ ] TextBoxNode (priority: HIGH)
- [ ] CheckBoxNode (priority: MEDIUM)
- [ ] RadioNode (priority: MEDIUM)
- [ ] PanelNode (priority: MEDIUM)
- [ ] GroupBoxNode (priority: LOW)
- [ ] ListBoxNode (priority: LOW)
- [ ] ComboBoxNode (priority: LOW)

#### 4.2 Node Features
- [ ] Resizable handles
- [ ] Selection highlight
- [ ] Double-click to edit
- [ ] Context menu
- [ ] Style customization

**Example:**
```jsx
// ButtonNode.jsx
import { memo } from 'react';
import { Handle, Position, NodeResizer } from 'reactflow';

export default memo(({ data, selected }) => {
  return (
    <>
      <NodeResizer 
        isVisible={selected} 
        minWidth={80}
        minHeight={30}
      />
      <div
        className="button-node"
        style={{
          width: data.width,
          height: data.height,
          backgroundColor: data.style.backgroundColor,
          color: data.style.textColor,
          fontSize: `${data.style.fontSize}px`
        }}
      >
        {data.label}
      </div>
    </>
  );
});
```

**Estimated Time:** 6-8 hours

---

### 🛠️ Phase 5: UI Components
**Goal:** Build toolbar, properties panel, code view

#### 5.1 Toolbar (Widget Palette)
- [ ] Create draggable widget items
- [ ] Group by category
- [ ] Search/filter widgets
- [ ] Drag-to-canvas functionality

#### 5.2 Properties Panel
- [ ] Bind to selected node
- [ ] Live property updates
- [ ] Input validation
- [ ] Color picker
- [ ] Font selector

#### 5.3 Code View
- [ ] MLP code generation
- [ ] Syntax highlighting
- [ ] Multi-language support (tr-TR, en-US)
- [ ] Export functionality

**Estimated Time:** 8-10 hours

---

### 🔄 Phase 6: Code Generation
**Goal:** Generate MLP code from visual design

#### 6.1 Code Generator
**File:** `src/renderer/utils/codeGenerator.js`

- [ ] Import from old code-generator.js
- [ ] Adapt to React Flow nodes
- [ ] Maintain language support
- [ ] Support syntax styles (default, C-style, Python-style)

#### 6.2 Language Support
- [ ] Turkish (tr-TR) keywords
- [ ] English (en-US) keywords
- [ ] Syntax variants

**Estimated Time:** 4-5 hours

---

### 💾 Phase 7: Project Management
**Goal:** Save/load projects

#### 7.1 File Operations
- [ ] Save project (.mlpgui format)
- [ ] Load project
- [ ] Auto-save
- [ ] Recent files list

#### 7.2 Export Options
- [ ] Export to .mlp code
- [ ] Export to JSON
- [ ] Import from old designer

**Estimated Time:** 3-4 hours

---

### ⚡ Phase 8: Performance Optimization
**Goal:** Achieve 60 FPS with 500+ widgets

#### 8.1 React Optimization
- [ ] Implement memo() for all nodes
- [ ] Use useCallback for event handlers
- [ ] Optimize re-render logic
- [ ] Profile with React DevTools

#### 8.2 React Flow Optimization
- [ ] Enable fitViewOnInit
- [ ] Configure nodesDraggable wisely
- [ ] Use onlyRenderVisibleElements
- [ ] Optimize edge calculations

#### 8.3 Benchmarking
- [ ] FPS measurement tool
- [ ] Memory profiling
- [ ] Load time testing
- [ ] Stress test (1000+ nodes)

**Target Metrics:**
- 60 FPS with 500 nodes
- < 2s initial load
- < 100ms interaction response
- < 200MB memory usage

**Estimated Time:** 4-6 hours

---

### 🧪 Phase 9: Testing & QA
**Goal:** Ensure stability and feature parity

#### 9.1 Unit Tests
- [ ] Adapter functions
- [ ] Node components
- [ ] Code generation

#### 9.2 Integration Tests
- [ ] Drag & drop flow
- [ ] Property editing
- [ ] Save/load cycle

#### 9.3 User Acceptance
- [ ] Feature parity with old designer
- [ ] Performance benchmarks
- [ ] Bug fixing

**Estimated Time:** 6-8 hours

---

### 📦 Phase 10: Build & Release
**Goal:** Package and distribute

#### 10.1 Build Configuration
- [ ] Electron builder config
- [ ] Code signing (optional)
- [ ] Auto-update setup

#### 10.2 Distribution
- [ ] Windows build
- [ ] Linux build
- [ ] macOS build (optional)

**Estimated Time:** 2-3 hours

---

## 📊 Progress Tracking

### Overall Progress
- **Phase 0:** 30% (In Progress)
- **Phase 1:** 0%
- **Phase 2:** 0%
- **Phase 3:** 0%
- **Phase 4:** 0%
- **Phase 5:** 0%
- **Phase 6:** 0%
- **Phase 7:** 0%
- **Phase 8:** 0%
- **Phase 9:** 0%
- **Phase 10:** 0%

**Total:** 3/100 phases complete

---

## 🎯 Success Criteria

### Must-Have Features
- [x] Project initialization
- [ ] React Flow canvas working
- [ ] All 9 widget types rendered
- [ ] Drag & drop from toolbar
- [ ] Properties panel functional
- [ ] MLP code generation
- [ ] Save/load projects
- [ ] 60 FPS performance

### Nice-to-Have
- [ ] Mini-map navigation
- [ ] Undo/redo stack
- [ ] Keyboard shortcuts
- [ ] Grid snapping
- [ ] Alignment guides
- [ ] Export to PNG/SVG

---

## 📚 Reference Materials

### Key Documentation
- [React Flow Docs](https://reactflow.dev/)
- [React Flow Examples](https://reactflow.dev/examples)
- [Electron Docs](https://www.electronjs.org/docs)
- [Vite Guide](https://vitejs.dev/guide/)

### Old Codebase
- `mlp_gui_designer/src/components/widget.js` - Widget structure
- `mlp_gui_designer/src/components/designer.js` - Designer logic
- `mlp_gui_designer/src/components/code-generator.js` - Code gen
- `mlp_gui_designer/src/components/properties.js` - Properties panel

### Migration Reference
**Old Widget Format:**
```javascript
{
  id: "widget-123",
  type: "button",
  properties: {
    x: 100, y: 50,
    width: 120, height: 40,
    text: "Click Me",
    backgroundColor: "#007bff",
    textColor: "#ffffff",
    fontSize: 14
  }
}
```

**New Node Format:**
```javascript
{
  id: "widget-123",
  type: "buttonNode",
  position: { x: 100, y: 50 },
  data: {
    label: "Click Me",
    width: 120,
    height: 40,
    style: {
      backgroundColor: "#007bff",
      textColor: "#ffffff",
      fontSize: 14
    }
  }
}
```

---

## 🚀 Quick Start Commands

```bash
# Install dependencies
npm install

# Development mode (hot reload)
npm run dev

# Build for production
npm run build

# Run tests
npm test

# Lint code
npm run lint
```

---

## 🐛 Known Issues & Solutions

### Issue: Electron + Vite integration
**Solution:** Use electron-vite or vite-plugin-electron

### Issue: Node resize performance
**Solution:** Throttle resize events, use CSS transforms

### Issue: Large project load times
**Solution:** Lazy load nodes, implement pagination

---

## 💡 Architecture Decisions

### Why React Flow?
- ✅ Built-in virtualization
- ✅ Optimized for node editors
- ✅ Smooth zoom/pan
- ✅ Active development
- ✅ TypeScript support
- ✅ Figma-like performance achievable

### Why Not Konva/PixiJS?
- Konva: Canvas-based, harder to integrate with React
- PixiJS: WebGL overkill, more complex API
- React Flow: Perfect balance of features and simplicity

### Architecture Pattern
```
Electron Main Process
    ↓
  Vite Dev Server
    ↓
React App
    ↓
React Flow Canvas
    ↓
Custom Node Components
    ↓
Widget Adapter Layer
    ↓
MLP Code Generator
```

---

## 📝 Notes for Future AI

### Context Preservation
This project is a migration from a DOM-based GUI designer to a React Flow-based editor for the MLP programming language. The old designer (`mlp_gui_designer/`) uses vanilla JavaScript with manual DOM manipulation, which causes performance issues.

### Key Requirements
1. **Performance:** Must achieve 60 FPS with 500+ widgets
2. **Feature Parity:** All features from old designer must work
3. **Code Generation:** Must generate valid MLP code in multiple languages
4. **Backward Compatibility:** Should be able to import old `.mlpgui` files

### Important Files
- Old designer: `mlp_gui_designer/src/components/`
- Widget structure: Check `widget.js` for data format
- Code generation: `code-generator.js` has language mappings
- MLP language: See `AI_RULES.md`, `SPECS.md`, `diller.json`

### Don't Break
- MLP code generation logic
- Language keyword mappings (tr-TR, en-US)
- Widget property structure
- Event handler naming conventions

### Testing Strategy
Always test with:
1. 10 widgets (basic functionality)
2. 100 widgets (moderate load)
3. 500 widgets (stress test)
4. Measure FPS, memory, load time

---

## 🎨 Design Guidelines

### Visual Consistency
- Follow MLP GUI Designer color scheme
- Use dark theme (VS Code style)
- Widget colors match MLP conventions

### UX Principles
- Drag & drop should feel natural
- Properties update in real-time
- Code generation is instant
- Save operations are auto-saved

---

## 📅 Timeline

**Estimated Total Time:** 40-50 hours
**Target Completion:** Early December 2025
**Current Status:** Day 1, Phase 0

### Week 1 (Nov 23-29)
- Days 1-2: Setup & React Flow basics
- Days 3-4: Custom nodes & adapter
- Days 5-7: UI components

### Week 2 (Nov 30 - Dec 6)
- Days 1-3: Code generation & project management
- Days 4-5: Performance optimization
- Days 6-7: Testing & bug fixing

---

## ✅ Checklist for Completion

### Before Marking as Done
- [ ] All 9 widget types working
- [ ] 60 FPS achieved with 500 nodes
- [ ] Code generation produces valid MLP
- [ ] Save/load working perfectly
- [ ] Old projects can be imported
- [ ] Documentation updated
- [ ] No memory leaks
- [ ] User testing completed

---

**Last Updated:** November 23, 2025 - 13:45
**Updated By:** GitHub Copilot (Initial setup)
**Next Action:** Install dependencies (Phase 1.1)
