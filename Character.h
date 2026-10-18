#pragma once
#include <algorithm>
#include "Object.h"

class Character : public Object {
  Team team;

  int health;
  int maxHealth;

public:
  Character(const sf::Texture& texture, Team team, int maxHealth)
    : Object(texture), team(team), health(maxHealth), maxHealth(maxHealth) {}

  Team getTeam() const { return team; }
  int getHealth() const { return health; }
  int getMaxHealth() const { return maxHealth; }

  virtual void takeDamage(int amount) { health = std::max(0, health - amount); }
};