#include "menu_controller.h"

#include "handlers/all_screen_handlers.h"
#include "menu_state.h"

namespace Menu {

static const int nScreens = 1;

void handleIncDec(Event e, int max) {
    menuState.focus += (int)e;

    if (menuState.focus >= max) {
        menuState.focus = -1;
    }

    if (menuState.focus < -1) {
        menuState.focus = max - 1;
    }
}

void handleClickMainScreen() {}

void handleEvent(Event e) {
    switch (e) {
        case Event::INCREMENT:
        case Event::DECREMENT:
            if (!menuState.selected) {
                handleIncDec(e, handlers[menuState.screen]->getNSelections(menuState));
            } else {
                if (menuState.focus != -1) {
                    handlers[menuState.screen]->handleIncDec(e, menuState);
                } else {
                    menuState.screen += (int)e;
                    if (menuState.screen >= N_SCREENS) {
                        menuState.screen = 0;
                    }
                    if (menuState.screen < 0) {
                        menuState.screen = N_SCREENS - 1;
                    }
                }
            }
            break;
        case Event::CLICK:
            if (menuState.focus != -1) {
                handlers[menuState.screen]->handleClick(menuState);
            } else {
                menuState.selected = !menuState.selected;
            }
            break;
        default:
            break;
    }
}
}  // namespace Menu