#include "../../sensors/all_sensors.h"
#include "../../settings/settings.h"

namespace Menu {
void tareAndSave() {
    Sensors::compoundStrainGauge.tare();
    settings.strainGauge.offset = Sensors::strainGauge.getOffset();
    saveSettings();
}

void calibrateAndSave(float realWeight) {
    Sensors::strainGauge.calibrate(realWeight);
    settings.strainGauge.scale = Sensors::strainGauge.getScale();
    saveSettings();
}
}  // namespace Menu
