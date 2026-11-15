#pragma once
#include "Enemy.h"

class OnionGuard final : public Enemy {
public:
  OnionGuard(Scene& scene, std::weak_ptr<Player> target);

private:
  void startAttack(sf::Vector2f direction) override;
};
