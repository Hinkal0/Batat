#include "Projectile.h"
#include "Textures.h"
#include <cmath>

static const float PROJECTILE_RADIUS = 12.0f;

static const sf::Color PLAYER_COLOR(255, 220, 90);
static const sf::Color ENEMY_COLOR(255, 80, 80);

Projectile::Projectile(std::shared_ptr<const Map> map,
  Team team, sf::Vector2f position, sf::Vector2f velocity, int damage)
: Object(*Textures::projectile), map(map), velocity(velocity), team(team), damage(damage) {
  
  sf::Vector2f textureSize(getTexture().getSize());
  setOrigin(textureSize / 2.0f);
  float scale = 2.0f * PROJECTILE_RADIUS / textureSize.x;
  setScale({scale, scale});
  setPosition(position);
  setColor(team == Team::Player ? PLAYER_COLOR : ENEMY_COLOR);
}

void Projectile::update(float deltaTime) {
  move(velocity * deltaTime);
  if (map->collides(getGlobalBounds())) {
    destroyed = true;
    return;
  }
}