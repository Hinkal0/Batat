#pragma once
#include <SFML/Graphics.hpp>

enum class Team {
  Player,
  Enemy
};

class Object : public sf::Sprite {
public:
  bool destroyed = false;

  Object(const sf::Texture& texture) : sf::Sprite(texture) {}

  virtual ~Object() = default;

  virtual void update(float deltaTime) = 0;
};