#include "Scene.h"
#include "Character.h"
#include "Projectile.h"
#include <algorithm>

Scene::Scene() {}

void Scene::spawn(Layer layer, std::shared_ptr<Object> object) {
  if (layer >= LAYER_COUNT) return;
  layers[layer].insert(std::move(object));
}

const std::unordered_set<std::shared_ptr<Object>>& Scene::getLayer(Layer layer) const {
  return layers[std::min((int)layer, LAYER_COUNT-1)];
}

void Scene::update(float deltaTime) {
  for (auto& layer : layers) {
    for (auto& object : layer) {
      object->update(deltaTime);
    }
  }

  resolveHits();

  for (auto& layer : layers) {
    std::erase_if(layer, [](const auto& object) { return object->destroyed; });
  }
}

void Scene::resolveHits() {
  for (auto& object : layers[LAYER_PROJECTILES]) {
    auto* projectile = dynamic_cast<Projectile*>(object.get());
    if (!projectile || projectile->destroyed) continue;

    sf::FloatRect bounds = projectile->getGlobalBounds();
    for (auto& other : layers[LAYER_CHARACTERS]) {
      auto* character = dynamic_cast<Character*>(other.get());
      if (!character || character->destroyed || character->getTeam() == projectile->getTeam()) continue;
      if (!bounds.findIntersection(character->getGlobalBounds())) continue;

      character->takeDamage(projectile->getDamage());
      projectile->destroyed = true;
      break;
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