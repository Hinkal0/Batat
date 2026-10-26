#pragma once
#include <memory>
#include "Character.h"
#include "Scene.h"
#include "Slash.h"

struct PlayerStats {
  int maxHealth = 6;
  float speed = 0.7f;
  float fireInterval = 400.0f;   
  float projectileSpeed = 1.0f;
  int damage = 1;
  float slashInterval = 800.0f;
};

class Player : public Character {
  Scene& scene;
  PlayerStats stats = {};

  sf::Vector2f aimDirection = {1.0f, 0.0f};
  float fireCooldown = 0.0f; 
  float slashCooldown = 0.0f;
  bool slashHeld = false;
  std::shared_ptr<Slash> activeSlash;

  float idleTime = 0.0f; 
  float walkPhase = 0.0f;
  float walkBlend = 0.0f;

  void shoot();
  void slash();

  void animate(float deltaTime, bool moving);
  sf::Transform animationTransform() const;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  Player(Scene& scene);

  sf::FloatRect getHitbox() const override;

  void aimAt(sf::Vector2f target);

  void update(float deltaTime) override;
};