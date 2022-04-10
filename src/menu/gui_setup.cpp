#include "gui_setup.h"

GFX::SSD1306Graphics g(WIDTH, HEIGHT, 0x3C, HALF_RES, ROWS_IN_RAM, yieldFunc);

void initGFX() {
    g.init();
    delay(10);
    g.setDisplayRotation(DISPLAY_ROTATION);
    g.clear();
    g.show();
}