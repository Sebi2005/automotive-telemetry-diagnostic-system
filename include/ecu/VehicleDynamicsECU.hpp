#pragma once

#include <atomic>
#include <thread>

#include "core/MessageBus.hpp"
#include "simulation/SimulationScenario.hpp"

class VehicleDynamicsECU {
public:
    explicit VehicleDynamicsECU(MessageBus& bus, SimulationScenario scenario); //the reference is used so all ECUs can share the same message bus, explicit is used to prevent implicit conversions

    void start();
    void stop();

private:
    void run();

    MessageBus& bus_;
    SimulationScenario scenario_;
    std::thread thread_;
    std::atomic<bool> running_{false};
};