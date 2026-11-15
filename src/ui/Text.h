#pragma once
#include <SFML/Graphics.hpp>
#include <chrono>
#include <string>
#include <string_view>

namespace Text {
  enum class Align {
    Left,
    Center,
    Right
  };

  const float GLYPH_HEIGHT = 7.0f;

  float width(std::string_view text, unsigned scale = 1);
  std::string formatTime(std::chrono::seconds time);

  void draw(sf::RenderTarget& target, std::string_view text, sf::Vector2f position,
    unsigned scale = 1, sf::Color color = sf::Color::White,
    Align align = Align::Left, bool shadow = true, sf::RenderStates states = sf::RenderStates::Default);
};
