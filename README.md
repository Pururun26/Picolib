API

print([x],[y],[color]);
cls([color]);
camera([x],[y]);

Пример:

```
#include "pilib.h"

// INIT
int16_t x = 1;
int16_t y = 1;
int16_t z = 1;

void update(void)
{
    camera(z, -10);
    z--;
}

void draw(void)
{
    cls(12);
    print("HELLO PILIB!", x, y, 7);
}
```
