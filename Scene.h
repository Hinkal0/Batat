#pragma once
#include <vector>
#include <unordered_set>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Object.h"
#include "Map.h"

enum Layer {
  LAYER_CHARACTERS,
  LAYER_PROJECTILES,
  LAYER_EFFECTS,
  LAYER_COUNT
};

class Scene {
  std::array<std::unordered_set<std::shared_ptr<Object>>, LAYER_COUNT> layers;

  void resolveHits();

public:
  sf::Vector2f cameraPos = {0.0f, 0.0f};

  std::shared_ptr<Map> map;

  Scene();

  void spawn(Layer layer, std::shared_ptr<Object> object);
  const std::unordered_set<std::shared_ptr<Object>>& getLayer(Layer layer) const;
  
  void update(float deltaTime);
  void draw(sf::RenderWindow& window) const;
};