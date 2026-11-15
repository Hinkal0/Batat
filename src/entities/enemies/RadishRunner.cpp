#include "RadishRunner.h"
#include <algorithm>
#include <utility>
#include "core/Resources.h"

RadishRunner::RadishRunner(Scene& scene, std::weak_ptr<Player> target)
  : Enemy(scene, std::move(target), *Textures::enemies[0], {
      .health = 3, .speed = 0.27f, .range = 220.0f, .interval = 1300.0f,
      .warning = 350.0f, .body = {48.0f, 48.0f},
      .breathPeriod = 1400.0f, .stepTime = 150.0f, .animationStrength = 1.0f,
      .pivot = {13.0f, 24.0f}, .ranged = false
    }) {}

void RadishRunner::startAttack(sf::Vector2f direction) {
  lungeDirection = direction;
  lungeTime = 230.0f;
}

bool RadishRunner::updateAttack(float dt) {
  if (lungeTime <= 0.0f) return false;

  lungeTime = std::max(0.0f, lungeTime - dt);
  moveToward(getPosition() + lungeDirection * 100.0f, 1.0f, dt);
  return true;
}
