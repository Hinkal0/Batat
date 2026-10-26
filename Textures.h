#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

namespace Textures {
    extern std::unique_ptr<sf::Texture> player;
    extern std::unique_ptr<sf::Texture> projectile;
    extern std::unique_ptr<sf::Texture> slash;
    extern std::unique_ptr<sf::Texture> tilemap;

    bool load();
    void unload();
};