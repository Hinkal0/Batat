#pragma once
#include <string>

struct WinConfig {
  std::string title = "Title";
  uint32_t w = 800, h = 600;
  uint32_t minW = 480, minH = 270;
  bool fullscreen = false;
};

struct GameConfig {
  WinConfig window;
  int volume = 100;
};