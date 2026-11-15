#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <array>

namespace Textures {
  inline std::optional<sf::Texture> player;
  inline std::optional<sf::Texture> projectile;
  inline std::optional<sf::Texture> slash;
  inline std::optional<sf::Texture> tilemap;
  inline std::array<std::optional<sf::Texture>, 4> enemies;

  bool load();
  void unload();
};

namespace Fonts {
  inline std::optional<sf::Font> pixel;

  bool load();
  void unload();
};
