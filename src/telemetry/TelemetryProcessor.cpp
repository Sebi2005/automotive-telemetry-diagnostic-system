#include "telemetry/TelemetryProcessor.hpp"
#include "utils/EnumStrings.hpp"

#include <algorithm>
#include <iostream>


void TelemetryProcessor::process(const TelemetryMessage& message) {
    SensorStatistics& stats = statistics_[message.sensor];

    if (stats.count == 0) {
        stats.minValue = message.value;
        stats.maxValue = message.value;
    } else {
        stats.minValue = std::min(stats.minValue, message.value);
        stats.maxValue = std::max(stats.maxValue, message.value);
    }

    ++stats.count;
    std::optional<double> zScore;

    switch (message.sensor) {

    case SensorType::ENGINE_TEMPERATURE:
        zScore = engineTemperatureDetector_.detect(message.value);
        break;

    case SensorType::RPM:
        zScore = rpmDetector_.detect(message.value);
        break;

    case SensorType::BATTERY_VOLTAGE:
        zScore = batteryVoltageDetector_.detect(message.value);
        break;
    case SensorType::ACCELERATION:
        zScore = accelerationDetector_.detect(message.value);
        break;

    default:
        break;
    }

    if (zScore.has_value()) {
        anomalies_.push_back({
            message.sensor,
            message.value,
            zScore.value(),
            message.timestamp
        });

        std::cout << "[ANOMALY] Value: "
                << message.value
                << " | z-score: "
                << zScore.value()
                << '\n';
    }
    
    diagnosticEngine_.process(message);

    switch (message.sensor) {
    case SensorType::RPM:
        std::cout << "RPM: " << message.value << '\n';
        break;

    case SensorType::ENGINE_TEMPERATURE:
        std::cout << "Engine temperature: "
                  << message.value << " C\n";
        break;

    case SensorType::THROTTLE_POSITION:
        std::cout << "Throttle position: "
                  << message.value << " %\n";
        break;
    case SensorType::BATTERY_VOLTAGE:
        std::cout << "Battery voltage: "
                  << message.value << " V\n";
        break;
    case SensorType::BATTERY_CURRENT:
        std::cout << "Battery current: "
                  << message.value << " A\n";
        break;
    case SensorType::SPEED:
        std::cout << "Speed: "
                  << message.value << " km/h\n";
        break;
    case SensorType::ACCELERATION:
        std::cout << "Acceleration: "
                  << message.value << " m/s^2\n";
        break;
    default:
        std::cout << "Unknown sensor value: "
                  << message.value << '\n';
        break;
    }
    logger_.log(message);
}


void TelemetryProcessor::printStatistics() const {
    std::cout << "\n=== Telemetry Statistics ===\n";

    for (const auto& [sensor, stats] : statistics_) {
        std::cout << toString(sensor) << '\n';

        std::cout << "  Count: " << stats.count << '\n'
                  << "  Min:   " << stats.minValue << '\n'
                  << "  Max:   " << stats.maxValue << '\n';
    }
}

void TelemetryProcessor::printDiagnostics() const {
    diagnosticEngine_.printFaultHistory();
}

void TelemetryProcessor::printAnomalies() const {
    std::cout << "\n=== Statistical Anomalies ===\n";

    if (anomalies_.empty()) {
        std::cout << "No statistical anomalies detected.\n";
        return;
    }

    for (const AnomalyEvent& anomaly : anomalies_) {
        std::cout << "Sensor: ";

        switch (anomaly.sensor) {
        case SensorType::RPM:
            std::cout << "RPM";
            break;

        case SensorType::ENGINE_TEMPERATURE:
            std::cout << "Engine Temperature";
            break;

        case SensorType::THROTTLE_POSITION:
            std::cout << "Throttle Position";
            break;

        case SensorType::BATTERY_VOLTAGE:
            std::cout << "Battery Voltage";
            break;

        case SensorType::BATTERY_CURRENT:
            std::cout << "Battery Current";
            break;

        case SensorType::SPEED:
            std::cout << "Speed";
            break;

        case SensorType::ACCELERATION:
            std::cout << "Acceleration";
            break;
        }

        std::cout << '\n'
                  << "  Value:   " << anomaly.value << '\n'
                  << "  Z-score: " << anomaly.zScore << '\n';
    }
}

const std::vector<Fault>& TelemetryProcessor::getFaultHistory() const {
    return diagnosticEngine_.getFaultHistory();
}

const std::vector<AnomalyEvent>& TelemetryProcessor::getAnomalies() const {
    return anomalies_;
}

void TelemetryProcessor::printFinalReport() const {
    std::cout << "\n========== FINAL DIAGNOSTIC REPORT ==========\n";

    std::cout << "Engine: "
              << toString(diagnosticEngine_.getEngineHealth())
              << '\n';

    std::cout << "Battery: "
              << toString(diagnosticEngine_.getBatteryHealth())
              << '\n';

    std::cout << "Vehicle Dynamics: "
              << toString(diagnosticEngine_.getVehicleDynamicsHealth())
              << '\n';

    std::cout << "\nTotal faults: "
              << diagnosticEngine_.getFaultHistory().size()
              << '\n';

    std::cout << "Total anomalies: "
              << anomalies_.size()
              << '\n';

    std::cout << "=============================================\n";
}