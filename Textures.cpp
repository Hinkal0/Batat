#include "Textures.h"

namespace Textures {
    std::unique_ptr<sf::Texture> player;
    std::unique_ptr<sf::Texture> projectile;
    std::unique_ptr<sf::Texture> slash;
    std::unique_ptr<sf::Texture> tilemap;

    bool load() {
        try {
            player = std::make_unique<sf::Texture>("textures/player.png");
            projectile = std::make_unique<sf::Texture>("textures/projectile.png");
            slash = std::make_unique<sf::Texture>("textures/slash.png");
            tilemap = std::make_unique<sf::Texture>("textures/tilemap.png");
        } catch (sf::Exception& e) {
            return false;
        }
        return true;
    }

    void unload() {
        player.reset();
        projectile.reset();
        slash.reset();
        tilemap.reset();
    }
};