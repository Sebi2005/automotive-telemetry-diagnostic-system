#include "storage/TelemetryLogger.hpp"
#include "utils/EnumStrings.hpp"

#include <chrono>

TelemetryLogger::TelemetryLogger(const std::string& filePath)
    : file_(filePath) {

    file_ << "timestamp,ecu,sensor,value\n";
}

void TelemetryLogger::log(const TelemetryMessage& message) {
    auto timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            message.timestamp.time_since_epoch()
        ).count();

    file_
        << timestamp << ','
        << toString(message.source) << ','
        << toString(message.sensor) << ','
        << message.value << '\n';
}
