#pragma once
#include <SFML/Graphics.hpp>

class Object : public sf::Sprite {
public:
  Object(const sf::Texture& texture) : sf::Sprite(texture) {}

  virtual ~Object() = default;

  virtual void update(float deltaTime) = 0;
};