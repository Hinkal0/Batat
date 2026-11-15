#include "Resources.h"

namespace Textures {
  bool load() {
    try {
      player.emplace("assets/textures/player.png");
      projectile.emplace("assets/textures/projectile.png");
      slash.emplace("assets/textures/slash.png");
      tilemap.emplace("assets/textures/tilemap.png");
      roomGate.emplace("assets/textures/room_gate.png");
      floorExit.emplace("assets/textures/floor_exit.png");
      const char* names[] = {"radish_runner", "pea_shooter", "onion_guard", "yabloko"};
      for (size_t i = 0; i < enemies.size(); ++i) {
        enemies[i].emplace(std::string("assets/textures/") + names[i] + ".png");
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
    roomGate.reset();
    floorExit.reset();
    for (auto& texture : enemies) texture.reset();
  }
};

namespace Fonts {
  bool load() {
    try {
      pixel.emplace("assets/fonts/PixelFont.ttf");
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
