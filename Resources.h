#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

namespace Textures {
  inline std::optional<sf::Texture> player;
  inline std::optional<sf::Texture> projectile;
  inline std::optional<sf::Texture> slash;
  inline std::optional<sf::Texture> tilemap;

  bool load();
  void unload();
};

namespace Fonts {
  inline std::optional<sf::Font> pixel;

  bool load();
  void unload();
};
