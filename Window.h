#pragma once
#include <SFML/Graphics.hpp>

struct WinConfig {
  const char* title;
  uint32_t w, h;
  uint32_t x = 0, y = 0;
  uint32_t minW, minH;
};

class Window {
  sf::RenderWindow handle;

public:
  Window() = default;
  Window(const WinConfig& config);

  bool isOpen() const;

  void pollEvents();

  void beginDraw();
  void endDraw();
};