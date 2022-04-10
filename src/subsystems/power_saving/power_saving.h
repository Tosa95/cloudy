#ifndef A15E5811_A96E_4F5C_99F2_81FCEDF4D8B8
#define A15E5811_A96E_4F5C_99F2_81FCEDF4D8B8

#include <Arduino.h>
#include "../../menu/gui_setup.h"

namespace Subsystems {

    class PowerSavingController {
        private:
            int64_t lastInteraction = 0;
            int64_t sleepAfter;
            int wakeupPin;
            int wakeupLevel;

        public:
            PowerSavingController(int64_t sleepAfter, int wakeupPin, int wakeupLevel = LOW): sleepAfter(sleepAfter), wakeupPin(wakeupPin), wakeupLevel(wakeupLevel) {}

            void init() {
                esp_sleep_enable_ext0_wakeup((gpio_num_t)wakeupPin, wakeupLevel);
                signalInteraction();
            }

            void signalInteraction() {
                lastInteraction = millis();
            }

            void update() {
                if (millis() - lastInteraction >= sleepAfter) {
                    g.turnOff();
                    esp_deep_sleep_start();
                }
            }

    };

}

#endif /* A15E5811_A96E_4F5C_99F2_81FCEDF4D8B8 */
