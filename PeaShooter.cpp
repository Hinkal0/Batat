#include "PeaShooter.h"
#include "Resources.h"
#include <utility>

PeaShooter::PeaShooter(Scene& scene, std::weak_ptr<Player> target)
  : Enemy(scene, std::move(target), *Textures::enemies[1], {
      .health = 3, .speed = 0.17f, .range = 650.0f, .interval = 1500.0f,
      .warning = 400.0f, .body = {48.0f, 56.0f},
      .breathPeriod = 1650.0f, .stepTime = 180.0f, .animationStrength = 0.85f,
      .pivot = {13.0f, 23.0f}
    }) {}

void PeaShooter::startAttack(sf::Vector2f direction) {
  fire(direction);
}
