#include <Arduino.h>
#include <unity.h>
#include <stdint.h>

#include "../../lib/ntc/ntc.h"
#include "../../lib/ntc/data/approximations/ntc_100k_b3950.h"

void test_ntc() {

    NTC::NTC_100K_B3950::init();

    Poly::PolyMap<float> *pm = NTC::NTC_100K_B3950::map;

    NTC::NTC<float> ntc(A0, 100.0, pm);

    TEST_ASSERT_FLOAT_WITHIN(0.1, 10, ntc.getTemp(198.530));
    TEST_ASSERT_FLOAT_WITHIN(0.1, 122, ntc.getTemp(3.455));
    TEST_ASSERT_FLOAT_WITHIN(0.1, -45, ntc.getTemp(4877.200));
    TEST_ASSERT_FLOAT_WITHIN(0.01, 100.2, ntc.computeCurrentR(512));
    TEST_ASSERT_FLOAT_WITHIN(0.1, 25.0, ntc.read(512));

    Serial.println(ntc.computeCurrentR(512));
}


void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(3000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_ntc);
    
    UNITY_END();
}

void loop() {

}