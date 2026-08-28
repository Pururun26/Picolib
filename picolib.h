#ifndef PICOLIB_H
#define PICOLIB_H

#include <stdint.h>
#include <stdbool.h>
#include "picolib_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

// Простая структура для возврата координат (аналог tuple в Lua)
typedef struct {
    int16_t x;
    int16_t y;
} picolib_vec2;

// --- Инициализация ---
void picolib_load_spritesheet(const char* filepath); // Загрузка спрайт-листа
void picolib_load_font(const char* filepath);
// Загружает все звуки из папки "sounds/" по именам файлов "0.wav", "1.wav" и т.д.
void picolib_load_sounds();

// --- Основные функции ---
void update(void);
void draw(void);

// --- API для рисования ---
void cls(uint8_t color);
void print(const char* format, int16_t x, int16_t y, uint8_t color, ...);
void circ(int16_t x, int16_t y, int16_t r, uint8_t color);
void circfill(int16_t x, int16_t y, int16_t r, uint8_t color);
void rect(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void rectfill(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);

// --- Спрайты ---
// Простая версия: рисует один спрайт 8x8
void spr(int16_t n, int16_t x, int16_t y);

// Полная версия: w и h теперь указывают КОЛИЧЕСТВО блоков 8x8 (а не пиксели!)
void spr_pro(int16_t n, int16_t x, int16_t y, uint8_t w, uint8_t h, bool flip_x, bool flip_y);

// --- API для ввода ---
bool btn(uint8_t id);
bool btnp(uint8_t id);

// --- API для камеры ---
// Устанавливает смещение камеры и возвращает предыдущее значение.
// Чтобы сбросить камеру, вызовите camera(0, 0).
picolib_vec2 camera(int16_t x, int16_t y);

// --- API для звука ---
// Проигрывает звук по индексу
void sfx(int index);

#ifdef __cplusplus
}
#endif

#endif
