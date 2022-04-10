#ifndef A25D3828_A687_4431_95B7_B828DAF3AC9C
#define A25D3828_A687_4431_95B7_B828DAF3AC9C

#include "../../lib/gui/graphics/ssd1306_graphics.h"
#include "menu_event.h"
#include "menu_state.h"

namespace Menu {
    extern char buffer[50];
    void drawStringWRect(int pos, const char *str, int16_t x, int16_t y, int16_t width, int16_t height, GFX::SSD1306Graphics &g, MenuState &state);
}

#endif /* A25D3828_A687_4431_95B7_B828DAF3AC9C */
