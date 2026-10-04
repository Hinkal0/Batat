#pragma once
#include <memory>
#include "Object.h"
#include "Map.h"

class Player : public Object {
  std::shared_ptr<const Map> map;

public:
  Player(const sf::Texture& texture, std::shared_ptr<const Map> map);

  void update(float deltaTime) override;
};