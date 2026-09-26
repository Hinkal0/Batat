#include "Window.h"

static float SCALE = 4.0f;
static uint32_t RESX = 480, RESY = 270;

static void updateView(sf::RenderWindow& handle, sf::View& view, sf::Vector2u size) {
  uint32_t scale = std::min(size.x/RESX, size.y/RESY);
  sf::Vector2f vSize = {scale*RESX/(float)size.x, scale*RESY/(float)size.y};
  sf::FloatRect viewport {{0.5f-vSize.x/2, 0.5f-vSize.y/2}, {vSize.x, vSize.y}};
  view.setViewport(viewport);

  handle.setView(view);
} 

Window::Window(const WinConfig& config) 
  : title(config.title), fullscreen(config.fullscreen), minSize(config.minW, config.minH) {
    if (config.fullscreen) {
      handle = sf::RenderWindow(sf::VideoMode::getDesktopMode(), config.title, sf::State::Fullscreen);
    } else {
      handle = sf::RenderWindow(sf::VideoMode({config.w, config.h}), config.title);
    }

    handle.setFramerateLimit(120);
    handle.setKeyRepeatEnabled(false);
    handle.setMinimumSize(minSize);

    sf::View view({0.0f, 0.0f}, {SCALE*RESX, SCALE*RESY});
    updateView(handle, view, handle.getSize());
    size = handle.getSize();
}

bool Window::isOpen() const {
  return handle.isOpen();
}

void Window::pollEvents() {
  static sf::Vector2f pos = {0.0f, 0.0f};
  while (const auto& event = handle.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      handle.close();
      return;
    }
    if (const auto* resized = event->getIf<sf::Event::Resized>()) {
      sf::View view = handle.getView();

      updateView(handle, view, resized->size);
      size = handle.getSize();
    }
    if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
      switch (key->code) {
      case sf::Keyboard::Key::F11:
        sf::View view = handle.getView();
        if (!fullscreen) {
          handle.create(sf::VideoMode::getDesktopMode(), title, sf::State::Fullscreen);
        } else {
          handle.create(sf::VideoMode(size), title);
        }
        updateView(handle, view, handle.getSize());
        handle.setFramerateLimit(120);
        handle.setKeyRepeatEnabled(false);
        handle.setMinimumSize(minSize);
        fullscreen ^= 1;
        break;
      }
    }
  }
}

void Window::beginDraw() {
  handle.clear();
  sf::RectangleShape rect({100, 100});
  sf::RectangleShape rect2({10000,10000});
  rect2.setFillColor(sf::Color::Blue);
  rect2.setPosition({-1000, -1000});
  rect.setOrigin({50, 50});
  rect.setPosition({0, 0});
  rect.setFillColor(sf::Color::White);
  
  handle.draw(rect2);
  handle.draw(rect);
}

void Window::endDraw() {
  handle.display();
}
