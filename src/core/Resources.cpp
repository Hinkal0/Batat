#include "Resources.h"

namespace Textures {
  bool load() {
    try {
      player.emplace("textures/player.png");
      projectile.emplace("textures/projectile.png");
      slash.emplace("textures/slash.png");
      tilemap.emplace("textures/tilemap.png");
      const char* names[] = {"radish_runner", "pea_shooter", "onion_guard", "yabloko"};
      for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].emplace(std::string("textures/") + names[i] + ".png");
      }
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
    for (auto& texture : enemies) texture.reset();
  }
};

namespace Fonts {
  bool load() {
    try {
      pixel.emplace("fonts/PixelFont.ttf");
    } catch (sf::Exception& e) {
      return false;
    }
    pixel->setSmooth(false);
    return true;
  }

  void unload() {
    pixel.reset();
  }
};
