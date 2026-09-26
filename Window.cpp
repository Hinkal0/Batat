#include "Window.h"

Window::Window(const WinConfig& config) 
  : handle(sf::VideoMode({config.w, config.h}), config.title) {
    sf::Vector2u minSize = {config.minW, config.minH};
    handle.setMinimumSize(minSize);
    handle.setPosition({(int32_t)config.x, (int32_t)config.y});
}

bool Window::isOpen() const {
  return handle.isOpen();
}

void Window::pollEvents() {
  while (const auto& event = handle.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      handle.close();
      return;
    }
    if (const auto* resized = event->getIf<sf::Event::Resized>()) {
      //sf::View view(sf::FloatRect({.0f, .0f}, {(float)resized->size.x, (float)resized->size.y}));
      //handle.setView(view);
    }
  }
}

void Window::beginDraw() {
  handle.clear();
}

void Window::endDraw() {
  handle.display();
}
