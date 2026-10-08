#include "storage/FaultLogger.hpp"
#include "utils/EnumStrings.hpp"

#include <chrono>

FaultLogger::FaultLogger(const std::string& filePath)
    : file_(filePath) {

    file_ << "code,description,severity,start_time,end_time,duration_ms\n";
}

void FaultLogger::log(const Fault& fault) {
    auto startTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            fault.startTime.time_since_epoch()
        ).count();

    file_
        << fault.code << ','
        << fault.description << ','
        << toString(fault.severity) << ','
        << startTime << ',';

    if (fault.endTime.has_value()) {
        auto endTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                fault.endTime.value().time_since_epoch()
            ).count();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                fault.endTime.value() - fault.startTime
            ).count();

        file_ << endTime << ','
              << duration;
    } else {
        file_ << "ACTIVE,ACTIVE";
    }

    file_ << '\n';
}