#include "picolib.h"


void init(void) {}

void update(void) {}

void draw(void) {
    print("Hello Picolib", 10, 10, 7);
}


int main(void) {
    picolib_init();
    picolib_run(init, update, draw);
    picolib_cleanup();
    return 0;
}