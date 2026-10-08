#include "ecu/VehicleDynamicsECU.hpp"
#include "thread"
#include "chrono"

VehicleDynamicsECU::VehicleDynamicsECU(MessageBus& bus, SimulationScenario scenario) : bus_(bus), scenario_(scenario) {}

void VehicleDynamicsECU::start() {
    running_ = true;
    thread_ = std::thread(&VehicleDynamicsECU::run, this);
}

void VehicleDynamicsECU::run() {
    double speed = 20.0;
    double acceleration = 1.5;

    int cycle = 0;

    while (running_) {
        ++cycle;

        switch (scenario_) {
        case SimulationScenario::NORMAL:
            speed = 50.0 + (cycle % 2 == 0 ? 1.0 : -1.0);
            acceleration = 1.0 + (cycle % 2 == 0 ? 0.1 : -0.1);
            break;

        case SimulationScenario::AGGRESSIVE_DRIVING:
            if (cycle <= 5) {
                speed = 50.0 + (cycle % 2 == 0 ? 1.0 : -1.0);
                acceleration = 1.0 + (cycle % 2 == 0 ? 0.1 : -0.1);
            }
            else if (cycle == 6) {
                speed = 85.0;
                acceleration = 4.5;
            }
            else if (cycle == 7) {
                speed = 110.0;
                acceleration = 5.5;
            }
            else if (cycle == 8) {
                speed = 135.0;
                acceleration = 6.0;
            }
            else {
                speed = std::max(50.0, speed - 20.0);
                acceleration = std::max(1.0, acceleration - 1.5);
            }

            break;

        default:
            speed = 50.0;
            acceleration = 1.0;
            break;
        }

        TelemetryMessage speedMessage{
            ECUType::VEHICLE_DYNAMICS,
            SensorType::SPEED,
            speed,
            std::chrono::system_clock::now()
        };

        TelemetryMessage accelerationMessage{
            ECUType::VEHICLE_DYNAMICS,
            SensorType::ACCELERATION,
            acceleration,
            std::chrono::system_clock::now()
        };

        bus_.publish(speedMessage);
        bus_.publish(accelerationMessage);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(600)
        );
    }
}

void VehicleDynamicsECU::stop() {
    running_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}
