#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
#include "Config.h"
#include "Scene.h"

class Window {
  sf::RenderWindow handle;
  sf::String title;
  sf::Vector2u windowedSize;
  sf::Vector2u maxWindowSize;
  std::optional<sf::Vector2u> requestedSize;

  bool fullscreen = false;
  bool cursorGrabbed = true;

  sf::View gameView;
  sf::View uiView;

  void create();
  void updateViews();
  void checkShrunk(sf::Vector2u requested, sf::Vector2u actual);

public:
  static constexpr sf::Vector2f UI_SIZE = {480.0f, 270.0f};

  Window() = default;
  Window(const WinConfig& config);

  bool isOpen() const;
  void close();

  std::optional<sf::Event> pollEvent();

  bool isFullscreen() const { return fullscreen; }
  void setFullscreen(bool value);

  std::vector<sf::Vector2u> windowSizePresets() const;

  sf::Vector2u getWindowSize() const { return windowedSize; }
  void setWindowSize(sf::Vector2u size);

  void setCursorGrabbed(bool value);

  void draw(const Scene& scene);

  sf::Vector2f getMousePos(const Scene& scene) const;
  sf::Vector2f getUIMousePos(void) const;
  sf::Vector2f mapPixelToUI(sf::Vector2i pixel) const;
};