#pragma once

#include <fstream>
#include <string>

#include "statistics/AnomalyEvent.hpp"

class AnomalyLogger {
public:
    explicit AnomalyLogger(const std::string& filePath);

    void log(const AnomalyEvent& anomaly);

private:
    std::ofstream file_;
};