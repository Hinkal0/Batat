#include "Scene.h"

Scene::Scene() {}

void Scene::update(float deltaTime) {
  for (auto& layer : layers) {
    for (auto& object : layer) {
      object->update(deltaTime);
    }
  }
}

void Scene::draw(sf::RenderWindow& window) const{
  if (map) window.draw(*map);

  for (const auto& layer : layers) {
    for (const auto& object : layer) {
      window.draw(*object);
    }
  }
}