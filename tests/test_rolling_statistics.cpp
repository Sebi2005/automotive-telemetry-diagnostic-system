#include <gtest/gtest.h>

#include "statistics/RollingStatistics.hpp"

TEST(RollingStatisticsTest, CalculatesMeanCorrectly) { // RollingStatisticsTest is the test suite name(grupul testului), and CalculatesMeanCorrectly is the test name
    RollingStatistics stats(5);

    stats.add(1.0);
    stats.add(2.0);
    stats.add(3.0);
    stats.add(4.0);
    stats.add(5.0);

    EXPECT_DOUBLE_EQ(stats.mean(), 3.0);
}

TEST(RollingStatisticsTest, ReportsWhenWindowIsFull) {
    RollingStatistics stats(3);

    EXPECT_FALSE(stats.isFull());

    stats.add(1.0);
    stats.add(2.0);

    EXPECT_FALSE(stats.isFull());

    stats.add(3.0);

    EXPECT_TRUE(stats.isFull());
}

TEST(RollingStatisticsTest, KeepsOnlyLatestValues) {
    RollingStatistics stats(3);

    stats.add(1.0);
    stats.add(2.0);
    stats.add(3.0);

    EXPECT_DOUBLE_EQ(stats.mean(), 2.0);

    stats.add(4.0);

    EXPECT_DOUBLE_EQ(stats.mean(), 3.0);
}

TEST(RollingStatisticsTest, CalculatesStandardDeviationCorrectly) {
    RollingStatistics stats(5);

    stats.add(2.0);
    stats.add(4.0);
    stats.add(4.0);
    stats.add(4.0);
    stats.add(5.0);

    EXPECT_NEAR(
        stats.standardDeviation(),
        0.979795897,
        1e-6
    );
}