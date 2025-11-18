# Building MLP with SDL2 GUI Support

## Quick Start

### 1. Install SDL2

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install libsdl2-dev libsdl2-ttf-dev
```

**macOS:**
```bash
brew install sdl2 sdl2_ttf
```

**Arch Linux:**
```bash
sudo pacman -S sdl2 sdl2_ttf
```

**Verify Installation:**
```bash
pkg-config --modversion sdl2
# Should output: 2.x.x
```

### 2. Compile SDL2 Runtime

```bash
cd MLP/runtime

# Compile SDL2 GUI runtime
gcc -c gui_sdl.c -o gui_sdl.o $(pkg-config --cflags sdl2)

# Verify
ls -lh gui_sdl.o
```

### 3. Build GUI Application

**Option A: Using mlpc wrapper (recommended)**
```bash
cd MLP

# Compile with SDL2 backend
./mlpc example_gui_button.mlp -o my_gui_app --gui=sdl2

# Run
./my_gui_app
```

**Option B: Manual compilation**
```bash
cd MLP

# Step 1: Preprocess (if multi-language)
python3 dil_cevirici.py example_gui_button.mlp example_gui_button.preprocessed.mlp

# Step 2: Compile to assembly
./c_compiler/compiler_test example_gui_button.mlp gui_app.asm

# Step 3: Assemble
nasm -f elf64 gui_app.asm -o gui_app.o

# Step 4: Link with SDL2 runtime
gcc gui_app.o runtime/runtime.o runtime/gui_sdl.o \
    -o my_gui_app $(pkg-config --libs sdl2) -no-pie

# Step 5: Run
./my_gui_app
```

## Example: Button Counter (Real GUI)

**Create `button_counter.mlp`:**
```mlp
-- lang: en-US
-- Real GUI Button Counter with SDL2

print "Starting GUI application..."

int window = gui_window_create("Button Counter", 400, 300);
int label = gui_label_create(window, "Count: 0", 150, 100);
int button = gui_button_create(window, "Click Me!", 150, 150, 100, 40);

int count = 0;
gui_window_show(window)

print "GUI window opened. Click the button!"
print "Close the window to exit."

while
    int event = gui_poll_event();

    if event == 1 then
        -- Quit event
        print "Window closed by user"
        break
    end

    if event == 2 then
        -- Button click
        int clicked = gui_get_clicked_button();
        if clicked == button then
            count = count + 1
            print "Button clicked! Count:"
            print count
            -- Note: Label text update requires SDL_ttf
            -- For now, we see visual button press
        end
    end
end

gui_window_destroy(window)
print "Application closed"
```

**Compile and run:**
```bash
./mlpc button_counter.mlp -o counter --gui=sdl2
./counter
```

**Expected behavior:**
- Real window opens (400x300 pixels)
- Gray button visible at coordinates (150, 150)
- Button turns darker when hovered
- Button turns dark gray when clicked
- Console shows click count
- Close window to exit

## Example: Canvas Drawing (Real GUI)

**Create `canvas_draw.mlp`:**
```mlp
-- lang: en-US
-- Drawing shapes with SDL2

int window = gui_window_create("Canvas Drawing", 800, 600);
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

-- Render to screen
gui_canvas_render(canvas)
gui_window_show(window)

print "Canvas window opened with shapes!"
print "Close window to exit."

-- Event loop
while
    int event = gui_poll_event();
    if event == 1 then
        break
    end
end

gui_window_destroy(window)
```

**Compile and run:**
```bash
./mlpc canvas_draw.mlp -o canvas --gui=sdl2
./canvas
```

**Expected behavior:**
- Window opens with shapes drawn
- Red rectangle at (100, 100)
- Green circle at center (400, 300)
- Blue diagonal line
- Smooth rendering with SDL2

## Switching Between Backends

### Mock Backend (No SDL2 needed)
```bash
# Uses console simulation
./mlpc example_gui_button.mlp -o test_mock
./test_mock
# Output: Console messages only, no real window
```

### SDL2 Backend (Real GUI)
```bash
# Uses SDL2 for real windows
./mlpc example_gui_button.mlp -o test_sdl --gui=sdl2
./test_sdl
# Output: Real GUI window opens!
```

## Updating mlpc Wrapper

Edit `mlpc` script to support `--gui=sdl2` flag:

```bash
#!/bin/bash

# ... existing code ...

# Detect GUI backend
GUI_BACKEND="mock"
if [[ "$@" == *"--gui=sdl2"* ]]; then
    GUI_BACKEND="sdl2"
fi

# Link step
if [ "$GUI_BACKEND" == "sdl2" ]; then
    gcc "$OBJ_FILE" runtime/runtime.o runtime/gui_sdl.o \
        -o "$OUTPUT_NAME" $(pkg-config --libs sdl2) -no-pie
else
    gcc "$OBJ_FILE" runtime/runtime.o runtime/gui_mock.o \
        -o "$OUTPUT_NAME" -no-pie
fi
```

## Troubleshooting

### SDL2 not found
```
Error: Package sdl2 was not found in the pkg-config search path
```

**Solution:** Install SDL2 development libraries:
```bash
sudo apt install libsdl2-dev
```

### Window doesn't open
**Check:**
1. SDL2 compiled correctly: `ls runtime/gui_sdl.o`
2. Linked with SDL2: `ldd my_gui_app | grep SDL2`
3. X11/Wayland display available

### Button not visible
- SDL2 backend shows buttons as colored rectangles
- Text rendering requires SDL_ttf (future enhancement)
- Button hover and click visual feedback works

### Performance issues
- SDL2 uses hardware acceleration
- Should be smooth for simple UIs
- For complex UIs, optimize rendering

## Advanced: Adding SDL_ttf for Text

To display actual text on buttons and labels:

```bash
# Install SDL_ttf
sudo apt install libsdl2-ttf-dev

# Modify gui_sdl.c to include:
#include <SDL2/SDL_ttf.h>

# Initialize in ensure_sdl_init():
TTF_Init();

# Render text in render_button():
TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 16);
SDL_Surface* surface = TTF_RenderText_Solid(font, btn->text, (SDL_Color){0, 0, 0, 255});
SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
SDL_RenderCopy(renderer, texture, NULL, &text_rect);
```

## Next Steps

1. ✅ Compile SDL2 runtime
2. ✅ Test basic window example
3. ✅ Test button click example
4. ✅ Test canvas drawing
5. 📋 Add SDL_ttf for text rendering
6. 📋 Add more widgets (textbox, checkbox, slider)
7. 📋 Add layout manager
8. 📋 Create visual GUI designer

**Your MLP applications now run with real native GUI!** 🚀
