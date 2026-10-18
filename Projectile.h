#pragma once
#include <memory>
#include "Object.h"
#include "Map.h"

class Projectile : public Object {
  std::shared_ptr<const Map> map;
  sf::Vector2f velocity;
  Team team;
  int damage;

public:
  Projectile(std::shared_ptr<const Map> map, Team team, sf::Vector2f position,
      sf::Vector2f velocity, int damage);

  void update(float deltaTime) override;

  Team getTeam() const { return team; }
  int getDamage() const { return damage; }
  sf::Vector2f getVelocity() const { return velocity; }
};