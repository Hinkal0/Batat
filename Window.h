#pragma once
#include <SFML/Graphics.hpp>
#include <string>

struct WinConfig {
  std::string title = "Title";
  uint32_t w = 800, h = 600;
  uint32_t minW = 600, minH = 600;
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