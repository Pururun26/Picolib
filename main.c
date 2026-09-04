#include "picolib.h"


void init(void) {}

void update(void) {
    if (btnp(4)) {
        tone(220, 10, 40, TONE_PULSE1);
    }
}

void draw(void) {
    // print("Hello Picolib", 10, 10, 7);
    // spr(1, 10, 10);
    line(10, 10, 20, 20, 7);
    oval(30, 30, 50, 50, 7);
    ovalfill(70, 70, 100, 10, 7);
}


int main(void) {
    picolib_init();
    picolib_run(init, update, draw);
    picolib_cleanup();
    return 0;
}