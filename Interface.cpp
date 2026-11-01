#include "Interface.h"
#include "Text.h"
#include <SFML/Audio/Listener.hpp>
#include <algorithm>

static const int VOLUME_STEP = 10;

static const sf::Color TITLE_COLOR(255, 214, 120);
static const sf::Color TITLE_SHADOW_COLOR(120, 60, 30);
static const sf::Color BEST_COLOR(240, 196, 90);
static const sf::Color HINT_COLOR(150, 150, 170);
static const sf::Color HINT_KEY_COLOR(220, 220, 235);

static const char* CONTROLS[][2] = {
  {"WASD", "MOVE"},
  {"MOUSE", "AIM"},
  {"LMB", "SHOOT"},
  {"RMB", "SLASH (BLOCK)"},
  {"ESC", "PAUSE"},
  {"ALT+ENTER", "FULLSCREEN"},
};

Interface::Interface(Window& window, GameConfig& config)
  : window(window), config(config), pauseMenu("PAUSED"), settingsMenu("SETTINGS") {

  mainMenu.top = 112.0f;
  mainMenu.addButton("PLAY", [this] { pending = Action::StartGame; });
  mainMenu.addButton("SETTINGS", [this] { open(Screen::Settings); });
  mainMenu.addButton("EXIT", [this] { pending = Action::Exit; });

  pauseMenu.top = 118.0f;
  pauseMenu.addButton("RESUME", [this] { pending = Action::Resume; });
  pauseMenu.addButton("SETTINGS", [this] { open(Screen::Settings); });
  pauseMenu.addButton("MAIN MENU", [this] { pending = Action::QuitToMenu; });
  pauseMenu.onBack([this] { pending = Action::Resume; });

  auto closeSettings = [this] {
    open(settingsReturn);
    pending = Action::SettingsChanged;
  };

  settingsMenu.top = 106.0f;
  settingsMenu.addOption("VOLUME",
    [this] { return this->config.volume == 0 ? std::string("OFF") : std::to_string(this->config.volume) + "%"; },
    [this](int direction) { changeVolume(direction); });
  settingsMenu.addOption("WINDOW SIZE",
    [this] {
      sf::Vector2u size = this->window.getWindowSize();
      return std::to_string(size.x) + "x" + std::to_string(size.y);
    },
    [this](int direction) { changeWindowSize(direction); },
    [this] { return !this->window.isFullscreen(); },
    [this](int direction) { return nextWindowSize(direction).has_value(); });
  settingsMenu.addOption("FULLSCREEN",
    [this] { return std::string(this->window.isFullscreen() ? "ON" : "OFF"); },
    [this](int) { this->window.setFullscreen(!this->window.isFullscreen()); });
  settingsMenu.addButton("BACK", closeSettings);
  settingsMenu.onBack(closeSettings);
}

std::optional<sf::Vector2u> Interface::nextWindowSize(int direction) const {
  std::vector<sf::Vector2u> presets = window.windowSizePresets();
  sf::Vector2u current = window.getWindowSize();
  unsigned area = current.x * current.y;

  if (direction > 0) {
    for (sf::Vector2u preset : presets) {
      if (preset.x * preset.y > area) return preset;
    }
  } else {
    for (auto it = presets.rbegin(); it != presets.rend(); ++it) {
      if (it->x * it->y < area) return *it;
    }
  }
  return std::nullopt;
}

void Interface::changeWindowSize(int direction) {
  if (std::optional<sf::Vector2u> size = nextWindowSize(direction)) {
    window.setWindowSize(*size);
  }
}

void Interface::changeVolume(int direction) {
  config.volume = std::clamp(config.volume + direction * VOLUME_STEP, 0, 100);
  sf::Listener::setGlobalVolume((float)config.volume);
}

void Interface::open(Screen next) {
  if (next == Screen::Settings && screen != Screen::Settings) settingsReturn = screen;
  screen = next;
  if (Menu* menu = activeMenu()) menu->reset();
}

const Menu* Interface::activeMenu() const {
  switch (screen) {
  case Screen::Main: return &mainMenu;
  case Screen::Pause: return &pauseMenu;
  case Screen::Settings: return &settingsMenu;
  default: return nullptr;
  }
}

Menu* Interface::activeMenu() {
  return const_cast<Menu*>(static_cast<const Interface*>(this)->activeMenu());
}

Interface::Action Interface::handleEvent(const sf::Event& event, sf::Vector2f uiMouse) {
  if (Menu* menu = activeMenu()) menu->handleEvent(event, uiMouse);

  Action action = pending;
  pending = Action::None;
  return action;
}

void Interface::drawOverlay(sf::RenderTarget& target, uint8_t alpha) const {
  sf::RectangleShape overlay(Window::UI_SIZE);
  overlay.setFillColor(sf::Color(8, 6, 14, alpha));
  target.draw(overlay);
}

void Interface::drawMainMenu(sf::RenderTarget& target) const {
  float centerX = Window::UI_SIZE.x / 2.0f;

  Text::draw(target, "BATAT", {centerX + 2.0f, 34.0f}, 6, TITLE_SHADOW_COLOR,
    Text::Align::Center, false);
  Text::draw(target, "BATAT", {centerX, 30.0f}, 6, TITLE_COLOR, Text::Align::Center, false);

  const float top = 200.0f, step = 11.0f;
  const float columns[] = {centerX - 132.0f, centerX - 4.0f};
  int count = sizeof(CONTROLS) / sizeof(CONTROLS[0]);
  float keyWidth = 0.0f;
  for (int i = 0; i < count; i++) keyWidth = std::max(keyWidth, Text::width(CONTROLS[i][0]));
  keyWidth += 8.0f;
  int rows = (count + 1) / 2;

  sf::RectangleShape panel({280.0f, rows * step + 16.0f});
  panel.setPosition({centerX - 140.0f, top - 8.0f});
  panel.setFillColor(sf::Color(16, 14, 24, 170));
  panel.setOutlineThickness(1.0f);
  panel.setOutlineColor(sf::Color(60, 56, 78));
  target.draw(panel);

  for (int i = 0; i < count; i++) {
    float x = columns[i / rows];
    float y = top + (i % rows) * step;
    Text::draw(target, CONTROLS[i][0], {x, y}, 1, HINT_KEY_COLOR);
    Text::draw(target, CONTROLS[i][1], {x + keyWidth, y}, 1, HINT_COLOR);
  }
}

void Interface::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  switch (screen) {
  case Screen::None:
    return;
  case Screen::Main:
    drawOverlay(target, 140);
    drawMainMenu(target);
    break;
  case Screen::Pause:
    drawOverlay(target, 170);
    break;
  case Screen::Settings:
    drawOverlay(target, settingsReturn == Screen::Main ? 190 : 170);
    break;
  }

  if (const Menu* menu = activeMenu()) target.draw(*menu, states);
}
