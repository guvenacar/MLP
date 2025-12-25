# 🎨 MLP Visual Block Editor

Modern, drag-and-drop visual programming editor for MLP language.

## Features

✨ **Visual Programming**
- Drag-and-drop code blocks (Scratch-like)
- Real-time code preview
- Instant compilation and execution

🌍 **Multi-Language Support**
- 8 languages: English, Turkish, Russian, Arabic, Chinese, Spanish, French, German
- 10+ syntax styles: Python, C, VB.NET, MATLAB, and more

📦 **Block Types**
- Control Flow: For loops, While loops, If/Else
- Variables: Declaration and assignment
- I/O: Print statements
- Functions: Definition and return

## Quick Start

### 1. Install Dependencies

```bash
# Flask for backend
pip3 install flask flask-cors

# Or use requirements.txt
pip3 install -r requirements.txt
```

### 2. Run the Editor

```bash
cd gui_editor
python3 server.py
```

### 3. Open in Browser

Navigate to: **http://localhost:5000**

## Usage

1. **Drag blocks** from left palette to center canvas
2. **Configure blocks** by editing fields (variable names, conditions, etc.)
3. **View generated code** in right panel (Code tab)
4. **Run program** by clicking ▶ Run button
5. **See output** in Output tab

## Architecture

```
User Interface (HTML/CSS/JS)
    ↓
Flask Backend (Python)
    ↓
MLP Compilation Pipeline
    ├── Preprocessor (mlp_preprocessor.py)
    ├── Compiler (mlp_compiler)
    ├── NASM Assembler
    └── GCC Linker + Runtime
    ↓
Program Execution
```

## Example Workflow

### Visual Blocks:
1. Drag "For Loop" → Set: `i = 0 to 10 step 2`
2. Drag "Print" inside loop → Set: `i`

### Generated Code:
```mlp
for i = 0 to 10 step 2
    print(i)
end
```

### Output:
```
0
2
4
6
8
10
```

## API Endpoints

### `POST /api/run`
Compile and run MLP program.

**Request:**
```json
{
  "code": "for i = 0 to 5\n    print(i)\nend"
}
```

**Response:**
```json
{
  "success": true,
  "output": "0\n1\n2\n3\n4\n5\n"
}
```

### `GET /api/health`
Check server status and dependencies.

## Development

### File Structure
```
gui_editor/
├── index.html       # Main UI
├── style.css        # Styling
├── blocks.js        # Block definitions & logic
├── editor.js        # Drag-drop & canvas management
├── server.py        # Flask backend
├── requirements.txt # Python dependencies
└── README.md        # This file
```

### Adding New Block Types

Edit `blocks.js`:

```javascript
blockTemplates['myblock'] = {
    name: 'My Block',
    color: 'io-block',
    fields: [
        { name: 'param', label: 'Param:', value: 'default' }
    ],
    generate: (fields) => `mycommand(${fields.param})`
};
```

## Browser Compatibility

- ✅ Chrome/Edge (recommended)
- ✅ Firefox
- ✅ Safari
- ⚠️ IE11 not supported

## Troubleshooting

### Server won't start
```bash
# Install Flask
pip3 install flask flask-cors

# Check paths in server.py
python3 -c "import os; print(os.path.exists('../c_compiler/mlp_compiler'))"
```

### Compilation fails
```bash
# Rebuild compiler
cd ../c_compiler
gcc -c c_lexer.c c_parser.c c_generator.c main.c
gcc *.o -o mlp_compiler
```

### Can't run programs
- Ensure NASM installed: `sudo apt install nasm`
- Ensure GCC installed: `sudo apt install gcc`
- Check runtime.c exists: `ls ../runtime/runtime.c`

## Future Enhancements

- [ ] Nested block support (drag into loops/conditions)
- [ ] Block library save/load
- [ ] Undo/redo functionality
- [ ] Block search and filter
- [ ] Collaborative editing
- [ ] Mobile responsive design
- [ ] Dark mode toggle
- [ ] Export to multiple formats (Python, C, etc.)

## License

Part of MLP project. See root LICENSE file.

## Credits

Built with ❤️ for MLP v3.0 multi-language compiler project.
