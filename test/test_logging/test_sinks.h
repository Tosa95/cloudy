#ifndef E8C6D022_F16B_49A2_A2E1_E9140D5B0C1A
#define E8C6D022_F16B_49A2_A2E1_E9140D5B0C1A

    #include <unity.h>
    #include "common.h"
    #include "../../lib/logging/sinks/buffered_sink.h"
    #include "../../lib/logging/sinks/disablable_sink.h"

    void test_buffered_sink(void) {
        const int N = 10000;

        StubSink ss;
        Logging::BufferedSink bs(3,&ss);

        for (int i = 0; i < N; i++) {
            ProgmemFormatStringProvider msg1(F("Test message 1")); 
            Logging::LogEntry entry1 (
                (Time::millisecond_t)3600000,
                Logging::LogLevel::INFO,
                (uint8_t)1,
                &msg1
            );
            bs.put(entry1);

            TEST_ASSERT_EQUAL(0, ss.nCalls);

            ProgmemFormatStringProvider msg2(F("Test message 2")); 
            Logging::LogEntry entry2 (
                (Time::millisecond_t)3600001,
                Logging::LogLevel::WARNING,
                (uint8_t)1,
                &msg2
            );
            bs.put(entry2);

            TEST_ASSERT_EQUAL(0, ss.nCalls);

            ProgmemFormatStringProvider msg3(F("Test message 3")); 
            Logging::LogEntry entry3 (
                (Time::millisecond_t)3600002,
                Logging::LogLevel::ERROR,
                (uint8_t)1,
                &msg3
            );
            bs.put(entry3);

            TEST_ASSERT_EQUAL(0, ss.nCalls);

            bs.flush();

            TEST_ASSERT_EQUAL(3, ss.nCalls);

            TEST_ASSERT_EQUAL(3600000, ss.entriesReceived.get(0).timestamp);
            TEST_ASSERT_EQUAL(3600001, ss.entriesReceived.get(1).timestamp);
            TEST_ASSERT_EQUAL(3600002, ss.entriesReceived.get(2).timestamp);

            ss.clear();
        }
    }

    void test_disablable_sink (void) {
        const int N = 10000;
        StubSink ss;
        Logging::DisablableSink ds(false,&ss);

        for (int i = 0; i < N; i++) {
            ProgmemFormatStringProvider msg1(F("Test message 1")); 
            Logging::LogEntry entry1 (
                (Time::millisecond_t)3600000,
                Logging::LogLevel::INFO,
                (uint8_t)1,
                &msg1
            );
            ds.put(entry1);
            
            TEST_ASSERT_EQUAL(0, ss.nCalls);

            ds.setEnabled(true);

            ProgmemFormatStringProvider msg2(F("Test message 2")); 
            Logging::LogEntry entry2 (
                (Time::millisecond_t)3600001,
                Logging::LogLevel::INFO,
                (uint8_t)1,
                &msg2
            );
            ds.put(entry2);

            TEST_ASSERT_EQUAL(1, ss.nCalls);
            TEST_ASSERT_EQUAL(3600001, ss.entriesReceived.get(0).timestamp);

            ds.setEnabled(false);

            ProgmemFormatStringProvider msg3(F("Test message 3")); 
            Logging::LogEntry entry3 (
                (Time::millisecond_t)3600002,
                Logging::LogLevel::ERROR,
                (uint8_t)1,
                &msg3
            );
            ds.put(entry3);

            TEST_ASSERT_EQUAL(1, ss.nCalls);

            ss.clear();
        }
    }

#endif /* E8C6D022_F16B_49A2_A2E1_E9140D5B0C1A */
