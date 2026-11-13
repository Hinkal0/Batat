#pragma once
#include "Enemy.h"

class Yabloko final : public Enemy {
public:
  Yabloko(Scene& scene, std::weak_ptr<Player> target);

private:
  int shotsLeft = 0;
  float volleyTimer = 0.0f;
  sf::Vector2f volleyDirection;

  void startAttack(sf::Vector2f direction) override;
  bool updateAttack(float deltaTime) override;
  bool canAdvance(float age) const override;
};
