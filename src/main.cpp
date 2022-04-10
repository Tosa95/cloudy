#include <Arduino.h>
#include <HX711.h>

#include "menu/menu_and_settings.h"
#include "pinout/pinout.h"
#include "sensors/rfid.h"
#include "sensors/strain_gauge.h"
#include "../lib/btns/button_controller.h"
#include "../lib/btns/encoder_controller.h"
#include "settings/settings.h"
#include "fs/filesystem.h"
#include "sensors/all_sensors.h"
#include "subsystems/all_subsystems.h"

void onEncoderChange(Btns::EncoderDirection direction);

Btns::ButtonController encoderClick(Pinout::ENCODER_CLICK);
Btns::EncoderController encoderController(Pinout::ENCODER_A, Pinout::ENCODER_B, onEncoderChange, true);

void setup() {
    Serial.begin(115200);
    Serial.println();

    Subsystems::powerSavingController.init();
    FileSystem::init();

    loadSettings();
    Serial.println(settings.strainGauge.offset);

    Sensors::strainGauge.setOffset(settings.strainGauge.offset);
    Sensors::strainGauge.setScale(settings.strainGauge.scale);

    initGFX();
    Sensors::rfid.init();
    Sensors::strainGauge.init();
}

char buff[30] = "";
int16_t buffSize = 10;
byte wabuff[100];

void loop() {
    Menu::drawMenu();
    Sensors::compoundStrainGauge.update();

    if (encoderClick.getTransition() == Btns::BtnTransition::CHANGED_TO_LOW) {
        Menu::handleEvent(Menu::Event::CLICK);
        Subsystems::powerSavingController.signalInteraction();
    }

    Subsystems::powerSavingController.update();
}

void yieldFunc() {}

void onEncoderChange(Btns::EncoderDirection direction) {
    Subsystems::powerSavingController.signalInteraction();

    switch (direction)
    {
    case Btns::EncoderDirection::INCREMENT:
        Menu::handleEvent(Menu::Event::INCREMENT);
        break;

    case Btns::EncoderDirection::DECREMENT:
        Menu::handleEvent(Menu::Event::DECREMENT);
        break;
    
    default:
        break;
    }
}