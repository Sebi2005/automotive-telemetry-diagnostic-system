#include <iostream>
#include <thread>
#include <chrono>

#include "core/MessageBus.hpp"
#include "ecu/EngineECU.hpp"
#include "ecu/BatteryECU.hpp"
#include "ecu/VehicleDynamicsECU.hpp"
#include "telemetry/TelemetryProcessor.hpp"
#include "simulation/SimulationScenario.hpp"
#include "storage/FaultLogger.hpp"
#include "storage/AnomalyLogger.hpp"

int main() {
    MessageBus bus;
    SimulationScenario scenario = SimulationScenario::AGGRESSIVE_DRIVING; 
    EngineECU engine(bus, scenario);
    VehicleDynamicsECU vehicleDynamics(bus, scenario);
    BatteryECU battery(bus, scenario);
    TelemetryProcessor processor;

    engine.start();
    battery.start();
    vehicleDynamics.start();

    

    std::thread consumer([&bus, &processor]() {
        while (true) {
            auto message = bus.waitAndPop();
            if (!message.has_value()) {
                break; // Exit the loop if the bus is closed and there are no messages left
            }
            processor.process(message.value());
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(8)); // main thread sleeps for 8 seconds to allow ECUs to generate messages
    
    engine.stop();
    battery.stop();
    vehicleDynamics.stop();
    
    bus.close();
    consumer.join();

    FaultLogger faultLogger("data/faults.csv");

    for (const Fault& fault : processor.getFaultHistory()) {
        faultLogger.log(fault);
    }

    AnomalyLogger anomalyLogger("data/anomalies.csv");

    for (const AnomalyEvent& anomaly : processor.getAnomalies()) {
        anomalyLogger.log(anomaly);
    }

    processor.printFinalReport();
    processor.printStatistics();
    processor.printDiagnostics();
    processor.printAnomalies();

    return 0;
}