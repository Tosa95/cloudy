#ifndef D01CFBD0_A377_4A68_95AD_6ECB1E039FE5
#define D01CFBD0_A377_4A68_95AD_6ECB1E039FE5

#include "../settings/settings.h"
#include "rfid.h"
#include "strain_gauge.h"

namespace Sensors {
class CompoundStrainGauge {
   private:
    StrainGauge &strainGauge;
    Rfid &rfid;
    Containers &containers;
    char buffer[30];
    float containerTare = 0;
    int64_t tareAt = -1;

   public:
    CompoundStrainGauge(StrainGauge &strainGauge, Rfid &rfid,
                        Containers &containers)
        : strainGauge(strainGauge), rfid(rfid), containers(containers) {}

    float getWeigthNoContainerTare() {
        float weight = strainGauge.getWeight();

        if (rfid.isCardPresent()) {
            rfid.dumpUidToStr(buffer);

            ContainerData *cd = containers.getContainer(buffer);

            if (cd != nullptr) {
                weight -= cd->weight;
            }
        }

        return weight;
    }

    float getWeight() { return getWeigthNoContainerTare() - containerTare; }

    void tare() {
        tareAt = millis() + 1000;
    }

    void update() {
        strainGauge.update();
        rfid.update();

        if (!rfid.isCardPresent()) {
            containerTare = 0;
        }

        if (tareAt > 0 && millis() > tareAt) {
            if (!rfid.isCardPresent()) {
                strainGauge.tare();
            } else {
                containerTare = getWeigthNoContainerTare();
            }
            tareAt = -1;
        }
    }
};
}  // namespace Sensors

#endif /* D01CFBD0_A377_4A68_95AD_6ECB1E039FE5 */
