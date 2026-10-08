#include "utils/EnumStrings.hpp"

std::string toString(ECUType ecu) {
    switch (ecu) {
    case ECUType::ENGINE:
        return "ENGINE";
    case ECUType::BATTERY:
        return "BATTERY";
    case ECUType::VEHICLE_DYNAMICS:
        return "VEHICLE_DYNAMICS";
    }

    return "UNKNOWN";
}

std::string toString(SensorType sensor) {
    switch (sensor) {
    case SensorType::RPM:
        return "RPM";
    case SensorType::ENGINE_TEMPERATURE:
        return "ENGINE_TEMPERATURE";
    case SensorType::THROTTLE_POSITION:
        return "THROTTLE_POSITION";
    case SensorType::BATTERY_VOLTAGE:
        return "BATTERY_VOLTAGE";
    case SensorType::BATTERY_CURRENT:
        return "BATTERY_CURRENT";
    case SensorType::SPEED:
        return "SPEED";
    case SensorType::ACCELERATION:
        return "ACCELERATION";
    }

    return "UNKNOWN";
}

std::string toString(Severity severity) {
    switch (severity) {
    case Severity::INFO:
        return "INFO";
    case Severity::WARNING:
        return "WARNING";
    case Severity::CRITICAL:
        return "CRITICAL";
    }

    return "UNKNOWN";
}

std::string toString(FaultState state) {
    switch (state) {
    case FaultState::NORMAL:
        return "NORMAL";
    case FaultState::SUSPECTED:
        return "SUSPECTED";
    case FaultState::CONFIRMED_FAULT:
        return "CONFIRMED_FAULT";
    case FaultState::RECOVERING:
        return "RECOVERING";
    }

    return "UNKNOWN";
}

std::string toString(HealthStatus status) {
    switch (status) {
    case HealthStatus::HEALTHY:
        return "HEALTHY";
    case HealthStatus::DEGRADED:
        return "DEGRADED";
    case HealthStatus::CRITICAL:
        return "CRITICAL";
    }

    return "UNKNOWN";
}