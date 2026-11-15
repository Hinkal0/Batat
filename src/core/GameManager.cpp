#include "GameManager.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <SFML/Audio/Listener.hpp>
#include "ui/Hud.h"
#include "config/Parser.h"
#include "Resources.h"
#include "InputSystem.h"
#include "entities/enemies/OnionGuard.h"
#include "entities/enemies/PeaShooter.h"
#include "entities/enemies/RadishRunner.h"
#include "entities/enemies/Yabloko.h"

static const char* CONFIG_FILE = "config/config.ini";

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

  InputSystem::init(window);

  if (!Textures::load() || !Fonts::load())
    throw std::runtime_error("Could not load game resources");

  sf::Listener::setGlobalVolume((float)config.volume);

  menuScene.map = std::make_shared<Map>(sf::Vector2u{100, 70}, 64.0f);
  menuScene.map->generate();
  menuScene.cameraPos = menuScene.map->getSpawnPoint();

  ui = std::make_shared<Interface>(window, config);
  ui->open(Interface::Screen::Main);
  menuScene.spawn(UI_LAYER_MENU, ui);

  window.setCursorGrabbed(false);

  InputSystem::onKeyPressed.addListener([this](auto key) {
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
  runStats = {};
  player = std::make_shared<Player>(gameScene);
  loadFloor();
  ui->open(Interface::Screen::None);
  window.setCursorGrabbed(true);
}

void GameManager::loadFloor() {
  gameScene.clearObjects();
  roomEnemies.clear();
  activeRoom.reset();
  runStats.enemiesRemaining = 0;
  runStats.isExitOpen = false;

  unsigned floorOffset = static_cast<unsigned>(runStats.floor - 1);
  sf::Vector2u size{50 + 2 * floorOffset, 40 + floorOffset};
  gameScene.map = std::make_shared<Map>(size, 64.0f);
  gameScene.map->generate(runStats.floor < RunStats::FLOOR_COUNT);
  gameScene.cameraPos = gameScene.map->getSpawnPoint();

  player->enterFloor(gameScene.cameraPos);
  gameScene.spawn(LAYER_CHARACTERS, player);
  spawnEnemies();

  gameScene.spawn(UI_LAYER_HUD, std::make_shared<Hud>(*player, runStats));
  gameScene.spawn(UI_LAYER_MENU, ui);
}

void GameManager::finishRun() {
  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(runStats.time);
  if (!config.bestTime || time < *config.bestTime) {
    config.bestTime = time;
    saveConfig(config, window);
  }
  endRun();
}

void GameManager::spawnEnemies() {
  const auto& rooms = gameScene.map->getRooms();
  const float tile = gameScene.map->getTileSize();
  const sf::Vector2f inset{2.0f, 2.0f};
  roomEnemies.resize(rooms.size());

  for (std::size_t roomIndex = 1; roomIndex < rooms.size(); ++roomIndex) {
    const auto& room = rooms[roomIndex];
    auto& enemies = roomEnemies[roomIndex];
    const sf::Vector2f low = sf::Vector2f(room.position) + inset;
    const sf::Vector2f high = sf::Vector2f(room.position + room.size) - inset;
    const std::array<sf::Vector2f, 5> positions{{
      low, high, {high.x, low.y}, {low.x, high.y}, {(low.x + high.x) / 2.0f, low.y}
    }};

    const int count = std::clamp(room.size.x * room.size.y / 60, 3, 5);
    for (int i = 0; i < count; ++i) {
      std::shared_ptr<Enemy> enemy;
      switch ((roomIndex + runStats.floor - 1 + i) % 4) {
        case 0: enemy = std::make_shared<RadishRunner>(gameScene, player); break;
        case 1: enemy = std::make_shared<PeaShooter>(gameScene, player); break;
        case 2: enemy = std::make_shared<OnionGuard>(gameScene, player); break;
        case 3: enemy = std::make_shared<Yabloko>(gameScene, player); break;
      }
      enemy->setPosition(positions[i] * tile);
      enemy->active = false;
      gameScene.spawn(LAYER_CHARACTERS, enemy);
      enemies.push_back(enemy);
    }
  }
}

void GameManager::beginEncounter(std::size_t roomIndex) {
  activeRoom = roomIndex;
  gameScene.map->lockRoom(roomIndex);
  auto& enemies = roomEnemies[roomIndex];
  for (const auto& enemy : enemies) {
    enemy->active = true;
  }
  runStats.enemiesRemaining = enemies.size();
}

void GameManager::updateEncounter() {
  if (!activeRoom) {
    auto room = gameScene.map->getRoomContaining(player->getHitbox());
    if (room && !roomEnemies[*room].empty()) beginEncounter(*room);
    return;
  }

  auto& enemies = roomEnemies[*activeRoom];
  std::erase_if(enemies, [](const auto& enemy) { return enemy->destroyed; });
  runStats.enemiesRemaining = enemies.size();
  if (!enemies.empty()) return;

  const auto clearedRoom = *activeRoom;
  activeRoom.reset();
  gameScene.map->unlockRoom();

  if (clearedRoom == roomEnemies.size() - 1) {
    if (runStats.floor == RunStats::FLOOR_COUNT) finishRun();
    else {
      gameScene.map->unlockExit();
      runStats.isExitOpen = true;
    }
  }
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
  activeRoom.reset();
  roomEnemies.clear();
  menuTime = 0.0f;

  ui->open(Interface::Screen::Main);
  window.setCursorGrabbed(false);
}

void GameManager::update(std::chrono::steady_clock::duration elapsed) {
  const float deltaTime = std::chrono::duration<float, std::milli>(elapsed).count();
  if (!inRun()) {
    menuTime += deltaTime;
    float angle = menuTime / MENU_CAMERA_PERIOD * 2.0f * 3.14159265f;
    menuScene.cameraPos = menuScene.map->getSpawnPoint() +
      sf::Vector2f(std::cos(angle), std::sin(angle)) * MENU_CAMERA_RADIUS;
    menuScene.update(deltaTime);
    return;
  }

  if (ui->getScreen() != Interface::Screen::None) return;

  runStats.time += elapsed;

  sf::Vector2f mouse = InputSystem::getMousePos(gameScene);
  player->aimAt(mouse);
  gameScene.update(deltaTime);
  if (player->getHealth() == 0) {
    endRun();
    return;
  }

  updateEncounter();
  if (!inRun()) return;

  if (gameScene.map->canUseExit(player->getPosition())) {
    ++runStats.floor;
    loadFloor();
    return;
  }

  gameScene.cameraPos = (
    mouse+player->getPosition().componentWiseMul({2,2}))
    .componentWiseDiv({3,3});
}

void GameManager::run() {
  auto start = std::chrono::steady_clock::now();
  while (window.isOpen()) {
    window.pollEvents();
    if (!window.isOpen()) break;

    auto now = std::chrono::steady_clock::now();
    auto elapsed = now - start;
    start = now;
    renderDelta += std::chrono::duration<float, std::milli>(elapsed).count();

    update(elapsed);

    if (1000.0f/renderDelta <= FRAMERATE) {
      window.draw(activeScene());
      renderDelta = 0.0f;
    }
  }

  saveConfig(config, window);
  Fonts::unload();
  Textures::unload();
}
