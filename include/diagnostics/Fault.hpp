#pragma once

#include <string>
#include <chrono>
#include <optional>

#include "diagnostics/FaultState.hpp"
#include "diagnostics/Severity.hpp"

struct Fault {
    std::string code;
    std::string description;
    Severity severity;
    FaultState state;

    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
};