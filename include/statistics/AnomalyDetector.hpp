#pragma once

#include "statistics/RollingStatistics.hpp"
#include <optional>

class AnomalyDetector {
public:
    AnomalyDetector(
        std::size_t windowSize,
        double zThreshold
    );

    std::optional<double> detect(double value);

private:
    RollingStatistics statistics_;
    double zThreshold_;
};