#pragma once
#include "Object.h"

class Player : public Object {
public:
  Player(const sf::Texture& texture);

  void update(float deltaTime) override;
};