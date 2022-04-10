#include "all_sensors.h"
#include "../pinout/pinout.h"
#include "../settings/settings.h"

namespace Sensors {
    Sensors::StrainGauge strainGauge(Pinout::HX711_DAT, Pinout::HX711_CLK, 5);
    Sensors::Rfid rfid(Pinout::RC522_SS, Pinout::RC522_RESET);
    Sensors::CompoundStrainGauge compoundStrainGauge(strainGauge, rfid, settings.containers);
}