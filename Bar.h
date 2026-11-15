#pragma once
#include "UIElement.h"

class Bar : public UIElement {
  sf::Vector2f size;

  void drawContents(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  float fill;
  sf::Color color;

  Bar(sf::Vector2f position, sf::Vector2f size, float fill, sf::Color color);
};
