# Automotive Telemetry & Diagnostic System

A multithreaded C++17 automotive telemetry simulation and diagnostic system.

The project simulates multiple Electronic Control Units (ECUs) that produce
sensor telemetry concurrently. Messages are transmitted through a thread-safe
message bus and processed by a centralized telemetry pipeline that performs
statistics, diagnostic fault detection, anomaly detection, health monitoring,
and CSV logging.

## Main Features

- Multithreaded ECU simulation
- Thread-safe producer-consumer message bus
- Diagnostic Trouble Code (DTC) detection
- Diagnostic fault state machine
- Rolling statistics and z-score anomaly detection
- ECU health status monitoring
- Telemetry, fault, and anomaly CSV logging
- GoogleTest unit tests
- CMake-based build system

## Architecture

```text
EngineECU --------\
BatteryECU --------> MessageBus ---> TelemetryProcessor
VehicleDynamicsECU-/                     |
                                        +--> DiagnosticEngine
                                        +--> Rolling Statistics
                                        +--> Anomaly Detection
                                        +--> CSV Logging
```

Each ECU runs in its own producer thread and publishes standardized
`TelemetryMessage` objects to a thread-safe `MessageBus`.

A consumer thread retrieves messages from the queue and forwards them to the
`TelemetryProcessor`, which coordinates statistics, diagnostic fault detection,
anomaly detection, health monitoring, and persistent logging.

## Simulated ECUs and Sensors

### Engine ECU

Simulated sensors:

- RPM
- Engine temperature
- Throttle position

### Battery ECU

Simulated sensors:

- Battery voltage
- Battery current

### Vehicle Dynamics ECU

Simulated sensors:

- Vehicle speed
- Acceleration

## Simulation Scenarios

The system supports several predefined scenarios:

- `NORMAL`
- `ENGINE_OVERHEAT`
- `BATTERY_FAILURE`
- `AGGRESSIVE_DRIVING`

Each scenario modifies simulated ECU sensor values in order to test diagnostic
rules, state transitions, anomaly detection, and recovery behavior.

## Diagnostic System

The diagnostic system evaluates incoming telemetry using rule-based thresholds.

Current Diagnostic Trouble Codes:

| DTC | Condition | Severity |
|---|---|---|
| `DTC_E001` | Engine temperature above 110 °C | CRITICAL |
| `DTC_E002` | Engine RPM above 6000 RPM | WARNING |
| `DTC_B001` | Battery voltage below 11.5 V | CRITICAL |
| `DTC_V001` | Vehicle acceleration above 4.0 m/s² | WARNING |

A fault is not immediately confirmed after a single abnormal measurement.

Each diagnostic rule follows a state machine:

```text
NORMAL
   |
   | abnormal value
   v
SUSPECTED
   |
   | repeated abnormal values
   v
CONFIRMED_FAULT
   |
   | normal value
   v
RECOVERING
   |
   | repeated normal values
   v
NORMAL
```

Three consecutive abnormal values are required to confirm a fault.

After a fault is confirmed, three consecutive normal values are required before
the system considers the fault cleared.

This reduces false positives caused by temporary sensor spikes.

## Health Monitoring

The system derives a current health status for each major subsystem:

- `HEALTHY`
- `DEGRADED`
- `CRITICAL`

Examples:

- Active engine overheating -> `CRITICAL`
- Active excessive RPM -> `DEGRADED`
- Active low battery voltage -> `CRITICAL`
- Active excessive acceleration -> `DEGRADED`

Health status represents the current system condition, while the fault history
preserves faults that occurred earlier during the simulation.

## Anomaly Detection

In addition to fixed diagnostic thresholds, the project includes statistical
anomaly detection.

Each monitored sensor maintains a rolling window of recent values.

The detector calculates:

```text
mean = average of recent values
standard deviation = variation of recent values

z-score = (current value - mean) / standard deviation
```

A value is considered anomalous when its absolute z-score exceeds the configured
threshold.

The current implementation uses:

```text
Rolling window size: 5
Z-score threshold: 3.0
```

The new value is compared against the previous rolling window before being added
to it. This prevents the current anomaly from immediately influencing its own
baseline.

This allows the system to detect unusual behavior even before a fixed diagnostic
threshold is reached.

## Threading Model

The project uses multiple threads to simulate independent automotive ECUs.

```text
Engine ECU thread
Battery ECU thread
Vehicle Dynamics ECU thread
        |
        v
Thread-safe MessageBus
        |
        v
Consumer thread
        |
        v
TelemetryProcessor
```

The `MessageBus` uses:

- `std::mutex`
- `std::condition_variable`
- `std::queue`
- `std::optional`

Each ECU uses:

- `std::thread`
- `std::atomic<bool>`

The message bus also supports graceful shutdown. After all producers stop, the
bus is closed and the consumer drains any remaining messages before terminating.

## Telemetry Message

All ECUs communicate through a common message format:

```cpp
struct TelemetryMessage {
    ECUType source;
    SensorType sensor;
    double value;
    std::chrono::system_clock::time_point timestamp;
};
```

This keeps ECU implementations decoupled from diagnostics and telemetry
processing.

## Data Logging

The application stores generated data in CSV files.

```text
data/
├── telemetry.csv
├── faults.csv
└── anomalies.csv
```

### telemetry.csv

Contains all received telemetry messages:

```csv
timestamp,ecu,sensor,value
1791387366200,ENGINE,RPM,1100
1791387366200,ENGINE,ENGINE_TEMPERATURE,85.2
1791387366200,ENGINE,THROTTLE_POSITION,22
```

### faults.csv

Contains confirmed diagnostic faults:

```csv
code,description,severity,start_time,end_time,duration_ms
DTC_B001,Low battery voltage,CRITICAL,1791387370443,1791387373270,2827
```

### anomalies.csv

Contains statistically detected anomalies:

```csv
timestamp,sensor,value,z_score
1791387369739,BATTERY_VOLTAGE,11.8,-12.5
```

CSV files are generated at runtime and are excluded from Git.

## Final Diagnostic Report

At the end of a simulation, the application prints a summary containing:

- Engine health status
- Battery health status
- Vehicle Dynamics health status
- Total confirmed faults
- Total detected anomalies
- Sensor statistics
- Fault history
- Anomaly history

Example:

```text
========== FINAL DIAGNOSTIC REPORT ==========
Engine: HEALTHY
Battery: CRITICAL
Vehicle Dynamics: HEALTHY

Total faults: 1
Total anomalies: 3
=============================================
```

## Testing

The project uses GoogleTest for automated unit testing.

Current tests cover:

- Rolling statistics
- Rolling-window behavior
- Standard deviation calculations
- Statistical anomaly detection
- Diagnostic state transitions
- Fault confirmation
- Fault recovery
- ECU health status
- Vehicle Dynamics diagnostic rules
- MessageBus publish/pop behavior
- Graceful MessageBus shutdown
- Producer-consumer synchronization

GoogleTest is automatically downloaded through CMake `FetchContent`.

## Project Structure

```text
Automotive-Telemetry-Diagnostic-System/
│
├── include/
│   ├── core/
│   │   ├── MessageBus.hpp
│   │   └── TelemetryMessage.hpp
│   │
│   ├── diagnostics/
│   │   ├── DiagnosticEngine.hpp
│   │   ├── Fault.hpp
│   │   ├── FaultState.hpp
│   │   ├── HealthStatus.hpp
│   │   └── Severity.hpp
│   │
│   ├── ecu/
│   │   ├── BatteryECU.hpp
│   │   ├── EngineECU.hpp
│   │   └── VehicleDynamicsECU.hpp
│   │
│   ├── simulation/
│   │   └── SimulationScenario.hpp
│   │
│   ├── statistics/
│   │   ├── AnomalyDetector.hpp
│   │   ├── AnomalyEvent.hpp
│   │   └── RollingStatistics.hpp
│   │
│   ├── storage/
│   │   ├── AnomalyLogger.hpp
│   │   ├── FaultLogger.hpp
│   │   └── TelemetryLogger.hpp
│   │
│   ├── telemetry/
│   │   └── TelemetryProcessor.hpp
│   │
│   └── utils/
│       └── EnumStrings.hpp
│
├── src/
│   ├── core/
│   ├── diagnostics/
│   ├── ecu/
│   ├── statistics/
│   ├── storage/
│   ├── telemetry/
│   ├── utils/
│   └── main.cpp
│
├── tests/
│   ├── test_anomaly_detector.cpp
│   ├── test_diagnostic_engine.cpp
│   ├── test_message_bus.cpp
│   └── test_rolling_statistics.cpp
│
├── data/
│   └── .gitkeep
│
├── CMakeLists.txt
├── .gitignore
└── README.md
```

## Build

Requirements:

- C++17 compatible compiler
- CMake 3.20+
- Internet access during the first configuration step for GoogleTest

Configure the project:

```powershell
cmake -S . -B build
```

Build:

```powershell
cmake --build build
```

Run the application:

```powershell
.\build\Debug\automotive_telemetry.exe
```

Run the tests:

```powershell
ctest --test-dir build -C Debug
```

## CMake Architecture

The shared application code is compiled into the internal library:

```text
automotive_core
```

Both the main application and the GoogleTest executable link against this
library:

```text
                  automotive_core
                  /             \
                 /               \
automotive_telemetry       automotive_tests
```

This avoids compiling and listing the same implementation files separately for
the application and tests.

## Technologies

- C++17
- CMake
- GoogleTest
- C++ Standard Library
- Multithreading
- Producer-consumer architecture
- Statistical anomaly detection
- CSV-based telemetry persistence