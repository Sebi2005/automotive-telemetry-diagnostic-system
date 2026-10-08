#pragma once

#include <string>

#include "core/TelemetryMessage.hpp"
#include "diagnostics/Severity.hpp"
#include "diagnostics/FaultState.hpp"
#include "diagnostics/HealthStatus.hpp"

std::string toString(ECUType ecu);
std::string toString(SensorType sensor);
std::string toString(Severity severity);
std::string toString(FaultState state);
std::string toString(HealthStatus status);