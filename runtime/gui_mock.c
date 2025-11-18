/*
 * MLP GUI Mock Runtime
 *
 * This is a simple console-based GUI simulator
 * Replace with real SDL2/GTK/Raylib implementation later
 */

#include <stdio.h>
#include <string.h>

// Window management
long gui_window_create(const char* title, long width, long height) {
    printf("[GUI] Creating window: '%s' (%ldx%ld)\n", title, width, height);
    return 1; // Mock window ID
}

void gui_window_show(long id) {
    printf("[GUI] Showing window %ld\n", id);
}

void gui_window_hide(long id) {
    printf("[GUI] Hiding window %ld\n", id);
}

void gui_window_destroy(long id) {
    printf("[GUI] Destroying window %ld\n", id);
}

// Widgets
long gui_button_create(long window_id, const char* text, long x, long y, long w, long h) {
    printf("[GUI] Creating button in window %ld: '%s' at (%ld, %ld) size %ldx%ld\n",
           window_id, text, x, y, w, h);
    return 100; // Mock button ID
}

long gui_label_create(long window_id, const char* text, long x, long y) {
    printf("[GUI] Creating label in window %ld: '%s' at (%ld, %ld)\n",
           window_id, text, x, y);
    return 200; // Mock label ID
}

void gui_label_set_text(long label_id, const char* text) {
    printf("[GUI] Setting label %ld text to: '%s'\n", label_id, text);
}

// Events
#define GUI_EVENT_NONE 0
#define GUI_EVENT_QUIT 1
#define GUI_EVENT_BUTTON_CLICK 2

static int event_count = 0;

long gui_poll_event() {
    // Simulate some events then quit
    event_count++;

    if (event_count == 1) {
        printf("[GUI] Event: Button clicked\n");
        return GUI_EVENT_BUTTON_CLICK;
    } else if (event_count == 2) {
        printf("[GUI] Event: Button clicked again\n");
        return GUI_EVENT_BUTTON_CLICK;
    } else if (event_count == 3) {
        printf("[GUI] Event: Window close requested\n");
        return GUI_EVENT_QUIT;
    }

    return GUI_EVENT_NONE;
}

long gui_get_clicked_button() {
    return 100; // Mock: always return our button ID
}

// Canvas drawing
long gui_canvas_create(long window_id, long x, long y, long w, long h) {
    printf("[GUI] Creating canvas in window %ld at (%ld, %ld) size %ldx%ld\n",
           window_id, x, y, w, h);
    return 300; // Mock canvas ID
}

void gui_canvas_clear(long canvas_id, long r, long g, long b) {
    printf("[GUI] Clearing canvas %ld with color RGB(%ld, %ld, %ld)\n",
           canvas_id, r, g, b);
}

void gui_canvas_set_color(long canvas_id, long r, long g, long b) {
    printf("[GUI] Setting canvas %ld color to RGB(%ld, %ld, %ld)\n",
           canvas_id, r, g, b);
}

void gui_canvas_draw_rect(long canvas_id, long x, long y, long w, long h) {
    printf("[GUI] Drawing rectangle on canvas %ld at (%ld, %ld) size %ldx%ld\n",
           canvas_id, x, y, w, h);
}

void gui_canvas_draw_circle(long canvas_id, long x, long y, long radius) {
    printf("[GUI] Drawing circle on canvas %ld at (%ld, %ld) radius %ld\n",
           canvas_id, x, y, radius);
}

void gui_canvas_render(long canvas_id) {
    printf("[GUI] Rendering canvas %ld\n", canvas_id);
}

// Utility function to convert int to string
char* int_to_string(long num) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld", num);
    return buffer;
}
