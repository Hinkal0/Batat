#include "OnionGuard.h"
#include "Resources.h"
#include <utility>

OnionGuard::OnionGuard(Scene& scene, std::weak_ptr<Player> target)
  : Enemy(scene, std::move(target), *Textures::enemies[2], {
      .health = 7, .speed = 0.11f, .range = 520.0f, .interval = 2200.0f,
      .warning = 600.0f, .body = {56.0f, 56.0f},
      .breathPeriod = 2000.0f, .stepTime = 230.0f, .animationStrength = 0.55f,
      .pivot = {13.0f, 23.0f}
    }) {}

void OnionGuard::startAttack(sf::Vector2f direction) {
  for (float angle : {-0.22f, 0.0f, 0.22f})
    fire(direction.rotatedBy(sf::radians(angle)));
}
