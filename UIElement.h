#pragma once
#include <SFML/Graphics.hpp>

class UIElement : public sf::Drawable {
public:
  bool destroyed = false;

  virtual ~UIElement() = default;

  virtual void update(float deltaTime) {}
};
