#pragma once
#include <memory>
#include "entities/Character.h"

class Scene;
class Player;

class Enemy : public Character {
protected:
  struct Settings {
    int health;
    float speed;
    float range;
    float interval;
    float warning;
    sf::Vector2f body;
    float breathPeriod;
    float stepTime;
    float animationStrength;
    sf::Vector2f pivot;
    bool ranged = true;
  };
  Scene& scene;
  std::weak_ptr<Player> target;
  const Settings settings;
  bool alerted = false;
  float cooldown = 600.0f;
  float windup = 0.0f;
  float age = 0.0f;
  float hurtTime = 0.0f;
  float walkPhase = 0.0f;
  float walkBlend = 0.0f;
  std::optional<sf::Vector2f> waypoint;
  sf::Vector2f attackDirection = {1.0f, 0.0f};

  void animate(float deltaTime, bool moving);
  sf::Transform animationTransform() const;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
public:
  sf::FloatRect getHitbox() const override;
  void takeDamage(int amount) override;
  void update(float deltaTime) override;

protected:

  Enemy(Scene& scene, std::weak_ptr<Player> target, const sf::Texture& texture,
        Settings settings);
  virtual void startAttack(sf::Vector2f direction) = 0;
  virtual bool updateAttack(float deltaTime);
  virtual bool canAdvance(float age) const;
  void moveToward(sf::Vector2f point, float speed, float deltaTime);
  void fire(sf::Vector2f direction);
};
