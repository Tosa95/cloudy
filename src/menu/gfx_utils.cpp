#include "gfx_utils.h"

namespace Menu {
    char buffer[50];
    void drawStringWRect(int pos, const char *str, int16_t x, int16_t y, int16_t width, int16_t height, GFX::SSD1306Graphics &g, MenuState &state) {
        g.drawStringCentered(x, y, width, height, (char*)str);
        if (state.focus == pos) {
            GFX::Rect<ADAGFX_TYPES> sz = g.getStringSize((char *)str);
            sz.h += 6;
            sz.w += 6;
            int16_t x_c = x + width/2;
            int16_t y_c = y + height/2;
            g.drawRect(x_c - sz.w/2, y_c - sz.h/2, sz.w, sz.h);

            if (state.selected) {
                 g.drawRect(x_c - sz.w/2 + 1, y_c - sz.h/2 + 1, sz.w - 2, sz.h - 2);
            }
        }
    }

}