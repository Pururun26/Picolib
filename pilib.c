#include "pilib.h"
#include <raylib.h>

#define TARGET_WINDOW_SIZE 512

// static int16_t screenWidth, screenHeight;
static RenderTexture2D target;

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

// --- Реализация камеры ---
pilib_vec2 camera(int16_t x, int16_t y) {
    pilib_vec2 prev = {cam_x, cam_y};
    cam_x = x;
    cam_y = y;
    return prev;
}

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

int main(void)
{
    int16_t scale = 512 / PILIB_WIDTH;
    if (scale < 1) scale = 1;
    InitWindow(PILIB_WIDTH * scale, PILIB_HEIGHT * scale, PILIB_TITLE);
    SetTargetFPS(30);

    target = LoadRenderTexture(PILIB_WIDTH, PILIB_HEIGHT);
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
        float scaleX = (float)screenW / PILIB_WIDTH;
        float scaleY = (float)screenH / PILIB_HEIGHT;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;

        // Вычисляем смещение для центрирования
        int offsetX = (int)((screenW - PILIB_WIDTH * scale) / 2);
        int offsetY = (int)((screenH - PILIB_HEIGHT * scale) / 2);

        // Отрисовываем текстуру с центрированием
        DrawTexturePro(
            target.texture,
            (Rectangle){ 0, 0, PILIB_WIDTH, -PILIB_HEIGHT }, // переворот по Y, если нужно
            (Rectangle){ (float)offsetX, (float)offsetY, PILIB_WIDTH * scale, PILIB_HEIGHT * scale },
            (Vector2){ 0, 0 },
            0.0f,
            WHITE
        );
        EndDrawing();
    }
    UnloadRenderTexture(target);
    CloseWindow();

    return 0;
}
