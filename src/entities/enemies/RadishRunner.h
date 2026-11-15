#pragma once
#include "Enemy.h"

class RadishRunner final : public Enemy {
public:
  RadishRunner(Scene& scene, std::weak_ptr<Player> target);

private:
  float lungeTime = 0.0f;
  sf::Vector2f lungeDirection;

  void startAttack(sf::Vector2f direction) override;
  bool updateAttack(float deltaTime) override;
};
