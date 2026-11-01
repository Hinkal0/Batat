#pragma once
#include <SFML/Graphics.hpp>
#include "Config.h"
#include "Menu.h"
#include "UIElement.h"
#include "Window.h"

class Interface : public UIElement {
public:
  enum class Screen {
    None,
    Main,
    Pause,
    Settings
  };

  enum class Action {
    None,
    StartGame,
    Resume,
    QuitToMenu,
    Exit,
    SettingsChanged
  };

private:
  Window& window;
  GameConfig& config;

  Screen screen = Screen::Main;
  Screen settingsReturn = Screen::Main;
  Action pending = Action::None;

  Menu mainMenu;
  Menu pauseMenu;
  Menu settingsMenu;

  void changeVolume(int direction);
  void changeWindowSize(int direction);
  std::optional<sf::Vector2u> nextWindowSize(int direction) const;

  const Menu* activeMenu() const;
  Menu* activeMenu();

  void drawOverlay(sf::RenderTarget& target, uint8_t alpha) const;
  void drawMainMenu(sf::RenderTarget& target) const;
  void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

public:
  Interface(Window& window, GameConfig& config);
  Interface(const Interface&) = delete;
  Interface operator=(const Interface&) = delete;

  Screen getScreen() const { return screen; }
  void open(Screen next);

  Action handleEvent(const sf::Event& event, sf::Vector2f uiMouse);
};
