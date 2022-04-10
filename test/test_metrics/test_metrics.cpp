#include <Arduino.h>
#include "../../lib/metrics/queue_based/queue_metric_variable_interval_iterator.h"
#include "../../lib/metrics/queue_based/queue_metric_variable_interval.h"
#include "../../lib/metrics/queue_based/queue_metric_fixed_interval_iterator.h"
#include "../../lib/metrics/queue_based/queue_metric_fixed_interval.h"
#include "../../lib/metrics/json_translation/json_metrics.h"
#include "../../lib/metrics/json_translation/printer.h"



#include <unity.h>

void test_queue_metric_iterator(void) {
    
    Queue<int> q(5);

    q.put(3);
    q.put(7);
    q.put(13);
    q.put(42);
    q.put(18);

    Metrics::QueueMetricVariableIntervalIterator<int> it(&q, [](int &v){return (Time::millisecond_t)v+13;}, 1, 3);

    Metrics::ValueAndTimestamp<int> *curr = it.next();
    TEST_ASSERT_EQUAL(7, curr->value);
    TEST_ASSERT_EQUAL(20, curr->timestamp);

    curr = it.next();
    TEST_ASSERT_EQUAL(13, curr->value);
    TEST_ASSERT_EQUAL(26, curr->timestamp);

    curr = it.next();
    TEST_ASSERT_EQUAL(42, curr->value);
    TEST_ASSERT_EQUAL(55, curr->timestamp);

    curr = it.next();
    TEST_ASSERT_EQUAL(nullptr, curr);

}

void test_queue_metric(void) {
    
    Metrics::QueueMetricVariableInterval<int> qm(5, [](int &v){return (Time::millisecond_t)v+1;});

    qm.add(5);
    qm.add(7);
    qm.add(8);

    Metrics::Iterator<int> *it = qm.query();

    Metrics::ValueAndTimestamp<int> *curr = it->next();
    TEST_ASSERT_EQUAL(5, curr->value);
    TEST_ASSERT_EQUAL(6, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(7, curr->value);
    TEST_ASSERT_EQUAL(8, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(8, curr->value);
    TEST_ASSERT_EQUAL(9, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(nullptr, curr);

    delete it;

    it = qm.query(7);

    curr = it->next();
    TEST_ASSERT_EQUAL(7, curr->value);
    TEST_ASSERT_EQUAL(8, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(8, curr->value);
    TEST_ASSERT_EQUAL(9, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(nullptr, curr);

    delete it;

    qm.add(9);
    qm.add(10);
    qm.add(11);
    qm.add(12);

    it = qm.query(0, 12);

    curr = it->next();
    TEST_ASSERT_EQUAL(8, curr->value);
    TEST_ASSERT_EQUAL(9, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(9, curr->value);
    TEST_ASSERT_EQUAL(10, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(10, curr->value);
    TEST_ASSERT_EQUAL(11, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(11, curr->value);
    TEST_ASSERT_EQUAL(12, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(nullptr, curr);

    delete it;

}

void test_queue_metric_fixed_interval_iterator(void) {
    
    Queue<int> q(5);

    q.put(3);
    q.put(7);
    q.put(13);
    q.put(42);
    q.put(18);

    Metrics::QueueMetricFixedIntervalIterator<int> it(
        &q, 
        0, 
        1, 
        1,
        3);

    Metrics::ValueAndTimestamp<int> *curr = it.next();
    TEST_ASSERT_EQUAL(7, curr->value);
    TEST_ASSERT_EQUAL(0, curr->timestamp);

    curr = it.next();
    TEST_ASSERT_EQUAL(13, curr->value);
    TEST_ASSERT_EQUAL(1, curr->timestamp);

    curr = it.next();
    TEST_ASSERT_EQUAL(42, curr->value);
    TEST_ASSERT_EQUAL(2, curr->timestamp);

    curr = it.next();
    TEST_ASSERT_EQUAL(nullptr, curr);

}

void test_queue_metric_fixed_interval(void) {
    
    Metrics::QueueMetricFixedInterval<int> qm(
        5, 
        [](){return 0ULL;},
        3
    );

    qm.add(5);
    qm.add(7);
    qm.add(8);

    Metrics::Iterator<int> *it = qm.query();

    Metrics::ValueAndTimestamp<int> *curr = it->next();
    TEST_ASSERT_EQUAL(5, curr->value);
    TEST_ASSERT_EQUAL(0, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(7, curr->value);
    TEST_ASSERT_EQUAL(3, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(8, curr->value);
    TEST_ASSERT_EQUAL(6, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(nullptr, curr);

    delete it;

    it = qm.query(4);

    curr = it->next();
    TEST_ASSERT_EQUAL(8, curr->value);
    TEST_ASSERT_EQUAL(6, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(nullptr, curr);

    delete it;

    qm.add(9);
    qm.add(10);
    qm.add(11);
    qm.add(12);

    it = qm.query(0, 12);

    curr = it->next();
    TEST_ASSERT_EQUAL(8, curr->value);
    TEST_ASSERT_EQUAL(6, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(9, curr->value);
    TEST_ASSERT_EQUAL(9, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(10, curr->value);
    TEST_ASSERT_EQUAL(12, curr->timestamp);

    curr = it->next();
    TEST_ASSERT_EQUAL(nullptr, curr);

    delete it;

}

void test_queue_metric_fixed_interval_long_run(void) {
    
    int QS = 300;
    int N = 100000;

    Metrics::QueueMetricFixedInterval<int> qm(
        QS, 
        [](){return 0ULL;},
        30000000
    );

    int i = 0;

    for (;i<N;i++) {
        qm.add(i);
        yield();

        if (i > QS) {
            Metrics::Iterator<int> *it = qm.query();
            int j = 0;
            for (;j<QS;j++) {
                Metrics::ValueAndTimestamp<int> *curr = it->next();
                TEST_ASSERT_EQUAL(i-(QS-j-1), curr->value);
                TEST_ASSERT_EQUAL((i-(QS-j-1))*30000000LLU, curr->timestamp);
            }
            TEST_ASSERT_EQUAL(QS,j);
            delete it;
        }
    }

    

}

void test_queue_metric_to_json(void) {
    char buffer[100];

    Metrics::QueueMetricVariableInterval<int> qm(5, [](int &v){return (Time::millisecond_t)v+1;});

    qm.add(5);
    qm.add(7);
    qm.add(8);

    Metrics::IntJsonMetric jm(&qm);

    Metrics::BufferPrinter printer(buffer, 100);

    jm.setPrinter(&printer);

    Metrics::Iterator<void *> *it = jm.query();

    it->next();
    TEST_ASSERT_EQUAL_STRING("[5,6]", buffer);

    it->next();
    TEST_ASSERT_EQUAL_STRING("[7,8]", buffer);

    it->next();
    TEST_ASSERT_EQUAL_STRING("[8,9]", buffer);

    delete it;

}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_queue_metric_iterator);
    RUN_TEST(test_queue_metric);
    RUN_TEST(test_queue_metric_fixed_interval_iterator);
    RUN_TEST(test_queue_metric_fixed_interval);
    RUN_TEST(test_queue_metric_fixed_interval_long_run);
    RUN_TEST(test_queue_metric_to_json);


    UNITY_END();
}

void loop() {

}