#pragma once

#include "core/TelemetryMessage.hpp"
#include "diagnostics/DiagnosticEngine.hpp"
#include "statistics/AnomalyDetector.hpp"
#include "statistics/AnomalyEvent.hpp"
#include "storage/TelemetryLogger.hpp"
#include <vector>
#include <map>
#include <cstddef>
#include <algorithm>

struct SensorStatistics {
    std::size_t count = 0;
    double minValue = 0.0;
    double maxValue = 0.0;

};

class TelemetryProcessor {
public:
    void process(const TelemetryMessage& message);
    void printStatistics() const;
    void printDiagnostics() const;
    void printAnomalies() const;
    const std::vector<Fault>& getFaultHistory() const;
    const std::vector<AnomalyEvent>& getAnomalies() const;
    void printFinalReport() const;
private:
    std::map<SensorType, SensorStatistics> statistics_;
    DiagnosticEngine diagnosticEngine_;
    AnomalyDetector engineTemperatureDetector_{5, 3.0};
    AnomalyDetector rpmDetector_{5, 3.0};
    AnomalyDetector batteryVoltageDetector_{5, 3.0};
    AnomalyDetector accelerationDetector_{5, 3.0};
    std::vector<AnomalyEvent> anomalies_;   
    TelemetryLogger logger_{"data/telemetry.csv"};

};