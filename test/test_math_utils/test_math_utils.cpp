#include <Arduino.h>
#include <unity.h>
#include <stdint.h>

#include "../../lib/math_utils/poly.h"

void test_polyval() {
    Poly::Poly<float> p {.start = 0, .end = 10, .deg = 2, .coeffs = new float[3] {2, 1, 1}};

    TEST_ASSERT_FLOAT_WITHIN(0.0001, 4.0, p.eval(1));
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 11.0, p.eval(2));
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 2.0, p.eval(0.5));
}

void test_polymap() {
    Poly::Poly<float> p1 {.start = 0, .end = 10, .deg = 2, .coeffs = new float[3] {2, 1, 1}};
    Poly::Poly<float> p2 {.start = 10, .end = 20, .deg = 2, .coeffs = new float[3] {1, 0, 0}};

    Poly::DirectMemoryPolyMap<float> pm (
        new Poly::Poly<float> [2] {
            Poly::Poly<float> {.start = 0, .end = 10, .deg = 2, .coeffs = new float[3] {2, 1, 1}},
            Poly::Poly<float> {.start = 10, .end = 20, .deg = 2, .coeffs = new float[3] {1, 0, 0}}
        },
        2
    );


    TEST_ASSERT_FLOAT_WITHIN(0.0001, 4.0, pm.eval(1));
    TEST_ASSERT_TRUE(pm.canEval(1));
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 11.0, pm.eval(2));
    TEST_ASSERT_TRUE(pm.canEval(2));
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 2.0, pm.eval(0.5));
    TEST_ASSERT_TRUE(pm.canEval(0.5));

    TEST_ASSERT_FLOAT_WITHIN(0.0001, 121, pm.eval(11));
    TEST_ASSERT_TRUE(pm.canEval(11));
    TEST_ASSERT_FLOAT_WITHIN(0.0001, 225, pm.eval(15));
    TEST_ASSERT_TRUE(pm.canEval(15));

    TEST_ASSERT_FALSE(pm.canEval(21));
    TEST_ASSERT_FALSE(pm.canEval(-5));

}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(3000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_polyval);
    RUN_TEST(test_polymap);
    
    UNITY_END();
}

void loop() {

}