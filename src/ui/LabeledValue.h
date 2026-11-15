#pragma once
#include <string>
#include "UIElement.h"

class LabeledValue : public UIElement {
  std::string label;
  sf::Color labelColor;
  sf::Color valueColor;

  void drawContents(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  std::string value;

  LabeledValue(sf::Vector2f rightEdge, std::string label, std::string value,
    sf::Color labelColor, sf::Color valueColor);
};
