#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

#include "TelemetryMessage.hpp"

class MessageBus {
private:
    std::queue<TelemetryMessage> messages_;
    std::mutex mutex_;
    std::condition_variable condition_;
    bool closed_ = false;

public:
    void publish(const TelemetryMessage& message);
    std::optional<TelemetryMessage> waitAndPop();
    void close();

};