#include <Arduino.h>
#include "../../lib/math_utils/hysteresis.h"
#include <unity.h>

using namespace Hysteresis;

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

void test_hysteresis_starts_with_farthest_threshold(void) {
    HysteresisController<int> h(10, 20);
    TEST_ASSERT_EQUAL(HysteresisState::UNDEFINED, h.getState());
    h.update(11);
    TEST_ASSERT_EQUAL(HysteresisState::BELOW, h.getState());
    TEST_ASSERT_EQUAL(20, h.getCurrentThreshold());

    h = HysteresisController<int>(75, 112);
    TEST_ASSERT_EQUAL(HysteresisState::UNDEFINED, h.getState());
    h.update(120);
    TEST_ASSERT_EQUAL(HysteresisState::ABOVE, h.getState());
    TEST_ASSERT_EQUAL(75, h.getCurrentThreshold());

    h = HysteresisController<int>(-12, 27);
    TEST_ASSERT_EQUAL(HysteresisState::UNDEFINED, h.getState());
    h.update(-18);
    TEST_ASSERT_EQUAL(HysteresisState::BELOW, h.getState());
    TEST_ASSERT_EQUAL(27, h.getCurrentThreshold());
}

void test_hysteresis_cycle1(void) {
    HysteresisController<int> h(10, 20);
    h.update(5);
    TEST_ASSERT_EQUAL(20, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::BELOW, h.getState());

    h.update(9);
    TEST_ASSERT_EQUAL(20, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::BELOW, h.getState());

    h.update(15);
    TEST_ASSERT_EQUAL(20, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::BELOW, h.getState());

    h.update(21);
    TEST_ASSERT_EQUAL(10, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::ABOVE, h.getState());

    h.update(13);
    TEST_ASSERT_EQUAL(10, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::ABOVE, h.getState());

    h.update(7);
    TEST_ASSERT_EQUAL(20, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::BELOW, h.getState());

    h.update(20);
    TEST_ASSERT_EQUAL(10, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::ABOVE, h.getState());

    h.update(25);
    TEST_ASSERT_EQUAL(10, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::ABOVE, h.getState());

    h.update(27);
    TEST_ASSERT_EQUAL(10, h.getCurrentThreshold());
    TEST_ASSERT_EQUAL(HysteresisState::ABOVE, h.getState());
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_hysteresis_starts_with_farthest_threshold);
    RUN_TEST(test_hysteresis_cycle1);

    UNITY_END();
}

void loop() {

}