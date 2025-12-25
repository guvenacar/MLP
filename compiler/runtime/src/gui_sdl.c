/*
 * MLP GUI Runtime - SDL2 Backend
 *
 * Real GUI implementation using SDL2
 * Compile: gcc -c gui_sdl.c -o gui_sdl.o $(pkg-config --cflags sdl2)
 * Link: gcc ... gui_sdl.o $(pkg-config --libs sdl2)
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ===========================
// Constants & Configuration
// ===========================

#define MAX_WIDGETS 100
#define MAX_WINDOWS 10

#define GUI_EVENT_NONE 0
#define GUI_EVENT_QUIT 1
#define GUI_EVENT_BUTTON_CLICK 2
#define GUI_EVENT_KEY_PRESS 3
#define GUI_EVENT_MOUSE_MOVE 4

// ===========================
// Data Structures
// ===========================

typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXTBOX,
    WIDGET_CANVAS
} WidgetType;

typedef struct {
    long id;
    WidgetType type;
    long window_id;
    SDL_Rect rect;
    char text[256];
    int visible;
    // Button specific
    int is_hovered;
    int is_pressed;
    // Canvas specific
    SDL_Renderer* renderer;
    SDL_Color draw_color;
} Widget;

typedef struct {
    long id;
    SDL_Window* window;
    SDL_Renderer* renderer;
    char title[256];
    int width;
    int height;
    int visible;
    int closed;
} WindowInfo;

// ===========================
// Global State
// ===========================

static WindowInfo windows[MAX_WINDOWS];
static int window_count = 0;

static Widget widgets[MAX_WIDGETS];
static int widget_count = 0;

static int current_event = GUI_EVENT_NONE;
static long clicked_button_id = 0;
static int mouse_x = 0;
static int mouse_y = 0;

static int sdl_initialized = 0;

// Simple API state
static SDL_Window* simple_window = NULL;
static SDL_Renderer* simple_renderer = NULL;
static SDL_Color current_color = {255, 255, 255, 255};

// ===========================
// Helper Functions
// ===========================

static void ensure_sdl_init() {
    if (!sdl_initialized) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            fprintf(stderr, "[GUI] SDL_Init Error: %s\n", SDL_GetError());
            exit(1);
        }
        sdl_initialized = 1;
        printf("[GUI] SDL2 initialized\n");
    }
}

// ===========================
// Simple GUI API (New)
// ===========================

void gui_window_open(long width, long height, const char* title) {
    ensure_sdl_init();
    
    if (simple_window) {
        printf("[GUI] Warning: Window already open, closing previous\n");
        gui_close();
    }
    
    simple_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    
    if (!simple_window) {
        fprintf(stderr, "[GUI] SDL_CreateWindow Error: %s\n", SDL_GetError());
        return;
    }
    
    simple_renderer = SDL_CreateRenderer(simple_window, -1, SDL_RENDERER_ACCELERATED);
    if (!simple_renderer) {
        fprintf(stderr, "[GUI] SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(simple_window);
        simple_window = NULL;
        return;
    }
    
    // Clear to white
    SDL_SetRenderDrawColor(simple_renderer, 255, 255, 255, 255);
    SDL_RenderClear(simple_renderer);
    SDL_RenderPresent(simple_renderer);
    
    printf("[GUI] Window opened: '%s' (%ldx%ld)\n", title, width, height);
}

void gui_set_color(long r, long g, long b) {
    if (!simple_renderer) return;
    
    current_color.r = r;
    current_color.g = g;
    current_color.b = b;
    SDL_SetRenderDrawColor(simple_renderer, r, g, b, 255);
    
    printf("[GUI] Color set to RGB(%ld, %ld, %ld)\n", r, g, b);
}

void gui_draw_rect(long x, long y, long w, long h) {
    if (!simple_renderer) return;
    
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(simple_renderer, &rect);
    
    printf("[GUI] Rectangle drawn at (%ld, %ld) size %ldx%ld\n", x, y, w, h);
}

void gui_update() {
    if (!simple_renderer) return;
    
    SDL_RenderPresent(simple_renderer);
    
    // Process events to keep window responsive
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            gui_close();
        }
    }
    
    printf("[GUI] Screen updated\n");
}

void gui_wait(long milliseconds) {
    SDL_Delay(milliseconds);
    
    // Keep processing events during wait
    SDL_Event e;
    Uint32 start = SDL_GetTicks();
    while (SDL_GetTicks() - start < milliseconds) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                gui_close();
                return;
            }
        }
        SDL_Delay(10);
    }
    
    printf("[GUI] Waited %ld milliseconds\n", milliseconds);
}

void gui_close() {
    if (simple_renderer) {
        SDL_DestroyRenderer(simple_renderer);
        simple_renderer = NULL;
    }
    
    if (simple_window) {
        SDL_DestroyWindow(simple_window);
        simple_window = NULL;
    }
    
    printf("[GUI] Window closed\n");
}

// ===========================
// Legacy Widget-based API
// ===========================

static WindowInfo* find_window(long id) {
    for (int i = 0; i < window_count; i++) {
        if (windows[i].id == id) {
            return &windows[i];
        }
    }
    return NULL;
}

static Widget* find_widget(long id) {
    for (int i = 0; i < widget_count; i++) {
        if (widgets[i].id == id) {
            return &widgets[i];
        }
    }
    return NULL;
}

static void render_button(SDL_Renderer* renderer, Widget* btn) {
    if (!btn->visible) return;

    // Button background color
    if (btn->is_pressed) {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Dark gray when pressed
    } else if (btn->is_hovered) {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); // Light gray when hovered
    } else {
        SDL_SetRenderDrawColor(renderer, 180, 180, 180, 255); // Normal gray
    }
    SDL_RenderFillRect(renderer, &btn->rect);

    // Button border
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderDrawRect(renderer, &btn->rect);

    // TODO: Render text (requires SDL_ttf)
    // For now, button is just a colored rectangle
}

static void render_label(SDL_Renderer* renderer, Widget* lbl) {
    if (!lbl->visible) return;

    // Label background (semi-transparent white)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
    SDL_RenderFillRect(renderer, &lbl->rect);

    // TODO: Render text (requires SDL_ttf)
}

static void render_window(WindowInfo* win) {
    if (!win->visible || win->closed) return;

    SDL_Renderer* renderer = win->renderer;

    // Clear with white background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Render all widgets belonging to this window
    for (int i = 0; i < widget_count; i++) {
        Widget* w = &widgets[i];
        if (w->window_id != win->id) continue;

        switch (w->type) {
            case WIDGET_BUTTON:
                render_button(renderer, w);
                break;
            case WIDGET_LABEL:
                render_label(renderer, w);
                break;
            case WIDGET_CANVAS:
                // Canvas renders itself
                break;
            default:
                break;
        }
    }

    SDL_RenderPresent(renderer);
}

// ===========================
// Window Management
// ===========================

long gui_window_create(const char* title, long width, long height) {
    ensure_sdl_init();

    if (window_count >= MAX_WINDOWS) {
        fprintf(stderr, "[GUI] Maximum windows reached\n");
        return -1;
    }

    WindowInfo* win = &windows[window_count];
    win->id = window_count + 1;
    strncpy(win->title, title, sizeof(win->title) - 1);
    win->width = width;
    win->height = height;
    win->visible = 0;
    win->closed = 0;

    win->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );

    if (!win->window) {
        fprintf(stderr, "[GUI] SDL_CreateWindow Error: %s\n", SDL_GetError());
        return -1;
    }

    win->renderer = SDL_CreateRenderer(win->window, -1, SDL_RENDERER_ACCELERATED);
    if (!win->renderer) {
        fprintf(stderr, "[GUI] SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(win->window);
        return -1;
    }

    window_count++;
    printf("[GUI] Window created: '%s' (%ldx%ld) ID=%ld\n", title, width, height, win->id);
    return win->id;
}

void gui_window_show(long id) {
    WindowInfo* win = find_window(id);
    if (!win) return;

    win->visible = 1;
    SDL_ShowWindow(win->window);
    render_window(win);
    printf("[GUI] Window shown: ID=%ld\n", id);
}

void gui_window_hide(long id) {
    WindowInfo* win = find_window(id);
    if (!win) return;

    win->visible = 0;
    SDL_HideWindow(win->window);
    printf("[GUI] Window hidden: ID=%ld\n", id);
}

void gui_window_destroy(long id) {
    WindowInfo* win = find_window(id);
    if (!win) return;

    SDL_DestroyRenderer(win->renderer);
    SDL_DestroyWindow(win->window);
    win->closed = 1;
    printf("[GUI] Window destroyed: ID=%ld\n", id);
}

// ===========================
// Widget Management
// ===========================

long gui_button_create(long window_id, const char* text, long x, long y, long w, long h) {
    if (widget_count >= MAX_WIDGETS) {
        fprintf(stderr, "[GUI] Maximum widgets reached\n");
        return -1;
    }

    Widget* btn = &widgets[widget_count];
    btn->id = widget_count + 100; // Start button IDs at 100
    btn->type = WIDGET_BUTTON;
    btn->window_id = window_id;
    btn->rect.x = x;
    btn->rect.y = y;
    btn->rect.w = w;
    btn->rect.h = h;
    strncpy(btn->text, text, sizeof(btn->text) - 1);
    btn->visible = 1;
    btn->is_hovered = 0;
    btn->is_pressed = 0;

    widget_count++;
    printf("[GUI] Button created: '%s' at (%ld, %ld) size %ldx%ld ID=%ld\n",
           text, x, y, w, h, btn->id);
    return btn->id;
}

long gui_label_create(long window_id, const char* text, long x, long y) {
    if (widget_count >= MAX_WIDGETS) {
        fprintf(stderr, "[GUI] Maximum widgets reached\n");
        return -1;
    }

    Widget* lbl = &widgets[widget_count];
    lbl->id = widget_count + 200; // Start label IDs at 200
    lbl->type = WIDGET_LABEL;
    lbl->window_id = window_id;
    lbl->rect.x = x;
    lbl->rect.y = y;
    lbl->rect.w = strlen(text) * 8; // Approximate width
    lbl->rect.h = 20;
    strncpy(lbl->text, text, sizeof(lbl->text) - 1);
    lbl->visible = 1;

    widget_count++;
    printf("[GUI] Label created: '%s' at (%ld, %ld) ID=%ld\n", text, x, y, lbl->id);
    return lbl->id;
}

void gui_label_set_text(long label_id, const char* text) {
    Widget* lbl = find_widget(label_id);
    if (!lbl || lbl->type != WIDGET_LABEL) return;

    strncpy(lbl->text, text, sizeof(lbl->text) - 1);
    lbl->rect.w = strlen(text) * 8; // Update width
    printf("[GUI] Label text updated: ID=%ld text='%s'\n", label_id, text);
}

// ===========================
// Canvas Functions
// ===========================

long gui_canvas_create(long window_id, long x, long y, long w, long h) {
    if (widget_count >= MAX_WIDGETS) return -1;

    Widget* canvas = &widgets[widget_count];
    canvas->id = widget_count + 300; // Start canvas IDs at 300
    canvas->type = WIDGET_CANVAS;
    canvas->window_id = window_id;
    canvas->rect.x = x;
    canvas->rect.y = y;
    canvas->rect.w = w;
    canvas->rect.h = h;
    canvas->visible = 1;

    WindowInfo* win = find_window(window_id);
    if (win) {
        canvas->renderer = win->renderer;
    }

    canvas->draw_color.r = 0;
    canvas->draw_color.g = 0;
    canvas->draw_color.b = 0;
    canvas->draw_color.a = 255;

    widget_count++;
    printf("[GUI] Canvas created: at (%ld, %ld) size %ldx%ld ID=%ld\n",
           x, y, w, h, canvas->id);
    return canvas->id;
}

void gui_canvas_clear(long canvas_id, long r, long g, long b) {
    Widget* canvas = find_widget(canvas_id);
    if (!canvas || canvas->type != WIDGET_CANVAS) return;

    SDL_SetRenderDrawColor(canvas->renderer, r, g, b, 255);
    SDL_RenderClear(canvas->renderer);
}

void gui_canvas_set_color(long canvas_id, long r, long g, long b) {
    Widget* canvas = find_widget(canvas_id);
    if (!canvas || canvas->type != WIDGET_CANVAS) return;

    canvas->draw_color.r = r;
    canvas->draw_color.g = g;
    canvas->draw_color.b = b;
    SDL_SetRenderDrawColor(canvas->renderer, r, g, b, 255);
}

void gui_canvas_draw_rect(long canvas_id, long x, long y, long w, long h) {
    Widget* canvas = find_widget(canvas_id);
    if (!canvas || canvas->type != WIDGET_CANVAS) return;

    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(canvas->renderer, &rect);
}

void gui_canvas_draw_circle(long canvas_id, long cx, long cy, long radius) {
    Widget* canvas = find_widget(canvas_id);
    if (!canvas || canvas->type != WIDGET_CANVAS) return;

    // Simple circle drawing using midpoint algorithm
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(canvas->renderer, cx + dx, cy + dy);
            }
        }
    }
}

void gui_canvas_draw_line(long canvas_id, long x1, long y1, long x2, long y2) {
    Widget* canvas = find_widget(canvas_id);
    if (!canvas || canvas->type != WIDGET_CANVAS) return;

    SDL_RenderDrawLine(canvas->renderer, x1, y1, x2, y2);
}

void gui_canvas_render(long canvas_id) {
    Widget* canvas = find_widget(canvas_id);
    if (!canvas || canvas->type != WIDGET_CANVAS) return;

    WindowInfo* win = find_window(canvas->window_id);
    if (win) {
        SDL_RenderPresent(win->renderer);
    }
}

// ===========================
// Event Handling
// ===========================

long gui_poll_event() {
    SDL_Event event;

    // Reset current event
    current_event = GUI_EVENT_NONE;
    clicked_button_id = 0;

    // Process all pending events
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                current_event = GUI_EVENT_QUIT;
                printf("[GUI] Event: Quit\n");
                return current_event;

            case SDL_MOUSEBUTTONDOWN:
                mouse_x = event.button.x;
                mouse_y = event.button.y;

                // Check if mouse clicked on any button
                for (int i = 0; i < widget_count; i++) {
                    Widget* w = &widgets[i];
                    if (w->type != WIDGET_BUTTON || !w->visible) continue;

                    SDL_Rect* r = &w->rect;
                    if (mouse_x >= r->x && mouse_x <= (r->x + r->w) &&
                        mouse_y >= r->y && mouse_y <= (r->y + r->h)) {
                        w->is_pressed = 1;
                        clicked_button_id = w->id;
                        current_event = GUI_EVENT_BUTTON_CLICK;
                        printf("[GUI] Event: Button clicked ID=%ld\n", w->id);

                        // Re-render window
                        WindowInfo* win = find_window(w->window_id);
                        if (win) render_window(win);

                        return current_event;
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:
                // Reset all button states
                for (int i = 0; i < widget_count; i++) {
                    if (widgets[i].type == WIDGET_BUTTON) {
                        widgets[i].is_pressed = 0;
                    }
                }
                // Re-render all windows
                for (int i = 0; i < window_count; i++) {
                    render_window(&windows[i]);
                }
                break;

            case SDL_MOUSEMOTION:
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;

                // Update hover state
                int needs_render = 0;
                for (int i = 0; i < widget_count; i++) {
                    Widget* w = &widgets[i];
                    if (w->type != WIDGET_BUTTON || !w->visible) continue;

                    SDL_Rect* r = &w->rect;
                    int was_hovered = w->is_hovered;
                    w->is_hovered = (mouse_x >= r->x && mouse_x <= (r->x + r->w) &&
                                    mouse_y >= r->y && mouse_y <= (r->y + r->h));

                    if (was_hovered != w->is_hovered) {
                        needs_render = 1;
                    }
                }

                if (needs_render) {
                    for (int i = 0; i < window_count; i++) {
                        render_window(&windows[i]);
                    }
                }
                break;
        }
    }

    return current_event;
}

long gui_get_clicked_button() {
    return clicked_button_id;
}

// ===========================
// Utility Functions
// ===========================

char* int_to_string(long num) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld", num);
    return buffer;
}

// ===========================
// Cleanup
// ===========================

void gui_cleanup() {
    for (int i = 0; i < window_count; i++) {
        if (!windows[i].closed) {
            gui_window_destroy(windows[i].id);
        }
    }

    if (sdl_initialized) {
        SDL_Quit();
        printf("[GUI] SDL2 cleanup complete\n");
    }
}
