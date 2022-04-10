#include <Arduino.h>
#include "../../lib/img/algorithms/map.h"
#include "../../lib/img/algorithms/difference.h"
#include <unity.h>

using namespace Img::Algorithms;
using namespace Img;

ImgBuffer<uint8_t> *getTestImg(uint8_t factor = 1, uint16_t w = 4, uint16_t h = 4, uint16_t nc = 2)
{
    ImgBuffer<uint8_t> *buff = Img::emtpyImgBuffer<uint8_t>(w, h, nc);

    for (int c = 0; c < nc; c++)
    {
        for (int y = 0; y < h; y++)
        {
            for (int x = 0; x < w; x++)
            {
                buff->setPixelChannel(x, y, c, (x + y + c)*factor);
                // Serial.print((x + y + c)*factor);
                // Serial.print(",");
            }
            // Serial.println();
        }
        // Serial.println();
    }

    return buff;
}

void test_process_cell(void)
{
    for (int i = 0; i < 100000; i++)
    {
        ImgBuffer<uint8_t> *buff = getTestImg();

        float res = process_cell<uint32_t, float, uint8_t>(
            1,
            1,
            buff,
            [=](uint16_t x, uint16_t y) -> uint32_t * { uint32_t *res = new uint32_t; *res = 0; return res; },
            [=](uint16_t x, uint16_t y, uint16_t c, uint8_t v, uint32_t *sum) { *sum += v; },
            [=](uint32_t *sum) -> float { return (float)(*sum) / (3 * 3 * 2); },
            3,
            3,
            1);

        delete buff;

        TEST_ASSERT_FLOAT_WITHIN(0.01, 4.5, res);
    }
}

void test_process_cell_non_square(void)
{
    for (int i = 0; i < 100000; i++)
    {
        ImgBuffer<uint8_t> *buff = getTestImg();

        float res = process_cell<uint32_t, float, uint8_t>(
            0,
            0,
            buff,
            [=](uint16_t x, uint16_t y) -> uint32_t * { uint32_t *res = new uint32_t; *res = 0; return res; },
            [=](uint16_t x, uint16_t y, uint16_t c, uint8_t v, uint32_t *sum) { *sum += v; },
            [=](uint32_t *sum) -> float { return (float)(*sum) / (3 * 3 * 2); },
            3,
            2,
            1);

        delete buff;

        TEST_ASSERT_FLOAT_WITHIN(0.01, 1.33, res);
    }
}


void test_process_cell_w_step(void)
{
    for (int i = 0; i < 100000; i++)
    {
        ImgBuffer<uint8_t> *buff = getTestImg();

        float res = process_cell<uint32_t, float, uint8_t>(
            0,
            0,
            buff,
            [=](uint16_t x, uint16_t y) -> uint32_t * { uint32_t *res = new uint32_t; *res = 0; return res; },
            [=](uint16_t x, uint16_t y, uint16_t c, uint8_t v, uint32_t *sum) { *sum += v; },
            [=](uint32_t *sum) -> float { return (float)(*sum) / (3 * 3 * 2); },
            3,
            3,
            2);

        delete buff;

        TEST_ASSERT_FLOAT_WITHIN(0.01, 1.11, res);
    }
}

void test_process_cell_w_padding(void)
{
    for (int i = 0; i < 100000; i++)
    {
        ImgBuffer<uint8_t> *buff = getTestImg();

        float res = process_cell<uint32_t, float, uint8_t>(
            2,
            2,
            buff,
            [=](uint16_t x, uint16_t y) -> uint32_t * { uint32_t *res = new uint32_t; *res = 0; return res; },
            [=](uint16_t x, uint16_t y, uint16_t c, uint8_t v, uint32_t *sum) { *sum += v; },
            [=](uint32_t *sum) -> float { return (float)(*sum) / (3 * 3 * 2); },
            3,
            3,
            1,
            [](uint16_t x, uint16_t y, uint16_t c) -> uint8_t { return c==0 ? 1 : 0; });

        delete buff;

        TEST_ASSERT_FLOAT_WITHIN(0.01, 2.72, res);
    }
}

void test_grid_map(void)
{
    for (int i = 0; i < 10000; i++)
    {
        ImgBuffer<uint8_t> *buff = getTestImg();

        ImgBuffer<float> *res = grid_map<uint32_t, float, uint8_t>(
            buff,
            [=](uint16_t x, uint16_t y) -> uint32_t * { uint32_t *res = new uint32_t; *res = 0; return res; },
            [=](uint16_t x, uint16_t y, uint16_t c, uint8_t v, uint32_t *sum) { *sum += v; },
            [=](uint32_t *sum) -> float { return (float)(*sum) / (2 * 2 * 2); },
            2,
            2,
            1
        );

        TEST_ASSERT_FLOAT_WITHIN(0.01, 1.5, res->getPixelChannel(0,0,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 3.5, res->getPixelChannel(1,0,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 3.5, res->getPixelChannel(0,1,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 5.5, res->getPixelChannel(1,1,0));

        delete buff;
        delete res;
    }
}

void test_noise_insensitive_diff(void)
{
    for (int i = 0; i < 100000; i++)
    {
        ImgBuffer<uint8_t> *buff1 = getTestImg();
        ImgBuffer<uint8_t> *buff2 = getTestImg(2);

        DiffResult<uint8_t, int32_t> *res = computeNoiseInsensitiveDiff<uint8_t, int32_t>(buff1, buff2, 2);

        TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, res->diff->getPixelChannel(0,0,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 4.0, res->diff->getPixelChannel(1,0,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 4.0, res->diff->getPixelChannel(0,1,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 6.0, res->diff->getPixelChannel(1,1,0));

        delete buff1;
        delete buff2;
        delete res->diff;
        delete res;
    }
}

void test_noise_insensitive_diff_fast(void)
{
    for (int i = 0; i < 1000; i++)
    {
        ImgBuffer<uint8_t> *buff1 = getTestImg();
        ImgBuffer<uint8_t> *buff2 = getTestImg(2);

        DiffResult<uint8_t, int32_t> *res = computeNoiseInsensitiveDiffFast<uint8_t, int32_t>(buff1, buff2, 2);

        TEST_ASSERT_FLOAT_WITHIN(0.01, 2.0, res->diff->getPixelChannel(0,0,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 4.0, res->diff->getPixelChannel(1,0,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 4.0, res->diff->getPixelChannel(0,1,0));
        TEST_ASSERT_FLOAT_WITHIN(0.01, 6.0, res->diff->getPixelChannel(1,1,0));

        delete buff1;
        delete buff2;
        delete res->diff;
        delete res;
    }
}

void test_noise_insensitive_diff_fast_vs_slow(void)
{
    for (int i = 0; i < 10; i++) {
        ImgBuffer<uint8_t> *buff1 = getTestImg(1, 170, 200, 3);
        ImgBuffer<uint8_t> *buff2 = getTestImg(2, 170, 200, 3);

        uint64_t start = micros();
        DiffResult<uint8_t, int32_t> *res = computeNoiseInsensitiveDiff<uint8_t, int32_t>(buff1, buff2, 5);
        uint64_t timeSlow = micros() - start;
        delete res;

        start = micros();
        res = computeNoiseInsensitiveDiffFast<uint8_t, int32_t>(buff1, buff2, 5);
        uint64_t timeFast = micros() - start;
        delete res;

        Serial.printf("Fast: %d, Slow: %d\n", (int32_t)timeFast, (int32_t)timeSlow);

        delete buff1;
        delete buff2;
    }
}

void setup()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!

    // RUN_TEST(test_process_cell);
    // RUN_TEST(test_process_cell_w_padding);
    // RUN_TEST(test_process_cell_w_step);
    // RUN_TEST(test_process_cell_non_square);

    // RUN_TEST(test_grid_map);
    
    // RUN_TEST(test_noise_insensitive_diff);
    RUN_TEST(test_noise_insensitive_diff_fast);
    RUN_TEST(test_noise_insensitive_diff_fast_vs_slow);


    UNITY_END();
}

void loop()
{
}