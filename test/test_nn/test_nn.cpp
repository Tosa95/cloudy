#include <Arduino.h>
#include <unity.h>

#include "../../lib/logging/test_support/test_log.h"
#include "../../src/nn/model.h"
#include "../../src/nn/model_cat_classification_clustering.cpp"
#include "../../src/nn/model_cat_classification_clustering_quant.cpp"
#include "../../src/nn/utils.cpp"

void test_nn(void) {
    NN::Model model(model_cat_classification_clustering, 10 * 1024);

    float input_data_none[] = {
        0.45936885, 0.82743347, 0.19827627, 0.207, 0.04218695, 0.2,
        0.34260318, 0.24374388, 0.99033695, 0.181, 0.04993258, 0.2,
        0.21107867, 0.53449059, 0.6051966,  0.166, 0.07392552, 0.2,
        0.31657353, 0.31009799, 0.86954308, 0.12,  0.07007266, 0.2,
        0.29283044, 0.37225851, 0.71989828, 0.108, 0.08087437, 0.2};

    float input_data_kiko[] = {
        0.38716772, 0.26957816, 0.98095679, 0.132, 0.07805888, 0.6,
        0.41642997, 0.65519112, 0.28174222, 0.122, 0.0838792, 0.6,
        0.36744535, 0.49800357, 0.3820321,  0.11,  0.09303114, 0.6,
        0.41561168, 0.53113854, 0.25961518, 0.109, 0.09367169, 0.6,
        0.41149393, 0.06180103, 0.99989104, 0.108, 0.09186467, 0.6};

    float input_data_covid[] = {
        0.19132002, 0.99217981, 0.28120112, 0.337, 0.09003901, 0.6,
        0.18346676, 0.86979395, 0.50220239, 0.276, 0.06954624, 0.6,
        0.1781687,  0.65090919, 0.6475746,  0.167, 0.08930595, 0.6,
        0.45262468, 0.39561266, 0.77115142, 0.101, 0.09283216, 0.6,
        0.17087686, 0.54249769, 0.91287321, 0.092, 0.07320072, 0.6};

    float input_data_covid_2[] = {
        0.22, 0.41, 0.8, 0.334, 0.09003901, 0.6, 
        0.217, 0.5, 0.63, 0.309, 0.06954624, 0.6,
        0.28,  0.33, 0.63,  0.076, 0.08930595, 0.6,
        0.21, 0.77, 0.38, 0.067, 0.09283216, 0.6,
        0.21, 0.65, 0.50, 0.057, 0.07320072, 0.6};

    long long before = millis();

    for (int i = 0; i < 10000; i++) {
        TEST_ASSERT_EQUAL(0, NN::argmax(model.runInference(input_data_covid),
                                        model.getOutputSize()));
        TEST_ASSERT_EQUAL(0, NN::argmax(model.runInference(input_data_covid_2),
                                        model.getOutputSize()));
        TEST_ASSERT_EQUAL(1, NN::argmax(model.runInference(input_data_kiko),
                                        model.getOutputSize()));
        TEST_ASSERT_EQUAL(2, NN::argmax(model.runInference(input_data_none),
                                        model.getOutputSize()));
    }

    Serial.println(millis() - before);
}

void test_mults(void) {

    long long before = millis();
    int a = 0;

    for (int i = 0; i < 10000000; i++) {
        a += i * 10;
    }

    Serial.println(millis() - before);

    before = millis();

    int f = 0;

    for (float i = 0; i < 10000000; i++) {
        f += i * 10;
    }

    Serial.println(millis() - before);

    Serial.println(a);
    Serial.println(f);
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();  // IMPORTANT LINE!

    RUN_TEST(test_nn);
    // RUN_TEST(test_mults);

    UNITY_END();
}

void loop() {}