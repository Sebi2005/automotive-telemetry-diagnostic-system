#include "statistics/RollingStatistics.hpp"
#include <cmath>

RollingStatistics::RollingStatistics(std::size_t windowSize) : windowSize_(windowSize) {}

void RollingStatistics::add(double value){
    values_.push_back(value);
    if (values_.size() > windowSize_) {
        values_.pop_front();
    }
}

bool RollingStatistics::isFull() const {
    return values_.size() == windowSize_;
}

double RollingStatistics::mean() const {
    if (values_.empty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (double value : values_) {
        sum += value;
    }
    return sum / values_.size();
}

double RollingStatistics::standardDeviation() const {
    if (values_.empty()) {
        return 0.0;
    }
    double meanValue = mean();
    double sumSquaredDiff = 0.0;
    for (double value : values_) {
        double diff = value - meanValue;
        sumSquaredDiff += diff * diff;
    }
    return std::sqrt(sumSquaredDiff / values_.size());
}