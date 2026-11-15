#pragma once
#include <SFML/Graphics.hpp>

class UIElement : public sf::Drawable {
  void draw(sf::RenderTarget& target, sf::RenderStates states) const final {
    if (visible) {
      states.transform.translate(position);
      drawContents(target, states);
    }
  }

protected:
  explicit UIElement(sf::Vector2f position = {}) : position(position) {}

  virtual void drawContents(sf::RenderTarget& target, sf::RenderStates states) const = 0;

public:
  sf::Vector2f position;
  bool visible = true;
  bool destroyed = false;

  virtual ~UIElement() = default;

  virtual void update(float deltaTime) {}
};
