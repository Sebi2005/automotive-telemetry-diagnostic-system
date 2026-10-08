#pragma once

#include "core/MessageBus.hpp"
#include "simulation/SimulationScenario.hpp"
#include <thread>
#include <atomic>
class EngineECU {
    public:
        explicit EngineECU(MessageBus& bus, SimulationScenario scenario); //the reference is used so all ECUs can share the same message bus, explicit is used to prevent implicit conversions

        void start();
        void stop();

    private:
        void run();
        
        MessageBus& bus_;
        SimulationScenario scenario_;
        std::thread thread_;
        std::atomic<bool> running_{false}; //ecu starts in the stopped state, atomic is used for thread-safe access to the running state
     
};
