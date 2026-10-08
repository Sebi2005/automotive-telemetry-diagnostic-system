#pragma once
#include <chrono>

enum class ECUType {
    ENGINE,
    BATTERY,
    VEHICLE_DYNAMICS
};

enum class SensorType {
    RPM,
    ENGINE_TEMPERATURE,
    THROTTLE_POSITION,
    BATTERY_VOLTAGE,
    BATTERY_CURRENT,
    SPEED,
    ACCELERATION
};

struct TelemetryMessage{
    ECUType source;
    SensorType sensor;
    double value;
    std::chrono::system_clock::time_point timestamp;
};
