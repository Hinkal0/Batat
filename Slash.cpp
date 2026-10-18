#include "Slash.h"
#include "Projectile.h"
#include "Scene.h"
#define _USE_MATH_DEFINES
#include <cmath>

static const float SLASH_RADIUS = 170.0f;
static const float SLASH_HALF_ANGLE = M_PI * 0.4f;
static const float SLASH_DURATION = 150.0f;

static sf::Texture slashTexture("textures/slash.png");

Slash::Slash(Scene& scene, sf::Vector2f position, sf::Vector2f direction)
: Object(slashTexture), scene(scene), direction(direction.normalized()),
 timeLeft(SLASH_DURATION) {
  sf::Vector2f textureSize(getTexture().getSize());
  setOrigin({0.0f, textureSize.y / 2.0f});
  float scale = SLASH_RADIUS / textureSize.x;
  setScale({scale, scale});
  setRotation(sf::radians(std::atan2(this->direction.y, this->direction.x)));
  setPosition(position);
}

bool Slash::contains(sf::Vector2f point) const {
  sf::Vector2f offset = point - getPosition();
  float distance = offset.length();
  if (distance > SLASH_RADIUS) return false;
  if (distance == 0.0f) return true;
  return std::abs(direction.angleTo(offset).asRadians()) <= SLASH_HALF_ANGLE;
}

void Slash::update(float deltaTime) {
  for (auto& object : scene.getLayer(LAYER_PROJECTILES)) {
    auto* projectile = dynamic_cast<Projectile*>(object.get());
    if (!projectile || projectile->destroyed || projectile->getTeam() != Team::Enemy) continue;
    if (!contains(projectile->getPosition())) continue;

    projectile->destroyed = true;
  }

  timeLeft = std::max(timeLeft-deltaTime, 0.0f);
  if (timeLeft == 0.0f) {
    destroyed = true;
    return;
  }
  setColor(sf::Color(255, 255, 255, (uint8_t)(255 * timeLeft / SLASH_DURATION)));
}