#include <Arduino.h>
#include "../../lib/queue/queue.h"
#include <unity.h>

void test_queue(void) {
    
    Queue<int> q(5);

    TEST_ASSERT_TRUE(q.empty());
    TEST_ASSERT_FALSE(q.full());

    q.put(3);
    TEST_ASSERT_FALSE(q.empty());
    TEST_ASSERT_FALSE(q.full());
    q.put(7);
    q.put(13);
    q.put(42);
    q.put(18);

    TEST_ASSERT_FALSE(q.empty());
    TEST_ASSERT_TRUE(q.full());

    TEST_ASSERT_EQUAL(3, q.get());
    TEST_ASSERT_EQUAL(42, q.get(3));

    TEST_ASSERT_EQUAL(3, q.pop());
    TEST_ASSERT_FALSE(q.empty());
    TEST_ASSERT_FALSE(q.full());
    TEST_ASSERT_EQUAL(7, q.pop());

    TEST_ASSERT_EQUAL(42, q.get(1));

    q.put(84);
    q.put(168);
    TEST_ASSERT_FALSE(q.empty());
    TEST_ASSERT_TRUE(q.full());

    TEST_ASSERT_EQUAL(13, q.get());

    TEST_ASSERT_EQUAL(18, q.get(2));
    TEST_ASSERT_EQUAL(168, q.get(4));


}

void test_queue_long_run(void) {
    
    int N = 100;
    Queue<int> q(N);

    int i = 0;

    for (;i < N;i++) {
        q.put(i);
        TEST_ASSERT_EQUAL(i, q.get(i));
    }

    
    for (;i<100000;i++) {
        q.pop();
        q.put(i);

        for (int j = 0; j < N; j++) {
            TEST_ASSERT_EQUAL(i-(N-j-1), q.get(j));
        }

        yield();
        
    }

}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_queue);
    RUN_TEST(test_queue_long_run);

    UNITY_END();
}

void loop() {

}