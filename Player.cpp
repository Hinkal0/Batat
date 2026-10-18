#include "Player.h"
#include "Projectile.h"

Player::Player(const sf::Texture& texture, Scene& scene)
: Character(texture, Team::Player, stats.maxHealth), scene(scene) {
  this->setOrigin(this->getLocalBounds().size / 2.0f);
}

void Player::aimAt(sf::Vector2f target) {
  sf::Vector2f offset = target - getPosition();
  if (offset.length() != 0.0f) aimDirection = offset.normalized();
}

void Player::shoot() {
  scene.spawn(LAYER_PROJECTILES, std::make_shared<Projectile>(
    scene.map, Team::Player,
    getPosition(), aimDirection * stats.projectileSpeed, stats.damage
  ));
}

void Player::slash() {
  activeSlash = std::make_shared<Slash>(scene, getPosition(), aimDirection);
  scene.spawn(LAYER_EFFECTS, activeSlash);
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
  if (direction.length()) direction = direction.normalized();
  sf::Vector2f movement = direction * deltaTime * stats.speed;

  this->move(scene.map->resolveMovement(this->getGlobalBounds(), movement));

  fireCooldown = std::max(0.0f, fireCooldown - deltaTime);
  bool firePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
  if (firePressed && fireCooldown == 0.0f) {
    shoot();
    fireCooldown += stats.fireInterval;
  }

  slashCooldown = std::max(0.0f, slashCooldown - deltaTime);
  bool slashPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);
  if (slashPressed && !slashHeld && slashCooldown == 0.0f) {
    slash();
    slashCooldown = stats.slashInterval;
  }
  slashHeld = slashPressed;

  if (activeSlash) {
    if (!activeSlash->destroyed) activeSlash->setPosition(getPosition());
    else activeSlash.reset();
  }
}