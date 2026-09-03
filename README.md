```
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

void spr_scale(int16_t n, int16_t x, int16_t y, uint8_t zoom);

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

// --- API для карты .csv
void map_draw(int celx, int cely, int sx, int sy, int celw, int celh, uint8_t layer);
void map_full(void);
void map_full_layer(uint8_t layer);
uint8_t mget(int x, int y);
void mset(int x, int y, uint8_t id);

// --- API для сохранение .csv
void save(uint8_t pos, uint64_t value);
uint64_t load(uint8_t pos);

// --- Текстовые файлы ---
// Сохраняет текстовую строку в файл. Возвращает true при успехе, иначе false.
bool save_text(const char *fileName, const char *text);

// Загружает текстовый файл в память. Возвращает строку (выделенную память).
// Возвращает NULL, если файл не найден или ошибка.
char* load_text(const char* fileName);

// --- Бинарные файлы ---
// Сохраняет бинарные данные в файл. Возвращает true при успехе, иначе false.
bool save_data(const char *fileName, void *data, int bytesToWrite);

// Загружает бинарный файл в память. Размер файла записывается в *bytesRead.
// Возвращает указатель на массив байтов (выделенную память) или NULL при ошибке.
unsigned char* load_data(const char* fileName, int* bytesRead);

// --- Освобождение памяти ---
// Освобождает память, выделенную load_text().
void unload_text(char* text);

// Освобождает память, выделенную load_data().
void unload_data(unsigned char* data);


// --- API для столкновение ---
bool col_rect(Rect* a, Rect* b);



Пример:
#include "picolib.h"


void init(void) {}

void update(void) {
    if (btnp(4)) {
        tone(220, 10, 40, TONE_PULSE1);
    }
}

void draw(void) {
    print("Hello Picolib", 10, 10, 7);
    spr(1, 10, 10);
}


int main(void) {
    picolib_init();
    picolib_run(init, update, draw);
    picolib_cleanup();
    return 0;
}
```
