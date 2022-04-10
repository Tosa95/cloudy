#include <Arduino.h>
#include "../../lib/string_utils/string_provider.h"
#include "../../lib/string_utils/augmented_printf.h"
#include <unity.h>

void test_progmem_string_provider(void) {
    ProgmemStringProvider psp(F("TEST STRING"));

    char buff[10];

    psp.get(buff);

    Serial.println(buff);

    TEST_ASSERT_EQUAL_STRING("TEST STRING", buff);
}

void test_progmem_format_string_provider(void) {
    ProgmemFormatStringProvider psp(F("TEST %d %d"), 3, 2);

    char buff[10];

    psp.get(buff);

    TEST_ASSERT_EQUAL_STRING("TEST 3 2", buff);

    TEST_ASSERT_EQUAL(sizeof(psp) + 2*sizeof(int), psp.getSize());

    Serial.println(sizeof(psp));
}

void test_get_next_type(void) {
    FormatTypeResult res = getNextType("An int: %02d a short: %05hd a float: %.5f a string: %s", 0);

    TEST_ASSERT_EQUAL(FormatType::INT, res.type);
    TEST_ASSERT_EQUAL(sizeof(int), res.typeSz);
    TEST_ASSERT_EQUAL(8, res.start);
    TEST_ASSERT_EQUAL(12, res.end);

    res = getNextType((const char *)F("An int: %02d a short: %05hd a float: %.5f a string: %s"), res.end, true);

    TEST_ASSERT_EQUAL(FormatType::SHORT, res.type);
    TEST_ASSERT_EQUAL(sizeof(short), res.typeSz);
    TEST_ASSERT_EQUAL(22, res.start);
    TEST_ASSERT_EQUAL(27, res.end);

    res = getNextType((const char *)F("An int: %02d a short: %05hd a float: %.5f a string: %s"), res.end, true);

    TEST_ASSERT_EQUAL(FormatType::DOUBLE, res.type);
    TEST_ASSERT_EQUAL(sizeof(double), res.typeSz);
    TEST_ASSERT_EQUAL(37, res.start);
    TEST_ASSERT_EQUAL(41, res.end);

    res = getNextType((const char *)F("An int: %02d a short: %05hd a float: %.5f a %% string: %s"), res.end, true);

    TEST_ASSERT_EQUAL(FormatType::STRING, res.type);
    TEST_ASSERT_EQUAL(sizeof(char *), res.typeSz);
    TEST_ASSERT_EQUAL(55, res.start);
    TEST_ASSERT_EQUAL(57, res.end);
}

void test_augmented_sprintf(void) {
    char buff[100];

    char params[2*sizeof(int)+7];

    int a = 13;
    int b = 17;
    char str[] = "string";
    
    strcpy(params, str);
    memcpy(params + strlen(str) + 1, &a, sizeof(int));
    memcpy(params + strlen(str) + 1 + sizeof(int), &b, sizeof(int));

    augmentedSprintf(buff, (const char *)F("A string: %s An int: %03d hahah %d hehehe"), params, true);

    TEST_ASSERT_EQUAL_STRING("A string: string An int: 013 hahah 17 hehehe", buff);
}

void test_get_num_formats(void) {
    int res = getNumFormats((const char *)F("An int: %03d hahah %d hehehe %s"), true);

    TEST_ASSERT_EQUAL(3, res);
}

void test_get_total_data_size(void) {
    int res = getTotalDataSize(true, (const char *)F("An int: %03d hahah %d hehehe %s"), 3, 12, "abcde");
    
    TEST_ASSERT_EQUAL(14, res);
}

void test_get_printf_args(void) {
    for (int i = 0; i < 10000; i++) {
        const char *format = (const char *)F("I'm a printf %s with %003d args. %lld, %s");
        AugmentedPrintfArgs res = getPrintfArgs(true, format, "string", (int)2, 123456789101112LL, "heyy");

        char *args = (char*)res.args;

        TEST_ASSERT_EQUAL_STRING("string", args);
        args += strlen(args) + 1;

        int v;
        memcpy(&v, args, sizeof(v));
        TEST_ASSERT_EQUAL(2,v);
        args += sizeof(int);

        long long vl;
        memcpy(&vl, args, sizeof(vl));
        TEST_ASSERT_EQUAL(123456789101112LL,vl);
        args += sizeof(long long);

        TEST_ASSERT_EQUAL_STRING("heyy", args);

        TEST_ASSERT_EQUAL(7 + sizeof(int) + sizeof(long long) + 5, res.getDataSz(format, true));

        // TEST_ASSERT_EQUAL(7 + sizeof(int) + sizeof(long long), res.dataSz);

        // TEST_ASSERT_EQUAL(7 + sizeof(int) + sizeof(long long) + sizeof(res), res.getTotalSize());

        res.deleteData();

        yield();
    }
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();    // IMPORTANT LINE!

    RUN_TEST(test_progmem_string_provider);
    RUN_TEST(test_progmem_format_string_provider);
    RUN_TEST(test_get_next_type);
    RUN_TEST(test_augmented_sprintf);
    RUN_TEST(test_get_num_formats);
    RUN_TEST(test_get_printf_args);
    RUN_TEST(test_get_total_data_size);



    UNITY_END();
}

void loop() {

}