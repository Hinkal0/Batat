#include "Yabloko.h"
#include <cmath>
#include <utility>
#include "core/Resources.h"

Yabloko::Yabloko(Scene& scene, std::weak_ptr<Player> target)
  : Enemy(scene, std::move(target), *Textures::enemies[3], {
      .health = 2, .speed = 0.34f, .range = 600.0f, .interval = 1800.0f,
      .warning = 450.0f, .body = {56.0f, 52.0f},
      .breathPeriod = 1250.0f, .stepTime = 135.0f, .animationStrength = 1.1f,
      .pivot = {13.0f, 23.0f}
    }) {}

void Yabloko::startAttack(sf::Vector2f direction) {
  volleyDirection = direction;
  shotsLeft = 3;
  volleyTimer = 0.0f;
}

bool Yabloko::updateAttack(float dt) {
  if (shotsLeft == 0) return false;

  volleyTimer -= dt;
  if (volleyTimer <= 0.0f) {
    fire(volleyDirection);
    --shotsLeft;
    volleyTimer = 120.0f;
  }
  return true;
}

bool Yabloko::canAdvance(float age) const {
  return std::fmod(age, 650.0f) < 280.0f;
}
