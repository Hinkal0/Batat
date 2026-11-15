#pragma once
#include "Enemy.h"

class PeaShooter final : public Enemy {
public:
  PeaShooter(Scene& scene, std::weak_ptr<Player> target);

private:
  void startAttack(sf::Vector2f direction) override;
};
