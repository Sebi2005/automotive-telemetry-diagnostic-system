#pragma once

#include <optional>
#include <vector>

#include "core/TelemetryMessage.hpp"
#include "diagnostics/FaultState.hpp"
#include "diagnostics/Severity.hpp"
#include "diagnostics/Fault.hpp"
#include "utils/EnumStrings.hpp"


struct DiagnosticRuleState {
    FaultState state = FaultState::NORMAL;
    int abnormalCount = 0;
    int normalCount = 0;

    std::optional<std::size_t> activeFaultIndex;

};

class DiagnosticEngine {
public:
    void process(const TelemetryMessage& message);
    void printFaultHistory() const;
    FaultState getStateFor(SensorType sensor) const;
    const std::vector<Fault>& getFaultHistory() const; //the first const doesnt let you modify the vector e.g. diagnosticEngine.getFaultHistory().clear();, the reference is used to avoid copying the vector
    HealthStatus getEngineHealth() const;
    HealthStatus getBatteryHealth() const;
    HealthStatus getVehicleDynamicsHealth() const;

private:
    void updateFaultState(
        DiagnosticRuleState& rule,
        bool abnormal,
        const char* dtcCode,
        const char* description,
        Severity severity,
        std::chrono::system_clock::time_point timestamp
    );

    DiagnosticRuleState overheating_;
    DiagnosticRuleState excessiveRpm_;
    DiagnosticRuleState lowBattery_;
    DiagnosticRuleState excessiveAcceleration_;

    std::vector<Fault> faultHistory_;
};
