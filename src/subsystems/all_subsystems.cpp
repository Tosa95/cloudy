#include "all_subsystems.h"
#include "../pinout/pinout.h"

namespace Subsystems {
    PowerSavingController powerSavingController(5*60*1000, Pinout::ENCODER_CLICK);
}