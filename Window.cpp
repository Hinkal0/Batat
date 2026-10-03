#include "Window.h"
#include <algorithm>

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

    handle.setKeyRepeatEnabled(false);
    handle.setMinimumSize(minSize);
    handle.setVerticalSyncEnabled(true);
    handle.setMouseCursorGrabbed(true);

    sf::View view({0.0f, 0.0f}, {SCALE*RESX, -SCALE*RESY});
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

bool Window::isPaused() const {
  return paused;
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
      sf::View view;
      switch (key->code) {
      case sf::Keyboard::Key::F11:
        view = handle.getView();
        if (!fullscreen) {
          handle.create(sf::VideoMode::getDesktopMode(), title, sf::State::Fullscreen);
        } else {
          handle.create(sf::VideoMode(size), title);
        }
        updateView(handle, view, handle.getSize());
        handle.setKeyRepeatEnabled(false);
        handle.setMinimumSize(minSize);
        handle.setVerticalSyncEnabled(true);
        handle.setMouseCursorGrabbed(true);
        fullscreen ^= 1;
        break;
      case sf::Keyboard::Key::Escape:
        handle.setMouseCursorGrabbed(paused);
        paused ^= 1;
        break;
      }
    }
  }
}

void Window::draw(const Scene& scene) {
  sf::View view = handle.getView();
  view.setCenter(scene.cameraPos);
  handle.setView(view);

  handle.clear();
  sf::RectangleShape rect2({10000,10000});
  rect2.setFillColor(sf::Color(100, 100, 100));
  rect2.setPosition({-1000, -1000});
  
  handle.draw(rect2);

  scene.draw(handle);

  handle.display();
}

sf::Vector2f Window::getMousePos(void) const {
  sf::Vector2i pos = sf::Mouse::getPosition(handle);
  sf::Vector2f viewport = handle.getView().getViewport().size;
  sf::Vector2f fsize = {(float)size.x, (float)size.y};
  sf::FloatRect clampArea (
    (sf::Vector2f{0.5f,0.5f}-viewport.componentWiseDiv({2,2})).componentWiseMul(fsize),
    viewport.componentWiseMul(fsize)
  );
  
  pos.x = std::clamp<float>(pos.x, 
    clampArea.position.x, (clampArea.position+clampArea.size).x);
  pos.y = std::clamp<float>(pos.y, 
    clampArea.position.y, (clampArea.position+clampArea.size).y);

  
  return handle.mapPixelToCoords(pos);
}
