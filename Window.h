#pragma once
#include <SFML/Graphics.hpp>

struct WinConfig {
  const char* title;
  uint32_t w, h;
  uint32_t minW, minH;
  bool fullscreen = false;
};

class Window {
  sf::RenderWindow handle;
  sf::String title;
  sf::Vector2u size;
  sf::Vector2u minSize;
  bool fullscreen;
public:
  Window() = default;
  Window(const WinConfig& config);

  bool isOpen() const;

  void pollEvents();

  void beginDraw();
  void endDraw();
};