#pragma once

#include <cstddef>
#include <deque>

class RollingStatistics {
public:
    explicit RollingStatistics(std::size_t windowSize);

    void add(double value);

    bool isFull() const;

    double mean() const;
    double standardDeviation() const;

private:
    std::size_t windowSize_;
    std::deque<double> values_;
};