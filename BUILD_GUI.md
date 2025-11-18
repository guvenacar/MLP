# MLP SDL2 GUI Backend - Build Guide

Bu rehber, MLP projelerinde SDL2 tabanlı GUI backend'in nasıl kullanılacağını açıklar.

## İçindekiler

1. [Gereksinimler](#gereksinimler)
2. [Kurulum](#kurulum)
3. [Derleme](#derleme)
4. [Kullanım](#kullanım)
5. [API Referansı](#api-referansı)
6. [Örnekler](#örnekler)

## Gereksinimler

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y libsdl2-dev
```

### macOS

```bash
brew install sdl2
```

### Windows (MSYS2/MinGW)

```bash
pacman -S mingw-w64-x86_64-SDL2
```

### Versiyon Gereksinimleri

- SDL2 >= 2.0.5
- GCC >= 4.9 veya Clang >= 3.5
- Make >= 3.81

## Kurulum

SDL2 kütüphanesinin kurulu olduğunu doğrulayın:

```bash
sdl2-config --version
```

Eğer `sdl2-config` komutu bulunamazsa, SDL2'yi yukarıdaki komutlarla kurun.

## Derleme

### 1. Basit Test Programı

SDL2 GUI backend'in çalıştığını test etmek için:

```bash
cd runtime
gcc -DMLP_GUI_SDL_TEST gui_sdl.c -o gui_test $(sdl2-config --cflags --libs) -lm
./gui_test
```

Bu komut bir test penceresi açacak ve çeşitli şekiller çizecektir.

### 2. MLP Projesi ile Kullanım

MLP seed compiler'ı GUI desteği ile kullanmak için:

```bash
python3 stage0/seed-compiler.py your-program.mlp --gui=sdl2 -o output
```

veya

```bash
./mlpc your-program.mlp --gui=sdl2
```

### 3. Manuel Derleme

Kendi C kodunuzla entegre etmek için:

```bash
# SDL2 ile derleme
gcc -c runtime/gui_sdl.c -o build/gui_sdl.o $(sdl2-config --cflags)

# Runtime ile birlikte link etme
gcc your-program.c build/runtime.o build/gui_sdl.o \
    -o your-program \
    $(sdl2-config --libs) -lm
```

## Kullanım

### Basit Pencere Örneği

```c
#include "runtime/gui_sdl.c"

int main(void) {
    // Pencere oluştur (800x600, 60 FPS)
    MLPWindow* window = mlp_window_create("Merhaba MLP", 800, 600, 60);

    if (!window) {
        return 1;
    }

    // Ana döngü
    while (mlp_window_is_running(window)) {
        MLPEvent event;

        // Olayları işle
        while (mlp_window_poll_event(window, &event)) {
            if (event.type == MLP_EVENT_KEY_DOWN) {
                printf("Key pressed: %d\n", event.data.key.key_code);
            }
        }

        // Ekranı temizle (siyah)
        mlp_clear(window, MLP_COLOR_BLACK);

        // Mavi bir dikdörtgen çiz
        MLPRect rect = {100, 100, 200, 150};
        mlp_fill_rect(window, rect, MLP_COLOR_BLUE);

        // Ekranı güncelle
        mlp_present(window);
    }

    // Temizlik
    mlp_window_destroy(window);
    mlp_gui_quit();

    return 0;
}
```

### Yüksek Seviyeli Loop Helper

Daha basit kullanım için `mlp_run_loop` helper fonksiyonunu kullanabilirsiniz:

```c
#include "runtime/gui_sdl.c"

void my_update(MLPEvent* event) {
    if (event->type == MLP_EVENT_MOUSE_DOWN) {
        printf("Mouse clicked at: %d, %d\n",
               event->data.mouse.x,
               event->data.mouse.y);
    }
}

void my_render(MLPWindow* window) {
    mlp_clear(window, mlp_color_rgb(30, 30, 40));

    // Yeşil bir daire çiz
    mlp_fill_circle(window, 400, 300, 50, MLP_COLOR_GREEN);

    // Sarı bir çizgi çiz
    mlp_draw_line(window, 0, 0, 800, 600, MLP_COLOR_YELLOW);
}

int main(void) {
    MLPWindow* window = mlp_window_create("GUI Demo", 800, 600, 60);

    if (!window) return 1;

    // Helper loop kullan
    mlp_run_loop(window, my_update, my_render);

    mlp_window_destroy(window);
    mlp_gui_quit();

    return 0;
}
```

## API Referansı

### Pencere Yönetimi

#### `mlp_gui_init()`
SDL2'yi başlatır. Otomatik olarak `mlp_window_create` tarafından çağrılır.

```c
bool mlp_gui_init(void);
```

#### `mlp_window_create()`
Yeni bir pencere oluşturur.

```c
MLPWindow* mlp_window_create(const char* title, int width, int height, int fps);
```

**Parametreler:**
- `title`: Pencere başlığı
- `width`: Pencere genişliği (piksel)
- `height`: Pencere yüksekliği (piksel)
- `fps`: Hedef FPS (örn: 60)

**Dönüş:** Pencere pointer'ı veya NULL (hata durumunda)

#### `mlp_window_destroy()`
Pencereyi yok eder ve kaynakları temizler.

```c
void mlp_window_destroy(MLPWindow* window);
```

#### `mlp_window_is_running()`
Pencerenin çalışıp çalışmadığını kontrol eder.

```c
bool mlp_window_is_running(MLPWindow* window);
```

#### `mlp_window_set_title()`
Pencere başlığını değiştirir.

```c
void mlp_window_set_title(MLPWindow* window, const char* title);
```

#### `mlp_window_get_size()` / `mlp_window_set_size()`
Pencere boyutunu alır/ayarlar.

```c
void mlp_window_get_size(MLPWindow* window, int* width, int* height);
void mlp_window_set_size(MLPWindow* window, int width, int height);
```

### Olay İşleme

#### `mlp_window_poll_event()`
Sonraki olayı alır.

```c
bool mlp_window_poll_event(MLPWindow* window, MLPEvent* event);
```

**Olay Tipleri:**
- `MLP_EVENT_NONE`: Olay yok
- `MLP_EVENT_QUIT`: Pencere kapatma
- `MLP_EVENT_KEY_DOWN`: Tuş basıldı
- `MLP_EVENT_KEY_UP`: Tuş bırakıldı
- `MLP_EVENT_MOUSE_DOWN`: Fare tıklandı
- `MLP_EVENT_MOUSE_UP`: Fare bırakıldı
- `MLP_EVENT_MOUSE_MOVE`: Fare hareket etti
- `MLP_EVENT_WINDOW_RESIZE`: Pencere boyutu değişti

### Çizim Fonksiyonları

#### `mlp_clear()`
Ekranı belirtilen renkle temizler.

```c
void mlp_clear(MLPWindow* window, MLPColor color);
```

#### `mlp_draw_point()`
Tek bir nokta çizer.

```c
void mlp_draw_point(MLPWindow* window, int x, int y, MLPColor color);
```

#### `mlp_draw_line()`
Çizgi çizer.

```c
void mlp_draw_line(MLPWindow* window, int x1, int y1, int x2, int y2, MLPColor color);
```

#### `mlp_draw_rect()` / `mlp_fill_rect()`
Dikdörtgen çizer (çerçeve veya dolu).

```c
void mlp_draw_rect(MLPWindow* window, MLPRect rect, MLPColor color);
void mlp_fill_rect(MLPWindow* window, MLPRect rect, MLPColor color);
```

#### `mlp_draw_circle()` / `mlp_fill_circle()`
Daire çizer (çerçeve veya dolu).

```c
void mlp_draw_circle(MLPWindow* window, int cx, int cy, int radius, MLPColor color);
void mlp_fill_circle(MLPWindow* window, int cx, int cy, int radius, MLPColor color);
```

#### `mlp_present()`
Render edilen frame'i ekrana sunar.

```c
void mlp_present(MLPWindow* window);
```

### Renk Yönetimi

#### `mlp_color_create()` / `mlp_color_rgb()`
Renk oluşturur.

```c
MLPColor mlp_color_create(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
MLPColor mlp_color_rgb(Uint8 r, Uint8 g, Uint8 b);  // Alpha = 255
```

**Hazır Renkler:**
- `MLP_COLOR_BLACK`
- `MLP_COLOR_WHITE`
- `MLP_COLOR_RED`
- `MLP_COLOR_GREEN`
- `MLP_COLOR_BLUE`
- `MLP_COLOR_YELLOW`
- `MLP_COLOR_CYAN`
- `MLP_COLOR_MAGENTA`
- `MLP_COLOR_GRAY`

### Yardımcı Fonksiyonlar

#### `mlp_run_loop()`
Basitleştirilmiş ana döngü helper'ı.

```c
void mlp_run_loop(MLPWindow* window,
                  void (*update)(MLPEvent* event),
                  void (*render)(MLPWindow* window));
```

#### `mlp_get_ticks()`
SDL başlangıcından bu yana geçen milisaniyeyi döndürür.

```c
Uint32 mlp_get_ticks(void);
```

#### `mlp_gui_quit()`
SDL'i temizler ve kapatır.

```c
void mlp_gui_quit(void);
```

## Örnekler

### Örnek 1: Basit Animasyon

```c
#include "runtime/gui_sdl.c"

int circle_x = 100;
int circle_y = 300;
int velocity_x = 5;

void update(MLPEvent* event) {
    // Animasyon mantığı
    circle_x += velocity_x;

    // Sınır kontrolü
    if (circle_x > 750 || circle_x < 50) {
        velocity_x = -velocity_x;
    }
}

void render(MLPWindow* window) {
    mlp_clear(window, MLP_COLOR_BLACK);
    mlp_fill_circle(window, circle_x, circle_y, 30, MLP_COLOR_CYAN);
}

int main(void) {
    MLPWindow* window = mlp_window_create("Bouncing Ball", 800, 600, 60);
    if (!window) return 1;

    mlp_run_loop(window, update, render);

    mlp_window_destroy(window);
    mlp_gui_quit();
    return 0;
}
```

### Örnek 2: Fare ile Çizim

```c
#include "runtime/gui_sdl.c"

bool is_drawing = false;
int last_x = 0, last_y = 0;

void update(MLPEvent* event) {
    if (event->type == MLP_EVENT_MOUSE_DOWN) {
        is_drawing = true;
        last_x = event->data.mouse.x;
        last_y = event->data.mouse.y;
    }
    else if (event->type == MLP_EVENT_MOUSE_UP) {
        is_drawing = false;
    }
}

void render(MLPWindow* window) {
    // Ekranı temizleme - sadece başlangıçta
    static bool first = true;
    if (first) {
        mlp_clear(window, MLP_COLOR_WHITE);
        first = false;
    }

    // Çizim
    MLPEvent event;
    if (is_drawing && mlp_window_poll_event(window, &event)) {
        if (event.type == MLP_EVENT_MOUSE_MOVE) {
            mlp_draw_line(window, last_x, last_y,
                         event.data.mouse.x, event.data.mouse.y,
                         MLP_COLOR_BLACK);
            last_x = event.data.mouse.x;
            last_y = event.data.mouse.y;
        }
    }
}

int main(void) {
    MLPWindow* window = mlp_window_create("Paint App", 800, 600, 60);
    if (!window) return 1;

    mlp_run_loop(window, update, render);

    mlp_window_destroy(window);
    mlp_gui_quit();
    return 0;
}
```

## Sorun Giderme

### SDL2 bulunamadı

**Hata:** `SDL.h: No such file or directory`

**Çözüm:**
```bash
# SDL2'nin kurulu olduğunu doğrulayın
sdl2-config --cflags

# Yoksa kurun
sudo apt-get install libsdl2-dev
```

### Link hatası

**Hata:** `undefined reference to SDL_*`

**Çözüm:** SDL2 kütüphanesini link edin:
```bash
gcc program.c -o program $(sdl2-config --libs)
```

### Pencere açılmıyor

**Kontroller:**
1. Display server çalışıyor mu? (X11/Wayland)
2. SDL2 doğru yüklendi mi?
3. `mlp_gui_init()` başarılı döndü mü?

### Düşük FPS

**Çözümler:**
1. VSync devre dışı bırakın: Renderer oluşturulurken `SDL_RENDERER_PRESENTVSYNC` flag'ini kaldırın
2. Çizim sayısını azaltın
3. Hardware acceleration kontrol edin

## Gelecek Özellikler

- [ ] SDL2_ttf entegrasyonu (text rendering)
- [ ] SDL2_image entegrasyonu (resim yükleme)
- [ ] SDL2_mixer entegrasyonu (ses)
- [ ] Texture desteği
- [ ] Sprite rendering
- [ ] Gelişmiş input handling (gamepad)
- [ ] Multi-window desteği

## Katkıda Bulunma

GUI backend'e katkıda bulunmak için:

1. `runtime/gui_sdl.c` dosyasını düzenleyin
2. Değişikliklerinizi test edin
3. Pull request gönderin

## Lisans

MLP projesi ile aynı lisans altındadır. Detaylar için `LICENSE` dosyasına bakın.

---

**Son güncelleme:** 2025-01-18
**Versiyon:** 1.0.0
