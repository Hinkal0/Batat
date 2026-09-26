#include "Window.h"

static float SCALE = 4.0f;
static uint32_t RESX = 480, RESY = 270;

static uint32_t cursorTex[20*20];
static std::optional<sf::Cursor> cursor;

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

    for (int i = 0; i < 20; i++) {
      for (int j = 0; j < 20; j++) {
        if ((i >= 8 && i <= 11) || (j >= 8 && j <= 11)) cursorTex[i*20+j] = 0xffffffff;
        else cursorTex[i*20+j] = 0;
      }
    }

    cursor = sf::Cursor::createFromPixels((uint8_t*)cursorTex, {20, 20}, {10, 10});
    if (cursor.has_value()) handle.setMouseCursor(cursor.value());
    
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
    if (event->is<sf::Event::MouseEntered>()) {
      if (cursor.has_value()) handle.setMouseCursor(cursor.value());
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
