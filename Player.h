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

  void shoot();
  void slash();

public:
  Player(const sf::Texture& texture, Scene& scene);

  void aimAt(sf::Vector2f target);

  void update(float deltaTime) override;
};