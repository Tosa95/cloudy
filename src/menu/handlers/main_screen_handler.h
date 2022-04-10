#ifndef FB8658B7_2334_4001_8835_965474B111EE
#define FB8658B7_2334_4001_8835_965474B111EE

#include "screen_handler.h"
#include "../../sensors/all_sensors.h"
#include "../../settings/settings.h"
#include "utils.h"

namespace Menu {

class MainScreenHandler : public ScreenHandler {
   public:
    virtual void handleClick(MenuState &state) {
        switch (state.focus) {
            case 0:
                tareAndSave();
                break;
            case 1:
                if (Sensors::rfid.isCardPresent()) {
                    Sensors::rfid.dumpUidToStr(buffer);

                    ContainerData *cd = settings.containers.getContainer(buffer);

                    if (cd == nullptr) {
                        cd = settings.containers.getFreeContainer();
                    }

                    cd->setId(buffer);
                    cd->weight = Sensors::strainGauge.getWeight();
                    Serial.println(cd->id);

                    saveSettings();
                }
                break;
        }
    }
    virtual void handleIncDec(Event &e, MenuState &state) {}
    virtual void draw(GFX::SSD1306Graphics &g, MenuState &state) {
        g.setColor(WHITE);
        g.setFontSize(2);

        sprintf(buffer, "%.2fg", Sensors::compoundStrainGauge.getWeight());
        g.drawStringCentered(0, 0, WIDTH, HEIGHT / 2, buffer);

        

        g.setFontSize(1);
        drawStringWRect(0, "TARE", 0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2, g,
                        state);
        drawStringWRect(1, "CONT", WIDTH / 2, HEIGHT / 2, WIDTH / 2, HEIGHT / 2,
                        g, state);

        if (Sensors::rfid.isCardPresent()) {
            Sensors::rfid.dumpUidToStr(buffer);
            g.drawStringCentered(0, 56, 128, 8, buffer);
        }
        
    }

    virtual int getNSelections(MenuState &state) {
        return 2;
    }
};

}  // namespace Menu

#endif /* FB8658B7_2334_4001_8835_965474B111EE */
