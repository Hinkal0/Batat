#include "Player.h"

Player::Player(const sf::Texture& texture, std::shared_ptr<const Map> map) : Object(texture), map(map) {
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

  sf::Vector2f movement = direction.componentWiseMul({deltaTime, deltaTime});

  this->move(map->resolveMovement(this->getGlobalBounds(), movement));
}