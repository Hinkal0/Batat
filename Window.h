#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
#include "Config.h"
#include "Scene.h"
#include "Event.h"

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

  Event<sf::Keyboard::Key> onKeyPressed;
  Event<sf::Keyboard::Key> onKeyReleased;
  Event<sf::Vector2f> onMouseMoved;
  Event<sf::Mouse::Button> onMousePressed;
  Event<sf::Mouse::Button> onMouseReleased;
  Event<> onFocusLost;

  Window() = default;
  Window(const WinConfig& config);

  bool isOpen() const;
  void close();

  void pollEvents();

  bool isFullscreen() const { return fullscreen; }
  void setFullscreen(bool value);

  std::vector<sf::Vector2u> windowSizePresets() const;

  sf::Vector2u getWindowSize() const { return windowedSize; }
  void setWindowSize(sf::Vector2u size);

  void setCursorGrabbed(bool value);

  void draw(const Scene& scene);

  sf::Vector2f getMousePos(const Scene& scene) const;
  sf::Vector2f getUIMousePos(void) const;
};