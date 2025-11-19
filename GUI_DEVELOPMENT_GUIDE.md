# MLP Desktop GUI Development Workflow

## 🎯 Overview

This guide shows how to develop desktop GUI applications using MLP in VSCode with full IntelliSense, syntax highlighting, and one-click build/run support.

---

## 📋 Prerequisites

### 1. Install Required Tools

**On Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install -y build-essential nasm gcc libsdl2-dev
```

**On macOS:**
```bash
brew install nasm gcc sdl2
```

**On Windows (WSL):**
```bash
# Use WSL Ubuntu and follow Ubuntu instructions
sudo apt update
sudo apt install -y build-essential nasm gcc libsdl2-dev
```

### 2. Install VSCode Extensions

1. **MLP Language Support** (included in this repo)
   ```bash
   cd MLP
   code --install-extension vscode-mlp/mlp-language-*.vsix
   ```

2. **C/C++ Extension** (for debugging)
   - Open VSCode Extensions (Ctrl+Shift+X)
   - Search "C/C++" by Microsoft
   - Install

---

## 🚀 Development Workflow

### Step 1: Create New GUI Project

```bash
cd MLP
mkdir my_gui_app
cd my_gui_app
```

### Step 2: Write Your GUI Application

Create `main.mlp`:

```mlp
-- lang: en-US
-- My First GUI Application

print "Initializing GUI..."

int window = gui_window_create("My First App", 800, 600);
int label = gui_label_create(window, "Hello, World!", 300, 250);
int button = gui_button_create(window, "Click Me", 320, 300, 120, 40);

int click_count = 0;
gui_window_show(window)

print "GUI Ready! Close the window to exit."

while
    int event = gui_poll_event();

    if event == 1 then
        -- GUI_EVENT_QUIT
        print "User closed window"
        break
    end

    if event == 2 then
        -- GUI_EVENT_BUTTON_CLICK
        int clicked = gui_get_clicked_button();
        if clicked == button then
            click_count = click_count + 1
            print "Button clicked! Count:"
            print click_count

            gui_label_set_text(label, "Button was clicked!")
        end
    end
end

gui_window_destroy(window)
print "Application closed"
```

### Step 3: Configure VSCode Build Task

Create `.vscode/tasks.json`:

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Build MLP GUI App",
            "type": "shell",
            "command": "${workspaceFolder}/../mlpc",
            "args": [
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "reveal": "always",
                "panel": "new"
            },
            "problemMatcher": []
        },
        {
            "label": "Run MLP GUI App",
            "type": "shell",
            "command": "${fileDirname}/${fileBasenameNoExtension}",
            "dependsOn": "Build MLP GUI App",
            "group": {
                "kind": "test",
                "isDefault": true
            },
            "presentation": {
                "reveal": "always",
                "panel": "new"
            }
        }
    ]
}
```

### Step 4: Build and Run

**Method 1: Keyboard Shortcuts**
- Press `Ctrl+Shift+B` - Build
- Press `Ctrl+Shift+T` - Build and Run

**Method 2: Command Palette**
- Press `F1` or `Ctrl+Shift+P`
- Type "Run Build Task"
- Select "Build MLP GUI App"

**Method 3: Terminal**
```bash
# From project directory
../mlpc main.mlp -o my_app
./my_app
```

---

## 🎨 GUI Design Patterns

### Pattern 1: Simple Window

```mlp
-- lang: en-US
int window = gui_window_create("Simple App", 400, 300);
gui_window_show(window)

-- Event loop
while
    int event = gui_poll_event();
    if event == 1 then break end
end

gui_window_destroy(window)
```

**Turkish Version:**
```mlp
-- lang: tr-TR
SAYISAL pencere = gui_window_create("Basit Uygulama", 400, 300);
gui_window_show(pencere)

-- Olay döngüsü
DÖNGÜ
    SAYISAL olay = gui_poll_event();
    EĞER olay == 1 İSE DÖNGÜ_BITIR SON
SON

gui_window_destroy(pencere)
```

### Pattern 2: Button Counter

```mlp
-- lang: en-US
int window = gui_window_create("Counter", 400, 200);
int label = gui_label_create(window, "Count: 0", 150, 50);
int button = gui_button_create(window, "Click", 150, 100, 100, 40);

int count = 0;
gui_window_show(window)

while
    int event = gui_poll_event();
    if event == 1 then break end

    if event == 2 then
        int clicked = gui_get_clicked_button();
        if clicked == button then
            count = count + 1
            -- Update display
            print count
        end
    end
end

gui_window_destroy(window)
```

### Pattern 3: Drawing Canvas

```mlp
-- lang: en-US
int window = gui_window_create("Drawing", 800, 600);
int canvas = gui_canvas_create(window, 0, 0, 800, 600);

-- Clear with white background
gui_canvas_clear(canvas, 255, 255, 255)

-- Draw red rectangle
gui_canvas_set_color(canvas, 255, 0, 0)
gui_canvas_draw_rect(canvas, 100, 100, 200, 150)

-- Draw green circle
gui_canvas_set_color(canvas, 0, 255, 0)
gui_canvas_draw_circle(canvas, 400, 300, 80)

-- Draw blue line
gui_canvas_set_color(canvas, 0, 0, 255)
gui_canvas_draw_line(canvas, 50, 50, 750, 550)

gui_canvas_render(canvas)
gui_window_show(window)

while
    int event = gui_poll_event();
    if event == 1 then break end
end

gui_window_destroy(window)
```

---

## 🔧 VSCode Features for MLP GUI Development

### 1. Syntax Highlighting

The MLP VSCode extension provides:
- ✅ Keyword highlighting (if, while, function, etc.)
- ✅ String literal colors
- ✅ Comment styling
- ✅ Number highlighting
- ✅ GUI function recognition

### 2. Code Snippets

Type these shortcuts and press `Tab`:

| Shortcut | Expands To |
|----------|------------|
| `guiwin` | Basic GUI window template |
| `guibtn` | Button with event handler |
| `guicanvas` | Canvas drawing template |
| `guiloop` | Event loop structure |

### 3. IntelliSense

When you type `gui_`, VSCode shows:
- `gui_window_create`
- `gui_button_create`
- `gui_label_create`
- `gui_canvas_draw_rect`
- ... and more

### 4. Build Integration

- **Build Task**: `Ctrl+Shift+B`
- **Run Task**: `Ctrl+Shift+T`
- **Terminal**: Integrated terminal support

### 5. Error Highlighting

The extension highlights:
- ❌ Syntax errors
- ⚠️ Missing semicolons
- ⚠️ Undefined functions
- ⚠️ Type mismatches

---

## 📦 Project Structure

```
my_gui_app/
├── .vscode/
│   ├── tasks.json          # Build/run tasks
│   ├── launch.json         # Debug configuration (optional)
│   └── settings.json       # Project settings
├── assets/                 # Images, icons (future)
├── src/
│   ├── main.mlp           # Main application
│   ├── ui/
│   │   ├── window.mlp     # Window setup
│   │   └── widgets.mlp    # Widget helpers (future)
│   └── logic/
│       └── handlers.mlp   # Event handlers (future)
├── build/                 # Compiled outputs (gitignored)
└── README.md
```

---

## 🐛 Debugging

### Method 1: Print Debugging

```mlp
-- lang: en-US
print "DEBUG: Creating window..."
int window = gui_window_create("App", 800, 600);
print "DEBUG: Window created, ID:"
print window

print "DEBUG: Entering event loop..."
while
    int event = gui_poll_event();
    print "DEBUG: Event received:"
    print event

    if event == 1 then break end
end
```

### Method 2: Assembly Inspection

```bash
# Keep assembly file
../mlpc main.mlp --keep-asm

# View generated assembly
cat main.asm | less
```

### Method 3: GDB Debugging (Advanced)

```bash
# Compile with debug symbols
../mlpc main.mlp -o my_app

# Run with GDB
gdb ./my_app
(gdb) break main
(gdb) run
(gdb) step
```

---

## 🎨 Multi-Language Development

### Example: Calculator in Turkish

```mlp
-- lang: tr-TR
-- Basit Hesap Makinesi

SAYISAL pencere = gui_window_create("Hesap Makinesi", 300, 400);
SAYISAL sonuc_etiket = gui_label_create(pencere, "0", 50, 30);

-- Sayı butonları
SAYISAL btn_1 = gui_button_create(pencere, "1", 50, 100, 60, 40);
SAYISAL btn_2 = gui_button_create(pencere, "2", 120, 100, 60, 40);
SAYISAL btn_3 = gui_button_create(pencere, "3", 190, 100, 60, 40);

SAYISAL btn_topla = gui_button_create(pencere, "+", 50, 150, 60, 40);
SAYISAL btn_cikar = gui_button_create(pencere, "-", 120, 150, 60, 40);

SAYISAL toplam = 0;
gui_window_show(pencere)

DÖNGÜ
    SAYISAL olay = gui_poll_event();
    EĞER olay == 1 İSE DÖNGÜ_BITIR SON

    EĞER olay == 2 İSE
        SAYISAL tiklanan = gui_get_clicked_button();

        EĞER tiklanan == btn_1 İSE
            toplam = toplam + 1
            gui_label_set_text(sonuc_etiket, "Sayı artırıldı")
        SON

        EĞER tiklanan == btn_topla İSE
            toplam = toplam + 10
            gui_label_set_text(sonuc_etiket, "10 eklendi")
        SON
    SON
SON

gui_window_destroy(pencere)
```

---

## 🚀 Deployment

### Step 1: Build Release Version

```bash
# Build optimized version
../mlpc --release main.mlp -o my_app_release

# Or with custom name
../mlpc main.mlp -o "MyGuiApp"
```

### Step 2: Bundle Dependencies

Create `package.sh`:
```bash
#!/bin/bash
APP_NAME="MyGuiApp"
BUILD_DIR="dist"

mkdir -p $BUILD_DIR

# Copy executable
cp $APP_NAME $BUILD_DIR/

# Copy runtime libraries (if using SDL2)
cp /usr/lib/x86_64-linux-gnu/libSDL2-2.0.so.0 $BUILD_DIR/

# Create launcher script
cat > $BUILD_DIR/run.sh <<EOF
#!/bin/bash
export LD_LIBRARY_PATH="\$(dirname "\$0"):\$LD_LIBRARY_PATH"
./\$APP_NAME
EOF

chmod +x $BUILD_DIR/run.sh

echo "Package created in $BUILD_DIR/"
```

### Step 3: Create Desktop Entry (Linux)

Create `MyGuiApp.desktop`:
```ini
[Desktop Entry]
Name=My GUI App
Comment=MLP GUI Application
Exec=/path/to/my_app
Icon=/path/to/icon.png
Terminal=false
Type=Application
Categories=Utility;
```

---

## 📊 Performance Optimization

### Tip 1: Minimize Redraws

```mlp
-- BAD: Redraw every frame
while
    gui_canvas_clear(canvas, 255, 255, 255)
    gui_canvas_render(canvas)  -- Too frequent!
end

-- GOOD: Redraw only on events
int needs_redraw = 0;
while
    int event = gui_poll_event();

    if event == 2 then
        -- Button clicked, need redraw
        needs_redraw = 1
    end

    if needs_redraw == 1 then
        gui_canvas_clear(canvas, 255, 255, 255)
        gui_canvas_render(canvas)
        needs_redraw = 0
    end
end
```

### Tip 2: Use Local Variables

```mlp
-- GOOD: Local variables are faster
while
    int event = gui_poll_event();  -- Local
    -- Process event
end
```

### Tip 3: Avoid Repeated Function Calls

```mlp
-- BAD
while
    if gui_poll_event() == 1 then  -- Called twice!
        break
    end
    if gui_poll_event() == 2 then
        -- handle
    end
end

-- GOOD
while
    int event = gui_poll_event();  -- Called once
    if event == 1 then break end
    if event == 2 then
        -- handle
    end
end
```

---

## 🔮 Future Features

### Planned for Next Version

- [ ] **Form Builder**: Visual GUI designer in VSCode
- [ ] **Widget Library**: Pre-built components (Menu, Dialog, etc.)
- [ ] **Event System**: Callback-based event handling
- [ ] **Layout Manager**: Auto-arrange widgets
- [ ] **Theme Support**: Dark mode, custom colors
- [ ] **Hot Reload**: See changes without restart

### Example Future Syntax

```mlp
-- Future: Layout manager
int window = gui_window_create("App", 800, 600);
int layout = gui_layout_vertical(window);

gui_layout_add(layout, gui_label_create(window, "Name:"));
gui_layout_add(layout, gui_textbox_create(window));
gui_layout_add(layout, gui_button_create(window, "Submit"));

gui_layout_apply(layout)
```

---

## 📚 Example Projects

### 1. Todo List App

```mlp
-- lang: en-US
-- Simple Todo List Application

int window = gui_window_create("Todo List", 400, 500);
int input = gui_textbox_create(window, 20, 20, 300, 30);
int add_btn = gui_button_create(window, "Add", 330, 20, 50, 30);
int list_canvas = gui_canvas_create(window, 20, 60, 360, 400);

int todo_count = 0;
gui_window_show(window)

while
    int event = gui_poll_event();
    if event == 1 then break end

    if event == 2 then
        int clicked = gui_get_clicked_button();
        if clicked == add_btn then
            todo_count = todo_count + 1
            -- Redraw list
            gui_canvas_clear(list_canvas, 255, 255, 255)
            gui_canvas_render(list_canvas)
        end
    end
end

gui_window_destroy(window)
```

### 2. Image Viewer (Future)

```mlp
-- lang: en-US
-- Image Viewer (requires image support)

int window = gui_window_create("Image Viewer", 800, 600);
int canvas = gui_canvas_create(window, 0, 0, 800, 600);
int open_btn = gui_button_create(window, "Open", 10, 10, 80, 30);

gui_window_show(window)

while
    int event = gui_poll_event();
    if event == 1 then break end

    if event == 2 then
        int clicked = gui_get_clicked_button();
        if clicked == open_btn then
            -- string file = gui_file_dialog_open()
            -- gui_canvas_draw_image(canvas, file, 0, 0)
            print "File dialog would open here"
        end
    end
end

gui_window_destroy(window)
```

---

## 🎓 Learning Resources

### Tutorials
1. **GUI Basics**: Start with `example_gui_button.mlp`
2. **Event Handling**: Study the event loop patterns
3. **Canvas Drawing**: Experiment with shapes and colors
4. **Multi-Language**: Try Turkish/Russian examples

### References
- **MLP Language Spec**: `SPECS.md`
- **GUI API Reference**: `GUI_DESIGN.md`
- **Examples**: `example_gui_*.mlp`

### Community
- GitHub Issues: Report bugs, request features
- Discussions: Share your GUI apps!

---

## ✅ Quick Start Checklist

- [ ] Install prerequisites (gcc, nasm, SDL2)
- [ ] Install MLP VSCode extension
- [ ] Create project directory
- [ ] Write your first GUI app
- [ ] Configure VSCode tasks
- [ ] Build and run with `Ctrl+Shift+B`
- [ ] Try different languages (Turkish, Russian, etc.)
- [ ] Experiment with canvas drawing
- [ ] Share your creation!

---

**Happy GUI Coding with MLP!** 🎨✨

**Remember:** MLP GUI apps compile to native code, so they're fast and efficient. You can build real desktop applications in your native language!
