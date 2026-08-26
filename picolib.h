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

// --- Основные функции ---
void update(void);
void draw(void);

// --- API для рисования и ввода ---
void cls(uint8_t color);
void print(const char *text, int16_t x, int16_t y, uint8_t color);

// --- Спрайты ---
// Простая версия: рисует один спрайт 8x8 без отражений (самый частый кейс)
void spr(int16_t n, int16_t x, int16_t y);

// Полная версия (аналог DrawTexturePro): с масштабом, диапазоном и отражениями
void spr_pro(int16_t n, int16_t x, int16_t y, float w, float h, bool flip_x, bool flip_y);

// --- API для камеры ---
// Устанавливает смещение камеры и возвращает предыдущее значение.
// Чтобы сбросить камеру, вызовите camera(0, 0).
picolib_vec2 camera(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
