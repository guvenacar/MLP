# MLP GUI Programming Design

## Philosophy

MLP GUI programming should be:
- **Simple**: Easy syntax, beginner-friendly
- **Multi-language**: Support Turkish, Russian, Chinese, etc.
- **Native**: Use C GUI libraries (SDL2, GTK, Raylib)
- **Flexible**: Multiple backend options

---

## Proposed Syntax (English)

### Example 1: Simple Window

```mlp
-- lang: en-US
-- Simple GUI window

int window = gui_window_create("My App", 800, 600);
gui_window_show(window)

-- Event loop
while
    int event = gui_poll_event();

    if event == GUI_EVENT_QUIT then
        break
    end
end

gui_window_destroy(window)
```

### Example 2: Button and Label

```mlp
-- lang: en-US
-- Button click counter

int window = gui_window_create("Counter", 400, 300);
int label = gui_label_create(window, "Count: 0", 50, 50);
int button = gui_button_create(window, "Click Me", 50, 100, 150, 40);

int count = 0;
gui_window_show(window)

while
    int event = gui_poll_event();

    if event == GUI_EVENT_QUIT then
        break
    end

    if event == GUI_EVENT_BUTTON_CLICK then
        int clicked_id = gui_get_clicked_button();
        if clicked_id == button then
            count = count + 1
            string text = string_concat("Count: ", int_to_string(count));
            gui_label_set_text(label, text)
        end
    end
end

gui_window_destroy(window)
```

### Example 3: Drawing (Canvas)

```mlp
-- lang: en-US
-- Simple drawing

int window = gui_window_create("Drawing", 800, 600);
int canvas = gui_canvas_create(window, 0, 0, 800, 600);

gui_canvas_clear(canvas, 255, 255, 255)
gui_canvas_set_color(canvas, 255, 0, 0)
gui_canvas_draw_rect(canvas, 100, 100, 200, 150)

gui_canvas_set_color(canvas, 0, 255, 0)
gui_canvas_draw_circle(canvas, 400, 300, 50)

gui_canvas_render(canvas)
gui_window_show(window)

-- Event loop
while
    int event = gui_poll_event();
    if event == GUI_EVENT_QUIT then
        break
    end
end
```

---

## Turkish Syntax (Türkçe)

### Örnek 1: Basit Pencere

```mlp
-- lang: tr-TR
-- Basit GUI penceresi

SAYISAL pencere = GUI_PENCERE_AC("Uygulamam", 800, 600);
GUI_PENCERE_GOSTER(pencere)

-- Olay döngüsü
DÖNGÜ
    SAYISAL olay = GUI_OLAY_AL();

    EĞER olay == GUI_OLAY_KAPAT İSE
        DÖNGÜ_BITIR
    SON
SON

GUI_PENCERE_YOK_ET(pencere)
```

### Örnek 2: Buton ve Etiket

```mlp
-- lang: tr-TR
-- Buton tıklama sayacı

SAYISAL pencere = GUI_PENCERE_AC("Sayaç", 400, 300);
SAYISAL etiket = GUI_ETIKET_EKLE(pencere, "Sayı: 0", 50, 50);
SAYISAL buton = GUI_BUTON_EKLE(pencere, "Tıkla", 50, 100, 150, 40);

SAYISAL sayi = 0;
GUI_PENCERE_GOSTER(pencere)

DÖNGÜ
    SAYISAL olay = GUI_OLAY_AL();

    EĞER olay == GUI_OLAY_KAPAT İSE
        DÖNGÜ_BITIR
    SON

    EĞER olay == GUI_OLAY_BUTON_TIKLA İSE
        SAYISAL tiklanan = GUI_TIKLANAN_BUTON_AL();
        EĞER tiklanan == buton İSE
            sayi = sayi + 1
            METIN yazi = STRING_BIRLESTIR("Sayı: ", SAYI_METNE(sayi));
            GUI_ETIKET_YAZI_YAZ(etiket, yazi)
        SON
    SON
SON
```

---

## Built-in GUI Functions

### Window Management

| English | Turkish | Parameters | Returns |
|---------|---------|------------|---------|
| `gui_window_create(title, w, h)` | `GUI_PENCERE_AC(baslik, g, y)` | string, int, int | int (window id) |
| `gui_window_show(id)` | `GUI_PENCERE_GOSTER(id)` | int | void |
| `gui_window_hide(id)` | `GUI_PENCERE_GIZLE(id)` | int | void |
| `gui_window_destroy(id)` | `GUI_PENCERE_YOK_ET(id)` | int | void |
| `gui_window_set_title(id, title)` | `GUI_PENCERE_BASLIK(id, baslik)` | int, string | void |

### Widgets

| English | Turkish | Description |
|---------|---------|-------------|
| `gui_label_create(win, text, x, y)` | `GUI_ETIKET_EKLE(win, yazi, x, y)` | Static text |
| `gui_button_create(win, text, x, y, w, h)` | `GUI_BUTON_EKLE(win, yazi, x, y, g, y)` | Clickable button |
| `gui_textbox_create(win, x, y, w, h)` | `GUI_YAZI_KUTUSU_EKLE(win, x, y, g, y)` | Text input |
| `gui_checkbox_create(win, text, x, y)` | `GUI_ONAY_KUTUSU_EKLE(win, yazi, x, y)` | Checkbox |
| `gui_canvas_create(win, x, y, w, h)` | `GUI_TUVAL_EKLE(win, x, y, g, y)` | Drawing canvas |

### Events

| English | Turkish | Value |
|---------|---------|-------|
| `GUI_EVENT_QUIT` | `GUI_OLAY_KAPAT` | Window close |
| `GUI_EVENT_BUTTON_CLICK` | `GUI_OLAY_BUTON_TIKLA` | Button clicked |
| `GUI_EVENT_KEY_PRESS` | `GUI_OLAY_TUS_BAS` | Key pressed |
| `GUI_EVENT_MOUSE_MOVE` | `GUI_OLAY_FARE_HAREKET` | Mouse moved |
| `gui_poll_event()` | `GUI_OLAY_AL()` | Get next event |
| `gui_get_clicked_button()` | `GUI_TIKLANAN_BUTON_AL()` | Get button ID |

### Drawing (Canvas)

| English | Turkish | Description |
|---------|---------|-------------|
| `gui_canvas_clear(id, r, g, b)` | `GUI_TUVAL_TEMIZLE(id, k, y, m)` | Clear with color |
| `gui_canvas_set_color(id, r, g, b)` | `GUI_TUVAL_RENK(id, k, y, m)` | Set draw color |
| `gui_canvas_draw_rect(id, x, y, w, h)` | `GUI_TUVAL_DIKDORTGEN(id, x, y, g, y)` | Draw rectangle |
| `gui_canvas_draw_circle(id, x, y, r)` | `GUI_TUVAL_DAIRE(id, x, y, r)` | Draw circle |
| `gui_canvas_draw_line(id, x1, y1, x2, y2)` | `GUI_TUVAL_CIZGI(id, x1, y1, x2, y2)` | Draw line |
| `gui_canvas_render(id)` | `GUI_TUVAL_GOSTER(id)` | Display canvas |

---

## Implementation Options

### Option 1: SDL2 Backend

**Pros:**
- Cross-platform (Linux, Windows, macOS)
- Simple C API
- Good for games and multimedia
- Hardware acceleration
- Widely used

**Cons:**
- Manual widget creation (no built-in buttons)
- Need to implement widgets ourselves

**Code:**
```c
// runtime/gui_sdl.c
#include <SDL2/SDL.h>

int gui_window_create(const char* title, int width, int height) {
    SDL_Window* window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    return (int)(intptr_t)window;
}

void gui_window_show(int id) {
    SDL_Window* window = (SDL_Window*)(intptr_t)id;
    SDL_ShowWindow(window);
}

int gui_poll_event() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return GUI_EVENT_QUIT;
        }
        // ... handle other events
    }
    return 0;
}
```

**Compile:**
```bash
gcc runtime/gui_sdl.c -c -o runtime/gui_sdl.o $(pkg-config --cflags sdl2)
gcc output.o runtime/runtime.o runtime/gui_sdl.o -o myapp \
    $(pkg-config --libs sdl2) -no-pie
```

### Option 2: GTK Backend

**Pros:**
- Native Linux desktop widgets
- Professional look
- Built-in widgets (buttons, labels, etc.)
- Automatic layout management
- Theme support

**Cons:**
- Linux-focused (Windows/macOS possible but harder)
- Larger dependency
- More complex API

**Code:**
```c
// runtime/gui_gtk.c
#include <gtk/gtk.h>

static GtkApplication* app = NULL;

int gui_window_create(const char* title, int width, int height) {
    GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), title);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    return (int)(intptr_t)window;
}

int gui_button_create(int win_id, const char* text, int x, int y, int w, int h) {
    GtkWidget* window = (GtkWidget*)(intptr_t)win_id;
    GtkWidget* button = gtk_button_new_with_label(text);
    // ... position and add to window
    return (int)(intptr_t)button;
}
```

**Compile:**
```bash
gcc runtime/gui_gtk.c -c -o runtime/gui_gtk.o $(pkg-config --cflags gtk+-3.0)
gcc output.o runtime/runtime.o runtime/gui_gtk.o -o myapp \
    $(pkg-config --libs gtk+-3.0) -no-pie
```

### Option 3: Raylib Backend

**Pros:**
- Modern C library
- Very simple API
- Great for games and graphics
- Cross-platform
- No external dependencies

**Cons:**
- Primarily for games (less desktop widget support)
- Manual widget implementation needed

**Code:**
```c
// runtime/gui_raylib.c
#include "raylib.h"

int gui_window_create(const char* title, int width, int height) {
    InitWindow(width, height, title);
    SetTargetFPS(60);
    return 1;
}

void gui_canvas_draw_rect(int id, int x, int y, int w, int h) {
    DrawRectangle(x, y, w, h, RED);
}
```

**Compile:**
```bash
gcc output.o runtime/runtime.o runtime/gui_raylib.o -o myapp -lraylib -no-pie
```

---

## Recommended Approach

### Phase 1: Design API (DONE ✓)
- Define MLP syntax
- Create function list
- Multi-language keyword mapping

### Phase 2: Mock Implementation
- Simple console-based GUI simulator
- Test MLP code without real GUI
- Verify syntax and logic

### Phase 3: Choose Backend
- **For beginners:** Raylib (easiest)
- **For desktop apps:** GTK (native widgets)
- **For games/graphics:** SDL2 (flexible)

### Phase 4: Implement Runtime
- Add `runtime/gui.c` with chosen backend
- Update `diller.json` with GUI keywords
- Test examples

---

## Next Steps

1. **Choose GUI library:** SDL2, GTK, or Raylib?
2. **Add keywords to diller.json:**
   ```json
   {
     "keywords": {
       "gui_window_create": ["GUI_PENCERE_AC"],
       "gui_button_create": ["GUI_BUTON_EKLE"],
       ...
     }
   }
   ```
3. **Implement `runtime/gui.c`**
4. **Update compiler to recognize GUI functions**
5. **Create examples and documentation**

---

## Example Project Structure

```
MLP/
├── runtime/
│   ├── runtime.c           # Existing runtime
│   ├── gui.c               # GUI functions
│   └── gui.h               # GUI headers
├── examples/
│   ├── gui_hello.mlp       # Simple window
│   ├── gui_button.mlp      # Button example
│   └── gui_drawing.mlp     # Canvas drawing
├── diller.json             # Add GUI keywords
└── GUI_DESIGN.md           # This document
```

---

## Conclusion

MLP can easily support GUI programming by:
1. Extending the runtime library with C GUI functions
2. Adding GUI keywords to language dictionaries
3. Choosing a backend (SDL2/GTK/Raylib)

The syntax stays clean and multi-language compatible!

**Ready to implement when you choose your preferred GUI library!** 🚀
