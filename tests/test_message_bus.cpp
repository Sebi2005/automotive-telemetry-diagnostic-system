#include "gtest/gtest.h"
#include "core/MessageBus.hpp"
#include <thread>
#include <chrono>
#include <optional>

TEST(MessageBusTest, PublishesAndPopsMessage) {
    MessageBus bus;

    TelemetryMessage message{
        ECUType::ENGINE,
        SensorType::RPM,
        2500.0,
        std::chrono::system_clock::now()
    };

    bus.publish(message);

    auto result = bus.waitAndPop();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->source, ECUType::ENGINE);
    EXPECT_EQ(result->sensor, SensorType::RPM);
    EXPECT_DOUBLE_EQ(result->value, 2500.0);
}

TEST(MessageBusTest, ReturnsNulloptAfterCloseWhenEmpty) {
    MessageBus bus;

    bus.close();

    auto result = bus.waitAndPop();

    EXPECT_FALSE(result.has_value());
}

TEST(MessageBusTest, DrainsQueuedMessagesAfterClose) {
    MessageBus bus;

    TelemetryMessage message{
        ECUType::BATTERY,
        SensorType::BATTERY_VOLTAGE,
        12.4,
        std::chrono::system_clock::now()
    };

    bus.publish(message);
    bus.close();

    auto first = bus.waitAndPop();
    ASSERT_TRUE(first.has_value());
    EXPECT_DOUBLE_EQ(first->value, 12.4);

    auto second = bus.waitAndPop();
    EXPECT_FALSE(second.has_value());
}

TEST(MessageBusTest, ConsumerWaitsUntilProducerPublishes) {
    MessageBus bus;

    std::optional<TelemetryMessage> receivedMessage;

    std::thread consumer([&]() {
        receivedMessage = bus.waitAndPop();
    });

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100) // main thread sleeps for 100 milliseconds to ensure the consumer is waiting
    );

    TelemetryMessage message{
        ECUType::ENGINE,
        SensorType::RPM,
        3000.0,
        std::chrono::system_clock::now()
    };

    bus.publish(message); //notify the consumer that a message is available

    consumer.join();

    ASSERT_TRUE(receivedMessage.has_value());
    EXPECT_EQ(receivedMessage->sensor, SensorType::RPM);
    EXPECT_DOUBLE_EQ(receivedMessage->value, 3000.0);
}