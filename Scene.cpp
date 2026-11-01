#include "Scene.h"
#include "Character.h"
#include "Projectile.h"
#include <algorithm>

Scene::Scene() {}

void Scene::spawn(Layer layer, std::shared_ptr<Object> object) {
  if (layer >= LAYER_COUNT) return;
  layers[layer].insert(std::move(object));
}

void Scene::spawn(UILayer layer, std::shared_ptr<UIElement> element) {
  if (layer >= UI_LAYER_COUNT) return;
  uiLayers[layer].insert(std::move(element));
}

void Scene::clearObjects() {
  for (auto& layer : layers) layer.clear();
  for (auto& layer : uiLayers) layer.clear();
}

const std::unordered_set<std::shared_ptr<Object>>& Scene::getLayer(Layer layer) const {
  return layers[std::min((int)layer, LAYER_COUNT-1)];
}

const std::unordered_set<std::shared_ptr<UIElement>>& Scene::getLayer(UILayer layer) const {
  return uiLayers[std::min((int)layer, UI_LAYER_COUNT-1)];
}

void Scene::update(float deltaTime) {
  for (auto& layer : layers) {
    for (auto& object : layer) {
      object->update(deltaTime);
    }
  }

  for (auto& layer : uiLayers) {
    for (auto& element : layer) {
      element->update(deltaTime);
    }
  }

  resolveHits();

  for (auto& layer : layers) {
    std::erase_if(layer, [](const auto& object) { return object->destroyed; });
  }
  for (auto& layer : uiLayers) {
    std::erase_if(layer, [](const auto& element) { return element->destroyed; });
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
      if (!bounds.findIntersection(character->getHitbox())) continue;

      character->takeDamage(projectile->getDamage());
      projectile->destroyed = true;
      break;
    }
  }
}

void Scene::draw(sf::RenderTarget& target) const {
  if (map) target.draw(*map);

  for (const auto& layer : layers) {
    for (const auto& object : layer) {
      target.draw(*object);
    }
  }
}

void Scene::drawUI(sf::RenderTarget& target) const {
  for (const auto& layer : uiLayers) {
    for (const auto& element : layer) {
      target.draw(*element);
    }
  }
}