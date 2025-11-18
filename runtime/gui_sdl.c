/**
 * ===============================================
 * MLP SDL2 GUI Backend
 * ===============================================
 * Production-grade windowing and graphics support
 * using SDL2 library for cross-platform GUI applications
 *
 * Features:
 * - Window creation and management
 * - Event handling (keyboard, mouse, window events)
 * - 2D rendering with SDL2 renderer
 * - Basic shapes and text rendering
 * - Color management
 * - Frame timing and FPS control
 *
 * Dependencies:
 * - SDL2 (libsdl2-dev)
 * - SDL2_ttf for text rendering (optional)
 * - SDL2_image for image loading (optional)
 */

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ===============================================
// Type Definitions
// ===============================================

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool is_running;
    int width;
    int height;
    const char* title;
    Uint32 window_flags;
    int fps;
    Uint32 frame_delay;
} MLPWindow;

typedef struct {
    int x;
    int y;
} MLPPoint;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} MLPRect;

typedef struct {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} MLPColor;

typedef enum {
    MLP_EVENT_NONE = 0,
    MLP_EVENT_QUIT,
    MLP_EVENT_KEY_DOWN,
    MLP_EVENT_KEY_UP,
    MLP_EVENT_MOUSE_DOWN,
    MLP_EVENT_MOUSE_UP,
    MLP_EVENT_MOUSE_MOVE,
    MLP_EVENT_WINDOW_RESIZE
} MLPEventType;

typedef struct {
    MLPEventType type;
    union {
        struct {
            int key_code;
            bool shift;
            bool ctrl;
            bool alt;
        } key;
        struct {
            int x;
            int y;
            int button;
        } mouse;
        struct {
            int width;
            int height;
        } window;
    } data;
} MLPEvent;

// ===============================================
// Global State
// ===============================================

static MLPWindow* g_main_window = NULL;

// ===============================================
// Color Utilities
// ===============================================

MLPColor mlp_color_create(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    MLPColor color = {r, g, b, a};
    return color;
}

MLPColor mlp_color_rgb(Uint8 r, Uint8 g, Uint8 b) {
    return mlp_color_create(r, g, b, 255);
}

// Predefined colors
const MLPColor MLP_COLOR_BLACK = {0, 0, 0, 255};
const MLPColor MLP_COLOR_WHITE = {255, 255, 255, 255};
const MLPColor MLP_COLOR_RED = {255, 0, 0, 255};
const MLPColor MLP_COLOR_GREEN = {0, 255, 0, 255};
const MLPColor MLP_COLOR_BLUE = {0, 0, 255, 255};
const MLPColor MLP_COLOR_YELLOW = {255, 255, 0, 255};
const MLPColor MLP_COLOR_CYAN = {0, 255, 255, 255};
const MLPColor MLP_COLOR_MAGENTA = {255, 0, 255, 255};
const MLPColor MLP_COLOR_GRAY = {128, 128, 128, 255};

// ===============================================
// Window Management
// ===============================================

/**
 * Initialize SDL2 subsystems
 */
bool mlp_gui_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

/**
 * Create a new window
 */
MLPWindow* mlp_window_create(const char* title, int width, int height, int fps) {
    if (!mlp_gui_init()) {
        return NULL;
    }

    MLPWindow* window = (MLPWindow*)malloc(sizeof(MLPWindow));
    if (!window) {
        fprintf(stderr, "Failed to allocate window structure\n");
        return NULL;
    }

    window->title = title;
    window->width = width;
    window->height = height;
    window->fps = fps;
    window->frame_delay = 1000 / fps;
    window->is_running = true;
    window->window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    // Create SDL window
    window->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        window->window_flags
    );

    if (!window->window) {
        fprintf(stderr, "Window creation failed: %s\n", SDL_GetError());
        free(window);
        return NULL;
    }

    // Create SDL renderer with VSync
    window->renderer = SDL_CreateRenderer(
        window->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!window->renderer) {
        fprintf(stderr, "Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window->window);
        free(window);
        return NULL;
    }

    // Set blend mode for transparency
    SDL_SetRenderDrawBlendMode(window->renderer, SDL_BLENDMODE_BLEND);

    g_main_window = window;
    return window;
}

/**
 * Destroy window and cleanup
 */
void mlp_window_destroy(MLPWindow* window) {
    if (!window) return;

    if (window->renderer) {
        SDL_DestroyRenderer(window->renderer);
    }

    if (window->window) {
        SDL_DestroyWindow(window->window);
    }

    free(window);

    if (g_main_window == window) {
        g_main_window = NULL;
    }
}

/**
 * Check if window is running
 */
bool mlp_window_is_running(MLPWindow* window) {
    return window && window->is_running;
}

/**
 * Set window title
 */
void mlp_window_set_title(MLPWindow* window, const char* title) {
    if (window && window->window) {
        SDL_SetWindowTitle(window->window, title);
        window->title = title;
    }
}

/**
 * Get window size
 */
void mlp_window_get_size(MLPWindow* window, int* width, int* height) {
    if (window && window->window) {
        SDL_GetWindowSize(window->window, width, height);
    }
}

/**
 * Set window size
 */
void mlp_window_set_size(MLPWindow* window, int width, int height) {
    if (window && window->window) {
        SDL_SetWindowSize(window->window, width, height);
        window->width = width;
        window->height = height;
    }
}

// ===============================================
// Event Handling
// ===============================================

/**
 * Convert SDL key code to MLP key code
 */
static int sdl_to_mlp_keycode(SDL_Keycode key) {
    // Direct mapping for now
    return (int)key;
}

/**
 * Poll for events
 */
bool mlp_window_poll_event(MLPWindow* window, MLPEvent* event) {
    if (!window) return false;

    SDL_Event sdl_event;
    if (!SDL_PollEvent(&sdl_event)) {
        event->type = MLP_EVENT_NONE;
        return false;
    }

    SDL_Keymod mod = SDL_GetModState();

    switch (sdl_event.type) {
        case SDL_QUIT:
            event->type = MLP_EVENT_QUIT;
            window->is_running = false;
            break;

        case SDL_KEYDOWN:
            event->type = MLP_EVENT_KEY_DOWN;
            event->data.key.key_code = sdl_to_mlp_keycode(sdl_event.key.keysym.sym);
            event->data.key.shift = (mod & KMOD_SHIFT) != 0;
            event->data.key.ctrl = (mod & KMOD_CTRL) != 0;
            event->data.key.alt = (mod & KMOD_ALT) != 0;
            break;

        case SDL_KEYUP:
            event->type = MLP_EVENT_KEY_UP;
            event->data.key.key_code = sdl_to_mlp_keycode(sdl_event.key.keysym.sym);
            event->data.key.shift = (mod & KMOD_SHIFT) != 0;
            event->data.key.ctrl = (mod & KMOD_CTRL) != 0;
            event->data.key.alt = (mod & KMOD_ALT) != 0;
            break;

        case SDL_MOUSEBUTTONDOWN:
            event->type = MLP_EVENT_MOUSE_DOWN;
            event->data.mouse.x = sdl_event.button.x;
            event->data.mouse.y = sdl_event.button.y;
            event->data.mouse.button = sdl_event.button.button;
            break;

        case SDL_MOUSEBUTTONUP:
            event->type = MLP_EVENT_MOUSE_UP;
            event->data.mouse.x = sdl_event.button.x;
            event->data.mouse.y = sdl_event.button.y;
            event->data.mouse.button = sdl_event.button.button;
            break;

        case SDL_MOUSEMOTION:
            event->type = MLP_EVENT_MOUSE_MOVE;
            event->data.mouse.x = sdl_event.motion.x;
            event->data.mouse.y = sdl_event.motion.y;
            break;

        case SDL_WINDOWEVENT:
            if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED) {
                event->type = MLP_EVENT_WINDOW_RESIZE;
                event->data.window.width = sdl_event.window.data1;
                event->data.window.height = sdl_event.window.data2;
                window->width = event->data.window.width;
                window->height = event->data.window.height;
            } else {
                event->type = MLP_EVENT_NONE;
            }
            break;

        default:
            event->type = MLP_EVENT_NONE;
            break;
    }

    return event->type != MLP_EVENT_NONE;
}

// ===============================================
// Drawing Functions
// ===============================================

/**
 * Clear screen with color
 */
void mlp_clear(MLPWindow* window, MLPColor color) {
    if (!window || !window->renderer) return;

    SDL_SetRenderDrawColor(window->renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(window->renderer);
}

/**
 * Set draw color
 */
void mlp_set_draw_color(MLPWindow* window, MLPColor color) {
    if (!window || !window->renderer) return;
    SDL_SetRenderDrawColor(window->renderer, color.r, color.g, color.b, color.a);
}

/**
 * Draw a point
 */
void mlp_draw_point(MLPWindow* window, int x, int y, MLPColor color) {
    if (!window || !window->renderer) return;

    mlp_set_draw_color(window, color);
    SDL_RenderDrawPoint(window->renderer, x, y);
}

/**
 * Draw a line
 */
void mlp_draw_line(MLPWindow* window, int x1, int y1, int x2, int y2, MLPColor color) {
    if (!window || !window->renderer) return;

    mlp_set_draw_color(window, color);
    SDL_RenderDrawLine(window->renderer, x1, y1, x2, y2);
}

/**
 * Draw a rectangle outline
 */
void mlp_draw_rect(MLPWindow* window, MLPRect rect, MLPColor color) {
    if (!window || !window->renderer) return;

    SDL_Rect sdl_rect = {rect.x, rect.y, rect.width, rect.height};
    mlp_set_draw_color(window, color);
    SDL_RenderDrawRect(window->renderer, &sdl_rect);
}

/**
 * Draw a filled rectangle
 */
void mlp_fill_rect(MLPWindow* window, MLPRect rect, MLPColor color) {
    if (!window || !window->renderer) return;

    SDL_Rect sdl_rect = {rect.x, rect.y, rect.width, rect.height};
    mlp_set_draw_color(window, color);
    SDL_RenderFillRect(window->renderer, &sdl_rect);
}

/**
 * Draw a circle outline
 */
void mlp_draw_circle(MLPWindow* window, int cx, int cy, int radius, MLPColor color) {
    if (!window || !window->renderer) return;

    mlp_set_draw_color(window, color);

    // Midpoint circle algorithm
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SDL_RenderDrawPoint(window->renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(window->renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(window->renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(window->renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(window->renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(window->renderer, cx - y, cy - x);
        SDL_RenderDrawPoint(window->renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(window->renderer, cx + x, cy - y);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }

        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

/**
 * Draw a filled circle
 */
void mlp_fill_circle(MLPWindow* window, int cx, int cy, int radius, MLPColor color) {
    if (!window || !window->renderer) return;

    mlp_set_draw_color(window, color);

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x*x + y*y <= radius*radius) {
                SDL_RenderDrawPoint(window->renderer, cx + x, cy + y);
            }
        }
    }
}

/**
 * Present the rendered frame
 */
void mlp_present(MLPWindow* window) {
    if (!window || !window->renderer) return;
    SDL_RenderPresent(window->renderer);
}

/**
 * Frame timing - delay to maintain target FPS
 */
void mlp_frame_delay(MLPWindow* window, Uint32 frame_start) {
    if (!window) return;

    Uint32 frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < window->frame_delay) {
        SDL_Delay(window->frame_delay - frame_time);
    }
}

// ===============================================
// Utility Functions
// ===============================================

/**
 * Get current time in milliseconds
 */
Uint32 mlp_get_ticks(void) {
    return SDL_GetTicks();
}

/**
 * Cleanup SDL
 */
void mlp_gui_quit(void) {
    if (g_main_window) {
        mlp_window_destroy(g_main_window);
    }
    SDL_Quit();
}

// ===============================================
// High-level Helper Functions
// ===============================================

/**
 * Simple main loop helper
 */
void mlp_run_loop(MLPWindow* window,
                  void (*update)(MLPEvent* event),
                  void (*render)(MLPWindow* window)) {

    while (mlp_window_is_running(window)) {
        Uint32 frame_start = mlp_get_ticks();

        // Handle events
        MLPEvent event;
        while (mlp_window_poll_event(window, &event)) {
            if (update) {
                update(&event);
            }
        }

        // Render
        if (render) {
            render(window);
        }

        // Present
        mlp_present(window);

        // Frame timing
        mlp_frame_delay(window, frame_start);
    }
}

// ===============================================
// Example Usage (for testing)
// ===============================================

#ifdef MLP_GUI_SDL_TEST

void test_update(MLPEvent* event) {
    if (event->type == MLP_EVENT_KEY_DOWN) {
        if (event->data.key.key_code == SDLK_ESCAPE) {
            printf("Escape pressed - exiting\n");
        }
    }
}

void test_render(MLPWindow* window) {
    // Clear with dark gray
    mlp_clear(window, mlp_color_rgb(30, 30, 30));

    // Draw some shapes
    MLPRect rect = {100, 100, 200, 150};
    mlp_fill_rect(window, rect, MLP_COLOR_BLUE);

    mlp_draw_circle(window, 400, 300, 50, MLP_COLOR_RED);
    mlp_fill_circle(window, 600, 300, 40, MLP_COLOR_GREEN);

    mlp_draw_line(window, 50, 50, 750, 550, MLP_COLOR_YELLOW);
}

int main(void) {
    MLPWindow* window = mlp_window_create("MLP SDL2 Test", 800, 600, 60);

    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    printf("MLP SDL2 GUI Test\n");
    printf("Press ESC or close window to exit\n");

    mlp_run_loop(window, test_update, test_render);

    mlp_window_destroy(window);
    mlp_gui_quit();

    return 0;
}

#endif // MLP_GUI_SDL_TEST
