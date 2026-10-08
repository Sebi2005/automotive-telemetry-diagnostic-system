#pragma once

#include <fstream>
#include <string>

#include "diagnostics/Fault.hpp"

class FaultLogger {
public:
    explicit FaultLogger(const std::string& filePath);

    void log(const Fault& fault);

private:
    std::ofstream file_;
};