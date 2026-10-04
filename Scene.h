#pragma once
#include <vector>
#include <unordered_set>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Object.h"
#include "Map.h"

class Scene {
public:
  sf::Vector2f cameraPos = {0.0f, 0.0f};

  std::shared_ptr<Map> map;

  std::vector<std::unordered_set<std::shared_ptr<Object>>> layers;

  Scene();

  void update(float deltaTime);
  void draw(sf::RenderWindow& window) const;
};