#define _USE_MATH_DEFINES
#include "Player.h"
#include "Projectile.h"
#include "Textures.h"
#include <cmath>

static const sf::Vector2f HITBOX_SIZE = {64.0f, 68.0f};

static const sf::Vector2f EYES[] = {{8.0f, 9.0f}, {15.0f, 9.0f}};

static const sf::Vector2f PIVOT = {13.0f, 21.0f};

static const float IDLE_PERIOD = 1400.0f;
static const float IDLE_SQUASH = 0.05f;
static const float STEP_TIME = 160.0f;
static const float HOP_HEIGHT = 1.5f;
static const float HOP_STRETCH = 0.08f;
static const float LAND_SQUASH = 0.12f;
static const float WALK_TILT = 7.0f;
static const float WALK_BLEND_TIME = 120.0f;

Player::Player(Scene& scene)
: Character(*Textures::player, Team::Player, PlayerStats{}.maxHealth), scene(scene) {
  this->setOrigin(this->getLocalBounds().size / 2.0f);
  this->setScale({4, 4});
}

sf::FloatRect Player::getHitbox() const {
  return sf::FloatRect(getPosition() - HITBOX_SIZE / 2.0f, HITBOX_SIZE);
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

  sf::Vector2f resolved = scene.map->resolveMovement(getHitbox(), movement);
  this->move(resolved);
  animate(deltaTime, resolved != sf::Vector2f{0, 0});

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

void Player::animate(float deltaTime, bool moving) {
  idleTime = std::fmod(idleTime + deltaTime, IDLE_PERIOD);

  float deltaBlend = deltaTime / WALK_BLEND_TIME;
  walkBlend = std::clamp(walkBlend + (moving ? deltaBlend : -deltaBlend), 0.0f, 1.0f);

  if (walkBlend > 0.0f) walkPhase = std::fmod(walkPhase + deltaTime / STEP_TIME, 2.0f);
  else walkPhase = 0.0f;
}

sf::Transform Player::animationTransform() const {
  float breath = std::sin(idleTime / IDLE_PERIOD * 2.0f * M_PI) * IDLE_SQUASH;
  float hop = std::abs(std::sin(walkPhase * M_PI));
  float step = hop * HOP_STRETCH - (1.0f - hop) * LAND_SQUASH;
  float stretch = breath + (step - breath) * walkBlend;

  float lift = hop * HOP_HEIGHT * walkBlend;
  float tilt = std::sin(walkPhase * M_PI) * WALK_TILT * walkBlend;

  sf::Transform transform;
  transform
    .scale({1, -1}, getOrigin())
    .translate(PIVOT - sf::Vector2f{0.0f, lift})
    .rotate(sf::degrees(tilt))
    .scale({1.0f - stretch, 1.0f + stretch})
    .translate(-PIVOT);
  return transform;
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();

  states.transform *= animationTransform();
  
  target.draw(sf::Sprite(getTexture()), states);

  sf::RectangleShape pupil({2.0f, 3.0f});
  sf::Vector2f offset = {aimDirection.x >= 0.0f ? 1.0f : 0.0f, aimDirection.y >= 0.0f ? 0.0f : 1.0f};
  pupil.setFillColor(sf::Color(25, 25, 25));
  for (sf::Vector2f eye : EYES) {
    pupil.setPosition(eye + offset);
    target.draw(pupil, states);
  }
}
