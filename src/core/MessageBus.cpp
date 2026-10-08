#include "core/MessageBus.hpp"

void MessageBus::publish(const TelemetryMessage& message) {
    {
        std::lock_guard<std::mutex> lock(mutex_); //lock_guard is used to automatically release the lock when the scope ends
        messages_.push(message);
    }
    condition_.notify_one();
}

std::optional<TelemetryMessage> MessageBus::waitAndPop() {
    std::unique_lock<std::mutex> lock(mutex_);
    condition_.wait(lock, [this] {
        return !messages_.empty() || closed_;
    });
    if (closed_ && messages_.empty()) {
        return std::nullopt; // Return an empty optional if the bus is closed and there are no messages left
    }
    TelemetryMessage message = messages_.front();
    messages_.pop();
    return message;
}

void MessageBus::close() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        closed_ = true;
    }
    condition_.notify_all(); //Folosim notify_all(), nu notify_one(), pentru că la shutdown vrem ca toți consumerii care așteaptă să afle că bus-ul s-a închis.
}