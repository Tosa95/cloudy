#include <Arduino.h>
#include <unity.h>

#include "../../lib/logging/test_support/test_log.h"
#include "../../lib/ml/clustering/list_clustering.h"
#include "../../lib/ml/clustering/kmeans/bisecting_k_means.h"
#include "../../lib/ml/clustering/kmeans/criterions.h"
#include "../../lib/ml/distances/distances.h"

using namespace ML::Clustering::BKM;
using namespace ML::Clustering;

void test_list_clustering(void) {

    for (int i = 0; i < 100000; i++) {
        ML::Clustering::Clustering<float, int> *clustering = new ML::Clustering::ListClustering<float, int>(5);

        int *labels = clustering->getAllLabelsPerPoint();

        for (int i = 0; i < clustering->getNumPoints(); i++) {
            TEST_ASSERT_EQUAL(0, labels[i]);
        }

        float *c1 = new float;
        *c1 = 0.3;
        float *c2 = new float;
        *c2 = 0.5;

        clustering->addCluster(c1, 1);
        clustering->addCluster(c2, 2);

        labels[0] = 1;
        labels[1] = 1;
        labels[2] = 1;
        labels[3] = 2;
        labels[4] = 2;

        int *labelsPerCluster = clustering->getAllLabelsPerCluster();
        TEST_ASSERT_EQUAL(1, labelsPerCluster[0]);
        TEST_ASSERT_EQUAL(2, labelsPerCluster[1]);

        TEST_ASSERT_EQUAL(0.3, *(clustering->getCluster(1)));
        TEST_ASSERT_EQUAL(0.5, *(clustering->getCluster(2)));

        TEST_ASSERT_EQUAL(1, clustering->getLabel(0));
        TEST_ASSERT_EQUAL(1, clustering->getLabel(1));
        TEST_ASSERT_EQUAL(1, clustering->getLabel(2));
        TEST_ASSERT_EQUAL(2, clustering->getLabel(3));
        TEST_ASSERT_EQUAL(2, clustering->getLabel(4));

        TEST_ASSERT_EQUAL(5, clustering->getNumPoints());
        TEST_ASSERT_EQUAL(2, clustering->getNumClusters());
        
        delete clustering;
    }
}

void test_bisecting_kmeans_assign_to_cluster(void) {

    int points[16] = {
        0, 1,
        15, 16,
        -2, -1,
        -3, -2,
        1, 1,
        17, 15,
        18, 22,
        5, 6
    };

    for (int i = 0; i < 10000; i++) {
        BisectingKMeans<int, int> *bkm =  new BisectingKMeans<int, int>(points, 8, 2, 3, ML::Distances::L1Dist, new TooLowVarianceStopCriterion<int, int>(2), new MostVariantSelectCriterion<int, int>());

        bkm->addCluster(1);
        bkm->addCluster(0);

        bkm->getClustering()->getAllLabelsPerPoint()[0] = 1;
        bkm->getClustering()->getAllLabelsPerPoint()[1] = 1;

        int res = bkm->assignPointsToClusters(0, 1, 2);
        bkm->computeClustersStats(1, 2);

        int *labels = bkm->getClustering()->getAllLabelsPerPoint();

        TEST_ASSERT_EQUAL(7, res);

        TEST_ASSERT_EQUAL(2, labels[0]);
        TEST_ASSERT_EQUAL(1, labels[1]);
        TEST_ASSERT_EQUAL(2, labels[2]);
        TEST_ASSERT_EQUAL(2, labels[3]);
        TEST_ASSERT_EQUAL(2, labels[4]);
        TEST_ASSERT_EQUAL(1, labels[5]);
        TEST_ASSERT_EQUAL(1, labels[6]);
        TEST_ASSERT_EQUAL(2, labels[7]);

        TEST_ASSERT_EQUAL(3, bkm->getClustering()->getCluster(1)->nPoints);
        TEST_ASSERT_EQUAL(5, bkm->getClustering()->getCluster(2)->nPoints);

        TEST_ASSERT_EQUAL(16, bkm->getClustering()->getCluster(1)->centroid[0]);
        TEST_ASSERT_EQUAL(17, bkm->getClustering()->getCluster(1)->centroid[1]);

        TEST_ASSERT_EQUAL(0, bkm->getClustering()->getCluster(2)->centroid[0]);
        TEST_ASSERT_EQUAL(1, bkm->getClustering()->getCluster(2)->centroid[1]);

        TEST_ASSERT_EQUAL(4, bkm->getClustering()->getCluster(1)->avgDistFromCentroid);
        TEST_ASSERT_EQUAL(4.08, bkm->getClustering()->getCluster(2)->avgDistFromCentroid);

        delete bkm;

        yield();
    }
}

void test_bisecting_kmeans_n_sect_cluster(void) {

    int points[16] = {
        0, 1,
        -2, -1,
        15, 16,
        -3, -2,
        1, 1,
        17, 15,
        18, 22,
        5, 6
    };

    for (int i = 0; i < 10000; i++) {
        BisectingKMeans<int, int> *bkm = new BisectingKMeans<int, int>(points, 8, 2, 3, ML::Distances::L1Dist, new TooLowVarianceStopCriterion<int, int>(2), new MostVariantSelectCriterion<int, int>());

        bkm->nSectCluster(0, 2);

        int *labels = bkm->getClustering()->getAllLabelsPerPoint();

        int littleCluster = labels[2];
        int bigCluster = labels[1];

        TEST_ASSERT_EQUAL(bigCluster, labels[0]);
        TEST_ASSERT_EQUAL(bigCluster, labels[1]);
        TEST_ASSERT_EQUAL(littleCluster, labels[2]);
        TEST_ASSERT_EQUAL(bigCluster, labels[3]);
        TEST_ASSERT_EQUAL(bigCluster, labels[4]);
        TEST_ASSERT_EQUAL(littleCluster, labels[5]);
        TEST_ASSERT_EQUAL(littleCluster, labels[6]);
        TEST_ASSERT_EQUAL(bigCluster, labels[7]);

        TEST_ASSERT_EQUAL(3, bkm->getClustering()->getCluster(littleCluster)->nPoints);
        TEST_ASSERT_EQUAL(5, bkm->getClustering()->getCluster(bigCluster)->nPoints);

        TEST_ASSERT_EQUAL(16, bkm->getClustering()->getCluster(littleCluster)->centroid[0]);
        TEST_ASSERT_EQUAL(17, bkm->getClustering()->getCluster(littleCluster)->centroid[1]);

        TEST_ASSERT_EQUAL(0, bkm->getClustering()->getCluster(bigCluster)->centroid[0]);
        TEST_ASSERT_EQUAL(1, bkm->getClustering()->getCluster(bigCluster)->centroid[1]);

        TEST_ASSERT_EQUAL(4, bkm->getClustering()->getCluster(littleCluster)->avgDistFromCentroid);
        TEST_ASSERT_EQUAL(4.08, bkm->getClustering()->getCluster(littleCluster)->avgDistFromCentroid);

        delete bkm;

        yield();
    }
}

void test_most_variant_select_criterion() {
    int centr[] = {1, 2};
    
    BisectingKMeansCluster<int> *c1 = new BisectingKMeansCluster<int>(2, centr);
    c1->avgDistFromCentroid = 3;
    c1->nPoints = 1;
    BisectingKMeansCluster<int> *c2 = new BisectingKMeansCluster<int>(2, centr);
    c2->avgDistFromCentroid = 5;
    c2->nPoints = 1;
    BisectingKMeansCluster<int> *c3 = new BisectingKMeansCluster<int>(2, centr);
    c3->avgDistFromCentroid = 1;
    c3->nPoints = 1;

    MostVariantSelectCriterion<int, int> mvsc;
    ListClustering<BisectingKMeansCluster<int>, int> clustering(0);

    TEST_ASSERT_EQUAL(0, mvsc.select(&clustering));
    clustering.addCluster(c1, 1);
    TEST_ASSERT_EQUAL(1, mvsc.select(&clustering));
    clustering.addCluster(c2, 2);
    TEST_ASSERT_EQUAL(2, mvsc.select(&clustering));
    clustering.addCluster(c3, 3);
    TEST_ASSERT_EQUAL(2, mvsc.select(&clustering));
}

void test_too_low_variance_stop_criterion() {
    int centr[] = {1, 2};
    
    BisectingKMeansCluster<int> *c1 = new BisectingKMeansCluster<int>(2, centr);
    c1->avgDistFromCentroid = 3;
    c1->nPoints = 1;
    BisectingKMeansCluster<int> *c2 = new BisectingKMeansCluster<int>(2, centr);
    c2->avgDistFromCentroid = 5;
    c2->nPoints = 1;
    BisectingKMeansCluster<int> *c3 = new BisectingKMeansCluster<int>(2, centr);
    c3->avgDistFromCentroid = 7;
    c3->nPoints = 1;

    TooLowVarianceStopCriterion<int, int> tlvsc(5);
    ListClustering<BisectingKMeansCluster<int>, int> clustering(0);

    TEST_ASSERT_EQUAL(false, tlvsc.stop(&clustering));
    clustering.addCluster(c1, 1);
    TEST_ASSERT_EQUAL(true, tlvsc.stop(&clustering));
    clustering.addCluster(c2, 2);
    TEST_ASSERT_EQUAL(true, tlvsc.stop(&clustering));
    clustering.addCluster(c3, 3);
    TEST_ASSERT_EQUAL(false, tlvsc.stop(&clustering));
}

void test_fit(void) {

    int points[20] = {
        0, 1,
        -2, -1,
        100, 101,
        15, 16,
        -3, -2,
        1, 1,
        17, 15,
        18, 22,
        5, 6,
        105, 100,

    };

    for (int i = 0; i < 10000; i++) {
        BisectingKMeans<int, int> *bkm = new BisectingKMeans<int, int>(points, 10, 2, 10, ML::Distances::L1Dist, new TooLowVarianceStopCriterion<int, int>(5), new MostVariantSelectCriterion<int, int>());

        bkm->fit();

        int *labels = bkm->getClustering()->getAllLabelsPerPoint();

        int littleCluster = labels[3];
        int bigCluster = labels[1];
        int smallestCluster = labels[2];

        TEST_ASSERT_EQUAL(bigCluster, labels[0]);
        TEST_ASSERT_EQUAL(bigCluster, labels[1]);
        TEST_ASSERT_EQUAL(smallestCluster, labels[2]);
        TEST_ASSERT_EQUAL(littleCluster, labels[3]);
        TEST_ASSERT_EQUAL(bigCluster, labels[4]);
        TEST_ASSERT_EQUAL(bigCluster, labels[5]);
        TEST_ASSERT_EQUAL(littleCluster, labels[6]);
        TEST_ASSERT_EQUAL(littleCluster, labels[7]);
        TEST_ASSERT_EQUAL(bigCluster, labels[8]);
        TEST_ASSERT_EQUAL(smallestCluster, labels[9]);

        TEST_ASSERT_EQUAL(3, bkm->getClustering()->getCluster(littleCluster)->nPoints);
        TEST_ASSERT_EQUAL(5, bkm->getClustering()->getCluster(bigCluster)->nPoints);

        TEST_ASSERT_EQUAL(16, bkm->getClustering()->getCluster(littleCluster)->centroid[0]);
        TEST_ASSERT_EQUAL(17, bkm->getClustering()->getCluster(littleCluster)->centroid[1]);

        TEST_ASSERT_EQUAL(102, bkm->getClustering()->getCluster(smallestCluster)->centroid[0]);
        TEST_ASSERT_EQUAL(100, bkm->getClustering()->getCluster(smallestCluster)->centroid[1]);

        TEST_ASSERT_EQUAL(0, bkm->getClustering()->getCluster(bigCluster)->centroid[0]);
        TEST_ASSERT_EQUAL(1, bkm->getClustering()->getCluster(bigCluster)->centroid[1]);

        TEST_ASSERT_EQUAL(4, bkm->getClustering()->getCluster(littleCluster)->avgDistFromCentroid);
        TEST_ASSERT_EQUAL(4.08, bkm->getClustering()->getCluster(bigCluster)->avgDistFromCentroid);
        TEST_ASSERT_EQUAL(3, bkm->getClustering()->getCluster(smallestCluster)->avgDistFromCentroid);

        delete bkm;

        yield();
    }
}

void setup() {
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN();  // IMPORTANT LINE!

    // RUN_TEST(test_list_clustering);
    // RUN_TEST(test_bisecting_kmeans_assign_to_cluster);
    // RUN_TEST(test_bisecting_kmeans_n_sect_cluster);
    RUN_TEST(test_most_variant_select_criterion);
    RUN_TEST(test_too_low_variance_stop_criterion);
    RUN_TEST(test_fit);

    UNITY_END();
}

void loop() {}