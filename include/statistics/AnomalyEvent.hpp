#pragma once

#include <chrono>

#include "core/TelemetryMessage.hpp"

struct AnomalyEvent {
    SensorType sensor;
    double value;
    double zScore;
    std::chrono::system_clock::time_point timestamp;
};