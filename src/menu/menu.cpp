#include "menu.h"

#include "gui_setup.h"
#include "menu_state.h"
#include "handlers/all_screen_handlers.h"

namespace Menu {


    void drawScreenCorner() {
        if (menuState.focus == -1) {
            if (menuState.selected) {
                g.drawRect(1, 1, WIDTH-2, HEIGHT-2);
            }
            g.drawRect(0, 0, WIDTH, HEIGHT);
        }
    }

    void mainGFXFunc() {
        g.clear();

        handlers[menuState.screen]->draw(g, menuState);

        drawScreenCorner();
        g.show();
    }

    void drawMenu() {
        for (int i = 0; i < DRAW_CYCLES; i++) {
            mainGFXFunc();
        }
    }
}  // namespace Menu