#pragma once
#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

struct WinConfig {
  uint32_t w = 960, h = 540;
  bool fullscreen = true;
  std::string title = "Batat";
};

struct GameConfig {
  WinConfig window;
  int volume = 100;
  std::optional<std::chrono::milliseconds> bestTime;
};