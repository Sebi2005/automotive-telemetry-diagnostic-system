#include "ecu/BatteryECU.hpp"
#include "thread"
#include "chrono"

BatteryECU::BatteryECU(MessageBus& bus, SimulationScenario scenario) : bus_(bus), scenario_(scenario) {}

void BatteryECU::start() {
    running_ = true;
    thread_ = std::thread(&BatteryECU::run, this);
}

void BatteryECU::run() {
    double voltage = 13.0;
    double current = 20.0;

    int cycle = 0;

    while (running_) {
        ++cycle;

        switch (scenario_) {
        case SimulationScenario::NORMAL:
            voltage = 13.0 + (cycle % 2 == 0 ? 0.05 : -0.05);
            current = 20.0 + (cycle % 2 == 0 ? 0.2 : -0.2);
            break;

        case SimulationScenario::BATTERY_FAILURE:
            if (cycle <= 5) {
                voltage = 13.0 + (cycle % 2 == 0 ? 0.05 : -0.05);
                current = 20.0 + (cycle % 2 == 0 ? 0.2 : -0.2);
            }
            else if (cycle == 6) {
                voltage = 11.8;   // anomaly, dar încă peste pragul DTC de 11.5
                current = 24.0;
            }
            else if (cycle == 7) {
                voltage = 11.3;   // începe DTC_B001
                current = 26.0;
            }
            else if (cycle == 8) {
                voltage = 10.9;
                current = 28.0;
            }
            else if (cycle == 9) {
                voltage = 10.5;   // a treia valoare anormală -> confirmare
                current = 30.0;
            }
            else {
                voltage = std::min(13.0, voltage + 0.6);
                current = std::max(20.0, current - 2.0);
            }

            break;

        default:
            voltage = 13.0 + (cycle % 2 == 0 ? 0.05 : -0.05);
            current = 20.0;
            break;
        }

        TelemetryMessage voltageMessage{
            ECUType::BATTERY,
            SensorType::BATTERY_VOLTAGE,
            voltage,
            std::chrono::system_clock::now()
        };

        TelemetryMessage currentMessage{
            ECUType::BATTERY,
            SensorType::BATTERY_CURRENT,
            current,
            std::chrono::system_clock::now()
        };

        bus_.publish(voltageMessage);
        bus_.publish(currentMessage);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(700)
        );
    }
}

void BatteryECU::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}
