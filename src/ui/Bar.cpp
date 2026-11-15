#include "Bar.h"
#include <algorithm>
#include <cmath>

static const sf::Color TRACK_COLOR(34, 34, 48);
static const sf::Color OUTLINE_COLOR(8, 6, 12);

Bar::Bar(sf::Vector2f position, sf::Vector2f size, float fill, sf::Color color)
  : UIElement(position), size(size), fill(fill), color(color) {}

void Bar::drawContents(sf::RenderTarget& target, sf::RenderStates states) const {
  sf::RectangleShape track(size);
  track.setFillColor(TRACK_COLOR);
  track.setOutlineThickness(1.0f);
  track.setOutlineColor(OUTLINE_COLOR);
  target.draw(track, states);

  float width = std::round(size.x * std::clamp(fill, 0.0f, 1.0f));
  if (width > 0.0f) {
    sf::RectangleShape filled({width, size.y});
    filled.setFillColor(color);
    target.draw(filled, states);

    filled.setSize({width, 1.0f});
    filled.setFillColor(color + sf::Color(40, 40, 40));
    target.draw(filled, states);
  }
}
