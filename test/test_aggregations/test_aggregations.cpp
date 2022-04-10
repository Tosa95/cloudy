#include <Arduino.h>
#include "../../lib/logging/test_support/test_log.h"
#include "../../lib/metrics/aggregation/aggregators.h"
#include "../../lib/metrics/aggregation/fixed_interval_aggregation_metric.h"
#include "../../lib/metrics/queue_based/queue_metric_fixed_interval.h"
#include "../../lib/metrics/value_and_timestamp.h"


#include <unity.h>

void test_min_folder(void) {
    
    Metrics::MinFolder<int> mf;

    TEST_ASSERT_EQUAL(5,mf.fold(5,624234,0));
    TEST_ASSERT_EQUAL(5,mf.fold(5,6,1));
    TEST_ASSERT_EQUAL(-1,mf.fold(5,-1,3));
    TEST_ASSERT_EQUAL(5,mf.fold(5,5,13));

}

void test_max_folder(void) {
    
    Metrics::MaxFolder<int> mf;

    TEST_ASSERT_EQUAL(5,mf.fold(5,63254,0));
    TEST_ASSERT_EQUAL(6,mf.fold(5,6,1));
    TEST_ASSERT_EQUAL(5,mf.fold(5,-1,3));
    TEST_ASSERT_EQUAL(5,mf.fold(5,5,13));

}

void test_avg_folder(void) {
    
    Metrics::AvgFolder<int, float> mf;

    TEST_ASSERT_EQUAL(4.25,mf.fold(2,5,3));
    TEST_ASSERT_EQUAL(3,mf.fold(3,1215641,0));

    Metrics::AvgFolder<int, int> mfi;

    TEST_ASSERT_EQUAL(4,mfi.fold(2,5,3));
    TEST_ASSERT_EQUAL(3,mfi.fold(3,1215641,0));

}

void test_folding_aggregator(void) {
    Metrics::AvgFolder<int, float> mf;
    Metrics::FoldingAggregator<int,float> agg(&mf);

    TEST_ASSERT_EQUAL(0,agg.get());
    agg.add(3);
    TEST_ASSERT_EQUAL(3,agg.get());
    agg.add(4);
    TEST_ASSERT_EQUAL(3.5,agg.get());
    agg.add(5);
    TEST_ASSERT_EQUAL(4.0,agg.get());

    agg.clear();

    TEST_ASSERT_EQUAL(0,agg.get());
    agg.add(3);
    TEST_ASSERT_EQUAL(3,agg.get());
    agg.add(4);
    TEST_ASSERT_EQUAL(3.5,agg.get());
    agg.add(5);
    TEST_ASSERT_EQUAL(4.0,agg.get());

    Metrics::MinFolder<int> minf;
    Metrics::FoldingAggregator<int,int> minagg(&minf);

    TEST_ASSERT_EQUAL(0,minagg.get());
    minagg.add(7);
    TEST_ASSERT_EQUAL(7,minagg.get());
    minagg.add(4);
    TEST_ASSERT_EQUAL(4,minagg.get());
    minagg.add(5);
    TEST_ASSERT_EQUAL(4,minagg.get());

    minagg.clear();

    TEST_ASSERT_EQUAL(0,minagg.get());
    minagg.add(9);
    TEST_ASSERT_EQUAL(9,minagg.get());
    minagg.add(14);
    TEST_ASSERT_EQUAL(9,minagg.get());
    minagg.add(5);
    TEST_ASSERT_EQUAL(5,minagg.get());

}

void test_time_decreasing_counter_aggregator(void) {
    Metrics::TimeDecreasingCounterAggregator<float, int> agg(1000);

    TEST_ASSERT_EQUAL(0, agg.get());

    agg.add(1);

    TEST_ASSERT_EQUAL(1, agg.get());

    agg.add(10);

    TEST_ASSERT_EQUAL(11, agg.get());

    delay(5);

    agg.add(0);

    TEST_ASSERT_EQUAL(6, agg.get());

    delay(4);

    agg.add(0);

    TEST_ASSERT_EQUAL(2, agg.get());

    delay(12);

    agg.add(0);

    TEST_ASSERT_EQUAL(0, agg.get());

}

static Time::millisecond_t currTs = 1;
void text_fixed_interval_aggregation_metric(void) {
    Metrics::AvgFolder<int, float> mf;
    Metrics::FoldingAggregator<int,float> agg(&mf);

    Time::millisecond_t interval = 25;

    Metrics::QueueMetricFixedInterval<float> baseMetric(
        10,
        interval
    );

    Metrics::FixedIntervalAggregationMetric<int, float> metric(
        &agg,
        interval,
        &baseMetric,
        [](){return currTs;},
        10
    );

    metric.add(5); // 1
    currTs += 10ULL;
    metric.add(6); // 11
    currTs += 10ULL;
    metric.add(7); // 21 
    currTs += 10ULL;
    metric.add(13); // 31 --> send up to 21 (26)
    currTs += 10ULL;
    metric.add(-2); // 41
    currTs += 10ULL;
    metric.add(11); // 51 --> send up to 41 (51 excluded!)
    currTs += 10ULL;
    metric.add(36); // 61
    currTs += 103ULL;
    metric.add(38); // 164

    Metrics::Iterator<float> *it = metric.query();

    Metrics::ValueAndTimestamp<float> *v;
    v = it->next();
    TEST_ASSERT_EQUAL_FLOAT(6.0,v->value);
    TEST_ASSERT_EQUAL(26,v->timestamp);

    v = it->next();
    TEST_ASSERT_EQUAL(5.5,v->value);
    TEST_ASSERT_EQUAL(51,v->timestamp);

    v = it->next();
    TEST_ASSERT_EQUAL(23.5,v->value);
    TEST_ASSERT_EQUAL(76,v->timestamp);

    v = it->next();
    TEST_ASSERT_EQUAL(23.5,v->value);
    TEST_ASSERT_EQUAL(101,v->timestamp);

    v = it->next();
    TEST_ASSERT_EQUAL(23.5,v->value);
    TEST_ASSERT_EQUAL(126,v->timestamp);

    v = it->next();
    TEST_ASSERT_EQUAL(23.5,v->value);
    TEST_ASSERT_EQUAL(151,v->timestamp);

    v = it->next();
    TEST_ASSERT_EQUAL(nullptr,v);

    delete it;

    currTs += 276ULL;
    metric.add(42); // 440
    

    it = metric.query();

    v = it->next();
    TEST_ASSERT_EQUAL(38,v->value);
    TEST_ASSERT_EQUAL_INT(440,v->timestamp);

    v = it->next();
    TEST_ASSERT_EQUAL(nullptr,v);

    delete it;

}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_min_folder);
    RUN_TEST(test_max_folder);
    RUN_TEST(test_avg_folder);
    RUN_TEST(test_folding_aggregator);
    RUN_TEST(test_time_decreasing_counter_aggregator);
    RUN_TEST(text_fixed_interval_aggregation_metric);


    UNITY_END();
}

void loop() {

}