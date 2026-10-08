#include "statistics/AnomalyDetector.hpp"

AnomalyDetector::AnomalyDetector(std::size_t windowSize, double zThreshold)
    : statistics_(windowSize), zThreshold_(zThreshold) {}

std::optional<double> AnomalyDetector::detect(double value) {
    if (!statistics_.isFull()) {
        statistics_.add(value);
        return std::nullopt;
    }

    const double mean = statistics_.mean();
    const double stdDev = statistics_.standardDeviation();

    double zScore = 0.0;
    bool anomaly = false;

    if (stdDev < 0.000001) {
        anomaly = std::abs(value - mean) > 0.000001;
    } else {
         zScore = (value - mean) / stdDev;

        anomaly = std::abs(zScore) >= zThreshold_;
    }

    statistics_.add(value);

    if (anomaly) {
        return zScore;
    }
    return std::nullopt;
}