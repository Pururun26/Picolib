#include "picolib.h"
#include <stdbool.h>
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
        TraceLog(LOG_WARNING, "PICOLIB: Файл '%s' не найден. Используется заглушка.", filepath);
        spritesheet_loaded = true;
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

void print(const char *text, int16_t x, int16_t y, uint8_t color)
{
    if (color < 16)
    {
        DrawText(text, x - cam_x, y - cam_y, 10, palette[color]);
    }
    else
    {
        DrawText(text, x - cam_x, y - cam_y, 10, palette[7]);
    }
}

// --- 4. СПРАЙТЫ ---

// Полная версия (основная логика)
void spr_pro(int16_t n, int16_t x, int16_t y, float w, float h, bool flip_x, bool flip_y) {
    if (!spritesheet_loaded) return;

    // 1. Вычисляем базовую позицию спрайта в сетке (0..255)
    int16_t col = n % 16; // 16 спрайтов в ряд (128 / 8)
    int16_t row = n / 16;

    // 2. Формируем исходный прямоугольник (в пикселях текстуры)
    Rectangle src = {
        (float)(col * 8),
        (float)(row * 8),
        (float)(w * 8),
        (float)(h * 8)
    };

    // 3. Магия отражения (Flip) через отрицательные размеры источника
    if (flip_x) {
        src.x += src.width;
        src.width = -src.width;
    }
    if (flip_y) {
        src.y += src.height;
        src.height = -src.height;
    }

    // 4. Формируем целевой прямоугольник на экране (с учетом камеры)
    // Размеры dest всегда положительные, отражение уже учтено в src
    Rectangle dest = {
        (float)(x - cam_x),
        (float)(y - cam_y),
        (float)(w * 8),
        (float)(h * 8)
    };

    // 5. Рисуем
    DrawTexturePro(sprite_sheet, src, dest, (Vector2){0, 0}, 0.0f, WHITE);
}

// Простая версия (удобная обёртка, которой не хватало)
void spr(int16_t n, int16_t x, int16_t y) {
    spr_pro(n, x, y, 1.0f, 1.0f, false, false);
}

// --- 5. ГЛАВНЫЙ ЦИКЛ ---
int main(void) {
    // Исправлено: переименовали переменную, чтобы не было конфликта имен (shadowing) позже
    int16_t initial_scale = 512 / PICOLIB_WIDTH;
    if (initial_scale < 1) initial_scale = 1;
    
    InitWindow(PICOLIB_WIDTH * initial_scale, PICOLIB_HEIGHT * initial_scale, PICOLIB_TITLE);
    SetTargetFPS(30);

    // ВАЖНО: Загружаем спрайт-лист здесь! 
    picolib_load_spritesheet(PICOLIB_SS);

    target = LoadRenderTexture(PICOLIB_WIDTH, PICOLIB_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    while (!WindowShouldClose()) {
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
    if (spritesheet_loaded) {
        UnloadTexture(sprite_sheet);
    }
    CloseWindow();

    return 0;
}
