#include "storage/AnomalyLogger.hpp"
#include "utils/EnumStrings.hpp"

#include <chrono>

AnomalyLogger::AnomalyLogger(const std::string& filePath)
    : file_(filePath) {

    file_ << "timestamp,sensor,value,z_score\n";
}

void AnomalyLogger::log(const AnomalyEvent& anomaly) {
    auto timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            anomaly.timestamp.time_since_epoch()
        ).count();

    file_
        << timestamp << ','
        << toString(anomaly.sensor) << ','
        << anomaly.value << ','
        << anomaly.zScore << '\n';
}