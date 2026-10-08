#include <gtest/gtest.h>

#include "statistics/AnomalyDetector.hpp"

TEST(AnomalyDetectorTest, DoesNotFlagValuesWhileBuildingBaseline) {
    AnomalyDetector detector(5, 3.0);

    EXPECT_FALSE(detector.detect(90.0).has_value());
    EXPECT_FALSE(detector.detect(90.5).has_value());
    EXPECT_FALSE(detector.detect(89.5).has_value());
    EXPECT_FALSE(detector.detect(90.5).has_value());
    EXPECT_FALSE(detector.detect(89.5).has_value());
}

TEST(AnomalyDetectorTest, DetectsSuddenSpike) {
    AnomalyDetector detector(5, 3.0);

    detector.detect(90.0);
    detector.detect(90.5);
    detector.detect(89.5);
    detector.detect(90.5);
    detector.detect(89.5);

    auto result = detector.detect(105.0);

    ASSERT_TRUE(result.has_value());
    EXPECT_GT(result.value(), 3.0);
}

TEST(AnomalyDetectorTest, DoesNotFlagNormalValue) {
    AnomalyDetector detector(5, 3.0);

    detector.detect(90.0);
    detector.detect(90.5);
    detector.detect(89.5);
    detector.detect(90.5);
    detector.detect(89.5);

    auto result = detector.detect(90.2);

    EXPECT_FALSE(result.has_value());
}