#include "Player.h"

Player::Player(const sf::Texture& texture) : Object(texture) {
  this->setOrigin(this->getLocalBounds().size.componentWiseDiv({2,2}));
} 

void Player::update(float deltaTime) {
  sf::Vector2f direction = {0,0};
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) {
    direction.x -= 1.0f;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) {
    direction.x += 1.0f;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
    direction.y -= 1.0f;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) {
    direction.y += 1.0f;
  }

  this->move(direction.componentWiseMul({deltaTime, deltaTime}));
}