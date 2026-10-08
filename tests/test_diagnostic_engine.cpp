#include <gtest/gtest.h>

#include "diagnostics/DiagnosticEngine.hpp"

TEST(DiagnosticEngineTest, StartsInNormalState) {
    DiagnosticEngine engine;

    EXPECT_EQ(
        engine.getStateFor(SensorType::ENGINE_TEMPERATURE),
        FaultState::NORMAL
    );
}

TEST(DiagnosticEngineTest, MovesToSuspectedAfterFirstAbnormalTemperature) {
    DiagnosticEngine engine;

    TelemetryMessage message{
        ECUType::ENGINE,
        SensorType::ENGINE_TEMPERATURE,
        115.0,
        std::chrono::system_clock::now()
    };

    engine.process(message);

    EXPECT_EQ(
        engine.getStateFor(SensorType::ENGINE_TEMPERATURE),
        FaultState::SUSPECTED
    );
}

TEST(DiagnosticEngineTest, RecoversAfterThreeNormalTemperatures) {
    DiagnosticEngine engine;

    for (int i = 0; i < 3; ++i) {
        engine.process({
            ECUType::ENGINE,
            SensorType::ENGINE_TEMPERATURE,
            120.0,
            std::chrono::system_clock::now()
        });
    }

    EXPECT_EQ(
        engine.getStateFor(SensorType::ENGINE_TEMPERATURE),
        FaultState::CONFIRMED_FAULT
    );

    for (int i = 0; i < 3; ++i) {
        engine.process({
            ECUType::ENGINE,
            SensorType::ENGINE_TEMPERATURE,
            100.0,
            std::chrono::system_clock::now()
        });
    }

    EXPECT_EQ(
        engine.getStateFor(SensorType::ENGINE_TEMPERATURE),
        FaultState::NORMAL
    );
}

TEST(DiagnosticEngineTest, ClearsSuspicionIfNextValueIsNormal) {
    DiagnosticEngine engine;

    engine.process({
        ECUType::ENGINE,
        SensorType::ENGINE_TEMPERATURE,
        120.0,
        std::chrono::system_clock::now()
    });

    engine.process({
        ECUType::ENGINE,
        SensorType::ENGINE_TEMPERATURE,
        100.0,
        std::chrono::system_clock::now()
    });

    EXPECT_EQ(
        engine.getStateFor(SensorType::ENGINE_TEMPERATURE),
        FaultState::NORMAL
    );
}

TEST(DiagnosticEngineTest, EngineHealthBecomesCriticalAfterConfirmedOverheat) {
    DiagnosticEngine engine;

    auto now = std::chrono::system_clock::now();

    engine.process({ECUType::ENGINE, SensorType::ENGINE_TEMPERATURE, 120.0, now});
    engine.process({ECUType::ENGINE, SensorType::ENGINE_TEMPERATURE, 121.0, now});
    engine.process({ECUType::ENGINE, SensorType::ENGINE_TEMPERATURE, 122.0, now});

    EXPECT_EQ(engine.getEngineHealth(), HealthStatus::CRITICAL);
}

TEST(DiagnosticEngineTest, BatteryHealthBecomesCriticalAfterConfirmedLowVoltage) {
    DiagnosticEngine engine;

    auto now = std::chrono::system_clock::now();

    engine.process({ECUType::BATTERY, SensorType::BATTERY_VOLTAGE, 11.0, now});
    engine.process({ECUType::BATTERY, SensorType::BATTERY_VOLTAGE, 10.9, now});
    engine.process({ECUType::BATTERY, SensorType::BATTERY_VOLTAGE, 10.8, now});

    EXPECT_EQ(engine.getBatteryHealth(), HealthStatus::CRITICAL);
}

TEST(DiagnosticEngineTest, VehicleDynamicsBecomesDegradedAfterExcessiveAcceleration) {
    DiagnosticEngine engine;

    auto now = std::chrono::system_clock::now();

    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 4.5, now});
    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 5.0, now});
    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 5.5, now});

    EXPECT_EQ(
        engine.getVehicleDynamicsHealth(),
        HealthStatus::DEGRADED
    );
}

TEST(DiagnosticEngineTest, VehicleDynamicsReturnsToHealthyAfterRecovery) {
    DiagnosticEngine engine;

    auto now = std::chrono::system_clock::now();

    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 4.5, now});
    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 5.0, now});
    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 5.5, now});

    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 1.0, now});
    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 1.0, now});
    engine.process({ECUType::VEHICLE_DYNAMICS, SensorType::ACCELERATION, 1.0, now});

    EXPECT_EQ(
        engine.getVehicleDynamicsHealth(),
        HealthStatus::HEALTHY
    );
}