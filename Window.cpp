#include "Window.h"
#include <algorithm>

static float SCALE = 4.0f;
static sf::Vector2u MIN_SIZE = {480, 270};

static uint32_t cursorTex[20*20];
static std::optional<sf::Cursor> crosshair;

static const sf::Vector2u RESOLUTIONS[] = {
  {480, 270}, {960, 540}, {1440, 810}, {1920, 1080}
};

std::vector<sf::Vector2u> Window::windowSizePresets() const {
  std::vector<sf::Vector2u> result;
  for (sf::Vector2u resolution : RESOLUTIONS) {
    if (resolution.x <= maxWindowSize.x && resolution.y <= maxWindowSize.y) {
      result.push_back(resolution);
    }
  }
  if (result.empty()) result.push_back(RESOLUTIONS[0]);
  return result;
}

Window::Window(const WinConfig& config)
  : title(config.title), windowedSize(config.w, config.h),
    maxWindowSize(sf::VideoMode::getDesktopMode().size), fullscreen(config.fullscreen),
    gameView({0.0f, 0.0f}, sf::Vector2f(UI_SIZE.x, -UI_SIZE.y)*SCALE),
    uiView(sf::FloatRect({0.0f, 0.0f}, UI_SIZE)) {
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 20; j++) {
      if ((i >= 8 && i <= 11) || (j >= 8 && j <= 11)) cursorTex[i*20+j] = 0xffffffff;
      else cursorTex[i*20+j] = 0;
    }
  }

  crosshair = sf::Cursor::createFromPixels((uint8_t*)cursorTex, {20, 20}, {10, 10});

  create();
}

void Window::create() {
  if (fullscreen) {
    handle.create(sf::VideoMode::getDesktopMode(), title, sf::State::Fullscreen);
  } else {
    handle.create(sf::VideoMode(windowedSize), title, sf::Style::Default);
    checkShrunk(windowedSize, handle.getSize());
  }

  handle.setKeyRepeatEnabled(false);
  handle.setMinimumSize(sf::Vector2u(UI_SIZE.x, UI_SIZE.y));
  handle.setVerticalSyncEnabled(true);
  updateViews();
  if (crosshair.has_value()) handle.setMouseCursor(crosshair.value());
  handle.setMouseCursorGrabbed(cursorGrabbed);
}

void Window::checkShrunk(sf::Vector2u requested, sf::Vector2u actual) {
  if (actual.x < requested.x) maxWindowSize.x = std::min(maxWindowSize.x, actual.x);
  if (actual.y < requested.y) maxWindowSize.y = std::min(maxWindowSize.y, actual.y);
}

void Window::updateViews() {
  sf::Vector2u size = handle.getSize();
  if (size.x == 0 || size.y == 0) return;

  uint32_t scale = std::max(1u, std::min(size.x/MIN_SIZE.x, size.y/MIN_SIZE.y));
  sf::Vector2f vSize = {scale*MIN_SIZE.x/(float)size.x, scale*MIN_SIZE.y/(float)size.y};
  sf::FloatRect viewport {{0.5f-vSize.x/2, 0.5f-vSize.y/2}, {vSize.x, vSize.y}};

  gameView.setViewport(viewport);
  uiView.setViewport(viewport);
  handle.setView(gameView);
}

bool Window::isOpen() const {
  return handle.isOpen();
}

void Window::close() {
  handle.close();
}

void Window::pollEvents() {
  while (const auto& event = handle.pollEvent()) {
    if (event->is<sf::Event::Closed>()) {
      handle.close();
    } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
      if (!fullscreen) windowedSize = resized->size;
      if (requestedSize) {
        checkShrunk(*requestedSize, resized->size);
        requestedSize.reset();
      }
      updateViews();
    } else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
      if (key->code == sf::Keyboard::Key::Enter && key->alt) {
        setFullscreen(!fullscreen);
        continue;
      }
      onKeyPressed(key->code);
    } else if (const auto* key = event->getIf<sf::Event::KeyReleased>()) {
      onKeyReleased(key->code);
    } else if (const auto* mouse = event->getIf<sf::Event::MouseMoved>()) {
      onMouseMoved(getUIMousePos());
    } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
      onMousePressed(mouse->button);
    } else if (const auto* mouse = event->getIf<sf::Event::MouseButtonReleased>()) {
      onMouseReleased(mouse->button);
    } else if (event->is<sf::Event::FocusLost>()) {
      onFocusLost();
    }
  }
}

void Window::setFullscreen(bool value) {
  if (value == fullscreen) return;
  fullscreen = value;
  create();
}

void Window::setWindowSize(sf::Vector2u size) {
  size.x = std::max(size.x, MIN_SIZE.x);
  size.y = std::max(size.y, MIN_SIZE.y);
  windowedSize = size;
  if (fullscreen) return;

  if (size != handle.getSize()) requestedSize = size;
  handle.setSize(size);
  sf::Vector2u desktop = sf::VideoMode::getDesktopMode().size;
  handle.setPosition({
    std::max(0, ((int)desktop.x - (int)size.x) / 2),
    std::max(0, ((int)desktop.y - (int)size.y) / 2)
  });
  updateViews();
}

void Window::setCursorGrabbed(bool value) {
  cursorGrabbed = value;
  handle.setMouseCursorGrabbed(cursorGrabbed);
}

void Window::draw(const Scene& scene) {
  handle.clear();

  gameView.setCenter(scene.cameraPos);
  handle.setView(gameView);
  scene.draw(handle);
  handle.setView(uiView);
  scene.drawUI(handle);

  handle.display();
}

sf::Vector2f Window::getMousePos(const Scene& scene) const {
  sf::View view = gameView;
  view.setCenter(scene.cameraPos);
  return handle.mapPixelToCoords(sf::Mouse::getPosition(handle), view);
}

sf::Vector2f Window::getUIMousePos(void) const {
  return handle.mapPixelToCoords(sf::Mouse::getPosition(handle), uiView);
}