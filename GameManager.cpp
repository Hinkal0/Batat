#include "GameManager.h"
#include "Hud.h"
#include "Parser.h"
#include "Text.h"
#include "Resources.h"
#include <SFML/Audio/Listener.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>

static const char* CONFIG_FILE = "config.ini";

static float deltaTime = 0.0f;
static float renderDelta = 0.0f;
static const float FRAMERATE = 120.0f;

const static float MENU_CAMERA_RADIUS = 320.0f;
const static float MENU_CAMERA_PERIOD = 40000.0f;

GameConfig GameManager::loadConfig() {
  GameConfig config;
  config.window.title = "Batat";
  try {
    Parser::read(CONFIG_FILE);
    Parser::extractConfig(config);
  } catch (std::runtime_error& e) {}
  return config;
}

void GameManager::saveConfig() {
  sf::Vector2u resolution = window.getWindowSize();
  config.window.w = resolution.x;
  config.window.h = resolution.y;
  config.window.fullscreen = window.isFullscreen();
  Parser::save(CONFIG_FILE, config);
}

GameManager::GameManager()
  : config(loadConfig()), window(config.window) {
  uint32_t seed = time(nullptr);
  srand(seed);

  Textures::load();
  Fonts::load();
  sf::Listener::setGlobalVolume((float)config.volume);

  menuScene.map = std::make_shared<Map>(sf::Vector2u{100, 70}, 64.0f);
  menuScene.map->generate();
  menuScene.cameraPos = menuScene.map->getSpawnPoint();

  ui = std::make_shared<Interface>(window, config);
  ui->open(Interface::Screen::Main);
  menuScene.spawn(UI_LAYER_MENU, ui);

  gameScene.map = std::make_shared<Map>(sf::Vector2u{100, 70}, 64.0f);

  window.setCursorGrabbed(false);
}

void GameManager::startRun() {
  gameScene.clearObjects();
  gameScene.map->generate();

  player = std::make_shared<Player>(gameScene);
  player->setPosition(gameScene.map->getSpawnPoint());
  player->lockInputUntilRelease();
  gameScene.spawn(LAYER_CHARACTERS, player);

  runStats = {};
  gameScene.spawn(UI_LAYER_HUD, std::make_shared<Hud>(*player, runStats));

  ui->open(Interface::Screen::None);
  gameScene.spawn(UI_LAYER_MENU, ui);

  window.setCursorGrabbed(true);
}

void GameManager::pause() {
  ui->open(Interface::Screen::Pause);
  window.setCursorGrabbed(false);
}

void GameManager::resume() {
  ui->open(Interface::Screen::None);
  window.setCursorGrabbed(true);
  if (player) player->lockInputUntilRelease();
}

void GameManager::endRun() {
  gameScene.clearObjects();
  player.reset();
  menuTime = 0.0f;

  ui->open(Interface::Screen::Main);
  window.setCursorGrabbed(false);
}

void GameManager::handleEvents() {
  while (const std::optional<sf::Event> event = window.pollEvent()) {
    bool playing = inRun() && ui->getScreen() == Interface::Screen::None;

    if (playing) {
      const auto* key = event->getIf<sf::Event::KeyPressed>();
      if ((key && key->code == sf::Keyboard::Key::Escape) || event->is<sf::Event::FocusLost>()) {
        pause();
      }
      continue;
    }

    switch (ui->handleEvent(*event, window.getUIMousePos())) {
    case Interface::Action::StartGame:
      startRun();
      break;
    case Interface::Action::Resume:
      resume();
      break;
    case Interface::Action::QuitToMenu:
      endRun();
      break;
    case Interface::Action::Exit:
      window.close();
      break;
    case Interface::Action::SettingsChanged:
      saveConfig();
      break;
    case Interface::Action::None:
      break;
    }
  }
}

void GameManager::update(float deltaTime) {
  if (!inRun()) {
    menuTime += deltaTime;
    float angle = menuTime / MENU_CAMERA_PERIOD * 2.0f * 3.14159265f;
    menuScene.cameraPos = menuScene.map->getSpawnPoint() +
      sf::Vector2f(std::cos(angle), std::sin(angle)) * MENU_CAMERA_RADIUS;
    menuScene.update(deltaTime);
    return;
  }

  if (ui->getScreen() != Interface::Screen::None) return;

  runStats.time += deltaTime;

  sf::Vector2f mouse = window.getMousePos(gameScene);
  player->aimAt(mouse);
  gameScene.update(deltaTime);

  gameScene.cameraPos = (
    mouse+player->getPosition().componentWiseMul({2,2}))
    .componentWiseDiv({3,3});
}

void GameManager::run() {
  auto start = std::chrono::high_resolution_clock::now();
  while (window.isOpen()) {
    handleEvents();
    if (!window.isOpen()) break;

    auto dt = (std::chrono::high_resolution_clock::now()-start);
    deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(dt).count()/1000.0f;
    renderDelta += deltaTime;
    start = std::chrono::high_resolution_clock::now();

    update(deltaTime);

    if (1000.0f/renderDelta <= FRAMERATE) {
      window.draw(activeScene());
      renderDelta = 0.0f;
    }
  }

  saveConfig();
  Fonts::unload();
  Textures::unload();
}
