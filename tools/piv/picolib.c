#include "picolib.h"
#include <stdbool.h>
#include <stdio.h>
#include <raylib.h>

#define TARGET_WINDOW_SIZE 512

static RenderTexture2D target;

// Переменные для спрайт-листа (теперь они статические и скрыты внутри файла)
static Texture2D sprite_sheet;
static bool spritesheet_loaded = false;

static int16_t cam_x = 0;
static int16_t cam_y = 0;

static Color palette[16] = {
    {0x00, 0x00, 0x00, 0xFF}, // 0: black
    {0x1D, 0x2B, 0x53, 0xFF}, // 1: dark-blue
    {0x7E, 0x25, 0x53, 0xFF}, // 2: dark-purple
    {0x00, 0x87, 0x51, 0xFF}, // 3: dark-green
    {0xAB, 0x52, 0x36, 0xFF}, // 4: brown
    {0x5F, 0x57, 0x4F, 0xFF}, // 5: dark-grey
    {0xC2, 0xC3, 0xC7, 0xFF}, // 6: light-grey
    {0xFF, 0xF1, 0xE8, 0xFF}, // 7: white
    {0xFF, 0x00, 0x4D, 0xFF}, // 8: red
    {0xFF, 0xA3, 0x00, 0xFF}, // 9: orange
    {0xFF, 0xEC, 0x27, 0xFF}, // 10: yellow
    {0x00, 0xE4, 0x36, 0xFF}, // 11: green
    {0x29, 0xAD, 0xFF, 0xFF}, // 12: blue
    {0x83, 0x76, 0x9C, 0xFF}, // 13: lavender
    {0xFF, 0x77, 0xA8, 0xFF}, // 14: pink
    {0xFF, 0xCC, 0xAA, 0xFF}  // 15: light-peach
};


// --- 1. РЕАЛИЗАЦИЯ ЗАГРУЗКИ СПРАЙТ-ЛИСТА ---
void picolib_load_spritesheet(const char* filepath)
{
    sprite_sheet = LoadTexture(filepath);
    if (sprite_sheet.id != 0)
    {
        SetTextureFilter(sprite_sheet, TEXTURE_FILTER_POINT); // КРИТИЧНО для пиксель-арта!
        spritesheet_loaded = true;
    }
    else
    {
        // Если файл не найден, создаем безопасную заглушку (пурпурный квадрат 128x128), 
        // чтобы игра не упала с ошибкой сегментации.
        Image img = GenImageColor(128, 128, MAGENTA);
        sprite_sheet = LoadTextureFromImage(img);
        UnloadImage(img);
        SetTextureFilter(sprite_sheet, TEXTURE_FILTER_POINT);
        TraceLog(LOG_WARNING, "PICOLIB: Spritesheet '%s' not found. Using placeholder.", filepath);
        spritesheet_loaded = true;
    }
}

// --- ПЕРЕМЕННЫЕ ДЛЯ ШРИФТА ---
static Font pico_font;
static bool font_loaded = false;
static float font_size = 5.0f;    // Размер шрифта (5px идеально для сетки 128x128)
static float font_spacing = 1.0f; // Расстояние между буквами

// --- 1. РЕАЛИЗАЦИЯ ЗАГРУЗКИ ШРИФТА ---
void picolib_load_font(const char* filepath) {
    Image img = LoadImage(filepath);
    
    if (img.data != NULL) {
        // Загружаем шрифт из изображения. 
        // MAGENTA используется как цвет прозрачности (если фон не альфа-канал).
        // 32 - это ASCII код пробела (первый символ в сетке должен быть пробелом).
        pico_font = LoadFontFromImage(img, MAGENTA, 32);
        UnloadImage(img);
        font_loaded = true;
        TraceLog(LOG_INFO, "PICOLIB: Font '%s' loaded successfully.", filepath);
    } else {
        // Если файл не найден, используем стандартный шрифт Raylib в качестве запасного варианта
        pico_font = GetFontDefault();
        font_size = 10.0f; // Стандартный шрифт лучше смотрится в размере 10
        font_loaded = false;
        TraceLog(LOG_WARNING, "PICOLIB: Font file '%s' not found. Using default font.", filepath);
    }
}


// --- 2. КАМЕРА ---
picolib_vec2 camera(int16_t x, int16_t y)
{
    picolib_vec2 prev = {cam_x, cam_y};
    cam_x = x;
    cam_y = y;
    return prev;
}


// --- 3. РИСОВАНИЕ ---
void cls(uint8_t color)
{
    if (color < 16 )
    {
        ClearBackground(palette[color]);
    }
    else
    {
        ClearBackground(palette[0]);
    }
}

void print(const char *text, int16_t x, int16_t y, uint8_t color) {
    Color c = (color < 16) ? palette[color] : palette[7];
    
    // Позиция с учетом камеры
    Vector2 pos = { (float)(x - cam_x), (float)(y - cam_y) };
    
    // Используем DrawTextEx для отрисовки кастомного шрифта
    DrawTextEx(pico_font, text, pos, font_size, font_spacing, c);
}

void print_pro(const char* format, int16_t x, int16_t y, uint8_t color, ...) {
    va_list args;
    va_start(args, color); // последний именованный параметр перед ...
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    print(buffer, x, y, color);
}


// --- ПРИМИТИВЫ РИСОВАНИЯ ---
void circ(int16_t x, int16_t y, int16_t r, uint8_t color) {
    DrawCircleLines(x - cam_x, y - cam_y, (float)r, palette[color]);
}

void circfill(int16_t x, int16_t y, int16_t r, uint8_t color) {
    DrawCircle(x - cam_x, y - cam_y, (float)r, palette[color]);
}

void rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
    // МАГИЯ: x1 и y1 инклюзивны, поэтому добавляем 1 к ширине и высоте
    int16_t w = (x1 - x0) + 1;
    int16_t h = (y1 - y0) + 1;
    
    // Если ширина или высота получились <= 0, ничего не рисуем (защита от ошибок)
    if (w <= 0 || h <= 0) return;

    DrawRectangleLines(x0 - cam_x, y0 - cam_y, w, h, palette[color]);
}

void rectfill(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
    int16_t w = (x1 - x0) + 1;
    int16_t h = (y1 - y0) + 1;
    
    if (w <= 0 || h <= 0) return;

    DrawRectangle(x0 - cam_x, y0 - cam_y, w, h, palette[color]);
}


// --- 4. СПРАЙТЫ ---
void spr_pro(int16_t n, int16_t x, int16_t y, uint8_t w, uint8_t h, bool flip_x, bool flip_y) {
    if (!spritesheet_loaded) return;
    if (w == 0 || h == 0) return;

    int16_t col = n % 16; 
    int16_t row = n / 16;
    int16_t pixel_w = w * 8;
    int16_t pixel_h = h * 8;

    Rectangle src = {
        (float)(col * 8),
        (float)(row * 8),
        (float)pixel_w,
        (float)pixel_h
    };

    // ОТРАЖЕНИЕ ЧЕРЕЗ ОТРИЦАТЕЛЬНЫЕ РАЗМЕРЫ src
    if (flip_x) {
        src.width = -pixel_w;   // отражаем по X, x остаётся без изменений
    }
    if (flip_y) {
        src.height = -pixel_h;  // отражаем по Y, y остаётся без изменений
    }

    Rectangle dest = { 
        (float)(x - cam_x), 
        (float)(y - cam_y), 
        (float)pixel_w, 
        (float)pixel_h 
    };

    DrawTexturePro(sprite_sheet, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

// Простая версия (удобная обёртка)
void spr(int16_t n, int16_t x, int16_t y) {
    // Теперь передаем целые числа: 1 блок шириной, 1 блок высотой
    spr_pro(n, x, y, 1, 1, false, false);
}


// --- 5. API для ввода ---
bool btn(uint8_t id)
{
    switch (id)
    {
        case 0: return IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
        case 1: return IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
        case 2: return IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
        case 3: return IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
        case 4: return IsKeyDown(KEY_J) || IsKeyDown(KEY_Z);
        case 5: return IsKeyDown(KEY_K) || IsKeyDown(KEY_X);
        default: return false;
    }
}

bool btnp(uint8_t id)
{
    switch (id)
    {
        case 0: return IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT);
        case 1: return IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT);
        case 2: return IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
        case 3: return IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN);
        case 4: return IsKeyPressed(KEY_J) || IsKeyPressed(KEY_Z);
        case 5: return IsKeyPressed(KEY_K) || IsKeyPressed(KEY_X);
        default: return false;
    }
}


// --- 6. ГЛАВНЫЙ ЦИКЛ ---
int main(void)
{
    // Исправлено: переименовали переменную, чтобы не было конфликта имен (shadowing) позже
    int16_t initial_scale = 512 / PICOLIB_WIDTH;
    if (initial_scale < 1) initial_scale = 1;
    
    InitWindow(PICOLIB_WIDTH * initial_scale, PICOLIB_HEIGHT * initial_scale, PICOLIB_TITLE);
    SetTargetFPS(30);

    // ВАЖНО: Загружаем спрайт-лист, шрифт здесь! 
    picolib_load_spritesheet(PICOLIB_SS);
    picolib_load_font(PICOLIB_FONT);

    target = LoadRenderTexture(PICOLIB_WIDTH, PICOLIB_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose())
    {
        update();

        if (IsKeyPressed(KEY_F11)) ToggleFullscreen(); 

        BeginTextureMode(target);
        draw();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        int screenW = GetScreenWidth();
        int screenH = GetScreenHeight();

        // Вычисляем масштаб с сохранением пропорций
        float scaleX = (float)screenW / PICOLIB_WIDTH;
        float scaleY = (float)screenH / PICOLIB_HEIGHT;
        float current_scale = (scaleX < scaleY) ? scaleX : scaleY; // Переименовали в current_scale

        // Вычисляем смещение для центрирования
        int offsetX = (int)((screenW - PICOLIB_WIDTH * current_scale) / 2);
        int offsetY = (int)((screenH - PICOLIB_HEIGHT * current_scale) / 2);

        // Отрисовываем текстуру с центрированием
        DrawTexturePro(
            target.texture,
            (Rectangle){ 0, 0, (float)PICOLIB_WIDTH, (float)-PICOLIB_HEIGHT },
            (Rectangle){ (float)offsetX, (float)offsetY, PICOLIB_WIDTH * current_scale, PICOLIB_HEIGHT * current_scale },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
        EndDrawing();
    }
    
    // Очистка памяти при выходе
    UnloadRenderTexture(target);
    if (spritesheet_loaded) UnloadTexture(sprite_sheet);
    CloseWindow();

    return 0;
}
