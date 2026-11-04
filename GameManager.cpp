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

static GameConfig loadConfig() {
  GameConfig config;
  config.window.title = "Batat";
  try {
    Parser::read(CONFIG_FILE);
    Parser::extractConfig(config);
  } catch (std::runtime_error& e) {}
  return config;
}

static void saveConfig(GameConfig& config, const Window& window) {
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

  window.onKeyPressed.addListener([this](auto key) {
    if (inRun() && ui->getScreen() == Interface::Screen::None && key == sf::Keyboard::Key::Escape)
      pause();
    }
  );
  window.onFocusLost.addListener([this]() {
    if (inRun() && ui->getScreen() == Interface::Screen::None)
      pause();
    }
  );

  ui->startGame.addListener([this]() { startRun(); });
  ui->resume.addListener([this]() { resume(); });
  ui->quitToMenu.addListener([this]() { endRun(); });
  ui->exit.addListener([this]() { window.close(); });
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
    window.pollEvents();

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

  saveConfig(config, window);
  Fonts::unload();
  Textures::unload();
}
