#include "diagnostics/DiagnosticEngine.hpp"

#include <iostream>


void DiagnosticEngine::updateFaultState(
    DiagnosticRuleState& rule,
    bool abnormal,
    const char* dtcCode,
    const char* description,
    Severity severity,
    std::chrono::system_clock::time_point timestamp
) {
    switch (rule.state) {

    case FaultState::NORMAL:
        if (abnormal) {
            rule.state = FaultState::SUSPECTED;
            rule.abnormalCount = 1;

            std::cout << "[" << dtcCode << "] "
                      << description
                      << " suspected\n";
        }
        break;

    case FaultState::SUSPECTED:
        if (abnormal) {
            ++rule.abnormalCount;

            if (rule.abnormalCount >= 3) {
                rule.state = FaultState::CONFIRMED_FAULT;
                rule.normalCount = 0;
                
                Fault fault{
                    dtcCode,
                    description,
                    severity,
                    FaultState::CONFIRMED_FAULT,
                    timestamp,
                    std::nullopt
            };

    faultHistory_.push_back(fault);

    rule.activeFaultIndex = faultHistory_.size() - 1;

                std::cout << "[" << dtcCode << "] "
                          << description
                          << " CONFIRMED\n";
            }
        } else {
            rule.state = FaultState::NORMAL;
            rule.abnormalCount = 0;

            std::cout << "[" << dtcCode << "] "
                      << description
                      << " suspicion cleared\n";
        }
        break;

    case FaultState::CONFIRMED_FAULT:
        if (!abnormal) {
            rule.state = FaultState::RECOVERING;
            rule.normalCount = 1;

            std::cout << "[" << dtcCode << "] "
                      << description
                      << " recovering\n";
        }
        break;

    case FaultState::RECOVERING:
        if (!abnormal) {
            ++rule.normalCount;

            if (rule.normalCount >= 3) {
                
                if (rule.activeFaultIndex.has_value()) {
                    faultHistory_[rule.activeFaultIndex.value()].endTime = timestamp;
                    faultHistory_[rule.activeFaultIndex.value()].state = FaultState::NORMAL;

                    rule.activeFaultIndex = std::nullopt;
                }

                rule.state = FaultState::NORMAL;
                rule.abnormalCount = 0;
                rule.normalCount = 0;

                std::cout << "[" << dtcCode << "] "
                          << description
                          << " cleared\n";
            }
        } else {
            rule.state = FaultState::CONFIRMED_FAULT;
            rule.normalCount = 0;

            std::cout << "[" << dtcCode << "] "
                      << description
                      << " returned during recovery\n";
        }
        break;
    }
}
void DiagnosticEngine::process(const TelemetryMessage& message) {
    switch (message.sensor) {

    case SensorType::ENGINE_TEMPERATURE: {
        const bool abnormal = message.value > 110.0;

        updateFaultState(
            overheating_,
            abnormal,
            "DTC_E001",
            "Engine overheating",
            Severity::CRITICAL,
            message.timestamp
        );

        break;
    }

    case SensorType::RPM: {
        const bool abnormal = message.value > 6000.0;

        updateFaultState(
            excessiveRpm_,
            abnormal,
            "DTC_E002",
            "Excessive RPM",
            Severity::WARNING,
            message.timestamp
        );

        break;
    }

    case SensorType::BATTERY_VOLTAGE: {
        const bool abnormal = message.value < 11.5;

        updateFaultState(
            lowBattery_,
            abnormal,
            "DTC_B001",
            "Low battery voltage",
            Severity::CRITICAL,
            message.timestamp
        );

        break;
    }

    case SensorType::ACCELERATION:
    updateFaultState(
        excessiveAcceleration_,
        message.value > 4.0,
        "DTC_V001",
        "Excessive vehicle acceleration",
        Severity::WARNING,
        message.timestamp
    );
    break;

    default:
        break;
    }
}


void DiagnosticEngine::printFaultHistory() const {
    std::cout << "\n=== Fault History ===\n";

    if (faultHistory_.empty()) {
        std::cout << "No confirmed faults detected.\n";
        return;
    }

    for (const Fault& fault : faultHistory_) {
        std::cout << "\nDTC: " << fault.code << '\n';
        std::cout << "Description: " << fault.description << '\n';

        std::cout << "Severity: "
          << toString(fault.severity)
          << '\n';

        

        if (fault.endTime.has_value()) {
            auto duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    fault.endTime.value() - fault.startTime
                );

            std::cout << "Duration: "
                      << duration.count()
                      << " ms\n";
        } else {
            std::cout << "Status: ACTIVE\n";
        }
    }
}

FaultState DiagnosticEngine::getStateFor(SensorType sensor) const {
    switch (sensor) {
    case SensorType::ENGINE_TEMPERATURE:
        return overheating_.state;

    case SensorType::RPM:
        return excessiveRpm_.state;

    case SensorType::BATTERY_VOLTAGE:
        return lowBattery_.state;
    case SensorType::ACCELERATION:
        return excessiveAcceleration_.state;

    default:
        return FaultState::NORMAL;
    }
}

const std::vector<Fault>& DiagnosticEngine::getFaultHistory() const {
    return faultHistory_;
}

HealthStatus DiagnosticEngine::getEngineHealth() const {
    bool overheatingActive =
        overheating_.state == FaultState::CONFIRMED_FAULT ||
        overheating_.state == FaultState::RECOVERING;

    bool excessiveRpmActive =
        excessiveRpm_.state == FaultState::CONFIRMED_FAULT ||
        excessiveRpm_.state == FaultState::RECOVERING;

    if (overheatingActive) {
        return HealthStatus::CRITICAL;
    }

    if (excessiveRpmActive) {
        return HealthStatus::DEGRADED;
    }

    return HealthStatus::HEALTHY;
}

HealthStatus DiagnosticEngine::getBatteryHealth() const {
    bool lowBatteryActive =
        lowBattery_.state == FaultState::CONFIRMED_FAULT ||
        lowBattery_.state == FaultState::RECOVERING;

    if (lowBatteryActive) {
        return HealthStatus::CRITICAL;
    }

    return HealthStatus::HEALTHY;
}

HealthStatus DiagnosticEngine::getVehicleDynamicsHealth() const {
    bool excessiveAccelerationActive =
        excessiveAcceleration_.state == FaultState::CONFIRMED_FAULT ||
        excessiveAcceleration_.state == FaultState::RECOVERING;

    if (excessiveAccelerationActive) {
        return HealthStatus::DEGRADED;
    }

    return HealthStatus::HEALTHY;
}