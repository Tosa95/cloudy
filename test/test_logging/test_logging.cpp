#include <Arduino.h>
#include "../../lib/logging/formatters/standard_formatter.h"
#include "../../lib/logging/formatters/json_formatter.h"
#include "../../lib/logging/log_entry.h"
#include "test_sinks.h"
#include <unity.h>

using namespace Logging;

const __FlashStringHelper *test;

void test_standard_formatter(void) {

    const char *labelsTable[3] = {
        "LBL1", "LBL2", "LBL3"
    };

    Serial.println(labelsTable[1]);

    StandardFormatter f(labelsTable, false);

    test = F("Log message with parameters: %d and %.2f");

    ProgmemFormatStringProvider msg(F("Log message with parameters: %d and %.2f"), 42, 5.7); 

    LogEntry entry (
        (Time::millisecond_t)3600000,
        LogLevel::INFO,
        (uint8_t)1,
        &msg
    );

    char buffer[100];
    f.format(entry, buffer);

    Serial.println(buffer);

    TEST_ASSERT_EQUAL_STRING("1970-01-01T01:00:00.000 |  INFO |       LBL2 | Log message with parameters: 42 and 5.70", buffer);
}

void test_json_formatter(void) {

    const char *labelsTable[3] = {
        "LBL1", "LBL2", "LBL3"
    };

    Serial.println(labelsTable[1]);

    JsonFormatter f(labelsTable, false);

    ProgmemFormatStringProvider msg(test, 42, 5.7); 

    LogEntry entry (
        (Time::millisecond_t)3600000,
        LogLevel::INFO,
        (uint8_t)1,
        &msg
    );

    char buffer[100];
    f.format(entry, buffer);

    Serial.println(buffer);

    TEST_ASSERT_EQUAL_STRING("{\"ts\":\"1970-01-01T01:00:00.000\",\"level\":\"INFO\",\"label\":\"LBL2\",\"msg\":\"Log message with parameters: 42 and 5.70\"}", buffer);
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_standard_formatter);
    RUN_TEST(test_json_formatter);
    RUN_TEST(test_buffered_sink);
    RUN_TEST(test_disablable_sink);

    UNITY_END();
}

void loop() {

}