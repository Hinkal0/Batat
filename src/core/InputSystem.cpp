#include "InputSystem.h"

void InputSystem::init(const Window& window) {
  InputSystem::window = &window;
}
  
sf::Vector2f InputSystem::getMousePos(const Scene& scene) {
  if (!window) return {};

  sf::View view = window->gameView;
  view.setCenter(scene.cameraPos);
  return window->handle.mapPixelToCoords(sf::Mouse::getPosition(window->handle), view);
}

sf::Vector2f InputSystem::getUIMousePos(void) {
  if (!window) return {};
  return window->handle.mapPixelToCoords(sf::Mouse::getPosition(window->handle), window->uiView);
}