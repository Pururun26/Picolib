#ifndef PILIB_H
#define PILIB_H

#include <stdint.h>
#include "pilib_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

// Простая структура для возврата координат (аналог tuple в Lua)
typedef struct {
    int16_t x;
    int16_t y;
} pilib_vec2;

// --- Основные функции ---
void update(void);
void draw(void);

// --- API для рисования и ввода ---
void cls(uint8_t color);
void print(const char *text, int16_t x, int16_t y, uint8_t color);

// --- API для камеры ---
// Устанавливает смещение камеры и возвращает предыдущее значение.
// Чтобы сбросить камеру, вызовите camera(0, 0).
pilib_vec2 camera(int16_t x, int16_t y);

#ifdef __cplusplus
}
#endif

#endif
