#ifndef C957925F_BB19_4FA8_B4AC_8D85AEDE58CF
#define C957925F_BB19_4FA8_B4AC_8D85AEDE58CF

#include <MFRC522Debug.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522v2.h>

namespace Sensors {
    class Rfid {
        private:
            int resetPin;
            int nRetryPresence;
            int i = 0;

            MFRC522DriverPinSimple ssPin;  // Create pin driver. See typical pin layout above.

            SPIClass &spiClass = SPI;  // Alternative SPI e.g. SPI2 or from library e.g. softwarespi.

            const SPISettings spiSettings = SPISettings(SPI_CLOCK_DIV4, MSBFIRST,SPI_MODE0);  // May have to be set if hardware is not fully
                                                                                              // compatible to Arduino specifications.

            MFRC522DriverSPI driver;  // Create SPI driver.

            MFRC522 mfrc522;  // Create MFRC522 instance.

            bool cardPresent;
            bool tmpPresent;

            int16_t buffSize = 100;
            byte wabuff[100];

            char tmpBuff[10];

        public:
            
            Rfid(int ssPin, int resetPin, int nRetryPresence = 3): ssPin(ssPin), driver(this->ssPin, spiClass, spiSettings), mfrc522(driver), resetPin(resetPin), nRetryPresence(nRetryPresence) {}

            bool init() {
                pinMode(resetPin, OUTPUT);
                digitalWrite(resetPin, HIGH);
                delay(100);
                bool res = mfrc522.PCD_Init();
                MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);  // Show details of PCD - MFRC522 Card Reader details.
                return res;
            }

            void update() {

                if (mfrc522.PICC_IsNewCardPresent()) {
                    mfrc522.PICC_ReadCardSerial();
                    tmpPresent = true;
                } else if (tmpPresent) {
                    if (i == 0) {
                        tmpPresent = mfrc522.PICC_WakeupA(wabuff, (byte *)&buffSize) == MFRC522::StatusCode::STATUS_OK;
                    } else {
                        tmpPresent |= (mfrc522.PICC_WakeupA(wabuff, (byte *)&buffSize) == MFRC522::StatusCode::STATUS_OK);
                    }
                }

                i++;
                if (i > nRetryPresence) {
                    i = 0;
                    cardPresent = tmpPresent;
                }
                
            }

            bool isCardPresent() {
                return cardPresent;
            }

            void dumpUidToStr(char *buff) {
                if (cardPresent) {
                    int uidSize = mfrc522.uid.size;

                    strcpy(buff, "");

                    for (int i = 0; i < uidSize; i++) {
                        sprintf(tmpBuff, "%x", mfrc522.uid.uidByte[i]);
                        strcat(buff, tmpBuff);
                    }
                } else {
                    strcpy(buff, "none");
                }
            }
    };
}

#endif /* C957925F_BB19_4FA8_B4AC_8D85AEDE58CF */
