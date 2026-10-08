#pragma once

#include <fstream>
#include <string>

#include "core/TelemetryMessage.hpp"

class TelemetryLogger {
public:
    explicit TelemetryLogger(const std::string& filePath);

    void log(const TelemetryMessage& message);

private:
    std::ofstream file_;
};