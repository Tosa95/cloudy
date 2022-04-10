#include <Arduino.h>
#include "../../lib/metrics/alerts/single_threshold_alert.h"
#include "../../lib/metrics/alerts/hysteresis_alert.h"
#include "../../lib/metrics/alerts/callbacks/thresholded_alert_callback.h"

#include "../../lib/metrics/test_support/stub_metric.h"



#include <unity.h>

template <class In, class Out>
class StubCallback: public Metrics::AlertCallback<In, Out> {
    public:
        In lastValue = -1;
        Metrics::Metric<In,Out> *lastMetric = nullptr;
        int callCount = 0;

        virtual void call(In currentValue, Metrics::Metric<In,Out> *metric) {
            lastValue = currentValue;
            lastMetric = metric;
            callCount++;
        }
};

void test_thresholded_alert(void) {
    
    Metrics::Test::StubMetric<int,int> inner;

    Metrics::SingleThresholdAlert<int,int> alert(&inner, 10);

    StubCallback<int,int> clbk;

    alert.addCallback(&clbk);

    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::UNDEFINED, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::UNDEFINED, alert.getOldState());


    alert.add(2);

    TEST_ASSERT_EQUAL(2, clbk.lastValue);
    TEST_ASSERT_EQUAL(1, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::UNDEFINED, alert.getOldState());

    alert.add(9);

    TEST_ASSERT_EQUAL(2, clbk.lastValue);
    TEST_ASSERT_EQUAL(1, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getOldState());


    alert.add(10);

    TEST_ASSERT_EQUAL(2, clbk.lastValue);
    TEST_ASSERT_EQUAL(1, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getOldState());


    alert.add(11);

    TEST_ASSERT_EQUAL(11, clbk.lastValue);
    TEST_ASSERT_EQUAL(2, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::ABOVE, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getOldState());


    alert.add(15);

    TEST_ASSERT_EQUAL(11, clbk.lastValue);
    TEST_ASSERT_EQUAL(2, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::ABOVE, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::ABOVE, alert.getOldState());


}

void test_hysteresis_alert(void) {
    
    Metrics::Test::StubMetric<int,int> inner;

    Metrics::HysteresisAlert<int,int> alert(&inner, 10, 30);

    StubCallback<int,int> clbk;

    alert.addCallback(&clbk);

    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::UNDEFINED, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::UNDEFINED, alert.getOldState());


    alert.add(2);

    TEST_ASSERT_EQUAL(2, clbk.lastValue);
    TEST_ASSERT_EQUAL(1, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::UNDEFINED, alert.getOldState());

    alert.add(15);

    TEST_ASSERT_EQUAL(2, clbk.lastValue);
    TEST_ASSERT_EQUAL(1, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getOldState());


    alert.add(29);

    TEST_ASSERT_EQUAL(2, clbk.lastValue);
    TEST_ASSERT_EQUAL(1, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getOldState());


    alert.add(31);

    TEST_ASSERT_EQUAL(31, clbk.lastValue);
    TEST_ASSERT_EQUAL(2, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::ABOVE, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getOldState());


    alert.add(15);

    TEST_ASSERT_EQUAL(31, clbk.lastValue);
    TEST_ASSERT_EQUAL(2, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::ABOVE, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::ABOVE, alert.getOldState());

    alert.add(9);

    TEST_ASSERT_EQUAL(9, clbk.lastValue);
    TEST_ASSERT_EQUAL(3, clbk.callCount);
    TEST_ASSERT_EQUAL(&alert, clbk.lastMetric);
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::BELOW, alert.getState());
    TEST_ASSERT_EQUAL(Metrics::ThresholdedAlertState::ABOVE, alert.getOldState());


}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_thresholded_alert);
    RUN_TEST(test_hysteresis_alert);

    UNITY_END();
}

void loop() {

}