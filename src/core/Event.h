#pragma once
#include <vector>
#include <functional>

template <typename... Args>
class Event {
    std::vector<std::function<void(Args...)>> listeners;
public:
  using Callback = std::function<void(Args...)>;

  size_t addListener(Callback callback) {
    listeners.push_back(callback);
    return listeners.size() - 1;
  }

  void removeListener(size_t id) {
    listeners.erase(listeners.begin()+id);
  }

  void operator()(Args... args) const {
    for (const auto& listener : listeners) {
      listener(args...);
    }
  }
};      