#define FORMAT_SPIFFS_IF_FAILED true
#include "fs.h"

#ifdef SDCARD_PRESENT           
    #include <Arduino.h>    
    #include "../sd/SD_MMC.h"           
#endif

#ifndef ESP32
    #include "LITTLEFS.h"
    #define FS_REF LittleFS
#else
    #include "SPIFFS.h"
    #define FS_REF SPIFFS
#endif

#include "../logging/logging.h"

namespace FileSystem {
    fs::FS *mainFS;
    fs::FS *externalFS;

    void init() {
        FS_REF.begin();
        mainFS = &FS_REF;

        #ifdef SDCARD_PRESENT
            externalFS = &SD_MMC;
            if(!SD_MMC.begin("/sdcard", true)){
                Logging::warn(0,F("SD Card Mount Failed"));
                externalFS = NULL;
            }
            
            uint8_t cardType = SD_MMC.cardType();
            if(cardType == CARD_NONE){
                Logging::warn(0,F("No SD Card attached"));
                externalFS = NULL;
            }
            if (externalFS != NULL) {
                SD_MMC.begin("/sdcard", true);
            }
        #else
            externalFS = NULL;
        #endif
    }
}