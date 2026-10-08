#include "ecu/EngineECU.hpp"
#include "thread"
#include "chrono"
#include "algorithm"

EngineECU::EngineECU(MessageBus& bus, SimulationScenario scenario) : bus_(bus), scenario_(scenario) {}

void EngineECU::start() {
    running_ = true;
    thread_ = std::thread(&EngineECU::run, this);
}

void EngineECU::run() {
    double rpm = 1000.0;
    double temperature = 85.0;
    double throttle = 20.0;

    int cycle = 0;

    while (running_) {
        ++cycle;

        
        switch (scenario_) {
        case SimulationScenario::NORMAL:
            rpm += 100.0;
            temperature += 0.2;
            throttle += 2.0;
            break;

        case SimulationScenario::ENGINE_OVERHEAT:
            if (cycle <= 5) {
        temperature = 90.0 + (cycle % 2 == 0 ? 0.5 : -0.5);
            }
            else if (cycle == 6) {
                temperature = 105.0;
            }
            else if (cycle == 7) {
                temperature = 115.0;
            }
            else if (cycle == 8) {
                temperature = 125.0;
            }
            else if (cycle == 9) {
                temperature = 135.0;
            }
            else {
                temperature = std::max(90.0, temperature - 15.0);
            }
            rpm += 250.0;
            throttle += 5.0;
            break;

        case SimulationScenario::AGGRESSIVE_DRIVING:
            rpm += 1200.0;
            temperature += 1.0;
            throttle += 15.0;
            break;

        case SimulationScenario::BATTERY_FAILURE:
            rpm += 100.0;
            temperature += 0.2;
            throttle += 2.0;
            break;
        }
        
        TelemetryMessage rpmMessage{
            ECUType::ENGINE,
            SensorType::RPM,
            rpm,
            std::chrono::system_clock::now()
        };

        TelemetryMessage temperatureMessage{
            ECUType::ENGINE,
            SensorType::ENGINE_TEMPERATURE,
            temperature,
            std::chrono::system_clock::now()
        };

        TelemetryMessage throttleMessage{
            ECUType::ENGINE,
            SensorType::THROTTLE_POSITION,
            throttle,
            std::chrono::system_clock::now()
        };

        bus_.publish(rpmMessage);
        bus_.publish(temperatureMessage);
        bus_.publish(throttleMessage);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(500) // Sleep for 500 milliseconds to simulate the time between telemetry messages
        );
    }
}
void EngineECU::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}
