cmake_minimum_required(VERSION 3.20)

project(AutomotiveTelemetrySystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(automotive_core
    src/core/MessageBus.cpp
    src/ecu/EngineECU.cpp
    src/ecu/BatteryECU.cpp
    src/ecu/VehicleDynamicsECU.cpp
    src/telemetry/TelemetryProcessor.cpp
    src/diagnostics/DiagnosticEngine.cpp
    src/statistics/RollingStatistics.cpp
    src/statistics/AnomalyDetector.cpp
    src/utils/EnumStrings.cpp
    src/storage/TelemetryLogger.cpp
    src/storage/FaultLogger.cpp
    src/storage/AnomalyLogger.cpp
)

target_include_directories(automotive_core
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(automotive_telemetry
    src/main.cpp
)

target_link_libraries(
    automotive_telemetry
    PRIVATE automotive_core
)


# we use fetchcontent so we dont download google test manually
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# creating the test executable
add_executable(automotive_tests
    tests/test_rolling_statistics.cpp
    tests/test_anomaly_detector.cpp
    tests/test_diagnostic_engine.cpp
    tests/test_message_bus.cpp

    
)

# linking it with google test  
target_link_libraries(
    automotive_tests
    PRIVATE
        automotive_core
        GTest::gtest_main
)


include(GoogleTest)

gtest_discover_tests(automotive_tests)