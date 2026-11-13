#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"
#include "Scene.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>

Enemy::Enemy(Scene& scene, std::weak_ptr<Player> target, const sf::Texture& texture,
             Settings settings)
  : Character(texture, Team::Enemy, settings.health),
    scene(scene), target(std::move(target)), settings(settings) {
  setOrigin(getLocalBounds().size / 2.0f);
  setScale({4.0f, -4.0f});
  cooldown += static_cast<float>(std::rand() % 500);
  age = static_cast<float>(std::rand() % 1000);
}

sf::FloatRect Enemy::getHitbox() const {
  auto size = settings.body;
  return sf::FloatRect(getPosition() - size / 2.0f, size);
}

void Enemy::takeDamage(int amount) {
  if (destroyed || amount <= 0) return;
  Character::takeDamage(amount);
  alerted = true;
  hurtTime = 120.0f;
  if (getHealth() == 0) destroyed = true;
}

void Enemy::moveToward(sf::Vector2f point, float speed, float dt) {
  auto offset = point - getPosition();
  float distance = offset.length();
  if (distance < 0.01f) return;
  auto movement = offset / distance * std::min(distance, speed * dt);
  move(scene.map->resolveMovement(getHitbox(), movement));
}

void Enemy::fire(sf::Vector2f direction) {
  auto player = target.lock();
  if (!player || !scene.map->hasLineOfSight(getPosition(), player->getPosition())) return;
  scene.spawn(LAYER_PROJECTILES, std::make_shared<Projectile>(
      scene.map, Team::Enemy, getPosition(), direction * 0.40f, 1));
}

void Enemy::update(float dt) {
  if (destroyed) return;
  age += dt;
  hurtTime = std::max(0.0f, hurtTime - dt);
  auto player = target.lock();
  if (!player || player->getHealth() == 0) {
    animate(dt, false);
    return;
  }
  auto offset = player->getPosition() - getPosition();
  float distance = offset.length();
  bool visible = scene.map->hasLineOfSight(getPosition(), player->getPosition());
  if (distance < 950.0f && visible) alerted = true;
  if (!alerted) {
    animate(dt, false);
    return;
  }

  cooldown = std::max(0.0f, cooldown - dt);
  const sf::Vector2f previousPosition = getPosition();

  if (updateAttack(dt)) {
  } else if (windup > 0.0f) {
    windup = std::max(0.0f, windup - dt);
    if (windup == 0.0f) {
      cooldown = settings.interval;
      startAttack(attackDirection);
    }
  } else if (visible && distance <= settings.range && cooldown == 0.0f) {
    if (distance > 0.01f) attackDirection = offset / distance;
    windup = settings.warning;
  } else {
    if (!waypoint || (*waypoint-getPosition()).length() < 1.0f) {
      waypoint = scene.map->nextWaypoint(getPosition(), player->getPosition(), settings.body);
    }
    if (settings.ranged && visible && distance < 220.0f) {
      if (distance > 0.01f)
        moveToward(getPosition() - offset / distance * 100.0f, settings.speed, dt);
    } else if (!settings.ranged || !visible || distance > settings.range * 0.75f) {
      if (canAdvance(age) && waypoint) moveToward(*waypoint, settings.speed, dt);
    }
  }
  animate(dt, getPosition() != previousPosition);
  if (getHitbox().findIntersection(player->getHitbox())) player->takeDamage(1);
}

bool Enemy::updateAttack(float) { return false; }

bool Enemy::canAdvance(float) const { return true; }

void Enemy::animate(float dt, bool moving) {
  walkBlend = std::clamp(walkBlend + (moving ? dt : -dt) / 120.0f, 0.0f, 1.0f);
  if (walkBlend > 0.0f)
    walkPhase = std::fmod(walkPhase + dt / settings.stepTime, 2.0f);
  else
    walkPhase = 0.0f;
}

sf::Transform Enemy::animationTransform() const {
  constexpr float PI = 3.14159265f;
  float breath = std::sin(std::fmod(age, settings.breathPeriod) / settings.breathPeriod * 2.0f * PI)
      * 0.035f * settings.animationStrength;
  float hop = std::abs(std::sin(walkPhase * PI));
  float step = (hop * 0.06f - (1.0f - hop) * 0.09f) * settings.animationStrength;
  float stretch = breath + (step - breath) * walkBlend;
  float lift = hop * 1.5f * settings.animationStrength * walkBlend;
  float tilt = std::sin(walkPhase * PI) * 5.5f * settings.animationStrength * walkBlend;

  sf::Transform transform;
  transform.translate(settings.pivot - sf::Vector2f(0.0f, lift))
    .rotate(sf::degrees(tilt))
    .scale({1.0f - stretch, 1.0f + stretch})
    .translate(-settings.pivot);
  return transform;
}

void Enemy::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  sf::RenderStates bodyStates = states;
  bodyStates.transform *= animationTransform();
  sf::Sprite sprite(getTexture());
  if (windup > 0.0f) {
    float pulse = 1.0f + 0.06f * std::sin(age / 40.0f);
    auto pivot = settings.pivot;
    bodyStates.transform.translate(pivot).scale({pulse, 2.0f - pulse}).translate(-pivot);
    sprite.setColor(sf::Color(255, 185, 140));
  }
  if (hurtTime > 0.0f) sprite.setColor(sf::Color(255, 110, 110));
  target.draw(sprite, bodyStates);
  if (getHealth() < getMaxHealth()) {
    sf::RectangleShape bar({16.0f, 2.0f});
    bar.setPosition({5.0f, 0.0f});
    bar.setFillColor(sf::Color(50, 20, 20));
    target.draw(bar, states);
    bar.setSize({16.0f * getHealth() / getMaxHealth(), 2.0f});
    bar.setFillColor(sf::Color(235, 75, 65));
    target.draw(bar, states);
  }
}
