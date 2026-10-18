#pragma once
#include "Object.h"

class Scene;

class Slash : public Object {
  Scene& scene;
  sf::Vector2f direction;
  float timeLeft;

  bool contains(sf::Vector2f point) const;

public:
  Slash(Scene& scene, sf::Vector2f position, sf::Vector2f direction);

  void update(float deltaTime) override;
};