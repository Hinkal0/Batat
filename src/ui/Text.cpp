#include "Text.h"
#include <cmath>
#include <sstream>
#include "core/Resources.h"

static const unsigned PIXEL_SIZE = 8;

static const sf::Color SHADOW_COLOR(0, 0, 0, 170);

static sf::Text makeText(std::string_view text, unsigned scale) {
  return sf::Text(*Fonts::pixel, sf::String(std::string(text)), PIXEL_SIZE * scale);
}

static float capitalTop(unsigned scale) {
  unsigned size = PIXEL_SIZE * scale;
  return (float)size + Fonts::pixel->getGlyph(U'H', size, false).bounds.position.y;
}

namespace Text {
  std::string formatTime(std::chrono::seconds time) {
    long long total = time.count();
    long long hours = total / 3600;
    long long minutes = total / 60 % 60;
    long long seconds = total % 60;
    std::ostringstream oss;
    oss << hours << ':' << std::setfill('0') << std::setw(2) << minutes << ':' << std::setfill('0') << std::setw(2) << seconds;
    return oss.str();
  }

  float width(std::string_view text, unsigned scale) {
    if (!Fonts::pixel || text.empty()) return 0.0f;
    return makeText(text, scale).findCharacterPos(text.size()).x - (float)scale;
  }

  void draw(sf::RenderTarget& target, std::string_view text, sf::Vector2f position,
      unsigned scale, sf::Color color, Align align, bool shadow, sf::RenderStates states) {
    if (!Fonts::pixel || text.empty()) return;

    float w = width(text, scale);
    if (align == Align::Center) position.x -= w / 2.0f;
    else if (align == Align::Right) position.x -= w;
    position = {std::floor(position.x + 0.5f), std::floor(position.y + 0.5f) - capitalTop(scale)};

    sf::Text label = makeText(text, scale);

    if (shadow) {
      sf::Color shadowColor = SHADOW_COLOR;
      shadowColor.a = (uint8_t)(shadowColor.a * color.a / 255);
      label.setFillColor(shadowColor);
      label.setPosition(position + sf::Vector2f(1.0f, 1.0f) * (float)scale);
      target.draw(label, states);
    }

    label.setFillColor(color);
    label.setPosition(position);
    target.draw(label, states);
  }
};
