#ifndef FB8658B7_2334_4001_8835_965474B111EEA
#define FB8658B7_2334_4001_8835_965474B111EEA

#include "../../sensors/all_sensors.h"
#include "../../settings/settings.h"
#include "screen_handler.h"
#include "utils.h"

namespace Menu {

class CalibrationScreenHandler : public ScreenHandler {
   private:
    float realWeight = 318;

   public:
    virtual void handleClick(MenuState &state) {
        switch (state.focus) {
            case 0:
                state.selected = !state.selected;
                break;

            case 1:
                tareAndSave();
                break;

            case 2:
                calibrateAndSave(realWeight);
                break;
        }
    }
    virtual void handleIncDec(Event &e, MenuState &state) {
        switch (state.focus) {
            case 0:
                realWeight += (int)e * 0.1;
                break;

            default:
                break;
        }
    }
    virtual void draw(GFX::SSD1306Graphics &g, MenuState &state) {
        g.drawStringCentered(0, 4, WIDTH, 12, "CALIBRATION");

        sprintf(buffer, "measured: %.2fg", Sensors::strainGauge.getWeight());
        g.drawStringCentered(0, 16, WIDTH, 12, buffer);

        sprintf(buffer, "real: %.2fg", realWeight);
        drawStringWRect(0, buffer, 0, 28, WIDTH, 12, g, state);

        drawStringWRect(1, "TARE", 0, 40, WIDTH/2, 24, g, state);
        drawStringWRect(2, "CALIB", WIDTH/2, 40, WIDTH/2, 24, g, state);
    }
    virtual int getNSelections(MenuState &state) { return 3; }
};

}  // namespace Menu

#endif /* FB8658B7_2334_4001_8835_965474B111EEA */
