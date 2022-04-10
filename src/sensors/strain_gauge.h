#ifndef CEFD5562_245E_4A98_9B15_E331D0A6C1F7
#define CEFD5562_245E_4A98_9B15_E331D0A6C1F7

#include <HX711.h>
#include <stdint.h>

namespace Sensors {

    class StrainGauge {
        private:
            int datPin;
            int sckPin;
            HX711 loadcell;

            int64_t offset = 0;
            float scale = 1;

            float currWeight = 0;
            int64_t currWeightRaw = 0;
            int64_t accumulator = 0;

            int nReadings = 5;
            int currReading = 0;
        
            void resetCounts() {
                this->accumulator = 0;
                this->currReading = 0;
            }

        public:
            StrainGauge(int datPin, int sckPin, int nReadings = 5): datPin(datPin), sckPin(sckPin), nReadings(nReadings) {}

            void init() {
                loadcell.begin(datPin, sckPin);
            }

            void setOffset(int64_t offset) {
                this->offset = offset;
                this->resetCounts();
            }

            void setScale(float scale) {
                this->scale = scale;
                this->resetCounts();
            }

            int64_t getOffset() {
                return this->offset;
            }

            int64_t getScale() {
                return this->scale;
            }

            bool update() {
                if (loadcell.wait_ready_timeout(100)) {
                    accumulator += loadcell.read();
                } else {
                    return false;
                }

                currReading++;

                if (currReading > nReadings) {
                    currWeightRaw = accumulator/nReadings;
                    currWeight = (currWeightRaw - offset) / scale;
                    currReading = 0;
                    accumulator = 0;
                }

                return true;
            }

            float getWeight() {
                return currWeight;
            }

            void tare() {
                setOffset(currWeightRaw);
            }

            void calibrate(float realWeight) {
                setScale((currWeightRaw - getOffset()) / realWeight);
            }
        
    };

}

#endif /* CEFD5562_245E_4A98_9B15_E331D0A6C1F7 */
