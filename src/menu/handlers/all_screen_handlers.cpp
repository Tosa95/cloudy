#include "all_screen_handlers.h"
#include "main_screen_handler.h"
#include "calibration_screen_handler.h"
#include "food_screen.h"

namespace Menu {
    ScreenHandler *handlers[N_SCREENS] = {
        new MainScreenHandler(),
        new FoodScreenHandler(0),
        new FoodScreenHandler(1),
        new FoodScreenHandler(2),
        new CalibrationScreenHandler()
    };
}